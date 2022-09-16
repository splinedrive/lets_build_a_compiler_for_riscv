.section .text
        .global _start
        _start:
#pa=a+1e
        .text
        .global _start
        _start:
L00:
        lw t0,A
        addi sp,sp,-4
        sw t0,0(sp)
        li t0,1
        lw t1,0(sp)
        addi sp,sp,4
        add t0,t1,t0
        la t1,A
        sw t0,0(t1)
        j L00
        ret
.section .data
  A: .word 0
