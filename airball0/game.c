
#include "includes.h"

struct	SGame	gGame;

//=====================================

// Set colors - Title screen.
void SetColors_Menu(void)
{
	u32	i;

	for (i = 0; i < 256; i++)
		gVar.pColorsLog[i] = gVar.pTitle->format->palette->colors[i];

	SDL_SetPalette(gVar.pScreen, SDL_LOGPAL, gVar.pColorsLog, 0, 256);

}

// Set colors - Jeu.
void SetColors_Game(void)
{
	u32	i;

	for (i = 0; i < 256; i++)
		gVar.pColorsLog[i] = gVar.pWalls->format->palette->colors[i];

	SDL_SetPalette(gRoom.pBkg, SDL_LOGPAL, gVar.pColorsLog, 0, 256);	// Pour les couleurs 32 > 47.
	SDL_SetPalette(gVar.pScreen, SDL_LOGPAL, gVar.pColorsLog, 0, 256);

}

// Set colors - Texte.
void SetColors_Txt(void)
{
	u32	i;

	for (i = 0; i < 256; i++)
		gVar.pColorsLog[i] = gVar.pMarble->format->palette->colors[i];

	SDL_SetPalette(gGame.pTxt, SDL_LOGPAL, gVar.pColorsLog, 0, 256);
	SDL_SetPalette(gVar.pScreen, SDL_LOGPAL, gVar.pColorsLog, 0, 256);

}

//=====================================

struct STxtData
{
	u8	nLnNb;		// Nb de lignes.
	s8	nLink;		// Index du texte suivant, -1 si rien. -2 = mort, -3 = victoire.
	s16	nTimer;		// -1 = pas de timer / 0+ = nb de frames max.
	char	*pTb[11];	// Txt. 1er chr = n° de la palette à utiliser.
};

