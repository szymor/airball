// <> Smartphones are lame! <>

// Includes.
#include <stdlib.h>
#include <unistd.h>
#include "SDL.h"
#include <math.h>
#include <time.h>
#include <assert.h>

#include "ctypes.h"

#include "frame.h"
#include "render.h"
#include "preca.h"
#include "font.h"
#include "rooms.h"
#include "editor.h"
#include "gfx.h"


// Macros.
#define	ABS(x)	((x) < 0 ? -(x) : (x))
#define	SGN(x)	((x) >= 0 ? 1 : -1)
#define	MIN(x,y) ((x) < (y) ? (x) : (y))
#define NBELEM(tab) (sizeof(tab) / sizeof(tab[0]))

// Define.
#define SCR_Width 320
#define SCR_Height 300//240


#define	GFX_BLK_W	32
#define	GFX_BLK_H	40

#define	MAP_TILEW	16
#define	MAP_TILEH	16
#define	MAP_TILEZ	16


// Types de variables.
struct SGene
{
	SDL_Surface *pScreen;	// Ptr sur le buffer écran.

	SDL_Surface	*pBlk;		// Les blocs en vue ISO.
	SDL_Surface	*pWalls;	// Les murs.
	SDL_Surface	*pEdStuff;	// Gfx divers pour l'édition.

	u8	*pKeys;		// Buffer clavier.

	SDL_Color	pColors[256];	// Palette générale, à réinitialiser au changement de mode.

	SDL_Color	pCurPal[256];	// Palette lors des fades, pour le Scale 2x en 32 BPP.

	s16	*pSin;			// Ptrs sur les tables.
	s16	*pCos;

	s32	nFadeVal;		// 0 = Noir / 256 = Couleurs finales.

	u16	nMousePosX, nMousePosY;
	u16	nMouseButtons;

};
#define	MOUSE_LMB	(1)
#define	MOUSE_RMB	(1 << 1)
#define	MOUSE_WHEELUP	(1 << 2)
#define	MOUSE_WHEELDOWN	(1 << 3)


// Variables générales.
extern struct SGene gVar;


