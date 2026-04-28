

static volatile unsigned long ticks = 0;

void i_timer_handler(void) {
  ticks++;
}

unsigned long get_ticks(void) {
  return ticks;
}
