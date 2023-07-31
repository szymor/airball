
#include "includes.h"

struct SRoomData	*gpRoomsOrg;	// Pièces originales, qu'on recopie à chaque début de partie.
u32	gnRoomDataSz;					// Taille des datas à copier.

struct SRoomData	*gpRooms;		// Pièces dans lesquelles on joue.
struct SRoom	gRoom;

//=====================================

// Les blocs qui font mal.

enum
{
	e_Pain_North	= (1 << 0),
	e_Pain_East		= (1 << 1),
	e_Pain_South	= (1 << 2),
	e_Pain_West		= (1 << 3),
	e_Pain_Top		= (1 << 4),
	e_Pain_Bottom	= (1 << 5),

	e_Pain_All		= e_Pain_North | e_Pain_East | e_Pain_South | e_Pain_West | e_Pain_Top | e_Pain_Bottom,
	e_Pain_Sides	= e_Pain_North | e_Pain_East | e_Pain_South | e_Pain_West,

};

u8	gpnPainfulBlocks[] = {
0,0,0,0, 0,0,0,0, 0,0,e_Pain_Top,e_Pain_Top, e_Pain_Top,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
e_Pain_Top,e_Pain_All,e_Pain_All,0, 0,0,0,0, 0,0,0,e_Pain_Sides, e_Pain_All,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
e_Pain_Top,0,e_Pain_Top,e_Pain_Top, e_Pain_Top,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,e_Pain_Sides|e_Pain_Top, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
};

// Renvoie si un bloc est piquant.
u32 Room_BlockIsSpiky(u32 nBlkNo, u32 nDir)
{
	return (gpnPainfulBlocks[nBlkNo] & nDir);
}

//=====================================

// Les blocs qu'on peut porter.

// 1 bit.
u8	gpnPortableBlocks[] = {
0, 0,
0, 0,
(1<<3)|(1<<4), (1<<5)|(1<<6),
0, 0,

0, 0,
0, (1<<4)|(1<<5),
0, 0,
0, 0,

0, 0,
0, (1<<5),
0, 0,
(1<<2)|(1<<5), (1<<7),

0, (1<<0),
0, 0,
0, 0,
0, 0,
};

// Renvoie si un bloc est portable ou non.
u32 Room_BlockIsPortable(u32 nBlkNo)
{
	u32	nOffset;
	u8	nMask;

	nOffset = nBlkNo >> 3;
	nMask = 1 << (nBlkNo & 7);

	return (gpnPortableBlocks[nOffset] & nMask);
}

//=====================================

// Les cadeaux.

// Blocs sur lesquels on peut poser un cadeau :
// 1>9 13>15 47 49 53 59 85 90>91 94>95 108>112 142>144 215 245>246

// 1 bit.
u8	gpnDropableBlocks[] = {
(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7), (1<<0)|(1<<1)|(1<<5)|(1<<6)|(1<<7),	// 1>7 / 8>9 13>15
0, 0,
0, (1<<7),	// / 47
(1<<1)|(1<<5), (1<<3),	// 49 53 / 59

0, 0,
(1<<5), (1<<2)|(1<<3)|(1<<6)|(1<<7),	// 85 / 90>91 94>95
0, (1<<4)|(1<<5)|(1<<6)|(1<<7),	// / 108>111
(1<<0), 0,	// 112

0, (1<<6)|(1<<7),	// / 142>143
(1<<0), 0,	// 144
0, 0,
0, 0,

0, 0,
(1<<7), 0,	// 215
0, 0,
(1<<5)|(1<<6), 0,	// 245>246
};

/*
// Test des n° de blocs : Ok !
void tstblk(void)
{
	u32	i;
	for (i = 0; i < 256; i++)
		printf("blk %d - %s\n", i, gpnDropableBlocks[i >> 3] & (1 << (i & 7)) ? "YES" : "no");
}
*/

