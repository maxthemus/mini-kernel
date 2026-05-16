[bits 32]


global _start
_start:
user_loop:
  mov eax, 3
  int 0x81
  ;mov eax, 0x90000
  ;mov ebx,[eax]
  ;jmp user_loop
  mov eax, 4
  int 0x81
freeze:
  jmp freeze
