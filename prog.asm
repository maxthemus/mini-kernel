[org 0x90000]

[bits 32]

jmp start

start:
	; Init segemnents
	mov ax, 0x10      ; DATA_SEG gdt entry 02
	mov ds, ax
	mov es, ax
	mov ss, ax

	; Init stack
	mov esp, stack_top

	call fill_blue_screen
	call cursor_init

	;cli
	;call pic_remap
	;call idt_init
	;call fill_blue_screen
	;lidt [idtr]
	;sti
	cli

	call idt_init
	call pic_remap
	lidt [idtr]

	sti

  call init_mem
  call enable_vaddr

  ; Shouldn't get here because of far jump in enable_vaddr
  jmp freeze


main:
    ; call fill_green_screen
    sti

    mov esi, wel32Msg 	; Setting PM welcome message to be printed
	call terminal_write_string	; Printing PM Welcome message
    call cursor_newline

    ; First we need to set our bitmap for memory allocated 
    ; Currently we have kernel code [org 0x90000] index 44
    ; and we have bootloader [org 0x7c00]  index 7
    ; mov eax, 7
    ; call set_bit

    ; mov eax, 44
    ; call set_bit

    call alloc_page
    call print_eax_hex
    call cursor_newline
    call alloc_page
    call print_eax_hex
    call cursor_newline
    call alloc_page
    call print_eax_hex
    call cursor_newline

    

    ; Testing mapping page
    mov eax, 0xFF000
    mov ebx, 0xb8000
    mov ecx, 0x3
    ; call map_page
    ; call print_eax_hex
    ; call fill_mapped_purple_screen
    mov eax, [0xFF800000]
    call fill_green_screen


main_loop:
	hlt
	jmp main_loop

print_eax_hex:
    pusha

    mov edx, eax      ; copy value
    mov ecx, 8

.loop:
    rol edx, 4
    mov ebx, edx
    and ebx, 0xF

    cmp bl, 9
    jg .letter

    add bl, '0'
    jmp .print

.letter:
    add bl, 'A' - 10

.print:
    mov al, bl
    call terminal_putchar

    loop .loop

    popa
    ret

	;sti
 	; cli
	; mov al, 11111111b
	; out 0x21, al
	; out 0xA1, al

 	; int 32
	; jmp $

	;call fill_blue_screen
	;call cursor_init
	; mov edi, wel32Msg 	; Setting PM welcome message to be printed
	; mov ecx, wel32MsgLen ; Length of the message
	; call terminal_write	; Printing PM Welcome message
	; call cursor_newline
	; mov edi, wel32Msg 	; Setting PM welcome message to be printed
	; call terminal_write_string ; Testing terminal_write_string with null-terminated string
	; mov edi, wel32Msg 	; Setting PM welcome message to be printed
	; mov ecx, 0x05
	; call terminal_writeln ; Testing terminal_writeln with null-terminated string

	; call terminal_backspace
	; call terminal_backspace
	; call terminal_backspace
	; mov al, 'A'
	; call terminal_putchar ; Testing terminal_putchar with null-terminated string

	; call terminal_backspace
	; call terminal_backspace
	; call terminal_backspace
	; mov al, 'A'
	; call terminal_putchar ; Testing terminal_putchar with null-terminated string

	; 	call terminal_backspace
	; call terminal_backspace
	; call terminal_backspace
	; mov al, 'A'
	; call terminal_putchar ; Testing terminal_putchar with null-terminated string

	; call terminal_backspace
	; call terminal_backspace
	; call terminal_backspace
	; mov al, 'A'
	; call terminal_putchar ; Testing terminal_putchar with 

	;test:
	;hlt
	;jmp test



fill_green_screen:
  mov edi, 0xb8000 ; Vga memeory start address
  mov ecx, 2000 ; 80 * 25 cells
  mov al, ' '          ; space character
  mov ah, 0xA9         ; white text on blue background
.loop:
  mov [edi], ax        ; write char + color
  add edi, 2
  loop .loop

  ret

fill_mapped_purple_screen:
  mov edi, 0xFF000 ; Vga memeory start address
  mov ecx, 2000 ; 80 * 25 cells
  mov al, ' '          ; space character
  mov ah, 0xA9         ; purple background
.loop:
  mov [edi], ax        ; write char + color
  add edi, 2
  loop .loop

  ret

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

print_counter:
    pusha
    mov eax, ecx
    call print_eax_hex
    call cursor_newline

    popa
    ret

freeze:
	jmp freeze

%include "cursor.asm"
%include "interrupt.asm"
%include "terminal.asm"
%include "mem.asm"


section .data
wel32Msg db "Testing cursor", 0
; wel32MsgLen equ $ - wel32Msg
wel32MsgLen equ 5

idtr:
	dw idt_end - idt - 1
	dd idt

page_fault_msg db "Page fault at address: ", 0

PIC1_COMMAND equ 0x20
PIC1_DATA equ 0x21
PIC2_COMMAND equ 0xA0
PIC2_DATA equ 0xA1

ICW1_INIT equ 0x10
ICW1_ICW4 equ 0x01
ICW4_8086 equ 0x01

scancode_table:
db 0              ; 0x00
db 27             ; ESC 0x01
db '1'            ; 0x02
db '2'            ; 0x03
db '3'            ; 0x04
db '4'            ; 0x05
db '5'            ; 0x06
db '6'            ; 0x07
db '7'            ; 0x08
db '8'            ; 0x09
db '9'            ; 0x0A
db '0'            ; 0x0B
db '-'            ; 0x0C
db '='            ; 0x0D
db 0              ; backspace 0x0E
db 0              ; tab 0x0F
db 'q'            ; 0x10
db 'w'
db 'e'
db 'r'
db 't'
db 'y'
db 'u'
db 'i'
db 'o'
db 'p'
db '['
db ']'
db 0              ; enter is special (0x1C later)
db 0
db 'a'
db 's'
db 'd'
db 'f'
db 'g'
db 'h'
db 'j'
db 'k'
db 'l'
db ';'
db "'"
db '`'
db 0
db '\'
db 'z'
db 'x'
db 'c'
db 'v'
db 'b'
db 'n'
db 'm'
db ','
db '.'
db '/'
times 128 - ($ - scancode_table) db 0

section .bss
align 16
stack_bottom:
    resb 4096
stack_top:

align 16
idt:
resb 256*8 ; 256 entries, each 8 bytes
idt_end:

cursor_x resb 1
cursor_y resb 1

