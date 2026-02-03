# Summit OS

A lightweight x86_64 operating system inspired by Terry Davis and TempleOS.

Summit OS is the home of the Summit programming language. It aims to be small, fast, and eventually self-hosting with its own assembler. One day, the entire OS will be rewritten in Summit.

## Prerequisites

- gcc
- ld (binutils)
- GNU EFI libraries (`gnu-efi`)
- QEMU (`qemu-system-x86_64`)
- OVMF firmware
- xorriso
- mtools

### Ubuntu/Debian
```bash
sudo apt install build-essential gnu-efi qemu-system-x86 ovmf xorriso mtools
```

### Arch/Manjaro
```bash
sudo pacman -S base-devel gnu-efi qemu-system-x86 edk2-ovmf xorriso mtools
```

### Fedora/RHEL
```bash
sudo dnf install gcc binutils gnu-efi-devel qemu-system-x86 edk2-ovmf xorriso mtools
```

### openSUSE
```bash
sudo zypper install gcc binutils gnu-efi-devel qemu-x86 qemu-ovmf-x86_64 xorriso mtools
```

## Building

```bash
git clone https://github.com/awkwardmachine/summit-os
cd summit-os
make run-iso
```

To rebuild:
```bash
make clean && make run-iso
```

## Goals

- Stay lightweight and minimal
- Build the Summit programming language
- Create a custom assembler
- Eventually rewrite everything in Summit
- Achieve the goals met by TempleOS

## Warning

**Summit OS is very unstable and will be buggy until a stable release.**