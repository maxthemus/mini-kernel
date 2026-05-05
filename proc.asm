[bits 32]

global switch_proc
global run_sti
global start_tasks:

section .text
switch_proc:
  ; context **old, context *new
  mov eax, [esp + 4]
  mov edx, [esp + 8]

  ; Saving register inorder
  push ebp
  push ebx
  push esi
  push edi

  ; Swtiching stacks
  mov [eax], esp
  mov esp, edx

  ; Restoring registers
  pop edi
  pop esi
  pop ebx
  pop ebp

  ret

run_sti:
  sti
  ret


start_tasks:
    ; void start_tasks(unsigned long stackPtr);
    mov eax, [esp + 4]
    mov esp, eax

    popa
    iretd
