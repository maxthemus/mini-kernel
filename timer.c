#include "timer.h"
#include "kprintf.h"

static volatile unsigned long ticks = 0;

void i_timer_handler(void) {
  kprintf("TIMER");
  ticks++;
}

unsigned long get_ticks() {
  return ticks;
}

