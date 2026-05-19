#include "kproc.h"
#include "kheap.h"
#include "kprintf.h"
#include "gdt.h"

extern tss_t tss;
void run_sti(void);
void start_tasks(unsigned long stackPtr);
void halt_system(void);
void yield(void);
proc *get_cur_proc(void);

/**
 * Logic for context switching using PIC interrupt handler.
 * 
 * INT
 * Store registers on stack.
 * Store stack ptr.
 *
 * clear_interrupt send EOI.
 *
 * Call Interrupt handler
 * - Tick++
 * - call sched
 *   - Find PROC to run.
 *   - Return (unsigned long)stack ptr <- for new program to run.
 * - Return str ptr
 *
 * Store current stack pointer in context for current running proc.
 * Switch Stack ptr to ret ptr from sched.
 * Iretd
 *
 */

int current_idx = 0;
int task_count = 0;

proc tasks[5];
proc *idle_task;

void idle_task_func(void) {
  while (1) {
    // kprintf("I");
  }
}

void start_task(void) {
  tasks[0].task_state = TASK_RUNNING;
  start_tasks(tasks[0].esp);
}

unsigned long schedule(unsigned long current_esp) {
  proc *cur_p = tasks + current_idx;
  cur_p->esp = current_esp;
  // We are coming from interrupt so switch back to running.
  // If we are currently blocked from yield() then we keep it as blocked.
  if (cur_p->task_state == TASK_RUNNING) {
    cur_p->task_state = TASK_READY;
  }

  if (cur_p->task_state == TASK_DEAD) {
    kprintf("MOVING FROM DEAD");
  }


  for (int i = 0; i < task_count; i++) {
    int idx = (current_idx + i + 1) % (task_count);
    proc *p = tasks + idx;
    if (p->task_state == TASK_READY) {
      p->task_state = TASK_RUNNING;
      
      current_idx = idx;
      tss.esp0 = p->kernel_stack_base;
      // kprintf("i:%d", idx);
      return p->esp;
    }
  } 

  // kprintf("IDLE");
  // All procs are currently blocked or no tasks in loop
  idle_task->task_state = TASK_RUNNING;
  tss.esp0 = idle_task->kernel_stack_base;
  return idle_task->esp;
  //cur_p->task_state = TASK_RUNNING;
  //tss.esp0 = cur_p->kernel_stack_base;
  //return cur_p->esp;
}

void schedule_task(void (*func)(void)) {
    proc *t = &tasks[task_count];

    void *stack = kmalloc(STACK_SIZE);
    unsigned long *sp = (unsigned long *)((char *)stack + STACK_SIZE);
    t->kernel_stack_base = (unsigned long)sp;

    // -------------------------
    // IRET frame
    // -------------------------
    *--sp = 0x202;              // EFLAGS
    *--sp = 0x08;              // CS
    *--sp = (unsigned long)func;        // EIP
                                        //
    // -------------------------
    // PUSHA frame (must match popa)
    // -------------------------
    *--sp = 0; // EAX
    *--sp = 0; // ECX
    *--sp = 0; // EDX
    *--sp = 0; // EBX
    *--sp = 0; // ESP (ignored by popa)
    *--sp = 0; // EBP
    *--sp = 0; // ESI
    *--sp = 0; // EDI

    // Kernel is 0x10
    *--sp = 0x23;  // gs (kernel data segment)
    *--sp = 0x23;  // fs
    *--sp = 0x23;  // es
    *--sp = 0x23;  // ds

    *--sp = 0; // Trap no

    t->esp = (unsigned long)sp;
    t->stack_base = stack;
    t->pid = task_count;
    t->run_tick_count = 0;
    t->task_state = TASK_READY;

  task_count++;
}


void init_scheduler(void) {
  proc *t = (proc *)kmalloc(sizeof(proc));

  void *stack = kmalloc(STACK_SIZE);
  unsigned long *sp = (unsigned long *)((char *)stack + STACK_SIZE);
  t->kernel_stack_base = (unsigned long)sp;

  // -------------------------
  // IRET frame
  // -------------------------
  *--sp = 0x202;              // EFLAGS
  *--sp = 0x08;              // CS
  *--sp = (unsigned long)idle_task_func;        // EIP
                                      //
  // -------------------------
  // PUSHA frame (must match popa)
  // -------------------------
  *--sp = 0; // EAX
  *--sp = 0; // ECX
  *--sp = 0; // EDX
  *--sp = 0; // EBX
  *--sp = 0; // ESP (ignored by popa)
  *--sp = 0; // EBP
  *--sp = 0; // ESI
  *--sp = 0; // EDI

  *--sp = 0x10;  // gs (kernel data segment)
  *--sp = 0x10;  // fs
  *--sp = 0x10;  // es
  *--sp = 0x10;  // ds

  *--sp = 0; // Trap no

  t->esp = (unsigned long)sp;
  t->stack_base = stack;
  t->pid = 10;
  t->run_tick_count = 0;
  t->task_state = TASK_READY;

  idle_task = t;
}

void block_current_task(enum B_reasons reason) {
  proc *cur_task = tasks + current_idx;
  switch (reason) {
    case WAIT_KEYBOARD:
      cur_task->task_state = TASK_BLOCKED; 
      cur_task->blocked_reason = reason;
      yield();
      break;
    default:
      kprintf("\nBLOCKED GIVEN INVALID REASON\n");
  }
}

void wake_up_tasks(enum B_reasons reason) {
  for (int i = 0; i < task_count + 1; i++) {
    proc *p = tasks + i;
    if (p->task_state == TASK_BLOCKED) {
      if (p->blocked_reason == reason) {
        p->task_state = TASK_READY;
      }
    }
  }
}

proc* get_proc(int idx) {
  return &tasks[idx];
}

proc* get_cur_proc(void) {
  return &tasks[current_idx];
}
