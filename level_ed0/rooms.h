

#define	ROOM_MAX_X	8
#define	ROOM_MAX_Y	8
#define	ROOM_MAX_Z	11

enum
{
	e_Room_ExitUp = 0,
	e_Room_ExitRight,
	e_Room_ExitDown,
	e_Room_ExitLeft,
};

#pragma pack(1)
struct SRoomData
{
	s16	pExits[4];		// Sorties (Haut, droite, bas, gauche). -1, pas de sortie.
	u8	nDimX, nDimY;	// Dimensions de la pièce.
	u8	nWallType;		// Type de mur.
	u8	nLightDefault;	// Eclairage par défaut. 1 = Oui, 0 = Non.
	u8	pTab[ROOM_MAX_X * ROOM_MAX_Y * ROOM_MAX_Z];
};
#pragma pack()

struct SRoom
{
	u32	nRoomNo;
	struct SRoomData	*pRoomData;	// Pointeur sur la pièce en cours.
	u32	nDispOffsX, nDispOffsY;		// Offset pour l'affichage de la pièce.

};
extern struct SRoom	gRoom;
extern struct SRoomData	**gppRooms;		// Pointeur sur les pièces.
extern u32	gnRoomsNb;					// Nb de pièces.


void Room_Clear(struct SRoomData *pRoom);
void Room_Set(u32 nRoomNo);
void Room_Add(void);
void Room_Del(u32 nRoomNo);
void Room_Ins(u32 nRoomNo, u32 nDir);
void Room_FreeAll(void);
void Room_WallPos(void);
void Room_Display(void);

