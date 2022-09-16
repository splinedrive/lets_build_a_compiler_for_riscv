.section .text
        .global _start
        _start:
             li t0,5
        addi sp,sp,-4
        sw t0,0(sp)
        li t0,6
        addi sp,sp,-4
        sw t0,0(sp)
        li t0,3
        addi sp,sp,-4
        sw t0,0(sp)
        li t0,7
        addi sp,sp,-4
        sw t0,0(sp)
        li t0,10
        lw t1,0(sp)
        addi sp,sp,4
        add t0,t1,t0
        lw t1,0(sp)
        addi sp,sp,4
        mul t0,t1,t0
        lw t1,0(sp)
        addi sp,sp,4
        add t0,t1,t0
        lw t1,0(sp)
        addi sp,sp,4
        mul t0,t1,t0
        la t1,A
        sw t0,0(t1)
        ret
.section .data
  A: .word 0
