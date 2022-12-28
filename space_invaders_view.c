#include "SDL.h"

int main(void) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("unable to initialize: %s", SDL_GetError());
	}
	
	SDL_Window *window = SDL_CreateWindow("space invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
						 224 * 2, 256 * 2, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	if (window == NULL) {
		return 1;
	}
	SDL_SetWindowMinimumSize(window, 224, 256);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == NULL) {
		return 1;
	}

	SDL_RenderSetLogicalSize(renderer, 224, 256);
	SDL_RendererInfo renderer_info;
	SDL_GetRendererInfo(renderer, &renderer_info);
	SDL_Log("using renderer %s", renderer_info.name);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, 224, 256);
	if (texture == NULL) {
		SDL_Log("unable to create texture: %s", SDL_GetError());
		return 1;
	}
}