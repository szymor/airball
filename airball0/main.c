
// Airball - 25th Anniversary Revival.
// Code: 17o2!!
// Contact: Clement CORDE, c1702@yahoo.com

#include "includes.h"

//#define DEBUG_KEYS	1	// A commenter.

// Variables générales.
struct SGene gVar;

//=====================================

// Gestionnaire d'évènements.
int EventHandler(u32 nInGame)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			gVar.pKeys[event.key.keysym.sym] = 1;

			// Toggle fullscreen/windowed.
			if (gVar.pKeys[SDLK_F10])
			{
				gRender.nFullscreenMode ^= 1;
				Render_SetVideoMode();
			}
			// Toggle render mode.
			if (gVar.pKeys[SDLK_F9])
			{
				if (++gRender.nRenderMode >= e_RenderMode_MAX) gRender.nRenderMode = 0;
				Render_SetVideoMode();
			}

			// Esc : Quitter.
			if (gVar.pKeys[SDLK_ESCAPE]) return (1);			// Emergency exit.

			break;

		case SDL_KEYUP:
			gVar.pKeys[event.key.keysym.sym] = 0;
			break;

		}
	}


	// Gestion du joystick.
	if (gVar.pJoystick != NULL)
	{
//bug?		static u16	pHatMsk[e_CfgKey_MAX] = { SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT, 0x100, 0x200 };//, 0x400 };
//bug?		static u8	pKeyVal[e_CfgKey_MAX] = { e_CfgKey_Up, e_CfgKey_Down, e_CfgKey_Left, e_CfgKey_Right, e_CfgKey_ButtonA, e_CfgKey_ButtonB };//, e_CfgKey_ButtonC };
		static u16	pHatMsk[e_CfgKey_LAST] = { SDL_HAT_UP, SDL_HAT_DOWN, SDL_HAT_LEFT, SDL_HAT_RIGHT, 0x100, 0x200 };//, 0x400 };
		static u8	pKeyVal[e_CfgKey_LAST] = { e_CfgKey_Up, e_CfgKey_Down, e_CfgKey_Left, e_CfgKey_Right, e_CfgKey_ButtonA, e_CfgKey_ButtonB };//, e_CfgKey_ButtonC };

		u32	i;
		s16	nAxis;
		u16	nHat, nVal;
		nHat = 0;
		// Croix.
		if (gVar.nJoystickNoHat == 0)
			nHat = SDL_JoystickGetHat(gVar.pJoystick, 0);
		// Stick.
		if (gVar.nJoystickNoAxes == 0)
		{
			nAxis = SDL_JoystickGetAxis(gVar.pJoystick, 0);	// X
			if (ABS(nAxis) > 3200) nHat |= (nAxis > 0 ? SDL_HAT_RIGHT : SDL_HAT_LEFT);
			nAxis = SDL_JoystickGetAxis(gVar.pJoystick, 1);	// Y
			if (ABS(nAxis) > 3200) nHat |= (nAxis > 0 ? SDL_HAT_DOWN : SDL_HAT_UP);
		}
		// Boutons.
		if (SDL_JoystickGetButton(gVar.pJoystick, gAirballCfg.pKeys[e_CfgKey_JoyButtonA])) nHat |= 0x100;
		if (SDL_JoystickGetButton(gVar.pJoystick, gAirballCfg.pKeys[e_CfgKey_JoyButtonB])) nHat |= 0x200;
//		if (SDL_JoystickGetButton(gVar.pJoystick, gAirballCfg.pKeys[e_CfgKey_JoyButtonC])) nHat |= 0x400;

		nVal = gVar.nJoystickState ^ nHat;
		gVar.nJoystickState = nHat;		// Sauvegarde pour le prochain tour.

//bug?		for (i = 0; i < e_CfgKey_MAX; i++)
		for (i = 0; i < e_CfgKey_LAST; i++)
		if (nVal & pHatMsk[i])
		{
			gVar.pKeys[gAirballCfg.pKeys[pKeyVal[i]]] = (nHat & pHatMsk[i] ? 1 : 0);
			gVar.pKeysSDL[gAirballCfg.pKeys[pKeyVal[i]]] = (nHat & pHatMsk[i] ? 1 : 0);
		}

		// Pas en cours de jeu ?
//		if (nInGame == 0 && nVal) MenuTimer_Reset();		// RAZ du timer de time out pour les menus.

	}

	return (0);
}