struct STxtData	gTxtData[] = {
	{  1, -1, FPS_1S, { "0GET THE SPELLBOOK!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }},
	{  4,  2, -1, { "0WELL DONE, BALL!", "0", "0BUT YOUR CHALLENGE", "0HAS JUST BEGUN!", NULL, NULL, NULL, NULL, NULL, NULL, NULL }},
	{ 11, -1, -1, { "0BRING ME BACK THESE", "0INGREDIENTS:", "0", "1A CRUCIFIX,", "1A BUDDHA,", "1A PUMPKIN,", "1A DRAGON,", "1A FLASK,", "1AND SOME BEANS.", "0", "0NOW GO!" }},
//	{  4, -2, -1, { "0YOU DIED.", "0", "0YOU SCORED 00000", "0POINTS.", NULL, NULL, NULL, NULL, NULL, NULL, NULL }},
	{  3, -2, -1, { "0YOU DIED.", "0", "0YOU FOUND 0 ITEMS.", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }},
	{  4, -3, FPS_1S * 10, { "0WELL DONE, BALL!", "0", "0I'LL TURN YOU BACK", "0INTO A HUMAN!", NULL, NULL, NULL, NULL, NULL, NULL, NULL }},

};

// Initialisation d'un texte.
#define	TXT_INTERLN	4
#define	TXT_FNT_HT	(16 + TXT_INTERLN)
void Game_TxtInit(void)
{
	u32	i;
	u32	nPosX, nPosY;

	nPosY = (SCR_Height / 2) - (((gTxtData[gGame.nTxtNo].nLnNb * TXT_FNT_HT) - TXT_INTERLN) / 2);

	SetColors_Txt();
	SDL_BlitSurface(gVar.pMarble, NULL, gGame.pTxt, NULL);

	// On recopie les pointeurs de chaîne en local...
	char	*pTb[11];
	for (i = 0; i < 11; i++)
		pTb[i] = gTxtData[gGame.nTxtNo].pTb[i];

	s16	nTimer = gTxtData[gGame.nTxtNo].nTimer;		// On recopie le timer, pour possible altération. (Mort et pas de son).

	// ...pour pouvoir faire des modifs si nécessaire !
	char	pStr[32];
//	// Score sur le texte de mort.
	// Nb d'objets sur le texte de mort.
	if (gGame.nTxtNo == e_Txt_Death)
	{
		strcpy(pStr, gTxtData[gGame.nTxtNo].pTb[2]);
//		// Score.
//		MyItoA(gHero.nScore, &pStr[12]);
		// Nb d'objets.
		pStr[11] += gHero.nObjFound & 0x7F;		// C'est sûr, il ne faut pas dépasser 9.
		if ((gHero.nObjFound & 0x7F) == 1) { pStr[17] = '.'; pStr[18] = 0; }	// 1 Item (sans 's').
		//
		pTb[2] = pStr;

		// Musique mort. (Puisqu'il y a un cas particulier, autant en profiter).
		Music_Start(e_YmMusic_Death, 1);
		// ... et si ça merde (pas de son), on force le timer.
		if (Music_GetMusicNo() != e_YmMusic_Death) nTimer = FPS_1S * 10;
	}

	// Affichage des chaînes.
	for (i = 0; i < gTxtData[gGame.nTxtNo].nLnNb; i++)
	{
		nPosX = (SCR_Width / 2) - ((strlen(pTb[i] + 1) * 16) / 2);
		FontPrint16(gGame.pTxt, gVar.pFont16, nPosX, nPosY, pTb[i] + 1, *pTb[i] - '0');
		nPosY += TXT_FNT_HT;
	}

	// Timer (si présent).
	gGame.nTxtTimer = nTimer;	//gTxtData[gGame.nTxtNo].nTimer;

	gGame.nTxtNo = gTxtData[gGame.nTxtNo].nLink;	// Index du texte suivant.

}

//=====================================

// Init.
void Game_Init(void)
{

	// Recopie les pièces originales dans les pièces pour jouer.
	memcpy(gpRooms, gpRoomsOrg, gnRoomDataSz);

	// On place le crucifix.
	if (gGame.nLastDeathRoom != 0xFFFF)
	{
		u32	nCrucifixX = gGame.nLastDeathBlkX;

		if (gGame.nLastDeathRoom == PUMP_START_ROOM &&	// Si au point de départ, décalage.
			gGame.nLastDeathBlkX == PUMP_START_X &&
			gGame.nLastDeathBlkY == PUMP_START_Y &&
			gGame.nLastDeathBlkZ == PUMP_START_Z - 1)
			nCrucifixX++;

		gpRooms[gGame.nLastDeathRoom].pTab[
			(gGame.nLastDeathBlkZ * ROOM_MAX_X * ROOM_MAX_Y) +
			(gGame.nLastDeathBlkY * ROOM_MAX_X) + nCrucifixX] = BLK_CRUCIFIX;
	}

	SetColors_Game();

	AnmInitEngine();
	Hero_Init();
	Room_Init(PUMP_START_ROOM, -1);
	gGame.nRoomFade = 256;

	gGame.nQuest = 0;
	gGame.nExitCode = 0;

	gGame.nPhase = e_Game_Normal;

	gGame.pLoadSaveTxt = NULL;
	gGame.nLoadSaveCnt = 0;

//	gGame.nScoreDisp = 0;		// Score affiché.

	// On commence par un texte d'intro.
	gGame.nFadeAll = 1;
	gGame.nRoomFade = 0;
	gGame.nTxtNo = e_Txt_Intro;
	gGame.nPhase = e_Game_TxtTransitOn;

	// Music.
	Music_Start(e_YmMusic_Game, 1);

}


#define	ROOM_FADE_STEP	32

// Jeu.
void Game_Manage(void)
{
	// Temps de jeu.
	if (gGame.nPhase <= e_Game_RoomTransitOffToTxt)
	{
		if (gHero.nTimeStop)
			gHero.nTimeStop--;
		else
		if (++gHero.nTime > GAME_TIME_MAX) gHero.nTime = GAME_TIME_MAX;
	}

	// Phases.
	switch (gGame.nPhase)
	{
	case e_Game_Normal:		// Jeu normal.
		Hero_Move();

		// Lecture / Sauvegarde.
		if (gGame.nPhase == e_Game_Normal && AnmGetKey(gHero.nAnm) != e_AnmKey_Hero_Death)
		{
			u32	i;
			for (i = SDLK_F5; i <= SDLK_F8; i++)
				if (gVar.pKeys[i])
				{
					Game_SaveSlot(i - SDLK_F5);
					gVar.pKeys[i] = 0;
					// Sfx.
					Sfx_PlaySfx(e_Sfx_Menu_Click, 100);
					break;
				}

			for (i = SDLK_F1; i <= SDLK_F4; i++)
				if (gVar.pKeys[i])
				{
					Game_LoadSlot(i - SDLK_F1);
					gVar.pKeys[i] = 0;
					// Sfx.
					Sfx_PlaySfx(e_Sfx_Menu_Click, 100);
					break;
				}
		}

		Game_Draw();
		break;

	case e_Game_RoomTransitOff:		// Transition entre les salles - Fade out.
		if (gGame.nRoomFade)
			gGame.nRoomFade -= ROOM_FADE_STEP;		// Fade off.
		else
		{
			// Fade fini, init de la pièce suivante.
			Room_Init(gGame.nRoomNextNo, gGame.nRoomNextDir);
			gGame.nPhase = e_Game_RoomTransitOn;	// Transit on.
		}
		Room_PalSetNo(gRoom.nRoomPal, gGame.nRoomFade);
		Game_Draw();
		break;

	case e_Game_RoomTransitOn:		// Transition entre les salles - Fade on.
		if (gGame.nRoomFade < 256)
			gGame.nRoomFade += ROOM_FADE_STEP;		// Fade on.
		else
			gGame.nPhase = e_Game_Normal;			// Retourne au jeu normal.
		Room_PalSetNo(gRoom.nRoomPal, gGame.nRoomFade);
		if (gGame.nPhase == e_Game_Normal) gGame.nFadeAll = 0;	// ...
		Game_Draw();
		break;

	case e_Game_RoomTransitOffToTxt:	// Transition fade off vers un texte.
		if (gGame.nRoomFade)
			gGame.nRoomFade -= ROOM_FADE_STEP;		// Fade off.
		else
			gGame.nPhase = e_Game_TxtTransitOn;		// Txt transit on.
		Room_PalSetNo(gRoom.nRoomPal, gGame.nRoomFade);
		Game_Draw();
		break;

	case e_Game_TxtTransitOff:		// Texte - Fade off.
		if (gGame.nRoomFade)
			gGame.nRoomFade -= ROOM_FADE_STEP;		// Fade off.
		else
		{
			if (gGame.nTxtNo <= -2)
				gGame.nExitCode = gGame.nTxtNo;			// On quitte !
			if (gGame.nTxtNo == -1)
			{
				SetColors_Game();
				gGame.nPhase = e_Game_RoomTransitOn;	// Fade fini, on repart sur la pièce en cours.
			}
			else
				gGame.nPhase = e_Game_TxtTransitOn;		// Texte suivant.
		}
		FadeColors(gGame.nRoomFade, 256);
		SDL_BlitSurface(gGame.pTxt, NULL, gVar.pScreen, NULL);
		break;

	case e_Game_TxtTransitOn:	// Texte - Fade on.
		if (gGame.nRoomFade == 0) Game_TxtInit();	// Init du texte au premier passage.

		if (gGame.nRoomFade < 256)
			gGame.nRoomFade += ROOM_FADE_STEP;		// Fade on.
		else
		{
			if (gGame.nTxtTimer > 0) gGame.nTxtTimer--;		// Timer.

			if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] ||
				gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]] ||
				gGame.nTxtTimer == 0 ||
				//(gVar.nMusicNo == e_YmMusic_Death && ymMusicIsOver(gVar.ppMusic[gVar.nMusicNo]) == YMTRUE) )
				(Music_GetMusicNo() == e_YmMusic_Death && Music_IsOver()) )
			{
				gGame.nPhase = e_Game_TxtTransitOff;	// Fade off (puis retourne au jeu normal ou un autre texte).
				gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]] = gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]] = 0;
			}
		}
		FadeColors(gGame.nRoomFade, 256);
		SDL_BlitSurface(gGame.pTxt, NULL, gVar.pScreen, NULL);
		break;

	}

}

