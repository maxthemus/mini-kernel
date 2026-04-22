
void terminal_write(char *addr, unsigned long int length);


void kernel_main(void) {
	char greetings[] = "Hello World!";
	unsigned long int len = 12;
	terminal_write(greetings, len);

	while (1) { }
}
