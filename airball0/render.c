
//
// Routines d'affichage : 1x / 2x / TV2x.
//
// 8 BPP en mode x1, 16 ou 32 BPP en mode 2x et TV2x.
// En 8 BPP, on passe en 16 BPP pour le mode TV2x (pas trop le choix).
//

#include "includes.h"

#if defined (__LINUX__) || defined (__APPLE__)
#define	BPP32_2X	1		// Useless with Windows - Vital with Linux.
#elif defined (__AMIGA__)
#define	BPP16_2X	1		// Amiga OS.
#endif

//#define	BPP32_2X	1		// Useless with Windows - Vital with Linux.
//#define	BPP16_2X	1		// Amiga OS.

struct SRender	gRender;

//=================
#define	TV2x_FACTOR	200		// * factor, / 256
u8	gTV256Shades[256];

// Précalcul des valeurs ombrées des composantes.
void TVShadesPrecalculation(void)
{
	u32	i;
	for (i = 0; i < 256; i++)
		gTV256Shades[i] = (i * TV2x_FACTOR) / 256;
}
//=================

#if defined BPP32_2X
// TV2x. 8 > 32 BPP.
void Render_TV2x(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	// Conversion de la palette courante.
	u32	pPal[256];
	u32	pPal2[256];
	u32	i;
	for (i = 0; i < 256; i++)
	{
		pPal[i] = SDL_MapRGB(pSDL_Dst->format, gVar.pColorsPhy[i].r, gVar.pColorsPhy[i].g, gVar.pColorsPhy[i].b);
		pPal2[i] = SDL_MapRGB(pSDL_Dst->format, gTV256Shades[gVar.pColorsPhy[i].r], gTV256Shades[gVar.pColorsPhy[i].g], gTV256Shades[gVar.pColorsPhy[i].b]);
	}

	// 2x.
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels;
	u32	nClr;

	u8	*pSrc2;
	u32	*pDst2a, *pDst2b;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = pSrc;
		pDst2a = (u32 *)pDst;
		pDst2b = (u32 *)(pDst + pSDL_Dst->pitch);
		for (x = 0; x < SCR_Width; x++)
		{
			nClr = pPal[*pSrc2];
			*pDst2a++ = nClr;
			*pDst2a++ = nClr;
			nClr = pPal2[*pSrc2++];
			*pDst2b++ = nClr;
			*pDst2b++ = nClr;
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch * 2;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}

// Scaling 2x. 8 > 32 BPP.
void Render_Scale2x(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	// Conversion de la palette courante.
	u32	pPal[256];
	u32	i;
	for (i = 0; i < 256; i++)
		pPal[i] = SDL_MapRGB(pSDL_Dst->format, gVar.pColorsPhy[i].r, gVar.pColorsPhy[i].g, gVar.pColorsPhy[i].b);

	// 2x.
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels;
	u32	nClr;

	u8	*pSrc2;
	u32	*pDst2a, *pDst2b;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = pSrc;
		pDst2a = (u32 *)pDst;
		pDst2b = (u32 *)(pDst + pSDL_Dst->pitch);
		for (x = 0; x < SCR_Width; x++)
		{
			nClr = pPal[*pSrc2++];
			*pDst2a++ = nClr;
			*pDst2a++ = nClr;
			*pDst2b++ = nClr;
			*pDst2b++ = nClr;
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch * 2;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}
#elif defined BPP16_2X
// Scaling 2x. 8 > 16 BPP.
void Render_Scale2x(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	// Conversion de la palette courante.
	u32	pPal[256];
	u32	i;
	u32	nClr;
	for (i = 0; i < 256; i++)
	{
		nClr = SDL_MapRGB(pSDL_Dst->format, gVar.pColorsPhy[i].r, gVar.pColorsPhy[i].g, gVar.pColorsPhy[i].b);
		nClr = (nClr << 16) | nClr;
		pPal[i] = nClr;
	}

	// 2x.
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels;

	u8	*pSrc2;
	u32	*pDst2a, *pDst2b;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = pSrc;
		pDst2a = (u32 *)pDst;
		pDst2b = (u32 *)(pDst + pSDL_Dst->pitch);
		for (x = 0; x < SCR_Width; x++)
		{
			nClr = pPal[*pSrc2++];
			*pDst2a++ = nClr;
			*pDst2b++ = nClr;
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch * 2;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}
#else
// Scaling 2x. 8 BPP.
void Render_Scale2x(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels;
	u16	nClr;

	u8	*pSrc2;
	u16	*pDst2a, *pDst2b;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = (u8 *)pSrc;
		pDst2a = (u16 *)pDst;
		pDst2b = (u16 *)(pDst + pSDL_Dst->pitch);
		for (x = 0; x < SCR_Width; x++)
		{
			nClr = *pSrc2++;
			nClr = nClr | (nClr << 8);
			*pDst2a++ = nClr;
			*pDst2b++ = nClr;
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch * 2;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}
#endif

#if !defined BPP32_2X
// TV2x. 8 > 16 BPP.
void Render_TV2x(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst)
{
	// Conversion de la palette courante.
	u32	pPal[256];
	u32	pPal2[256];
	u32	i;
	u32	nClr;
	for (i = 0; i < 256; i++)
	{
		nClr = SDL_MapRGB(pSDL_Dst->format, gVar.pColorsPhy[i].r, gVar.pColorsPhy[i].g, gVar.pColorsPhy[i].b);
		nClr = (nClr << 16) | nClr;
		pPal[i] = nClr;
		nClr = SDL_MapRGB(pSDL_Dst->format, gTV256Shades[gVar.pColorsPhy[i].r], gTV256Shades[gVar.pColorsPhy[i].g], gTV256Shades[gVar.pColorsPhy[i].b]);
		nClr = (nClr << 16) | nClr;
		pPal2[i] = nClr;
	}

	// 2x.
	SDL_LockSurface(pSDL_Src);
	SDL_LockSurface(pSDL_Dst);

	u32	y, x;
	u8	*pSrc = pSDL_Src->pixels;
	u8	*pDst = pSDL_Dst->pixels;

	u8	*pSrc2;
	u32	*pDst2a, *pDst2b;

	for (y = 0; y < SCR_Height; y++)
	{
		pSrc2 = pSrc;
		pDst2a = (u32 *)pDst;
		pDst2b = (u32 *)(pDst + pSDL_Dst->pitch);
		for (x = 0; x < SCR_Width; x++)
		{
			nClr = pPal[*pSrc2];
			*pDst2a++ = nClr;
			nClr = pPal2[*pSrc2++];
			*pDst2b++ = nClr;
		}
		pSrc += pSDL_Src->pitch;
		pDst += pSDL_Dst->pitch * 2;
	}

	SDL_UnlockSurface(pSDL_Src);
	SDL_UnlockSurface(pSDL_Dst);
}
#endif

typedef void (*pRenderFct)(SDL_Surface *pSDL_Src, SDL_Surface *pSDL_Dst);
// Rendu + Flip.
void RenderFlip(u32 nSync)
{
	static pRenderFct	pFctTb[e_RenderMode_MAX] = { NULL, Render_Scale2x, Render_TV2x };

	if (pFctTb[gRender.nRenderMode] != NULL) pFctTb[gRender.nRenderMode](gVar.pScreen, gRender.pScreen2x);

	if (nSync) FrameWait();
//	SDL_Flip(gRender.nRenderMode == e_RenderMode_Normal ? gVar.pScreen : gRender.pScreen2x);
	SDL_UpdateRect((gRender.nRenderMode == e_RenderMode_Normal ? gVar.pScreen : gRender.pScreen2x), 0, 0, 0, 0);

	// Palette ?
	if (gRender.nPalFlip)
	{
		SDL_SetPalette((gRender.nRenderMode == e_RenderMode_Normal ? gVar.pScreen : gRender.pScreen2x), SDL_PHYSPAL, gVar.pColorsPhy, 0, 256);		// Palette PHYSIQUE de la surface écran réelle.
		gRender.nPalFlip = 0;
	}

}

// Set video mode.
SDL_Surface * VideoModeSet(u32 nScrWidth, u32 nScrHeight, u32 nBPP, u32 nSDL_Flags)
{
	SDL_Surface *pSurf;

	pSurf = SDL_SetVideoMode(nScrWidth, nScrHeight, nBPP, SDL_SWSURFACE | nSDL_Flags);
	if (pSurf == NULL)
	{
		fprintf(stderr, "VideoModeSet(): Couldn't set video mode: %sn",SDL_GetError());
		//exit(1);
	}
	else
		SDL_FillRect(pSurf, NULL, 0);		// Sous Windows, SDL_SetVideoMode génère un event SDL_VIDEOEXPOSE. Comme on gère F9 dans les events, ça évite un flash si 0 n'est pas la couleur noire. (Faire le FillRect avec le noir, évidement).
	return (pSurf);
}

// Met le mode video qui va bien.
void Render_SetVideoMode(void)
{
	switch (gRender.nRenderMode)
	{
	case e_RenderMode_TV2x:
	case e_RenderMode_Scale2x:
#if defined (BPP32_2X)
		gRender.pScreen2x = VideoModeSet(SCR_Width * 2, SCR_Height * 2, 32, (gRender.nFullscreenMode ? SDL_FULLSCREEN : 0));
#elif defined (BPP16_2X)
		gRender.pScreen2x = VideoModeSet(SCR_Width * 2, SCR_Height * 2, 16, (gRender.nFullscreenMode ? SDL_FULLSCREEN : 0));
#else
		gRender.pScreen2x = VideoModeSet(SCR_Width * 2, SCR_Height * 2, (gRender.nRenderMode == e_RenderMode_TV2x ? 16 : 8), (gRender.nFullscreenMode ? SDL_FULLSCREEN : 0));
#endif

		gVar.pScreen = gRender.pScreenBuf2;
//		if (gRender.pScreen2x != NULL) return;		// Ok.
		if (gRender.pScreen2x != NULL)		// Ok.
		{
			SDL_SetPalette(gRender.pScreen2x, SDL_LOGPAL, gVar.pColorsLog, 0, 256);
			SDL_SetPalette(gRender.pScreen2x, SDL_PHYSPAL, gVar.pColorsPhy, 0, 256);
			SDL_SetPalette(gRender.pScreenBuf2, SDL_LOGPAL, gVar.pColorsLog, 0, 256);
			return;
 		}
		// Erreur => On repasse en mode Normal et Windowed.
		gRender.nRenderMode = e_RenderMode_Normal;
		gRender.nFullscreenMode = 0;
		// ... et pas de break.
	case e_RenderMode_Normal:
	default:
		gVar.pScreen = VideoModeSet(SCR_Width, SCR_Height, 8, (gRender.nFullscreenMode ? SDL_FULLSCREEN : 0));
		gRender.pScreen2x = NULL;
		if (gVar.pScreen == NULL) exit(1);	// Message d'erreur dans VideoModeSet.
		SDL_SetPalette(gVar.pScreen, SDL_LOGPAL, gVar.pColorsLog, 0, 256);
		SDL_SetPalette(gVar.pScreen, SDL_PHYSPAL, gVar.pColorsPhy, 0, 256);
		break;
	}

}

// Renvoie un ptr sur la surface écran réelle (pour les palettes).
SDL_Surface * Render_GetRealVideoSurfPtr(void)
{
	return (gRender.nRenderMode == e_RenderMode_Normal ? gVar.pScreen : gRender.pScreen2x);
}

// Init de la vidéo.
void Render_InitVideo(void)
{
	gRender.nRenderMode = e_RenderMode_Normal;
	gRender.nFullscreenMode = 0;

	gRender.pScreen2x = NULL;		// En mode 2x, ptr sur la surface écran.
	gRender.pScreenBuf2 = NULL;		// Buffer de rendu pour le jeu en mode 2x (à la place de la surface écran réelle).

	// On initialise d'abord un écran en mode e_RenderMode_Normal. Important, car on fait un CreateRGBSurface à partir de cette surface.
	gVar.pScreen = VideoModeSet(SCR_Width, SCR_Height, 8, gRender.nFullscreenMode ? SDL_FULLSCREEN : 0);
	if (gVar.pScreen == NULL) exit(1);
	// On créé un buffer de la taille de l'écran.
	// => En mode 2x, on switche le ptr pScreen sur cette surface, les rendus du jeu se font donc dedans. Puis on fait le scale/filtre du buffer vers la vraie surface écran.
	gRender.pScreenBuf2 = SDL_CreateRGBSurface(SDL_SWSURFACE, SCR_Width, SCR_Height, 8, gVar.pScreen->format->Rmask, gVar.pScreen->format->Gmask, gVar.pScreen->format->Bmask, 0);
	if (gRender.pScreenBuf2 == NULL)
	{
		fprintf(stderr, "Render_InitVideo(): Unable to allocate SDL surface: %s\n", SDL_GetError());
		exit(1);
	}

	// Petit précalcul pour le mode TV2x.
	TVShadesPrecalculation();
}

// Libère les ressources du rendu. (1 fois !).
void RenderRelease(void)
{
	SDL_FreeSurface(gRender.pScreenBuf2);
}

