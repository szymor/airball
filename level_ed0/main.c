
// Airball editor.
// Code: 17o2!!
// Contact: Clement CORDE, c1702@yahoo.com

#include "includes.h"

// Variables générales.
struct SGene gVar;

// Gestionnaire d'évènements.
int EventHandler(u32 nInGame)
{
	SDL_Event event;

	gVar.nMouseButtons &= ~(MOUSE_WHEELUP | MOUSE_WHEELDOWN);	// Clear wheel.

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			// Toggle fullscreen/windowed.
			if (gVar.pKeys[SDLK_F10])
			{
				gRender.nFullscreenMode ^= 1;
				Render_SetVideoMode();
			}
			// Toggle render mode.
			if (gVar.pKeys[SDLK_F9])
			{
				if (++gRender.nRenderMode >= e_RenderMode_MAX) gRender.nRenderMode = 0;
				Render_SetVideoMode();
			}

			// Esc : Quitter.
//			if (gVar.pKeys[SDLK_ESCAPE]) return (1);			// Emergency exit.
			if (gVar.pKeys[SDLK_x]) return (1);			// Emergency exit.

			break;

		case SDL_KEYUP:
			break;

		case SDL_MOUSEMOTION:
			gVar.nMousePosX = event.motion.x;
			gVar.nMousePosY = event.motion.y;
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT) gVar.nMouseButtons |= MOUSE_LMB;
			if (event.button.button == SDL_BUTTON_RIGHT) gVar.nMouseButtons |= MOUSE_RMB;
			//
			if (event.button.button == SDL_BUTTON_WHEELUP) gVar.nMouseButtons |= MOUSE_WHEELUP;
			if (event.button.button == SDL_BUTTON_WHEELDOWN) gVar.nMouseButtons |= MOUSE_WHEELDOWN;
			break;

		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT) gVar.nMouseButtons &= ~MOUSE_LMB;
			if (event.button.button == SDL_BUTTON_RIGHT) gVar.nMouseButtons &= ~MOUSE_RMB;
			break;

		}
	}
	return (0);
}

/*
// Création de la palette :
// On recopie dans la palette générale la partie de palette correspondant au décor + la palette des sprites.
void SetPalette(SDL_Surface *pBkg, SDL_Color *pSprPal, u32 nSprPalIdx)
{
	u32	i;
	SDL_Color	*pSrcPal = pBkg->format->palette->colors;

	// Couleurs du décor.
	for (i = 0; i < nSprPalIdx; i++)
	{
		gVar.pColors[i] = pSrcPal[i];
	}
	// Couleurs des sprites.
	for (; i < 256; i++)
	{
		gVar.pColors[i] = pSprPal[i - SPR_Palette_Idx];
	}
	// Palette logique.
	SDL_SetPalette(gVar.pScreen, SDL_LOGPAL, gVar.pColors, 0, 256);

}
*/

// Fade.
void Fade(s32 nFadeVal)
{
	if (nFadeVal < 0) return;

	SDL_Color	*pSrc = gVar.pColors;
	u32	i, nNbColors;

	nNbColors = 256;
	if (nFadeVal > 255) nFadeVal = 255;
	for (i = 0; i < nNbColors; i++)
	{
		gVar.pCurPal[i].r = (pSrc->r * nFadeVal) / 255;
		gVar.pCurPal[i].g = (pSrc->g * nFadeVal) / 255;
		gVar.pCurPal[i].b = (pSrc->b * nFadeVal) / 255;
		pSrc++;
	}
	SDL_SetPalette(Render_GetRealVideoSurfPtr(), SDL_PHYSPAL, gVar.pCurPal, 0, nNbColors);		// Pour le fade, c'est bien la palette PHYSIQUE de la surface écran réelle.

}

//=====================================

#define	DATA_FILENAME	"rooms.bin"

// Point d'entrée.
int main(int argc, char *argv[])
{
	u32	i;

	// SDL Init.
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	// atexit : Quand on quittera (exit, return...), SDL_Quit() sera appelée.
	atexit(SDL_Quit);

	SDL_putenv("SDL_VIDEO_CENTERED=center");	// Center the window.
	// Video mode init.
	Render_InitVideo();
	SDL_WM_SetCaption("Airball Editor", NULL);	// Nom de la fenêtre.
	gRender.nRenderMode = e_RenderMode_Scale2x;
	Render_SetVideoMode();

	gVar.pKeys = SDL_GetKeyState(NULL);		// Init du ptr clavier.
	SDL_ShowCursor(SDL_DISABLE);	// Cache le pointeur de la souris.


	// Preca Sinus et Cosinus.
	PrecaSinCos();

	// Lecture des blocs.
	gVar.pBlk = LoadBMP("blocks.bmp");
	SDL_SetColorKey(gVar.pBlk, SDL_SRCCOLORKEY, SDL_MapRGB(gVar.pBlk->format, 255, 0, 255));
	gVar.pWalls = LoadBMP("room_walls2.bmp");
	SDL_SetColorKey(gVar.pWalls, SDL_SRCCOLORKEY, SDL_MapRGB(gVar.pWalls->format, 255, 0, 255));
	gVar.pEdStuff = LoadBMP("arrows1.bmp");
	SDL_SetColorKey(gVar.pEdStuff, SDL_SRCCOLORKEY, SDL_MapRGB(gVar.pEdStuff->format, 255, 0, 255));


	// Couleurs.
	for (i = 0; i < gVar.pBlk->format->palette->ncolors; i++)
	{
		gVar.pColors[i] = gVar.pBlk->format->palette->colors[i];
//		gVar.pColors[i].r = i;
//		gVar.pColors[i].g = i;
//		gVar.pColors[i].b = i;
	}
	// Palette logique.
	SDL_SetPalette(gVar.pScreen, SDL_LOGPAL, gVar.pColors, 0, 256);

	Fade(256);					// Force la palette physique.

	Editor_Init();
	Editor_FileLoad(DATA_FILENAME);

//	Room_Set(0);

	// Main loop.
	FrameInit();
	while (1)
	{
		// Gestion des évenements.
		if (EventHandler(1)) break;

		Editor_Keys();

		SDL_FillRect(gVar.pScreen, NULL, 0);	// Clear screen.
		Room_Display();
		Editor_ModeDisp();

//if (gVar.pKeys[SDLK_y]) { FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); FrameWait(); }

		RenderFlip(1);		// Wait for frame, Flip.
	}

	SDL_ShowCursor(SDL_ENABLE);		// Réautorise l'affichage du curseur de la souris.

	Room_FreeAll();

	SDL_FreeSurface(gVar.pBlk);
	SDL_FreeSurface(gVar.pWalls);
	SDL_FreeSurface(gVar.pEdStuff);

	// Libère les ressources de rendu.
	RenderRelease();

	return (0);
}



