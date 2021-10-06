#!/bin/bash

cd ~/work/bolsa-prace/quantum-simulator-sandbox
make clean && make

# Por cada iteracao, correr o codigo incrementando o numero de processos
python scripts/test.py myqasmTests/q4_test1.myqasm 4 "0 100 50 100"
python scripts/test.py myqasmTests/q4_test2.myqasm 4 "50 100 0 0"
python scripts/test.py myqasmTests/q4_test3.myqasm 4 "50 50 100 0"

exit 0