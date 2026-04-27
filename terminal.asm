[bits 32]

;print_char()
;print_string()
;handle_newline()
;handle_backspace()
;scroll_screen()
;cursor management

;VGA memory (0xB8000)
global terminal_write
global terminal_write_string
global terminal_writeln
global terminal_putchar
global terminal_pchar
global terminal_newline
global terminal_backspace
global terminal_clear

; terminal_write(char* addr, uint32_t length) 
; Currently no support for new line characters and others.
terminal_write:
    pusha 

    mov esi, [esp + 36]
    mov ecx, [esp + 40]

.loop:
    cmp ecx, 0
    je .done

    lodsb                  ; AL = [ESI]
    call cursor_put_char

    dec ecx
    jmp .loop

.done:
    popa
    ret

; Writes a null terminated string to the terminal
; terminal_write_string(ESI)
terminal_write_string:
    pusha 

.loop:
    lodsb                  ; AL = [ESI]
    or al, al
    jz .done

    call cursor_put_char
    jmp .loop

.done:
    popa
    ret


;terminal_writeln(ESI, ECX)
terminal_writeln:
    pusha

    call terminal_write
    call cursor_newline

.done:
    popa
    ret

;terminal_putchar(AL)
terminal_putchar:
    call cursor_put_char
    ret

terminal_pchar:
  mov al, [esp + 4]
  call cursor_put_char
  ret

;terminal_newline()
terminal_newline:
    call cursor_newline
    ret


;terminal_backspace()
terminal_backspace:
    pusha
    call cursor_backspace
    popa 
    ret


;terminal_clear()
terminal_clear:
    pusha

    mov edi, 0xb8000 ; Vga memeory start address
    mov ecx, 2000 ; 80 * 25 cells
    mov al, ' '          ; space character
    mov ah, 0x1F         ; white text on blue background

.loop:
    mov [edi], ax        ; write char + color
    add edi, 2
    loop .loop
.done:
    popa
    ret


