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




.globl write; .type write, @function; .align 0; write:
    push %ebp
    movl %esp, %ebp




    push %ebx


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


    cmpl $0, %eax
    jge SYSWRITENOERR
    negl %eax
    movl %eax, errno
    movl $-1, %eax

SYSWRITENOERR:




    pop %ebx

    pop %ebp


    ret



.globl gettime; .type gettime, @function; .align 0; gettime:
    push %ebp
    movl %esp, %ebp


    movl $10, %eax


    push %ecx
    push %edx


    push $POSTSYSGETTIME

    push %ebp
    movl %esp, %ebp


    sysenter

POSTSYSGETTIME:

    pop %ebp
    add $4, %esp
    pop %edx
    pop %ecx
# 107 "wrappers.S"
    pop %ebp


    ret


.globl getpid; .type getpid, @function; .align 0; getpid:
    push %ebp
    movl %esp, %ebp
    movl $20, %eax
    push %ecx
    push %edx
    push $POSTSYSGETPID
    push %ebp
    movl %esp, %ebp
    sysenter
POSTSYSGETPID:
    pop %ebp
    add $4, %esp
    pop %edx
    pop %ecx
    pop %ebp
    ret


.globl fork; .type fork, @function; .align 0; fork:
    push %ebp
    movl %esp, %ebp
    movl $2, %eax
    push %ecx
    push %edx
    push $POSTSYSFORK
    push %ebp
    movl %esp, %ebp
    sysenter
POSTSYSFORK:
    pop %ebp
    add $4, %esp
    pop %edx
    pop %ecx
    cmpl $0, %eax
    jge SYSFORKNOERR
    negl %eax
    movl %eax, errno
    movl $-1, %eax

SYSFORKNOERR:
    pop %ebp
    ret