// "Reprogrammation" du bloc 255 avec le cadeau qui va bien.
void Room_PresentBlkReprogram(void)
{
	SDL_Rect	sSrc, sDst;
	sSrc.x = gRoom.nPresent * GFX_BLK_W;
	sSrc.y = 0;
	sSrc.w = GFX_BLK_W;
	sSrc.h = GFX_BLK_H;
	sDst.x = (255 & 15) * GFX_BLK_W;
	sDst.y = (255 >> 4) * GFX_BLK_H;
	sDst.w = GFX_BLK_W;
	sDst.h = GFX_BLK_H;
	SDL_BlitSurface(gVar.pTreasures, &sSrc, gVar.pBlk, &sDst);
}

// Initialisation d'un cadeau.
void Room_PresentInit(void)
{
	u32	x, y, z;
	u32	nBlk;

	gRoom.nPresent = -1;

	static	u8	nCnt = 0;
//	if (++nCnt & 3) return;
	if (++nCnt < 3) return;
	nCnt = 0;

	// Position du cadeau.
	x = rand();
	y = (x >> 8) & 7;
	x &= 7;

	// Peut-on poser le cadeau ici ?
	for (z = ROOM_MAX_Z - 2; z >= 0; z--)
		if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (ROOM_MAX_X * y) + x] == 0)
		{
			nBlk = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (z + 1)) + (ROOM_MAX_X * y) + x];
			if (gpnDropableBlocks[nBlk >> 3] & (1 << (nBlk & 7))) break;	// Ok.
			return;		// Ca ne va pas. Tant pis.
		}

	// Ok !
	gRoom.nPresentPosX = x;
	gRoom.nPresentPosY = y;
	gRoom.nPresentPosZ = z;
	//gRoom.nPresent = (x + y + z) & 3;
	static	u8	pPresent[] = { 0,0, 0,1, 0,2, 0,3 };		// Vert/Bleu/Rouge/Lingot. 2 valeurs pour chaque.
	gRoom.nPresent = pPresent[gnFrame & 7];

	Room_PresentBlkReprogram();		// "Reprogrammation" du bloc 255 avec le cadeau qui va bien.

}

//=====================================

// Palettes.

u8 gpnPalettes[] =
{
0x00, 0x00, 0x00,
0x60, 0x00, 0x80,
0xA0, 0x00, 0xE0,
0x20, 0x20, 0x60,
0x60, 0x60, 0x80,
0x80, 0x80, 0xA0,
0xA0, 0xA0, 0xC0,
0xC0, 0xC0, 0xE0,
0x00, 0xC0, 0xC0,
0x00, 0x80, 0x00,
0xA0, 0xC0, 0x60,
0xE0, 0xE0, 0x60,
0xE0, 0xE0, 0xA0,
0x80, 0x80, 0x60,
0xE0, 0x60, 0x40,
0x80, 0x00, 0x00,

/*
0x00, 0x00, 0x00,
0x00, 0x00, 0x20,
0x00, 0x00, 0x20,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x20,
0x00, 0x00, 0x00,
0x00, 0x00, 0x20,
0x00, 0x20, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x20, 0x00, 0x20,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
*/
0x00, 0x00, 0x00,
0x00, 0x00, 0x20+0x20,
0x00, 0x00, 0x20+0x20,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x20+0x20,
0x00, 0x00, 0x00,
0x00, 0x00, 0x20+0x20,
0x00, 0x20+0x20, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,
0x20+0x20, 0x00, 0x20+0x20,
0x00, 0x00, 0x00,
0x00, 0x00, 0x00,

0x00, 0x00, 0x00,
0x40, 0x00, 0x40,
0x60, 0x00, 0x40,
0x80, 0x40, 0x20,
0x60, 0x20, 0x20,
0xA0, 0x60, 0x20,
0xA0, 0x80, 0x00,
0x60, 0x40, 0x20,
0x80, 0x60, 0x60,
0x80, 0x60, 0x00,
0xC0, 0xA0, 0x20,
0x60, 0x00, 0x40,
0x60, 0x40, 0x40,
0xC0, 0x80, 0x00,
0xA0, 0x20, 0x20,
0x60, 0x00, 0x00,

0x00, 0x00, 0x40,
0x80, 0x00, 0xE0,
0xA0, 0x00, 0x00,
0x60, 0x60, 0x80,
0x80, 0xA0, 0xA0,
0xA0, 0xA0, 0xE0,
0xC0, 0xE0, 0xE0,
0xC0, 0xE0, 0xE0,
0x20, 0xE0, 0xE0,
0x40, 0xA0, 0x00,
0xE0, 0x20, 0xE0,
0xE0, 0xE0, 0x80,
0xE0, 0xE0, 0xC0,
0x80, 0xC0, 0x80,
0xC0, 0x20, 0x40,
0x80, 0x20, 0x20,
};

