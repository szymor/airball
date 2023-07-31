

// Enums.
enum
{
	MENU_Null = 0,
	MENU_Main,
	MENU_Credits,
	MENU_HighScores,
	MENU_Game,
	MENU_Quit,

};

void MenuMain_Init(void);
u32 MenuMain_Main(void);
void MenuCredits_Init(void);
u32 MenuCredits_Main(void);

void MenuHighScores_Init(void);
void MenuHighScores_Init2(void);
u32 MenuHighScores_Main(void);
s32 Scr_CheckHighSc(u32 nTime, u32 nObjects);
s32 Scr_PutNameInTable(char *pName, u32 nTime, u32 nObjects);
void Scr_RazTable(void);
u32 Scr_CalcChecksum(void);
void Scr_Load(void);
void Scr_Save(void);


//=====================================

// Enum touches de la config.
enum
{
	e_CfgKey_Up = 0,
	e_CfgKey_Down,
	e_CfgKey_Left,
	e_CfgKey_Right,
	e_CfgKey_ButtonA,
	e_CfgKey_ButtonB,

	e_CfgKey_LAST,

	e_CfgKey_JoyButtonA = e_CfgKey_LAST,
	e_CfgKey_JoyButtonB,

	e_CfgKey_MAX
};

#pragma pack(1)
struct SAirballCfg
{
	u16	pKeys[e_CfgKey_MAX];
	u16	nVideoMode;			// 0 = 320x224 / 1 = x2 / 2 = TV2x.
	u16	nChecksum;
};
#pragma pack()
extern struct SAirballCfg	gAirballCfg;

u32 CfgLoad(void);


