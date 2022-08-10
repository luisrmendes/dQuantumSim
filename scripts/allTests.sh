#!/bin/bash

cd ~/work/bolsa-prace/quantum-simulator-sandbox
make clean && make -j 8

# 3 qubit tests
python3 scripts/test.py myqasmTests/q3_grovers.openqasm 3 "100 100 0"
python3 scripts/test.py myqasmTests/q3_test1.openqasm 3 "100 50 100"

# 4 qubit tests
python3 scripts/test.py myqasmTests/q4_test1.openqasm 4 "0 100 50 100"
python3 scripts/test.py myqasmTests/q4_test2.openqasm 4 "50 100 0 0"
python3 scripts/test.py myqasmTests/q4_test3.openqasm 4 "50 50 100 0"

# 5 qubit tests
python3 scripts/test.py myqasmTests/q5_test1.openqasm 4 "100 0 0 0 0"

# 11 qubit tests grover
python3 scripts/test.py myqasmTests/q11_grovers.openqasm 6 "99973 99973 99973 99973 99973 99973 99973 99973 99973 99973 0"

# 17 qubit tests
python3 scripts/test.py myqasmTests/q17_hadamard.openqasm 6 "50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50"

# 20 qubit tests
python3 scripts/test.py myqasmTests/q20_hadamard.openqasm 6 "50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50"

# 23 qubit tests
python3 scripts/test.py myqasmTests/q23_paulix.openqasm 6 "100 100 100 100 100 100 \
100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100"
python3 scripts/test.py myqasmTests/q23_hadamard.openqasm 6 "50 50 50 50 50 50 \
50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50"

# 26 qubit tests
python3 scripts/test.py myqasmTests/q26_paulix.openqasm 6 "100 100 100 100 100 100 \
100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100"
python3 scripts/test.py myqasmTests/q26_test2.openqasm 6 "100 100 100 100 100 100 \
100 100 100 50 100 100 100 100 100 100 100 50 100 100 100 100 100 100 100 50"
python3 scripts/test.py myqasmTests/q26_hadamard.openqasm 6 "50 50 50 50 50 50 \
50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50"

# 16 qubit tests grover
python3 scripts/test.py myqasmTests/q16_grovers.openqasm 6 "999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 0"

# 17 qubit tests grover
python3 scripts/test.py myqasmTests/q17_grovers.openqasm 6  "999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 0"


exit 0