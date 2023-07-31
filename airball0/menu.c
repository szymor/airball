
#include "includes.h"

#define	MENU_FADE_STEP	32

#define	MENU_TIMEOUT	(FPS_1S * 3)

//=============================================================================

struct SMenuGen
{
	u16	nPhase;
	u16	nFadeVal;
	u32	nRetVal;

	u16	nTimer;

};
struct SMenuGen	gMenuGen;

enum
{
	e_MenuGen_FadeIn = 0,
	e_MenuGen_FadeOut,
	e_MenuGen_Last,
};

// Phases fade in/out.
u32 MenuGen_Phases(u32 nPhaseMain)
{
	u32	nRetVal = MENU_Null;

	switch (gMenuGen.nPhase)
	{
	case e_MenuGen_FadeIn:				// Fade in.
		if (gMenuGen.nFadeVal < 256)
			gMenuGen.nFadeVal += MENU_FADE_STEP;		// Fade on.
		else
			gMenuGen.nPhase = nPhaseMain;
		FadeColors(gMenuGen.nFadeVal, 256);
		break;

	case e_MenuGen_FadeOut:				// Fade out.
		if (gMenuGen.nFadeVal)
			gMenuGen.nFadeVal -= MENU_FADE_STEP;		// Fade off.
		else
			nRetVal = gMenuGen.nRetVal;
		FadeColors(gMenuGen.nFadeVal, 256);
		break;
	}

	return (nRetVal);
}

//=============================================================================

struct SMenuMain
{
	u8	nBlink;		// Pour clignotement du texte.

};
struct SMenuMain	gMenuMain;

enum
{
	e_MenuMain_Manage = e_MenuGen_Last,
};

// Menu main : Init.
void MenuMain_Init(void)
{
	SetColors_Menu();
	FadeColors(256, 256);	// fade val / nb colors

	gMenuGen.nPhase = e_MenuGen_FadeIn;
	gMenuGen.nFadeVal = 0;
	gMenuGen.nRetVal = MENU_Main;	// Choix par défaut.
	gMenuGen.nTimer = 0;

	gMenuMain.nBlink = 0;

	// Music.
	Music_Start(e_YmMusic_Menu, 0);

}

// Menu main : Main.
u32 MenuMain_Main(void)
{
	u32	nRetVal = MENU_Null;

	gMenuGen.nTimer++;

	// Gestion.
	if (gMenuGen.nPhase < e_MenuGen_Last)
		nRetVal = MenuGen_Phases(e_MenuMain_Manage);
	else
	switch (gMenuGen.nPhase)
	{
	case e_MenuMain_Manage:				// Gestion normale.
		// Time out ?
		if (gMenuGen.nTimer > MENU_TIMEOUT)
		{
			static	u8	nCur = 0;
			gMenuGen.nRetVal = (++nCur & 1 ? MENU_Credits : MENU_HighScores);
			gMenuGen.nPhase = e_MenuGen_FadeOut;
		}

		// Appui sur bouton A ou bouton B ?
		if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] ||
			gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]])
		{
			gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] = gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]] = 0;
			gMenuGen.nRetVal = MENU_Game;			// Choix à la fin du fade out.
			gMenuGen.nPhase = e_MenuGen_FadeOut;
		}
		break;
	}

	// Affichage.

	// Copie du bkg.
	SDL_BlitSurface(gVar.pMarble, NULL, gVar.pScreen, NULL);
	SDL_BlitSurface(gVar.pTitle, NULL, gVar.pScreen, NULL);

	// Press fire...
	char	*pPress = "PRESS FIRE TO PLAY";
	if (++gMenuMain.nBlink & (32 | 16))
		FontPrint16(gVar.pScreen, gVar.pFont16, (SCR_Width / 2) - (18 * 8), SCR_Height - 32, pPress, 0);

	return (nRetVal);
}

//=============================================================================

char *pCreditsOrg[] =
{
"1ORIGINAL CREDITS",
"1",
"2PROGRAM: EDWARD SCIO",
"2GRAPHICS: PETE LYON",
"2MUSIC: PAUL SHIELDS",
"2ROOMS: PETER SCOTT",
"3",
"3(C) 1987 MICRODEAL",
"3(CR) 1987 TSUNOO"
};

