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
	//runjp_rank(1,16); // 16 OK
	//runjp_rank(16,20); // 3 OK 2 MED
	//runjp_rank(21, 21); // bad, page fault ???
	//runjp_rank(22, 25); // 3 OK 1 BAD (23: BAD INITTEST: unable to initialize semaphore 0)
	//runjp_rank(26, 32); // 7 OK
	




while(1);


	/*struct stats state;
	w("1\n");
	int pid = fork();
	w("1\n");
	while(1){
	


	if(pid!=0)
	get_stats(getpid(), &state);
	char buf[5];

	itoa(state.user_ticks, buf);
	write(1, "user ticks from parent: ", 22);
	write(1, buf, strlen(buf));
	write(1, "\n", 1);

	itoa(state.ready_ticks, buf);
	write(1, "ready ticks from parent: ", 23);
	write(1, buf, strlen(buf));
	write(1, "\n", 1);


	get_stats(pid, &state);

	itoa(state.user_ticks, buf);
	write(1, "user ticks from child: ", 21);
	write(1, buf, strlen(buf));
	write(1, "\n", 1);*/




	// pseudo fork bomb works
	for (int i = 0; i < 1000; i++) {
		if (fork() == 0) writepid();
		else exit();
	}
	
	
	
	

	while(1){}


	return 0;
}

