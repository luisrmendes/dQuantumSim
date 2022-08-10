#include "QubitLayerMPI.hpp"
#include "argparse.hpp"
#include "consoleUtils.hpp"
#include "constants.hpp"
#include "debug.hpp"
#include "debugLogFlags.hpp"
#include "mpi.h"
#include "parser.hpp"
#include "utilsMPI.hpp"
#include <array>
#include <filesystem>
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 2) {
		cerr << "Usage: ./dqsim <file_name>" << endl << endl;
		exit(EXIT_FAILURE);
	}

	/*
		INITIALISE PHASE
			- create debug logs
			- initialise MPI
			- Output useful information
	*/

#ifdef OUTPUT_LOGS
	if(rank == 0) {
		filesystem::remove_all("logs");
		filesystem::create_directory("logs");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	appendDebugLog("\n--------------- Node ", ::rank, " logs --------------- \n\n");
#endif

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if(rank == 0) {
		cout << "\n\n";
		cout << printBold("      ▓▓  ▓▓▓▓▓▓   ▓▓▓▓▓▓  ▓▓▓▓▓▓ ▓▓       ▓▓ ") << endl;
		cout << printBold("      ▓▓ ▓▓    ▓▓ ▓▓         ▓▓   ▓▓▓     ▓▓▓ ") << endl;
		cout << printBold("  ▓▓▓▓▓▓ ▓▓    ▓▓  ▓▓▓▓▓▓    ▓▓   ▓▓ ▓▓ ▓▓ ▓▓ ") << endl;
		cout << printBold("▓▓    ▓▓ ▓▓    ▓▓       ▓▓   ▓▓   ▓▓  ▓▓▓  ▓▓ ") << endl;
		cout << printBold(" ▓▓▓▓▓▓▓   ▓▓▓▓▓▓  ▓▓▓▓▓▓  ▓▓▓▓▓▓ ▓▓       ▓▓ ") << endl;
		cout << printBold("             ▓▓                               ") << endl;
	}

	/*
		PARSE INSTRUCTIONS
			- distribute if necessary
	*/
	vector<unsigned int> instructions = sourceParser(argv[1]);
	unsigned int numQubits = instructions[0];
	// Enable if MPI setup does not share environment across nodes
	// instructions = broadcastInstructions(instructions);

	if(rank == 0) {
		long pages = sysconf(_SC_PHYS_PAGES);
		long page_size = sysconf(_SC_PAGE_SIZE);
		double system_memory = ((pages * page_size) * pow(10, -9)) / 1.073741824;
		double expected_distributed_memory =
			2 * 2 * sizeof(PRECISION_TYPE) * pow(2, numQubits) * pow(10, -9);

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

	/*
		INITIALISE QUBIT REGISTER
			- create debug logs
			- initialise MPI
			- Output useful information
	*/

	if(rank == 0)
		cout << printBold("Allocating vector...\n\n");

	QubitLayerMPI qL(numQubits, rank, size);

	if(rank == 0)
		cout << printBold("Executing operations...\n\n");

	for(size_t i = 1; i < instructions.size(); i++) {

		// Progress output
		if(rank == 0) {
			std::cout << "\x1b[1A"
					  << "\x1b[2K";
			cout << "\tProgress: "
				 << round((float)((float)i / (float)instructions.size()) * 100)
				 << "%" << endl;
		}

		switch(instructions[i]) {
		case 1:
			qL.pauliX(instructions[i + 1]);
			i += 1;
			break;
		case 2:
			qL.pauliY(instructions[i + 1]);
			i += 1;
			break;
		case 3:
			qL.pauliZ(instructions[i + 1]);
			i += 1;
			break;
		case 4:
			qL.hadamard(instructions[i + 1]);
			i += 1;
			break;
		case 5:
			qL.controlledX(instructions[i + 1], instructions[i + 2]);
			i += 2;
			break;
		case 6:
			qL.controlledZ(instructions[i + 1], instructions[i + 2]);
			i += 2;
			break;
		case 7:
			qL.toffoli(
				instructions[i + 1], instructions[i + 2], instructions[i + 3]);
			i += 3;
			break;
		case 8:
			qL.sqrtPauliX(instructions[i + 1]);
			i += 1;
			break;
		case 9:
			qL.sqrtPauliY(instructions[i + 1]);
			i += 1;
			break;
		case 10:
			qL.sGate(instructions[i + 1]);
			i += 1;
			break;
		case 11:
			qL.tGate(instructions[i + 1]);
			i += 1;
			break;
		default:
			cerr << "Unrecognized operation " << instructions[i] << endl;
			exit(EXIT_FAILURE);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	if(rank == 0) {
		cout << "\x1b[1A"
			 << "\x1b[2K";
		cout << "\tProgress: 100%" << endl;
		cout << printBold("\nCalculate state probabilities...\n\n");
	}

	qL.calculateStateProbabilities();

	if(rank == 0)
		cout << printBold("Calculate qubit probabilities...\n\n");

	PRECISION_TYPE qubitProbabilities[MAX_NUMBER_QUBITS] = {
		0}; // array de resultados
	qL.measureQubits(qubitProbabilities);

	MPI_Barrier(MPI_COMM_WORLD);

	if(rank == 0)
		cout << printBold("Gathering all results...\n\n");

	array<PRECISION_TYPE, MAX_NUMBER_QUBITS> gatheredResults;
	gatheredResults = qL.gatherResults(qubitProbabilities);

	// print results
	if(rank == 0) {
		cout << "Results: \n";
		for(size_t i = 0; i < numQubits; i++) {
			cout << "Qubit " << i + 1 << " -> " << gatheredResults[i] * 100
				 << "% chance of being |1>\n";
		}
	}

	MPI_Finalize();

	return 0;
}
