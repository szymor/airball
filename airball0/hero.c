
#include "includes.h"

struct SHero	gHero;

#define	HERO_LIVES_START	(3)

#define	GAME_TIMERSTOP	(15 * FPS_1S)
#define	GAME_TIMERSTOP_MAX	(60 * FPS_1S)

//=====================================

// Initialisation du héros.
void Hero_Init(void)
{
	// Init des coordonnées du premier gonfleur, pour le premier Room_Init().
	gRoom.nPumpRoom = PUMP_START_ROOM;
	gRoom.nPumpX = PUMP_START_X;
	gRoom.nPumpY = PUMP_START_Y;
	gRoom.nPumpZ = PUMP_START_Z;

	gHero.nAnm = AnmSet(gAnm_Hero_Idle, -1);
	assert(gHero.nAnm != -1);

	gHero.nTime = 0;
	gHero.nTimeStop = 0;
	gHero.nObjFound = 0;
	gHero.nObjTime = 0;

	gHero.nLives = HERO_LIVES_START;		// Nombre de vies.
//	gHero.nScore = 0;

	gHero.nObject = 0;			// Objet porté. (No du bloc).
//gHero.nObject = BLK_SPELLBOOK;//BLK_CANDLE;//BLK_TORCH;//0;	//tst

	Hero_LifeInit();	// Init générique. (En fait, même pas utile, appelé dans RoomInit juste après).

}


enum
{
	COLL_NULL	= 0,
	COLL_HARD	= 1,
	COLL_SPIKE	= 1 << 1,
	COLL_PUMP	= 1 << 2,
};

