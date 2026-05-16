#include "kprintf.h"
#include "kproc.h"

#define SYS_TIMER 0x1
#define SYS_YIELD 0x2
#define SYS_TEST  0x3
#define SYS_EXIT  0x4

typedef struct trap_frame {
  // pushed last (lowest address), popped first
  unsigned long trap_no;

  unsigned long gs;
  unsigned long fs;
  unsigned long es;
  unsigned long ds;

  // pusha order
  unsigned long edi;
  unsigned long esi;
  unsigned long ebp;
  unsigned long esp;   // ignored by popa
  unsigned long ebx;
  unsigned long edx;
  unsigned long ecx;
  unsigned long eax;

  // pushed by hardware on interrupt
  unsigned long eip;
  unsigned long cs;
  unsigned long eflags;

  // only present on privilege level change (ring3 -> ring0)
  unsigned long user_esp;
  unsigned long user_ss;
  // First to be pushed
} trap_frame;

unsigned long syscall_handler_c(trap_frame *tf) {
  disable_interrupts();

  // First we are going to start by handling the yield syscall.
  if (tf->trap_no == SYS_TIMER || tf->trap_no == SYS_TIMER) {
    unsigned long current_esp = (unsigned long)tf;
    unsigned long trap_no = tf->trap_no;

    enable_interrupts();
    return schedule(current_esp);  
  } 

  if (tf->trap_no == SYS_TEST) {
    kprintf("SYSCALL TEST");

    enable_interrupts();
    return (unsigned long)tf; // Returning current tf
  }

  if (tf->trap_no == SYS_EXIT) {
    kprintf("SYSCALL EXIT");
    proc *cur_proc = get_cur_proc();
    cur_proc->task_state = TASK_DEAD;
    unsigned long current_esp = (unsigned long)tf;

    enable_interrupts();
    return schedule(current_esp);
  }

  return (unsigned long)tf; // Returning current tf
}
