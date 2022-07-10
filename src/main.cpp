#include "QubitLayerMPI.h"
#include "_macros.h"
#include "_utils.h"
#include "consoleUtils.h"
#include "debug.h"
#include "flags.h"
#include "mpi.h"
#include "parser.h"
#include "utilsMPI.h"
#include <complex>
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include "_macros.h"
#include <algorithm>
#include <array>
#include <set>
#include <unistd.h>

int rank, size;
std::vector<size_t> layerAllocs; // layer allocation number, input and output pairs
std::vector<uint64_t> layerLimits; // layer limits per node

// struct Com_D_Cmp {
// 	constexpr bool operator()(const std::complex<double>& lhs,
// 							  const std::complex<double>& rhs) const
// 	{
// 		return ((lhs.real() < rhs.real()) || lhs.imag() < rhs.imag());
// 	}
// };

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

	// vector<unsigned int> instructions = sourceParser(argv[1]);
	// instructions = instructionsHandlerMPI(instructions);

	// if(::rank == 0) {
	// 	long pages = sysconf(_SC_PHYS_PAGES);
	// 	long page_size = sysconf(_SC_PAGE_SIZE);
	// 	double system_memory = ((pages * page_size) * pow(10, -9)) / 1.073741824;
	// 	double expected_distributed_memory =
	// 		2 * 2 * sizeof(PRECISION_TYPE) * pow(2, instructions[0]) * pow(10, -9);

	// 	cout << "\nAvaliable System Memory: \n\t~" << system_memory << " GB \n\n";
	// 	if(expected_distributed_memory > system_memory) {
	// 		cout << printYellowBold("Expected Distributed Memory Usage: \n\t~")
	// 			 << expected_distributed_memory << " GB \n\n";
	// 	} else {
	// 		cout << "Expected Distributed Memory Usage: \n\t~"
	// 			 << expected_distributed_memory << " GB \n\n";
	// 	}
	// }

	// MPI_Barrier(MPI_COMM_WORLD);

	// if(::rank == 0)
	// 	cout << printBold("Allocating vector...\n\n");

	unsigned int numQubits = 2;

	::layerAllocs = calculateLayerAlloc(numQubits, ::size);
	::layerLimits = calculateLayerLimits(::layerAllocs);

	QubitLayerMPI qL(numQubits);
	for(size_t i = 0; i < qL.getStates().size(); i+=2)
		cout << qL.getStates()[i] << " ";

	cout << endl;
	
	// qL.rotationX(0, 30);
	qL.sqrtPauliX(0);
	qL.sqrtPauliY(1);
	qL.sqrtPauliZ(0);
	qL.sqrtPauliX(0);
	qL.sqrtPauliY(1);

	for(size_t i = 0; i < qL.getStates().size(); i+=2)
		cout << qL.getStates()[i] << " ";

	cout << endl;
	
	// QubitLayerMPI qL(instructions[0]);

	// dynamic_bitset aux = ::layerLimits[::rank];
	// cout << "Rank: " << ::rank << " layerLimits: " << aux.printBitset() << " "
	// 	 << ::layerLimits[::rank] << " States size: " << qL.getStates().size() << " "
	// 	 << qL.getStates()[qL.getStates().size()] << " globalStart: " << qL.globalStartIndex << " globalEnd: " << qL.globalEndIndex << endl << endl;

#ifdef STATE_VECTOR_INFO
	appendDebugLog("--- STATE_VECTOR_INFO ---\n\n");
	appendDebugLog(
		"Rank ", ::rank, " Size of State Vector: ", qL.getStates().size(), "\n");
	appendDebugLog("Rank ", ::rank, " layerAllocs: ", layerAllocs[::rank], "\n");
	appendDebugLog("Rank ", ::rank, " layerLimits: ", layerLimits[::rank], "\n");
	appendDebugLog(
		"Rank ", ::rank, " globalStartIndex: ", qL.getGlobalStartIndex(), "\n");
	appendDebugLog("Rank ", ::rank, " globalEndIndex: ", qL.getGlobalEndIndex(), "\n");
	appendDebugLog("\n--- STATE_VECTOR_INFO ---\n\n");
