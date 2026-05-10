#ifndef PROC
#define PROC

#define STACK_SIZE 1024

enum P_state {
  TASK_READY,
  TASK_RUNNING,
  TASK_BLOCKED
};

enum B_reasons {
  WAIT_KEYBOARD
};


typedef struct {
    unsigned long esp;
    void *stack_base;
    int pid;
    int run_tick_count;
    enum P_state task_state;
    enum B_reasons blocked_reason;
    unsigned long kernel_stack_base;
} proc;


void sched(void);
void start_task(void);
void store_current_proc(unsigned long stackAddr);
unsigned long load_next_proc(void);
unsigned long schedule(unsigned long current_esp);
void schedule_task(void (*func)(void));
void init_scheduler(void);
void block_current_task(enum B_reasons reason);
void wake_up_tasks(enum B_reasons reason);
void disable_interrupts(void);
void enable_interrupts(void);
proc *get_proc(int idx);


#endif // !DEBUG