//=====================================

// Une position écran à partir d'un (x,y,z).
void ISO_ScreenPosition(s32 nPosX, s32 nPosY, s32 nPosZ, s32 *pnPosX, s32 *pnPosY)
{
	*pnPosX = (nPosX - nPosY) + gRoom.nDispOffsX;
	*pnPosY = ((nPosX + nPosY) / 2) + gRoom.nDispOffsY;
	*pnPosY += nPosZ;

}

//=====================================

// Serpent sur le côté de l'écran.
void Room_SnakeDisp(u8 *pnIdx, u8 *pnCnt, s8 *pnInc, u8 *pnFrmMax, u32 nPosX)
{
	SDL_Rect	sSrc, sDst;

	// Avance.
	*pnCnt = *pnCnt + 1;
	if (*pnCnt > *pnFrmMax)
	{
		*pnCnt = 0;
		*pnIdx = (*pnIdx + *pnInc) & 7;
		if (*pnIdx == 0)
		{
			*pnInc = (rand() & 1 ? 1 : -1);
			*pnFrmMax = (rand() & 4 ? 5 : 6);
		}
	}

	// Tête.
	sSrc.x = (*pnIdx) << 5; sSrc.y = 242;
	sSrc.w = 32; sSrc.h = 48;
	sDst.x = nPosX; sDst.y = 0;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	// Corps 1.
	sSrc.y = 242 + 48;
	sSrc.h = 16;
	sDst.x = nPosX; sDst.y = 0 + 48;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	sDst.x = nPosX; sDst.y = 0 + 48 + 32;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	sDst.x = nPosX; sDst.y = 0 + 48 + 32 + 32;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	sDst.x = nPosX; sDst.y = 0 + 48 + 32 + 32 + 32;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	// Corps 2.
	sSrc.y = 242 + 48 + 16;
	sDst.x = nPosX; sDst.y = 0 + 48 + 16;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	sDst.x = nPosX; sDst.y = 0 + 48 + 16 + 32;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	sDst.x = nPosX; sDst.y = 0 + 48 + 16 + 32 + 32;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	// Queue.
	sSrc.y = 242 + 48 + 32;
	sSrc.h = 32;
	sDst.x = nPosX; sDst.y = 0 + 48 + 32 + 32 + 32 + 16;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);

}

