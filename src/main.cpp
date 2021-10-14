#include "QubitLayerMPI.h"
#include "debug.h"
#include "macros.h"
#include "mpi.h"
#include "parser.h"
#include "utils.h"
#include "utilsMPI.h"
#include <filesystem>
#include <iostream>
#include <stdio.h>
#ifdef GET_STATE_LAYER_INFO_DEBUG_LOGS
#include <bitset>
constexpr int numQubitsMPI = 10;
#endif

int rank, size;
std::vector<unsigned long long> layerAllocs;

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 2) {
		cerr << "Usage: ./simulator <file_name>" << endl << endl;
		exit(EXIT_FAILURE);
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &::rank);
	MPI_Comm_size(MPI_COMM_WORLD, &::size);

#ifdef OUTPUT_LOGS
	if(::rank == 0) {
		filesystem::remove_all("logs");
		filesystem::create_directory("logs");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	appendDebugLog(::rank,
				   ::size,
				   "\n--------------- Node ",
				   ::rank,
				   " logs --------------- \n\n");
#endif

	vector<unsigned int> instructions;

	if(::rank == 0)
		instructions = sourceParser(argv[1]);

	instructionsHandlerMPI(instructions, ::rank, ::size);

	MPI_Barrier(MPI_COMM_WORLD);

	::layerAllocs = calculateLayerAlloc(instructions[0], ::size);
	
	QubitLayerMPI qL(instructions[0]);

#ifdef GET_STATE_LAYER_INFO_DEBUG_LOGS
	appendDebugLog(
		rank, size, "Size of States: ", qL.getLayerAllocs()[rank] / 2, "\n");

	int localStartIndex = qL.getLocalStartIndex();
	size_t j = 0;

	while(j < qL.getStates().size()) {
		appendDebugLog(
			rank, size, "|", std::bitset<numQubitsMPI>(localStartIndex / 2), "> ");

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

	double results[instructions[0] * 2];
	qL.measureQubits(results);
	gatherResultsMPI(::rank, ::size, instructions[0], results);

	// print results
	if(::rank == 0) {
		cout << "Results: \n";
		for(size_t i = 0; i < instructions[0] * 2; i += 2) {
			cout << "Qubit " << (i / 2) + 1 << " -> " << results[i + 1] * 100
				 << "% chance of being ON\n";
		}
	}

#ifdef MEASURE_STATE_VALUES_DEBUG_LOGS
	qL.measure();
#endif

	MPI_Finalize();

	return 0;
}