char *pCreditsNew[] =
{
/*
"125TH ANNIVERSARY",
"1EDITION CREDITS",
"1",
"2ALL CODE AND",
"2EVERYTHING ELSE:",
"3",
"3CLEMENT CORDE",
*/

"125TH ANNIVERSARY",
"1EDITION CREDITS",
"1",
"2USING ST-SOUND BY",
"3LEONARD - OXG",
"1",
"2EVERYTHING ELSE BY",
"3CLEMENT CORDE",
};

// Menu credits : Init.
void MenuCredits_Init(void)
{
	SetColors_Txt();
	FadeColors(256, 256);	// fade val / nb colors

	gMenuGen.nPhase = e_MenuGen_FadeIn;
	gMenuGen.nFadeVal = 0;
	gMenuGen.nRetVal = MENU_Main;	// Choix par défaut.
	gMenuGen.nTimer = 0;

	// Crédits originaux ou new.
	char	**pCredits;
	u32	nNbElem;
	static	u8	nCurCred = 0;
	if (++nCurCred & 1)
	{
		pCredits = pCreditsOrg;
		nNbElem = NBELEM(pCreditsOrg);
	}
	else
	{
		pCredits = pCreditsNew;
		nNbElem = NBELEM(pCreditsNew);
	}

	// Préparation du fond.
//	SDL_FillRect(gGame.pTxt, NULL, 0);
	SDL_BlitSurface(gVar.pMarble, NULL, gGame.pTxt, NULL);
	u32	i;
	u32	nY;// = 24;
	s32	nX;
	nY = ((SCR_Height / 2) - 16) - (nNbElem * 9);
//	for (i = 0; i < NBELEM(pCredits); i++)
	for (i = 0; i < nNbElem; i++)
	{
		nX = (SCR_Width / 2) - (strlen(pCredits[i] + 1) * 8);
		FontPrint16(gGame.pTxt, gVar.pFont16, nX, nY, pCredits[i] + 1, *(pCredits[i]) - '0');
		nY += 18;
	}

	gMenuMain.nBlink = 0;

	// Music.
	Music_Start(e_YmMusic_Menu, 0);

}

// Menu credits : Main.
u32 MenuCredits_Main(void)
{
	u32	nRetVal = MENU_Null;

	gMenuGen.nTimer++;

	// Gestion.
	if (gMenuGen.nPhase < e_MenuGen_Last)
		nRetVal = MenuGen_Phases(e_MenuMain_Manage);
	else
	switch (gMenuGen.nPhase)
	{
	case e_MenuMain_Manage:				// Gestion normale.
		// Time out ?
		if (gMenuGen.nTimer > MENU_TIMEOUT)
		{
			gMenuGen.nRetVal = MENU_Main;
			gMenuGen.nPhase = e_MenuGen_FadeOut;
		}

		// Appui sur bouton A ou bouton B ?
		if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] ||
			gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]])
		{
			gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] = gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]] = 0;
			gMenuGen.nRetVal = MENU_Game;			// Choix à la fin du fade out.
			gMenuGen.nPhase = e_MenuGen_FadeOut;
		}
		break;
	}

	// Affichage.

	// Copie du bkg.
	SDL_BlitSurface(gGame.pTxt, NULL, gVar.pScreen, NULL);

	// Press fire...
	char	*pPress = "PRESS FIRE TO PLAY";
	if (++gMenuMain.nBlink & (32 | 16))
		FontPrint16(gVar.pScreen, gVar.pFont16, (SCR_Width / 2) - (18 * 8), SCR_Height - 32, pPress, 0);

	return (nRetVal);
}

//=============================================================================

#define HISC_Nb	(4)
#define HISC_NameLg (16+1)
#define HISC_Filename "high.scr"

#define	HISC_DefaultName	"LUPIN III"

#pragma pack(1)
struct SScore
{
	char	pName[HISC_NameLg];
	u32	nTime;
	u8	nObjects;
};
#pragma pack()
struct SScore	gpHighScores[HISC_Nb];

struct SMenuHS
{
	u8	nEntry;		// 0, pas de saisie du nom / 1, saisie d'un high score.
	s32	nRank;		// Hi-score en cours.
	u8	nPos;		// Position dans le texte.

};
struct SMenuHS	gMenuHS;

