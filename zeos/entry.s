# 1 "entry.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "entry.S"




# 1 "include/asm.h" 1
# 6 "entry.S" 2
# 1 "include/segment.h" 1
# 7 "entry.S" 2
# 1 "include/errno.h" 1




# 1 "/usr/include/asm-generic/errno-base.h" 1 3 4
# 6 "include/errno.h" 2
# 8 "entry.S" 2
# 74 "entry.S"
.globl keyboard_handler; .type keyboard_handler, @function; .align 0; keyboard_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 movb $0x20, %al; outb %al, $0x20 ;
 call keyboard_routine
 popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 iret


.globl system_call_handler; .type system_call_handler, @function; .align 0; system_call_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 cmpl $0, %EAX
 jl err
 cmpl $MAX_SYSCALL, %EAX
 jg err
 call *sys_call_table(, %EAX, 0x04)
 jmp fin
err: movl $-38, %EAX
fin: movl %EAX, 24(%esp)
 popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 sysexit



.globl clock_handler; .type clock_handler, @function; .align 0; clock_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 movb $0x20, %al; outb %al, $0x20 ;
 call clock_routine
 popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 iret

.globl writeMSR; .type writeMSR, @function; .align 0; writeMSR:
 push %ebp
 movl %esp,%ebp

 movl 12(%ebp), %eax
 movl 8(%ebp), %ecx
 wrmsr

 pop %ebp
 ret

.globl syscall_handler_sysenter; .type syscall_handler_sysenter, @function; .align 0; syscall_handler_sysenter:
 push $0x2B
 push %EBP

 pushfl
 push $0x23
 push 4(%EBP)

 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 cmpl $0, %EAX
 jl sysenter_err
 cmpl $MAX_SYSCALL, %EAX
 jg sysenter_err
 call *sys_call_table(, %EAX, 0x04)
 jmp sysenter_fin
sysenter_err:
 movl $-38, %EAX
sysenter_fin:
 movl %EAX, 0x18(%ESP)
 popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
 movl (%ESP), %EDX

 movl 12(%ESP), %ECX
 sti
 sysexit
