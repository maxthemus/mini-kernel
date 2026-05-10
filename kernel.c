#include "kheap.h"
#include "kprintf.h"
#include "keyboard.h"
#include "kshell.h"
#include "kproc.h"
#include "timer.h"
#include "mem.h"

void terminal_write(char *addr, unsigned long int length);
void terminal_pchar(char c);

void *alloc_page(void);


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

void kernel_main(void) {
  disable_interrupts();

  kmalloc_init();

  init_scheduler();
  schedule_task(idle_task_main);
  schedule_task(shell_task);
  enable_interrupts();
  start_task();
}