// Conversion du temps en hh:mm:ss.
// In: pTxtTime, une chaîne du type : " 0 00 00". / nTime = Temps en frames.
void Game_TimeToStr(char *pTxtTime, u32 nTime)
{
	u32	nFrm, nSec, nMin, nHrs;
	nFrm = nTime;
	nSec = nFrm / FPS_1S;
	nFrm -= nSec * FPS_1S;
	nMin = nSec / 60;
	nSec -= nMin * 60;
	nHrs = nMin / 60;
	nMin -= nHrs * 60;
//	if (nHrs > 99) { nHrs = 99; nMin = 59; nSec = 59; }		// Affichage bloqué. => Inutile, valeur du compteur bloqué.
	pTxtTime[5] = 0;
	pTxtTime[2] = 0;
	MyItoA(nSec, &pTxtTime[6]);
	MyItoA(nMin, &pTxtTime[3]);
	MyItoA(nHrs, &pTxtTime[0]);
	pTxtTime[5] = (nFrm > FPS_1S / 2 ? '.' : ':');
	pTxtTime[2] = ':';

}


// Tracé du jeu.
void Game_Draw(void)
{

	SDL_BlitSurface(gRoom.pBkg, NULL, gVar.pScreen, NULL);		// Copie du décor.

	// Serpents sur les côtés de l'écran.
	static	u8	nSnake1Idx = 0;
	static	u8	nSnake1Cnt = 0;
	static	u8	nSnake1FrmMax = 6;
	static	s8	nSnake1Inc = 1;
	static	u8	nSnake2Idx = 0;
	static	u8	nSnake2Cnt = 0;
	static	u8	nSnake2FrmMax = 5;
	static	s8	nSnake2Inc = 1;
	Room_SnakeDisp(&nSnake1Idx, &nSnake1Cnt, &nSnake1Inc, &nSnake1FrmMax, 0);
	Room_SnakeDisp(&nSnake2Idx, &nSnake2Cnt, &nSnake2Inc, &nSnake2FrmMax, SCR_Width - 32);

	// Si cadeau, on le rajoute dans les blocs au moment de l'affichage.
	if (gRoom.nPresent >= 0)
		gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * gRoom.nPresentPosZ) + (ROOM_MAX_X * gRoom.nPresentPosY) + gRoom.nPresentPosX] = 255;

	// Le décor.
	SDL_Rect	sSrc, sDst;
	s32	nScrX, nScrY;

	s32	x, y, z;
	u32	nBlk;

	// Position du joueur.
	u32	nPlayerXMin, nPlayerXMax, nPlayerYMin, nPlayerYMax;
	nPlayerXMin = gHero.nPosX >> 12;
	nPlayerYMin = gHero.nPosY >> 12;
	nPlayerXMax = (gHero.nPosX + ((MAP_TILEW - 1) << 8)) >> 12;
	nPlayerYMax = (gHero.nPosY + ((MAP_TILEH - 1) << 8)) >> 12;

	// Tout ce qui est derrière le joueur.
	for (y = 0; y < ROOM_MAX_Y; y++)
	for (x = 0; x < ROOM_MAX_X; x++)
	{
		if (x < nPlayerXMin || y < nPlayerYMin)
		if (x < gRoom.pRoomData->nDimX && y < gRoom.pRoomData->nDimY)
		for (z = ROOM_MAX_Z - 2; z >= 0; z--)
		{
			nBlk = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (y * ROOM_MAX_X) + x];
			if (nBlk)
			{
				sSrc.x = (nBlk & 15) * GFX_BLK_W;
				sSrc.y = (nBlk >> 4) * GFX_BLK_H;
				sSrc.w = GFX_BLK_W;
				sSrc.h = GFX_BLK_H;
				ISO_ScreenPosition(x * MAP_TILEW, y * MAP_TILEH, z * MAP_TILEZ, &nScrX, &nScrY);
				sDst.x = nScrX - (GFX_BLK_W / 2);
				sDst.y = nScrY - (GFX_BLK_H - 32);
				SDL_BlitSurface(gVar.pBlk, &sSrc, gVar.pScreen, &sDst);
			}
		}
	}

	// Les cases au niveau du joueur, dessous.
	for (y = nPlayerYMin; y <= nPlayerYMax; y++)
	for (x = nPlayerXMin; x <= nPlayerXMax; x++)
	for (z = ROOM_MAX_Z - 2; z >= gHero.nPosZ >> 12; z--)
	{
		nBlk = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (y * ROOM_MAX_X) + x];
		if (nBlk)
		{
			sSrc.x = (nBlk & 15) * GFX_BLK_W;
			sSrc.y = (nBlk >> 4) * GFX_BLK_H;
			sSrc.w = GFX_BLK_W;
			sSrc.h = GFX_BLK_H;
			ISO_ScreenPosition(x * MAP_TILEW, y * MAP_TILEH, z * MAP_TILEZ, &nScrX, &nScrY);
			sDst.x = nScrX - (GFX_BLK_W / 2);
			sDst.y = nScrY - (GFX_BLK_H - 32);
			SDL_BlitSurface(gVar.pBlk, &sSrc, gVar.pScreen, &sDst);
		}
	}

	// Le héros.