// Menu high-scores : Init.
void MenuHighScores_Init(void)
{
	SetColors_Txt();
	FadeColors(256, 256);	// fade val / nb colors

	gMenuGen.nPhase = e_MenuGen_FadeIn;
	gMenuGen.nFadeVal = 0;
	gMenuGen.nRetVal = MENU_Main;	// Choix par défaut.
	gMenuGen.nTimer = 0;

/*
	// Préparation du fond.
//	SDL_FillRect(gGame.pTxt, NULL, 0);
	SDL_BlitSurface(gVar.pMarble, NULL, gGame.pTxt, NULL);

	static	char *pHall = "HALL OF FAME";
	FontPrint16(gGame.pTxt, gVar.pFont16, (SCR_Width / 2) - (strlen(pHall) * 8), 16, pHall, 1);

	u32	i;
	u32	nY = 24+12+8;
	char	pPos[] = "0 -";
	for (i = 0; i < HISC_Nb; i++)
	{
		// L1 : Position + nom.
		pPos[0] = i + '1';
		FontPrint16(gGame.pTxt, gVar.pFont16, 0, nY, pPos, 0);
		FontPrint16(gGame.pTxt, gVar.pFont16, 4 * 16, nY, gpHighScores[i].pName, 2);
		// L2 : Objets + Temps.
		char	pItems[] = "0";
		FontPrint16(gGame.pTxt, gVar.pFont16, 4*16, nY+16, "ITM:", 2);
		if (gpHighScores[i].nObjects & 0x80)
			FontPrint16(gGame.pTxt, gVar.pFont16, 8*16, nY+16, "ALL", 2);
		else
		{
			pItems[0] = '0' + gpHighScores[i].nObjects;
			FontPrint16(gGame.pTxt, gVar.pFont16, 8*16, nY+16, pItems, 2);
		}
		//
		char	pTimeOrg[] = " 0 00 00";
		char	pTime[] = " 0 00 00";
		strcpy(pTime, pTimeOrg);
		Game_TimeToStr(pTime, gpHighScores[i].nTime);
		pTime[5] = ':';
		FontPrint16(gGame.pTxt, gVar.pFont16, SCR_Width - (8 * 16), nY+16, pTime, 2);
		//
		nY += 32+8;
	}
*/

	gMenuMain.nBlink = 0;

	gMenuHS.nEntry = 0;		// 0, pas de saisie du nom.

	// Music.
	Music_Start(e_YmMusic_Menu, 0);

}

#define	MENU_HISC_ENTRY_TIMEOUT	(FPS_1S * 10)		// Time out sur l'entrée du nom.
// Menu high-scores : Init2, Initialise la saisie du nom.
void MenuHighScores_Init2(void)
{
	MenuHighScores_Init();
	// Music.
	Music_Start(e_YmMusic_HighScore, 1);

	gMenuHS.nEntry = 1;		// 1, saisie d'un high score.
	gMenuHS.nRank = Scr_PutNameInTable("", gHero.nObjTime, gHero.nObjFound);
	gMenuHS.nPos = 0;		// Position dans le texte.

}

// Menu high-scores : Main.
u32 MenuHighScores_Main(void)
{
	u32	nRetVal = MENU_Null;

	gMenuGen.nTimer++;

	// Gestion.
	if (gMenuGen.nPhase < e_MenuGen_Last)
		nRetVal = MenuGen_Phases(e_MenuMain_Manage);
	else
	switch (gMenuGen.nPhase)
	{
	case e_MenuMain_Manage:				// Gestion normale.

		// Saisie en cours ?
		if (gMenuHS.nEntry)
		{	// Oui.
			u32	i;
			u32	nLastPos = gMenuHS.nPos;
			// Touches ?
			if (gMenuHS.nPos < HISC_NameLg - 1)
			{
				for (i = SDLK_a; i <= SDLK_z; i++)
					if (gVar.pKeys[i])
					{
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos++] = i - SDLK_a + 'A';
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos] = 0;
						gVar.pKeys[i] = 0;
						goto _OneLetterDone;
					}
				for (i = SDLK_0; i <= SDLK_9; i++)
					if (gVar.pKeys[i])
					{
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos++] = i - SDLK_0 + '0';
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos] = 0;
						gVar.pKeys[i] = 0;
						goto _OneLetterDone;
					}
				for (i = SDLK_KP0; i <= SDLK_KP9; i++)
					if (gVar.pKeys[i])
					{
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos++] = i - SDLK_KP0 + '0';
						gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos] = 0;
						gVar.pKeys[i] = 0;
						goto _OneLetterDone;
					}
				if (gVar.pKeys[SDLK_SPACE])
				{
					gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos++] = ' ';
					gpHighScores[gMenuHS.nRank].pName[gMenuHS.nPos] = 0;
					gVar.pKeys[SDLK_SPACE] = 0;
					goto _OneLetterDone;
				}
			}