#endif

	if(::rank == 0)
		cout << printBold("Executing operations...\n\n");

	// vector<int> num_unique_values;

// 	for(size_t i = 1; i < instructions.size(); i++) {
// #ifdef MEASURE_DEBUG_LOGS
// 		qL.measure();
// #endif
// 		if(::rank == 0) {
// 			std::cout << "\x1b[1A"
// 					  << "\x1b[2K"; // Delete the entire line
// 			cout << "\tProgress: "
// 				 << round((float)((float)i / (float)instructions.size()) * 100)
// 				 << "%" << endl;
// 		}

	// 	switch(instructions[i]) {
	// 	case 1:
	// 		qL.pauliX(instructions[i + 1]);
	// 		i += 1;
	// 		MPI_Barrier(MPI_COMM_WORLD);
	// 		break;
	// 	case 2:
	// 		qL.pauliY(instructions[i + 1]);
	// 		i += 1;
	// 		MPI_Barrier(MPI_COMM_WORLD);
	// 		break;
	// 	case 3:
	// 		qL.pauliZ(instructions[i + 1]);
	// 		i += 1;
	// 		MPI_Barrier(MPI_COMM_WORLD);
	// 		break;
	// 	case 4:
	// 		qL.hadamard(instructions[i + 1]);
	// 		i += 1;
	// 		MPI_Barrier(MPI_COMM_WORLD);
	// 		break;
	// 	case 5:
	// 		qL.controlledX(instructions[i + 1], instructions[i + 2]);
	// 		i += 2;
	// 		MPI_Barrier(MPI_COMM_WORLD);
	// 		break;
	// 	case 6:
	// 		qL.controlledZ(instructions[i + 1], instructions[i + 2]);
	// 		i += 2;
	// 		MPI_Barrier(MPI_COMM_WORLD);
	// 		break;
	// 	case 7:
	// 		qL.toffoli(
	// 			instructions[i + 1], instructions[i + 2], instructions[i + 3]);
	// 		i += 3;
	// 		MPI_Barrier(MPI_COMM_WORLD);
	// 		break;
	// 	default:
	// 		cerr << "Unrecognized operation " << instructions[i] << endl;
	// 		exit(EXIT_FAILURE);
	// 	}
	// 	// num_unique_values.push_back(set<complex<PRECISION_TYPE>, Com_D_Cmp>(
	// 	// 								qL.getStates().begin(), qL.getStates().end())
	// 	// 								.size());
	// }


	MPI_Barrier(MPI_COMM_WORLD);

	if(::rank == 0)
		cout << printBold("\nCalculate state probabilities...\n\n");

	qL.calculateStateProbabilities();

	// qL.clearStates();

	if(::rank == 0)
		cout << printBold("Calculate qubit probabilities...\n");
	PRECISION_TYPE results[MAX_NUMBER_QUBITS] = {0}; // array de resultados
	qL.measureQubits(results);

	MPI_Barrier(MPI_COMM_WORLD);

	if(::rank == 0)
		cout << printBold("Gathering all results...\n\n");
	
	array<PRECISION_TYPE, MAX_NUMBER_QUBITS> gatheredResults;
	gatheredResults = gatherResultsMPI(numQubits, results);

	// print results
	if(::rank == 0) {
		cout << "Results: \n";
		for(size_t i = 0; i < numQubits; i++) {
			cout << "Qubit " << i + 1 << " -> " << gatheredResults[i] * 100
				 << "% chance of being |1>\n";
		}
	}

#ifdef MEASURE_STATE_VALUES_DEBUG_LOGS
	qL.measure();
#endif

	// if(::rank == 0) {
	// 	// cout << endl;
	// 	// for(auto i : num_unique_values) {
	// 	// 	cout << i << " ";
	// 	// }
	// 	cout << endl << endl;

	// 	cout << printBold(to_string(*max_element(num_unique_values.begin(),
	// 											 num_unique_values.end())))
	// 		 << endl
	// 		 << endl;
	// 	// cout << "Number of unique elements: "
	// 	// 	 << std::set<double>(finalResults.begin(), finalResults.end()).size() << endl << endl;
	// }

	MPI_Finalize();

	return 0;
}
