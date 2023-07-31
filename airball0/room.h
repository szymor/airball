

#define	EXIT_BLK_X0	20
#define	EXIT_BLK_X1	22
#define	EXIT_BLK_Y0	16
#define	EXIT_BLK_Y1	18

#define	BLK_CANDLE	36
#define	BLK_TORCH	45
#define BLK_LAMPION	178

#define	BLK_PUMP	24

#define	BLK_SPELLBOOK	93

#define	BLK_BUDDHA		92
#define	BLK_CRUCIFIX	46
#define	BLK_PUMPKIN		181
#define	BLK_DRAGON		191
#define	BLK_FLASK		157
#define	BLK_BEANS		200

// Position du premier gonfleur => Pt de départ.
#define	PUMP_START_ROOM	(0)
#define	PUMP_START_X	(3)
#define	PUMP_START_Y	(4)
#define	PUMP_START_Z	(ROOM_MAX_Z - 2)



#define	ROOM_MAX_X	8
#define	ROOM_MAX_Y	8
#define	ROOM_MAX_Z	11

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
extern struct SRoomData	*gpRooms;		// Pièces dans lesquelles on joue.
extern struct SRoomData	*gpRoomsOrg;	// Pièces originales, qu'on recopie à chaque début de partie.
extern u32	gnRoomDataSz;				// Taille des datas à copier.

struct SRoom
{
	struct SRoomData	*pRoomData;	// Pointeur sur la pièce en cours.
	u32	nDispOffsX, nDispOffsY;		// Offset pour l'affichage de la pièce.

	SDL_Surface	*pBkg;				// Surface de "décor", le sol et les murs.

	u16	nRoomNo;					// Pièce en cours, au final, ça peut servir. (Pour nPumpRoom par exemple).
	u16	nPumpRoom;					// N° de salle et position du dernier gonfleur touché, pour restart.
	u8	nPumpX, nPumpY, nPumpZ;

	s8	nPresent;		// -1, pas de cadeau, sinon n° du cadeau [0;3].
	u8	nPresentPosX, nPresentPosY, nPresentPosZ;

	u8	nRoomPal;		// Palette en cours.

};
extern struct SRoom	gRoom;


enum
{
	e_Pal_Normal = 0,
	e_Pal_Dark,
	e_Pal_Candle,
	e_Pal_Torch,
};
//void Room_PalSetNo(u32 nPalNo);
void Room_PalSetNo(u32 nPalNo, u32 nFadeVal);
void FadeColors(u32 nFadeVal, u32 nNbColors);

void Room_DataLoad(char *pFilename);
void Room_DataFree(void);

void Room_Init(u32 nRoomNo, s32 nDir);

void Room_PresentBlkReprogram(void);

void Room_HeroCurObjectDisplay(u32 nObject);
u32 Room_BlockIsPortable(u32 nBlkNo);
u32 Room_BlockIsSpiky(u32 nBlkNo, u32 nDir);

u32 Room_AllObjTst(void);