// Met la palette de la salle en cours dans la palette physique pour l'écran.
/*
void Room_PalSetNo(u32 nPalNo)
{
	u32	i;

	for (i = 0; i < 16; i++)
	{
		gVar.pColorsPhy[i].r = gpnPalettes[(nPalNo * 3 * 16) + (i * 3) + 0];
		gVar.pColorsPhy[i].g = gpnPalettes[(nPalNo * 3 * 16) + (i * 3) + 1];
		gVar.pColorsPhy[i].b = gpnPalettes[(nPalNo * 3 * 16) + (i * 3) + 2];
	}
//	Fade(256);					// Force la palette physique.

}
*/
void Room_PalSetNo(u32 nPalNo, u32 nFadeVal)
{
	u32	i;

	gRender.nPalFlip = 1;	// Flag pour la boucle principale.

	// Les couleurs de la salle.
	if (nFadeVal > 255) nFadeVal = 255;
	for (i = 0; i < 16; i++)
	{
		gVar.pColorsPhy[i].r = (gpnPalettes[(nPalNo * 3 * 16) + (i * 3) + 0] * nFadeVal) / 255;
		gVar.pColorsPhy[i].g = (gpnPalettes[(nPalNo * 3 * 16) + (i * 3) + 1] * nFadeVal) / 255;
		gVar.pColorsPhy[i].b = (gpnPalettes[(nPalNo * 3 * 16) + (i * 3) + 2] * nFadeVal) / 255;
	}
	if (gGame.nFadeAll == 0) return;
	// Le reste si nécessaire.
	for (i = 16; i < 256; i++)
	{
		gVar.pColorsPhy[i].r = (gVar.pColorsLog[i].r * nFadeVal) / 255;
		gVar.pColorsPhy[i].g = (gVar.pColorsLog[i].g * nFadeVal) / 255;
		gVar.pColorsPhy[i].b = (gVar.pColorsLog[i].b * nFadeVal) / 255;
	}

}
// Fader.
void FadeColors(u32 nFadeVal, u32 nNbColors)
{
	u32	i;

	gRender.nPalFlip = 1;	// Flag pour la boucle principale.

	if (nFadeVal > 255) nFadeVal = 255;
	for (i = 0; i < nNbColors; i++)
	{
		gVar.pColorsPhy[i].r = (gVar.pColorsLog[i].r * nFadeVal) / 255;
		gVar.pColorsPhy[i].g = (gVar.pColorsLog[i].g * nFadeVal) / 255;
		gVar.pColorsPhy[i].b = (gVar.pColorsLog[i].b * nFadeVal) / 255;
	}

}

//=====================================

