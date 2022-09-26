A c/RISCV version of the "Let's Build a Compiler" by Jack Crenshaw 
http://compilers.iecc.com/crenshaw/
A similiar project you can find here but for x86 code https://github.com/lotabout/Let-s-build-a-compiler (c/x86). 

I have done the tutorial and written it in c and will produce RISCV/rv32im assembler code
I also patched the spike simulator with a hack to have an uart output.
Just apply the patch spike_uart.patch for riscv-isa-sim repository. But I put spike into ./bin/spike and the patched source of spike
is in riscv-isa-sim.

just go to chapter 12 and try:
```bash
./make_spike_firmware.sh ascii_loop.ty 
```
to execute a ascii print loop of these tiny program:
```
{ascii loop}
program;
var i = 33;
begin
  while 1
      Write (i);
      if i = 128
        i = 33;
        else
        i = i + 1;
      endif;
  endwhile;
end;

hd@host:~/src/lets_build_a_compiller_for_riscv/12 $ cat ascii_loop.ty | ./main 
        .global _start
        .section .data
        I:       .word 33
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

```
I want to share this work! Feel free to experiement and modify, optimizie, extend and more...

Best,

Hirosh




