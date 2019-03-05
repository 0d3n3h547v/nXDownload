#include <stdio.h>
#include <stdlib.h>
#include <switch.h>
#include "includes/helper.h"

SDL_Color WHITE = {255, 255, 255, 255};
SDL_Color BLACK = {0, 0, 0, 255};
SDL_Color RED   = {255, 0, 0, 255};
SDL_Color GREEN = {0, 255, 0, 255};
SDL_Color BLUE  = {0, 0, 255, 255};

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Surface *surface = NULL;

TTF_Font *font = NULL;

int main(int argc, char **argv) {
	SDL_Everything();

	// Create an SDL window & renderer
	window = SDL_CreateWindow("main", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	// Background
	texture = LoadImageAsBackground("romfs:/images/background.png", renderer);
	font = TTF_OpenFont("romfs:/fonts/OpenSans_Regular.ttf", 25);
	
	while (appletMainLoop())
	{
		hidScanInput();
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);

		if (kDown & KEY_PLUS) break;
		
		flushCacheScreen(renderer, texture);
		if (kHeld & KEY_A) DrawText(renderer, font, "hello", 100, 100, BLACK);
	}
	
	SDL_Cleanup(window);
	return (EXIT_SUCCESS);
	
}