/*
AnmGetImage(gHero.nAnm);
	sSrc.x = 32;//6*32;//0;
	sSrc.y = 0;
	sSrc.w = GFX_BLK_W;
	sSrc.h = GFX_BLK_H;
	ISO_ScreenPosition(gHero.nPosX >> 8, gHero.nPosY >> 8, gHero.nPosZ >> 8, &nScrX, &nScrY);
	sDst.x = nScrX - (GFX_BLK_W / 2);
	sDst.y = nScrY - (GFX_BLK_H - 32);
	SDL_BlitSurface(gVar.pBlk, &sSrc, gVar.pScreen, &sDst);
*/
/*
*/
//	x = AnmGetImage(gHero.nAnm);
	x = (gGame.nPhase == e_Game_Normal ? AnmGetImage(gHero.nAnm) : AnmGetLastImage(gHero.nAnm));
	if (x != SPR_NoSprite)
	{
		y = (AnmGetKey(gHero.nAnm) == e_AnmKey_Hero_Death ? 0 : 4 - ((gHero.nAirLevel >> 8) / 32) );	// Selon le niveau d'air...
		sSrc.x = (y + x) * GFX_BLK_W;
		sSrc.y = 0;
		sSrc.w = GFX_BLK_W;
		sSrc.h = GFX_BLK_H;
		ISO_ScreenPosition(gHero.nPosX >> 8, gHero.nPosY >> 8, gHero.nPosZ >> 8, &nScrX, &nScrY);
		sDst.x = nScrX - (GFX_BLK_W / 2);
		sDst.y = nScrY - (GFX_BLK_H - 32);
		SDL_BlitSurface(gVar.pBall, &sSrc, gVar.pScreen, &sDst);
	}

	// Les cases au niveau du joueur, dessus.
	for (y = nPlayerYMin; y <= nPlayerYMax; y++)
	for (x = nPlayerXMin; x <= nPlayerXMax; x++)
