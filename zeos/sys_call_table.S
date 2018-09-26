#include <asm.h>
#include <segment.h>

ENTRY (sys_call_table)
	.long sys_ni_syscall    // 0
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_write         // 
	.long sys_ni_syscall    // 5
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_ni_syscall    // 
	.long sys_gettime    	// 10

.globl MAX_SYSCALL
	MAX_SYSCALL = (. - sys_call_table)/4
