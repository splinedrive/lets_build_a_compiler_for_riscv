.section .text
#fa=1 5 c=Tee
        .text
        .global _start
        _start:
        li t0,1
        la t1,A
        sw t0,0(t1)
        li t0,5
        addi sp,sp,-4
        sw t0,0(sp)
L00:
        la t1,A
        lw t0,0(t1)
        addi t0,t0,1
        sw t0,0(t1)
        lw t1,0(sp)
        bgt t0,t1,L01
        li t0,-1
        la t1,C
        sw t0,0(t1)
        j L00
L01:
        addi sp,sp,4
        ret
.section .data
  A: .word 0
  C: .word 0
