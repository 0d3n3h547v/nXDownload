#include <switch.h>
#include "includes/helper.h"

/* simple crash for SDL in case something went wrong */
void errorCrash(const char *text) {
	SDL_Quit();
	consoleInit(NULL);
	
	printf("%sError: %s%s\n", CONSOLE_RED, text, CONSOLE_RESET);
	printf("Press [HOME] to exit");
	
	while(appletMainLoop()) {
		consoleUpdate(NULL);
	}
	
	consoleExit(NULL);
	exit(0);
}

/*************** Initizialization ********************/
void SDL_Everything(void) {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	romfsInit();
	return;
}
void SDL_Cleanup(SDL_Window *window) {
	romfsExit();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return;
}

/************* Main Functions ******************/
SDL_Texture *LoadImageAsBackground(const char *path, SDL_Renderer *renderer)
{
	SDL_Surface *surface = NULL;
	surface = IMG_Load(path);
	
	SDL_Texture *texture = NULL;
	if ((texture = SDL_CreateTextureFromSurface(renderer, surface)) == NULL) errorCrash("Couldn't load image");
	SDL_FreeSurface(surface);
	
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	return texture;
}

void flushCacheScreen(SDL_Renderer *renderer, SDL_Texture *texture) {
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	return;
}

void DrawText(SDL_Renderer *renderer, TTF_Font *font, char *text, int h, int w, SDL_Color color) {
	SDL_Surface *surface = NULL;
	if ((surface = TTF_RenderText_Blended_Wrapped(font, text, color, 1280)) == NULL) errorCrash("Rendering text (DrawText)");
	SDL_SetSurfaceAlphaMod(surface, color.a);

	SDL_Texture *texture = NULL;
	if ((texture = SDL_CreateTextureFromSurface(renderer, surface)) == NULL) errorCrash("Creating texture (DrawText)");
	SDL_FreeSurface(surface);

	SDL_Rect position;
	position.x = w; position.y = h;
	SDL_QueryTexture(texture, NULL, NULL, &position.w, &position.h);
	SDL_RenderCopy(renderer, texture, NULL, &position);
	return;
}