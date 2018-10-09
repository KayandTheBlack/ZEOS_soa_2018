# 1 "schedAss.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "schedAss.S"

# 1 "include/asm.h" 1
# 3 "schedAss.S" 2
# 1 "include/segment.h" 1
# 4 "schedAss.S" 2


.globl task_switch; .type task_switch, @function; .align 0; task_switch:
    pushl %ebp
    movl %esp, %ebp
    pushl %edi
    pushl %esi
    pushl %ebx
    pushl 8(%ebp)
    call inner_task_switch
    addl $4, %esp
    popl %ebx
    popl %esi
    popl %edi
    popl %ebp
    ret

.globl getebp; .type getebp, @function; .align 0; getebp:
    movl %ebp, %eax
    ret

.globl setesp; .type setesp, @function; .align 0; setesp:
    movl 4(%esp), %esp
    pop %ebp
    ret
