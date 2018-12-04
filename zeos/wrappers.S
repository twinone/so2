#include<asm.h>
ENTRY(write)
	push %ebp
	mov %esp, %ebp

	push %ecx // save ecx
	push %edx // save edx
	push %ebx // save ebx

	mov 16(%ebp), %edx   #size
	mov 12(%ebp), %ecx   #buffer
	mov 8(%ebp), %ebx    #fd

	lea ret, %eax // sysenter return correctly
	push %eax
	push %ebp // fake dynamic link
	movl %esp, %ebp
	mov $4, %eax
	sysenter
ret:    pop %ebp
	add $4, %esp // remove the lea
	
	cmp $0, %eax // check return value of the implementation
	je end_err // if 0, leave it as is, the syscall succeeded
	// if syscall fails, store the reason in errno
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax // return -1
	jmp end
end_err:mov 16(%ebp), %eax // if write succeeds, the written bytes are equal to the size parameter
end:    
	pop %ebx
	pop %edx
	pop %ecx

	mov %ebp, %esp
	pop %ebp
	ret


ENTRY(read)
	push %ebp
	mov %esp, %ebp

	push %ecx // save ecx
	push %edx // save edx
	push %ebx // save ebx

	mov 16(%ebp), %edx   #size
	mov 12(%ebp), %ecx   #buffer
	mov 8(%ebp), %ebx    #fd

	lea ret_read, %eax // sysenter return correctly
	push %eax
	push %ebp // fake dynamic link
	movl %esp, %ebp
	mov $0, %eax
	sysenter
ret_read:
	pop %ebp
	add $4, %esp // remove the lea
	
	cmp $0, %eax // check return value of the implementation
	jge end_err_read // if 0, leave it as is, the syscall succeeded
	// if syscall fails, store the reason in errno
	lea errno, %ebx
	not %eax
	add $1, %eax
	mov %eax, (%ebx)
	mov $-1, %eax // return -1
	jmp end_read
end_err_read:
	mov 16(%ebp), %eax // if write succeeds, the written bytes are equal to the size parameter
end_read:    
	pop %ebx
	pop %edx
	pop %ecx

	mov %ebp, %esp
	pop %ebp
	ret



ENTRY(gettime)
	push %ebp
	mov %esp, %ebp

	lea ret2, %eax // sysenter return correctly
	push %eax
	push %ebp // fake dynamic link
	movl %esp, %ebp
	mov $4, %eax
	sysenter
ret2:   pop %ebp
	add $4, %esp

   	mov %ebp, %esp
	pop %ebp
	ret

ENTRY(getpid)
	push %ebp
	mov %esp, %ebp

	lea ret3, %eax // sysenter return correctly
	push %eax
	push %ebp // fake dynamic link
	movl %esp, %ebp
	mov $20, %eax
	sysenter
ret3:   pop %ebp
	add $4, %esp

   	mov %ebp, %esp
	pop %ebp
	ret

ENTRY(fork)
	push %ebp
	mov %esp, %ebp

	lea ret4, %eax // sysenter return correctly
	push %eax
	push %ebp // fake dynamic link
	movl %esp, %ebp
	mov $2, %eax
	sysenter
ret4:	pop %ebp
	add $4, %esp // remove the lea
	cmp $0, %eax // check return value of the implementation
	jge end4 // if 0, leave it as is, the syscall succeeded
	// if syscall fails, store the reason in errno
	lea errno, %ebx
	// we have to make the return value positive
	not %eax
	add $1, %eax
	mov %eax, (%ebx)
	mov $-1, %eax // return -1
end4:    
	mov %ebp, %esp
	pop %ebp
	ret

ENTRY(clone)
	push %ebp
	mov %esp, %ebp

	push %ebx
	push %ecx

	mov 12(%ebp), %ecx   #stack
	mov 8(%ebp), %ebx    #function

	lea ret42, %eax // sysenter return correctly
	push %eax
	push %ebp // fake dynamic link
	movl %esp, %ebp
	mov $19, %eax
	sysenter
