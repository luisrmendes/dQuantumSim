#!/bin/bash

rm quantum.targz >/dev/null 2>&1

tar -czf quantum.targz ../quantum-simulator-sandbox

scp quantum.targz mendes1@juwels-cluster.fz-juelich.de:~

# ssh mendes1@juwels-cluster.fz-juelich.de 'module load Stages/2020 Intel/2020.2.254-GCC-9.3.0 IntelMPI/2019.8.254 >/dev/null 2>&1\
#     && yes | rm -r /p/home/jusers/mendes1/juwels/quantum-simulator-sandbox >/dev/null 2>&1\
#     && tar -xzf /p/home/jusers/mendes1/juwels/quantum.targz >/dev/null 2>&1\
#     && cd quantum-simulator-sandbox >/dev/null 2>&1\
#     && make clean \
#     && make \
#     && mpirun -np 2 ./simulator myqasmTests/testing.myqasm'

ssh mendes1@juwels08.fz-juelich.de 'module load Intel/2021.4.0 IntelMPI/2021.4.0 \
    && yes | rm -r /p/home/jusers/mendes1/juwels/quantum-simulator-sandbox \
    && tar -xzf /p/home/jusers/mendes1/juwels/quantum.targz \
    && cd quantum-simulator-sandbox \
    && make clean \
    && make \
    && (time mpirun -np 1 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 2 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 3 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 4 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 5 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 6 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 7 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 8 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 9 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 10 ./simulator myqasmTests/q30_paulix.myqasm) >> output.txt \
    && (time mpirun -np 1 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 2 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 3 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 4 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 5 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 6 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 7 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 8 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 9 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 10 ./simulator myqasmTests/q31_paulix.myqasm) >> output.txt \
    && (time mpirun -np 1 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 2 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 3 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 4 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 5 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 6 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 7 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 8 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 9 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 10 ./simulator myqasmTests/q32_paulix.myqasm) >> output.txt \
    && (time mpirun -np 1 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 2 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 3 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 4 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 5 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 6 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 7 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 8 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 9 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 10 ./simulator myqasmTests/q33_paulix.myqasm) >> output.txt \
    && (time mpirun -np 1 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt \
    && (time mpirun -np 2 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt \
    && (time mpirun -np 3 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt \
    && (time mpirun -np 4 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt \
    && (time mpirun -np 5 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt \
    && (time mpirun -np 6 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt \
    && (time mpirun -np 7 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt \
    && (time mpirun -np 8 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt \
    && (time mpirun -np 9 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt \
    && (time mpirun -np 10 ./simulator myqasmTests/q34_paulix.myqasm) >> output.txt'
