#include <libc.h>



int pid;

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

//	writepid();

	//int p = fork();
	
	//if (p == 0) write(1, "child\n", 6);
	//else write(1, "parent\n", 7);

	while(1) { write(1,"a",0);}

	return 0;
}

