ARCH = x86_64

# Directories
BOOTLOADER_DIR = Bootloader
KERNEL_DIR = Kernel
BUILD_DIR = Build

# Output files
BOOTLOADER_EFI = $(BUILD_DIR)/BOOTX64.EFI
KERNEL_BIN = $(BUILD_DIR)/kernel.bin

# OVMF paths
OVMF_CODE = /usr/share/edk2/x64/OVMF_CODE.4m.fd
OVMF_VARS = /usr/share/edk2/x64/OVMF_VARS.4m.fd

.PHONY: all clean bootloader kernel iso img run run-iso run-img help

all: bootloader kernel

bootloader:
	@$(MAKE) -C $(BOOTLOADER_DIR)

kernel:
	@$(MAKE) -C $(KERNEL_DIR)

# Image creation
iso: all
	@mkdir -p $(BUILD_DIR)/iso/EFI/BOOT
	@cp $(BOOTLOADER_EFI) $(BUILD_DIR)/iso/EFI/BOOT/BOOTX64.EFI
	@cp $(KERNEL_BIN) $(BUILD_DIR)/iso/kernel.bin
	@dd if=/dev/zero of=$(BUILD_DIR)/efiboot.img bs=1M count=10 2>/dev/null
	@mkfs.vfat -F 12 $(BUILD_DIR)/efiboot.img >/dev/null 2>&1
	@mmd -i $(BUILD_DIR)/efiboot.img ::/EFI ::/EFI/BOOT
	@mcopy -i $(BUILD_DIR)/efiboot.img $(BUILD_DIR)/iso/EFI/BOOT/BOOTX64.EFI ::/EFI/BOOT/
	@mcopy -i $(BUILD_DIR)/efiboot.img $(BUILD_DIR)/iso/kernel.bin ::/
	@mv $(BUILD_DIR)/efiboot.img $(BUILD_DIR)/iso/
	@xorriso -as mkisofs -o $(BUILD_DIR)/summit-os-x86_64.iso \
	         -e efiboot.img -no-emul-boot -no-pad \
	         $(BUILD_DIR)/iso 2>/dev/null
	@rm -f $(BUILD_DIR)/iso/efiboot.img

img: all
	@dd if=/dev/zero of=$(BUILD_DIR)/boot.img bs=1M count=10 2>/dev/null
	@mkfs.vfat -F 12 $(BUILD_DIR)/boot.img >/dev/null 2>&1
	@mmd -i $(BUILD_DIR)/boot.img ::/EFI ::/EFI/BOOT
	@mcopy -i $(BUILD_DIR)/boot.img $(BOOTLOADER_EFI) ::/EFI/BOOT/BOOTX64.EFI
	@mcopy -i $(BUILD_DIR)/boot.img $(KERNEL_BIN) ::/kernel.bin

# Run targets
run: all $(BUILD_DIR)/OVMF_VARS.fd
	@mkdir -p $(BUILD_DIR)/esp/EFI/BOOT
	@cp $(BOOTLOADER_EFI) $(BUILD_DIR)/esp/EFI/BOOT/BOOTX64.EFI
	@cp $(KERNEL_BIN) $(BUILD_DIR)/esp/kernel.bin
	@echo "fs0:\EFI\BOOT\BOOTX64.EFI" > $(BUILD_DIR)/esp/startup.nsh
	qemu-system-x86_64 \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(BUILD_DIR)/OVMF_VARS.fd \
		-drive format=raw,file=fat:rw:$(BUILD_DIR)/esp \
		-net none

run-iso: iso $(BUILD_DIR)/OVMF_VARS.fd
	qemu-system-x86_64 \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(BUILD_DIR)/OVMF_VARS.fd \
		-cdrom $(BUILD_DIR)/summit-os-x86_64.iso \
		-net none

run-img: img $(BUILD_DIR)/OVMF_VARS.fd
	qemu-system-x86_64 \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_CODE) \
		-drive if=pflash,format=raw,file=$(BUILD_DIR)/OVMF_VARS.fd \
		-drive format=raw,file=$(BUILD_DIR)/boot.img \
		-net none

$(BUILD_DIR)/OVMF_VARS.fd:
	@mkdir -p $(BUILD_DIR)
	@cp $(OVMF_VARS) $@

clean:
	@$(MAKE) -C $(BOOTLOADER_DIR) clean
	@$(MAKE) -C $(KERNEL_DIR) clean
	@rm -rf $(BUILD_DIR)