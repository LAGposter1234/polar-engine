#include "render.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "font.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

int render_init(const char* title, int width, int height) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return 0;
	}

	window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN
	);

	if (!window) {
		fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 0;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer) {
		fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 0;
	}

	return 1;
}

void render_clear(void) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}

void render_present(void) {
	SDL_RenderPresent(renderer);
}

void render_quit(void) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int render_character(char c, Font font, int x, int y) {
	if (c < 32 || c > 126) return 0;

	int idx = c - 32;
	int size;
	const uint32_t (*data)[8] = NULL;

	switch (font) {
		case VGA8:   size = 8;  data = (const uint32_t(*)[8])font_vga_8;   break;
		case VGA16:  size = 16; data = (const uint32_t(*)[8])font_vga_16;  break;
		case VGA32:  size = 32; data = (const uint32_t(*)[8])font_vga_32;  break;
		case SANS8:  size = 8;  data = (const uint32_t(*)[8])font_sans_8;  break;
		case SANS16: size = 16; data = (const uint32_t(*)[8])font_sans_16; break;
		case SANS32: size = 32; data = (const uint32_t(*)[8])font_sans_32; break;
		case SERIF8:  size = 8;  data = (const uint32_t(*)[8])font_serif_8;  break;
		case SERIF16: size = 16; data = (const uint32_t(*)[8])font_serif_16; break;
		case SERIF32: size = 32; data = (const uint32_t(*)[8])font_serif_32; break;
		case COMIC8:  size = 8;  data = (const uint32_t(*)[8])font_comic_8;  break;
		case COMIC16: size = 16; data = (const uint32_t(*)[8])font_comic_16; break;
		case COMIC32: size = 32; data = (const uint32_t(*)[8])font_comic_32; break;
		default: return 0;
	}

	for (int row = 0; row < size; row++) {
		uint32_t bits = ((const uint32_t*)data)[idx * size + row];
		for (int col = 0; col < size; col++) {
			if (bits & (1 << (31 - col))) {
				SDL_RenderDrawPoint(renderer, x + col, y + row);
			}
		}
	}

	return 1;
}

static void set_color(Color c) {
	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
}

int render_rect(int x, int y, int w, int h, Color color) {
	set_color(color);
	SDL_Rect rect = {x, y, w, h};
	return SDL_RenderDrawRect(renderer, &rect) == 0;
}

int render_rect_fill(int x, int y, int w, int h, Color color) {
	set_color(color);
	SDL_Rect rect = {x, y, w, h};
	return SDL_RenderFillRect(renderer, &rect) == 0;
}

int render_line(int x1, int y1, int x2, int y2, Color color) {
	set_color(color);
	return SDL_RenderDrawLine(renderer, x1, y1, x2, y2) == 0;
}

int render_pixel(int x, int y, Color color) {
	set_color(color);
	return SDL_RenderDrawPoint(renderer, x, y) == 0;
}

int render_circle(int x, int y, int radius, Color color) {
	set_color(color);
	int dx = radius, dy = 0, err = 0;
	while (dx >= dy) {
		SDL_RenderDrawPoint(renderer, x+dx, y+dy);
		SDL_RenderDrawPoint(renderer, x+dy, y+dx);
		SDL_RenderDrawPoint(renderer, x-dy, y+dx);
		SDL_RenderDrawPoint(renderer, x-dx, y+dy);
		SDL_RenderDrawPoint(renderer, x-dx, y-dy);
		SDL_RenderDrawPoint(renderer, x-dy, y-dx);
		SDL_RenderDrawPoint(renderer, x+dy, y-dx);
		SDL_RenderDrawPoint(renderer, x+dx, y-dy);
		if (err <= 0) { dy++; err += 2*dy+1; }
		if (err > 0)  { dx--; err -= 2*dx+1; }
	}
	return 1;
}

int render_circle_fill(int x, int y, int radius, Color color) {
	set_color(color);
	for (int dy = -radius; dy <= radius; dy++) {
		int dx = (int)sqrt((double)(radius*radius - dy*dy));
		SDL_RenderDrawLine(renderer, x-dx, y+dy, x+dx, y+dy);
	}
	return 1;
}

Color render_get_pixel(int x, int y) {
	Color c = {0, 0, 0};
	SDL_Surface* surface = SDL_GetWindowSurface(SDL_RenderGetWindow(renderer));
	if (!surface) return c;
	SDL_LockSurface(surface);
	uint32_t* pixels = (uint32_t*)surface->pixels;
	uint32_t pixel = pixels[y * surface->w + x];
	SDL_GetRGB(pixel, surface->format, &c.r, &c.g, &c.b);
	SDL_UnlockSurface(surface);
	return c;
}
