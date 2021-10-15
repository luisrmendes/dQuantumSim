#!/bin/bash

rm quantum.targz >/dev/null 2>&1

tar -czf quantum.targz ../quantum-simulator-sandbox

scp quantum.targz mendes1@juwels-cluster.fz-juelich.de:~

ssh mendes1@juwels-cluster.fz-juelich.de 'module load Intel/2020.2.254-GCC-9.3.0 IntelMPI/2019.8.254 >/dev/null 2>&1\
    && yes | rm -r /p/home/jusers/mendes1/juwels/quantum-simulator-sandbox >/dev/null 2>&1\
    && tar -xzf /p/home/jusers/mendes1/juwels/quantum.targz >/dev/null 2>&1\
    && cd quantum-simulator-sandbox >/dev/null 2>&1\
    && make clean \
    && make \
    && mpirun -np 30 ./simulator myqasmTests/q32_test1.myqasm'

exit 0