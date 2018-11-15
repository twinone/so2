/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <mm_address.h>
#include <stats.h>
#include <THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING.h>

#define NR_SEMAPHORES 20
#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024
#define DEFAULT_QUANTUM THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE_THE_UNIVERSE_AND_EVERYTHING


enum state_t { ST_RUN, ST_READY, ST_BLOCKED, ST_ZOMBIE };

struct task_struct {
	// THIS STRUCTURE HAS ITS FIELDS ACCESSED IN ASSEMBLY
	// ALWAYS ADD NEW FIELDS TO THE END OF THIS STRUCTURE AND NEVER REMOVE ANYTHING
	int PID; // used in asm
	page_table_entry *dir_pages_baseAddr; // used in asm
	int kernel_esp; // used in asm
	struct list_head anchor;
	int quantum;
	enum state_t state;
	struct stats stats;
	int dirPos; // position of the page table entry, set by allocate_DIR
};

union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

extern union task_union protected_tasks[NR_TASKS+2];
extern union task_union *task; /* Vector de tasques */
extern struct task_struct *idle_task;

// Contador de referencias de directorios (para threads)
int refcounter[NR_TASKS];

extern struct list_head freequeue;
extern struct list_head readyqueue;

struct semaphore {
	int id;
	int value;
	int owner; // pid
	struct list_head procs;
};

struct semaphore semaphores[NR_SEMAPHORES];

#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

#define INITIAL_ESP       	KERNEL_ESP(&task[1])

/* Inicialitza les dades del proces inicial */
void init_task1(void);

void init_idle(void);

void init_sched(void);

struct task_struct * current();

void task_switch(union task_union*t);

struct task_struct *list_head_to_task_struct(struct list_head *l);

int allocate_DIR(struct task_struct *t);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;

void inner_task_switch(union task_union *new);
extern void task_switch(union task_union *new);


extern int (*usr_main)(void);
extern void writeMSR(int reg, int val);
extern void inner_inner_task_switch();

extern void save_ebp();

void init_stats(struct task_struct *t);

/* Headers for the scheduling policy */
void sched_next_rr();
void update_process_state_rr(struct task_struct *t, struct list_head *dest);
int needs_sched_rr();
void update_sched_data_rr();
void schedule(); // called every clock tick

#endif  /* __SCHED_H__ */
