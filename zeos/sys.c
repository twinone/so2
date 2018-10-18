/*
 * sys.c - Syscalls implementation
 */

#include <THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING.h>

#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <system.h>
#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

#define FIRST_FREE_PAGE (PAG_LOG_INIT_DATA + NUM_PAG_DATA)


int nextPID = 2;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}



#define BLOCK_SIZE 4
int sys_write(int fd, char *buf, int size) {
	int e = check_fd(fd, ESCRIPTURA);

	if (e) return -e;
	
	if (buf == NULL) return EFAULT;  /* Invalid argument */
	if (size < 0) return EINVAL; // use 0 for flush?
	
	char mybuf[BLOCK_SIZE];
	int offset = 0;
	while (offset + BLOCK_SIZE < size) {
		copy_from_user(buf+offset, mybuf, BLOCK_SIZE);
		sys_write_console(mybuf, BLOCK_SIZE);
		offset += BLOCK_SIZE;
	}
	if (offset < size) {
		int res = size - offset;
		copy_from_user(buf+offset, mybuf, res);
		sys_write_console(mybuf, res);
	}
	return 0;
}

int sys_gettime() {
	return zeos_ticks;
}


int sys_getpid() {
	return current()->PID;
}

extern int getebp();

int ret_from_fork() { return 0; }


void printNum(char *text, int num) {
	char buf2[20];
	itoa(num, buf2);
	printk(text);
	printk(buf2);
	printk("\n");
}

int sys_fork() {
	if (list_empty(&freequeue)) return -EAGAIN;
	struct list_head *e = list_first(&freequeue);

	list_del(e);

	struct task_struct *new_t = list_entry(e, struct task_struct, anchor);
	union task_union *new_u = (union task_union*) new_t;

	struct task_struct *curr_t = current();
	union task_union *curr_u = (union task_union*) curr_t;

	
	copy_data(curr_u, new_u, KERNEL_STACK_SIZE * sizeof(long));
	allocate_DIR(new_t);

	page_table_entry *new_PT =  get_PT(new_t);
	page_table_entry *curr_PT =  get_PT(curr_t);	

	for (int i = 0; i < NUM_PAG_CODE; i++)
		set_ss_pag(new_PT, PAG_LOG_INIT_CODE + i, curr_PT[PAG_LOG_INIT_CODE+i].bits.pbase_addr);
	for (int i = 0; i < NUM_PAG_KERNEL; i++)
		set_ss_pag(new_PT, i, curr_PT[i].bits.pbase_addr);

 	int dataFrames[NUM_PAG_DATA];
	for (int i = 0; i < NUM_PAG_DATA; i++){
		dataFrames[i] = alloc_frame();
		if (dataFrames[i] < 0) {
			for (int j = 0;j < i; j++) free_frame(dataFrames[j]);
			return -ENOMEM;
		}
	}
	for (int i = 0; i < NUM_PAG_DATA; i++) {
		set_ss_pag(new_PT, PAG_LOG_INIT_DATA + i, dataFrames[i]); 
		set_ss_pag(curr_PT, FIRST_FREE_PAGE + i, dataFrames[i]);
		copy_data((PAG_LOG_INIT_DATA + i)*PAGE_SIZE, (FIRST_FREE_PAGE+i)*PAGE_SIZE, PAGE_SIZE); 
		del_ss_pag(curr_PT, FIRST_FREE_PAGE + i);
		set_cr3(curr_t->dir_pages_baseAddr);
	}

	new_t->PID = nextPID++;

	int ebp_offset = getebp() & 0xfff;
	int new_ebp = ebp_offset + (int)new_t;

	new_u->task.kernel_esp = new_ebp - 1*sizeof(long);
	new_u->stack[ebp_offset/sizeof(long) - 0] = &ret_from_fork;
	new_u->stack[ebp_offset/sizeof(long) - 1] = THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING;

	new_t->state = ST_READY;
	list_add_tail(&new_t->anchor, &readyqueue);
	return new_t->PID;
}

void sys_exit() {
}
