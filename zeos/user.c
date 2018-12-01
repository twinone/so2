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

void clone_func() {
	w("I'm Mr. Meeseeks, look at me!");
	exit();
}
long stack[THREAD_STACK_SIZE];

int test_clone() {
	return clone(&clone_func, &stack[THREAD_STACK_SIZE]);
}



	

extern void runjp();
int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
      //__asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); 
	
	w("Hello from userland\n");

	int pid = fork();
	if (pid == 0) {
		w("child start read\n");
		char buf[3];
		read(0, &buf, 3);
		w("child end read\n");
	}
	else {
		yield();
		w("parent getstats start \n");
		struct stats st;
		get_stats(pid, &st);
		w("parent getstats end \n");
	}

	while(1);


	runjp_rank(0, 3); // skip 4-6 they are SO SLOW come on (they are OK)
	runjp_rank(7, 7); // this one hangs
	runjp_rank(8, 10);
	
	
	while(1);
	return 0;
}

