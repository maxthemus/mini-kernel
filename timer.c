#include "timer.h"
#include "kproc.h"
#include "kprintf.h"

#define LAPIC_BASE      0x400000        // your virtual mapping
#define LAPIC_SVR       (0xF0  / 4)    // Spurious Vector Register
#define LAPIC_EOI_REG   (0xB0  / 4)    // EOI Register
#define LAPIC_LVT_TIMER (0x320 / 4)    // LVT Timer Register
#define LAPIC_TDCR      (0x3E0 / 4)    // Timer Divide Config Register
#define LAPIC_TICR      (0x380 / 4)    // Timer Initial Count Register

#define TIMER_VECTOR    32
#define TIMER_PERIODIC  (1 << 17)
#define LAPIC_ENABLE    (1 << 8)

static volatile unsigned long ticks = 0;
volatile unsigned int *lapic = (volatile unsigned int *)LAPIC_BASE;
void lapic_verify(void);
void run_sti(void);

void lapic_init(void) {
    // Enable LAPIC, set spurious vector to 0xFF
    lapic[LAPIC_SVR] = LAPIC_ENABLE | 0xFF;

    // Divide config: divide by 16
    lapic[LAPIC_TDCR] = 0x3;

    // LVT Timer: periodic, vector 32, unmasked
    lapic[LAPIC_LVT_TIMER] = TIMER_PERIODIC | TIMER_VECTOR;

    // Initial count — fires every N bus cycles
    lapic[LAPIC_TICR] = 10000000;
    // lapic_verify();
}

static inline void lapic_eoi(void) {
    lapic[LAPIC_EOI_REG] = 0;
}

void i_timer_handler(void) {
    kprintf("\nISR: %x\n", lapic[0x100 / 4]);
    ticks++;
    lapic_eoi();           // ACK before any work
    kprintf("tick %ul\n", ticks);
    kprintf("\nISR after: %x\n", lapic[0x100 / 4]);

    run_sti();
    //while (1) {}
    sched();
}

unsigned long get_ticks() {
  return ticks;
}

void lapic_verify(void) {
    kprintf("TEST: %x\n", 0xFEE00000);  // does this print 0xFEE00000 correctly?
    kprintf("SVR:       %x\n", lapic[0xF0 / 4]);   // should have bit 8 set
    kprintf("LVT Timer: %x\n", lapic[0x320 / 4]);  // should show your config
    kprintf("TDCR:      %x\n", lapic[0x3E0 / 4]);  // divide config
    kprintf("TICR:      %x\n", lapic[0x380 / 4]);  // initial count
    kprintf("TCCR:      %x\n", lapic[0x390 / 4]);  // current count (counting down?)
}
