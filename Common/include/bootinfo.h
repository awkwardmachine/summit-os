#ifndef BOOTINFO_H
#define BOOTINFO_H

typedef struct {
    unsigned int entry_count;
    unsigned long entry_size;
    void *entries;
} MemoryMap;

typedef struct {
    MemoryMap memory_map;
    void *framebuffer_base;
    unsigned int framebuffer_width;
    unsigned int framebuffer_height;
    unsigned int framebuffer_pitch;
} BootInfo;

#endif