	A:	 .word 0
	B:	 .word 0
	C:	 .word 0
X:
	addi sp,sp,0
	
# LoadParam
	lw t0,0(sp)
	
# StoreParam
	sw t0,-8(sp)
	
# LoadParam
	lw t0,-8(sp)
	
# StoreParam
	sw t0,-4(sp)
	
# LoadParam
	lw t0,-4(sp)
	
# StoreParam
	sw t0,0(sp)
	addi sp,sp,0
	
# Return
	ret
	.section .text
_start:
	
# CallProc
	
# Param
	la t0,A
	addi sp,sp,-4
	sw t0,0(sp)
	
# Param
	la t0,B
	addi sp,sp,-4
	sw t0,0(sp)
	
# Param
	la t0,C
	addi sp,sp,-4
	sw t0,0(sp)
	call X
	addi sp,sp,12
	ret
