#include "distrEngine.h"
#include "_macros.h"
#include "consoleUtils.h"
#include "debug.h"
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
	 * Vector index points to the target node
	 */
	for(size_t i = 0; i < statesAndAmplitudesOOB.size(); i++) {
		uint64_t* state = &get<0>(statesAndAmplitudesOOB[i]);
		complex<PRECISION_TYPE>* amplitude = &get<1>(statesAndAmplitudesOOB[i]);

		int node = getNodeOfState(*state);
		uint64_t nodeLocalIndex = getLocalIndexFromGlobalState(*state, node);

		localStatesAmplitudesToSend[node].push_back(
			complex<PRECISION_TYPE>(nodeLocalIndex));
		localStatesAmplitudesToSend[node].push_back(*amplitude);
	}

	/**
	 * SEND statesOOB and RECEIVE incomingStates
	 */
	complex<PRECISION_TYPE> recvBuffer[MPI_RECV_BUFFER_SIZE];
	vector<complex<PRECISION_TYPE>> receivedOperations;
	MPI_Request req;
	MPI_Status status;

	auto nonBlockingSend = [&](int targetNode) {
		if(localStatesAmplitudesToSend[targetNode].size() == 0) {
			bool end = -1;
			MPI_Isend(
				&end, 1, MPI_BYTE, targetNode, 0, MPI_COMM_WORLD, &req);
		} else {
			complex<PRECISION_TYPE>* msg =
				&localStatesAmplitudesToSend[targetNode][0];

			MPI_Isend(msg,
					  localStatesAmplitudesToSend[targetNode].size() * 8 * 2,
					  MPI_BYTE,
					  targetNode,
					  localStatesAmplitudesToSend[targetNode].size(),
					  MPI_COMM_WORLD,
					  &req);
		}
	};

	auto blockingReceive = [&](int targetNode) {
		MPI_Recv(&recvBuffer,
				 MPI_RECV_BUFFER_SIZE * 8 * 2,
				 MPI_BYTE,
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
		nonBlockingSend(targetNode);
		blockingReceive(targetNode);
		MPI_Wait(&req, &status);
	}

#ifdef DISTR_STATES_OOB
	appendDebugLog("\t--- END SEND_STATES_OOB ---\n");
#endif
	return receivedOperations;
}

int getNodeOfState(const uint64_t& state)
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
