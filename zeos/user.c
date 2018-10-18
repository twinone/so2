#include <libc.h>

void writepid() {
	int pid = getpid();
	char buf[5];
	itoa(pid, buf);
	write(1, "pid: ", 5);
	write(1, buf, strlen(buf));
	write(1, "\n", 1);
}


extern void runjp();
int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
      //__asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); 
	
	char *str = "Hello from userland\n";
	write(1, str, strlen(str));

	int p = fork();
	while(1){

		char buf[4];
		itoa(p, buf);
		write(1, buf, strlen(buf));
		write(1, " ", 1);
	}


	return 0;
}

