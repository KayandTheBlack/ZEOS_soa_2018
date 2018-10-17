/**
Wrappers for sys_calls

**/

#include <asm.h>
#include <segment.h>
//#include <libc.h>


    
ENTRY(write)
    push %ebp
    movl %esp, %ebp
    
    //Parameter passing

//WARNING, STORE EBX AND THINGS!!
    push %ebx
    //push %ecx
    //push %edx
    movl 8(%ebp), %ebx
    movl 12(%ebp), %ecx
    movl 16(%ebp), %edx
    
    // put identifier in eax
    movl $4, %eax
    
    //store ecx edx in user stack (WHY? it's parameters now?)
    push %ecx
    push %edx
    
    //store return @ of user code in stack (next instruction to sysenter)
    push $POSTSYSWRITE //use lea better.
    //create fake dynamic link
    push %ebp
    movl %esp, %ebp
    
    //generate trap
    sysenter
    
POSTSYSWRITE:
    //pop ebp, ret address, edx ecx  
    pop %ebp
    add $4, %esp
    pop %edx
    pop %ecx
    
    //process result (if neg, put in perror and return -1, do it another time)
    cmpl $0, %eax
    jge SYSWRITENOERR
    negl %eax
    movl %eax, errno
    movl $-1, %eax
    
SYSWRITENOERR:
    
    //undo dyna link
    //pop %edx//warning
    //pop %ecx
    pop %ebx
    
    pop %ebp
    
    //return
    ret



ENTRY(gettime)
    push %ebp
    movl %esp, %ebp
    //no params
    // put identifier in eax
    movl $10, %eax
    
    //store ecx edx in user stack
    push %ecx
    push %edx
    
    //store return @ of user code in stack (next instruction to sysenter)
    push $POSTSYSGETTIME
    //create fake dynamic link
    push %ebp
    movl %esp, %ebp
    
    //generate trap
    sysenter
    
POSTSYSGETTIME:
    //pop ebp, ret address, edx ecx
    pop %ebp
    add $4, %esp
    pop %edx
    pop %ecx
    
    //process result (if neg, put in perror and return -1, do it another time)
    /*cmpl $0, %eax
    jge SYSGETTIMENOERR
    subl $0, %eax
    //movl %eax, errno //PENDING, AS ERRNO IS NOT IN THIS COMPILATION PACK
    movl $-1, %eax
    
SYSGETTIMENOERR:*/
    
    //undo dyna link
    pop %ebp
    
    //return
    ret


ENTRY(getpid)
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


ENTRY(fork)
    push %ebp
    movl %esp, %ebp
    push %ecx
    push %edx
    //push $POSTSYSFORK
    lea POSTSYSFORK, %eax
    pushl %eax
    movl $2, %eax
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


ENTRY(exit)
    push %ebp
    movl %esp, %ebp
    movl $1, %eax
    push %ecx
    push %edx
    push $POSTSYSEXIT
    push %ebp
    movl %esp, %ebp
    sysenter
POSTSYSEXIT: //not actually necessary
    pop %ebp
    add $4, %esp
    pop %edx
    pop %ecx
    pop %ebp
    ret

