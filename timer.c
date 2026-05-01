#include "kproc.h"
#include "kprintf.h"

#define LAPIC_BASE 0xFF0000
#define EOI 0xB0

void run_sti(void);

static volatile unsigned long ticks = 0;

unsigned int *lapic;

void clear_interrupt(void);

void i_timer_handler(void) {
  kprintf("TIMER CALLED");
  ticks++;
  clear_interrupt();
  sched();
  kprintf("TIMER END");
}

unsigned long get_ticks(void) {
  return ticks;

}

void lapic_init() {
    lapic = (unsigned int*)LAPIC_BASE;
}

void clear_interrupt(void) {
  // run_sti();
    lapic[EOI] = 0;
}
