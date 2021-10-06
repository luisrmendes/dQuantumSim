#!/bin/bash

cd ~/work/bolsa-prace/quantum-simulator-sandbox
make clean && make

# Por cada iteracao, correr o codigo incrementando o numero de processos
python scripts/test.py myqasmTests/q26_test1.myqasm 26 "100 100 100 100 100 100 \
100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100"

python scripts/test.py myqasmTests/q26_test2.myqasm 26 "100 100 100 100 100 100 \
100 100 100 50 100 100 100 100 100 100 100 50 100 100 100 100 100 100 100 50"

exit 0
