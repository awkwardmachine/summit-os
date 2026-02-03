ARCH = x86_64

# Directories
SRC_DIR = src
DRIVERS_DIR = drivers
INC_DIR = include
ARCH_DIR = arch/$(ARCH)
BUILD_DIR = ../Build

# Output
TARGET = $(BUILD_DIR)/kernel.bin

# Sources
C_SOURCES = $(wildcard $(SRC_DIR)/*.c) \
            $(wildcard $(DRIVERS_DIR)/*.c)

ASM_SOURCES = $(ARCH_DIR)/boot.S

# Objects
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c)) \
            $(patsubst $(DRIVERS_DIR)/%.c,$(BUILD_DIR)/%.o,$(wildcard $(DRIVERS_DIR)/*.c))

ASM_OBJECTS = $(BUILD_DIR)/boot.o

# Includes
INCLUDES = -I$(INC_DIR) \
           -I$(DRIVERS_DIR) \
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

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(DRIVERS_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/boot.o: $(ASM_SOURCES)
	@mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(TARGET): $(ASM_OBJECTS) $(C_OBJECTS) kernel.ld
	ld $(LDFLAGS) $(ASM_OBJECTS) $(C_OBJECTS) -o $@

clean:
	@rm -f $(BUILD_DIR)/*.o $(TARGET)