// Affiche l'objet en cours (incrusté dans gRoom.pBkg).
void Room_HeroCurObjectDisplay(u32 nObject)
{
	s32	x, y;
	SDL_Rect	sSrc, sDst;

	sDst.x = 0;
	sDst.y = SCR_Height - GFX_BLK_H - 0;
	sDst.w = GFX_BLK_W;
	sDst.h = GFX_BLK_H;

	if (nObject)
	{
		// L'objet porté par le joueur.
		sSrc.x = (gHero.nObject & 15) * GFX_BLK_W;
		sSrc.y = (gHero.nObject >> 4) * GFX_BLK_H;
		sSrc.w = GFX_BLK_W;
		sSrc.h = GFX_BLK_H;
		SDL_BlitSurface(gVar.pBlk, &sSrc, gRoom.pBkg, &sDst);

		// Add 32 pour passer en palette qui ne fade pas.
		SDL_LockSurface(gRoom.pBkg);
		u8	*pScr;
		pScr = gRoom.pBkg->pixels;
		pScr += (gRoom.pBkg->pitch * sDst.y) + sDst.x;
		for (y = 0; y < GFX_BLK_H; y++)
		{
			for (x = 0; x < GFX_BLK_W; x++)
			{
				if (*(pScr + x)) *(pScr + x) += 32;
			}
			pScr += gRoom.pBkg->pitch;
		}
		SDL_UnlockSurface(gRoom.pBkg);

	}
	else
	{
		// Vide.
		SDL_FillRect(gRoom.pBkg, &sDst, 0);
	}

}


// Teste si tous les objets ont été ramenés.
// Fonction appellée uniquement en salle 0 !
u32 Room_AllObjTst(void)
{
	static	u8 pObj[] = { BLK_CRUCIFIX, BLK_BUDDHA, BLK_FLASK, BLK_PUMPKIN, BLK_DRAGON, BLK_BEANS };
//	static	u8 pObj[] = { BLK_CRUCIFIX, BLK_TORCH, 35 };//, BLK_CANDLE };
	u8	nMsk;

	u32	x, y, z, k;
	u32	nBlk;

	u8	nLastObjFound = gHero.nObjFound;
	gHero.nObjFound = 0;	// Appel uniquement en salle 0, donc pas de problème.
	nMsk = 0;
	for (z = 0; z < ROOM_MAX_Z - 1; z++)
	for (y = 0; y < gRoom.pRoomData->nDimY; y++)
	for (x = 0; x < gRoom.pRoomData->nDimX; x++)
	{
		nBlk = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (y * ROOM_MAX_X) + x];
		if (nBlk)
		{
			for (k = 0; k < NBELEM(pObj); k++)
				if ((nMsk & (1 << k)) == 0)		// Ne compte qu'une item de chaque sorte.
				if (nBlk == pObj[k])
				{
					gHero.nObjFound++;
					nMsk |= (1 << k);
//					if (nMsk == (1 << NBELEM(pObj)) - 1) return (1);	// Tout trouvé !
					if (nMsk == (1 << NBELEM(pObj)) - 1) { gHero.nObjTime = gHero.nTime; gHero.nObjFound |= 0x80; return (1); }	// Tout trouvé !
					break;
				}
		}
	}
	// Update du time, si nécessaire.
	if (gHero.nObjFound > nLastObjFound) gHero.nObjTime = gHero.nTime;

	return (0);
}