// Teste le déplacement du héros en X.
// Out: 0 = Ok / 1 = Mvt pas possible.
u32 Hero_CheckMoveX(s32 nSpdX)
{
	s32	nPosX;

	if (nSpdX == 0) return (COLL_NULL);

	// Limites de la salle.
	if (gHero.nPosX + nSpdX < 0 || (gHero.nPosX + nSpdX + ((MAP_TILEW - 1) << 8)) >> 12 >= gRoom.pRoomData->nDimX) return (COLL_HARD);

	// Blocs.
	nPosX = gHero.nPosX + nSpdX;
	if (nSpdX > 0) nPosX += (MAP_TILEW - 1) << 8;

	u8	pBlk[4];
	u32	i;
	u32	nCol;

	pBlk[0] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (gHero.nPosZ >> 12)) + ((gHero.nPosY >> 12) * ROOM_MAX_X) + (nPosX >> 12)];
	pBlk[1] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (gHero.nPosZ >> 12)) + (((gHero.nPosY + ((MAP_TILEW - 1) << 8)) >> 12) * ROOM_MAX_X) + (nPosX >> 12)];
	pBlk[2] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * ((gHero.nPosZ + ((MAP_TILEZ - 1) << 8)) >> 12)) + ((gHero.nPosY >> 12) * ROOM_MAX_X) + (nPosX >> 12)];
	pBlk[3] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * ((gHero.nPosZ + ((MAP_TILEZ - 1) << 8)) >> 12)) + (((gHero.nPosY + ((MAP_TILEW - 1) << 8)) >> 12) * ROOM_MAX_X) + (nPosX >> 12)];

	// Une collision ?
	nCol = 0;
	for (i = 0; i < 4; i++) nCol += pBlk[i];

	// Si col, piquant ?
	if (nCol)
	{
		for (i = 0; i < 4; i++)
			if (Room_BlockIsSpiky(pBlk[i], (nSpdX > 0 ? 1 << 3 : 1 << 1))) return (COLL_HARD | COLL_SPIKE);	// Col piquante.
		return (COLL_HARD);	// Col normale.
	}

	return (COLL_NULL);		// Pas de col.

}
// Teste le déplacement du héros en Y.
// Out: 0 = Ok / 1 = Mvt pas possible.
u32 Hero_CheckMoveY(s32 nSpdY)
{
	s32	nPosY;

	if (nSpdY == 0) return (COLL_NULL);

	// Limites de la salle.
	if (gHero.nPosY + nSpdY < 0 || (gHero.nPosY + nSpdY + ((MAP_TILEH - 1) << 8)) >> 12 >= gRoom.pRoomData->nDimY) return (COLL_HARD);

	// Blocs.
	nPosY = gHero.nPosY + nSpdY;
	if (nSpdY > 0) nPosY += (MAP_TILEH - 1) << 8;

	u8	pBlk[4];
	u32	i;
	u32	nCol;

	pBlk[0] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (gHero.nPosZ >> 12)) + ((nPosY >> 12) * ROOM_MAX_X) + (gHero.nPosX >> 12)];
	pBlk[1] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (gHero.nPosZ >> 12)) + ((nPosY >> 12) * ROOM_MAX_X) + ((gHero.nPosX + ((MAP_TILEW - 1) << 8)) >> 12)];
	pBlk[2] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * ((gHero.nPosZ + ((MAP_TILEZ - 1) << 8)) >> 12)) + ((nPosY >> 12) * ROOM_MAX_X) + (gHero.nPosX >> 12)];
	pBlk[3] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * ((gHero.nPosZ + ((MAP_TILEZ - 1) << 8)) >> 12)) + ((nPosY >> 12) * ROOM_MAX_X) + ((gHero.nPosX + ((MAP_TILEW - 1) << 8)) >> 12)];

	// Une collision ?
	nCol = 0;
	for (i = 0; i < 4; i++) nCol += pBlk[i];

	// Si col, piquant ?
	if (nCol)
	{
		for (i = 0; i < 4; i++)
			if (Room_BlockIsSpiky(pBlk[i], (nSpdY > 0 ? 1 << 2 : 1 << 0))) return (COLL_HARD | COLL_SPIKE);	// Col piquante.
		return (COLL_HARD);	// Col normale.
	}

	return (COLL_NULL);		// Pas de col.

}
// Teste la position du héros en Z.
// Out: 0 = Ok / 1 = Mvt pas possible.
u32 Hero_CheckMoveZ(s32 nPosZ, s32 nDir)
{
	// Blocs.
	u8	pBlk[4];
	u32	i;
	u32	nCol;

/*
	pBlk[0] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ >> 12)) + ((gHero.nPosY >> 12) * ROOM_MAX_X) + (gHero.nPosX >> 12)];
	pBlk[1] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ >> 12)) + ((gHero.nPosY >> 12) * ROOM_MAX_X) + ((gHero.nPosX + ((MAP_TILEW - 1) << 8)) >> 12)];
	pBlk[2] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ >> 12)) + (((gHero.nPosY + ((MAP_TILEH - 1) << 8)) >> 12) * ROOM_MAX_X) + (gHero.nPosX >> 12)];
	pBlk[3] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ >> 12)) + (((gHero.nPosY + ((MAP_TILEH - 1) << 8)) >> 12) * ROOM_MAX_X) + ((gHero.nPosX + ((MAP_TILEW - 1) << 8)) >> 12)];
*/

	u32	pPosXY[8];	// Coordonnées (x,y) des blocs testés, en fait juste pour le gonfleur.
	// x, y
	pPosXY[0] = gHero.nPosX >> 12;
	pPosXY[1] = gHero.nPosY >> 12;
	// x+1, y
	pPosXY[2] = (gHero.nPosX + ((MAP_TILEW - 1) << 8)) >> 12;
	pPosXY[3] = pPosXY[1];
	// x, y+1
	pPosXY[4] = pPosXY[0];
	pPosXY[5] = (gHero.nPosY + ((MAP_TILEH - 1) << 8)) >> 12;
	// x+1, y+1
	pPosXY[6] = pPosXY[2];
	pPosXY[7] = pPosXY[5];

	pBlk[0] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ >> 12)) + (pPosXY[1] * ROOM_MAX_X) + pPosXY[0]];
	pBlk[1] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ >> 12)) + (pPosXY[3] * ROOM_MAX_X) + pPosXY[2]];
	pBlk[2] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ >> 12)) + (pPosXY[5] * ROOM_MAX_X) + pPosXY[4]];
	pBlk[3] = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ >> 12)) + (pPosXY[7] * ROOM_MAX_X) + pPosXY[6]];

	// Une collision ?
	nCol = 0;
	for (i = 0; i < 4; i++) nCol += pBlk[i];

	// Si col, piquant ?
	if (nCol)
	{
		u32	nRet = COLL_HARD;
		for (i = 0; i < 4; i++)
		{
//			if (nDir > 0 && pBlk[i] == BLK_PUMP) nRet |= COLL_PUMP;		// Gonfleur ?
			if (nDir > 0 && pBlk[i] == BLK_PUMP)	// Gonfleur ?
			{
				// On note la position de la pompe.
				gRoom.nPumpRoom = gRoom.nRoomNo;
				gRoom.nPumpX = pPosXY[i * 2];
				gRoom.nPumpY = pPosXY[(i * 2) + 1];
				gRoom.nPumpZ = nPosZ >> 12;
//printf("pump room=%d / x=%d / y=%d / z=%d\n", gRoom.nPumpRoom, gRoom.nPumpX, gRoom.nPumpY, gRoom.nPumpZ);
				nRet |= COLL_PUMP;
			}

			if (Room_BlockIsSpiky(pBlk[i], (nDir > 0 ? 1 << 4 : 1 << 5))) return (COLL_HARD | COLL_SPIKE);	// Col piquante.
		}
		return (nRet);	// Col normale.
	}

	return (COLL_NULL);		// Pas de col.

}


