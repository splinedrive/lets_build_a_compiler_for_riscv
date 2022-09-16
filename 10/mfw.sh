#!/bin/bash -x
cat test.ty | ./main  > a.s
pushd riscv_sim
cp ../a.s .
make -f Makefile.kian
./elf2firmware.sh ./a.out
cp firmware.hex ~/asic/kianRISCV/kianv_harris_mcycle_edition/gateware/firmware 
popd
