#include "QubitLayer.h"
#include "utils.h"
#include <iostream>

using namespace std;

#define USING_MPI

#ifdef USING_MPI
#include "QubitLayerMPI.h"
#include "debug.h"
#include "macros.h"
#include "mpi.h"
#include "parser.h"
#include "utilsMPI.h"
#include <filesystem>
#include <stdio.h>

int main(int argc, char* argv[])
{
	if(argc != 2) {
		cerr << "Usage: ./simulator <file_name>" << endl << endl;
		exit(-1);
	}

	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

#ifdef OUTPUT_LOGS
	if(rank == 0) {
		filesystem::remove_all("logs");
		filesystem::create_directory("logs");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	appendDebugLog(
		rank, size, "\n--------------- Node ", rank, " logs --------------- \n\n");
#endif

	// Handle instructions
	vector<unsigned int> instructions;

	if(rank == 0)
		instructions = sourceParser(argv[1]);

	instructionsHandlerMPI(instructions, rank, size);
	MPI_Barrier(MPI_COMM_WORLD);

	vector<unsigned int> layerAllocs = calculateLayerAlloc(instructions[0], size);
	QubitLayerMPI qL(layerAllocs, rank, size);

	// Initialze state vector as |0...0>
	if(rank == 0) {
		/** TODO: ha maneiras melhores de fazer isto **/
		qubitLayer states = qL.getStates();
		states[0] = 1;
		qL.setStates(states);
	}

#ifdef GET_STATE_LAYER_INFO_DEBUG_LOGS
	appendDebugLog(
		rank, size, "Size of States: ", qL.getLayerAllocs()[rank] / 2, "\n");

	int localStartIndex = qL.getLocalStartIndex();
	size_t j = 0;

	while(j < qL.getStates().size()) {
		appendDebugLog(
			rank, size, "|", bitset<numQubitsMPI>(localStartIndex / 2), "> ");

		localStartIndex += 2;
		j += 2;
	}
	appendDebugLog(rank, size, "\n");
#endif

	for(size_t i = 1; i < instructions.size(); i++) {
#ifdef MEASURE_DEBUG_LOGS
		qL.measure();
#endif
		switch(instructions[i]) {
		case 1:
			qL.pauliX(instructions[i + 1]);
			i += 1;
			MPI_Barrier(MPI_COMM_WORLD);
			break;
		case 2:
			qL.pauliY(instructions[i + 1]);
			i += 1;
			MPI_Barrier(MPI_COMM_WORLD);
			break;
		case 3:
			qL.pauliZ(instructions[i + 1]);
			i += 1;
			MPI_Barrier(MPI_COMM_WORLD);
			break;
		case 4:
			qL.hadamard(instructions[i + 1]);
			i += 1;
			MPI_Barrier(MPI_COMM_WORLD);
			break;
		case 5:
			qL.controlledX(instructions[i + 1], instructions[i + 2]);
			i += 2;
			MPI_Barrier(MPI_COMM_WORLD);
			break;
		case 6:
			qL.controlledZ(instructions[i + 1], instructions[i + 2]);
			i += 2;
			MPI_Barrier(MPI_COMM_WORLD);
			break;
		case 7:
			qL.toffoli(
				instructions[i + 1], instructions[i + 2], instructions[i + 3]);
			i += 3;
			MPI_Barrier(MPI_COMM_WORLD);
			break;
		default:
			cerr << "Unrecognized operation " << instructions[i] << endl;
			exit(EXIT_FAILURE);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	double results[numQubitsMPI * 2];
	qL.measureQubits(results);
	gatherResults(rank, size, results);

	// print results
	if(rank == 0) {
		cout << "\nResults: \n";
		for(size_t i = 0; i < numQubitsMPI * 2; i += 2) {
			cout << "Qubit " << (i / 2) + 1 << " -> " << results[i + 1] * 100
				 << "% chance of being ON\n";
		}
		cout << "\n";
	}
	
#ifdef MEASURE_STATE_VALUES_DEBUG_LOGS
	qL.measure();
#endif

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

	qL.hadamard(1);
	qL.hadamard(0);
	qL.hadamard(2);

	qL.pauliY(1);
	qL.pauliX(2);

	// init
	// qL.hadamard(0);
	// qL.hadamard(1);
	// qL.hadamard(2);

	// // oracle
	// qL.pauliX(0);
	// qL.pauliX(1);
	// qL.toffoli(0, 1, 2);
	// qL.pauliX(0);
	// qL.pauliX(1);

	// // grover diffusion
	// qL.hadamard(0);
	// qL.hadamard(1);
	// qL.hadamard(2);
	// qL.pauliX(0);
	// qL.pauliX(1);
	// qL.controlledZ(0, 1);
	// qL.pauliX(0);
	// qL.pauliX(1);
	// qL.hadamard(0);
	// qL.hadamard(1);

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