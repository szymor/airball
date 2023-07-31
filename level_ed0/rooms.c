
#include "includes.h"

struct SRoom	gRoom;

struct SRoomData	**gppRooms;		// Pointeur sur les pièces.
u32	gnRoomsNb;						// Nb de pièces.

//=====================================

// Libère tout.
void Room_FreeAll(void)
{
	if (gnRoomsNb == 0) return;

	u32	i;
	for (i = 0; i < gnRoomsNb; i++) free(*(gppRooms + i));
	free(gppRooms);

}

// Ajoute une pièce à la liste.
void Room_Add(void)
{
	u32	nRoomNo = gnRoomsNb;

	// Table de ptrs.
	gnRoomsNb++;
	if ((gppRooms = (struct SRoomData **)realloc(gppRooms, sizeof(struct SRoomData *) * gnRoomsNb)) == NULL)
	{
		fprintf(stderr, "Room_Add(): realloc failed.\n");
		exit (1);
	}
	// Malloc de la salle.
	if ((*(gppRooms + nRoomNo) = (struct SRoomData *)malloc(sizeof(struct SRoomData))) == NULL)
	{
		fprintf(stderr, "Room_Add(): malloc failed.\n");
		exit (1);
	}

	// Init data de la pièce.
	struct SRoomData	*pRoom;
	pRoom = *(gppRooms + nRoomNo);

	u32	i;
	for (i = 0; i < 4; i++) pRoom->pExits[i] = -1;		// Sorties (Haut, droite, bas, gauche). -1, pas de sortie.
	pRoom->nDimX = ROOM_MAX_X;	// Dimensions de la pièce.
	pRoom->nDimY = ROOM_MAX_Y;
	pRoom->nWallType = 0;		// Type de mur.
	pRoom->nLightDefault = 1;	// Eclairage par défaut. 1 = Oui, 0 = Non.

	Room_Clear(pRoom);

}

// Suppression d'une pièce existante.
void Room_Del(u32 nRoomNo)
{
	struct SRoomData	*pRoom;
	u32	i, k;
	u32	nExitNb;
	u32	nExits;

	pRoom = *(gppRooms + nRoomNo);

	// Vérification du nombre de sorties.
	nExitNb = 0;
	nExits = 0;
	for (i = 0; i < 4; i++)
	{
		if (pRoom->pExits[i] != -1)
		{
			nExitNb++;
			nExits |= 1 << i;
		}
	}

	if (nExitNb == 0 || nExitNb > 2) return;	// 0 sorties ou plus que 2, on ne peut pas supprimer.
	// Si on a deux sorties, on vérifie que ce sont des sorties opposées.
	if (nExitNb == 2)
		if (!(nExits == 5 || nExits == 10)) return;

	// Ok, suppression possible.

	// Update des liens des pièces connexes.
	s32	nRoomToGo = -1;
	for (i = 0; i < 4; i++)
	{
		if (nExits & (1 << i))
		{
			(*(gppRooms + pRoom->pExits[i]))->pExits[(i + 2) & 3] =
				pRoom->pExits[(i + 2) & 3];

			if (nRoomToGo == -1) nRoomToGo = pRoom->pExits[i];	// Note la pièce de sortie au passage.
		}
	}

	// Ok, maintenant on va modifier tous les indices des pièces existantes,
	// si > à la pièce à supprimer, -1.
	for (k = 0; k < gnRoomsNb; k++)
	for (i = 0; i < 4; i++)
		if ( (*(gppRooms + k))->pExits[i] != -1)
			if ( (*(gppRooms + k))->pExits[i] > nRoomNo)
				(*(gppRooms + k))->pExits[i]--;

	if (nRoomToGo > nRoomNo) nRoomToGo--;

	// Free.
	free(*(gppRooms + nRoomNo));
	*(gppRooms + nRoomNo) = NULL;
	gnRoomsNb--;

	// Décalage des pointeurs sur les pièces.
	for (k = nRoomNo; k < gnRoomsNb; k++)
		*(gppRooms + k) = *(gppRooms + k + 1);

	// Déplacement.
	Room_Set(nRoomToGo);

}

// Insertion d'une pièce.
void Room_Ins(u32 nRoomNo, u32 nDir)
{
	u32	nNewRoom;

	// Création d'une nouvelle pièce.
	Room_Add();
	nNewRoom = gnRoomsNb - 1;

	if (gRoom.pRoomData->pExits[nDir] == -1)
	{
		// La direction était vide.
		gRoom.pRoomData->pExits[nDir] = nNewRoom;
		(*(gppRooms + nNewRoom))->pExits[(nDir + 2) & 3] = gRoom.nRoomNo;
	}
	else
	{
		// Il y avait une pièce.
		u32	nOldRoom;
		nOldRoom = gRoom.pRoomData->pExits[nDir];
		// Nouvelle pièce.
		(*(gppRooms + nNewRoom))->pExits[nDir] = nOldRoom;
		(*(gppRooms + nNewRoom))->pExits[(nDir + 2) & 3] = gRoom.nRoomNo;
		// Pièce en cours.
		gRoom.pRoomData->pExits[nDir] = nNewRoom;
		// Ancienne pièce.
		(*(gppRooms + nOldRoom))->pExits[(nDir + 2) & 3] = nNewRoom;
	}

	// On se place dans la nouvelle pièce.
	Room_Set(nNewRoom);

}

