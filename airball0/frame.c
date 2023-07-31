
#include "includes.h"

#define FPS_Default (1000 / FPS_1S)
//#define FPS_Default 1000 / 60//70
u32	gnTimer1;
u32	gnFrame = 0;

// Init timers.
void FrameInit(void)
{
	gnTimer1 = SDL_GetTicks();
}

// Attente de la frame.
void FrameWait(void)
{
	u32 gnTimer2;

	// S'assurer qu'on ne va pas trop vite...
	while (1)
	{
		gnTimer2 = SDL_GetTicks() - gnTimer1;
		if (gnTimer2 >= FPS_Default) break;
		SDL_Delay(3);
	}
	gnTimer1 = SDL_GetTicks();
	gnFrame++;
}




