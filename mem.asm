[bits 32]

; This script will be use for setting up meme for the machine
; Current mapped memory in system 
;
; [org 0x7c00] Boot loader 1 sector 512 bytes
; [org 0x90000] Kernel entry code 2 sectors 2*512 bytes
; I am going to start out with a identity mapped memory 
; meaning that Virtal Adr === Physical Adr
;
; Virtual address structure
; P == Page i.e (Pdir == Page Dir)
; 32 bits : [31-22 PDir][21-12 PTbl][11-0 Offset]
; Because Pdir and Ptbl are only 10 bits we can only have a max of 2^10 (1024) Entries in these parts.
; Because Offset is 12 bits we can have 4K pages because 2^12 (4096)
;
;
 
section .text

init_mem:
  ; Currently init mem is just going to init the first page dir and its page table.
  ; We need to create a page dir entry that maps to 'first_page_table' 
  ; then fill in this page table to map the first 1024 entries of mem
  pusha
  
  ; Creating first entry in page dir
  mov eax, first_page_table
  or eax, 0x3
  mov dword [page_directory], eax

  ; Mapping the first page identity mapping the first table
  ; Loop through 1024 entries in pTable mapping to 4K Pages
  ; Incrementing 0 -> 4096 -> 8192 ...
  mov ecx, 0; Counter set to 0

.fill_table_entry:
  mov eax, ecx ; Counter set to 0
  shl eax, 12 ; Incrementing by 4096
  or eax, 0x3 ; present + writable

  mov [first_page_table + ecx*4], eax

  inc ecx
  cmp ecx, 1024
  jne .fill_table_entry

  popa
  ret


enable_vaddr:
  pusha

  cli ; Stopping interrupt for safety


  ; Loading page dir into cr3
  mov eax, page_directory
  mov cr3, eax

  ; Enabling paging in cr0
  mov eax, cr0
  or eax, 0x80000000 
  mov cr0, eax

  popa
  jmp 0x08:main ; far jump for flush


;page_index = physical_address / 4096
;byte_index = page_index / 8
;bit_index  = page_index % 8
set_bit:
  ; eax = page index

  mov ebx, eax
  shr ebx, 3
  and eax, 7

  mov cl, al
  mov dl, 1
  shl dl, cl

  or [bitmap + ebx], dl
  ret

clear_bit:
; eax = page index

  mov ebx, eax
  shr ebx, 3
  and eax, 7

  mov cl, al
  mov dl, 1
  shl dl, cl
  not dl

  and [bitmap + ebx], dl
  ret

test_bit:
  ; eax = page index
  ; ret eax = 0 if free, 1 if used
  mov ebx, eax
  shr ebx, 3

  mov ecx, eax
  and ecx, 7

  mov dl, 1
  shl dl, cl

  test byte [bitmap + ebx], dl
  setnz al        ; AL = 1 if used, 0 if free
  movzx eax, al
  ret

alloc_page:
    mov ecx, 0

.loop:
    mov eax, ecx
    call test_bit
    jnz .next

.found:
    mov eax, ecx
    push eax
    call set_bit
    pop eax

    mov eax, ecx
    shl eax, 12
    ret

.next:
    inc ecx
    cmp ecx, bitmap_size
    jne .loop

.freeze:
    call .freeze

free_page:
  ; eax = page index to free
  pusha 
  call clear_bit  
  popa
  ret

map_page:
    ; EAX = virt
    ; EBX = phys
    ; ECX = flags

  push ebx

    mov esi, eax
    mov edx, ecx

    ; -------------------------
    ; dir index
    ; -------------------------
    mov eax, esi
    shr eax, 22
    mov ebx, eax

    ; -------------------------
    ; get PDE
    ; -------------------------
    mov eax, [page_directory + ebx*4]
    test eax, 1
    jnz .pde_ok

    ; -------------------------
    ; allocate page table
    ; -------------------------
    call alloc_page        ; EAX = phys page for page table

    push eax               ; save page table base

    mov edi, eax
    xor eax, eax
    mov ecx, 1024
    rep stosd              ; clear page table

    pop eax                ; restore page table base

    or eax, 0x003          ; present + writable
    mov [page_directory + ebx*4], eax

.pde_ok:
    ; -------------------------
    ; reload page table base
    ; -------------------------
    mov eax, [page_directory + ebx*4]
    and eax, 0xFFFFF000
    mov edi, eax

    ; -------------------------
    ; table index
    ; -------------------------
    mov eax, esi
    shr eax, 12
    and eax, 0x3FF

    ; -------------------------
    ; write PTE
    ; -------------------------
    mov ebx, [esp]
    and ebx, 0xFFFFF000
    or ebx, edx

    mov [edi + eax*4], ebx

    ; flush
    invlpg [esi]

    pop ebx

    ret

unmap_page:
  ; EAX = virt
  pusha

  mov esi, eax

  ; dir index
  mov eax, esi
  shr eax, 22
  mov ebx, eax

  ; get PDE
  mov eax, [page_directory + ebx*4]
  test eax, 1
  jz .unmap_done

  ; page table base
  and eax, 0xFFFFF000
  mov edi, eax

  ; table index
  mov eax, esi
  shr eax, 12
  and eax, 0x3FF
  mov ebx, eax

  ; get PTE
  mov eax, [edi + ebx*4]
  test eax, eax
  jz .skip_free

  and eax, 0xFFFFF000
  call free_page

.skip_free:
  ; clear PTE
  mov dword [edi + ebx*4], 0

  ; flush TLB
  invlpg [esi]

.unmap_done:
  popa
  ret


section .bss
align 4096
  
page_directory:
  resd 1024
  
first_page_table:
  resd 1024

align 4
bitmap:
  resb 512 ; supports 4096 pages

bitmap_size equ 4096


