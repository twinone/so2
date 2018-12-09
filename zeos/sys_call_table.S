#include <asm.h>
#include <segment.h>

ENTRY (sys_call_table)
	.long sys_ni_syscall	// 0
	.long sys_exit		// 
	.long sys_fork    	// 
	.long sys_read          // ZeOS document read id missing, assuming 3 (like Linux)
	.long sys_write         // 
	.long sys_ni_syscall    // 5
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_gettime    	// 10
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 15
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_clone		// 
	.long sys_getpid    	// 20
	.long sys_sem_init    	// 
	.long sys_sem_wait    	// 
	.long sys_sem_signal    // 
	.long sys_sem_destroy   // 
	.long sys_yield         // 25
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 30
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_get_stats     // 35
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 40
	.long sys_ni_syscall    // 
	.long sys_yield    	// 42 // should be 24 for Linux but that one's occupied by awesome semaphores
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_sbrk          // 45 ZeOS document sbrk id missing, assuming 3 (like Linux's brk())

.globl MAX_SYSCALL
	MAX_SYSCALL = (. - sys_call_table)/4
