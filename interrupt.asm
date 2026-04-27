[bits 32]

extern i_keyboard

section .text
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

    mov eax, isr14
    mov ebx, 14
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

  ;in al, 0x60

  ; ignoring relase key
  ;test al, 0x80
  ;jnz .done

  ; Convertin from scancode to ascii
  ;movzx ebx, al
  ;mov al, [scancode_table + ebx]

  ; Ignoring unmapped keys
  ;cmp al, 0
  ;je .done

  ;call terminal_putchar 
;.done:
  ;mov al, 0x20
  ;out 0x20, al
  call i_keyboard
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

isr14:
    pusha

    mov esi, page_fault_msg
    call terminal_write_string

    mov eax, cr2
    
    call print_eax_hex
    popa
    add esp, 4     ; discard error code
    call freeze