//=====================================

/*
void Menu_ScrollInit(void);
void Menu_ScrollRelease(void);
void Menu_ScrollManage(void);
*/

// Le Menu (générique).
u32 Menu(void (*pFctInit)(void), u32 (*pFctMain)(void))
{
	u32	nMenuVal = MENU_Null;

	(*pFctInit)();

	// Main loop.
	FrameInit();
	while (nMenuVal == MENU_Null)
	{
		// Gestion des évenements.
		if (EventHandler(0) != 0) { nMenuVal = MENU_Quit; break; }
		// Menu Main.
		nMenuVal = (*pFctMain)();

#ifdef DEBUG_KEYS
if (gVar.pKeys[SDLK_y]) { FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); }
#endif

		RenderFlip(1);		// Wait for frame, Flip.
	}

	return (nMenuVal);
}


// La boucle de jeu.
u32 Game(void)
{
	Game_Init();

	// Main loop.
	FrameInit();
	while (gGame.nExitCode == 0)
	{
		// Gestion des évenements.
//		if (EventHandler(1)) return (MENU_Main);
		if (EventHandler(1)) { FadeColors(0, 256); RenderFlip(1); return (MENU_Main); }		// Pour éviter le flash de palette sur la sortie avec Esc.

		Game_Manage();

#ifdef DEBUG_KEYS
if (gVar.pKeys[SDLK_y]) { FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); }
#endif

		RenderFlip(1);		// Wait for frame, Flip.
	}

	// High-score ?
	if (gGame.nExitCode == -2 || gGame.nExitCode == -3)		// -2 == mort / -3 == win.
	if (gGame.nQuest != 0)	// Si le joueur n'a pas ramené le Spell Book, pas de score.
	{
		s32	nRank;
		if ((nRank = Scr_CheckHighSc(gHero.nObjTime, gHero.nObjFound)) >= 0)
		{
			return (Menu(MenuHighScores_Init2, MenuHighScores_Main));
		}
	}

	return (MENU_Main);
}

//=====================================

//#define	EXE_CHECKSUM	1		// Commenter la ligne pour virer le test.

#ifdef EXE_CHECKSUM
// Calcul d'un Checksum.
u32 ChecksumCalc(u8 *pBuf, u32 nSz)
{
	u32	i;
	u32	nSum = 0;

	for (i = 0; i < nSz / 4; i++)
	{
		nSum ^= *(u32 *)pBuf;
		pBuf += sizeof(u32);
	}
	for (i = 0; i < (nSz & 3); i++)
		nSum += *pBuf++;

	return (nSum);
}

// Recherche d'une chaîne dans un buffer binaire.
u8 * StrFind(u8 *pToFind, u32 nToFindSz, u8 *pToSearch, u32 nToSearchSz)
{
	if (nToFindSz > nToSearchSz) return (NULL);

	u32	i;
	for (i = 0; i <= nToSearchSz - nToFindSz; i++)
	{
		// On ne compare que le premier char pour commencer.
		if (*pToSearch == *pToFind)
			if (strncmp((char *)pToSearch, (char *)pToFind, nToFindSz) == 0) return (pToSearch);
		pToSearch++;
	}
	return (NULL);
}
// Teste le checksum de l'exe.
void ExeChecksumTst(char *pFilename)
{
	FILE	*fPt = NULL;
	u8	*pBuf = NULL;

	// Ouverture du fichier.
	if ((fPt = fopen(pFilename, "rb")) == NULL)
	{
		fprintf(stderr, "Error opening file '%s'.\n", pFilename);
		goto _err_exit1;
	}
	// Récupération de la taille du fichier.
	s32	nFileSzToLoad, nSz;
	fseek(fPt, 0L, SEEK_END);
	nFileSzToLoad = ftell(fPt);
	fseek(fPt, 0L, SEEK_SET);
//printf("fsize = %d\n", nFileSzToLoad);
	// Malloc du buffer de lecture.
	if ((pBuf = (u8 *)malloc(nFileSzToLoad)) == NULL)
	{
		fprintf(stderr, "malloc failed (pBuf).\n");
		goto _err_exit1;
	}
	// Lecture.
	nSz = fread(pBuf, 1, nFileSzToLoad, fPt);
	if (nSz != nFileSzToLoad)
	{
		fprintf(stderr, "fread error.\n");
		goto _err_exit1;
	}
	fclose(fPt); fPt = NULL;

	// Recherche de la chaîne qui va bien.
	static char	*pStrToFind = "CC\x17\x02****";		// !!! Attention à bien laisser les 4 étoiles pour poke externe du checksum !!!
	u8	*pPtr;

	pPtr = StrFind((u8 *)pStrToFind, 4, pBuf, nFileSzToLoad);
	if (pPtr == NULL)
	{
		fprintf(stderr, "File '%s' has been altered. Aborted.\n", pFilename);
		goto _err_exit1;
	}
//printf("Substring found at offset %X\n", (int)(pPtr - pBuf));
	// Checksum du fichier.
	u32	nSum0 = *(u32 *)(pPtr + 4);
	// On efface les 4 cases du checksum.
	memset(pPtr + 4, 0, 4);
	// Recalcul du Checksum.
	u32	nSum1 = ChecksumCalc(pBuf, nFileSzToLoad);
//printf("Exe checksum = %X\n", nSum1);
	free(pBuf); pBuf = NULL;

	// Ok ?
	if (nSum0 != nSum1)
	{
		fprintf(stderr, "File '%s' has been altered. Aborted.\n", pFilename);
		exit(1);
	}
	return;

_err_exit1:
	if (fPt != NULL) fclose(fPt);
	if (pBuf != NULL) free(pBuf);
	exit (1);

}
#endif

