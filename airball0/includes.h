// <> Smartphones are lame! <>

// Includes.
#include <stdlib.h>
#include <unistd.h>
#include "SDL.h"
#include <math.h>
#include <time.h>
#include <assert.h>

#include "ctypes.h"

#include "animspr.h"
#include "anims.h"
#include "frame.h"
#include "render.h"
#include "font.h"
#include "menu.h"
#include "game.h"
#include "room.h"
#include "hero.h"
#include "psd.h"
#include "sfx.h"


// Macros.
#define	ABS(x)	((x) < 0 ? -(x) : (x))
#define	SGN(x)	((x) >= 0 ? 1 : -1)
#define	MIN(x,y) ((x) < (y) ? (x) : (y))
#define NBELEM(tab) (sizeof(tab) / sizeof(tab[0]))

// Define.
#define SCR_Width 320
#define SCR_Height 240

#define	TXTPAL_NB	4//8//4

// Types de variables.
struct SGene
{
	SDL_Surface *pScreen;	// Ptr sur le buffer écran.

	SDL_Surface	*pBlk;		// Les blocs en vue ISO.
	SDL_Surface	*pTreasures;	// Les blocs de cadeaux.
	SDL_Surface	*pWalls;	// Les murs.
	SDL_Surface	*pBall;		// La bulle.
	SDL_Surface	*pTitle;	// Title screen.
	SDL_Surface	*pMarble;	// Fond marbre.
	SDL_Surface	*pFont16;	// Fonte 16x16.

	u8	*pKeysSDL;			// Pointeur sur le buffer clavier de la SDL.
	u8	pKeys[SDLK_LAST];	// Buffer clavier.

	SDL_Joystick	*pJoystick;
	u16	nJoystickState;		// 8 bits pour la croix (la fct de Hat renvoie un u8) et les boutons sur les 8 bits de poids fort.
	u8	nJoystickNoHat;		// Pas de croix ?
	u8	nJoystickNoAxes;	// Pas de stick analogique ?


	SDL_Color	pColorsLog[256];	// Palette logique.
	SDL_Color	pColorsPhy[256];	// Palette physique.

//	SDL_Color	pColors[256];	// Palette générale, à réinitialiser au changement de mode.
//	SDL_Color	pSprColors[256 - SPR_Palette_Idx];	// Palette des sprites.

//	SDL_Color	pCurPal[256];	// Palette lors des fades, pour le Scale 2x en 32 BPP.

	s32	nFadeVal;		// 0 = Noir / 256 = Couleurs finales.

};

// Variables générales.
extern struct SGene gVar;


