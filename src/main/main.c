#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tools/piss/piss.hpp"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <SDL2/SDL.h>
#include "../include/render.h"

static lua_State* L = NULL;

static int lua_render_rect(lua_State* L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	int w = luaL_checkinteger(L, 3);
	int h = luaL_checkinteger(L, 4);
	Color c = {luaL_checkinteger(L, 5), luaL_checkinteger(L, 6), luaL_checkinteger(L, 7)};
	lua_pushboolean(L, render_rect(x, y, w, h, c));
	return 1;
}

static int lua_render_rect_fill(lua_State* L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	int w = luaL_checkinteger(L, 3);
	int h = luaL_checkinteger(L, 4);
	Color c = {luaL_checkinteger(L, 5), luaL_checkinteger(L, 6), luaL_checkinteger(L, 7)};
	lua_pushboolean(L, render_rect_fill(x, y, w, h, c));
	return 1;
}

static int lua_render_line(lua_State* L) {
	int x1 = luaL_checkinteger(L, 1);
	int y1 = luaL_checkinteger(L, 2);
	int x2 = luaL_checkinteger(L, 3);
	int y2 = luaL_checkinteger(L, 4);
	Color c = {luaL_checkinteger(L, 5), luaL_checkinteger(L, 6), luaL_checkinteger(L, 7)};
	lua_pushboolean(L, render_line(x1, y1, x2, y2, c));
	return 1;
}

static int lua_render_pixel(lua_State* L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	Color c = {luaL_checkinteger(L, 3), luaL_checkinteger(L, 4), luaL_checkinteger(L, 5)};
	lua_pushboolean(L, render_pixel(x, y, c));
	return 1;
}

static int lua_render_circle(lua_State* L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	int r = luaL_checkinteger(L, 3);
	Color c = {luaL_checkinteger(L, 4), luaL_checkinteger(L, 5), luaL_checkinteger(L, 6)};
	lua_pushboolean(L, render_circle(x, y, r, c));
	return 1;
}

static int lua_render_circle_fill(lua_State* L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	int r = luaL_checkinteger(L, 3);
	Color c = {luaL_checkinteger(L, 4), luaL_checkinteger(L, 5), luaL_checkinteger(L, 6)};
	lua_pushboolean(L, render_circle_fill(x, y, r, c));
	return 1;
}

static int lua_render_clear(lua_State* L) {
	render_clear();
	return 0;
}

static int lua_render_present(lua_State* L) {
	render_present();
	return 0;
}

static void register_lua_api(lua_State* L) {
	lua_register(L, "render_rect",        lua_render_rect);
	lua_register(L, "render_rect_fill",   lua_render_rect_fill);
	lua_register(L, "render_line",        lua_render_line);
	lua_register(L, "render_pixel",       lua_render_pixel);
	lua_register(L, "render_circle",      lua_render_circle);
	lua_register(L, "render_circle_fill", lua_render_circle_fill);
	lua_register(L, "render_clear",       lua_render_clear);
	lua_register(L, "render_present",     lua_render_present);
}

int polar_enginestart(const char* lua_path) {
	if (!render_init("Polar Engine", 800, 600)) return -1;

	L = luaL_newstate();
	if (!L) {
		render_quit();
		return -1;
	}
	luaL_openlibs(L);
	register_lua_api(L);

	if (luaL_loadfile(L, lua_path) || lua_pcall(L, 0, 0, 0)) {
		printf("Lua error: %s\n", lua_tostring(L, -1));
		lua_close(L);
		render_quit();
		return -1;
	}
	SDL_Event event;
	int running = 1;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) running = 0;
		}
		render_clear();
		lua_getglobal(L, "update");
		if (lua_isfunction(L, -1)) {
			lua_pcall(L, 0, 0, 0);
		} else {
			lua_pop(L, 1);
		}
		render_present();
	}

	lua_close(L);
	render_quit();
	return 0;
}

int polar_initengine(char* mainpath) {
	polar_initpiss();
	return 0;
};

int polar_engineopen() {
	polar_piss_open();
	return 0;
}

int main(int argc, char** argv) {
	if (argc < 2) return 0;
	if (strcmp(argv[1], "init") == 0) {
		int init;
		if (argc < 3) {
			int init = polar_initengine("main.lua");
		} else {
			int init = polar_initengine(argv[2]);
		}
		if (init != 0) return init;
		polar_initpiss();
	} else
	if (strcmp(argv[1], "open") == 0) {
		int init;
		init = polar_engineopen();
		if (init != 0) return init;
	}
	if (strcmp(argv[1], "run") == 0) {
		polar_engineopen();
	}
	return 0;
}
