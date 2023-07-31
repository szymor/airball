

#define	HERO_AIR_MAX	(150)

#pragma pack(1)
struct SHero
{
	s32	nPosX, nPosY, nPosZ;
	s32	nSpdZ;
	s32	nSpd;
	s32	nJumpPosZOrg;		// Z original au moment du départ du saut.
	u8	nGnd;
	s8	nAnm;

	u8	nObject;			// Objet porté. (No du bloc).

	u8	nLives;				// Nombre de vies.
	u16	nAirLevel;			// Niveau d'air. (8 bits de VF).

	u8	nJumpKb;			// Etat du clavier pour le saut.
	u8	nKbSav;				// Sauvegarde du kb pour les sauts devant les portes.
	s32	nJumpAcc;			// Accumulateur pour avance du saut.

//	u32	nScore;

	u32	nTime;				// Temps de jeu.
	u16	nTimeStop;			// Si != 0 => stoppe le timer.
	u32	nObjTime;			// Temps pour high-scores, on note quand on ramène un nouvel objet.
	u8	nObjFound;			// Nb d'objets trouvés. Si b7 => Tout trouvé.

};
#pragma pack()
extern struct SHero	gHero;


void Hero_Init(void);
void Hero_LifeInit(void);
void Hero_Move(void);


