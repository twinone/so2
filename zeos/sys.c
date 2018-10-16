/*
 * sys.c - Syscalls implementation
 */
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

extern int ret_from_fork();// { return 0; }



int sys_fork() {
	int PID=-1;

	// a
	if (list_empty(&freequeue)) return -1; // NO FREE PROCESS
	struct list_head *e = list_first(&freequeue);
	list_del(e);

	struct task_struct *new_t = list_entry(e, struct task_struct, anchor);
	union task_union *new_u = (union task_union*) new_t;

	char buf[30]; itoa(new_t->PID, &buf); printk("PID: "); printk(buf); printk("\n");
	
	struct task_struct *curr_t = current();
	union task_union *curr_u = (union task_union*) curr_t;

	// b
	copy_data(curr_u, new_u, KERNEL_STACK_SIZE * sizeof(long));
	// FIXME si peta: Determine whether it is necessary to modify the page table
	// of the parent to access the child’s system data


	// c
	allocate_DIR(new_t);
	
	// d
	int dataFrames[NUM_PAG_DATA];
	for (int i = 0; i < NUM_PAG_DATA; i++) {
		dataFrames[i] = alloc_frame();
		if (dataFrames[i] == -1) {
			// free all allocated pages until now, we won't use them
			for (int j = 0; j <= i; j++) free_frame(dataFrames[j]);
			return -1; // Not enough physical pages left for new process
		}
	}
	

	// e.i
	page_table_entry *new_PT =  get_PT(new_t);
	page_table_entry *curr_PT =  get_PT(curr_t);	
	// e.i.A	
	copy_data(curr_PT, new_PT, NUM_PAG_CODE * sizeof(page_table_entry));
	// e.i.B
	for (int i = 0; i < NUM_PAG_DATA; i++) {
		set_ss_pag(new_PT, PAG_LOG_INIT_DATA + i, dataFrames[i]);
	}

	// e.ii
	int firstFree = NUM_PAG_KERNEL + NUM_PAG_CODE + NUM_PAG_DATA;
	int p = firstFree;
	for (int i = 0; i < NUM_PAG_CODE; i++) {
		// p = first free page (entry != 0)
		while (curr_PT[p].entry != 0 && p < TOTAL_PAGES) p++;
		// we didn't find a page
		if (p == TOTAL_PAGES) {
			if (p == 0) return -1; // no free pages in parent process
			
			// we found a page at some point, but now we're full, flush TLB and start over
			set_cr3(curr_t->dir_pages_baseAddr);
			p = firstFree;
			i--;
			continue;
		}
		// there is a free page
		// e.ii.A
		set_ss_pag(curr_PT, p, curr_PT[PAG_LOG_INIT_DATA+i].bits.pbase_addr);
		// e.ii.B

		copy_data(((PAG_LOG_INIT_DATA+i)*PAGE_SIZE), p*PAGE_SIZE, PAGE_SIZE);
		// e.ii.C
		del_ss_pag(curr_PT, p);

		
		// flush tlb
		set_cr3(curr_t->dir_pages_baseAddr);
	}

	// f
	new_t->PID++; // TODO find a better pid lol

	// g
	// eax = 0 in the child
	new_u->stack[KERNEL_STACK_SIZE-5] = 0; // return 0 to child

	int ebp_offset = getebp() & 0xfff;
	int new_ebp = ebp_offset + new_t;


	new_u->stack[ebp_offset+8] = 0;
	new_u->task.kernel_esp = &new_u->stack[ebp_offset];
	new_u->stack[ebp_offset+1] = &ret_from_fork;



	// h
	
	// i
	list_add_tail(&new_t->anchor, &readyqueue);

	// j
	return PID;
}

void sys_exit() {
}
