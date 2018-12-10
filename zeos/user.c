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

void test_block_read_cutre() {
	int pid = fork();
	char buf[4];
	if (pid != 0) {
		w("parent start\n");
		// replace this with yield() syscall (already implemented)
		while (1) {
			struct stats st;
			get_stats(getpid(), &st);
			if (st.total_trans > 1) break;
		}

		w("parent after yield\n");
		struct stats st;
		get_stats(getpid(), &st);
		w("parent end\n");
		
	} else {
		w("child start\n");
		read(0, buf, 3);
		w("child end, read: "); w(buf); w("\n");
	}
}

extern void runjp();
int __attribute__ ((__section__(".text.main")))
  main(void)
{
	w("Hello from userland\n");

	// this works correctly:
	//test_block_read_cutre();

	/*

In the following output the dots (...) are cpu_idle routine:

* Running Test [7] : [testStatsBlocked] READ(get stats of a process blocked by reading keyboard )
 sys_fork

 sys_read start
 
Waiting a context switch...

Current Ready->Run transitions: 1; Remain quantum: 42
Looping until Context Switch...

Final Ready->Run transitions: 2; Remain quantum: 42; Iterations: 25001/1000000
USER INTERACTION NEEDED: Enter 5 characters '12345'..............12..3.4..5
 sys_read end
..............................................

	*/
	int pid = fork();
	if (pid>0){
		int i = sem_init(2,0);
		char b[5];
		read(0,b,5);
		w("sending signal\n");
		i = sem_signal(2);
		w("send signal\n");
		while(1);	
		
	}	
	else{
		char b;
		w("im the son and its sleepy time\n");
		for(int i =0; i<10000000;++i)b=5;
		int i = sem_wait(2);
		w("i'm awake again\n");
	}


	//runjp_rank(7,7);


	

	while(1) {}
	return 0;
}

