#include "utilsMPI.h"
#include "macros.h"
#include "debug.h"
#include "flags.h"
#include "mpi.h"
#include <iostream>

using namespace std;

size_t getLocalIndexFromGlobalState(dynamic_bitset receivedIndex, int node)
{
	dynamic_bitset result = 0;

	for(size_t i = 0; i < ::layerAllocs.size(); ++i) {
		if(i == (size_t)node)
			break;
		result += (::layerAllocs[i] / 2);
	}

	return (receivedIndex - result).to_ullong();
}

dynamic_bitset getLocalStartIndex()
{
	dynamic_bitset result = 0;

	for(int i = 0; i < ::rank; i++) {
		result += ::layerAllocs[i] >> 1;
	}

	return result;
}

void gatherResultsMPI(int rank,
					  int size,
					  unsigned int numQubits,
					  double* finalResults)
{
	MPI_Status status;

	if(rank == 0) {
		double receivedResults[MAX_NUMBER_QUBITS];

		/** TODO: MPI_Gather? **/
		for(int node = 1; node < size; node++) {
			MPI_Recv(&receivedResults,
					 numQubits,
					 MPI_DOUBLE,
					 node,
					 0,
					 MPI_COMM_WORLD,
					 &status);

			for(size_t i = 0; i < numQubits; i++) {
				finalResults[i] += receivedResults[i];
			}
		}
		return;
	} else {
		MPI_Send(finalResults, numQubits, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		return;
	}
}

void instructionsHandlerMPI(vector<unsigned int>& instructions, int rank, int size)
{
	if(rank == 0) {
		vector<int> ranks;
		for(int i = 0; i < size; i++) {
			ranks.push_back(i);
		}

#ifdef INSTRUCTIONS_HANDLER_LOGS
		appendDebugLog("Instructions to send: \n");
		for(size_t i = 0; i < instructions.size(); i++) {
			appendDebugLog(instructions[i], " ");
		}
		appendDebugLog("\n\n");
#endif

		// converter o vetor para array
		unsigned int instructions_arr[(instructions.size())];
		copy(instructions.begin(), instructions.end(), instructions_arr);

		/** TODO: MPI_Broadcast? **/
		// le, parse e envia as instucoes
		for(int i = 1; i < size; i++) {
			MPI_Send(instructions_arr,
					 instructions.size(),
					 MPI_INT,
					 i,
					 instructions.size(),
					 MPI_COMM_WORLD);
		}
	} else {
		// espera para receber as instrucoes
		MPI_Status status;
		unsigned int instructions_arr[MPI_RECV_BUFFER_SIZE];
		MPI_Recv(instructions_arr,
				 MPI_RECV_BUFFER_SIZE,
				 MPI_INT,
				 0,
				 MPI_ANY_TAG,
				 MPI_COMM_WORLD,
				 &status);

		for(int i = 0; i < status.MPI_TAG; i++) {
			instructions.push_back(instructions_arr[i]);
		}

#ifdef INSTRUCTIONS_HANDLER_LOGS
		appendDebugLog("Instructions received: \n");
		for(size_t i = 0; i < instructions.size(); i++) {
			appendDebugLog(instructions[i], " ");
		}
		appendDebugLog("\n\n");
#endif

		return;
	}
}