//	for (z = gHero.nPosZ >> 12; z >= 0; z--)
	for (z = (gHero.nPosZ >> 12) - 1; z >= 0; z--)
	{
		nBlk = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (y * ROOM_MAX_X) + x];
		if (nBlk)
		{
			sSrc.x = (nBlk & 15) * GFX_BLK_W;
			sSrc.y = (nBlk >> 4) * GFX_BLK_H;
			sSrc.w = GFX_BLK_W;
			sSrc.h = GFX_BLK_H;
			ISO_ScreenPosition(x * MAP_TILEW, y * MAP_TILEH, z * MAP_TILEZ, &nScrX, &nScrY);
			sDst.x = nScrX - (GFX_BLK_W / 2);
			sDst.y = nScrY - (GFX_BLK_H - 32);
			SDL_BlitSurface(gVar.pBlk, &sSrc, gVar.pScreen, &sDst);
		}
	}

	// Tout ce qui est devant le joueur.
	for (y = 0; y < ROOM_MAX_Y; y++)
	for (x = 0; x < ROOM_MAX_X; x++)
	{
		if (x > nPlayerXMax || y > nPlayerYMax)
		if (x < gRoom.pRoomData->nDimX && y < gRoom.pRoomData->nDimY)
		for (z = ROOM_MAX_Z - 2; z >= 0; z--)
		{
			nBlk = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (y * ROOM_MAX_X) + x];
			if (nBlk)
			{
				sSrc.x = (nBlk & 15) * GFX_BLK_W;
				sSrc.y = (nBlk >> 4) * GFX_BLK_H;
				sSrc.w = GFX_BLK_W;
				sSrc.h = GFX_BLK_H;
				ISO_ScreenPosition(x * MAP_TILEW, y * MAP_TILEH, z * MAP_TILEZ, &nScrX, &nScrY);
				sDst.x = nScrX - (GFX_BLK_W / 2);
				sDst.y = nScrY - (GFX_BLK_H - 32);
				SDL_BlitSurface(gVar.pBlk, &sSrc, gVar.pScreen, &sDst);
			}
		}
	}

	// On retire le cadeau des blocs de collision.
	if (gRoom.nPresent >= 0)
		gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * gRoom.nPresentPosZ) + (ROOM_MAX_X * gRoom.nPresentPosY) + gRoom.nPresentPosX] = 0;

	// Air.
	sDst.x = HUD_AIR_POSX;
	sDst.y = HUD_AIR_POSY;
	sDst.w = gHero.nAirLevel >> 8;
	sDst.h = 1;
	SDL_FillRect(gVar.pScreen, &sDst, 12 +32);
	sDst.y += 1;
	SDL_FillRect(gVar.pScreen, &sDst, 11 +32);
/*
// > Fond de jauge passé dans le décor.
	if (gHero.nAirLevel >> 8 < HERO_AIR_MAX)
	{
		sDst.x = HUD_AIR_POSX + (gHero.nAirLevel >> 8);
		sDst.y = HUD_AIR_POSY;
		sDst.w = HERO_AIR_MAX - (gHero.nAirLevel >> 8);
		sDst.h = 2;
		SDL_FillRect(gVar.pScreen, &sDst, 15 +32);
	}
*/

	// Score.
/*
//	FontPrint16(gVar.pScreen, gVar.pFont16, HUD_SCORE_POSX, HUD_SCORE_POSYL1, "SCORE", 0);
	if (gGame.nScoreDisp < gHero.nScore) gGame.nScoreDisp++;	// Score affiché, progression.
	char	pTxtScore[] = "00000";
	MyItoA(gGame.nScoreDisp, pTxtScore);
	FontPrint16(gVar.pScreen, gVar.pFont16, HUD_SCORE_POSX, HUD_SCORE_POSYL2, pTxtScore, 0);
*/
	// Time.
	x = (gHero.nTimeStop ? 1 : 0);	// Palette ! Violet = normal / rouge = stop.
	char	pTxtTime[] = " 0 00 00";
	Game_TimeToStr(pTxtTime, gHero.nTime);
	FontPrint16(gVar.pScreen, gVar.pFont16, HUD_SCORE_POSX-(16*3), HUD_SCORE_POSYL2, pTxtTime, x);
	FontPrint16(gVar.pScreen, gVar.pFont16, HUD_SCORE_POSX + 16, HUD_SCORE_POSYL1, "TIME", x);

	// Texte load/save.
	if (gGame.nLoadSaveCnt)
	{
		FontPrint16(gVar.pScreen, gVar.pFont16, (SCR_Width / 2) - (strlen(&gGame.pLoadSaveTxt[1]) * 8), 16, &gGame.pLoadSaveTxt[1], gGame.pLoadSaveTxt[0] - '0');
		gGame.nLoadSaveCnt--;
	}

}

//=====================================

#pragma pack(1)
struct	SSaveSlot
{
	u32	nMagic;
	u32	nCheckSum;

	u16	gRoom_nRoomNo;
	u16	gRoom_nPumpRoom;
	u8	gRoom_nPumpX, gRoom_nPumpY, gRoom_nPumpZ;
	u8	nPad0;

	s8	gRoom_nPresent;
	u8	gRoom_nPresentPosX, gRoom_nPresentPosY, gRoom_nPresentPosZ;

	struct SHero	gHero_gHero;

