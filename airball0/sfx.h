

// Prototypes.
void Sfx_SoundInit(void);
void Sfx_SoundOn(void);
void Sfx_SoundOff(void);
void Sfx_LoadWavFiles(void);
void Sfx_FreeWavFiles(void);
void Sfx_LoadYMFiles(void);
void Sfx_FreeYMFiles(void);

void Sfx_PlaySfx(u32 nSfxNo, u32 nSfxPrio);
void Sfx_StopSfx(u32 nSfxNo);
u32 Sfx_IsPlaying(u32 nSfxNo);
void Sfx_ClearChannels(void);

void Music_Start(s32 nMusicNo, u32 nForceRestart);
s32 Music_GetMusicNo(void);
u32 Music_IsOver(void);


// Enum YM.
enum
{
	e_YmMusic_Menu = 0,
	e_YmMusic_Game,
	e_YmMusic_HighScore,
	e_YmMusic_Death,

	e_YmMusic_MAX
};

// Enums Sfx.
enum
{
	e_Sfx_Menu_Click = 0,
	e_Sfx_Game_Death,
	e_Sfx_Game_ObjectDrop,
	e_Sfx_Game_ObjectGet,
	e_Sfx_Game_Present,

	e_Sfx_MAX
};



/*
enum
{
	e_SfxPrio_0 = 0,
	e_SfxPrio_10 = 10,
	e_SfxPrio_20 = 20,
	e_SfxPrio_30 = 30,
	e_SfxPrio_40 = 40,
	e_SfxPrio_Max = 254,

	e_SfxPrio_Enemy_Shot = 10,
	e_SfxPrio_Enemy_Death = 12,
	e_SfxPrio_Explosion = 15,
	e_SfxPrio_Hero_Shot = 20,

};
*/

