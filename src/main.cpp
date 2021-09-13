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
	if(argc != 2) {
		cerr << "Usage: ./simulator <number_of_qubits>" << endl << endl;
		exit(-1);
	}

	int qubitCount = stoi(argv[1]);

	int rank, value, size;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// Specify number of qubits, also in QubitLayer.h
	// calculate total size
	vector<int> layerAllocs = calculateLayerAlloc(qubitCount, size);

	QubitLayer qL(layerAllocs[rank]);

	if(rank == 0) {
		// Initialze state vector as |0...0>
		qubitLayer states = qL.getStates();
		states[0] = 1;
		qL.setStates(states);
	}

	qL.pauliX(0);
	qL.measure();

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
	QubitLayer qL(v_size);
	qubitLayer states = qL.getStates();
	states[0] = 1;
	qL.setStates(states);

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