	u16	gGame_nLastDeathRoom;
	u8	gGame_nLastDeathBlkX, gGame_nLastDeathBlkY, gGame_nLastDeathBlkZ;
	u8	gGame_nQuest;

};
#pragma pack()

u8	*gpSvgSlot;
u32	gnSvgSlotSz;
#define	SAV_MAGIC	0x41495200

// Petit calcul de checksum pour la sauvegarde.
u32 Slot_sub_Checksum(void)
{
	u32	nSum = 0;
	u32	i;

	for (i = 0; i < gnSvgSlotSz; i++)
		nSum ^= (*(gpSvgSlot + i)) << (8 * (i & 3));

//	printf("Checksum = %x\n", nSum);
	return (nSum);
}

// Init du slot de sauvegarde.
void Game_LoadSaveInit(void)
{
	gnSvgSlotSz = sizeof(struct SSaveSlot) + gnRoomDataSz;
	if ((gpSvgSlot = (u8 *)malloc(gnSvgSlotSz)) == NULL)
	{
		fprintf(stderr, "Game_LoadSaveInit(): malloc error. Aborted.\n");
		exit(1);
	}
}

// Libère le slot de sauvegarde.
void Game_LoadSaveRelease(void)
{
	free(gpSvgSlot);
}

#define	GAME_TXTLOADSAVE_CNT	(90)
char	pTxtError[] = "1SLOT 0 - ERROR";

// Sauvegarde.
u32	Game_SaveSlot(u32 nSlotNo)
{
	// Les datas.
	struct	SSaveSlot	*pSav;
	pSav = (struct SSaveSlot *)gpSvgSlot;

	pSav->nMagic = SAV_MAGIC;
	pSav->nCheckSum = 0;		// Pour première passe.

	pSav->gRoom_nRoomNo = gRoom.nRoomNo;
	pSav->gRoom_nPumpRoom = gRoom.nPumpRoom;
	pSav->gRoom_nPumpX = gRoom.nPumpX;
	pSav->gRoom_nPumpY = gRoom.nPumpY;
	pSav->gRoom_nPumpZ = gRoom.nPumpZ;

	pSav->gRoom_nPresent = gRoom.nPresent;
	pSav->gRoom_nPresentPosX = gRoom.nPresentPosX;
	pSav->gRoom_nPresentPosY = gRoom.nPresentPosY;
	pSav->gRoom_nPresentPosZ = gRoom.nPresentPosZ;

	memcpy(&pSav->gHero_gHero, &gHero, sizeof(struct SHero));

	pSav->gGame_nLastDeathRoom = gGame.nLastDeathRoom;
	pSav->gGame_nLastDeathBlkX = gGame.nLastDeathBlkX;
	pSav->gGame_nLastDeathBlkY = gGame.nLastDeathBlkY;
	pSav->gGame_nLastDeathBlkZ = gGame.nLastDeathBlkZ;
	pSav->gGame_nQuest = gGame.nQuest;

	// Les salles.
	memcpy(gpSvgSlot + sizeof(struct SSaveSlot), gpRooms, gnRoomDataSz);

	// Checksum.
	pSav->nCheckSum = Slot_sub_Checksum();
//printf("Save / Checksum = %x\n", pSav->nCheckSum);

	// Sauvegarde.
	FILE	*fPt;
	char	pFilename[] = "slot0.svg";
	static char	pTxtSave[] = "0SLOT 0 - SAVED";
	u32	nSz;

	pFilename[4] += nSlotNo & 3;		// N° du slot.
	pTxtSave[5+1] = (nSlotNo & 3) + '1';	// N° du slot.
	pTxtError[5+1] = (nSlotNo & 3) + '1';	// N° du slot.
	gGame.nLoadSaveCnt = 0;		// Au cas où, on n'affiche plus rien.

	// Par défaut, texte "error".
	gGame.pLoadSaveTxt = pTxtError;
	gGame.nLoadSaveCnt = GAME_TXTLOADSAVE_CNT;

	if ((fPt = fopen(pFilename, "wb")) == NULL)
	{
		fprintf(stderr, "Game_SaveSlot(): fopen error.\n");
		return (1);
	}
	nSz = fwrite(gpSvgSlot, 1, gnSvgSlotSz, fPt);
	fclose(fPt);

	if (nSz != gnSvgSlotSz)
	{
		fprintf(stderr, "Game_SaveSlot(): fwrite error.\n");
		return (1);
	}

	// Affichage "ok".
	gGame.pLoadSaveTxt = pTxtSave;
	gGame.nLoadSaveCnt = GAME_TXTLOADSAVE_CNT;
	return (0);
}

