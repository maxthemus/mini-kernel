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
	main:
	hlt
	jmp main

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


; PIC REMAP
; IRQ0-7  => INT 32-39
; IRQ8-15 => INT 40-47
pic_remap:
    mov al, ICW1_INIT | ICW1_ICW4
    out PIC1_COMMAND, al
    out PIC2_COMMAND, al

    mov al, 32
    out PIC1_DATA, al

    mov al, 40
    out PIC2_DATA, al

    mov al, 4
    out PIC1_DATA, al

    mov al, 2
    out PIC2_DATA, al

    mov al, ICW4_8086
    out PIC1_DATA, al
    out PIC2_DATA, al

    ; unmask timer + keyboard only
    mov al, 11111100b
    out PIC1_DATA, al

    mov al, 11111111b
    out PIC2_DATA, al

    ret


idt_init:

    ; clear table
    mov edi, idt
    mov ecx, 256*8/4
    xor eax, eax
    rep stosd

	mov ecx,256
    xor ebx,ebx
.fill:
	; Fill with stubs
	mov eax, default_int
    call idt_set_gate
    inc ebx
    loop .fill

    ; exceptions
    mov eax, isr0
    mov ebx, 0
    call idt_set_gate

    mov eax, isr13
    mov ebx, 13
    call idt_set_gate

    ; timer irq
    mov eax, irq0
    mov ebx, 32
    call idt_set_gate

    ; keyboard irq
    mov eax, irq1
    mov ebx, 33
    call idt_set_gate
    
    ret


; idt_set_gate
; eax = handler address
; ebx = vector
idt_set_gate:
    pusha

    mov edi, idt
    mov ecx, ebx
    shl ecx, 3
    add edi, ecx

    mov word [edi], ax
    mov word [edi+2], 0x08
    mov byte [edi+4], 0
    mov byte [edi+5], 10001110b
    shr eax, 16
    mov word [edi+6], ax

    popa
    ret

;ISR handlers
isr0:
    cli
    pusha

    popa
    iretd

; GPF pushes error code
isr13:
    cli
    add esp, 4
	pusha
	popa
	iretd

; IRQ handlers
; Timer handler
irq0:
    pusha

    mov al, 0x20
    out 0x20, al

    popa
    iretd

irq1:
    pusha

	in al, 0x60

	; ignoring relase key
	test al, 0x80
	jnz .done

	; Convertin from scancode to ascii
	movzx ebx, al
	mov al, [scancode_table + ebx]

	; Ignoring unmapped keys
	cmp al, 0
	je .done

	call terminal_putchar 
.done:
    mov al, 0x20
    out 0x20, al

    popa
	iretd

default_int:
    cli
	pusha

	popa 
	iretd


.hang:
    hlt
    jmp .hang

freeze:
	jmp freeze

%include "cursor.asm"
%include "terminal.asm"
;%include "interrupt.asm"


section .data
wel32Msg db "Testing cursor", 0
; wel32MsgLen equ $ - wel32Msg
wel32MsgLen equ 5

PIC1_COMMAND equ 0x20
PIC1_DATA equ 0x21
PIC2_COMMAND equ 0xA0
PIC2_DATA equ 0xA1

ICW1_INIT equ 0x10
ICW1_ICW4 equ 0x01
ICW4_8086 equ 0x01


idtr:
	dw idt_end - idt - 1
	dd idt

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
idt:
resb 256*8 ; 256 entries, each 8 bytes
idt_end:

align 16
stack_bottom:
    resb 4096
stack_top:

cursor_x resb 1
cursor_y resb 1
