#include <libc.h>

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


extern void runjp();
int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
      //__asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); 
	

	w("Hello from userland\n");
	int p = fork();
	
	if (p == 0) {
		w("  Child");		
	} else {
		w("  Parent");
		int p2 = fork();
		
		if (p2 == 0) {
			w("   Child 2");		
		} else {
			w("  Parent After 2nd child");
		}
	}

	while(1){
		continue;
		char buf[4];
		itoa(p, buf);
		write(1, buf, strlen(buf));
		write(1, " ", 1);
	}


	return 0;
}

