
#include "includes.h"


// Put pixel.
void plot(s32 x, s32 y)
{
	if ((u32)x >= SCR_Width || (u32)y >= SCR_Height) return;

	SDL_LockSurface(gVar.pScreen);
	u8	*pScr = gVar.pScreen->pixels;

	*(pScr + ((gVar.pScreen->pitch) * y) + x) = 0xFF;

	SDL_UnlockSurface(gVar.pScreen);

}

// Ligne H.
void lineH(s32 x1, s32 x2, s32 y)
{
	s32	t;

	if ((u32)y >= SCR_Height) return;
	if (x1 > x2) { t = x1; x1 = x2; x2 = t; }
	if (x1 >= SCR_Width || x2 < 0) return;

	if (x1 < 0) x1 = 0;
	if (x2 >= SCR_Width) x2 = SCR_Width - 1;

	SDL_LockSurface(gVar.pScreen);

	u8	*pScr = gVar.pScreen->pixels;
	for (t = x1; t <= x2; t++)
		*(pScr + ((gVar.pScreen->pitch) * y) + t) = 0xFF;

	SDL_UnlockSurface(gVar.pScreen);

}

// Ligne V.
void lineV(s32 x, s32 y1, s32 y2)
{
	s32	t;

	if ((u32)x >= SCR_Width) return;
	if (y1 > y2) { t = y1; y1 = y2; y2 = t; }
	if (y1 >= SCR_Height || y2 < 0) return;

	if (y1 < 0) y1 = 0;
	if (y2 >= SCR_Height) y2 = SCR_Height - 1;

	SDL_LockSurface(gVar.pScreen);

	u8	*pScr = gVar.pScreen->pixels;
	for (t = y1; t <= y2; t++)
		*(pScr + ((gVar.pScreen->pitch) * t) + x) = 0xFF;

	SDL_UnlockSurface(gVar.pScreen);

}

// Une position écran à partir d'un (x,y,z).
void ISO_ScreenPosition(s32 nPosX, s32 nPosY, s32 nPosZ, s32 *pnPosX, s32 *pnPosY)
{
	*pnPosX = (nPosX - nPosY) + gRoom.nDispOffsX;
	*pnPosY = ((nPosX + nPosY) / 2) + gRoom.nDispOffsY;
	*pnPosY += nPosZ;

}

// Load image secure.
SDL_Surface * LoadBMP(char *pFilename)
{
	SDL_Surface *pSurf;
	pSurf = SDL_LoadBMP(pFilename);
	if (pSurf == NULL)
	{
		fprintf(stderr, "LoadBMP(): Error: %s\n", SDL_GetError());
		exit(1);
	}
	return (pSurf);
}

