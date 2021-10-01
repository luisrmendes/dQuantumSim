#!/bin/bash

cd ~/work/bolsa-prace/quantum-simulator-sandbox
make clean && make

# Por cada iteracao, correr o codigo incrementando o numero de processos
i=1
while [ $i -le $1 ]
do
    mpirun --mca opal_warn_on_missing_libcuda 0 -np $i ./simulator $2 | awk '{print $4}'
    ((i++))
done

exit 0