// Init pièce.
void Room_Init(u32 nRoomNo, s32 nDir)
{
	s32	x, y, z, i;

	gRoom.nRoomNo = nRoomNo;
	gRoom.pRoomData = &gpRooms[nRoomNo];	// Pointeur sur la pièce en cours.

	// Position du joueur.
	if (nDir == -2)
	{
		// Load slot.
	}
	else
	if (nDir == -1)
	{
		Hero_LifeInit();	// (Re)apparition sur un gonfleur.
	}
	else
	{
		// Recherche du bloc de porte qui va bien.
		u32	nExitBlk;
		u32	nOffset;
		if (nDir & 1)
		{
			// DG : On cherche sur les colonnes 0 et DimX-1.
			nExitBlk = EXIT_BLK_X0;
			nOffset = (nDir == 1 ? gRoom.pRoomData->nDimX - 1 : 0);

			for (z = ROOM_MAX_Z - 2; z >= 0; z--)
			for (i = 0; i < gRoom.pRoomData->nDimY; i++)
				if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (i * ROOM_MAX_X) + nOffset] == nExitBlk)
				{
					gHero.nPosX = (nDir == 1 ? nOffset - 1 : 1) << 12;
					gHero.nPosY = (i << 12) - ((MAP_TILEH / 2) << 8);
//					gHero.nPosZ = z << 12;
					gHero.nPosZ = (gHero.nPosZ & 0xF00) | (z << 12);
					gHero.nPosZ -= (gHero.nPosZ & 0xF00 ? 1 << 12 : 0);
					gHero.nJumpPosZOrg = z << 12;
					// Si objet devant la porte...
					if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (i * ROOM_MAX_X) + (gHero.nPosX >> 12)] != 0)
						gHero.nPosY &= ~0xFFF;
					else
					if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + ((i - 1) * ROOM_MAX_X) + (gHero.nPosX >> 12)] != 0)
						gHero.nPosY = i << 12;
					goto _searchstop;
				}
		}
		else
		{
			// HB : On cherche sur les lignes 0 et DimY-1.
			nExitBlk = EXIT_BLK_Y0;
			nOffset = (nDir == 2 ? gRoom.pRoomData->nDimY - 1 : 0);

			for (z = ROOM_MAX_Z - 2; z >= 0; z--)
			for (i = 0; i < gRoom.pRoomData->nDimX; i++)
				if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (nOffset * ROOM_MAX_X) + i] == nExitBlk)
				{
					gHero.nPosX = (i << 12) + ((MAP_TILEW / 2) << 8);
					gHero.nPosY = (nDir == 2 ? nOffset - 1 : 1) << 12;
//					gHero.nPosZ = z << 12;
					gHero.nPosZ = (gHero.nPosZ & 0xF00) | (z << 12);
					gHero.nPosZ -= (gHero.nPosZ & 0xF00 ? 1 << 12 : 0);
					gHero.nJumpPosZOrg = z << 12;
					// Si objet devant la porte...
					if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + ((gHero.nPosY >> 12) * ROOM_MAX_X) + i + 1] != 0)
						gHero.nPosX &= ~0xFFF;
					else
					if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + ((gHero.nPosY >> 12) * ROOM_MAX_X) + i] != 0)
						gHero.nPosX = (i + 1) << 12;
					goto _searchstop;
				}
		}

	}
