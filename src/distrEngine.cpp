#include "distrEngine.h"
#include "mpi.h"
#include <algorithm>

using namespace std;

void sendStatesOOB(vector<complex<double>> statesOOB)
{
	// Receives the vector statesOOB that contains (state, intended_value) pairs,
	// Searches for each state the node that owns the state,
	// Sends a message for the respective node, the state and value for the operation,
	// Sends -1 for every node that had no intended message
	// Receives every message, applies the operation of a received message if its not -1

	// Create vector with all ranks to keep track ranks that have no intended operations
	vector<int> ranks;
	for(int i = 0; i < ::size; i++) {
		ranks.push_back(i);
	}

	// send messages for all states in the vector
	int node = -1;
	vector<complex<double>> msgToSend;

	// #ifdef HANDLER_STATES_DEBUG
	// 	// if(statesOOB.size() != 0) {
	// 	// 	appendDebugLog(rank, size, "Wants to send this: \n");
	// 	// 	for(size_t i = 0; i < statesOOB.size(); i += 2) {
	// 	// 		appendDebugLog(rank,
	// 	// 					   size,
	// 	// 					   "\t|",
	// 	// 					   bitset<numQubitsMPI>(statesOOB[i].real()),
	// 	// 					   "> value: ",
	// 	// 					   statesOOB[i + 1],
	// 	// 					   "\n");
	// 	// 	}
	// 	// 	appendDebugLog(rank, size, "\n");
	// 	// }
	// #endif

	// map<node, (states, intended_values)>
	map<unsigned int, vector<complex<double>>> mapMsgToSend;
	map<unsigned int, vector<complex<double>>>::iterator it;

	for(size_t i = 0; i < statesOOB.size(); i += 2) {
		node = getNodeOfState(statesOOB[i].real());

		// check if map already has node
		it = mapMsgToSend.find(node);

		// if hasn't found node
		if(it == mapMsgToSend.end()) {
			vector<complex<double>> vec;
			vec.push_back(statesOOB[i]);
			vec.push_back(statesOOB[i + 1]);
			mapMsgToSend.insert({node, vec});
		}
		// if already has node, append the statesOOB
		else {
			it->second.push_back(statesOOB[i]);
			it->second.push_back(statesOOB[i + 1]);
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
		// Erase the rank that has a intended operation
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

	// #ifdef HANDLER_STATES_DEBUG
	// 	// appendDebugLog(rank, size, "Sending to node ", node, "\n");
	// 	// for(size_t z = 0; z < msgToSend.size(); z += 2) {
	// 	// 	appendDebugLog(rank,
	// 	// 				   size,
	// 	// 				   "\t|",
	// 	// 				   bitset<numQubitsMPI>(msgToSend[z].real()),
	// 	// 				   "> value: ",
	// 	// 				   msgToSend[z + 1],
	// 	// 				   "\n");
	// 	// }
	// 	// appendDebugLog(rank, size, "\n");
	// #endif

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

int getNodeOfState(unsigned long long state)
{
	/** TODO: melhor maneira de fazer isto **/
	unsigned int lowerBound = 0;
	unsigned int upperBound = ::layerAllocs[0] / 2;

	int i = 0;
	for(; i <= ::size; ++i) {
		if(state < upperBound && state >= lowerBound)
			break;
		lowerBound = upperBound;
		upperBound += ::layerAllocs[i + 1] / 2;
	}

	return i;
}