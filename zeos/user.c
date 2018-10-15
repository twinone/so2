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
	
//runjp_rank(0, 8); // OK
	//runjp_rank(9, 13); // OK 9-13 WORK
	// runjp_rank(9, 14); // OK 9-14 WORK BUT page fault at the END
	// runjp_rank(14, 14); // OK 14 only, also works (probably bad memory freeing somewhere (where?))

	// runjp_rank(15, 15); // page fault
	// runjp_rank(16, 16); // page fault


	// runjp_rank(17, 17); // does NOT work [forks, no free process available, page fault]
		// we don't understand how this has no free processes in the free queue, since our fork bomb works
	
	// runjp_rank(18, 18); // OK 
	// runjp_rank(19, 20); // medium medium (no errno) // no time to change wrapper now, we need the failing tests to pass
	
	// runjp_rank(21, 21); // BAD & page fault (???)
	// runjp_rank(22, 22); // OK
	// runjp_rank(23, 23); // BAD INITTEST: unable to initialize semaphore 0
	// runjp_rank(24, 32); // OK x9

	// Results:
	// 0-14, 18, 22, 24-32 OK
	// 19, 20 MEDIUM
	// 15, 16, 17, 21, 23: BAD / ERROR

	// SUM:
	// 26 OK
	// 2  MEDIUM
	// 5  BAD / ERROR

	/*
		El scheduling funciona cuando tenemos procesos que escriben su PID, y cuando hacen exit
		se va al idle correctamente.
		
		El fork() funciona (ver fork bomb debajo).

		Muy probablemente tengamos algun memory leak por ahi, o algun fallo en la gestion de listas

		Necesitamos una hora de consulta para estos fallos y ver que tenemos mal con mas detalle.

		Eso si, muy interesante.

	*/
	
	
	while(1);

	// pseudo fork bomb works
	for (int i = 0; i < 1000; i++) {
		if (fork() == 0) writepid();
		else exit();
	}
	
	
	
	

	while(1){}


	return 0;
}

