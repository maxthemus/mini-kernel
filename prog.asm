[org 0x90000]

[bits 32]

jmp start


start:
	; Init segemnents
	mov ax, 0x10      ; DATA_SEG gdt entry 02
	mov ds, ax
	mov es, ax

	; Init stack
	mov esp, stack_top
	xor ebp, ebp


	call fill_blue_screen
	call cursor_init
	mov esi, wel32Msg 	; Setting PM welcome message to be printed
	call Print32		; Printing PM Welcome message

	cli
	hlt


loop:
	jmp loop


fill_blue_screen:
	mov edi, 0xb8000 ; Vga memeory start address
	mov ecx, 2000 ; 80 * 25 cells
	mov al, ' '          ; space character
    mov ah, 0x1F         ; white text on blue background
.loop:
    mov [edi], ax        ; write char + color
    add edi, 2
    loop .loop

    ret


; Sub routine to print message in 32 bit protected mode
Print32:
	pusha 
.repeat:
	lodsb
	or al, al
	jz .done

	call cursor_put_char

	jmp .repeat
.done:
	popa
	ret


%include "cursor.asm"


wel32Msg db "Testing cursor", 0

section .bss
stack_bottom:
    resb 4096
stack_top:

cursor_x resb 1
cursor_y resb 1