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
# 8 "entry.S" 2
# 74 "entry.S"
.globl keyboard_handler; .type keyboard_handler, @function; .align 0; keyboard_handler:
    pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
    movb $0x20, %al; outb %al, $0x20;
    call keyboard_routine;
    popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
    iret;

.globl system_call_handler; .type system_call_handler, @function; .align 0; system_call_handler:
    push $0x2B;
    push %ebp
    pushfl
    push $0x23
    push 4(%ebp)
    pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
    cmpl $0, %eax;
    jl err;
    cmpl $MAX_SYSCALL, %eax;
    jg err;
    call *sys_call_table(,%eax, 0x04);
    jmp fin;
err:
    movl $-38, %eax;
fin:
    movl %eax, 0x18(%esp);
    popl %ebx; popl %ecx; popl %edx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
    movl (%esp), %edx;
    movl 12(%esp), %ecx;
    sti;
    sysexit;
    add $1245, %eax
