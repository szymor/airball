
#include "includes.h"
#include "font8x8.h"

// itoa.
void MyItoA(s32 nNb, char *pDst)
{
	char	cMin = ' ';
	char	*pPtr;
	u32	nTmp;

	// Cas des nombres négatifs.
	if (nNb < 0)
	{
		cMin = '-';
		nNb = -nNb;
	}

	pPtr = pDst + strlen(pDst) - 1;
	nTmp = nNb;
	do
	{
		*pPtr-- = (char)((nTmp % 10) + '0');
	} while (pPtr >= pDst && (nTmp /= 10) > 0);

	// Négatifs.
	if (cMin != ' ' && pPtr >= pDst) *pPtr = cMin;

}

/*
// itoa.
void MyItoA(u32 nNb, char *pDst)
{
	char	*pPtr;
	u32	nTmp;

	pPtr = pDst + strlen(pDst) - 1;
	nTmp = nNb;
	do
	{
		*pPtr-- = (char)((nTmp % 10) + '0');
	} while (pPtr >= pDst && (nTmp /= 10) > 0);

}
*/

// Affichage avec une fonte 8x8.
// !!! Attention, ce n'est pas prévu pour afficher en dehors de l'écran !!!
void FontPrint(s32 nPosX, s32 nPosY, char *pStr)
{
	u32	nOffFnt;
	u32	x, y;
	char	cChr;
	u8	*pScr;//, *pFnt;
	s32	nScrPitch;//, nFntPitch;

	pScr = (u8 *)gVar.pScreen->pixels;
	nScrPitch = gVar.pScreen->pitch;

	SDL_LockSurface(gVar.pScreen);

	while (*pStr)
	{
		cChr = *pStr++;
		if (cChr != ' ')
		{
			nOffFnt = (((u32)cChr) - ' ') << 6;
			for (y = 0; y < 8; y++)
			for (x = 0; x < 8; x++)
			{
				if (gpFont8[nOffFnt + (y * 8) + x])
					*(pScr + ((nPosY + y) * nScrPitch) + nPosX + x) = 0xFF;
			}
		}
		nPosX += 8;
	}

	SDL_UnlockSurface(gVar.pScreen);

}