ret42:	pop %ebp
	add $4, %esp // remove the lea

	pop %ecx
	pop %ebx

	cmp $0, %eax // check return value of the implementation
	jge end42 // if 0, leave it as is, the syscall succeeded
	// if syscall fails, store the reason in errno
	lea errno, %ebx
	// we have to make the return value positive
	not %eax
	add $1, %eax
	mov %eax, (%ebx)
	mov $-1, %eax // return -1
end42:    
	mov %ebp, %esp
	pop %ebp
	ret



ENTRY(yield)
	push %ebp
	mov %esp, %ebp

	lea ret_yield, %eax
	push %eax
	push %ebp // fake dynamic link
	movl %esp, %ebp
	mov $42, %eax
	sysenter
ret_yield:  
	pop %ebp
	add $4, %esp

   	mov %ebp, %esp
	pop %ebp
	ret




ENTRY(exit)
	push %ebp
	mov %esp, %ebp

	lea ret5, %eax // sysenter return correctly
	push %eax
	push %ebp // fake dynamic link
	movl %esp, %ebp
	mov $1, %eax
	sysenter
ret5:   pop %ebp
	add $4, %esp

   	mov %ebp, %esp
	pop %ebp
	ret

ENTRY(get_stats)
	push %ebp
	mov %esp, %ebp

	push %ecx // save ecx
	push %ebx // save ebx

	mov 12(%ebp), %ecx #stats*
	mov 8(%ebp), %ebx #pid

	lea ret6, %eax // sysenter return correctly
	push %eax
	push %ebp // fake dynamic link
	movl %esp, %ebp
	mov $35, %eax
	sysenter
ret6:	pop %ebp
	add $4, %esp // remove the lea

	pop %ebx // save ebx
	pop %ecx // save ecx

	cmp $0, %eax // check return value of the implementation
	je end6 // if 0, leave it as is, the syscall succeeded
	// if syscall fails, store the reason in errno
	lea errno, %ebx

	// invert return value
	not %eax
	add $1, %eax

	mov %eax, (%ebx)
	mov $-1, %eax // return -1
end6:    
	mov %ebp, %esp
	pop %ebp
	ret


ENTRY(sbrk)
	push %ebp
	mov %esp, %ebp
	push %ebx
	mov 8(%ebp), %ebx
	lea ret_brk, %eax
	push %eax
	push %ebp 
	movl %esp, %ebp
	mov $45, %eax
	sysenter
ret_brk:
	pop %ebp
	add $4, %esp 
	pop %ebx
	cmp $0, %eax
	jge end_brk 
	lea errno, %ebx
	not %eax
	add $1, %eax
	mov %eax, (%ebx)
	mov $-1, %eax
end_brk:    
	mov %ebp, %esp
	pop %ebp
	ret







// SEMAPHORES
ENTRY(sem_init)
	push %ebp
	mov %esp, %ebp
	push %ecx
	push %ebx
	mov 12(%ebp), %ecx   // value
	mov 8(%ebp), %ebx    // id
	lea ret_sem, %eax
	push %eax
	push %ebp
	movl %esp, %ebp
	mov $21, %eax
	sysenter

ENTRY(sem_wait)
	push %ebp
	mov %esp, %ebp
	push %ebx
	mov 8(%ebp), %ebx // id
	lea ret_sem, %eax
	push %eax
	push %ebp
	movl %esp, %ebp
	mov $22, %eax
	sysenter

ENTRY(sem_signal)
	push %ebp
	mov %esp, %ebp
	push %ebx
	mov 8(%ebp), %ebx // id
	lea ret_sem, %eax
	push %eax
	push %ebp
	movl %esp, %ebp
	mov $23, %eax
	sysenter


ENTRY(sem_destroy)
	push %ebp
	mov %esp, %ebp
	push %ebx
	mov 8(%ebp), %ebx // id
	lea ret_sem, %eax
	push %eax
	push %ebp
	movl %esp, %ebp
	mov $24, %eax
	sysenter

// SINGLE return point for all semaphores
ret_sem:
    	pop %ebp
	add $4, %esp
	cmp $0, %eax
	je end_sem
	lea errno, %ebx
	not %eax
	add $1, %eax
	mov %eax, (%ebx)
	mov $-1, %eax
end_sem:
	pop %ebx
	pop %ecx
	mov %ebp, %esp
	pop %ebp
	ret

