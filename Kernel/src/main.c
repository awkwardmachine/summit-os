#include "bootinfo.h"
#include "framebuffer.h"

__attribute__((section(".text.kernel_main")))
void kernel_main(BootInfo *boot_info) {
    if (boot_info == (void*)0 || boot_info->framebuffer_base == (void*)0) {
        unsigned short *vga = (unsigned short*)0xB8000;
        vga[0] = 0x0F00 | 'N';
        vga[1] = 0x0F00 | 'O';
        vga[2] = 0x0F00 | ' ';
        vga[3] = 0x0F00 | 'F';
        vga[4] = 0x0F00 | 'B';
        while(1) {
            __asm__ volatile ("hlt");
        }
    }

    fb_init((unsigned int*)boot_info->framebuffer_base,
            boot_info->framebuffer_width,
            boot_info->framebuffer_height,
            boot_info->framebuffer_pitch,
            32);

    fb_clear(COLOR_BLACK);

    fb.fg_color = COLOR_GREEN;
    fb_print("Summit OS Kernel started!\n\n");

    fb.fg_color = COLOR_CYAN;
    fb_print("Resolution: ");
    fb.fg_color = COLOR_WHITE;

    unsigned int width = boot_info->framebuffer_width;
    unsigned int height = boot_info->framebuffer_height;

    char width_buffer[12];
    char *ptr = width_buffer + 11;
    *ptr = '\0';
    if (width == 0) {
        *(--ptr) = '0';
    } else {
        while (width > 0) {
            *(--ptr) = '0' + (width % 10);
            width /= 10;
        }
    }
    fb_print(ptr);

    fb_print(" x ");

    char height_buffer[12];
    ptr = height_buffer + 11;
    *ptr = '\0';
    if (height == 0) {
        *(--ptr) = '0';
    } else {
        while (height > 0) {
            *(--ptr) = '0' + (height % 10);
            height /= 10;
        }
    }
    fb_print(ptr);
    fb_print("\n\n");

    while(1) {
        __asm__ volatile ("hlt");
    }
}