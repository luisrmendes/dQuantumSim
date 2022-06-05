#include "distrEngine.h"
#include "_macros.h"
#include "consoleUtils.h"
#include "debug.h"
#include "dynamic_bitset.h"
#include "flags.h"
#include "mpi.h"
#include "utilsMPI.h"
#include <algorithm>
#include <map>
#include <utility>
#include <variant>

using namespace std;

vector<complex<PRECISION_TYPE>> distributeAndGatherStatesOOB(
	vector<tuple<uint64_t, complex<PRECISION_TYPE>>>& statesAndAmplitudesOOB)
{
#ifdef DISTR_STATES_OOB
	appendDebugLog("\n\t--- DISTR_STATES_OOB ---\n");
#endif

	vector<vector<complex<PRECISION_TYPE>>> localStatesAmplitudesToSend(
		::size, vector<complex<PRECISION_TYPE>>());

	/**
	 * Restructure statesOOB into vector(node => {localIndex, ampl})
	 * vector index points to the target node
	 */
	for(size_t i = 0; i < statesAndAmplitudesOOB.size(); i++) {
		uint64_t state = get<0>(statesAndAmplitudesOOB[i]);
		complex<PRECISION_TYPE> amplitude = get<1>(statesAndAmplitudesOOB[i]);

		int node = getNodeOfState(state);
		uint64_t nodeLocalIndex = getLocalIndexFromGlobalState(state, node);

		localStatesAmplitudesToSend[node].push_back(
			complex<PRECISION_TYPE>(nodeLocalIndex));
		localStatesAmplitudesToSend[node].push_back(amplitude);
	}

	/**
	 * SEND statesOOB and RECEIVE incomingStates
	 */
	complex<PRECISION_TYPE> recvBuffer[MPI_RECV_BUFFER_SIZE];
	vector<complex<PRECISION_TYPE>> receivedOperations;

	auto syncSend = [&](int targetNode) {
		if(localStatesAmplitudesToSend[targetNode].size() == 0) {
			complex<PRECISION_TYPE> end = -1;
			MPI_Send(&end, 1, MPI_DOUBLE_COMPLEX, targetNode, 0, MPI_COMM_WORLD);
		} else {
			complex<PRECISION_TYPE>* msg =
				&localStatesAmplitudesToSend[targetNode][0];
			MPI_Send(msg,
					 localStatesAmplitudesToSend[targetNode].size(),
					 MPI_DOUBLE_COMPLEX,
					 targetNode,
					 localStatesAmplitudesToSend[targetNode].size(),
					 MPI_COMM_WORLD);
		}
	};

	auto asyncSend = [&](int targetNode) {
		MPI_Request mpi_req;
		if(localStatesAmplitudesToSend[targetNode].size() == 0) {
			complex<PRECISION_TYPE> end = -1;
			MPI_Isend(&end,
					  1,
					  MPI_DOUBLE_COMPLEX,
					  targetNode,
					  0,
					  MPI_COMM_WORLD,
					  &mpi_req);
		} else {
			complex<PRECISION_TYPE>* msg =
				&localStatesAmplitudesToSend[targetNode][0];

			MPI_Isend(msg,
					  localStatesAmplitudesToSend[targetNode].size(),
					  MPI_DOUBLE_COMPLEX,
					  targetNode,
					  localStatesAmplitudesToSend[targetNode].size(),
					  MPI_COMM_WORLD,
					  &mpi_req);
		}
	};

	auto syncReceive = [&](int targetNode) {
		MPI_Status status;
		MPI_Recv(&recvBuffer,
				 MPI_RECV_BUFFER_SIZE,
				 MPI_DOUBLE_COMPLEX,
				 targetNode,
				 MPI_ANY_TAG,
				 MPI_COMM_WORLD,
				 &status);
		if(status.MPI_TAG != 0) {
			receivedOperations.reserve(status.MPI_TAG);
			receivedOperations.insert(receivedOperations.end(),
									  &recvBuffer[0],
									  &recvBuffer[status.MPI_TAG]);
		}
	};

	for(int targetNode = 0; targetNode < ::size; targetNode++) {
		if(targetNode == ::rank)
			continue;
		if(::rank % 2 == 0) {
			syncSend(targetNode);
			syncReceive(targetNode);
		} else {
			syncSend(targetNode);
			syncReceive(targetNode);
		}
	}

#ifdef DISTR_STATES_OOB
	appendDebugLog("\t--- END SEND_STATES_OOB ---\n");
#endif
	return receivedOperations;
}

