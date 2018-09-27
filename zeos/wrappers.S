#include <asm.h>
#include <segment.h>


ENTRY(gettime)
	push   %ebp
	mov    %esp,%ebp

	mov    $10,%eax // gettime syscall
	sysenter

	pop    %ebp
	ret

ENTRY(write)
	push %ebp
	mov  %esp,%ebp

	movl 8(%ebp), %ebx // fd
	movl 12(%ebp), %ecx // buffer
	movl 16(%ebp), %edx // size
	

	movl $4, %eax // 4 is the write syscall
	sysenter

	movl %eax, errno
	pop  %ebp
	ret



