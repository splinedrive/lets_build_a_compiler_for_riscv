A c/RISCV version of the "Let's Build a Compiler" by Jack Crenshaw 
http://compilers.iecc.com/crenshaw/
A similiar project you can find here but for x86 code https://github.com/lotabout/Let-s-build-a-compiler (c/x86). 

I have done the tutorial and written it in c and will produce RISCV/rv32im assembler code
I also patched the spike simulator with a hack to have an uart ouput.
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
```
I want to share this work! Feel free to experiement and modify, optimizie, extend and more...

Best,

Hirosh