//=====================================

// Point d'entrée.
int main(int argc, char *argv[])
{
	u32	nLoop;
	u32	nMenuVal;

#ifdef EXE_CHECKSUM
	// Test d'intégrité sur l'exe.
	ExeChecksumTst(argv[0]);
#endif

	srand(time(NULL));	// Init hasard.

	// SDL Init.
//	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	// atexit : Quand on quittera (exit, return...), SDL_Quit() sera appelée.
	atexit(SDL_Quit);

	// Video mode init.
	Render_InitVideo();
	SDL_WM_SetCaption("Airball by 17o2!!", NULL);	// Nom de la fenêtre.

	// Lecture du fichier de conf.
	CfgLoad();
	// Passe dans le mode video de la conf.
	if (gRender.nRenderMode != gAirballCfg.nVideoMode)
	{
		gRender.nRenderMode = gAirballCfg.nVideoMode;
		Render_SetVideoMode();
	}

//	gRender.nRenderMode = e_RenderMode_Scale2x;
//	Render_SetVideoMode();


	// Init joystick.
	gVar.pJoystick = NULL;
	gVar.nJoystickState = 0;
	if (SDL_NumJoysticks() > 0)
	{
		SDL_JoystickEventState(SDL_ENABLE);
		gVar.pJoystick = SDL_JoystickOpen(0);		// Si pas NULL, joystick !
		if (gVar.pJoystick != NULL)
		{
			u32	nJoystickNumButtons = SDL_JoystickNumButtons(gVar.pJoystick);
			gVar.nJoystickNoHat = (SDL_JoystickNumHats(gVar.pJoystick) < 1 ? 1 : 0);
			gVar.nJoystickNoAxes = (SDL_JoystickNumAxes(gVar.pJoystick) < 2 ? 1 : 0);

			// Config boutons ok ?
			if (gAirballCfg.pKeys[e_CfgKey_JoyButtonA] >= nJoystickNumButtons ||
				gAirballCfg.pKeys[e_CfgKey_JoyButtonB] >= nJoystickNumButtons)// ||
				//gAirballCfg.pKeys[e_CfgKey_JoyButtonC] >= nJoystickNumButtons)
			{
				fprintf(stderr, "Joystick configuration can't be use with this joystick.\nPlease run the configuration program.\n");
				goto _JoyOff;
			}
			// Hardware ok ?
//			if (nJoystickNumButtons < 3 || (gVar.nJoystickNoHat && gVar.nJoystickNoAxes))
			if (nJoystickNumButtons < 2 || (gVar.nJoystickNoHat && gVar.nJoystickNoAxes))
			{
				fprintf(stderr, "Unable to handle joystick.\n");
_JoyOff:
				fprintf(stderr, "Joystick disabled.\n");
				// Pas assez de boutons ? => Disable.
				SDL_JoystickClose(gVar.pJoystick);
				gVar.pJoystick = NULL;
			}
		}
	}


	// Allocation d'une surface pour le décor.
	gRoom.pBkg = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 8, gRender.pScreenBuf2->format->Rmask, gVar.pScreen->format->Gmask, gVar.pScreen->format->Bmask, 0);
	if (gRoom.pBkg == NULL)
	{
		fprintf(stderr, "gRoom.pBkg: Unable to allocate SDL surface: %s\n", SDL_GetError());
		exit(1);
	}
	// Allocation d'une surface pour les textes.
	gGame.pTxt = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 8, gRender.pScreenBuf2->format->Rmask, gVar.pScreen->format->Gmask, gVar.pScreen->format->Bmask, 0);
	if (gGame.pTxt == NULL)
	{
		fprintf(stderr, "gGame.pTxt: Unable to allocate SDL surface: %s\n", SDL_GetError());
		exit(1);
	}

	// Init du clavier.
	gVar.pKeysSDL = SDL_GetKeyState(NULL);
	memset(gVar.pKeys, 0, SDLK_LAST);



	gVar.pBlk = PSDLoadToSDLSurf("data/blocks.psd");
	SDL_SetColorKey(gVar.pBlk, SDL_SRCCOLORKEY, 255);
	gVar.pTreasures = PSDLoadToSDLSurf("data/treasures.psd");
	gVar.pWalls = PSDLoadToSDLSurf("data/room_walls2.psd");
	SDL_SetColorKey(gVar.pWalls, SDL_SRCCOLORKEY, 255);
	gVar.pBall = PSDLoadToSDLSurf("data/ball.psd");
	SDL_SetColorKey(gVar.pBall, SDL_SRCCOLORKEY, 255);

	gVar.pTitle = PSDLoadToSDLSurf("data/titlescr.psd");	// Titre. Couleurs 32 > 47
	SDL_SetColorKey(gVar.pTitle, SDL_SRCCOLORKEY, 255);
	gVar.pMarble = PSDLoadToSDLSurf("data/marble.psd");		// Fond marbre. Couleurs 0 > 31
	gVar.pFont16 = PSDLoadToSDLSurf("data/font16.psd");		// Fonte 16x16.

	// Lecture des salles.
	Room_DataLoad("data/rooms.bin");
	Game_LoadSaveInit();			// *APRES* Room_DataLoad().
	gGame.nLastDeathRoom = 0xFFFF;	// 0xFFFF => Pas encore valide. (Crucifix).

	Scr_Load();

