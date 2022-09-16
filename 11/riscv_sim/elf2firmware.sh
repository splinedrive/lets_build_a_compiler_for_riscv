riscv32-unknown-elf-objcopy -O binary $1 firmware.bin
riscv32-unknown-elf-objdump -d -M no-aliases $1
./makehex.py firmware.bin $((8192)) > firmware.hex


