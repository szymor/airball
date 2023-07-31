#include "includes.h"


// 1 u32/u64 : 1 clef.
// Ensuite :
// 1 uu32/64 : Nb de frames d'affichage
//  Si b31, code de contrôle : Voir liste en haut de anims.h.
// 1 u32/u64 : N° du sprite.

/*
// Macro pour le clignotement de mort.
#define	ANIM_CLIGNO(sprno)	\
6, SPR_NoSprite,	\
6, sprno,	\
6, SPR_NoSprite,	\
6, sprno,	\
6, SPR_NoSprite,	\
6, sprno,	\
6, SPR_NoSprite,	\
6, sprno,

// Macro pour jouer un son ou l'autre.
#define	ANIM_SFXRND(proba, sfx1no, sfx1prio, sfx2no, sfx2prio)	\
e_Anm_RndJump, proba, 4+0, 4+5,	\
e_Anm_Sfx, sfx1no, sfx1prio,	\
e_Anm_Jump, 5,					\
e_Anm_Sfx, sfx2no, sfx2prio,
*/

// Idle.
u64 gAnm_Hero_Idle[] =
{
e_AnmKey_Hero_Idle,
128, 0,
e_Anm_Goto, (u64)gAnm_Hero_Idle
};

// Roule.
#define	BALL_WALK_T0	5
u64 gAnm_Hero_Walk[] =
{
e_AnmKey_Hero_Idle,
BALL_WALK_T0, 0,
BALL_WALK_T0, 1,
BALL_WALK_T0, 2,
BALL_WALK_T0, 1,
e_Anm_Goto, (u64)gAnm_Hero_Walk
};

// Fall.
u64 gAnm_Hero_Fall[] =
{
e_AnmKey_Hero_Fall,
128, 0,
e_Anm_Goto, (u64)gAnm_Hero_Fall
};

// Atterrissage.
#define	BALL_WALK_T1	2
u64 gAnm_Hero_Land[] =
{
e_AnmKey_Hero_Land,
BALL_WALK_T1, 0,
BALL_WALK_T1, 1,
BALL_WALK_T1, 2,
BALL_WALK_T1, 1,
e_Anm_Goto, (u64)gAnm_Hero_Idle
};



// Mort.
#define	BALL_DEATH_T1	3
u64 gAnm_Hero_Death[] =
{
e_AnmKey_Hero_Death,
e_Anm_Sfx, e_Sfx_Game_Death, 200,
BALL_DEATH_T1, 7,    BALL_DEATH_T1, 8,    BALL_DEATH_T1, 9,
BALL_DEATH_T1, 10,   BALL_DEATH_T1, 11,   BALL_DEATH_T1, 12,   BALL_DEATH_T1, 13,   BALL_DEATH_T1, 14,
BALL_DEATH_T1, 15,   BALL_DEATH_T1, 16,   BALL_DEATH_T1, 17,   BALL_DEATH_T1, 18,   BALL_DEATH_T1, 19,
BALL_DEATH_T1, 20,   BALL_DEATH_T1, 21,   BALL_DEATH_T1, 22,   BALL_DEATH_T1, 23,
e_Anm_End
};


/*
// Nage - Repos (legs).
u64	gAnm_HeroSub_Stance[] =
{
e_AnmKey_HeroSub_Stance,
12, e_Spr_HeroSubLegs_Run,
12, e_Spr_HeroSubLegs_Run + 1,
12, e_Spr_HeroSubLegs_Run,
//12, e_Spr_HeroSubLegs_Run + 2,
//12, e_Spr_HeroSubLegs_Run + 1,
12, e_Spr_HeroSubLegs_Run + 5,
12, e_Spr_HeroSubLegs_Run + 6,
//12, e_Spr_HeroSubLegs_Run + 7,
//12, e_Spr_HeroSubLegs_Run + 6,
12, e_Spr_HeroSubLegs_Run + 5,
e_Anm_Goto, (u64)gAnm_HeroSub_Stance
};
// Nage - Avance (legs).
u64	gAnm_HeroSub_Run[] =
{
e_AnmKey_HeroSub_Run,
4, e_Spr_HeroSubLegs_Run,
4, e_Spr_HeroSubLegs_Run + 1,
4, e_Spr_HeroSubLegs_Run + 2,
4, e_Spr_HeroSubLegs_Run + 3,
4, e_Spr_HeroSubLegs_Run + 4,
//4, e_Spr_HeroSubLegs_Run + 3,
4, e_Spr_HeroSubLegs_Run + 2,
4, e_Spr_HeroSubLegs_Run + 1,
4, e_Spr_HeroSubLegs_Run,
4, e_Spr_HeroSubLegs_Run + 5,
4, e_Spr_HeroSubLegs_Run + 6,
4, e_Spr_HeroSubLegs_Run + 7,
4, e_Spr_HeroSubLegs_Run + 8,
4, e_Spr_HeroSubLegs_Run + 9,
//4, e_Spr_HeroSubLegs_Run + 8,
4, e_Spr_HeroSubLegs_Run + 7,
4, e_Spr_HeroSubLegs_Run + 6,
4, e_Spr_HeroSubLegs_Run + 5,
e_Anm_Goto, (u64)gAnm_HeroSub_Run
};
// Nage - Mort générique.
u64	gAnm_HeroSub_DeathGen[] =
{
e_AnmKey_Hero_Death,
e_Anm_Sfx, e_Sfx_Death_Scream1, e_SfxPrio_Hero_Shot + 10,
4, e_Spr_HeroSub_DeathGen,      4, e_Spr_HeroSub_DeathGen + 1,  4, e_Spr_HeroSub_DeathGen + 2,
4, e_Spr_HeroSub_DeathGen + 3,  4, e_Spr_HeroSub_DeathGen + 4,  4, e_Spr_HeroSub_DeathGen + 5,
4, e_Spr_HeroSub_DeathGen + 6,  4, e_Spr_HeroSub_DeathGen + 7,  4, e_Spr_HeroSub_DeathGen + 8,
4, e_Spr_HeroSub_DeathGen + 9,
4, e_Spr_HeroSub_DeathGen + 10, 4, e_Spr_HeroSub_DeathGen + 11, 4, e_Spr_HeroSub_DeathGen + 12,
4, e_Spr_HeroSub_DeathGen + 13, 4, e_Spr_HeroSub_DeathGen + 14, 4, e_Spr_HeroSub_DeathGen + 15,
4, e_Spr_HeroSub_DeathGen + 16, 4, e_Spr_HeroSub_DeathGen + 17, 4, e_Spr_HeroSub_DeathGen + 18,
4, e_Spr_HeroSub_DeathGen + 19,
4, e_Spr_HeroSub_DeathGen + 20, 4, e_Spr_HeroSub_DeathGen + 21, 4, e_Spr_HeroSub_DeathGen + 22,
ANIM_CLIGNO(e_Spr_HeroSub_DeathGen + 22)
e_Anm_End
};
*/


