#include "QubitLayer.h"
#include "StateAccess.h"
#include "utils.h"
#include <iostream>

using namespace std;

#define USING_MPI

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

	// Specify number of qubits, also in QubitLayer.h
	// calculate total size
	int qubitCount = 3;
	vector<int> layerAllocs = calculateLayerAlloc(10, 5);

	for(size_t i = 0; i < layerAllocs.size(); ++i) {
		cout << layerAllocs[i] << endl;
	} 

	// if (rank == size-1) {
	// 	QubitLayer qL(layerAllocs[rank]);
	// 	// qL.printStateVector();
	// }
	// else {
	// 	QubitLayer qL(layerAllocs[rank]);
	// 	// qL.printStateVector();
	// }

	// do {
	// 	if(rank == 0) {
	// 		size_t qLayerSegSize = layerSize / size;
	// 		MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
	// 	} else {
	// 		MPI_Recv(&value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
	// 		if(rank < size - 1)
	// 			MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
	// 	}
	// 	printf("Process %d got %d\n", rank, value);
	// } while(value >= 0);

	MPI_Finalize();

	return 0;
}
#else
int main(int argc, char* argv[])
{
	printf("\n");

	// Specify number of qubits, also in QubitLayer.h
	// calculate total size
	int qubitCount = 3;
	size_t v_size = 2 * pow(2, qubitCount);
	QubitLayer qL(0, v_size);

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