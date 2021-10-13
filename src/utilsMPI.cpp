#include "utilsMPI.h"
#include "constants.h"
#include "debug.h"
#include "macros.h"
#include "mpi.h"
#include <iostream>

using namespace std;

void gatherResultsMPI(int rank, int size, unsigned int numQubits, double* finalResults)
{
	MPI_Status status;
	size_t resultsSize = numQubits * 2;

	if(rank == 0) {
		double receivedResults[resultsSize];

		/** TODO: MPI_Gather? **/
		for(int node = 1; node < size; node++) {
			MPI_Recv(&receivedResults,
					 resultsSize,
					 MPI_DOUBLE,
					 node,
					 0,
					 MPI_COMM_WORLD,
					 &status);

			for(size_t i = 0; i < resultsSize; i += 2) {
				finalResults[i + 1] += receivedResults[i + 1];
			}
		}
		return;
	} else {
		MPI_Send(finalResults, resultsSize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
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
		appendDebugLog(rank, size, "Instructions to send: \n");
		for(size_t i = 0; i < instructions.size(); i++) {
			appendDebugLog(rank, size, instructions[i], " ");
		}
		appendDebugLog(rank, size, "\n\n");
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
		appendDebugLog(rank, size, "Instructions received: \n");
		for(size_t i = 0; i < instructions.size(); i++) {
			appendDebugLog(rank, size, instructions[i], " ");
		}
		appendDebugLog(rank, size, "\n\n");
#endif

		return;
	}
}