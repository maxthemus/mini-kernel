; TestOS - Simple OS/Bootloader Script 
; To demostrate jumping from 16-Bit Real Mode to 32 Bit Protected mode
; Tested in QEMU
; Simply by setting up only GDT. 
; A20 Gate is not part of of this sccript.
; No keyboard inputs required. I think, That would have made this interactive and better.
; 
; Single grouped Command to Test the script in Linux (Qemu is Required)
; nasm t1.asm -o t1.bin && dd conv=notrunc if=t1.bin of=t1.flp && qemu-system-i386 -fda t1.flp
; No license and Warranty - Use at your own risk after studying the code below.
;
; Have a great day,
; Krishna

BITS 16
org 0x7c00 ; Our code will be loaded here

jmp start

;*******************************************
; Global Descriptor Table (GDT)
;*******************************************

GDT: 
	; null descriptor
	dd 0                
	dd 0 
	
	; code descriptor
	dw 0xFFFF           ; limit low
	dw 0                ; base low
	db 0                ; base middle
	db 10011010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high
	
	; data descriptor
	dw 0xFFFF           ; limit low (Same as code)10:56 AM 7/8/2007
	dw 0                ; base low
	db 0                ; base middle
	db 10010010b        ; access
	db 11001111b        ; granularity
	db 0                ; base high
END_GDT:

; GDT Pointer
GDT_PTR: 
	dw END_GDT - GDT - 1 	; limit (Size of GDT)
	dd GDT 			; base of GDT
	
; Location of entries in GDT
CODE_SEG equ 0x08
DATA_SEG equ 0x10

Print16:
	lodsb
	or al, al
	jz .done
	mov ah, 0xe
	int 0x10
	jmp Print16
.done:
	ret

load:
    mov ah, 0x02
    mov al, 14          ; IMPORTANT: load more sectors
    mov ch, 0
    mov cl, 2
    mov dh, 0

    mov bx, 0x9000
	mov es, bx
	xor bx, bx

    int 0x13
    jc disk_error

    ret

disk_error:
	mov ah, 0x0E
	mov al, 'E'
	int 0x10	

	mov ah, 0x0E
	mov al, 'R'
	int 0x10

	mov ah, 0x0E
	mov al, 'R'
	int 0x10

	jmp hang

hang:
    jmp hang        ; infinite loop


; Entry point in 16 bit Real Mode
; See the jmp instruction at the top of this file
start:
	;-------------------------------;
	;   Setup segments and stack	;
	;-------------------------------;
	
	xor	ax, ax				; null segments
	mov	ds, ax
	mov	es, ax
	mov	ax, 0x9000			; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
	
	; Welcome Message
	mov si, wel
	call Print16
	
	lgdt [GDT_PTR] ; Install GDT
	
	; Jumping to Protected Mode
	; Notifying the user here
	mov si, pms
	call Print16

	call load
	
	; Enabling PM Mode
	cli

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp CODE_SEG:PmodeEntry

	jmp $
	wel db "Welcome! Warming Up ...", 0x0D, 0x0A, 0
	pms db "Now, Enabling and Jumping to protected mode...", 0x0D, 0x0A, 0

[BITS 32]
wel32Msg db "Welcomes to the protected world!", 0

; ENTRY Point/subroutine for Protected mode
PmodeEntry:
	; Reload stack and data segment registers with GDT entry
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov esp, 0x9FFFF


	; jumping to loaded program 0x90000
	jmp CODE_SEG:0x90000
	
	; mov si, wel32Msg 	; Setting PM welcome message to be printed
	; call Print32		; Printing PM Welcome message

	; Halting the system
	cli
	hlt
	
; Sub routine to print message in 32 bit protected mode
Print32:
	;Print to screen
	mov edi, 0xb8000
.repeat:
	lodsb
	or al, al
	jz .done
	mov byte [edi], al
	inc edi
	mov byte [edi], 0x1B
	inc edi
	call .repeat
.done:
	ret
	

times 510 - ($ - $$) db 0  ; pad to 510 bytes
dw 0xAA55                  ; boot signature
