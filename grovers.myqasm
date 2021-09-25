# ---------- Grover's Algorithm in 3 qubits ----------

# Initialize 3 qubit circuit
init 3

# Init
hadamard 0
hadamard 1
hadamard 2

# Oracle
pauliX 0
pauliX 1
toffoli 0 1 2
pauliX 0
pauliX 0

# Grover diffusion
hadamard 0
hadamard 1
hadamard 2
pauliX 0
pauliX 1
controlledZ 0 1
pauliX 0
pauliX 1
hadamard 0
hadamard 1