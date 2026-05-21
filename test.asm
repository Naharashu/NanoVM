#include header.asm
#include header.asm ; not includes twice
_start:
ld R0 5 ; load 5 into r0
ld R1 3 ; load 3 into r1
add R0 R1 ; r0 = r0 + r1
ld r3 1
call add_r3_r3
; _start:
hlt ; end program
