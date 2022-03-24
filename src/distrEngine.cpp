#include "distrEngine.h"
#include "macros.h"
#include "dynamic_bitset.h"
#include "mpi.h"
#include "utilsMPI.h"
#include <algorithm>
#include <map>

using namespace std;

void sendStatesOOB(vector<tuple<dynamic_bitset, complex<double>>> statesOOB)
{
	vector<int> ranks;
	for(int i = 0; i < ::size; i++) {
		ranks.push_back(i);
	}

	int node = -1;
	vector<complex<double>> msgToSend;

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

	map<unsigned int, vector<complex<double>>> mapMsgToSend;
	map<unsigned int, vector<complex<double>>>::iterator it;

	for(size_t i = 0; i < statesOOB.size(); ++i) {
		// mudar para dynamic_bitset
		node = getNodeOfState(get<0>(statesOOB[i]));

		it = mapMsgToSend.find(node);
		uint64_t nodeLocalIndex = getLocalIndexFromGlobalState(get<0>(statesOOB[i]), node);

		// if hasn't found node
		if(it == mapMsgToSend.end()) {
			vector<complex<double>> vec;
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

	for(auto it = mapMsgToSend.begin(); it != mapMsgToSend.end(); ++it) {
		// probabily a better way to do this
		ranks.erase(remove(ranks.begin(), ranks.end(), it->first), ranks.end());

#ifdef RECV_BUFFER_OVERFLOW_WARNING
		if(it->second.size() >= MPI_RECV_BUFFER_SIZE) {
			cerr << "RECV Buffer overload, segfaulting!\nmsgToSend size = "
				 << it->second.size() << endl;
		}
#endif

		complex<double> msg[it->second.size()];
		copy(it->second.begin(), it->second.end(), msg);

		// Send the array to the intended node, MPI_TAG = tamanho da mensagem
		MPI_Send(msg,
				 it->second.size(),
				 MPI_DOUBLE_COMPLEX,
				 it->first,
				 it->second.size(),
				 MPI_COMM_WORLD);
	}

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
	complex<double> end = -1;
	for(size_t i = 0; i < ranks.size(); i++) {
		// exceto a ele proprio
		if(ranks[i] == ::rank)
			continue;

		MPI_Send(&end, 1, MPI_DOUBLE_COMPLEX, ranks[i], 0, MPI_COMM_WORLD);
	}

	return;
}

vector<complex<double>> receiveStatesOOB()
{
	// Receber todas as mensagens

	// Constroi um vetor com as operacoes recebidas
	vector<complex<double>> receivedOperations;

	MPI_Status status;
	complex<double> msg[MPI_RECV_BUFFER_SIZE];
	msg[0] = 0;
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
			for(int i = 0; i < status.MPI_TAG; i++) {
				receivedOperations.push_back(msg[i]);
			}
		}
	}
	// #ifdef HANDLER_STATES_DEBUG
	// 	if(receivedOperations.size() != 0) {
	// 		appendDebugLog(rank, size, "Has received this: \n");
	// 		for(size_t i = 0; i < receivedOperations.size(); i += 2) {
	// 			appendDebugLog(rank,
	// 						   size,
	// 						   "\t|",
	// 						   bitset<numQubitsMPI>(receivedOperations[i].real()),
	// 						   "> value: ",
	// 						   receivedOperations[i + 1],
	// 						   "\n");
	// 		}
	// 		appendDebugLog(rank, size, "\n");
	// 	}
	// #endif

	return receivedOperations;
}

int getNodeOfState(dynamic_bitset state)
{
	/** TODO: melhor maneira de fazer isto **/
	dynamic_bitset lowerBound = 0;
	dynamic_bitset upperBound = ::layerAllocs[0] / 2;

	int i = 0;
	for(; i <= ::size; ++i) {
		if(state < upperBound && state >= lowerBound)
			break;
		lowerBound = upperBound;
		upperBound += ::layerAllocs[i + 1] / 2;
	}

	return i;
}
