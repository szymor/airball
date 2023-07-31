

#define	MAP_TILEW	16
#define	MAP_TILEH	16
#define	MAP_TILEZ	16

#define	GFX_BLK_W	32
#define	GFX_BLK_H	40

#define	GAME_TIME_MAX	((FPS_1S * 60 * 60 * 100) - 1)

#define	HUD_AIR_POSX	(104)
#define	HUD_AIR_POSY	(SCR_Height - 5)

#define	HUD_SCORE_POSX	(240)
#define	HUD_SCORE_POSYL1	(SCR_Height - 40)
#define	HUD_SCORE_POSYL2	(SCR_Height - 24)

#define	HUD_GAUGE_POSX	(80)
#define	HUD_GAUGE_POSY	(SCR_Height - 24)

#define	HUD_LIVES_POSX	(32 + 4)
#define	HUD_LIVES_POSY	(SCR_Height - 24)
#define	HUD_LIVES_SPC	(8)


enum
{
	e_Txt_Intro = 0,
	e_Txt_Spellbook,
	e_Txt_Spellbook2,
	e_Txt_Death,
	e_Txt_Win,

};


enum
{
	e_Game_Normal = 0,
	e_Game_RoomTransitOff,
	e_Game_RoomTransitOn,
	e_Game_RoomTransitOffToTxt,
	e_Game_TxtTransitOff,
	e_Game_TxtTransitOn,

};

struct	SGame
{
	u8	nPhase;
	s8	nExitCode;		// Code de sortie du jeu.

	u16	nRoomNextNo;	// Salle suivante, n°.
	s8	nRoomNextDir;	// Salle suivante, dir.
	u16	nRoomFade;		// Fade salle.

	u8	nQuest;			// 0 = Rien / 1 = Spellbook ramené.

	s8	nTxtNo;			// No du texte à afficher.
	s16	nTxtTimer;		// -1 = pas de timer / 0+ = nb de frames max.
	u8	nFadeAll;		// Pour fade palette totale/partielle.

	u16	nLastDeathRoom;	// 0xFFFF => Pas encore valide.
	u8	nLastDeathBlkX, nLastDeathBlkY, nLastDeathBlkZ;

	SDL_Surface	*pTxt;	// Surface pour les textes.

	char	*pLoadSaveTxt;	// Pour affichage du texte load/save.
	u8	nLoadSaveCnt;

//	u32	nScoreDisp;		// Score affiché.

};
extern struct	SGame	gGame;


void ISO_ScreenPosition(s32 nPosX, s32 nPosY, s32 nPosZ, s32 *pnPosX, s32 *pnPosY);

void SetColors_Menu(void);
void SetColors_Game(void);
void SetColors_Txt(void);

void Game_Init(void);
void Game_Manage(void);
void Game_Draw(void);
void Game_TimeToStr(char *pTxtTime, u32 nTime);

void Game_LoadSaveInit(void);
void Game_LoadSaveRelease(void);

u32	Game_SaveSlot(u32 nSlotNo);
u32	Game_LoadSlot(u32 nSlotNo);


