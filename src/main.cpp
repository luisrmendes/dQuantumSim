#include "QubitLayerMPI.h"
#include "_utils.h"
#include "consoleUtils.h"
#include "debug.h"
#include "dynamic_bitset.h"
#include "flags.h"
#include "macros.h"
#include "mpi.h"
#include "parser.h"
#include "utilsMPI.h"
#include <filesystem>
#include <iostream>
#ifdef GET_STATE_LAYER_INFO_DEBUG_LOGS
#include <bitset>
constexpr int numQubitsMPI = 10;
#endif
#include <unistd.h>

int rank, size;
std::vector<size_t> layerAllocs; // layer allocation number, input and output pairs

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

	if(::rank == 0) {
		cout << "\n\n";
		cout << printBold("      ▓▓  ▓▓▓▓▓▓   ▓▓▓▓▓▓  ▓▓▓▓▓▓ ▓▓       ▓▓ ") << endl;
		cout << printBold("      ▓▓ ▓▓    ▓▓ ▓▓         ▓▓   ▓▓▓     ▓▓▓ ") << endl;
		cout << printBold("  ▓▓▓▓▓▓ ▓▓    ▓▓  ▓▓▓▓▓▓    ▓▓   ▓▓ ▓▓ ▓▓ ▓▓ ") << endl;
		cout << printBold("▓▓    ▓▓ ▓▓    ▓▓       ▓▓   ▓▓   ▓▓  ▓▓▓  ▓▓ ") << endl;
		cout << printBold(" ▓▓▓▓▓▓▓   ▓▓▓▓▓▓  ▓▓▓▓▓▓  ▓▓▓▓▓▓ ▓▓       ▓▓ ") << endl;
		cout << printBold("             ▓▓                               ") << endl;
	}

#ifdef OUTPUT_LOGS
	if(::rank == 0) {
		filesystem::remove_all("logs");
		filesystem::create_directory("logs");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	appendDebugLog("\n--------------- Node ", ::rank, " logs --------------- \n\n");
#endif

	vector<unsigned int> instructions;
	// if(::rank == 0)
	instructions = sourceParser(argv[1]);

	// instructionsHandlerMPI(instructions, ::rank, ::size);

	if(::rank == 0) {
		long pages = sysconf(_SC_PHYS_PAGES);
		long page_size = sysconf(_SC_PAGE_SIZE);
		double system_memory = ((pages * page_size) * pow(10, -9)) / 1.073741824;
		double expected_distributed_memory =
			2 * 2 * 8 * pow(2, instructions[0]) * pow(10, -9);

		cout << "\nAvaliable System Memory: \n\t~" << system_memory << " GB \n\n";
		if(expected_distributed_memory > system_memory) {
			cout << printYellowBold("Expected Distributed Memory Usage: \n\t~")
				 << expected_distributed_memory << " GB \n\n";
		} else {
			cout << "Expected Distributed Memory Usage: \n\t~"
				 << expected_distributed_memory << " GB \n\n";
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if(::rank == 0)
		cout << printBold("Allocating vector...\n\n");

	::layerAllocs = calculateLayerAlloc(instructions[0], ::size);

	QubitLayerMPI qL(instructions[0]);

#ifdef GET_STATE_LAYER_INFO_DEBUG_LOGS
	appendDebugLog("Size of States: ", layerAllocs[::rank] / 2, "\n");

	int localStartIndex = qL.getLocalStartIndex();
	size_t j = 0;

	while(j < qL.getStates().size()) {
		appendDebugLog("|", std::bitset<numQubitsMPI>(localStartIndex / 2), "> ");

		localStartIndex += 2;
		j += 2;
	}
	appendDebugLog("\n");
#endif

	if(::rank == 0)
		cout << printBold("Executing operations...\n\n");

	for(size_t i = 1; i < instructions.size(); i++) {
#ifdef MEASURE_DEBUG_LOGS
		qL.measure();
#endif
		if(::rank == 0) {
			std::cout << "\x1b[1A"
					  << "\x1b[2K"; // Delete the entire line
			cout << "\tProgress: "
				 << round((float)((float)i / (float)instructions.size()) * 100)
				 << "%" << endl;
		}

		// cout << "Rank: " << ::rank << " " << instructions[i] << " " << i << endl;
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

	if(::rank == 0)
		cout << printBold("Calculate final results...\n\n");

	std::vector<double> finalResults = qL.calculateFinalResults();
	qL.clearStates();

	if(::rank == 0)
		cout << printBold("Gathering results...\n");

	double results[MAX_NUMBER_QUBITS] = {0}; // array de resultados
	qL.measureQubits(results, finalResults);
	gatherResultsMPI(::rank, ::size, instructions[0], results);

	// print results
	if(::rank == 0) {
		cout << "Results: \n";
		for(size_t i = 0; i < instructions[0]; i++) {
			cout << "Qubit " << i + 1 << " -> " << results[i] * 100
				 << "% chance of being ON\n";
		}
	}

#ifdef MEASURE_STATE_VALUES_DEBUG_LOGS
	qL.measure();
#endif

	MPI_Finalize();

	return 0;
}