_OneLetterDone:
			if (nLastPos != gMenuHS.nPos) gMenuGen.nTimer = 0;	// RAZ timer si touche.

			// Backspace ?
			if (gVar.pKeys[SDLK_BACKSPACE])
			{
				if (gMenuHS.nPos) gpHighScores[gMenuHS.nRank].pName[--gMenuHS.nPos] = 0;
				gVar.pKeys[SDLK_BACKSPACE] = 0;
				gMenuGen.nTimer = 0;	// RAZ timer. Cas spécial pour backspace en pos 0.
			}

			// Validation ? (Ou time out ?)
			if (gVar.pKeys[SDLK_RETURN] ||
				gMenuGen.nTimer >= MENU_HISC_ENTRY_TIMEOUT)
			{
				// Pas de nom ? => Nom par défaut.
				if (gMenuHS.nPos == 0) strcpy(gpHighScores[gMenuHS.nRank].pName, HISC_DefaultName);
				else
				{	// Que des espaces ? => Nom par défaut.
					i = 0;
					while (gpHighScores[gMenuHS.nRank].pName[i])
					{
						if (gpHighScores[gMenuHS.nRank].pName[i] != ' ') break;
						i++;
					}
					if (gpHighScores[gMenuHS.nRank].pName[i] == 0) strcpy(gpHighScores[gMenuHS.nRank].pName, HISC_DefaultName);
				}
				// Sauvegarde + repassage en mode normal.
				Scr_Save();
				gMenuHS.nEntry = 0;
				gMenuGen.nTimer = 0;
				gVar.pKeys[SDLK_RETURN] = 0;

				// Music. On repasse sur la musique du menu.
				Music_Start(e_YmMusic_Menu, 1);

			}

		}
		else
		{	// Non.

			// Time out ?
			if (gMenuGen.nTimer > MENU_TIMEOUT)
			{
				gMenuGen.nRetVal = MENU_Main;
				gMenuGen.nPhase = e_MenuGen_FadeOut;
			}

			// Appui sur bouton A ou bouton B ?
			if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] ||
				gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]])
			{
				gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] = gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]] = 0;
				gMenuGen.nRetVal = MENU_Game;			// Choix à la fin du fade out.
				gMenuGen.nPhase = e_MenuGen_FadeOut;
			}

		}
		break;
	}

	// Affichage.

	// Copie du bkg.
