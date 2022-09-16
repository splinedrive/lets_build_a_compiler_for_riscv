#!/bin/bash -x
cat $1 | ./main  > a.s
pushd riscv_sim
cp ../a.s .
make -f Makefile
./run_sim.sh
popd
