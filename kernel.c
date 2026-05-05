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

void kernel_main(void) {
	char greetings[] = "Hello World!";
	unsigned long int len = 12;
	terminal_write(greetings, len);

  char str[] = "a";
  unsigned long test = 100;
  kprintf("bcccc%d, %ul", 200, test);

  kmalloc_init();

  schedule_task(0);
  schedule_task(1);
  start_task();

  // Shell loop
  // Steps
  // 1- Prompt
  // 2- read line
  // 3- parse command
  // 4- exec command
  // 5- clear buffer
  /*
  while (1) {
    print_prompt();

    int command_complete = 0;
    while (!command_complete) {
      if (has_char()) {
        char c = kb_pop_char();
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
    }

    // Parse the command
    execute_command();
    clear_buffer();
  }
  */
}
