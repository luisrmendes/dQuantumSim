#!/bin/bash

make clean
make
mpirun --mca opal_warn_on_missing_libcuda 0 -np $1 ~/work/bolsa-prace/quantum-simulator-sandbox/simulator $2
