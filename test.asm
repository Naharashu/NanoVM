#include header.asm
jmp _start
; _start:
ld R0 5 ; load 5 into r0
; _start:
ld R1 3 ; load 3 into r1
_start:
add R0 R1 ; r0 = r0 + r1
hlt ; end program