//	SDL_BlitSurface(gGame.pTxt, NULL, gVar.pScreen, NULL);


	SDL_BlitSurface(gVar.pMarble, NULL, gVar.pScreen, NULL);

	static	char *pHall = "HALL OF FAME";
	FontPrint16(gVar.pScreen, gVar.pFont16, (SCR_Width / 2) - (strlen(pHall) * 8), 16, pHall, 1);

	u32	i;
	u32	nY = 24+12+8;
	u32	nFont;
	char	pPos[] = "0 -";
	for (i = 0; i < HISC_Nb; i++)
	{
		nFont = 2;
		if (gMenuHS.nEntry && i == gMenuHS.nRank) nFont = 3;

		// L1 : Position + nom.
		pPos[0] = i + '1';
		FontPrint16(gVar.pScreen, gVar.pFont16, 0, nY, pPos, 0);
		FontPrint16(gVar.pScreen, gVar.pFont16, 4 * 16, nY, gpHighScores[i].pName, nFont);
		// L2 : Objets + Temps.
		char	pItems[] = "0";
		FontPrint16(gVar.pScreen, gVar.pFont16, 4*16, nY+16, "ITM:", nFont);
		if (gpHighScores[i].nObjects & 0x80)
			FontPrint16(gVar.pScreen, gVar.pFont16, 8*16, nY+16, "ALL", nFont);
		else
		{
			pItems[0] = '0' + gpHighScores[i].nObjects;
			FontPrint16(gVar.pScreen, gVar.pFont16, 8*16, nY+16, pItems, nFont);
		}
		//
		char	pTimeOrg[] = " 0 00 00";
		char	pTime[] = " 0 00 00";
		strcpy(pTime, pTimeOrg);
		Game_TimeToStr(pTime, gpHighScores[i].nTime);
		pTime[5] = ':';
		FontPrint16(gVar.pScreen, gVar.pFont16, SCR_Width - (8 * 16), nY+16, pTime, nFont);
		//
		nY += 32+8;
	}

	// Press fire...
	char	*pPress = "PRESS FIRE TO PLAY";
	char	*pName = "ENTER YOUR NAME";
	char	*pStr = (gMenuHS.nEntry ? pName : pPress);
	if (++gMenuMain.nBlink & (32 | 16))
		FontPrint16(gVar.pScreen, gVar.pFont16, (SCR_Width / 2) - (strlen(pStr) * 8), SCR_Height - 32, pStr, 0);

	return (nRetVal);
}

// Scores - Check si un score entre au Hall of Fame.
// Out : -1, pas dedans / >= 0, rang.
s32 Scr_CheckHighSc(u32 nTime, u32 nObjects)
{
	s32	i, nRank;

	nRank = -1;
	for (i = HISC_Nb - 1; i >= 0; i--)
	{
		if (nObjects > gpHighScores[i].nObjects)
		{
			nRank = i;
		}
		else
		if (nObjects == gpHighScores[i].nObjects && nTime <= gpHighScores[i].nTime)
		{
			nRank = i;
		}
	}

	return (nRank);
}

// Insère un nom dans la table.
s32 Scr_PutNameInTable(char *pName, u32 nTime, u32 nObjects)
{
	s32	nRank = Scr_CheckHighSc(nTime, nObjects);
	s32	i;

	if (nRank < 0) return (nRank);		// Ne devrait pas arriver.

	// Décalage de la table.
	for (i = HISC_Nb - 2; i >= nRank; i--)
	{
		strcpy(gpHighScores[i + 1].pName, gpHighScores[i].pName);
		gpHighScores[i + 1].nObjects = gpHighScores[i].nObjects;
		gpHighScores[i + 1].nTime = gpHighScores[i].nTime;
	}
	// Le score à insérer.
	strcpy(gpHighScores[nRank].pName, pName);
	gpHighScores[nRank].nObjects = nObjects;
	gpHighScores[nRank].nTime = nTime;

	return (nRank);
}

// RAZ de la table des high scores.
void Scr_RazTable(void)
{
//	char	pDefault[HISC_NameLg] = "LUPIN III";//"LUPIN III.......";
	u32	i;

	for (i = 0; i < HISC_Nb; i++)
	{
		memset(gpHighScores[i].pName, 0, HISC_NameLg);
//		strcpy(gpHighScores[i].pName, pDefault);
		strcpy(gpHighScores[i].pName, HISC_DefaultName);
		gpHighScores[i].nTime = GAME_TIME_MAX;
		gpHighScores[i].nObjects = 0;
	}

}

// Calcule le checksum de la table des scores.
u32 Scr_CalcChecksum(void)
{
	u32	i, j;
	u32	nChk = 0;

	for (i = 0; i < HISC_Nb; i++)
	{
		nChk += gpHighScores[i].nTime;
		nChk += gpHighScores[i].nObjects;
		for (j = 0; j < HISC_NameLg; j++) nChk ^= ((u32)gpHighScores[i].pName[j]) << (8 * (j & 3));
	}
	return (nChk);
}

