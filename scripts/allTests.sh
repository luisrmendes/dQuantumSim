#!/bin/bash

# Receber 

cd ~/work/bolsa-prace/quantum-simulator-sandbox
make clean && make

# 3 qubit tests
python3 scripts/test.py myqasmTests/q3_grovers.myqasm 3 "50 50 0"
python3 scripts/test.py myqasmTests/q3_test1.myqasm 3 "100 50 100"

# 4 qubit tests
python3 scripts/test.py myqasmTests/q4_test1.myqasm 4 "0 100 50 100"
python3 scripts/test.py myqasmTests/q4_test2.myqasm 4 "50 100 0 0"
python3 scripts/test.py myqasmTests/q4_test3.myqasm 4 "50 50 100 0"

# 5 qubit tests
python3 scripts/test.py myqasmTests/q5_test1.myqasm 4 "100 0 0 0 0"

# 26 qubit tests
python3 scripts/test.py myqasmTests/q26_test1.myqasm 26 "100 100 100 100 100 100 \
100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100"
python3 scripts/test.py myqasmTests/q26_test2.myqasm 26 "100 100 100 100 100 100 \
100 100 100 50 100 100 100 100 100 100 100 50 100 100 100 100 100 100 100 50"

exit 0