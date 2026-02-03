#include "bootloader.h"

EFI_STATUS LoadKernel(EFI_HANDLE ImageHandle,
                      EFI_SYSTEM_TABLE *SystemTable,
                      VOID **KernelBuffer,
                      EFI_PHYSICAL_ADDRESS *KernelEntry)
{
    EFI_STATUS Status;
    EFI_LOADED_IMAGE *LoadedImage;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem;
    EFI_FILE *Root;
    EFI_FILE *KernelFile;
    UINTN KernelSize;

    // Get loaded image protocol
    Status = uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle,
                               &LoadedImageProtocol, (VOID**)&LoadedImage);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get LoadedImage protocol: %r\n", Status);
        return Status;
    }

    // Get file system protocol
    Status = uefi_call_wrapper(BS->HandleProtocol, 3, LoadedImage->DeviceHandle,
                               &FileSystemProtocol, (VOID**)&FileSystem);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get FileSystem protocol: %r\n", Status);
        return Status;
    }

    // Open root directory
    Status = uefi_call_wrapper(FileSystem->OpenVolume, 2, FileSystem, &Root);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open root volume: %r\n", Status);
        return Status;
    }

    // Open Kernel file
    Status = uefi_call_wrapper(Root->Open, 5, Root, &KernelFile, KERNEL_FILENAME,
                               EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to open %s: %r\n", KERNEL_FILENAME, Status);
        return Status;
    }

    // Get Kernel file size
    EFI_FILE_INFO *FileInfo;
    UINTN FileInfoSize = sizeof(EFI_FILE_INFO) + 256;
    FileInfo = AllocatePool(FileInfoSize);
    Status = uefi_call_wrapper(KernelFile->GetInfo, 4, KernelFile, &GenericFileInfo,
                               &FileInfoSize, FileInfo);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to get Kernel file info: %r\n", Status);
        return Status;
    }

    KernelSize = FileInfo->FileSize;
    Print(L"Kernel size: %d bytes\n", KernelSize);
    FreePool(FileInfo);

    // Allocate memory for Kernel at specified address
    *KernelEntry = KERNEL_LOAD_ADDRESS;
    UINTN Pages = (KernelSize + 0xFFF) / 0x1000;
    Status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress,
                               EfiLoaderData, Pages, KernelEntry);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to allocate memory for Kernel: %r\n", Status);
        return Status;
    }

    *KernelBuffer = (VOID*)*KernelEntry;

    // Read Kernel into memory
    Status = uefi_call_wrapper(KernelFile->Read, 3, KernelFile, &KernelSize, *KernelBuffer);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to read Kernel: %r\n", Status);
        return Status;
    }

    // Close files
    uefi_call_wrapper(KernelFile->Close, 1, KernelFile);
    uefi_call_wrapper(Root->Close, 1, Root);

    Print(L"Kernel loaded at 0x%x\n", *KernelEntry);

    // Verify Kernel was loaded
    unsigned char *kernel_check = (unsigned char*)*KernelEntry;
    Print(L"Kernel header: %02x %02x %02x %02x\n",
          kernel_check[0], kernel_check[1], kernel_check[2], kernel_check[3]);

    return EFI_SUCCESS;
}

EFI_STATUS SetupGraphics(EFI_GRAPHICS_OUTPUT_PROTOCOL **Gop,
                         VOID **FramebufferBase,
                         UINT32 *FramebufferWidth,
                         UINT32 *FramebufferHeight,
                         UINT32 *FramebufferPitch)
{
    EFI_STATUS Status;

    // Locate Graphics Output Protocol
    Status = uefi_call_wrapper(BS->LocateProtocol, 3, &GraphicsOutputProtocol, NULL, (VOID**)Gop);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to locate GOP: %r\n", Status);
        return Status;
    }

    // Use current mode
    UINT32 ModeIndex = (*Gop)->Mode->Mode;
    UINTN SizeOfInfo;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;

    // Try to find preferred resolution
    for (UINT32 i = 0; i < (*Gop)->Mode->MaxMode; i++) {
        Status = uefi_call_wrapper((*Gop)->QueryMode, 4, *Gop, i, &SizeOfInfo, &Info);
        if (!EFI_ERROR(Status)) {
            if (Info->HorizontalResolution == PREFERRED_RESOLUTION_WIDTH &&
                Info->VerticalResolution == PREFERRED_RESOLUTION_HEIGHT) {
                ModeIndex = i;
                break;
            }
        }
    }

    // Set the graphics mode
    Status = uefi_call_wrapper((*Gop)->SetMode, 2, *Gop, ModeIndex);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to set graphics mode: %r\n", Status);
        return Status;
    }

    Print(L"Graphics mode: %dx%d\n",
          (*Gop)->Mode->Info->HorizontalResolution,
          (*Gop)->Mode->Info->VerticalResolution);

    // Get framebuffer information
    *FramebufferBase = (VOID*)(*Gop)->Mode->FrameBufferBase;
    *FramebufferWidth = (*Gop)->Mode->Info->HorizontalResolution;
    *FramebufferHeight = (*Gop)->Mode->Info->VerticalResolution;
    *FramebufferPitch = (*Gop)->Mode->Info->PixelsPerScanLine * 4;

    return EFI_SUCCESS;
}

