#include "kprintf.h"
#include "keyboard.h"

char kb_buffer[KB_BUFFER_SIZE];
volatile unsigned int head = 0;
volatile unsigned int tail = 0;

extern unsigned char scancode_table[128];
void kb_push_char(char c);
char kb_pop_char(void);
int has_char(void);

unsigned char inb(unsigned int port);

void i_keyboard(void) {
  unsigned char scancode = inb(0x60);
  char c = scancode_table[scancode];
  if (c) {
    kb_push_char(c);
  }
}

void kb_push_char(char c) {
  unsigned int next = (head + 1) % KB_BUFFER_SIZE;

  // Check if buffer is full
  if (next == tail)
    return;

  kb_buffer[head] = c;
  head = next;
}

char kb_pop_char(void) {
  if (head == tail) {
    // Nothing its empty
    return -1;
  }

  char c = kb_buffer[tail];
  tail = (tail + 1) % KB_BUFFER_SIZE;
  return c;
}

int has_char(void) {
  return head != tail;
}
