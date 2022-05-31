#include "distrEngine.h"
#include "_macros.h"
#include "consoleUtils.h"
#include "flags.h"
#include "mpi.h"
#include "utilsMPI.h"
#include <algorithm>
#include <map>

using namespace std;

void sendStatesOOB(vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesOOB)
{
	MPI_Request mpi_req;
	vector<int> ranks;
	for(int i = 0; i < ::size; i++) {
		ranks.push_back(i);
	}

	int node = -1;
	vector<complex<PRECISION_TYPE>> msgToSend;

#ifdef HANDLER_STATES_DEBUG
	// if(statesOOB.size() != 0) {
	// 	appendDebugLog(rank, size, "Wants to send this: \n");
	// 	for(size_t i = 0; i < statesOOB.size(); i += 2) {
	// 		appendDebugLog(rank,
	// 					   size,
	// 					   "\t|",
	// 					   bitset<numQubitsMPI>(statesOOB[i].real()),
	// 					   "> value: ",
	// 					   statesOOB[i + 1],
	// 					   "\n");
	// 	}
	// 	appendDebugLog(rank, size, "\n");
	// }
#endif

	map<unsigned int, vector<complex<PRECISION_TYPE>>> mapMsgToSend;
	map<unsigned int, vector<complex<PRECISION_TYPE>>>::iterator it;

	for(size_t i = 0; i < statesOOB.size(); ++i) {
		node = getNodeOfState(get<0>(statesOOB[i]));
		uint64_t nodeLocalIndex =
			getLocalIndexFromGlobalState(get<0>(statesOOB[i]), node);

		it = mapMsgToSend.find(node);

		// if hasn't found node
		if(it == mapMsgToSend.end()) {
			vector<complex<PRECISION_TYPE>> vec;
			vec.push_back(nodeLocalIndex);
			vec.push_back(get<1>(statesOOB[i]));
			mapMsgToSend.insert({node, vec});
		}
		// if already has node, append the statesOOB
		else {
			it->second.push_back(nodeLocalIndex);
			it->second.push_back(get<1>(statesOOB[i]));
		}
	}

#ifdef HANDLER_STATES_DEBUG
	// appendDebugLog(rank, size, mapMsgToSend.size(), " ");
	// if(statesOOB.size() != 0) {
	// 	appendDebugLog(rank, size, "MAP msgToSend:\n");
	// 	for(auto it = mapMsgToSend.begin(); it != mapMsgToSend.end(); ++it) {
	// 		appendDebugLog(rank, size, "Node ", it->first, "\n");
	// 		for(size_t i = 0; i < it->second.size(); ++i) {
	// 			if(i % 2 == 0)
	// 				appendDebugLog(rank,
	// 							   size,
	// 							   "\t ",
	// 							   it->second[i],
	// 							   " => |",
	// 							   bitset<numQubitsMPI>(it->second[i].real()),
	// 							   ">\n");
	// 			else
	// 				appendDebugLog(rank, size, "\t ", it->second[i], "\n");
	// 		}
	// 	}
	// }
	// appendDebugLog(rank, size, "\n");
#endif

	/* 	O array abaixo pode ser ajustado para um vla com o tamanho de it->second
		Quanto custa a transmissão no MPI de arrays grandes, talvez
		valha a pena usar vla
	*/
	/**	TODO: Por alguma razão arrays em stack fazem asneira com o q10_grovers 
	 * 	Vale a pena ver isto numa questão de performance, stack é mais rápida que heap
	*/
	// complex<PRECISION_TYPE> msg[MPI_RECV_BUFFER_SIZE];
	complex<double>* msg = new complex<double>[MPI_RECV_BUFFER_SIZE];

	for(auto it = mapMsgToSend.begin(); it != mapMsgToSend.end(); ++it) {
		ranks.erase(remove(ranks.begin(), ranks.end(), it->first), ranks.end());

		copy(it->second.begin(), it->second.end(), msg);
		// Send the array to the intended node, MPI_TAG = tamanho da mensagem
		MPI_Isend(msg,
				  it->second.size(),
				  MPI_DOUBLE_COMPLEX,
				  it->first,
				  it->second.size(),
				  MPI_COMM_WORLD,
				  &mpi_req);
	}

	delete[] msg;

#ifdef HANDLER_STATES_DEBUG
// appendDebugLog(rank, size, "Sending to node ", node, "\n");
// for(size_t z = 0; z < msgToSend.size(); z += 2) {
// 	appendDebugLog(rank,
// 				   size,
// 				   "\t|",
// 				   bitset<numQubitsMPI>(msgToSend[z].real()),
// 				   "> value: ",
// 				   msgToSend[z + 1],
// 				   "\n");
// }
// appendDebugLog(rank, size, "\n");
#endif

	// envia mensagem -1 para todos os ranks que nao receberam uma operacao
	complex<PRECISION_TYPE> end = -1;
	for(size_t i = 0; i < ranks.size(); i++) {
		// exceto a ele proprio
		if(ranks[i] == ::rank)
			continue;

		MPI_Send(&end, 1, MPI_DOUBLE_COMPLEX, ranks[i], 0, MPI_COMM_WORLD);
	}

	return;
}

vector<complex<PRECISION_TYPE>> receiveStatesOOB()
{
	// Constroi um vetor com as operacoes recebidas
	vector<complex<PRECISION_TYPE>> receivedOperations;
	complex<PRECISION_TYPE> msg[MPI_RECV_BUFFER_SIZE];
	msg[0] = 0;
	MPI_Status status;

	for(int node = 0; node < ::size; node++) {
		// exceto a dele proprio
		if(node == ::rank)
			continue;

		MPI_Recv(&msg,
				 MPI_RECV_BUFFER_SIZE,
				 MPI_DOUBLE_COMPLEX,
				 node,
				 MPI_ANY_TAG,
				 MPI_COMM_WORLD,
				 &status);

		// Se mensagem for de uma operacao
		if(status.MPI_TAG != 0) {
			receivedOperations.reserve(status.MPI_TAG);
			receivedOperations.insert(
				receivedOperations.end(), &msg[0], &msg[status.MPI_TAG]);
		}
	}

	return receivedOperations;
}

int getNodeOfState(uint64_t state)
{
	size_t i = 0;
	while(i < ::layerLimits.size()) {
		if(state < ::layerLimits[i]) {
			return i;
		}
		i++;
	}

	return i;

	/** TODO: melhor maneira de fazer isto **/
	// dynamic_bitset lowerBound = 0;
	// dynamic_bitset upperBound = ::layerAllocs[0] / 2;

	// int i = 0;
	// for(; i <= ::size; ++i) {
	// 	if(state < upperBound && state >= lowerBound)
	// 		break;
	// 	lowerBound = upperBound;
	// 	upperBound += ::layerAllocs[i + 1] / 2;
	// }

	// return i;
}
