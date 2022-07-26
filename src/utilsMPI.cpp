#include "utilsMPI.hpp"
#include "constants.hpp"
#include "debug.hpp"
#include "debugLogFlags.hpp"
#include "mpi.h"
#include <iostream>

using namespace std;

size_t getLocalIndexFromGlobalState(uint64_t receivedIndex, int node)
{
	if(node == 0)
		return receivedIndex;

	return (receivedIndex - ::layerLimits[node - 1]);
	// dynamic_bitset result = 0;

	// for(size_t i = 0; i < ::layerAllocs.size(); ++i) {
	// 	if(i == (size_t)node)
	// 		break;
	// 	result += (::layerAllocs[i] / 2);
	// }

	// return (receivedIndex - result).to_ullong();
}

std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS>
gatherResultsMPI(unsigned int numQubits, PRECISION_TYPE* finalResults)
{
	PRECISION_TYPE* receivedResults = new PRECISION_TYPE[MAX_NUMBER_QUBITS * ::size];

	MPI_Gather(finalResults,
			   numQubits,
			   MPI_DOUBLE,
			   receivedResults,
			   numQubits,
			   MPI_DOUBLE,
			   0,
			   MPI_COMM_WORLD);

	std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS> gatheredResults = {0};

	if(::rank == 0) {
		for(unsigned int i = 0; i < numQubits * ::size; i++) {
			gatheredResults[i % numQubits] += receivedResults[i];
		}
	}

	delete[] receivedResults;
	return gatheredResults;

	// -------------------------------------------------------------------

	// if(rank == 0) {
	// 	PRECISION_TYPE receivedResults[MAX_NUMBER_QUBITS];

	// 	/** TODO: MPI_Gather? **/
	// 	for(int node = 1; node < size; node++) {
	// 		MPI_Recv(&receivedResults,
	// 				 numQubits,
	// 				 MPI_PRECISION_TYPE,
	// 				 node,
	// 				 0,
	// 				 MPI_COMM_WORLD,
	// 				 &status);

	// 		for(size_t i = 0; i < numQubits; i++) {
	// 			finalResults[i] += receivedResults[i];
	// 		}
	// 	}
	// 	return;
	// } else {
	// 	MPI_Send(finalResults, numQubits, MPI_PRECISION_TYPE, 0, 0, MPI_COMM_WORLD);
	// 	return;
	// }
}

vector<unsigned int> instructionsHandlerMPI(vector<unsigned int> &instructions)
{
	size_t num_instructions = instructions.size();
	unsigned int* instructions_arr = &instructions[0];

	MPI_Bcast(&num_instructions, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast(instructions_arr, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	vector<unsigned int> returnInstructions(instructions_arr, instructions_arr + num_instructions);
	return returnInstructions;

	// 	if(rank == 0) {
	// 		vector<int> ranks;
	// 		for(int i = 0; i < size; i++) {
	// 			ranks.push_back(i);
	// 		}

	// #ifdef INSTRUCTIONS_HANDLER_LOGS
	// 		appendDebugLog("Instructions to send: \n");
	// 		for(size_t i = 0; i < instructions.size(); i++) {
	// 			appendDebugLog(instructions[i], " ");
	// 		}
	// 		appendDebugLog("\n\n");
	// #endif

	// 		// converter o vetor para array
	// 		unsigned int* instructions_arr = new unsigned int[instructions.size()];
	// 		copy(instructions.begin(), instructions.end(), instructions_arr);

	// 		/** TODO: MPI_Broadcast? **/
	// 		// le, parse e envia as instucoes
	// 		for(int i = 1; i < size; i++) {
	// 			MPI_Send(instructions_arr,
	// 					 instructions.size(),
	// 					 MPI_INT,
	// 					 i,
	// 					 instructions.size(),
	// 					 MPI_COMM_WORLD);
	// 		}
	// 	} else {
	// 		// espera para receber as instrucoes
	// 		MPI_Status status;
	// 		unsigned int instructions_arr[MPI_RECV_BUFFER_SIZE];
	// 		MPI_Recv(instructions_arr,
	// 				 MPI_RECV_BUFFER_SIZE,
	// 				 MPI_INT,
	// 				 0,
	// 				 MPI_ANY_TAG,
	// 				 MPI_COMM_WORLD,
	// 				 &status);

	// 		for(int i = 0; i < status.MPI_TAG; i++) {
	// 			instructions.push_back(instructions_arr[i]);
	// 		}

#ifdef INSTRUCTIONS_HANDLER_LOGS
	appendDebugLog("Instructions received: \n");
	for(size_t i = 0; i < instructions.size(); i++) {
		appendDebugLog(instructions[i], " ");
	}
	appendDebugLog("\n\n");
#endif

}