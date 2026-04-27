
global inb

section .text
inb:
; uint8_t inb(uint16_t port);
    mov dx, [esp + 4]   ; port argument
    xor eax, eax
    in al, dx           ; read byte from port into al
    ret