// Lecture du fichier des high-scores.
void Scr_Load(void)
{
	FILE	*pFile;
	u32	nChk;
	u32	nSz;

	if ((pFile = fopen(HISC_Filename, "rb")) != NULL)
	{
		// Le fichier existe, lecture.
		nSz = fread(gpHighScores, 1, HISC_Nb * sizeof(struct SScore), pFile);
		nSz += fread(&nChk, 1, sizeof(u32), pFile);
		fclose(pFile);
		// Taille ok ?
		if (nSz != (HISC_Nb * sizeof(struct SScore)) + sizeof(u32))
		{
			fprintf(stderr, "Scr_Load(): fread error! Resetting table.\n");
			Scr_RazTable();
		}
		// Checksum ok ?
		if (nChk != Scr_CalcChecksum())
		{
			// Wrong checksum, RAZ table.
			fprintf(stderr, "Scr_Load(): Wrong checksum! Resetting table.\n");
			Scr_RazTable();
		}
	}
	else
	{
		// Le fichier n'existe pas, RAZ table.
		Scr_RazTable();
	}

}

// Sauvegarde du fichier des high-scores.
void Scr_Save(void)
{
	FILE	*pFile;
	u32	nChk;
	u32	nSz;

	if ((pFile = fopen(HISC_Filename, "wb")) == NULL)
	{
		fprintf(stderr, "Scr_Save(): Unable to save highscores table.\n");
		return;
	}
	// Sauvegarde des enregistrements.
	nSz = fwrite(gpHighScores, 1, HISC_Nb * sizeof(struct SScore), pFile);
	// Checksum.
	nChk = Scr_CalcChecksum();
	nSz += fwrite(&nChk, 1, sizeof(u32), pFile);
	fclose(pFile);
	// Taille ok ?
	if (nSz != (HISC_Nb * sizeof(struct SScore)) + sizeof(u32))
	{
		fprintf(stderr, "Scr_Save(): fwrite error!\n");
	}

}



//=============================================================================

/*
char *pScrollTxt = "AIRBALL - 25TH ANNIVERSARY REVIVAL - A TRIBUTE BY CLEMENT CORDE...   ";

struct SScroll
{
	char	*pCurChr;
	u32	nPos;
	SDL_Surface	*pScroll;	// Scroll txt.
};
struct SScroll	gScroll;

void Menu_ScrollPutLetter(char pChr, u32 nOffset);

// Init.
void Menu_ScrollInit(void)
{
	gScroll.pCurChr = pScrollTxt;
	gScroll.nPos = 0;

	// Allocation d'une surface pour le texte.
	// Ht = 16 + 1 pour l'ombre !
	gScroll.pScroll = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width * 2, gVar.pFont16->h + 1, 8, gRender.pScreenBuf2->format->Rmask, gVar.pScreen->format->Gmask, gVar.pScreen->format->Bmask, 0);
	if (gScroll.pScroll == NULL)
	{
		fprintf(stderr, "Menu_ScrollInit: Unable to allocate SDL surface: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_FillRect(gScroll.pScroll, NULL, 0);		// Clear.

	// Placement de la première lettre.
	Menu_ScrollPutLetter(*gScroll.pCurChr, ((gScroll.nPos >> 8) & ~0xF) + SCR_Width);

}

// Release.
void Menu_ScrollRelease(void)
{
	SDL_FreeSurface(gScroll.pScroll);

}

void Font_SetTxtColors(SDL_Surface *pDstSurf, u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2, u8 nPalNo, s32 nPosY);

// Affiche une lettre.
// In: nOffset, pile ou il faut.
void Menu_ScrollPutLetter(char pChr, u32 nOffset)
{
	SDL_Rect	sDst;

	// Effacement de l'emplacement actuel.
	sDst.x = nOffset;
	sDst.y = 0;
	sDst.w = sDst.h = 16;
	SDL_FillRect(gScroll.pScroll, &sDst, 0);

	// La lettre en cours.
	if (pChr == ' ') return;
	char	pTxt[2] = " ";
	pTxt[0] = pChr;
	Font_SetTxtColors(gScroll.pScroll, 224, 0, 224,   248, 224, 248,  0,  0);
	FontPrint16(gScroll.pScroll, gVar.pFont16, nOffset, 0, pTxt, 0);

}

// Scroll.
void Menu_ScrollManage(void)
{
	SDL_Rect	sSrc, sDst;
	u32	nLastPos;

	nLastPos = gScroll.nPos;
	gScroll.nPos += 0x380;//0x300;

	// Changement de case ?
	if ((nLastPos ^ gScroll.nPos) >> 12 !=  0)
	{
		// Copie de l'ancienne lettre à gauche.
//		Menu_ScrollPutLetter(*gScroll.pCurChr, ((gScroll.nPos >> 8) & ~0xF) - 16);

		sSrc.x = ((gScroll.nPos >> 8) & ~0xF) + SCR_Width - 16;
		sSrc.y = 0;
		sSrc.w = sSrc.h = 16;
		sDst.x = ((gScroll.nPos >> 8) & ~0xF) - 16;
		sDst.y = 0;
		sDst.w = sDst.h = 16;
		SDL_BlitSurface(gScroll.pScroll, &sSrc, gScroll.pScroll, &sDst);

		// Avance dans le txt.
		gScroll.pCurChr++;
		if (*gScroll.pCurChr == 0) gScroll.pCurChr = pScrollTxt;
		// Swap ?
		if (gScroll.nPos >= SCR_Width << 8) gScroll.nPos -= SCR_Width << 8;
		// Copie de la nouvelle lettre à droite.
		Menu_ScrollPutLetter(*gScroll.pCurChr, ((gScroll.nPos >> 8) & ~0xF) + SCR_Width);
	}

	// Blit.
	sSrc.x = gScroll.nPos >> 8;
	sSrc.y = 0;
	sSrc.w = SCR_Width;
	sSrc.h = 16;
	sDst.x = 0;
	sDst.y = SCR_Height - 16;
	sDst.w = SCR_Width;
	sDst.h = 16;
	SDL_BlitSurface(gScroll.pScroll, &sSrc, gVar.pScreen, &sDst);

}
*/

