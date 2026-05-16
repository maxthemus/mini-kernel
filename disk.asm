[bits 32]

global read_sector
read_sector:
    push ebx

    mov ecx, [esp+8]    ; lba
    mov ebx, [esp+12]   ; buffer

    ; drive select + top 4 bits of LBA
    mov al, 0xE0
    mov edx, ecx
    shr edx, 24
    and dl, 0x0F
    or  al, dl
    mov dx, 0x1F6
    out dx, al

    mov al, 1
    mov dx, 0x1F2
    out dx, al

    mov al, cl
    mov dx, 0x1F3
    out dx, al

    mov al, ch
    mov dx, 0x1F4
    out dx, al

    shr ecx, 16
    mov al, cl
    mov dx, 0x1F5
    out dx, al

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

.wait:
    mov dx, 0x1F7
    in  al, dx
    test al, 0x08
    jz  .wait

    mov dx, 0x1F0
    mov ecx, 256
    mov edi, ebx
    rep insw

    pop ebx
    ret
