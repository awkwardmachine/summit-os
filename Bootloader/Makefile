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
SOURCES = $(SRC_DIR)/bootloader.c

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

$(BUILD_DIR)/bootloader.o: $(SOURCES)
	@mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/bootloader.so: $(BUILD_DIR)/bootloader.o
	ld $< $(EFI_LIB)/crt0-efi-$(ARCH).o $(LDFLAGS) -o $@

$(TARGET): $(BUILD_DIR)/bootloader.so
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic \
	        -j .dynsym -j .rel -j .rela -j .reloc \
	        --target=efi-app-$(ARCH) $< $@

clean:
	@rm -f $(BUILD_DIR)/bootloader.o $(BUILD_DIR)/bootloader.so $(TARGET)