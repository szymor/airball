
#include "includes.h"

struct	SEditor	gEditor;

// Blocs doubles.
static	u8	gpnBlksDoubles[] = {
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
1,0,1,0, 1,0,1,0, 0,1,0,1, 0,1,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

1,0,1,0, 1,0,1,0, 1,0,1,0, 1,0,1,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
1,0,1,0, 1,0,1,0, 1,0,1,0, 0,0,0,0,
0,1,0,1, 0,0,0,1, 0,1,0,1, 0,1,0,0,

1,0,1,0, 1,0,1,0, 1,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,1,0,
0,0,0,1, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,1, 0,0,1,0, 0,1,0,1, 0,1,0,0,

1,0,1,0, 1,0,1,0, 0,1,0,1, 0,1,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

};

//=====================================

struct SMsg
{
	s16	nPosX, nPosY;
	char	*pStr;
	u16	nTimer;			// 0 = pas de message.

};
struct SMsg	gMsg;
#define	MSG_TIMER	(64)

// Initialise un message.
void Msg_Init(char *pStr, s16 nPosX, s16 nPosY)
{
	if (pStr == NULL) return;

	gMsg.nPosX = nPosX;
	gMsg.nPosY = nPosY;
	gMsg.pStr = pStr;
	gMsg.nTimer = MSG_TIMER;

}

// Affichage du message en cours.
void Msg_Display(void)
{
	if (gMsg.nTimer == 0) return;

	FontPrint(gMsg.nPosX, gMsg.nPosY, gMsg.pStr);
	gMsg.nTimer--;

}

//=====================================

enum
{
	e_KbNext_Goto = 1,
	e_KbNext_ManualLink,
};

struct SKbInput
{
	u8	nKbInput;
	s32	nKbNb;			// -1 = Annulé.
	u8	nKbNextAction;
	char	*pKbStr;
	s16	nKbPosX, nKbPosY;

	u8	nKbSavDir;		// Manual link, extra var.
};
struct SKbInput	gKbInput;


// Initialise une saisie.
void KbInput_Init(u32 nNextAction, char *pStr, s16 nPosX, s16 nPosY)
{
	gKbInput.nKbInput = 1;
	gKbInput.nKbNextAction = nNextAction;
	gKbInput.nKbNb = 0;
	gKbInput.pKbStr = pStr;
	gKbInput.nKbPosX = nPosX;
	gKbInput.nKbPosY = nPosY;

}

// Affichage.
void KbInput_Display(void)
{
	u32	nOffset = 0;
	char	pTb[] = "   ";

	// Texte, s'il y en a un.
	if (gKbInput.pKbStr != NULL)
	{
		FontPrint(gKbInput.nKbPosX, gKbInput.nKbPosY, gKbInput.pKbStr);
		nOffset += strlen(gKbInput.pKbStr) * 8;
	}
	// Le nombre en cours.
	MyItoA(gKbInput.nKbNb, pTb);
	FontPrint(gKbInput.nKbPosX + nOffset, gKbInput.nKbPosY, pTb);

}

// Récupération des chiffres saisis.
void KbInput_NumberGet(void)
{
	// Esc : Annulation.
	if (gVar.pKeys[SDLK_ESCAPE])
	{
		gKbInput.nKbInput = 0;
		gKbInput.nKbNextAction = 0;
		gVar.pKeys[SDLK_ESCAPE] = 0;
		return;
	}

	// Return : Validation.
	if (gVar.pKeys[SDLK_RETURN] || gVar.pKeys[SDLK_KP_ENTER])
	{
		gKbInput.nKbInput = 0;
		gVar.pKeys[SDLK_RETURN] = gVar.pKeys[SDLK_KP_ENTER] = 0;
		return;
	}

	// Backspace.
	if (gVar.pKeys[SDLK_BACKSPACE])
	{
		gKbInput.nKbNb /= 10;
		gVar.pKeys[SDLK_BACKSPACE] = 0;
		return;
	}

	// 0-9.
	u32	i;
	for (i = SDLK_0; i <= SDLK_9; i++)
	if (gVar.pKeys[i])
	{
		gKbInput.nKbNb *= 10;
		gKbInput.nKbNb += i - SDLK_0;
		gVar.pKeys[i] = 0;
		return;
	}

	for (i = SDLK_KP0; i <= SDLK_KP9; i++)
	if (gVar.pKeys[i])
	{
		gKbInput.nKbNb *= 10;
		gKbInput.nKbNb += i - SDLK_KP0;
		gVar.pKeys[i] = 0;
		return;
	}

}

//=====================================

