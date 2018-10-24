/*
 * sched.c - initializes struct for task 0 and task 1
 */

#include <THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING.h>
#include <sched.h>
#include <mm.h>
#include <io.h>
#include <libc.h>


struct task_struct *idle_task;

struct list_head freequeue;
struct list_head readyqueue;

int ticks = 0;



/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, anchor);
}


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
	while(1){printk("i"); for(int i = 0; i < 1000000; i++){printk("");}}
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
	u->stack[KERNEL_STACK_SIZE-1] = (long) &cpu_idle;
	u->stack[KERNEL_STACK_SIZE-2] = THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING;
	
	idle_task->kernel_esp = (int) &u->stack[KERNEL_STACK_SIZE-2];
}

void init_stats(struct task_struct *t) {
	t->state = ST_READY;
	t->stats.user_ticks = 0;
	t->stats.system_ticks = 0;
	t->stats.blocked_ticks = 0;
	t->stats.ready_ticks = 0;
	t->stats.elapsed_total_ticks = 0;
	t->stats.total_trans = 0;
	t->stats.remaining_ticks = DEFAULT_QUANTUM;
}

void update_esp(int esp) {
  	tss.esp0 = esp;
	writeMSR(0x175, esp);
}


void init_task1() {

	struct list_head *e = list_first(&freequeue);
	list_del(e); 
	struct task_struct *t = list_entry(e, struct task_struct, anchor);
	
	t->PID = 1;
	t->state = ST_RUN;
	allocate_DIR(t);
	set_user_pages(t);

        // initialize the fake context for this process
	// we need to set %esp to &task[1] and push @cpu_idle, push 42 so that task_switch can
	// undo this fake dynamic link
	union task_union *u = (union task_union *) t;
	
	
	// 4
	update_esp((int) &u->stack[KERNEL_STACK_SIZE]);


	// 5
	set_cr3(t->dir_pages_baseAddr);

}


void init_sched() {

	INIT_LIST_HEAD(&freequeue);
	INIT_LIST_HEAD(&readyqueue);


	for (int i = 0; i < NR_TASKS; i++) {
		struct task_struct *el = (struct task_struct *) &task[i];

		el->PID = THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING +i;
		el->quantum = DEFAULT_QUANTUM;
		el->state = -THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING;

		list_add_tail(&el->anchor, &freequeue);
	}
}




void inner_task_switch(union task_union *new) {
	update_esp((int)&new->stack[KERNEL_STACK_SIZE]);
	set_cr3(new->task.dir_pages_baseAddr);
	inner_inner_task_switch(&(new->task.kernel_esp), &(current()->kernel_esp));
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


/* Round robin policy implementation */


int needs_sched_rr() {
	return current()->quantum <= ticks;
}

void update_sched_data_rr() {
	ticks++;
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest) {
	if (t->state != ST_RUN) {
		list_del(&t->anchor);
	}
	if (dest != NULL) {
		list_add_tail(&t->anchor, dest);
	}
	if (dest == &readyqueue) {
		t->state = ST_READY;
	} else if (dest == &freequeue) {
		t->state = -THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING;
	} else if (dest == NULL) {
		t->state = ST_RUN;
			//printk("\ninc total trans\n");
		t->stats.total_trans++;
	} else { // unknown list (blocked queue for example)
		t->state = ST_READY; // don't use blocked yet
	}
}


void sched_next_rr() {
	if (list_empty(&readyqueue)) {
		task_switch((union task_union *)idle_task);
		return;
	}
	struct list_head *e = list_first(&readyqueue);
	struct task_struct *t = list_entry(e, struct task_struct, anchor);
		
	update_process_state_rr(t, NULL);
	ticks = 0;
	
	task_switch((union task_union *)t);
}


void schedule() {
	update_sched_data_rr();
	if (needs_sched_rr()) {
		if (list_empty(&readyqueue)) return;
		
		update_process_state_rr(current(), &readyqueue);
		sched_next_rr();
	}
}


int get_quantum(struct task_struct *t) {
	return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum) {
	t->quantum = new_quantum;
}