EFI_STATUS PrepareBootInfo(EFI_HANDLE ImageHandle,
                           VOID *FramebufferBase,
                           UINT32 FramebufferWidth,
                           UINT32 FramebufferHeight,
                           UINT32 FramebufferPitch,
                           BootInfo **bootInfo)
{
    EFI_STATUS Status;
    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMapBuf = NULL;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;

    // Get memory map size
    Status = uefi_call_wrapper(BS->GetMemoryMap, 5, &MemoryMapSize, MemoryMapBuf,
                               &MapKey, &DescriptorSize, &DescriptorVersion);

    // Allocate buffer for memory map
    MemoryMapSize += 2 * DescriptorSize;
    MemoryMapBuf = AllocatePool(MemoryMapSize);

    // Allocate boot info structure
    *bootInfo = AllocatePool(sizeof(BootInfo));

    Print(L"Exiting boot services...\n");

    // Retry loop for ExitBootServices
    UINTN retries = 0;
    while (retries < EXIT_BOOT_SERVICES_RETRIES) {
        Status = uefi_call_wrapper(BS->GetMemoryMap, 5, &MemoryMapSize, MemoryMapBuf,
                                   &MapKey, &DescriptorSize, &DescriptorVersion);
        if (EFI_ERROR(Status)) {
            Print(L"Failed to get memory map: %r\n", Status);
            return Status;
        }

        // Fill boot info structure
        (*bootInfo)->memory_map.entry_count = MemoryMapSize / DescriptorSize;
        (*bootInfo)->memory_map.entry_size = DescriptorSize;
        (*bootInfo)->memory_map.entries = MemoryMapBuf;
        (*bootInfo)->framebuffer_base = FramebufferBase;
        (*bootInfo)->framebuffer_width = FramebufferWidth;
        (*bootInfo)->framebuffer_height = FramebufferHeight;
        (*bootInfo)->framebuffer_pitch = FramebufferPitch;

        Status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, MapKey);
        if (!EFI_ERROR(Status)) {
            return EFI_SUCCESS;
        }

        retries++;
    }

    // Failed after all retries
    Print(L"Failed to exit boot services after %d retries: %r\n", retries, Status);
    return Status;
}


EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS Status;
    VOID *KernelBuffer;
    EFI_PHYSICAL_ADDRESS KernelEntry;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    VOID *FramebufferBase;
    UINT32 FramebufferWidth;
    UINT32 FramebufferHeight;
    UINT32 FramebufferPitch;
    BootInfo *bootInfo;

    Print(L"Summit OS Bootloader Starting...\n");

    // Load Kernel from filesystem
    Status = LoadKernel(ImageHandle, SystemTable, &KernelBuffer, &KernelEntry);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to load Kernel\n");
        while(1);
    }

    // Setup graphics mode
    Status = SetupGraphics(&Gop, &FramebufferBase, &FramebufferWidth,
                          &FramebufferHeight, &FramebufferPitch);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to setup graphics\n");
        while(1);
    }

    // Prepare boot info and exit boot services
    Status = PrepareBootInfo(ImageHandle, FramebufferBase, FramebufferWidth,
                            FramebufferHeight, FramebufferPitch, &bootInfo);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to exit boot services\n");
        while(1);
    }

    // Jump to Kernel entry point
    void (*kernel_main)(BootInfo*) = (void(*)(BootInfo*))KernelEntry;
    kernel_main(bootInfo);

    while(1) {
        __asm__ volatile ("hlt");
    }

    return EFI_SUCCESS;
}