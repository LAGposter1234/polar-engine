#include <stdint.h>
#ifndef RENDER_H
#define RENDER_H

int render_init(const char* title, int width, int height);
void render_clear(void);
void render_present(void);
void render_quit(void);

typedef enum {
    VGA8, VGA16, VGA32,
    SANS8, SANS16, SANS32,
    SERIF8, SERIF16, SERIF32,
    COMIC8, COMIC16, COMIC32
} Font;

int render_character(char c, Font font, int x, int y);

typedef struct { uint8_t r, g, b; } Color;

int render_rect(int x, int y, int w, int h, Color color);
int render_rect_fill(int x, int y, int w, int h, Color color);
int render_line(int x1, int y1, int x2, int y2, Color color);
int render_pixel(int x, int y, Color color);
int render_circle(int x, int y, int radius, Color color);
int render_circle_fill(int x, int y, int radius, Color color);
Color render_get_pixel(int x, int y);

#endif
