#include "QubitLayer.h"
#include "StateAccess.h"
#include "matrix.h"
#include <iostream>

using namespace std;

#ifdef USING_MPI
#include "mpi.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	int rank, value, size;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	do {
		if(rank == 0) {
			scanf("%d", &value);
			MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
		} else {
			MPI_Recv(&value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
			if(rank < size - 1)
				MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
		}
		printf("Process %d got %d\n", rank, value);
	} while(value >= 0);

	MPI_Finalize();

	return 0;
}
#else
int main(int argc, char* argv[])
{
	printf("\n");

	// Specify number of qubits, also in QubitLayer.h

	QubitLayer qL(3);

	// init
	qL.hadamard(0);
	qL.hadamard(1);
	qL.hadamard(2);

	// oracle
	qL.pauliX(0);
	qL.pauliX(1);
	qL.toffoli(0, 1, 2);
	qL.pauliX(0);
	qL.pauliX(1);

	// grover diffusion
	qL.hadamard(0);
	qL.hadamard(1);
	qL.hadamard(2);
	qL.pauliX(0);
	qL.pauliX(1);
	qL.controlledZ(0, 1);
	qL.pauliX(0);
	qL.pauliX(1);
	qL.hadamard(0);
	qL.hadamard(1);

	qL.measure();

	/**
	 * PHASE ORACLE
	 */
	// QubitLayer qL(2);

	// // init
	// qL.hadamard(0);
	// qL.hadamard(1);

	// // oracle
	// qL.pauliX(0);
	// qL.pauliX(1);
	// qL.controlledZ(0, 1);
	// qL.pauliX(0);
	// qL.pauliX(1);

	// // grover diffusion
	// qL.hadamard(0);
	// qL.hadamard(1);
	// qL.pauliX(0);
	// qL.pauliX(1);
	// qL.controlledZ(0, 1);
	// qL.pauliX(0);
	// qL.pauliX(1);
	// qL.hadamard(0);
	// qL.hadamard(1);

	// qL.measure();

	return 0;
}
#endif