#ifndef K_SHELL
#define K_SHELL

#define K_SHELL_BUFFER_SIZE 128

void print_prompt(void);
void clear_buffer(void);
void add_char(char c);
char *get_buffer_str(void);

typedef struct command {
  const char *cname;
  void (*func)(void);
} command_t;


void execute_command(void);

#endif // !K_SHELL
