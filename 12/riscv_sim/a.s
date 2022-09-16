	.global _start
	.section .data
	I:	 .word 33
	.section .text
_start:
L00:
	
# LoadConst
	li t0,1
	
# BranchFalse
	beq t0,x0,L01
	
# LoadVar
	lw t0,I
	
# WriteIt
	la t1,0x30000000
	sw t0,0(t1)
	
# LoadVar
	lw t0,I
	
# Push
	addi sp,sp,-4
	sw t0,0(sp)
	
# Equals
	
# CompareExpression
	
# LoadConst
	li t0,128
	
# PopCompare
	addi sp,sp,4
	lw t1,-4(sp)
	sub t0,t1,t0
	
# SetEqual
	seqz t0,t0
	
# BranchFalse
	beq t0,x0,L02
	
# LoadConst
	li t0,33
	
# StoreVar
	la t1,I
	sw t0,0(t1)
	
# Branch
	j L03
L02:
	
# LoadVar
	lw t0,I
	
# Push
	addi sp,sp,-4
	sw t0,0(sp)
	
# LoadConst
	li t0,1
	
# PopAdd
	addi sp,sp,4
	lw t1,-4(sp)
	add t0,t1,t0
	
# StoreVar
	la t1,I
	sw t0,0(t1)
L03:
	
# Branch
	j L00
L01:
	ret
