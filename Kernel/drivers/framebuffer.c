#include "drivers/framebuffer.h"
#include "font.h"

Framebuffer fb = {0};

void fb_init(unsigned int *addr, unsigned int w,
             unsigned int h, unsigned int pitch, unsigned int bpp) {
    fb.address = addr;
    fb.width = w;
    fb.height = h;
    fb.pitch = pitch;
    fb.cursor_x = 0;
    fb.cursor_y = 0;
    fb.fg_color = COLOR_WHITE;
    fb.bg_color = COLOR_BLACK;
}

void fb_putpixel(unsigned int x, unsigned int y, unsigned int color) {
    if (x >= fb.width || y >= fb.height) return;
    unsigned int pitch_pixels = fb.pitch / 4;
    fb.address[y * pitch_pixels + x] = color;
}

void fb_draw_char(char c, unsigned int x, unsigned int y, unsigned int color) {
    if (c < 32 || c > 126) c = ' ';
    const unsigned char *glyph = font_8x16[c - 32];

    for (unsigned int row = 0; row < 16; row++) {
        unsigned char row_data = glyph[row];
        for (unsigned int col = 0; col < 8; col++) {
            if (row_data & (1 << (7 - col))) {
                fb_putpixel(x + col, y + row, color);
            }
        }
    }
}

void fb_print(const char *str) {
    unsigned int start_x = fb.cursor_x;

    while (*str) {
        char c = *str;

        if (c == '\n') {
            fb.cursor_y += 16;
            fb.cursor_x = 0;
            str++;
            continue;
        }

        if (c == '\r') {
            fb.cursor_x = 0;
            str++;
            continue;
        }

        if (fb.cursor_y >= fb.height) {
            unsigned int pitch_pixels = fb.pitch / 4;
            for (unsigned int y = 16; y < fb.height; y++) {
                for (unsigned int x = 0; x < fb.width; x++) {
                    fb.address[(y - 16) * pitch_pixels + x] =
                    fb.address[y * pitch_pixels + x];
                }
            }
            for (unsigned int y = fb.height - 16; y < fb.height; y++) {
                for (unsigned int x = 0; x < fb.width; x++) {
                    fb_putpixel(x, y, fb.bg_color);
                }
            }
            fb.cursor_y -= 16;
        }

        if (fb.cursor_x + 8 > fb.width) {
            fb.cursor_x = start_x;
            fb.cursor_y += 16;
        }

        fb_draw_char(c, fb.cursor_x, fb.cursor_y, fb.fg_color);
        fb.cursor_x += 8;
        str++;
    }
}

void fb_clear(unsigned int color) {
    unsigned int pitch_pixels = fb.pitch / 4;
    for (unsigned int y = 0; y < fb.height; y++) {
        for (unsigned int x = 0; x < fb.width; x++) {
            fb.address[y * pitch_pixels + x] = color;
        }
    }
    fb.cursor_x = 0;
    fb.cursor_y = 0;
}