//	Menu_ScrollInit();		// !! Après la lecture de la fonte !!

	// Creation des palettes pour les textes.
	static	u8	pTxtPals[TXTPAL_NB * 6] = {
		224,   0, 224,   248, 224, 248,		// 0 violet
		224,   0,   0,   248, 224, 224,		// 1 rouge
//		  0,   0, 224,   224, 224, 248,		// 2 bleu
		  0,  64, 224,   224, 232, 248,		// 2 bleu
		224, 224,   0,   248, 248, 224,		// 3 jaune
	};
	u32	i;
	for (i = 0; i < TXTPAL_NB; i++)
		Font_CreateTxtColors(pTxtPals[i * 6], pTxtPals[(i * 6) + 1], pTxtPals[(i * 6) + 2],
				pTxtPals[(i * 6) + 3], pTxtPals[(i * 6) + 4], pTxtPals[(i * 6) + 5], i);
	// On affecte ces couleurs à toutes les surfaces.
	// -18 : Bug incompréhensible de palette en full screen 8BPP dans les couleurs supérieures.
	SDL_SetPalette(gRoom.pBkg, SDL_LOGPAL, &gVar.pColorsLog[256-18 - (TXTPAL_NB * 8)], 256-18 - (TXTPAL_NB * 8), (TXTPAL_NB * 8)+0);
	SDL_SetPalette(gGame.pTxt, SDL_LOGPAL, &gVar.pColorsLog[256-18 - (TXTPAL_NB * 8)], 256-18 - (TXTPAL_NB * 8), (TXTPAL_NB * 8)+0);
	SDL_SetPalette(gVar.pBlk, SDL_LOGPAL, &gVar.pColorsLog[256-18 - (TXTPAL_NB * 8)], 256-18 - (TXTPAL_NB * 8), (TXTPAL_NB * 8)+0);
	SDL_SetPalette(gVar.pTreasures, SDL_LOGPAL, &gVar.pColorsLog[256-18 - (TXTPAL_NB * 8)], 256-18 - (TXTPAL_NB * 8), (TXTPAL_NB * 8)+0);
	SDL_SetPalette(gVar.pWalls, SDL_LOGPAL, &gVar.pColorsLog[256-18 - (TXTPAL_NB * 8)], 256-18 - (TXTPAL_NB * 8), (TXTPAL_NB * 8)+0);
	SDL_SetPalette(gVar.pBall, SDL_LOGPAL, &gVar.pColorsLog[256-18 - (TXTPAL_NB * 8)], 256-18 - (TXTPAL_NB * 8), (TXTPAL_NB * 8)+0);
	SDL_SetPalette(gVar.pTitle, SDL_LOGPAL, &gVar.pColorsLog[256-18 - (TXTPAL_NB * 8)], 256-18 - (TXTPAL_NB * 8), (TXTPAL_NB * 8)+0);
	SDL_SetPalette(gVar.pTitle, SDL_LOGPAL, gVar.pMarble->format->palette->colors, 0, 32);	// On place la palette du marbre dans celle du titre.
	SDL_SetPalette(gVar.pMarble, SDL_LOGPAL, &gVar.pColorsLog[256-18 - (TXTPAL_NB * 8)], 256-18 - (TXTPAL_NB * 8), (TXTPAL_NB * 8)+0);
	SDL_SetPalette(gVar.pFont16, SDL_LOGPAL, &gVar.pColorsLog[256-18 - (TXTPAL_NB * 8)], 256-18 - (TXTPAL_NB * 8), (TXTPAL_NB * 8)+0);



	// Init sound.
	Sfx_SoundInit();
	Sfx_LoadWavFiles();
	Sfx_LoadYMFiles();
	Sfx_SoundOn();		// Starts playback.


	//
