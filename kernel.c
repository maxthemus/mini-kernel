#include "kheap.h"
#include "kprintf.h"

void terminal_write(char *addr, unsigned long int length);
void terminal_pchar(char c);


void kernel_main(void) {
	char greetings[] = "Hello World!";
	unsigned long int len = 12;
	terminal_write(greetings, len);

  char str[] = "a";
  unsigned long test = 100;
  kprintf("bcccc%d, %ul", 200, test);

  kmalloc_init();

  char *charPtr = (char *)kmalloc(sizeof(char) * 13);
  char *charPtr_1 = (char *)kmalloc(sizeof(char) * 13);
  char *charPtr_2 = (char *)kmalloc(sizeof(char) * 13);
  char *charPtr_3 = (char *)kmalloc(sizeof(char) * 13);
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
  kfree(charPtr_2);
  kfree(charPtr_1);
  char *charPtr_4 = (char *)kmalloc(sizeof(char) * 13);
  k_heap_dump();


}
