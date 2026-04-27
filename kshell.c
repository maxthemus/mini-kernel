#include "kshell.h"
#include "kprintf.h"

// ASM defs
void terminal_clear(void);
void cursor_init(void);

int strcmp(const char *buf_one, const char *buf_two);
void clear_screen(void);

char in_buffer[K_SHELL_BUFFER_SIZE];
static int NUM_COMMANDS = 1;
command_t commands[] = {
  {"clear", clear_screen},
};

volatile unsigned int buf_idx;

void print_prompt(void) {
  kprintf("> ");
}

void clear_buffer(void) {
  buf_idx = 0;
}

void add_char(char c) {
  if (buf_idx + 1 >= K_SHELL_BUFFER_SIZE) {
    return;
  }

  in_buffer[buf_idx++] = c;
}

char *get_buffer_str(void) {
  in_buffer[buf_idx] = '\0';
  return in_buffer;
}


void (*find_command(char *name))(void) {
  for (int i = 0; i < NUM_COMMANDS; i++) {
    if (strcmp(name, commands[i].cname) == 1) {
      return commands[i].func;
    }
  }
  return 0;
}

void execute_command(void) {
  in_buffer[buf_idx] = '\0';
  void (*cmd)(void) = find_command(in_buffer);
  if (cmd) {
    cmd();
  }
}

int strcmp(const char *buf_one, const char *buf_two) {
  int idx = 0;
  // Looping until \0;
  // Checking that buf[idx] match if not return 0
  // if we reach both \0 then return 1
  while (*(buf_one + idx) != '\0') {
    // Check if buf_two has terminated
    if (*(buf_two +idx) == '\0') {
      return 0;
    }

    if(*(buf_one +idx) != *(buf_two + idx)) {
      return 0;
    }

    idx++;
  }

  // We can confirm that buf_one is a substring or same as buf_two;
  return 1;
}


void clear_screen(void) {
  terminal_clear();
  cursor_init();
}
