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
	mov edi, wel32Msg 	; Setting PM welcome message to be printed
	mov ecx, wel32MsgLen ; Length of the message
	call terminal_write	; Printing PM Welcome message
	call cursor_newline
	mov edi, wel32Msg 	; Setting PM welcome message to be printed
	call terminal_write_string ; Testing terminal_write_string with null-terminated string
	mov edi, wel32Msg 	; Setting PM welcome message to be printed
	mov ecx, 0x05
	call terminal_writeln ; Testing terminal_writeln with null-terminated string

	call terminal_backspace
	call terminal_backspace
	call terminal_backspace
	mov al, 'A'
	call terminal_putchar ; Testing terminal_putchar with null-terminated string

	call terminal_backspace
	call terminal_backspace
	call terminal_backspace
	mov al, 'A'
	call terminal_putchar ; Testing terminal_putchar with null-terminated string

		call terminal_backspace
	call terminal_backspace
	call terminal_backspace
	mov al, 'A'
	call terminal_putchar ; Testing terminal_putchar with null-terminated string

	call terminal_backspace
	call terminal_backspace
	call terminal_backspace
	mov al, 'A'
	call terminal_putchar ; Testing terminal_putchar with 

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


%include "cursor.asm"
%include "terminal.asm"


wel32Msg db "Testing cursor", 0
; wel32MsgLen equ $ - wel32Msg
wel32MsgLen equ 5

section .bss
stack_bottom:
    resb 4096
stack_top:

cursor_x resb 1
cursor_y resb 1