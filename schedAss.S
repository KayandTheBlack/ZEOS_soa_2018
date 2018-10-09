
#include <asm.h>
#include <segment.h>


ENTRY(task_switch)
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

ENTRY(getebp)
    movl %ebp, %eax
    ret

ENTRY(setesp)
    movl 4(%esp), %esp
    pop %ebp
    ret