#define	TXT_INIT(nTxtId) \
gGame.nFadeAll = 1; \
gGame.nTxtNo = nTxtId; \
gGame.nPhase = e_Game_RoomTransitOffToTxt; \


#define	EXIT_TST_X \
		if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * nBlkZ) + (nBlkY * ROOM_MAX_X) + (nBlkX)] == EXIT_BLK_X1) \
		{ \
			nExitNo = gRoom.pRoomData->pExits[(nSpdX > 0 ? 1 : 3)]; \
			if (nExitNo >= 0) \
			{ \
				gGame.nPhase = e_Game_RoomTransitOff; \
				gGame.nRoomNextNo = nExitNo; \
				gGame.nRoomNextDir = ((nSpdX > 0 ? 1 : 3) + 2) & 3; \
				return (1); \
			} \
		}

#define EXIT_TST_Y \
		if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * nBlkZ) + (nBlkY * ROOM_MAX_X) + (nBlkX)] == EXIT_BLK_Y0) \
		{ \
			nExitNo = gRoom.pRoomData->pExits[(nSpdY > 0 ? 2 : 0)]; \
			if (nExitNo >= 0) \
			{ \
				gGame.nPhase = e_Game_RoomTransitOff; \
				gGame.nRoomNextNo = nExitNo; \
				gGame.nRoomNextDir = ((nSpdY > 0 ? 2 : 0) + 2) & 3; \
				return (1); \
			} \
		}

// Test des sorties.
// Out: 1 = Sortie / 0 = Rien.
u32 Hero_ExitCheck(s32 nSpdX, s32 nSpdY)
{
	s32	nBlkX, nBlkY, nBlkZ;
	s32	nExitNo;

	if (nSpdX)
	{
		nBlkX = (gHero.nPosX + nSpdX + (nSpdX > 0 ? (MAP_TILEW - 1) << 8 : 0) ) >> 12;
		if ((u32)nBlkX >= gRoom.pRoomData->nDimX) goto _skipx;
		nBlkY = gHero.nPosY >> 12;
		nBlkZ = (gHero.nPosZ + ((MAP_TILEZ - 1) << 8)) >> 12;
		EXIT_TST_X
		// Re-test 1 pixel plus loin pour le cas ou on est pile sur le 2ème bloc de sortie.
		nBlkY = (gHero.nPosY - 0x100) >> 12;
		if ((u32)nBlkY >= gRoom.pRoomData->nDimY) goto _skipx;
		EXIT_TST_X
	}
_skipx:
	if (nSpdY)
	{
		nBlkY = (gHero.nPosY + nSpdY + (nSpdY > 0 ? (MAP_TILEH - 1) << 8 : 0) ) >> 12;
		if ((u32)nBlkY >= gRoom.pRoomData->nDimY) goto _skipy;
		nBlkX = gHero.nPosX >> 12;
		nBlkZ = (gHero.nPosZ + ((MAP_TILEZ - 1) << 8)) >> 12;
		EXIT_TST_Y
		// Re-test 1 pixel plus loin pour le cas ou on est pile sur le 2ème bloc de sortie.
		nBlkX = (gHero.nPosX - 0x100) >> 12;
		if ((u32)nBlkX >= gRoom.pRoomData->nDimX) goto _skipy;
		EXIT_TST_Y
	}
_skipy:
	return (0);
}

