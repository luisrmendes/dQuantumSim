#include "utils.h"
#include "constants.h"
#include "mpi.h"
#include <bitset>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

void instructionsHandlerMPI(vector<unsigned int>& instructions, int rank, int size)
{
	if(rank == 0) {
		vector<int> ranks;
		for(int i = 0; i < size; i++) {
			ranks.push_back(i);
		}

		// converter o vetor para array
		unsigned int instructions_arr[(instructions.size())];
		copy(instructions.begin(), instructions.end(), instructions_arr);

		/** TODO: MPI_Broadcast? **/
		// le, parse e envia as instucoes
		for(int i = 0; i < size; i++) {
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
		return;
	}
}

vector<int> calculateLayerAlloc(int qubitCount, int nodeCount)
{
	vector<int> result;
	size_t layerSize = pow(2, qubitCount);

	if(layerSize / 2 < (size_t)nodeCount) {
		cerr << "Known allocation bug, too many processes for few qubits" << endl;
		exit(-1);
	}

	int quocient = layerSize / nodeCount;
	int remainder = layerSize % nodeCount;

	for(int i = 0; i < nodeCount; ++i) {
		result.push_back(quocient);
	}

	// Spreads the remainder through the vector
	int i = 0;
	while(remainder != 0) {
		result[i] += 1;
		--remainder;
		++i;
	}

	// Duplicate the size of each number
	for(size_t i = 0; i < result.size(); ++i) {
		result[i] *= 2;
	}

	return result;
}
