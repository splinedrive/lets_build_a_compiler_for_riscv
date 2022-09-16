	.global _start
	.section .data
	I:	 .word 33
	.section .text
_start:
L00:
	li t0,1
	addi sp,sp,-4
	sw t0,0(sp)
	li t0,2
	addi sp,sp,4
	lw t1,-4(sp)
	sub t0,t1,t0
	sltz t0,t0
	beq t0,x0,L01
	lw t0,I
	la t1,0x30000000
	sw t0,0(t1)
	lw t0,I
	addi sp,sp,-4
	sw t0,0(sp)
	li t0,128
	addi sp,sp,4
	lw t1,-4(sp)
	sub t0,t1,t0
	seqz t0,t0
	beq t0,x0,L02
	li t0,33
	la t1,I
	sw t0,0(t1)
	j L03
L02:
	lw t0,I
	addi sp,sp,-4
	sw t0,0(sp)
	li t0,1
	addi sp,sp,4
	lw t1,-4(sp)
	add t0,t1,t0
	la t1,I
	sw t0,0(t1)
L03:
	j L00
L01:
	ret