// Prend/pose un objet.
void Hero_GetDrop(void)
{
	s32	nPosZ;

	nPosZ = gHero.nPosZ >> 12;

	s32	nXMin, nXMax, nYMin, nYMax;

	nXMin = gHero.nPosX >> 12;
	nXMax = nXMin + ((gHero.nPosX & 0xF00) ? 1 : 0);
	nYMin = gHero.nPosY >> 12;
	nYMax = nYMin + ((gHero.nPosY & 0xF00) ? 1 : 0);

	s32	iMin, iMax, jMin, jMax;

	if (nXMin == nXMax)
	{	// Cas où on est pile sur un bloc.
		iMin = nXMin - 1;
		iMax = nXMax + 1;
	}
	else
	if ((gHero.nPosX & 0xF00) > 0x800)
	{
		iMin = nXMin;
		iMax = nXMax + 1;
	}
	else
	{
		iMin = nXMin - 1;
		iMax = nXMax;
	}

	if (nYMin == nYMax)
	{
		jMin = nYMin - 1;
		jMax = nYMax + 1;
	}
	else
	if ((gHero.nPosY & 0xF00) > 0x800)
	{	// Cas où on est pile sur un bloc.
		jMin = nYMin;
		jMax = nYMin + 1;
	}
	else
	{
		jMin = nYMin - 1;
		jMax = nYMin;
	}

	s32	i, j;
	for (j = jMin; j <= jMax; j++)
	for (i = iMin; i <= iMax; i++)
	{
		if (j == nYMin - 1 || j == nYMax + 1 || i == nXMin - 1 || i == nXMax + 1)	// Pas sur le joueur.
		if ((u32)i < gRoom.pRoomData->nDimX && (u32)j < gRoom.pRoomData->nDimY)
		{

			if (gHero.nObject)
			{
				// On cherche à poser.

				// Pas sur un cadeau !
				if (gRoom.nPresent == -1 ||
					(gRoom.nPresentPosX == i && gRoom.nPresentPosY == j && gRoom.nPresentPosZ == nPosZ) == 0)

				if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * nPosZ) + (ROOM_MAX_X * j) + i] == 0 &&
					(nPosZ == ROOM_MAX_Z - 1 || gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ + 1)) + (ROOM_MAX_X * j) + i] != 0))
				{

					// Spellbook ramené en salle initiale ?
					if (gHero.nObject == BLK_SPELLBOOK &&
						gRoom.nRoomNo == PUMP_START_ROOM && gGame.nQuest == 0)
					{
						TXT_INIT(e_Txt_Spellbook)
						//
						gHero.nObject = 0;		// Le livre disparaît.
						gGame.nQuest = 1;		// Quête.
						gHero.nObjTime = gHero.nTime;	// Temps du Spellbook.
					}

					gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * nPosZ) + (ROOM_MAX_X * j) + i] = gHero.nObject;
					gHero.nObject = 0;
					Room_HeroCurObjectDisplay(gHero.nObject);
					// Sfx.
					Sfx_StopSfx(e_Sfx_Game_ObjectGet);
					Sfx_PlaySfx(e_Sfx_Game_ObjectDrop, 10);
					return;
				}
			}
			else
			{
				// On cherche à prendre.
				u32	nBlk;

				nBlk = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * nPosZ) + (ROOM_MAX_X * j) + i];
				if (Room_BlockIsPortable(nBlk))
				{
					// On est tout en haut || au dessus c'est vide || au dessus c'est pas portable (donc un mur).
					if (nPosZ == 0 ||
						gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ - 1)) + (ROOM_MAX_X * j) + i] == 0 ||
						Room_BlockIsPortable(gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (nPosZ - 1)) + (ROOM_MAX_X * j) + i]) == 0)
					{
						gHero.nObject = nBlk;
						gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * nPosZ) + (ROOM_MAX_X * j) + i] = 0;
						Room_HeroCurObjectDisplay(gHero.nObject);
						// Sfx.
						Sfx_StopSfx(e_Sfx_Game_ObjectDrop);
						Sfx_PlaySfx(e_Sfx_Game_ObjectGet, 10);
						return;
					}
				}
			}

		}
	}

}