// Lecture.
u32	Game_LoadSlot(u32 nSlotNo)
{
	// Lecture.
	FILE	*fPt;
	char	pFilename[] = "slot0.svg";
	static char	pTxtLoad[] = "0SLOT 0 - LOADED";
	u32	nSz;

	pFilename[4] += nSlotNo & 3;		// N° du slot.
	pTxtLoad[5+1] = (nSlotNo & 3) + '1';	// N° du slot.
	pTxtError[5+1] = (nSlotNo & 3) + '1';	// N° du slot.
	gGame.nLoadSaveCnt = 0;		// Au cas où, on n'affiche plus rien.

	// Par défaut, texte "error".
	gGame.pLoadSaveTxt = pTxtError;
	gGame.nLoadSaveCnt = GAME_TXTLOADSAVE_CNT;

	if ((fPt = fopen(pFilename, "rb")) == NULL)
	{
		fprintf(stderr, "Game_LoadSlot(): fopen error.\n");
		return (1);
	}
	nSz = fread(gpSvgSlot, 1, gnSvgSlotSz, fPt);
	fclose(fPt);

	if (nSz != gnSvgSlotSz)
	{
		fprintf(stderr, "Game_LoadSlot(): read error.\n");
		return (1);
	}

	// Tests.
	struct	SSaveSlot	*pSav;
	pSav = (struct SSaveSlot *)gpSvgSlot;

	// Magic number.
	if (pSav->nMagic != SAV_MAGIC)
	{
		fprintf(stderr, "Game_LoadSlot(): Magic number not found.\n");
		return (1);
	}
	// Checksum.
	u32	nChecksumL, nChecksumC;
	nChecksumL = pSav->nCheckSum;
	pSav->nCheckSum = 0;
	nChecksumC = Slot_sub_Checksum();
	if (nChecksumL != nChecksumC)
	{
		fprintf(stderr, "Game_LoadSlot(): Wrong checksum (Loaded: %x / Calculated: %x).\n", nChecksumL, nChecksumC);
		return (1);
	}

	// Les datas.
	gRoom.nRoomNo = pSav->gRoom_nRoomNo;
	gRoom.nPumpRoom = pSav->gRoom_nPumpRoom;
	gRoom.nPumpX = pSav->gRoom_nPumpX;
	gRoom.nPumpY = pSav->gRoom_nPumpY;
	gRoom.nPumpZ = pSav->gRoom_nPumpZ;

	memcpy(&gHero, &pSav->gHero_gHero, sizeof(struct SHero));
//	gGame.nScoreDisp = gHero.nScore;

	//>>> !!! Attention !!!
	// Comme il n'y a qu'une seule anim dans tout le jeu, l'anim du joueur est TOUJOURS la 0.
	// Et comme on lit depuis le jeu, le slot est initialisé.
	// => Solution de facilité, on force l'anim de idle. (Très peu de cas possibles : idle / course / atterrissage. Seul l'atterrissage sera coupé).
	AnmSet(gAnm_Hero_Idle, gHero.nAnm);
	//<<< !!! Attention !!!

	gGame.nLastDeathRoom = pSav->gGame_nLastDeathRoom;
	gGame.nLastDeathBlkX = pSav->gGame_nLastDeathBlkX;
	gGame.nLastDeathBlkY = pSav->gGame_nLastDeathBlkY;
	gGame.nLastDeathBlkZ = pSav->gGame_nLastDeathBlkZ;
	gGame.nQuest = pSav->gGame_nQuest;

	// Les salles.
	memcpy(gpRooms, gpSvgSlot + sizeof(struct SSaveSlot), gnRoomDataSz);

	// Initialisation de la salle en cours.
	Room_Init(gRoom.nRoomNo, -2);
	Room_PalSetNo(gRoom.nRoomPal, 256);

	// Et on restaure le cadeau de la sauvegarde !
	gRoom.nPresent = pSav->gRoom_nPresent;
	gRoom.nPresentPosX = pSav->gRoom_nPresentPosX;
	gRoom.nPresentPosY = pSav->gRoom_nPresentPosY;
	gRoom.nPresentPosZ = pSav->gRoom_nPresentPosZ;
	Room_PresentBlkReprogram();

	// Affichage "ok".
	gGame.pLoadSaveTxt = pTxtLoad;
	gGame.nLoadSaveCnt = GAME_TXTLOADSAVE_CNT;
	return (0);
}

