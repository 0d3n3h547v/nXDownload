#ifndef HELPER_H
#define HELPER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

/* simple crash for SDL in case something went wrong */
void errorCrash(const char *text);

/*************** Initizialization ********************/
void SDL_Everything(void);
void SDL_Cleanup(SDL_Window *window);

/************* Main Functions ******************/
SDL_Texture *LoadImageAsBackground(const char *path, SDL_Renderer *renderer);
void flushCacheScreen(SDL_Renderer *renderer, SDL_Texture *texture);
void DrawText(SDL_Renderer *renderer, TTF_Font *font, char *text, int h, int w, SDL_Color color);

#endif