// Teste si le héros ramasse un cadeau.
void Hero_PresentCheck(void)
{
	if (gRoom.nPresent < 0) return;

	s32	nHeroX1, nHeroX2, nHeroY1, nHeroY2, nHeroZ1, nHeroZ2;

	nHeroX1 = gHero.nPosX >> 12;
	nHeroX2 = (gHero.nPosX + ((MAP_TILEW - 1) << 8)) >> 12;
	nHeroY1 = gHero.nPosY >> 12;
	nHeroY2 = (gHero.nPosY + ((MAP_TILEH - 1) << 8)) >> 12;
	nHeroZ1 = gHero.nPosZ >> 12;
	nHeroZ2 = (gHero.nPosZ + ((MAP_TILEZ - 1) << 8)) >> 12;

	if (gRoom.nPresentPosX >= nHeroX1 &&  gRoom.nPresentPosX <= nHeroX2 &&
		gRoom.nPresentPosY >= nHeroY1 &&  gRoom.nPresentPosY <= nHeroY2 &&
		gRoom.nPresentPosZ >= nHeroZ1 &&  gRoom.nPresentPosZ <= nHeroZ2)
	{
/*
		// Score.
		static	u8	pScores[] = { 25,50,  50,100,  31,31,  75,75 };		// Vert/Bleu/Rouge/Lingot. 2 valeurs pour chaque.
		gHero.nScore += pScores[(gRoom.nPresent * 2) + (gnFrame & 1)];
*/
		// Timer stop.
		gHero.nTimeStop += GAME_TIMERSTOP;	// Timer stop !
		if (gHero.nTimeStop > GAME_TIMERSTOP_MAX) gHero.nTimeStop = GAME_TIMERSTOP_MAX;
		// Sfx.
		Sfx_PlaySfx(e_Sfx_Game_Present, 10);
		//
		gRoom.nPresent = -1;
	}

}


// Gestion de la mort.
// Out: 0 = Pas mort / 1 = Mort.
u32 Hero_DeathManage(void)
{
	if (AnmGetKey(gHero.nAnm) != e_AnmKey_Hero_Death) return (0);
	if (AnmCheckEnd(gHero.nAnm) == 0) return (1);

	if (gHero.nLives)
	{	// Il reste des vies.
		gHero.nLives--;
		gGame.nPhase = e_Game_RoomTransitOff;
		gGame.nRoomNextNo = gRoom.nPumpRoom;
		gGame.nRoomNextDir = -1;
	}
	else
	{	// Mort.
		// Position pour le crucifix à la prochaine partie.
		gGame.nLastDeathRoom = gRoom.nRoomNo;
		gGame.nLastDeathBlkX = gHero.nPosX >> 12;
		gGame.nLastDeathBlkY = gHero.nPosY >> 12;
		gGame.nLastDeathBlkZ = gHero.nPosZ >> 12;
		//
		TXT_INIT(e_Txt_Death)
	}

	return (1);
}



#define	HERO_SPD	(0x200)
#define	HERO_JUMP_DST_MAX	(3 << 12)
#define	HERO_JUMP	(-0x330)
#define	GRAVITY		(-(2 * HERO_JUMP) / 24)
#define	SPDZ_MAX	0x200
#define	HERO_FALL_MAX_HEIGHT	(3 << 12)


// Table des anims du héros.
enum
{
	e_HeroAnm_Idle = 0,
	e_HeroAnm_Walk,
	e_HeroAnm_Fall,
	e_HeroAnm_Land,

	e_HeroAnm_Death,

	e_HeroAnm_MAX
};
u64 *gpHeroAnm[e_HeroAnm_MAX] =
{
	gAnm_Hero_Idle,
	gAnm_Hero_Walk,
	gAnm_Hero_Fall,
	gAnm_Hero_Land,

	gAnm_Hero_Death,

};


// Init d'une vie.
void Hero_LifeInit(void)
{
	AnmSet(gpHeroAnm[e_HeroAnm_Idle], gHero.nAnm);		// Force l'anim d'attente.

	// Apparition sur le dernier gonfleur touché.
//	gRoom.nPumpRoom = 0;
	gHero.nPosX = (u32)gRoom.nPumpX << 12;
	gHero.nPosY = (u32)gRoom.nPumpY << 12;
	gHero.nPosZ = (u32)(gRoom.nPumpZ - 1) << 12;
	gHero.nJumpPosZOrg = gHero.nPosZ;
	gHero.nGnd = 1;

	gHero.nSpdZ = 0;
	gHero.nSpd = 0;

	gHero.nJumpKb = 0;
	gHero.nKbSav = 0;
	gHero.nJumpAcc = 0;

	gHero.nAirLevel = 4 << 8;		// Un peu d'air pour placement sur la pompe.

	gHero.nTimeStop = 0;			// Coupe le timer stop en cas de mort.

}


