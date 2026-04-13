;80 columns per row
;2 bytes per cell

[bits 32]

global cursor_init
global cursor_put_char
global cursor_newline

cursor_init:
    mov byte [cursor_x], 0
    mov byte [cursor_y], 0
    ret

cursor_newline:
    mov byte [cursor_x], 0
    inc byte [cursor_y]
    ret

cursor_put_char:
    pusha

    mov byte cl, al ; Store the character to print in r8b    

    ; (y * 80 + x) * 2
    movzx eax, byte [cursor_y]
    mov ebx, 80
    mul ebx

    movzx ebx, byte [cursor_x]
    add eax, ebx
    shl eax, 1

    mov edi, 0xB8000
    add edi, eax

    mov byte ah, 0x1F
    mov byte al, cl
    mov [edi], ax

    inc byte [cursor_x]

    cmp byte [cursor_x], 80
    jl .done

    mov byte [cursor_x], 0
    inc byte [cursor_y]

.done:
    popa
    ret;offset = (y * 80 + x) * 2