
#define	SPR_NoSprite	-2

// Codes de contrôle.
#define	BIT31	(1 << 31)
enum
{
	e_Anm_Jump		= BIT31 | 1,	// Ptr + offset. e_Anm_Jump, offset.
	e_Anm_Goto		= BIT31 | 2,	// Initialise une autre anim. e_Anm_Goto, adresse.
	e_Anm_End		= BIT31 | 3,	// Fin d'une anim. Renvoie SPR_NoSprite, place e_AnmFlag_End, ne libère pas le slot.
	e_Anm_Kill		= BIT31 | 4,	// Fin d'une anim. Renvoie -1 et libère le slot (ex: dust).
	e_Anm_Sfx		= BIT31 | 5,	// Joue un son. e_Anm_Sfx, No sfx, Prio sfx.
	e_Anm_StepFlag	= BIT31 | 6,	// Flag pour laisser des traces de pas.
	e_Anm_RndGoto	= BIT31 | 7,	// Goto aléatoire. e_Anm_RndGoto, proba (sur 255), adr si <, adr si >.
	e_Anm_RndJump	= BIT31 | 8,	// Jump aléatoire. e_Anm_RndJump, proba (sur 255), offs si <, offs si >.
	e_Anm_SfxStop	= BIT31 | 9,	// Stoppe un son. e_Anm_SfxStop, No sfx.

};

// Clefs d'anim.	16b = Priorité (à faire) | 16b = No.
#define	ANMPRIO(x)	(x << 16)
enum
{
	e_AnmKey_Null			= 0,

	e_AnmKey_Hero_Idle	= 1,
	e_AnmKey_Hero_Fall	= 2,//ANMPRIO(1) + 1,
	e_AnmKey_Hero_Land	= ANMPRIO(1) + 1,
	e_AnmKey_Hero_Death	= ANMPRIO(3) + 1,


/*
	e_AnmKey_Hero_Stance	= 1,
	e_AnmKey_Hero_Run		= 2,
	e_AnmKey_Hero_CrouchStance	= 3,
	e_AnmKey_Hero_CrouchWalk	= 4,
	e_AnmKey_Hero_Jump		= 5,
	e_AnmKey_Hero_Shot		= ANMPRIO(1) + 6,
	e_AnmKey_Hero_ShotEnd	= ANMPRIO(1) + 7,
	e_AnmKey_Hero_ThrowHG	= 8,
	e_AnmKey_Hero_Knife		= ANMPRIO(2) + 8,
	e_AnmKey_Hero_KnifeEnd	= ANMPRIO(1) + 10,
	e_AnmKey_Hero_SlugOut	= ANMPRIO(1) + 9,
	e_AnmKey_Hero_Death		= ANMPRIO(3) + 1,
	e_AnmKey_Hero_DeathAir	= ANMPRIO(3) + 2,
	e_AnmKey_Hero_RunStop			= 20,
	e_AnmKey_Hero_CrouchWalkStop	= 21,
	e_AnmKey_Hero_UpDown			= 22,
	e_AnmKey_Hero_Victory	= ANMPRIO(4) + 1,
	e_AnmKey_Hero_Bored		= 30,

	e_AnmKey_HeroSub_Stance	= 11,
	e_AnmKey_HeroSub_Run	= 12,

	e_AnmKey_Enemy_Flip	= 1,
	e_AnmKey_Enemy_Shot	= ANMPRIO(1) + 2,
	e_AnmKey_Enemy_Key3	= 3,
	e_AnmKey_Enemy_Strike	= ANMPRIO(1) + 4,
*/

};


// Définition des anims.

extern u64 gAnm_Hero_Idle[];
extern u64 gAnm_Hero_Walk[];
extern u64 gAnm_Hero_Fall[];
extern u64 gAnm_Hero_Land[];

extern u64 gAnm_Hero_Death[];



