RV_PREFIX = riscv32-unknown-elf-#Prefix for our RISC-V tools (e.g. gcc and objdump)
CC = $(RV_PREFIX)gcc #Our compiler
CCFLAGS = -nostdlib -mcmodel=medany -Tlink.ld #Flags telling GCC to compile without libraries (e.g. baremetal), to use a differen memory model so that we can put our code at 0x80000000 and to use the specified linker script.
OUT_FILE = a.out

all: init exec

#Compile init.s
init:
	$(CC) -c init.s -o init.o

#Compile hello.c and link hello.o and init.o
exec:
	#$(CC) $(CCFLAGS) -c hello.c -o hello.o
	$(CC) $(CCFLAGS) -c a.s -o a.o
	$(CC) $(CCFLAGS) -o $(OUT_FILE) a.o init.o

#Remove .out and .o files
clean:
	rm *.out *.o

#For debugging purposes and objdump of our output file.
dump:
	$(RV_PREFIX)objdump -D $(OUT_FILE)
dumpnoalias:
	$(RV_PREFIX)objdump -D -M no-aliases $(OUT_FILE)