#define	ROOM_FLOOR_DEFAULT	1		// Bloc de sol par défaut.
// Nettoyage d'une pièce (RAZ).
void Room_Clear(struct SRoomData *pRoom)
{
	u32	i;
	for (i = 0; i < ROOM_MAX_X * ROOM_MAX_Y * (ROOM_MAX_Z - 1); i++)
		pRoom->pTab[i] = 0;
	for (i = 0; i < ROOM_MAX_X * ROOM_MAX_Y; i++)
		pRoom->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (ROOM_MAX_Z - 1)) + i] = ROOM_FLOOR_DEFAULT;

}

// Calcul de l'excentrage de la pièce en cours.
void Room_WallPos(void)
{
	gRoom.nDispOffsX = (SCR_Width / 2) + ((ROOM_MAX_X - gRoom.pRoomData->nDimX) * 8) - ((ROOM_MAX_Y - gRoom.pRoomData->nDimY) * 8);
	gRoom.nDispOffsY = ((SCR_Height / 2) + 16) - (ROOM_MAX_Z * MAP_TILEZ);

}

// Init.
void Room_Set(u32 nRoomNo)
{
	gRoom.nRoomNo = nRoomNo;
	gRoom.pRoomData = gppRooms[nRoomNo];	// Pointeur sur la pièce en cours.
	Room_WallPos();

}

// Affichage de la pièce.
void Room_Display(void)
{
	SDL_Rect	sSrc, sDst;
	s32	nScrX, nScrY;

	s32	x, y, z;
	u32	nBlk;

	// Affichage des murs.
	// Partie gauche.
	sSrc.x = 64 * gRoom.pRoomData->nWallType;
	sSrc.y = 0;
	sSrc.w = 16;
	sSrc.h = 128+32;
	ISO_ScreenPosition(0 * MAP_TILEW, (gRoom.pRoomData->nDimY - 1) * MAP_TILEH, (ROOM_MAX_Z - 1) * MAP_TILEZ, &nScrX, &nScrY);
	sDst.x = nScrX - (GFX_BLK_W / 2);
	sDst.y = nScrY + 8 - (128+32);
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	// Partie centrale.
	sSrc.x = (64 * gRoom.pRoomData->nWallType) + 16;
	sSrc.y = 0;
	sSrc.w = 32;
	sSrc.h = 128+32;
	ISO_ScreenPosition(0 * MAP_TILEW, 0 * MAP_TILEH, (ROOM_MAX_Z - 1) * MAP_TILEZ, &nScrX, &nScrY);
	sDst.x = nScrX - (GFX_BLK_W / 2);
	sDst.y = nScrY + 8 - (64+32)-8;
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);
	// Partie droite.
	sSrc.x = (64 * gRoom.pRoomData->nWallType) + (16 * 3);
	sSrc.y = 0;
	sSrc.w = 16;
	sSrc.h = 128+32;
	ISO_ScreenPosition((gRoom.pRoomData->nDimX - 1) * MAP_TILEW, 0 * MAP_TILEH, (ROOM_MAX_Z - 1) * MAP_TILEZ, &nScrX, &nScrY);
	sDst.x = nScrX;// - (GFX_BLK_W / 2);
	sDst.y = nScrY + 8 - (128+32);
	SDL_BlitSurface(gVar.pWalls, &sSrc, gVar.pScreen, &sDst);

	// Les blocs.
	s32	nXMax, nYMax, nZMin;
	nXMax = ROOM_MAX_X;
	nYMax = ROOM_MAX_Y;
	nZMin = 0;
	if (gEditor.nMode == e_EditorMode_Map)
	{	// Affichages par "tranches".
		if (gVar.pKeys[SDLK_1]) nXMax = gEditor.nPosX + 1;
		if (gVar.pKeys[SDLK_2]) nYMax = gEditor.nPosY + 1;
		if (gVar.pKeys[SDLK_3]) nZMin = gEditor.nPosZ;
	}

//	for (y = 0; y < ROOM_MAX_Y; y++)
	for (y = 0; y < nYMax; y++)
//	for (x = 0; x < ROOM_MAX_X; x++)
	for (x = 0; x < nXMax; x++)
	{
		if (x < gRoom.pRoomData->nDimX && y < gRoom.pRoomData->nDimY)
//		for (z = ROOM_MAX_Z - 1; z >= 0; z--)
		for (z = ROOM_MAX_Z - 1; z >= nZMin; z--)
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

}