// Init.
void Editor_Init(void)
{
	gEditor.nMode = e_EditorMode_Basic;

	Editor_SelBlk_PosReset();
	gEditor.nBlk = 0;		// Bloc à poser.

	gEditor.nMouseArrowsZone = -1;

	gppRooms = NULL;		// Pointeur sur les pièces.
	gnRoomsNb = 0;			// Nb de pièces.

	gKbInput.nKbInput = 0;
	gKbInput.nKbNb = 0;		// -1 = Annulé.
	gKbInput.nKbNextAction = 0;

	gMsg.nTimer = 0;

}

// RAZ position du bloc d'édition en cours.
void Editor_SelBlk_PosReset(void)
{
	gEditor.nPosX = 0;
	gEditor.nPosY = 0;
	gEditor.nPosZ = ROOM_MAX_Z - 1;
};

// Affiche le bloc courant.
void Editor_SelBlk_Show(void)
{
	s32	nScrX, nScrY;
	u32	i;

	// Le cube.
	for (i = 0; i < MAP_TILEW; i+= 2)
	{
		ISO_ScreenPosition((gEditor.nPosX << 4) + i, gEditor.nPosY << 4, gEditor.nPosZ << 4, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		ISO_ScreenPosition((gEditor.nPosX << 4) + i+1, (gEditor.nPosY << 4) + MAP_TILEW-1, gEditor.nPosZ << 4, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		plot(nScrX+0, nScrY + MAP_TILEZ-1);

		ISO_ScreenPosition((gEditor.nPosX << 4), (gEditor.nPosY << 4) + i, gEditor.nPosZ << 4, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		ISO_ScreenPosition((gEditor.nPosX << 4) + MAP_TILEW-1, (gEditor.nPosY << 4) + i, gEditor.nPosZ << 4, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		plot(nScrX+0, nScrY + MAP_TILEZ-1);

		ISO_ScreenPosition((gEditor.nPosX << 4) + MAP_TILEW-1, (gEditor.nPosY << 4), (gEditor.nPosZ << 4) + i+1, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		ISO_ScreenPosition((gEditor.nPosX << 4), (gEditor.nPosY << 4) + MAP_TILEH-1, (gEditor.nPosZ << 4) + i+1, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		ISO_ScreenPosition((gEditor.nPosX << 4) + MAP_TILEW-1, (gEditor.nPosY << 4) + MAP_TILEH-1, (gEditor.nPosZ << 4) + i+1, &nScrX, &nScrY);
		plot(nScrX, nScrY);
	}

	// Position sur le mur gauche.
	for (i = 0; i < MAP_TILEW; i+= 2)
	{
		ISO_ScreenPosition((gEditor.nPosX << 4) + i, 0, gEditor.nPosZ << 4, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		plot(nScrX+1, nScrY + MAP_TILEZ-1);
		ISO_ScreenPosition((gEditor.nPosX << 4), 0, (gEditor.nPosZ << 4) + i, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		ISO_ScreenPosition((gEditor.nPosX << 4) + MAP_TILEW-1, 0, (gEditor.nPosZ << 4) + i+1, &nScrX, &nScrY);
		plot(nScrX, nScrY);
	}

	// Position sur le mur droit.
	for (i = 0; i < MAP_TILEW; i+= 2)
	{
		ISO_ScreenPosition(0, (gEditor.nPosY << 4) + i, gEditor.nPosZ << 4, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		plot(nScrX-1, nScrY + MAP_TILEZ-1);
		ISO_ScreenPosition(0, (gEditor.nPosY << 4), (gEditor.nPosZ << 4) + i, &nScrX, &nScrY);
		plot(nScrX, nScrY);
		ISO_ScreenPosition(0, (gEditor.nPosY << 4) + MAP_TILEH-1, (gEditor.nPosZ << 4) + i+1, &nScrX, &nScrY);
		plot(nScrX, nScrY);
	}

}

// Affiche le bloc en cours.
void Editor_sub_BlkDisp(u32 nBlk, s16 nScrPosX, s16 nScrPosY)
{
	SDL_Rect	sSrc, sDst;

	sSrc.x = (nBlk & 15) * GFX_BLK_W;
	sSrc.y = (nBlk >> 4) * GFX_BLK_H;
	sSrc.w = GFX_BLK_W;
	sSrc.h = GFX_BLK_H;
	sDst.x = nScrPosX;
	sDst.y = nScrPosY;
	SDL_BlitSurface(gVar.pBlk, &sSrc, gVar.pScreen, &sDst);

}


// Affichage du mode.
#define	EDMODE_POSX	200
#define	EDMODE_POSY	4
#define	EDROOM_POSX	16
#define	EDROOM_POSY	4
#define	EDLIGHT_POSX	(EDROOM_POSX)
#define	EDLIGHT_POSY	(EDROOM_POSY + 16)
#define	EDGOTO_POSX	(EDLIGHT_POSX)
#define	EDGOTO_POSY	(EDLIGHT_POSY + 16)
#define	EDARROWS_POSX	32
#define	EDARROWS_POSY	(SCR_Height - 32)
#define	EDBLK_POSX	(32+4)
#define	EDBLK_POSY	(SCR_Height - GFX_BLK_H - 10)
#define	EDSPKEYS_POSX	(EDARROWS_POSX + 6)	//(140)
#define	EDSPKEYS_POSY	(EDARROWS_POSY - 12)	//(SCR_Height - 12)
void Editor_ModeDisp(void)
{
	SDL_Rect	sSrc, sDst;

	// Room no.
	char	pRoomNo[] = "ROOM #000";
	MyItoA(gRoom.nRoomNo, &pRoomNo[6]);
	FontPrint(EDROOM_POSX, EDROOM_POSY, pRoomNo);
	char	pRoomNo2[] = "/000";
	MyItoA(gnRoomsNb - 1, &pRoomNo2[1]);
	FontPrint(EDROOM_POSX + (8 * strlen(pRoomNo)), EDROOM_POSY, pRoomNo2);

	// La lumière par défaut.
	char	pLight[] = "LIGHT OFF";
	if (gRoom.pRoomData->nLightDefault) { pLight[7] = 'N'; pLight[8] = ' '; };
	FontPrint(EDLIGHT_POSX, EDLIGHT_POSY, pLight);


	// En fct du mode.
	switch (gEditor.nMode)
	{
	case e_EditorMode_Map:
		FontPrint(EDMODE_POSX, EDMODE_POSY, "MAP MODE");
		{
			char	pTb[] = "( \0 \0  \0";
			MyItoA(gEditor.nPosX, &pTb[1]);
			MyItoA(gEditor.nPosY, &pTb[3]);
			MyItoA(gEditor.nPosZ, &pTb[5]);
			pTb[2] = pTb[4] = ','; pTb[7] = ')';
			FontPrint(EDMODE_POSX, EDMODE_POSY+8, pTb);
		}
		Editor_SelBlk_Show();	// Affiche le bloc d'édition courant.



		// Affichage du bloc en cours.

		// Les blocs avant et après.
		s32	nBlkMax;
		nBlkMax = (gVar.pBlk->h / GFX_BLK_H) * (gVar.pBlk->w / GFX_BLK_W);

		if ((s32)gEditor.nBlk - 1 >= 0)
			Editor_sub_BlkDisp(gEditor.nBlk - 1, EDBLK_POSX -32-4, EDBLK_POSY -16-2);
		if (gEditor.nBlk + 1 < nBlkMax)
			Editor_sub_BlkDisp(gEditor.nBlk + 1, EDBLK_POSX +32+4, EDBLK_POSY +16+2);

		// Le bloc en cours.
		Editor_sub_BlkDisp(gEditor.nBlk, EDBLK_POSX, EDBLK_POSY);
		lineH(EDBLK_POSX - 1, EDBLK_POSX + GFX_BLK_W, EDBLK_POSY - 1);
		lineV(EDBLK_POSX - 1, EDBLK_POSY - 1, EDBLK_POSY + GFX_BLK_H);
		lineV(EDBLK_POSX + GFX_BLK_W, EDBLK_POSY - 1, EDBLK_POSY + GFX_BLK_H);
		lineH(EDBLK_POSX - 1, EDBLK_POSX + GFX_BLK_W, EDBLK_POSY + GFX_BLK_H);

		// N° du bloc.
		{
			char	pBlock[] = "   ";
			MyItoA(gEditor.nBlk, pBlock);
			FontPrint(EDBLK_POSX + 4, EDBLK_POSY + GFX_BLK_H + 1+1, pBlock);
		}

		break;

	default:
	case e_EditorMode_Basic:
		FontPrint(EDMODE_POSX, EDMODE_POSY, "BASIC MODE");

		{
		// Les directions des 4 sorties.
		s32	nVal;
		u32	i;
		static	s32	pnArrows[] = { 23,0, 23,15, 0,15, 0,0 };
		static	s32	pnLinks[] = { 23+20,0, 23+20,15+5, 0-24,15+5, 0-24,0 };
		char	pTb[4];
		for (i = 0; i < 4; i++)
		{
			nVal = gRoom.pRoomData->pExits[i];
			// Flèche Rouge/verte.
			sSrc.x = 12 + (i * 40) + (nVal >= 0 ? 20 : 0);
			sSrc.y = 0;
			sSrc.w = 20;
			sSrc.h = 12;
			sDst.x = EDARROWS_POSX + pnArrows[(i * 2)];
			sDst.y = EDARROWS_POSY + pnArrows[(i * 2) + 1];
			SDL_BlitSurface(gVar.pEdStuff, &sSrc, gVar.pScreen, &sDst);
			// N° de salle liée.
			pTb[0] = pTb[1] = pTb[2] = ' '; pTb[3] = 0;
			MyItoA(nVal, pTb);
			FontPrint(EDARROWS_POSX + pnLinks[(i * 2)], EDARROWS_POSY + pnLinks[(i * 2) + 1], pTb);
		}

		// Etat des touches Shift, Control, Alt.
		u16	pSpKeys[] = { SDLK_LSHIFT, SDLK_LCTRL, SDLK_LALT };
		char *pSpKeysNames[] = { "S", "C", "A" };
		for (i = 0; i < NBELEM(pSpKeys); i++)
		if (gVar.pKeys[pSpKeys[i]])
		{
			FontPrint(EDSPKEYS_POSX + (i * 12), EDSPKEYS_POSY, pSpKeysNames[i]);
			lineH(EDSPKEYS_POSX + (i * 12) - 2, EDSPKEYS_POSX + (i * 12) + 8, EDSPKEYS_POSY - 2);
			lineH(EDSPKEYS_POSX + (i * 12) - 2, EDSPKEYS_POSX + (i * 12) + 8, EDSPKEYS_POSY + 8);
			lineV(EDSPKEYS_POSX + (i * 12) - 2, EDSPKEYS_POSY - 2, EDSPKEYS_POSY + 8);
			lineV(EDSPKEYS_POSX + (i * 12) + 8, EDSPKEYS_POSY - 2, EDSPKEYS_POSY + 8);
		}

		// Affichage de la zone de clic en cours (si il y en a une).
		Mouse_ZoneDisp();

		// Affichage de la souris.
		sSrc.x = 0;
		sSrc.y = 0;
		sSrc.w = 12;
		sSrc.h = 20;
		sDst.x = gVar.nMousePosX;
		sDst.y = gVar.nMousePosY;
		SDL_BlitSurface(gVar.pEdStuff, &sSrc, gVar.pScreen, &sDst);
		}

		break;
	}

	// Saisie en cours ?
	if (gKbInput.nKbInput) KbInput_Display();

	// Message.
	Msg_Display();

}

// Déplacement dans une direction, création d'une nouvelle pièce si nécessaire.
void Editor_ArrowPressed(u32 nDir)
{

	// Ctrl + dir => Insertion.
	if (gVar.pKeys[SDLK_LCTRL])
	{
		Room_Ins(gRoom.nRoomNo, nDir);
		return;
	}


	if (gRoom.pRoomData->pExits[nDir] != -1)
	{
		// La pièce existe, on y va.
		Room_Set(gRoom.pRoomData->pExits[nDir]);
	}
	else
	// La pièce n'existe pas, si shift, on créé une nouvelle pièce.
	if (gVar.pKeys[SDLK_LSHIFT])
	{
		// Création d'une nouvelle pièce.
		u32	nLastRoom = gRoom.nRoomNo;
		Room_Add();
		gRoom.pRoomData->pExits[nDir] = gnRoomsNb - 1;
		Room_Set(gnRoomsNb - 1);
		gRoom.pRoomData->pExits[(nDir + 2) & 3] = nLastRoom;
	}
	else
	// La pièce n'existe pas, si alt, édition manuelle du lien.
	if (gVar.pKeys[SDLK_LALT])
	{
		gKbInput.nKbSavDir = nDir;
		KbInput_Init(e_KbNext_ManualLink, "LINK TO:", EDGOTO_POSX, EDGOTO_POSY);
	}

}

// Déplacement du cube d'édition.
void Editor_SelMove(s32 nMvt, s32 *pPos, s32 nValMax)
{
	s32	t;

	t = *pPos;
	if (nMvt) t += SGN(nMvt);

	if (t < 0) t = 0;
	if (t >= nValMax) t = nValMax - 1;

	*pPos = t;

}

#define	DATA_FILENAME	"rooms.bin"

#define	MOUSE_SENSITIVITY	16

// Gestion des touches en fct du mode.
void Editor_Keys(void)
{

	// Saisie en cours ?
	if (gKbInput.nKbInput)
	{
		KbInput_NumberGet();
		return;
	}
	// Après saisie.
	if (gKbInput.nKbNextAction)
	{
		switch (gKbInput.nKbNextAction)
		{
		case e_KbNext_Goto:		// Goto room #x.
			// Si la pièce #x existe, on y va.
			if (gKbInput.nKbNb < gnRoomsNb)
				Room_Set(gKbInput.nKbNb);
			else
				Msg_Init("NO SUCH ROOM!", EDGOTO_POSX, EDGOTO_POSY);
			break;

		case e_KbNext_ManualLink:	// Edition manuelle de liens.
			if (gKbInput.nKbNb < gnRoomsNb)
			{
				// Les directions des deux pièces étaient vides ?
				if (gRoom.pRoomData->pExits[gKbInput.nKbSavDir] == -1 &&
					(*(gppRooms + gKbInput.nKbNb))->pExits[(gKbInput.nKbSavDir + 2) & 3] == -1)
				{
					gRoom.pRoomData->pExits[gKbInput.nKbSavDir] = gKbInput.nKbNb;
					(*(gppRooms + gKbInput.nKbNb))->pExits[(gKbInput.nKbSavDir + 2) & 3] = gRoom.nRoomNo;
				}
			}
			break;
		}
		gKbInput.nKbNextAction = 0;
		return;
	}

	//

	// F8 : Save.
	if (gVar.pKeys[SDLK_F8])
	{
		Editor_FileSave(DATA_FILENAME);
		gVar.pKeys[SDLK_F8] = 0;
	}

	// KP0 : Basic / Map.
	if (gVar.pKeys[SDLK_KP0])
	{
		static	s32	nMouseSavX, nMouseSavY;

		gEditor.nMode ^= 1;
		gVar.pKeys[SDLK_KP0] = 0;

		if (gEditor.nMode)
		{
			nMouseSavX = gVar.nMousePosX;
			nMouseSavY = gVar.nMousePosY;
			SDL_WarpMouse(SCR_Width / 2, SCR_Height / 2);
		}
		else
		{
			gVar.nMousePosX = nMouseSavX;
			gVar.nMousePosY = nMouseSavY;
			SDL_WarpMouse(gVar.nMousePosX, gVar.nMousePosY);
		}
		return;
	}

	switch(gEditor.nMode)
	{
	case e_EditorMode_Map:

		// Déplacement du cube d'édition.
		// Mouse.
		{
			s32	nDiffX, nDiffY;
			nDiffX = gVar.nMousePosX - (SCR_Width / 2);
			nDiffY = gVar.nMousePosY - (SCR_Height / 2);
			if (ABS(nDiffX) >= MOUSE_SENSITIVITY)
			{
				Editor_SelMove(nDiffX, &gEditor.nPosX, gRoom.pRoomData->nDimX);
				SDL_WarpMouse(SCR_Width / 2, gVar.nMousePosY);
			}
			if (ABS(nDiffY) >= MOUSE_SENSITIVITY)
			{
				Editor_SelMove(nDiffY, &gEditor.nPosY, gRoom.pRoomData->nDimY);
				SDL_WarpMouse(gVar.nMousePosX, SCR_Height / 2);
			}
			// Wheel ?
			if (gVar.nMouseButtons & MOUSE_WHEELUP)
				Editor_SelMove(-1, &gEditor.nPosZ, ROOM_MAX_Z);
			if (gVar.nMouseButtons & MOUSE_WHEELDOWN)
				Editor_SelMove(1, &gEditor.nPosZ, ROOM_MAX_Z);
		}
		// Clavier.
		if (gVar.pKeys[SDLK_KP2])
		{
			Editor_SelMove(1, &gEditor.nPosY, gRoom.pRoomData->nDimY);
			gVar.pKeys[SDLK_KP2] = 0;
		}
		if (gVar.pKeys[SDLK_KP5])
		{
			Editor_SelMove(-1, &gEditor.nPosY, gRoom.pRoomData->nDimY);
			gVar.pKeys[SDLK_KP5] = 0;
		}
		if (gVar.pKeys[SDLK_KP3])
		{
			Editor_SelMove(1, &gEditor.nPosX, gRoom.pRoomData->nDimX);
			gVar.pKeys[SDLK_KP3] = 0;
		}
		if (gVar.pKeys[SDLK_KP1])
		{
			Editor_SelMove(-1, &gEditor.nPosX, gRoom.pRoomData->nDimX);
			gVar.pKeys[SDLK_KP1] = 0;
		}
		if (gVar.pKeys[SDLK_KP9])
		{
			Editor_SelMove(-1, &gEditor.nPosZ, ROOM_MAX_Z);
			gVar.pKeys[SDLK_KP9] = 0;
		}
		if (gVar.pKeys[SDLK_KP6])
		{
			Editor_SelMove(1, &gEditor.nPosZ, ROOM_MAX_Z);
			gVar.pKeys[SDLK_KP6] = 0;
		}

		// Changement du bloc courant.
		{
		s32	nBlkMax;
		nBlkMax = (gVar.pBlk->h / GFX_BLK_H) * (gVar.pBlk->w / GFX_BLK_W);

		if (gVar.pKeys[SDLK_UP])
		{
			if ((s32)gEditor.nBlk - (gVar.pBlk->w / GFX_BLK_W) >= 0) gEditor.nBlk -= (gVar.pBlk->w / GFX_BLK_W);
			gVar.pKeys[SDLK_UP] = 0;
		}
		if (gVar.pKeys[SDLK_DOWN])
		{
			if ((s32)gEditor.nBlk + (gVar.pBlk->w / GFX_BLK_W) < nBlkMax) gEditor.nBlk += (gVar.pBlk->w / GFX_BLK_W);
			gVar.pKeys[SDLK_DOWN] = 0;
		}
		if (gVar.pKeys[SDLK_LEFT])
		{
			if (gEditor.nBlk) gEditor.nBlk--;
			gVar.pKeys[SDLK_LEFT] = 0;
		}
		if (gVar.pKeys[SDLK_RIGHT])
		{
			if (gEditor.nBlk + 1 < nBlkMax) gEditor.nBlk++;
			gVar.pKeys[SDLK_RIGHT] = 0;
		}

		}

		// Poke ?
		if (gVar.nMouseButtons & MOUSE_LMB)
		{
			gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * gEditor.nPosZ) + (gEditor.nPosY * ROOM_MAX_X) + gEditor.nPosX] = gEditor.nBlk;
			// Bloc double ?
			if (gpnBlksDoubles[gEditor.nBlk])
			if (gEditor.nPosZ - 1 >= 0)
				gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (gEditor.nPosZ - 1)) + (gEditor.nPosY * ROOM_MAX_X) + gEditor.nPosX] = gEditor.nBlk + 1;
		}
		// Clear ?
		if (gVar.nMouseButtons & MOUSE_RMB)
		{
			s32	nBlk;

			nBlk = gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * gEditor.nPosZ) + (gEditor.nPosY * ROOM_MAX_X) + gEditor.nPosX];

			gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * gEditor.nPosZ) + (gEditor.nPosY * ROOM_MAX_X) + gEditor.nPosX] = 0;
			// Bloc double ?
			if (gpnBlksDoubles[nBlk])
			if (gEditor.nPosZ - 1 >= 0)
				gRoom.pRoomData->pTab[(ROOM_MAX_X * ROOM_MAX_Y * (gEditor.nPosZ - 1)) + (gEditor.nPosY * ROOM_MAX_X) + gEditor.nPosX] = 0;
		}

		break;

	default:
	case e_EditorMode_Basic:
		Mouse_ZoneClick();	// Teste si la souris est en zone de clic.

		// Clic sur une flèche ?
		if (gVar.nMouseButtons & MOUSE_LMB && gEditor.nMouseArrowsZone >= 0)
		{
			Editor_ArrowPressed(gEditor.nMouseArrowsZone);
			gVar.nMouseButtons &= ~MOUSE_LMB;
		}
		// Flèches de direction, déplacement dans le chateau.
		if (gVar.pKeys[SDLK_UP])
		{
			Editor_ArrowPressed(e_Room_ExitUp);
			gVar.pKeys[SDLK_UP] = 0;
		}
		if (gVar.pKeys[SDLK_DOWN])
		{
			Editor_ArrowPressed(e_Room_ExitDown);
			gVar.pKeys[SDLK_DOWN] = 0;
		}
		if (gVar.pKeys[SDLK_RIGHT])
		{
			Editor_ArrowPressed(e_Room_ExitRight);
			gVar.pKeys[SDLK_RIGHT] = 0;
		}
		if (gVar.pKeys[SDLK_LEFT])
		{
			Editor_ArrowPressed(e_Room_ExitLeft);
			gVar.pKeys[SDLK_LEFT] = 0;
		}


		// r : Reset room.
		if (gVar.pKeys[SDLK_r])
		{
			Room_Clear(gRoom.pRoomData);
			gVar.pKeys[SDLK_r] = 0;
		}

		// w : Type de mur.
		if (gVar.pKeys[SDLK_w])
		{
//			if (++gRoom.pRoomData->nWallType >= 3) gRoom.pRoomData->nWallType = 0;
			gRoom.pRoomData->nWallType = (gRoom.pRoomData->nWallType + (gVar.pKeys[SDLK_LSHIFT] ? -1 : 1)) & 3;
			gVar.pKeys[SDLK_w] = 0;
		}

		// l : Light.
		if (gVar.pKeys[SDLK_l])
		{
			gRoom.pRoomData->nLightDefault ^= 1;
			gVar.pKeys[SDLK_l] = 0;
		}

		// g : Goto room #x.
		if (gVar.pKeys[SDLK_g])
		{
			KbInput_Init(e_KbNext_Goto, "GO TO ROOM:", EDGOTO_POSX, EDGOTO_POSY);
			gVar.pKeys[SDLK_g] = 0;
		}


		// Réduction agrandissement de la surface utile de la salle.
		{
		s32	t;
		if (gVar.pKeys[SDLK_KP3])
		{
			t = gRoom.pRoomData->nDimX;
			if (++t <= ROOM_MAX_X)
			{
				gRoom.pRoomData->nDimX = t;
				Room_WallPos();
			}
			gVar.pKeys[SDLK_KP3] = 0;
		}
		if (gVar.pKeys[SDLK_KP1])
		{
			t = gRoom.pRoomData->nDimX;
			if (--t >= 2)
			{
				gRoom.pRoomData->nDimX = t;
				Room_WallPos();
				Editor_SelMove(0, &gEditor.nPosX, gRoom.pRoomData->nDimX);	// Pour recalage du bloc d'édition.
			}
			gVar.pKeys[SDLK_KP1] = 0;
		}
		if (gVar.pKeys[SDLK_KP2])
		{
			t = gRoom.pRoomData->nDimY;
			if (++t <= ROOM_MAX_Y)
			{
				gRoom.pRoomData->nDimY = t;
				Room_WallPos();
			}
			gVar.pKeys[SDLK_KP2] = 0;
		}
		if (gVar.pKeys[SDLK_KP5])
		{
			t = gRoom.pRoomData->nDimY;
			if (--t >= 2)
			{
				gRoom.pRoomData->nDimY = t;
				Room_WallPos();
				Editor_SelMove(0, &gEditor.nPosY, gRoom.pRoomData->nDimY);	// Pour recalage du bloc d'édition.
			}
			gVar.pKeys[SDLK_KP5] = 0;
		}
		}

		// Del : Suppression de la pièce.
		if (gVar.pKeys[SDLK_DELETE])
		{
			Room_Del(gRoom.nRoomNo);
			gVar.pKeys[SDLK_DELETE] = 0;
		}


		break;
	}

}

//=====================================

// Sous-routine, calcul du checksum sur une pièce.
u32 Editor_sub_RoomChecksum(u32 *pData, u32 nDWNb)
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

#define	LOAD(data_adr, data_type, fprintf_txt) \
		nSz = fread(data_adr, 1, sizeof(data_type), fPt); \
		if (nSz != sizeof(data_type)) \
		{ \
			fprintf(stderr, fprintf_txt); \
			fclose(fPt); \
			exit(1); \
		}

// Lecture du fichier de données.
void Editor_FileLoad(char *pFilename)
{
	FILE	*fPt;
	u32	i;
	s32	nSz;	//, nSzTot;

	assert((sizeof(struct SRoomData) & 3) == 0);

	// fopen.
	if ((fPt = fopen(pFilename, "rb")) == NULL)
	{
		printf("Data file not found. Restart from scratch.\n");
		//
		Room_Add();
		Room_Set(0);
		return;
	}
/*
	// Taille du fichier.
	fseek(fPt, 0, SEEK_END);
	nSzTot = ftell(fPt);
	fseek(fPt, 0, SEEK_SET);
*/

	// Header.
	LOAD(&i, u32, "Editor_FileLoad(): Read error (Header). Loading aborted.\n")
	if (i != FILE_MAGIC_NUMBER)
	{
		fprintf(stderr, "Editor_FileLoad(): Magic number not found. Loading aborted.\n");
		fclose(fPt);
		exit(1);
	}
	u32	nNbRooms;
	LOAD(&nNbRooms, u32, "Editor_FileLoad(): Read error (Header). Loading aborted.\n")

	// Lecture des pièces une par une... hmmm...
	u32	nChkSum = 0;
	u32	nChkSumLoaded;
	for (i = 0; i < nNbRooms; i++)
	{
		Room_Add();
		LOAD(*(gppRooms + i), struct SRoomData, "Editor_FileLoad(): Read error. Loading aborted.\n")
		nChkSum += Editor_sub_RoomChecksum((u32 *)*(gppRooms + i), sizeof(struct SRoomData) / 4);
	}

	// Lecture du checksum.
	LOAD(&nChkSumLoaded, u32, "Editor_FileLoad(): Read error (Checksum). Loading aborted.\n")

	fclose(fPt);

	// Test du checksum.
	if (nChkSumLoaded != nChkSum)
	{
		fprintf(stderr, "Editor_FileLoad(): Checksum error. Loaded = %X / Calculated = %X. Aborted.\n", nChkSumLoaded, nChkSum);
		fclose(fPt);
		exit(1);
	}

	printf("Data loaded. (%d rooms). Checksum: Loaded = %X / Calculated = %X.\n", nNbRooms, nChkSumLoaded, nChkSum);

	Room_Set(0);

}

#define	SAVE(data_adr, data_type, msg_txt, fprintf_txt) \
	nSz = fwrite(data_adr, 1, sizeof(data_type), fPt); \
	if (nSz != sizeof(data_type)) \
	{ \
		Msg_Init(msg_txt, EDGOTO_POSX, EDGOTO_POSY); \
		fprintf(stderr, fprintf_txt); \
		fclose(fPt); \
		return; \
	} \
	nSzTot += nSz;

// Sauvegarde des salles.
void Editor_FileSave(char *pFilename)
{
	FILE	*fPt;
	u32	i;
	u32	nSz, nSzTot;

	// fopen.
	if ((fPt = fopen(pFilename, "wb")) == NULL)
	{
		Msg_Init("SAVE: FOPEN ERROR.", EDGOTO_POSX, EDGOTO_POSY);
		fprintf(stderr, "Editor_FileSave(): Unable to open data file. Data not saved.\n");
		return;
	}

	// fwrite.
	nSzTot = 0;
	// Micro header.
	i = FILE_MAGIC_NUMBER;
	SAVE(&i, u32, "SAVE: WRITE ERROR (HEADER).", "Editor_FileSave(): Write error (Header). Save aborted.\n")
	//
	i = gnRoomsNb;
	SAVE(&i, u32, "SAVE: WRITE ERROR (HEADER).", "Editor_FileSave(): Write error (Header). Save aborted.\n")

	// Rooms.
	u32	nChkSum = 0;
	for (i = 0; i < gnRoomsNb; i++)
	{
		SAVE(*(gppRooms + i), struct SRoomData, "SAVE: WRITE ERROR.", "Editor_FileSave(): Write error. Save aborted.\n")
		nChkSum += Editor_sub_RoomChecksum((u32 *)*(gppRooms + i), sizeof(struct SRoomData) / 4);
	}

	// Checksum.
	SAVE(&nChkSum, u32, "SAVE: WRITE ERROR (CHK).", "Editor_FileSave(): Write error (Checksum). Save aborted.\n")

	Msg_Init("DATA SAVED.", EDGOTO_POSX, EDGOTO_POSY);
	printf("Data saved. (%d rooms / %d bytes). Checksum = %X\n", gnRoomsNb, (int)nSzTot, nChkSum);

	fclose(fPt);

}

//=====================================

#define	MOUSE_ARROWS_ZONE	4
#define	ARROWSZONE_W	48
#define	ARROWSZONE_H	16
s32	gpnArrowsZones[] = { 23-2,0-2, 23-2,15-2, 0-24-2,15-2, 0-24-2,0-2 };

// Teste si la souris est en zone de clic.
void Mouse_ZoneClick(void)
{
	u32	i;

	gEditor.nMouseArrowsZone = -1;

	for (i = 0; i < MOUSE_ARROWS_ZONE; i++)
	if (gVar.nMousePosX >= EDARROWS_POSX + gpnArrowsZones[(i * 2)] &&
		gVar.nMousePosX < EDARROWS_POSX + gpnArrowsZones[(i * 2)] + ARROWSZONE_W &&
		gVar.nMousePosY >= EDARROWS_POSY + gpnArrowsZones[(i * 2) + 1] &&
		gVar.nMousePosY < EDARROWS_POSY + gpnArrowsZones[(i * 2) + 1] + ARROWSZONE_H)
		{
			gEditor.nMouseArrowsZone = i;
			break;
		}

}

// Affiche la zone de clic en cours.
void Mouse_ZoneDisp(void)
{
	if (gEditor.nMouseArrowsZone < 0) return;

	lineH(EDARROWS_POSX + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2)], EDARROWS_POSX + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2)] + ARROWSZONE_W-1, EDARROWS_POSY + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2) + 1]);
	lineH(EDARROWS_POSX + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2)], EDARROWS_POSX + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2)] + ARROWSZONE_W-1, EDARROWS_POSY + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2) + 1] + ARROWSZONE_H-1);
	lineV(EDARROWS_POSX + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2)], EDARROWS_POSY + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2) + 1], EDARROWS_POSY + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2) + 1] + ARROWSZONE_H-1);
	lineV(EDARROWS_POSX + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2)] + ARROWSZONE_W-1, EDARROWS_POSY + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2) + 1], EDARROWS_POSY + gpnArrowsZones[(gEditor.nMouseArrowsZone * 2) + 1] + ARROWSZONE_H-1);

}