//=============================================================================

#define	CFG_FILENAME	"airball.cfg"
struct SAirballCfg	gAirballCfg;

// Calcul d'un petit checksum.
u32	CfgChecksumCalc(void)
{
	u32	i;
	u32	nSum = 0;

	for (i = 0; i < e_CfgKey_MAX; i++) nSum += gAirballCfg.pKeys[i];
	nSum += gAirballCfg.nVideoMode;

	return (nSum);
}

/*
// Cmp du qsort.
int CfgCmp(const void *a, const void *b)
{
  return (gSAirballCfg.pKeys[*(u16 *)a] - gSAirballCfg.pKeys[*(u16 *)b]);
}

// Teste si on a pas deux touches identiques.
u32 CfgCheck(void)
{
	u16	pKeysIdx[e_CfgKey_MAX];
	u32	i;

	// Tri de valeurs des touches.
	for (i = 0; i < e_CfgKey_MAX; i++) pKeysIdx[i] = i;
	qsort(pKeysIdx, e_CfgKey_MAX, sizeof(u16), CfgCmp);

	// Si il y a plus de 1 fois une touche, erreur.
	for (i = 0; i < e_CfgKey_MAX-1; i++)
		if (gSAirballCfg.pKeys[pKeysIdx[i + 1]] == gSAirballCfg.pKeys[pKeysIdx[i]]) return (1);

	return (0);
}
*/

// Lecture de la config.
u32 CfgLoad(void)
{
	FILE	*fPt;
	u32	nSz;

	static struct SAirballCfg	sCfgDefault = { { SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_RCTRL, SDLK_SPACE, 0, 1, }, 0, 0 };	// Cfg par défaut.

	fPt = fopen(CFG_FILENAME, "rb");
	if (fPt == NULL)
	{
		fprintf(stderr, "CfgLoad(): fopen error.\n");
		goto _CfgLoad_def;
	}
	nSz = fread(&gAirballCfg, 1, sizeof(struct SAirballCfg), fPt);
	fclose(fPt);
	if (nSz != sizeof(struct SAirballCfg))
	{
		fprintf(stderr, "CfgLoad(): fread error.\n");
		goto _CfgLoad_def;
	}
	// Test du checksum.
	nSz = CfgChecksumCalc();
	if (gAirballCfg.nChecksum != (u16)nSz)
	{
		fprintf(stderr, "CfgLoad(): Checksum error.\n");
		goto _CfgLoad_def;
	}
	return (0);
_CfgLoad_def:
	fprintf(stderr, "CfgLoad(): Default configuration used.\n");
	memcpy(&gAirballCfg, &sCfgDefault, sizeof(struct SAirballCfg));
	return (1);
}




