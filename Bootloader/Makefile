ARCH = x86_64

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = ../Build

# Output
TARGET = $(BUILD_DIR)/BOOTX64.EFI

# EFI paths
EFI_LIB = /usr/lib
EFI_INC = /usr/include/efi

# Sources
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Objects
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

# Includes
INCLUDES = -I$(INC_DIR) \
           -I$(EFI_INC) \
           -I$(EFI_INC)/$(ARCH) \
           -I../Common/include

# Compiler flags
CFLAGS = $(INCLUDES) \
         -fno-stack-protector \
         -fpic \
         -fshort-wchar \
         -mno-red-zone \
         -DEFI_FUNCTION_WRAPPER

# Linker flags
LDFLAGS = -nostdlib \
          -znocombreloc \
          -T $(EFI_LIB)/elf_$(ARCH)_efi.lds \
          -shared \
          -Bsymbolic \
          -L$(EFI_LIB) \
          -l:libgnuefi.a \
          -l:libefi.a

.PHONY: all clean

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/bootloader.so: $(C_OBJECTS)
	ld $(C_OBJECTS) $(EFI_LIB)/crt0-efi-$(ARCH).o $(LDFLAGS) -o $@

$(TARGET): $(BUILD_DIR)/bootloader.so
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic \
	        -j .dynsym -j .rel -j .rela -j .reloc \
	        --target=efi-app-$(ARCH) $< $@

clean:
	@rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.so $(TARGET)