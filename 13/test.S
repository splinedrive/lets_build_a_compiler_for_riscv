	.global _start
	.section .data
I:	 .word 0
J:	 .word 0
SUM:	 .word 0
	.section .text
_start:
L00:
	lw t0,I
	addi sp,sp,-4
	sw t0,0(sp)
	li t0,1000
	addi sp,sp,4
	lw t1,-4(sp)
	sub t0,t1,t0
	sltz t0,t0
	beq t0,x0,L01
	lw t0,SUM
	addi sp,sp,-4
	sw t0,0(sp)
	lw t0,I
	addi sp,sp,4
	lw t1,-4(sp)
	add t0,t1,t0
	la t1,I
	sw t0,0(t1)
	lw t0,I
	addi sp,sp,-4
	sw t0,0(sp)
	li t0,1
	addi sp,sp,4
	lw t1,-4(sp)
	add t0,t1,t0
	la t1,I
	sw t0,0(t1)
	lw t0,I
	addi sp,sp,-4
	sw t0,0(sp)
	li t0,5
	addi sp,sp,4
	lw t1,-4(sp)
	div t0,t1,t0
	not t0,t0
	beq t0,x0,L02
	lw t0,J
	addi sp,sp,-4
	sw t0,0(sp)
	li t0,1
	addi sp,sp,4
	lw t1,-4(sp)
	add t0,t1,t0
	la t1,J
	sw t0,0(t1)
L02:
	j L00
L01:
	ret
