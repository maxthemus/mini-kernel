#include "kprintf.h"

#define SYS_YIELD

typedef struct T_frame {

} trap_frame;


void syscall_handler_c(unsigned long current_esp) {
  trap_frame *tf = (trap_frame *)current_esp;

  kprintf("SYSCALL HANDLER");
}