// Déplacement du héros.
void Hero_Move(void)
{
	u32	nAnm = e_HeroAnm_Idle;	// Anm par défaut.
	u32	nAnmKey = AnmGetKey(gHero.nAnm);
	u8	nKb;

	// Gestion de la mort.
	if (Hero_DeathManage()) return;

	nKb = 0;
	if (gHero.nKbSav)
	{	// Cas pourri du saut juste devant la porte, on restaure le kb précédent dès qu'on arrive dans la salle.
		nKb = gHero.nKbSav;
		gHero.nKbSav = 0;
	}
	else
	if (nAnmKey == e_AnmKey_Hero_Idle)
	{
		// Cas normal.
		gHero.nJumpKb = 0;

		if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_Up]]) nKb |= 8;
		if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_Right]]) nKb |= 1;
		if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_Down]]) nKb |= 2;
		if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_Left]]) nKb |= 4;
		if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonA]]) nKb |= 0x10;	// Jump.
//		if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]]) nKb |= 0x20;	// Prendre/poser un objet.
	}
	else
	if (nAnmKey == e_AnmKey_Hero_Fall)
	{
		// Saut.
		if (gHero.nJumpAcc < HERO_JUMP_DST_MAX)	// Distance maxi pas encore atteinte. (Ca divise bien !).
		{
			gHero.nSpd = HERO_SPD;			// Vitesse max dès qu'on saute.
			gHero.nJumpAcc += gHero.nSpd;
		}
		else
		{
			gHero.nSpd = 0;
			gHero.nJumpKb = 0;
		}
		nKb = gHero.nJumpKb;
	}



	// On élimine les cas Haut-Bas et Droite-Gauche.
	if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_Up]] && gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_Down]]) nKb &= ~(1 | 4);
	if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_Right]] && gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_Left]]) nKb &= ~(2 | 8);
	//
	s32	nSpdX, nSpdY;
	if (nKb & 0x0F)
	{
		gHero.nSpd = HERO_SPD;

		nSpdX = nSpdY = 0;
		if (nKb & 1) nSpdY = -HERO_SPD;
		if (nKb & 2) nSpdX = HERO_SPD;
		if (nKb & 4) nSpdY = HERO_SPD;
		if (nKb & 8) nSpdX = -HERO_SPD;

	}
	else
	{
		nSpdX = nSpdY = 0;
		gHero.nSpd = 0;
	}


	// Sortie ?
	if (Hero_ExitCheck(nSpdX, nSpdY))
	{
		gHero.nKbSav = (nKb & 0x10 ? nKb : 0);	// Si bouton de saut, on sauvegarde le kb.
		return;
	}


	u32	nRet;
	u32 nColVal;

	nColVal = 0;

	// Saut.
	if ((nKb & 0x10) && gHero.nGnd)
	{
		gHero.nJumpKb = nKb & 0x0F;
		gHero.nJumpAcc = HERO_SPD;	// SPD et pas 0, compense la première avance de cette frame ! (Qu'on avance ou non est sans importance, c'est juste un accumulateur).

		//AnmSetIfNew(gpHeroAnm[e_HeroAnm_Fall], gHero.nAnm);	// Normalement inutile.

		gHero.nSpdZ = HERO_JUMP;
		gHero.nGnd = 0;
		gHero.nJumpPosZOrg = gHero.nPosZ;	// Z original au moment du départ du saut.
	}
	// Limite salle Z.
	if (gHero.nSpdZ < 0 && gHero.nPosZ + gHero.nSpdZ < 0)
	{
		gHero.nPosZ = 0;
		gHero.nSpdZ = 0;
	}
	// Plafond ?
	if (gHero.nSpdZ < 0)
	if ((nRet = Hero_CheckMoveZ(gHero.nPosZ + gHero.nSpdZ, -1)) != 0)
	{
		nColVal |= nRet;
		gHero.nSpdZ = 0;
	}

	// Gravité.
	gHero.nSpdZ += GRAVITY;
	if (gHero.nSpdZ > SPDZ_MAX) gHero.nSpdZ = SPDZ_MAX;
	gHero.nPosZ += gHero.nSpdZ;

	// Test des pieds.
	u8	nLastGnd = gHero.nGnd;
	s32 nLastJumpPosZOrg = gHero.nJumpPosZOrg;
	gHero.nGnd = 0;
	if ((nRet = Hero_CheckMoveZ(gHero.nPosZ + (MAP_TILEZ << 8), 1)) != 0)
	{
		nColVal |= nRet;
		gHero.nPosZ = (gHero.nPosZ & ~0xFFF);
		gHero.nSpdZ = 0;
		gHero.nGnd = 1;
		gHero.nJumpPosZOrg = gHero.nPosZ;	// Z original pour chute.
	}


	// Tombé de trop haut ?
	if (nLastGnd == 0 && gHero.nGnd)
	if (gHero.nPosZ - nLastJumpPosZOrg >= HERO_FALL_MAX_HEIGHT)
	{
		// Death.
		AnmSet(gpHeroAnm[e_HeroAnm_Death], gHero.nAnm);
		return;
	}


	// On fait tomber le héros dès qu'on est dans le vide.
	if (nLastGnd && gHero.nGnd == 0)
	{
		nSpdX = nSpdY = 0;
		// Pour accélérer un peu le début de la chute.
		gHero.nSpdZ = 0x100;
		gHero.nPosZ += gHero.nSpdZ;
	}


	// Déplacement x y.
	if (nSpdX)
	if ((nRet = Hero_CheckMoveX(nSpdX)) != 0)
	{
		nColVal |= nRet;
		nSpdX = 0;
	}
	gHero.nPosX += nSpdX;

	if (nSpdY)
	if ((nRet = Hero_CheckMoveY(nSpdY)) != 0)
	{
		nColVal |= nRet;
		nSpdY = 0;
	}
	gHero.nPosY += nSpdY;

	if (nSpdX || nSpdY)
	if (nAnmKey == e_AnmKey_Hero_Idle)
		nAnm = e_HeroAnm_Walk;


	// Collision qui pique ?
	if (nColVal & COLL_SPIKE)
	{
		// Death.
		AnmSet(gpHeroAnm[e_HeroAnm_Death], gHero.nAnm);
		return;
	}

	// Air level.
	gHero.nAirLevel -= 0x05;	// Semble assez exact avec 0x05. Val précédente : 0x04.
	if (gHero.nAirLevel <= 0x05)	// Oui parce que bon.
	{
		// Explose !
		gHero.nAirLevel = 0;
		AnmSet(gpHeroAnm[e_HeroAnm_Death], gHero.nAnm);
		return;
	}

	// Pompe ?
	if (nColVal & COLL_PUMP)
	{
		// Gonflage.
		gHero.nAirLevel += 0x100;
		if (gHero.nAirLevel > HERO_AIR_MAX << 8)
		{
			// Explose !
			gHero.nAirLevel = HERO_AIR_MAX << 8;
			AnmSet(gpHeroAnm[e_HeroAnm_Death], gHero.nAnm);
			return;
		}
	}


	// Anim de chute ?
	if (gHero.nGnd == 0) nAnm = e_HeroAnm_Fall;

	// Atterrissage ?
	if (nLastGnd == 0 && gHero.nGnd == 1) nAnm = e_HeroAnm_Land;

	// Anim.
	AnmSetIfNew(gpHeroAnm[nAnm], gHero.nAnm);

	// Test de ramassage de cadeau.
	Hero_PresentCheck();

	// Prendre/poser un objet.
	if (gHero.nGnd == 1)
	if (gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]])//if (nKb & 0x20)
	{
		Hero_GetDrop();
		gVar.pKeys[gAirballCfg.pKeys[e_CfgKey_ButtonB]] = 0;

		// Tous les objets ont été ramenés en salle 0 ?
		if (gRoom.nRoomNo == PUMP_START_ROOM)
		if (gGame.nQuest == 1)
//non		if (gHero.nObject == 0)		// Seulement si on vient de poser, pas au ramassage.
		if (Room_AllObjTst())
		{
			TXT_INIT(e_Txt_Win)
		}
	}

}



