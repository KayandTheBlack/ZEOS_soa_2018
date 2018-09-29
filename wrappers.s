# 1 "wrappers.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "wrappers.S"





# 1 "include/asm.h" 1
# 7 "wrappers.S" 2
# 1 "include/segment.h" 1
# 8 "wrappers.S" 2


.globl writeMSR; .type writeMSR, @function; .align 0; writeMSR:
    push %ebp
    movl %esp, %ebp
    movl 8(%ebp), %ecx
    movl 12(%ebp), %eax
    wrmsr
    popl %ebp
    ret


.globl write; .type write, @function; .align 0; write:
    push %ebp
    movl %esp, %ebp


    movl 8(%ebp), %ebx
    movl 12(%ebp), %ecx
    movl 16(%ebp), %edx


    movl $4, %eax


    push %ecx
    push %edx


    push $POSTSYSWRITE

    push %ebp
    movl %esp, %ebp


    sysenter

POSTSYSWRITE:

    pop %ebp
    add $4, %esp
    pop %edx
    pop %ecx


    movl $1, %eax



    pop %ebp


    ret
