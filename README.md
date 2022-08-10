# Distributed Quantum Simulator developed at FEUP

This project implements a parallel and scalable quantum simulator based on the state vector approach. By distributing the workload amongst various machines, we extend the capabilities of quantum simulation. 


## Features

The program receives a quantum assembly file as input. This simulator parses a subset of the OpenQASM 2.0 specification.
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
$ git@git.fe.up.pt:quantum-simulator/simulator.git

# Go into the repository
$ cd simulator

# Compile
$ make

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
