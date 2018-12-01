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
#include <interrupt.h>




#define LECTURA 0
#define ESCRIPTURA 1

#define FIRST_FREE_PAGE (PAG_LOG_INIT_DATA + NUM_PAG_DATA)


int nextPID = 2;
int read_num_pending_chars = 0;

int check_fd(int fd, int permissions)
{
  if (fd != 1 && fd != 0) return -EBADF;
  if (fd == 1 && permissions != ESCRIPTURA) return -EBADF;
  if (fd == 0 && permissions != LECTURA) return -EACCES;
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


int sys_read_keyboard(char *user_buf, int count) {

	if (!list_empty(&keyboardqueue)) {
		// somebody else already waiting, we go to wait :(
		update_process_state_rr(current(), &keyboardqueue);
		sched_next_rr();
		// at this point, it's our turn to read, because we got unblocked
		// notice there is NO return statement here!
	}
	
	// at this point this process will be the first in the kb q until it finishes it's read()
	// if the buffer is full: what TODO?
	read_num_pending_chars = count;
	int addr = (int) user_buf;
	while (read_num_pending_chars > 0) {
		if (cbuf_empty(&keyboard_buffer)) {
			// nothing to read yet, wait but WE WAIT FIRST here
			update_process_state_rr_impl(current(), &keyboardqueue, 1);
			sched_next_rr();
			// at this point we are unblocked, which means there is DATA :)
		}

		// gotta buffer the buffer
		int buflen = 8;	
		char buf[buflen]; // this is precious kernel stack space, don't waste it
		int i = 0;
		while (i < buflen && !cbuf_empty(&keyboard_buffer) && read_num_pending_chars > 0) { 
			buf[i] = cbuf_read(&keyboard_buffer);
			read_num_pending_chars--;
			i++;
		}
		printk("\n chars: \n");

		sys_write(1, &buf, i);
		// send the keyboard data to our servers (ehm, I mean to the user)
		copy_to_user(buf, addr, i);
		addr += i;
	}

	return count;
}	

int sys_read(int fd, char *buf, int count) {
	int e = check_fd(fd, LECTURA);
	if(e != 0) return e;
	if (!access_ok(VERIFY_WRITE, buf, count)) {
		return -EFAULT;
	}

	return sys_read_keyboard(buf, count);
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

	if (!access_ok(VERIFY_READ, ret_from_clone, 1)) {
		return -EFAULT;
	}

	if (!access_ok(VERIFY_WRITE, stack, 1)) {
		return -EFAULT;
	}

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
	new_u->stack[KERNEL_STACK_SIZE-2] = (long)stack;
	// set eip
	new_u->stack[KERNEL_STACK_SIZE-5] = (long)ret_from_clone;

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



struct task_struct *ts_from_pid(int pid) {
	for (int i = 0; i < NR_TASKS; i++) {
		if ((&task[i])->task.PID == pid) return (struct task_struct *)&task[i];
	}
	return NULL;
}



int sys_get_stats(int pid, struct stats* st) {

	if (pid < 0)  return -EINVAL;
	if (st == NULL) return -EFAULT;
	struct task_struct *t = ts_from_pid(pid);
	if (t == NULL) return -ESRCH;
	
	if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) {
		return -EFAULT;

	}
	
	if(t->state == ST_READY) {
		int total_ticks = get_ticks();
		t->stats.ready_ticks += total_ticks - t->stats.elapsed_total_ticks;;
		t->stats.elapsed_total_ticks = total_ticks;
	}

	if(t->state == ST_READY) {
		int total_ticks = get_ticks();
		t->stats.blocked_ticks += total_ticks - t->stats.elapsed_total_ticks;
		t->stats.elapsed_total_ticks = total_ticks;

	}	

	copy_to_user(&t->stats, st, sizeof(struct stats));
	return 0;
}

struct semaphore *sem_from_id(int id) {
	for (int i = 0; i < NR_SEMAPHORES; i++) 
		if (semaphores[i].id == id)
			return &semaphores[i];
	return NULL;
}

struct semaphore *get_free_sem() {
	for (int i = 0; i < NR_SEMAPHORES; i++) 
		if (semaphores[i].id < 0)
			return &semaphores[i];
	return NULL;
}

int sys_sem_init(int id, unsigned int value) {
	if (id < 0) return -EINVAL; // invalid id
	if (id >= NR_SEMAPHORES) return -EINVAL;
	if (sem_from_id(id) != NULL) return -EBUSY; // already used
	
	struct semaphore *s = get_free_sem();
	if (s == NULL) return -1; // no free semaphores
	
	s->id = id;
	s->value = value;
	s->owner = current()->PID;


	INIT_LIST_HEAD(&s->procs);

	return 0;
}

int sys_sem_wait(int id) {
	if (id < 0) return -EINVAL; // invalid id
	struct semaphore *s = sem_from_id(id);
	if (s == NULL) return -EINVAL; // invalid sem

	
	if (s->value > 0) {
		s->value--;
	} else {
		update_process_state_rr(current(), &s->procs);
		sched_next_rr();
		return current()->sem_destroyed == 1 ? -1 : 0;
	}

	return 0;
}

int sys_sem_signal(int id) {
	if (id < 0) return -EINVAL; // invalid id
	struct semaphore *s = sem_from_id(id);
	if (s == NULL) return -EINVAL; // invalid sem

	if (list_empty(&s->procs)) {
		s->value++;
	} else {
		struct task_struct *t = list_head_to_task_struct(list_first(&s->procs));
		update_process_state_rr(t, &readyqueue);
		if (t->sem_destroyed) return -1;
	}

	return 0;
}

int sys_sem_destroy(int id) {
	if (id < 0) return -EINVAL; // invalid id
	struct semaphore *s = sem_from_id(id);
	if (s == NULL) return -EINVAL; // invalid sem
	if (s->owner != current()->PID) return -1; // not owner
	
	while (!list_empty(&s->procs)) {
		struct task_struct *t = list_head_to_task_struct(list_first(&s->procs));
		update_process_state_rr(t, &readyqueue);
		t->sem_destroyed = 1;
	}

	s->id = -THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING;
	return 0;
}


void sys_exit()
{
	free_user_pages(current());
	update_process_state_rr(current(), &freequeue);

	// release any semaphores owned by this process
	for (int i = 0; i < NR_SEMAPHORES; i++)
		if (semaphores[i].owner == current()->PID)
			sys_sem_destroy(semaphores[i].id);

	current()->PID = -THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING;
	sched_next_rr();
	printk("\n u fucked up\n");	//execution never reaches here

}





