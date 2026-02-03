#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define COLOR_BLACK     0x00000000
#define COLOR_WHITE     0xFFFFFFFF
#define COLOR_RED       0xFFFF0000
#define COLOR_GREEN     0xFF00FF00
#define COLOR_BLUE      0xFF0000FF
#define COLOR_CYAN      0xFF00FFFF
#define COLOR_MAGENTA   0xFFFF00FF
#define COLOR_YELLOW    0xFFFFFF00

typedef struct {
    unsigned int *address;
    unsigned int width;
    unsigned int height;
    unsigned int pitch;
    unsigned int cursor_x;
    unsigned int cursor_y;
    unsigned int fg_color;
    unsigned int bg_color;
} Framebuffer;

extern Framebuffer fb;

void fb_init(unsigned int *addr, unsigned int w,
             unsigned int h, unsigned int pitch, unsigned int bpp);
void fb_putpixel(unsigned int x, unsigned int y, unsigned int color);
void fb_print(const char *str);
void fb_clear(unsigned int color);

#endif