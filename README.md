[![Build with Make](https://github.com/luisrmendes/dQuantumSim/actions/workflows/build.yml/badge.svg)](https://github.com/luisrmendes/dQuantumSim/actions/workflows/build.yml)
[![Testing](https://github.com/luisrmendes/dQuantumSim/actions/workflows/testing.yml/badge.svg)](https://github.com/luisrmendes/dQuantumSim/actions/workflows/testing.yml)

# Distributed Quantum Simulator developed at FEUP

This project implements a parallel and scalable quantum simulator based on the state vector approach. We extend the capabilities of quantum simulation by distributing the workload amongst various computer nodes and take advantage of combined shared memory.

This project is implemented in C++ and strictly conforms with the ISO/IEC 14882:2017 specification for C++17. MPI is used for inter-process communication amongst computer nodes.


## Features

This simulator supports a subset of the OpenQASM 2.0 quantum assembly specification.  
The following gate operations are implemented:

- Pauli X gate
- Pauli Y gate
- Pauli Z gate
- Hadamard gate
- Principle Square Root of X gate
- Principle Square Root of Y gate
- Principle Square Root of Z gate (S gate)
- T gate
- Controlled X gate
- Controlled Z gate
- Toffoli gate

## How to Run

Git, gcc, make and openmpi are required

```sh
# Clone this repository
$ git clone git@github.com:luisrmendes/dQuantumSim.git

# Go into the repository
$ cd dQuantumSim

# Compile
$ make -j5

# Run
$ mpirun --mca opal_warn_on_missing_libcuda 0 -np <number_of_processes> ./dqsim <openqasm_file>
```

## Example with Grover's Algorithm

```sh
mpirun --mca opal_warn_on_missing_libcuda 0 -np 2 ./dqsim openqasmTests/q3_grovers.openqasm
```

We provide an example of an implementation of Grover's algorithm using three qubits in the "openqasmTests" folder (q3_grovers.openqasm), alongside many other test examples.  


```
# ---------- Grover's Algorithm in 3 qubits ----------

# Initialize 3 qubit circuit
qreg q[3];

h q[0];
h q[1];
h q[1];
cx q[0],q[1];
h q[1];
h q[0];
x q[0];
h q[1];
x q[1];
h q[1];
cx q[0],q[1];
h q[1];
x q[0];
h q[0];
x q[1];
h q[1];
``` 
