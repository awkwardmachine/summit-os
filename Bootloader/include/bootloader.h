#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <efi.h>
#include <efilib.h>

#include "bootinfo.h"

#define KERNEL_FILENAME L"Kernel.bin"
#define KERNEL_LOAD_ADDRESS 0x100000
#define PREFERRED_RESOLUTION_WIDTH 1024
#define PREFERRED_RESOLUTION_HEIGHT 768
#define EXIT_BOOT_SERVICES_RETRIES 10

EFI_STATUS LoadKernel(EFI_HANDLE ImageHandle,
                      EFI_SYSTEM_TABLE *SystemTable,
                      VOID **KernelBuffer,
                      EFI_PHYSICAL_ADDRESS *KernelEntry);

EFI_STATUS SetupGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL **Gop,
                         VOID **FramebufferBase,
                         UINT32 *FramebufferWidth,
                         UINT32 *FramebufferHeight,
                         UINT32 *FramebufferPitch);

EFI_STATUS PrepareBootInfo(EFI_HANDLE ImageHandle,
                           VOID *FramebufferBase,
                           UINT32 FramebufferWidth,
                           UINT32 FramebufferHeight,
                           UINT32 FramebufferPitch,
                           BootInfo **bootInfo);

#endif