_searchstop:
	AnmReset(gHero.nAnm);	// Reset de l'anim en cours du héros. (Pour pb de l'anim qui change quand on s'arrête après un chgt de salle).

	// Calcul de l'excentrage de la pièce en cours.
	gRoom.nDispOffsX = (SCR_Width / 2) + ((ROOM_MAX_X - gRoom.pRoomData->nDimX) * 8) - ((ROOM_MAX_Y - gRoom.pRoomData->nDimY) * 8);
	gRoom.nDispOffsY = (SCR_Height / 2) - ((ROOM_MAX_Z + 1) * MAP_TILEZ) - 4 + 12;	// 320 x 240.

	// On place le sol et les murs dans un buffer, on copiera ce buffer comme fond d'écran dans le jeu.
	SDL_FillRect(gRoom.pBkg, NULL, 0);

	SDL_Rect	sSrc, sDst;
	s32	nScrX, nScrY;

	// Affichage des murs.
	// Partie gauche.
	sSrc.x = 64 * gRoom.pRoomData->nWallType;
	sSrc.y = 0;
	sSrc.w = 16;
	sSrc.h = 128+32;
	ISO_ScreenPosition(0 * MAP_TILEW, (gRoom.pRoomData->nDimY - 1) * MAP_TILEH, (ROOM_MAX_Z - 1) * MAP_TILEZ, &nScrX, &nScrY);
	sDst.x = nScrX - (GFX_BLK_W / 2);
	sDst.y = nScrY + 8 - (128+32);
	SDL_BlitSurface(gVar.pWalls, &sSrc, gRoom.pBkg, &sDst);
	// Partie centrale.
	sSrc.x = (64 * gRoom.pRoomData->nWallType) + 16;
	sSrc.y = 0;
	sSrc.w = 32;
	sSrc.h = 128+32;
	ISO_ScreenPosition(0 * MAP_TILEW, 0 * MAP_TILEH, (ROOM_MAX_Z - 1) * MAP_TILEZ, &nScrX, &nScrY);
	sDst.x = nScrX - (GFX_BLK_W / 2);
	sDst.y = nScrY + 8 - (64+32)-8;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gRoom.pBkg, &sDst);
	// Partie droite.
	sSrc.x = (64 * gRoom.pRoomData->nWallType) + (16 * 3);
	sSrc.y = 0;
	sSrc.w = 16;
	sSrc.h = 128+32;
	ISO_ScreenPosition((gRoom.pRoomData->nDimX - 1) * MAP_TILEW, 0 * MAP_TILEH, (ROOM_MAX_Z - 1) * MAP_TILEZ, &nScrX, &nScrY);
	sDst.x = nScrX;// - (GFX_BLK_W / 2);
	sDst.y = nScrY + 8 - (128+32);
	SDL_BlitSurface(gVar.pWalls, &sSrc, gRoom.pBkg, &sDst);

	// On dessine un carré noir derrière les portes côtés murs.
	// gauche blk 22 > EXIT_BLK_X1 / pos -12, -5, sz 12 12, z+1.
	for (z = ROOM_MAX_Z - 2; z >= 0; z--)
	for (i = 0; i < gRoom.pRoomData->nDimY; i++)
		if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (i * ROOM_MAX_X)] == EXIT_BLK_X1)
		{
			ISO_ScreenPosition(0, i * MAP_TILEH, (z + 1) * MAP_TILEZ, &nScrX, &nScrY);
			sDst.x = nScrX - 12;
			sDst.y = nScrY - 5;
			sDst.w = 12;
			sDst.h = 12;
			SDL_FillRect(gRoom.pBkg, &sDst, 0);
			goto _doorbkgxstop;
		}
_doorbkgxstop:
	// droite blk 18 > EXIT_BLK_Y1 / pos -14, -14 sz 14 12, z+1.
	for (z = ROOM_MAX_Z - 2; z >= 0; z--)
	for (i = 0; i < gRoom.pRoomData->nDimX; i++)
		if (gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + i] == EXIT_BLK_Y1)
		{
			ISO_ScreenPosition(i * MAP_TILEW, 0, (z + 1) * MAP_TILEZ, &nScrX, &nScrY);
			sDst.x = nScrX - 14;
			sDst.y = nScrY - 14;
			sDst.w = 14;
			sDst.h = 12;
			SDL_FillRect(gRoom.pBkg, &sDst, 0);
			goto _doorbkgystop;
		}
