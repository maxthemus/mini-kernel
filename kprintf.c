#include "kprintf.h"

void int_to_buffer(int value, char *buf);
void unsigned_long_to_buffer(unsigned long value, char *buf);
void terminal_write(char *addr, unsigned long int length);
void terminal_pchar(char c);

// ASM functions
void terminal_newline(void);


void kprintf(char *str, ...) {
  // Currently we are only supporting chars
  char buffer[32];
  char **argPtr = (char**)&str + 1;

  for (char *p = str; *p != '\0'; p++) {
    if (*p == '%' && *(p+1) != '\0') {
      p++;
      // Handle special char
      if (*p == 'd') {
        int num = *(int*)argPtr;
        int_to_buffer(num, buffer);
        for (char *i = buffer; *i != '\0';i++) {
          terminal_pchar(*i);
        }
        argPtr++;
      } else if (*p == 'u') {
        p++;
        if (*p == 'l') {
          unsigned long num = *(unsigned long*)argPtr;
          unsigned_long_to_buffer(num, buffer);
          for (char *i = buffer; *i != '\0';i++) {
            terminal_pchar(*i);
          }
        }
        argPtr++;
      } 
    } else {
      // Handling special characters
      if (*p == '\n') {
        terminal_newline();
      } else {
        terminal_pchar(*p);
      }
    }
  }
}


void int_to_buffer(int value, char *buf) {
  int i = 0;
  int negative = 0;
  unsigned int n;

  /* Handle negative numbers */
  if (value < 0) {
    negative = 1;
    n = (unsigned int)(-value);
  } else {
    n = (unsigned int)value;
  }

  /* Special case: zero */
  if (n == 0) {
    buf[i++] = '0';
  } else {
    /* Store digits in reverse order */
    while (n > 0) {
      buf[i++] = '0' + (n % 10);
      n /= 10;
    }
  }

  /* Add minus sign */
  if (negative) {
    buf[i++] = '-';
  }

  /* Null terminate */
  buf[i] = '\0';

  /* Reverse string in place */
  int start = 0;
  int end = i - 1;

  while (start < end) {
    char temp = buf[start];
    buf[start] = buf[end];
    buf[end] = temp;

    start++;
    end--;
  }
}

void unsigned_long_to_buffer(unsigned long value, char *buf) {
    int i = 0;

    /* Special case: zero */
    if (value == 0) {
        buf[i++] = '0';
        buf[i] = '\0';
        return;
    }

    /* Store digits in reverse order */
    while (value > 0) {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }

    /* Null terminate */
    buf[i] = '\0';

    /* Reverse string in place */
    int start = 0;
    int end = i - 1;

    while (start < end) {
        char temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;

        start++;
        end--;
    }
}