void sendStatesOOB(
	vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesAndAmplitudesOOB)
{
#ifdef SEND_STATES_OOB
	appendDebugLog("\n\t--- SEND_STATES_OOB ---\n");
#endif

	// MPI_Request mpi_req;

	vector<int> ranks(::size);
	for(int i = 0; i < ::size; i++) ranks[i] = i;

	int node = -1;
	vector<complex<PRECISION_TYPE>> msgToSend;

	map<unsigned int, vector<complex<PRECISION_TYPE>>> mapMsgToSend;
	map<unsigned int, vector<complex<PRECISION_TYPE>>>::iterator it;

	for(size_t i = 0; i < statesAndAmplitudesOOB.size(); i++) {
		uint64_t state = get<0>(statesAndAmplitudesOOB[i]);
		complex<PRECISION_TYPE> amplitude = get<1>(statesAndAmplitudesOOB[i]);

		node = getNodeOfState(state);
		uint64_t nodeLocalIndex = getLocalIndexFromGlobalState(state, node);

		// #ifdef SEND_STATES_OOB
		// 		dynamic_bitset globalIndex = get<0>(statesOOB[i]);
		// 		dynamic_bitset localIndex = nodeLocalIndex;
		// 		appendDebugLog("\tRank ",
		// 					   ::rank,
		// 					   " sending to node ",
		// 					   node,
		// 					   " globalIndex ",
		// 					   globalIndex.printBitset(),
		// 					   " converted to localIndex ",
		// 					   localIndex.printBitset(),
		// 					   "\n");
		// #endif

		it = mapMsgToSend.find(node);

		// if hasn't found node
		if(it == mapMsgToSend.end()) {
			vector<complex<PRECISION_TYPE>> statesAndAmplitudes(2);
			statesAndAmplitudes[0] = nodeLocalIndex;
			statesAndAmplitudes[1] = amplitude;
			mapMsgToSend.insert({node, statesAndAmplitudes});
		}
		// if already has node, append the nodeLocalIndex,amplitude to the
		else {
			it->second.push_back(nodeLocalIndex);
			it->second.push_back(amplitude);
		}
	}

	/* 	O array abaixo pode ser ajustado para um vla com o tamanho de it->second
		Depende de quanto custa a transmissão no MPI de arrays grandes, talvez
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
#ifdef SEND_STATES_OOB
		appendDebugLog("\tRank ",
					   ::rank,
					   " sending to node ",
					   it->first,
					   " statesOOB with size ",
					   it->second.size(),
					   "\n");
#endif
		// Send the array to the intended node, MPI_TAG = tamanho da mensagem
		// MPI_Isend(msg,
		// 		  it->second.size(),
		// 		  MPI_DOUBLE_COMPLEX,
		// 		  it->first,
		// 		  it->second.size(),
		// 		  MPI_COMM_WORLD,
		// 		  &mpi_req);
		MPI_Send(msg,
				 it->second.size(),
				 MPI_DOUBLE_COMPLEX,
				 it->first,
				 it->second.size(),
				 MPI_COMM_WORLD);
	}

	delete[] msg;
	// envia mensagem -1 para todos os ranks que nao receberam uma operacao
	complex<PRECISION_TYPE> end = -1;
	for(size_t i = 0; i < ranks.size(); i++) {

		// exceto a ele proprio
		if(ranks[i] == ::rank)
			continue;
#ifdef SEND_STATES_OOB
		appendDebugLog(
			"\tRank ", ::rank, " sending to node ", ranks[i], " close!\n");
#endif

		MPI_Send(&end, 1, MPI_DOUBLE_COMPLEX, ranks[i], 0, MPI_COMM_WORLD);
	}

#ifdef SEND_STATES_OOB
	appendDebugLog("\t--- END SEND_STATES_OOB ---\n\n");
#endif
	return;
}

vector<complex<PRECISION_TYPE>> receiveStatesOOB()
{
#ifdef RECEIVE_STATES_OOB
	appendDebugLog("\n\t--- RECEIVE_STATES_OOB ---\n");
#endif
	complex<PRECISION_TYPE> msg[MPI_RECV_BUFFER_SIZE];
	msg[0] = 0;
	MPI_Status status;
	// MPI_Request mpi_req;

	vector<complex<PRECISION_TYPE>> receivedOperations;

	/**
	 * Reversing the order of listening for Sends avoids gridlocking
	 * WARN: Maybe a better solution could be necessary
	 */
	for(int node = ::size - 1; node >= 0; node--) {
		// exceto a dele proprio
		if(node == ::rank)
			continue;

#ifdef RECEIVE_STATES_OOB
		appendDebugLog(
			"\tRank ", ::rank, " waiting for message from node ", node, "\n");
#endif

		MPI_Recv(&msg,
				 MPI_RECV_BUFFER_SIZE,
				 MPI_DOUBLE_COMPLEX,
				 node,
				 MPI_ANY_TAG,
				 MPI_COMM_WORLD,
				 &status);
		// MPI_Irecv(&msg,
		// 		 MPI_RECV_BUFFER_SIZE,
		// 		 MPI_DOUBLE_COMPLEX,
		// 		 node,
		// 		 MPI_ANY_TAG,
		// 		 MPI_COMM_WORLD,
		// 		 &mpi_req);

		// MPI_Wait(&mpi_req, &status);
		// #ifdef RECEIVE_STATES_OOB
		// 		appendDebugLog("\tRank ",
		// 					   ::rank,
		// 					   " received tag ",
		// 					   status.MPI_TAG,
		// 					   " from node ",
		// 					   node,
		// 					   "\n");
		// #endif

		// Se mensagem for de uma operacao
		if(status.MPI_TAG != 0) {
			// #ifdef RECEIVE_STATES_OOB
			// 			for(size_t i = 0; i < status.MPI_TAG; i++) {
			// 				appendDebugLog(
			// 					"\t\tRank ", ::rank, " received operations ", msg[i], "\n");
			// 			}
			// #endif
			receivedOperations.reserve(status.MPI_TAG);
			receivedOperations.insert(
				receivedOperations.end(), &msg[0], &msg[status.MPI_TAG]);
		}
	}

#ifdef RECEIVE_STATES_OOB
	appendDebugLog("\n\t--- END RECEIVE_STATES_OOB ---\n");
#endif

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
