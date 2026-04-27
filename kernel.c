#include "kheap.h"


void terminal_write(char *addr, unsigned long int length);
void terminal_pchar(char c);

void kprintf(char *str, ...) {
  // Currently we are only supporting chars
  char **argPtr = (char**)&str + 1;

  for (char *p = str; *p != '\0'; p++) {
    if (*p == '%' && *(p+1) != '\0') {
      p++;
      // Handle special char
      if (*p == 'd') {
        int num = *(int*)argPtr;
        terminal_pchar((char)('0'+num));
        argPtr++;
      }

    } else {
      terminal_pchar(*p);
    }
  }
}


void kernel_main(void) {
	char greetings[] = "Hello World!";
	unsigned long int len = 12;
	terminal_write(greetings, len);

  char str[] = "a";
  kprintf("bcccc%d", 2);

  kmalloc_init();

  char *charPtr = (char *)kmalloc(sizeof(char) * 13);
  charPtr[0] = 'H';
  charPtr[1] = 'E';
  charPtr[2] = 'L';
  charPtr[3] = 'L';
  charPtr[4] = 'O';
  charPtr[5] = ' ';
  charPtr[6] = 'W';
  charPtr[7] = 'O';
  charPtr[8] = 'R';
  charPtr[9] = 'L';
  charPtr[10] = 'D';
  charPtr[11] = '!';
  charPtr[12] = '\0';
  kprintf(charPtr, 12);
}
