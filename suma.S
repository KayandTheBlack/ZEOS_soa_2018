#include <asm.h>
ENTRY(add2)
push %ebp
mov %esp,%ebp
mov 0x08(%ebp), %eax
add 0x0c(%ebp), %eax
pop %ebp
ret
