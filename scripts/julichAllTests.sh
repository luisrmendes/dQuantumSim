#!/bin/bash

# 3 qubit tests
python3 scripts/julichTest.py myqasmTests/q3_grovers.myqasm 3 "50 50 0"
python3 scripts/julichTest.py myqasmTests/q3_test1.myqasm 3 "100 50 100"

# 4 qubit tests
python3 scripts/julichTest.py myqasmTests/q4_test1.myqasm 4 "0 100 50 100"
python3 scripts/julichTest.py myqasmTests/q4_test2.myqasm 4 "50 100 0 0"
python3 scripts/julichTest.py myqasmTests/q4_test3.myqasm 4 "50 50 100 0"

# 5 qubit tests
python3 scripts/julichTest.py myqasmTests/q5_test1.myqasm 4 "100 0 0 0 0"

# 11 qubit tests grover
python3 scripts/julichTest.py myqasmTests/q11_grovers.myqasm 6 "99973 99973 99973 99973 99973 99973 99973 99973 99973 99973 0"

# 17 qubit tests
python3 scripts/julichTest.py myqasmTests/q17_hadamard.myqasm 20 "50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50"

# 20 qubit tests
python3 scripts/julichTest.py myqasmTests/q20_hadamard.myqasm 20 "50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50"

# 23 qubit tests
python3 scripts/julichTest.py myqasmTests/q23_paulix.myqasm 20 "100 100 100 100 100 100 \
100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100"
python3 scripts/julichTest.py myqasmTests/q23_hadamard.myqasm 20 "50 50 50 50 50 50 \
50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50"

# 26 qubit tests
python3 scripts/julichTest.py myqasmTests/q26_paulix.myqasm 20 "100 100 100 100 100 100 \
100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100"
python3 scripts/julichTest.py myqasmTests/q26_test2.myqasm 20 "100 100 100 100 100 100 \
100 100 100 50 100 100 100 100 100 100 100 50 100 100 100 100 100 100 100 50"
python3 scripts/julichTest.py myqasmTests/q26_hadamard.myqasm 20 "50 50 50 50 50 50 \
50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50 50"

# 16 qubit tests grover
# python3 scripts/julichTest.py myqasmTests/q16_grovers.myqasm 20 "999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 999993 0"

# # 17 qubit tests grover
# python3 scripts/julichTest.py myqasmTests/q17_grovers.myqasm 20  "999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 999994 0"

exit 0