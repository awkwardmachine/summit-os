.section .text.entry
.global _start

_start:
    cld

    mov $0x200000, %rsp

    call kernel_main
.halt:
    hlt
    jmp .halt