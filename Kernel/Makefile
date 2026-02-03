ARCH = x86_64

# Directories
SRC_DIR = src
INC_DIR = include
ARCH_DIR = arch/$(ARCH)
BUILD_DIR = ../Build

# Output
TARGET = $(BUILD_DIR)/kernel.bin

# Sources
C_SOURCES = $(SRC_DIR)/main.c \
            $(SRC_DIR)/framebuffer.c

ASM_SOURCES = $(ARCH_DIR)/boot.S

# Objects
C_OBJECTS = $(BUILD_DIR)/kernel_main.o \
            $(BUILD_DIR)/kernel_graphics.o

ASM_OBJECTS = $(BUILD_DIR)/kernel_boot.o

# Includes
INCLUDES = -I$(INC_DIR) \
           -I$(ARCH_DIR)/include \
           -I../Common/include

# Compiler flags
CFLAGS = $(INCLUDES) \
         -ffreestanding \
         -fno-stack-protector \
         -fno-builtin \
         -nostdlib \
         -mno-red-zone \
         -mno-mmx \
         -mno-sse \
         -mno-sse2 \
         -Wall \
         -Wextra \
         -O2

# Linker flags
LDFLAGS = -T kernel.ld \
          -nostdlib \
          -n \
          -z max-page-size=0x1000

.PHONY: all clean

all: $(TARGET)

$(BUILD_DIR)/kernel_main.o: $(SRC_DIR)/main.c
	@mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel_graphics.o: $(SRC_DIR)/framebuffer.c
	@mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/kernel_boot.o: $(ASM_SOURCES)
	@mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(TARGET): $(ASM_OBJECTS) $(C_OBJECTS) kernel.ld
	ld $(LDFLAGS) $(ASM_OBJECTS) $(C_OBJECTS) -o $@

clean:
	@rm -f $(C_OBJECTS) $(ASM_OBJECTS) $(TARGET)