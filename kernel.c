#include "kheap.h"
#include "kprintf.h"
#include "keyboard.h"
#include "kshell.h"
#include "kproc.h"
#include "timer.h"
#include "mem.h"
#include "gdt.h"
#include "user.h"

extern tss_t tss;

void terminal_write(char *addr, unsigned long int length);
void terminal_pchar(char c);
void jump_usermode(unsigned long esp, unsigned long eip);

void *alloc_page(void);
void read_sector(unsigned long sector_num, void *buffer);

void user_program_start(void);
void user_program_finish(void);


void task_a(void) {
  while (1) {
    for (int i = 0; i < 999999999999; i++) {
      kprintf("A", i);
    }
  }
}

void shell_task(void) {
  // Shell loop
  // Steps
  // 1- Prompt
  // 2- read line
  // 3- parse command
  // 4- exec command
  // 5- clear buffer
  while (1) {
    print_prompt();

    int command_complete = 0;
    while (!command_complete) {
      char c = get_char();
      switch (c) {
        case '\n':
          command_complete = 1;
          kprintf("\n");
          break;
        default:
          kprintf("%c", c);
          add_char(c);
      }
    }

    // Parse the command
    execute_command();
    clear_buffer();
  }
}
void idle_task_main(void) {
  kprintf("IDLE");
  while (1) {
  }
}


/*
void launch_user_mode(void) {
  void *user_stack = kmalloc(STACK_SIZE);
  unsigned long esp = (unsigned long)user_stack + STACK_SIZE;
  unsigned long eip = (unsigned long)user_space;

  map_page((unsigned long)user_stack, (unsigned long)user_stack, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
  map_page((unsigned long)eip & 0xFFFFF000, (unsigned long)eip & 0xFFFFF000, PAGE_PRESENT | PAGE_USER);

  proc *current_proc = get_proc(0);
  tss.esp0 = current_proc->kernel_stack_base;

  jump_usermode(esp, eip);
}
*/

// This is actually called by prog.asm
void kernel_setup(void) {
  gdt_init();
}

void kernel_main(void) {
  disable_interrupts();

  setup_tss();
  kmalloc_init();

  // schedule_task(task_a);
  schedule_task(idle_task_main);


  unsigned long size = (unsigned long)(&user_program_finish - &user_program_start);
  void *user_prog_stack = alloc_page();
  unsigned char *user_prog_stack_base = ((unsigned char*)user_prog_stack) + 4096;

  unsigned char *user_prog_code = (unsigned char*)alloc_page();
  //memcp(user_prog_code, user_program_start, size);
  read_sector(1025, user_prog_code);


  // map_page((unsigned long)user_prog_stack, (unsigned long)user_prog_stack, PAGE_PRESENT | PAGE_WRITE | PAGE_USER);
  // map_page((unsigned long)user_prog_code, (unsigned long)user_prog_code, PAGE_PRESENT | PAGE_USER);

  // proc *current_proc = get_proc(0);
  // tss.esp0 = current_proc->kernel_stack_base;


  kprintf("\nSTART\n");
  init_scheduler();
  schedule_task((void (*)(void))user_prog_code);
  schedule_task(shell_task);

  // proc *task_a = get_proc(0);
  // task_a->task_state = TASK_RUNNING;

  enable_interrupts();
  // jump_usermode((unsigned long)user_prog_stack_base, (unsigned long)user_prog_code);
  start_task();
  //
  kprintf("\nKERNEL DONE");
  while(1) { }
}
