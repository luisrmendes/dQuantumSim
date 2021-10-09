#!/bin/bash

rm quantum.targz

tar -czvf quantum.targz ../quantum-simulator-sandbox

scp quantum.targz mendes1@juwels-cluster.fz-juelich.de:~

ssh mendes1@juwels-cluster.fz-juelich.de 'module load Intel/2020.2.254-GCC-9.3.0 IntelMPI/2019.8.254 \
    && yes | rm -r /p/home/jusers/mendes1/juwels/quantum-simulator-sandbox \
    && tar -xzf /p/home/jusers/mendes1/juwels/quantum.targz \
    && cd quantum-simulator-sandbox \
    && make clean \
    && make \
    && mpirun -np 50 ./simulator myqasmTests/q31_test1.myqasm'

exit 0