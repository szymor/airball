
#include "includes.h"


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

// Création des couleurs pour le texte.
void Font_CreateTxtColors(u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2, u8 nPalNo)
{
	u8	r, g, b;
	s32	i;
	u32	nPalIdx;

	for (i = 0; i < 8; i++)
	{
		r = r1 + (((s32)(r2 - r1) * i) / 7);
		g = g1 + (((s32)(g2 - g1) * i) / 7);
		b = b1 + (((s32)(b2 - b1) * i) / 7);
		nPalIdx = 256-18 - ((nPalNo + 1) * 8) + i;
		gVar.pColorsLog[nPalIdx].r = r;
		gVar.pColorsLog[nPalIdx].g = g;
		gVar.pColorsLog[nPalIdx].b = b;
	}

}

// Affichage avec une fonte de 16 pixels de large.
// !!! Attention, ce n'est pas prévu pour afficher en dehors de l'écran !!!
void FontPrint16(SDL_Surface *pDstSurf, SDL_Surface *pFont, s32 nPosX, s32 nPosY, char *pStr, u32 nPalNo)
{
	u32	nOffTmp, nOffTmpSh;
	u32	nOffFnt;
	u32	y;
	char	cChr;
	u8	*pDst, *pFnt;
	s32	nDstPitch, nFntPitch;
	u32	nClr;

	// Les couleurs de chaque ligne.
	u32	pClr[16];
	for (y = 0; y < 8; y++)
	{
		nClr = 256-18 - ((nPalNo + 1) * 8) + y;
		nClr = (nClr << 24) | (nClr << 16) | (nClr << 8) | nClr;
		pClr[y] = nClr;
		pClr[15 - y] = nClr;
	}

	pDst = (u8 *)pDstSurf->pixels;
	nDstPitch = pDstSurf->pitch;
	pFnt = (u8 *)pFont->pixels;
	nFntPitch = pFont->pitch;

	SDL_LockSurface(pDstSurf);

	nOffTmp = (nPosY * nDstPitch) + nPosX;
	nOffTmpSh = ((nPosY + 1) * nDstPitch) + nPosX - 1;
	while (*pStr)
	{
		cChr = *pStr++;
		if (cChr != ' ')
		{
			nOffFnt = (((u32)cChr) - ' ') << 4;
			for (y = 0; y < pFont->h; y++)
			{
				nClr = pClr[y];
				// L'ombre.
				*(u32 *)(pDst + nOffTmpSh + (y * nDstPitch)) &= ~*(u32 *)(pFnt + nOffFnt + (y * nFntPitch));
				*(u32 *)(pDst + nOffTmpSh + (y * nDstPitch) + 4) &= ~*(u32 *)(pFnt + nOffFnt + (y * nFntPitch) + 4);
				*(u32 *)(pDst + nOffTmpSh + (y * nDstPitch) + 8) &= ~*(u32 *)(pFnt + nOffFnt + (y * nFntPitch) + 8);
				*(u32 *)(pDst + nOffTmpSh + (y * nDstPitch) + 12) &= ~*(u32 *)(pFnt + nOffFnt + (y * nFntPitch) + 12);
				// La couleur.
				*(u32 *)(pDst + nOffTmp + (y * nDstPitch)) = ( *(u32 *)(pDst + nOffTmp + (y * nDstPitch)) & ~*(u32 *)(pFnt + nOffFnt + (y * nFntPitch)) ) | ( *(u32 *)(pFnt + nOffFnt + (y * nFntPitch)) & nClr );
				*(u32 *)(pDst + nOffTmp + (y * nDstPitch) + 4) = ( *(u32 *)(pDst + nOffTmp + (y * nDstPitch) + 4) & ~*(u32 *)(pFnt + nOffFnt + (y * nFntPitch) + 4) ) | ( *(u32 *)(pFnt + nOffFnt + (y * nFntPitch) + 4) & nClr );
				*(u32 *)(pDst + nOffTmp + (y * nDstPitch) + 8) = ( *(u32 *)(pDst + nOffTmp + (y * nDstPitch) + 8) & ~*(u32 *)(pFnt + nOffFnt + (y * nFntPitch) + 8) ) | ( *(u32 *)(pFnt + nOffFnt + (y * nFntPitch) + 8) & nClr );
				*(u32 *)(pDst + nOffTmp + (y * nDstPitch) + 12) = ( *(u32 *)(pDst + nOffTmp + (y * nDstPitch) + 12) &~ *(u32 *)(pFnt + nOffFnt + (y * nFntPitch) + 12) ) | ( *(u32 *)(pFnt + nOffFnt + (y * nFntPitch) + 12) & nClr );
			}
		}
		nOffTmp += 16;
		nOffTmpSh += 16;
	}

	SDL_UnlockSurface(pDstSurf);

}


