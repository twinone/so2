#include <libc.h>

#define THREAD_STACK_SIZE 1024

void writepid() {
	int pid = getpid();
	char buf[5];
	itoa(pid, buf);
	write(1, "pid: ", 5);
	write(1, buf, strlen(buf));
	write(1, "\n", 1);
}

void w(char *str) {
	write(1, str, strlen(str));
}

void n(int n) {
	char buf[10];
	itoa(n, buf);
	w(buf);
}

void clone_func() {
	w("I'm Mr. Meeseeks, look at me!");
	exit();
}

long stack[THREAD_STACK_SIZE];

int test_clone() {
	return clone(&clone_func, &stack[THREAD_STACK_SIZE]);
}




void test_sbrk() {
	char *brk = sbrk(4096 * 2);
	brk = sbrk(30);
	brk[2] = 'a';
	brk[3] = '\0';


	int pid = fork();	
	w(&brk[2]);
	if (pid == 0) exit();
}

extern void runjp();
int __attribute__ ((__section__(".text.main")))
  main(void)
{
	w("Hello from userland\n");

	
	
	runjp();

	

	while(1) {}
	return 0;
}