//	MenuInit();

	SDL_ShowCursor(SDL_DISABLE);	// Cache le pointeur de la souris.

	// Boucle infinie.
	nMenuVal = MENU_Main;
//nMenuVal = MENU_Game;
//nMenuVal = MENU_HighScores;
	nLoop = 1;
	while (nLoop)
	{
		switch (nMenuVal)
		{
		case MENU_Main:		// Main menu.
			nMenuVal = Menu(MenuMain_Init, MenuMain_Main);
			break;

		case MENU_Credits:	// Crédits.
			nMenuVal = Menu(MenuCredits_Init, MenuCredits_Main);
			break;

		case MENU_HighScores:	// High-scores.
			nMenuVal = Menu(MenuHighScores_Init, MenuHighScores_Main);
			break;

		case MENU_Game:		// Jeu.
//			Game();
//			nMenuVal = MENU_Main;
			nMenuVal = Game();
			break;

		case MENU_Quit:		// Sortie.
			nLoop = 0;
			break;
		}
	}

	SDL_ShowCursor(SDL_ENABLE);		// Réautorise l'affichage du curseur de la souris.

	if (gVar.pJoystick != NULL) SDL_JoystickClose(gVar.pJoystick);


	// Sfx off.
	Sfx_SoundOff();			// Stops playback.
	Sfx_FreeWavFiles();		// Libère les ressources des fx.
	Sfx_FreeYMFiles();		// Libère les ressources des YM.
	//


	SDL_FreeSurface(gVar.pBlk);
	SDL_FreeSurface(gVar.pTreasures);
	SDL_FreeSurface(gVar.pWalls);
	SDL_FreeSurface(gVar.pBall);

	SDL_FreeSurface(gVar.pTitle);
	SDL_FreeSurface(gVar.pFont16);
	SDL_FreeSurface(gVar.pMarble);

	SDL_FreeSurface(gRoom.pBkg);
	SDL_FreeSurface(gGame.pTxt);

//	Menu_ScrollRelease();

	Room_DataFree();		// Libère les pièces.
	Game_LoadSaveRelease();	// Libère la mémoire du slot de lecture/sauvegarde.

	// Libère les ressources de rendu.
	RenderRelease();

	return (0);
}



