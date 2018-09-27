#include <asm.h>
#include <segment.h>


ENTRY(gettime)
	push   %ebp
	mov    $10,%eax // gettime syscall
	mov    %esp,%ebp
	int    $0x80
	pop    %ebp
	ret

ENTRY(write)
	movl 8(%ebp), %ebx // fd
	movl 12(%ebp), %ecx // buffer
	movl 16(%ebp), %edx // size
	movl $4, %eax // 4 is the write syscall
	int $0x80
	movl %eax, errno
	ret


