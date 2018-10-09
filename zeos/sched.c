/*
 * sched.c - initializes struct for task 0 and task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <libc.h>


struct task_struct *idle_task;

struct list_head freequeue;
struct list_head readyqueue;



/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;
	// position of the task_struct in the task vector
	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void) {
	__asm__ __volatile__("sti": : :"memory");
	printk("\n\n\n\n\n");
	while(1){
		printk("idle ");
	}
}

void init_idle () {

	// Get the first element of the list
	struct list_head *e = list_first(&freequeue);
	// Remove the selected element from the list
	list_del(e); 
	// Get the container of this list element
	idle_task = list_entry(e, struct task_struct, anchor);
	
	idle_task->PID = 0;
	allocate_DIR(idle_task);

        // initialize the fake context for this process
	// we need to set %esp to &task[1] and push @cpu_idle, push 42 so that task_switch can
	// undo this fake dynamic link
	union task_union *u = (union task_union *) idle_task;
	u->stack[KERNEL_STACK_SIZE-1] = &cpu_idle;
	u->stack[KERNEL_STACK_SIZE-2] = 42;
	
	idle_task->esp = &u->stack[KERNEL_STACK_SIZE-2];
}

void init_task1() {
}


void init_sched() {

	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);


	for (int i = 0; i < NR_TASKS; i++) {
		struct task_struct *el = &task[i];
		// to debug we assign an initial PID ge 10, will be overwritten when task_switching
		el->PID = i;
		list_add_tail(&(el->anchor), &freequeue);
	}

	init_idle();
	init_task1();


	//printk("Current process:\n");
	//char buf[30];
	//itoa(current()->PID, &buf);

	//printk(buf);
	//printk("\n");

}


struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