_doorbkgystop:
	;

	// Affichage du sol, niveau 0.
	u32	nBlk;
	z = ROOM_MAX_Z - 1;
	for (y = 0; y < gRoom.pRoomData->nDimY; y++)
	for (x = 0; x < gRoom.pRoomData->nDimX; x++)
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
			SDL_BlitSurface(gVar.pBlk, &sSrc, gRoom.pBkg, &sDst);
		}
	}

	// Patch Room 152.
	if (nRoomNo == 152)
	{
		static u8	pBlk152[] = { 32, 34, 34, 32 };
		static u8	pX152[] = { 0, 0, 0, 0 };
		static u8	pY152[] = { 2, 3, 4, 5 };
		static u8	pZ152[] = { ROOM_MAX_Z-2, ROOM_MAX_Z-2, ROOM_MAX_Z-2, ROOM_MAX_Z-2 };

		for (i = 0; i < 4; i++)
		{
			nBlk = pBlk152[i];
			sSrc.x = (nBlk & 15) * GFX_BLK_W;
			sSrc.y = (nBlk >> 4) * GFX_BLK_H;
			sSrc.w = GFX_BLK_W;
			sSrc.h = GFX_BLK_H;
			ISO_ScreenPosition(pX152[i] * MAP_TILEW, pY152[i] * MAP_TILEH, pZ152[i] * MAP_TILEZ, &nScrX, &nScrY);
			sDst.x = nScrX - (GFX_BLK_W / 2);
			sDst.y = nScrY - (GFX_BLK_H - 32);
			SDL_BlitSurface(gVar.pBlk, &sSrc, gRoom.pBkg, &sDst);
		}
	}

	// Cache du bas des blocs de sol.
	sSrc.x = 0;
	sSrc.y = 160;
	sSrc.w = 256;
	sSrc.h = 82;
	ISO_ScreenPosition(gRoom.pRoomData->nDimX * MAP_TILEW, gRoom.pRoomData->nDimY * MAP_TILEH, (ROOM_MAX_Z - 1) * MAP_TILEZ, &nScrX, &nScrY);
	sDst.x = nScrX - 128;
	sDst.y = nScrY - 66;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gRoom.pBkg, &sDst);

	// Symbole de la jauge de pression.
	sSrc.x = 24;
	sSrc.y = 354;
	sSrc.w = 24;
	sSrc.h = 24;
	sDst.x = HUD_GAUGE_POSX;
	sDst.y = HUD_GAUGE_POSY;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gRoom.pBkg, &sDst);

	// Fond de la jauge d'air.
	sDst.x = HUD_AIR_POSX;
	sDst.y = HUD_AIR_POSY;
	sDst.w = HERO_AIR_MAX;
	sDst.h = 2;
	SDL_FillRect(gRoom.pBkg, &sDst, 15 +32);

	// Vies.
	for (i = 0; i < gHero.nLives; i++)
	{
		sSrc.x = 0;
		sSrc.y = 354;
		sSrc.w = 24;
		sSrc.h = 24;
//		sDst.x = 32 + (i * 6);	// Pos org.
		sDst.x = HUD_LIVES_POSX + (i * HUD_LIVES_SPC);
		sDst.y = HUD_LIVES_POSY;
		SDL_BlitSurface(gVar.pWalls, &sSrc, gRoom.pBkg, &sDst);
	}

	// "Score".
//	FontPrint16(gRoom.pBkg, gVar.pFont16, HUD_SCORE_POSX, HUD_SCORE_POSYL1, "SCORE", 0);
	// "Time".
//	FontPrint16(gRoom.pBkg, gVar.pFont16, HUD_SCORE_POSX - 8, HUD_SCORE_POSYL1, "TIME", 0);
//	FontPrint16(gRoom.pBkg, gVar.pFont16, HUD_SCORE_POSX + 16, HUD_SCORE_POSYL1, "TIME", 0);


	// Palette.
	u32	nPalNo;
	// De base, salle éclairée ou éteinte.
	nPalNo = (gRoom.pRoomData->nLightDefault ? e_Pal_Normal : e_Pal_Dark);
	// Si noir, on recherche si il y a un objet lumière dans la salle.
	if (nPalNo == e_Pal_Dark)
	{
		static	u8	pnLight[] = { BLK_CANDLE, BLK_TORCH, BLK_LAMPION };
		static	u8	pnPal[] = { e_Pal_Candle, e_Pal_Torch, e_Pal_Candle };	// Sert de prio en même temps.

		for (z = 0; z < ROOM_MAX_Z - 1; z++)
		for (y = 0; y < gRoom.pRoomData->nDimY; y++)
		for (x = 0; x < gRoom.pRoomData->nDimX; x++)
		{
			nBlk = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * z) + (y * ROOM_MAX_X) + x];
			if (nBlk)
				for (i = 0; i < NBELEM(pnLight); i++)
					if (pnLight[i] == nBlk)
						if (pnPal[i] > nPalNo)
						{
							nPalNo = pnPal[i];
							break;
						}
		}

		// L'objet porté est une lumière ?
		if (gHero.nObject)
			for (i = 0; i < NBELEM(pnLight); i++)
				if (pnLight[i] == gHero.nObject)
					if (pnPal[i] > nPalNo)
					{
						nPalNo = pnPal[i];
						break;
					}

	}
	gRoom.nRoomPal = nPalNo;


	// Affichage de l'objet porté par le joueur.
	Room_HeroCurObjectDisplay(gHero.nObject);

	// Init cadeau.
	Room_PresentInit();

}

