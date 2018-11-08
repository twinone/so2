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



void update_sys_ticks() {
	struct task_struct *t = current();
	int total_ticks= get_ticks();
	t->stats.system_ticks += total_ticks - current()->stats.elapsed_total_ticks;
	t->stats.elapsed_total_ticks=total_ticks;
}

void update_user_ticks() {
	struct task_struct *t= current();
	int total_ticks= get_ticks();
	t->stats.user_ticks += total_ticks - t->stats.elapsed_total_ticks;
	t->stats.elapsed_total_ticks = total_ticks;
}

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

int sys_clone(void (*ret_from_clone)(), void *stack) {
	//printk("\n sys_clone start\n");
	if (list_empty(&freequeue)) {
		printk("\n sys_fork no free process, try again\n");
		return -EAGAIN;
	}
	struct list_head *e = list_first(&freequeue);

	list_del(e);

	struct task_struct *new_t = list_entry(e, struct task_struct, anchor);
	union task_union *new_u = (union task_union*) new_t;

	struct task_struct *curr_t = current();
	union task_union *curr_u = (union task_union*) curr_t;

	
	copy_data(curr_u, new_u, KERNEL_STACK_SIZE * sizeof(long));

	new_t->PID = nextPID++;

	// set esp
	new_u->stack[KERNEL_STACK_SIZE-2] = stack;
	// set eip
	new_u->stack[KERNEL_STACK_SIZE-5] = ret_from_clone;

	int ebp_offset = getebp() & 0xfff;
	int new_ebp = ebp_offset + (int)new_t;

	new_u->task.kernel_esp = new_ebp - 1*sizeof(long);
	new_u->stack[ebp_offset/sizeof(long) - 0] = (long)&ret_from_fork;
	new_u->stack[ebp_offset/sizeof(long) - 1] = THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING;

	init_stats(new_t);
	refcounter[new_u->task.dirPos]++;

	list_add_tail(&new_t->anchor, &readyqueue);
	//printk("\n sys_clone end\n");
	return new_t->PID;
}


int sys_fork() {
	//printk("\n sys_fork start\n");
	if (list_empty(&freequeue)) {
		printk("\n sys_fork no free process, try again\n");
		return -EAGAIN;
	}
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
	printk("\n sys_fork ENOMEM\n");
			return -ENOMEM;
		}
	}
	for (int i = 0; i < NUM_PAG_DATA; i++) {
		set_ss_pag(new_PT, PAG_LOG_INIT_DATA + i, dataFrames[i]); 
		set_ss_pag(curr_PT, FIRST_FREE_PAGE + i, dataFrames[i]);
		copy_data((void*)((PAG_LOG_INIT_DATA + i)*PAGE_SIZE), (void*)((FIRST_FREE_PAGE+i)*PAGE_SIZE), PAGE_SIZE); 
		del_ss_pag(curr_PT, FIRST_FREE_PAGE + i);
	}
	set_cr3(curr_t->dir_pages_baseAddr);

	new_t->PID = nextPID++;

	int ebp_offset = getebp() & 0xfff;
	int new_ebp = ebp_offset + (int)new_t;

	new_u->task.kernel_esp = new_ebp - 1*sizeof(long);
	new_u->stack[ebp_offset/sizeof(long) - 0] = (long)&ret_from_fork;
	new_u->stack[ebp_offset/sizeof(long) - 1] = THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING;

	init_stats(new_t);

	list_add_tail(&new_t->anchor, &readyqueue);
	//printk("\n sys_fork end\n");
	return new_t->PID;
}


void sys_exit()
{
	//printk("\n sys_exit start\n");
	free_user_pages(current());
	update_process_state_rr(current(), &freequeue);
	current()->PID = -THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING;

	sched_next_rr();
	printk("\n u fucked up\n");	//execution never reaches here

}

struct task_struct *ts_from_pid(int pid) {
	for (int i = 0; i < NR_TASKS; i++) {
		if ((&task[i])->task.PID == pid) return (struct task_struct *)&task[i];
	}
	return NULL;
}



int sys_get_stats(int pid, struct stats* st) {
	//printk("\nenter sys_get_stats\n");
	if (pid < 0)  return -EINVAL;
	if (st == NULL) return -EFAULT;
	struct task_struct *t = ts_from_pid(pid);
	if (t == NULL) return -ESRCH; // INVALID PID
	
	// we have to check that the we can actually write to that pointer,
	// or we will have a serious security vuln

	if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) {
		printk("ok\n");
		return -EFAULT;

	}
	

	if(t->state == ST_READY){ //update process t stats on a ready process
		int total_ticks = get_ticks();
		t->stats.ready_ticks += total_ticks - t->stats.elapsed_total_ticks;;
		t->stats.elapsed_total_ticks = total_ticks;
	}

	if(t->state == ST_READY){ //update process t stats on a blocked process
		int total_ticks = get_ticks();
		t->stats.blocked_ticks += total_ticks - t->stats.elapsed_total_ticks;
		t->stats.elapsed_total_ticks = total_ticks;

	}	

	
	copy_to_user(&t->stats, st, sizeof(struct stats));
//	printk("\nexit sys_get_stats\n");

	return 0; // if we return 0 here everything breaks, in the teacher's lib
}












