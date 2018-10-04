# 1 "wrappers.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "wrappers.S"
# 1 "include/asm.h" 1
# 2 "wrappers.S" 2
.globl write; .type write, @function; .align 0; write:
 push %ebp
 mov %esp, %ebp

 push %ecx
 push %edx
 push %ebx
 push %esi
 push %edi

 mov 16(%ebp), %edx #size
 mov 12(%ebp), %ecx #buffer
 mov 8(%ebp), %ebx #fd

 lea ret, %eax
 push %eax
 push %ebp
 movl %esp, %ebp
 mov $4, %eax
 sysenter
ret: pop %ebp
 add $4, %esp

 cmp $0, %eax
 je end_err

 lea errno, %ebx
 mov %eax, (%ebx)
 mov $-1, %eax
 jmp end

end_err:mov 16(%ebp), %eax
end:
 pop %edi
 pop %esi
 pop %ebx
 pop %edx
 pop %ecx

 mov %ebp, %esp
 pop %ebp
 ret


.globl gettime; .type gettime, @function; .align 0; gettime:
 push %ebp
 mov %esp, %ebp

 push %ecx
 push %edx
 push %ebx
 push %esi
 push %edi

 lea ret2, %eax
 push %eax
 push %ebp
 movl %esp, %ebp
 mov $4, %eax
 sysenter
ret2: pop %ebp
 add $4, %esp


 pop %edi
 pop %esi
 pop %ebx
 pop %edx
 pop %ecx

    mov %ebp, %esp
 pop %ebp
 ret