//=====================================

// Sous-routine, calcul du checksum sur une pièce.
u32 Room_sub_RoomChecksum(u32 *pData, u32 nDWNb)
{
	u32	i;
	u32	nSum = 0;

	for (i = 0; i < nDWNb; i++)
		if (i & 1)
			nSum += *(pData + i);
		else
			nSum ^= *(pData + i);

	return (nSum);
}

#define	FILE_MAGIC_NUMBER	0xCC170206

#define	LOAD(data_adr, data_sz, fprintf_txt) \
		nSz = fread(data_adr, 1, data_sz, fPt); \
		if (nSz != data_sz) \
		{ \
			fprintf(stderr, fprintf_txt); \
			fclose(fPt); \
			exit(1); \
		}

#define	LOADF(data_adr, data_sz, fprintf_txt) \
		nSz = fread(data_adr, 1, data_sz, fPt); \
		if (nSz != data_sz) \
		{ \
			fprintf(stderr, fprintf_txt); \
			free(gpRoomsOrg); \
			fclose(fPt); \
			exit(1); \
		}

// Lecture des niveaux.
void Room_DataLoad(char *pFilename)
{
	FILE	*fPt;
	s32	nSz;
	u32	t, nRoomsNb;

	// fopen.
	if ((fPt = fopen(pFilename, "rb")) == NULL)
	{
		fprintf(stderr, "Room_DataLoad(): fopen error.\n");
		exit(1);
	}
/*
	// Taille du fichier.
	fseek(fPt, 0, SEEK_END);
	nSzTot = ftell(fPt);
	fseek(fPt, 0, SEEK_SET);
*/

	// Magic number.
	LOAD(&t, sizeof(u32), "Room_DataLoad(): read error (header).\n")
	if (t != FILE_MAGIC_NUMBER)
	{
		fclose(fPt);
		fprintf(stderr, "Room_DataLoad(): Magic number not found. Aborted.\n");
		exit(1);
	}
	// Nb de salles.
	LOAD(&nRoomsNb, sizeof(u32), "Room_DataLoad(): read error (header).\n")
	gnRoomDataSz = nRoomsNb * sizeof(struct SRoomData);		// Taille à allouer/lire.

	// Malloc.
	if ((gpRoomsOrg = (struct SRoomData *)malloc(gnRoomDataSz * 2)) == NULL)
	{
		fclose(fPt);
		fprintf(stderr, "Room_DataLoad(): malloc error.\n");
		exit(1);
	}
	gpRooms = (struct SRoomData *) ((u8 *)gpRoomsOrg + gnRoomDataSz);

	// Lecture des datas.
	LOADF(gpRoomsOrg, gnRoomDataSz, "Room_DataLoad(): read error (data).\n")

	// Checksum.
	u32	nChkSumFile, nChkSumData;
	LOADF(&nChkSumFile, sizeof(u32), "Room_DataLoad(): read error (checksum).\n")
	fclose(fPt);

	// Calcul du checksum sur les datas lus.
	nChkSumData = 0;
	for (t = 0; t < nRoomsNb; t++)
		nChkSumData += Room_sub_RoomChecksum((u32 *)&gpRoomsOrg[t], sizeof(struct SRoomData) / 4);

//	printf("Checksum: file=%X / calc=%X\n", nChkSumFile, nChkSumData);

	if (nChkSumFile != nChkSumData)
	{
		fprintf(stderr, "Data has been corrupted. Aborted.\n");
		free(gpRoomsOrg);
		exit(1);
	}

}

// Lecture des niveaux.
void Room_DataFree(void)
{
	free(gpRoomsOrg);	// gpRooms pointe dans cet espace mémoire, pas de free.

}

//=====================================

