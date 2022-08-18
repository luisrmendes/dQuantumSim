#include "QubitLayerMPI.hpp"
#include "constants.hpp"
#include "debug.hpp"
#include "debugLogFlags.hpp"
#include "mpi.h"
#include "utilsMPI.hpp"

#define MASK(N) (0x1ull << N)

using namespace std;

void QubitLayerMPI::manageDistr(
	vector<tuple<uint64_t, complex<PRECISION_TYPE>>>& statesOOB,
	const function<void(vector<complex<PRECISION_TYPE>>)>& operationFunc)
{
	vector<complex<PRECISION_TYPE>> receivedOps =
		distributeAndGatherStatesOOB(statesOOB);
	operationFunc(receivedOps);
}

void QubitLayerMPI::calculateStateProbabilities()
{
	for(size_t i = 0; i < this->states.size(); i += 2)
		this->states[i] = pow(abs(this->states[i]), 2);
}

void QubitLayerMPI::measureQubits(PRECISION_TYPE* results)
{
	uint64_t localStartIndex;
	if(this->rank == 0)
		localStartIndex = 0;
	else
		localStartIndex = this->layerLimits[this->rank - 1];

	for(size_t i = 0; i < this->states.size(); i += 2) {
		if(this->states[i] == 0i) {
			localStartIndex += 1;
			continue;
		}
		for(unsigned int j = 0; j < this->numQubits; j++) {
			if(localStartIndex & MASK(j)) {
				results[j] += this->states[i].real();
			}
		}
		localStartIndex += 1;
	}
}

bool QubitLayerMPI::checkStateOOB(uint64_t state)
{
	return state < this->globalLowerBound || state > this->globalUpperBound;
}

void QubitLayerMPI::toffoli(int controlQubit1, int controlQubit2, int targetQubit)
{
	// Executes pauliX if both control qubits are set to |1>
	auto applyReceivedOpsPauliX =
		[&](const vector<complex<PRECISION_TYPE>>& receivedOps) {
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1];
			}
		};

	vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesOOB;

	uint64_t limit = LOCKSTEP_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsPauliX);

			statesOOB.clear();
			limit += LOCKSTEP_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			if(state & MASK(controlQubit1) && state & MASK(controlQubit2)) {
				state = state ^ MASK(targetQubit);

				// if a state is OOB, store tuple (state, intended_value) to a vector
				if(!checkStateOOB(state)) {
					size_t localIndex =
						getLocalIndexFromGlobalState(state, this->rank);
					this->states[2 * localIndex + 1] = this->states[2 * i];
				} else {
#ifdef TOFFOLI_DEBUG_LOGS
					appendDebugLog("toffoli: State |", state, "> out of bounds!\n");
#endif
					// pair (state, intended_value)
					statesOOB.push_back({state, this->states[2 * i]});
				}
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}

	manageDistr(statesOOB, applyReceivedOpsPauliX);

	updateStates();
}

void QubitLayerMPI::controlledX(int controlQubit, int targetQubit)
{
	// Executes pauliX if control qubit is |1>
	auto applyReceivedOpsPauliX =
		[&](const vector<complex<PRECISION_TYPE>>& receivedOps) {
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1];
			}
		};
	// vector of (stateOOB, value) pairs
	vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesOOB;

	uint64_t limit = LOCKSTEP_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsPauliX);

			statesOOB.clear();
			limit += LOCKSTEP_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			if(state & MASK(controlQubit)) {
				state = state ^ MASK(targetQubit);

				// if a state is OOB, store tuple (state, intended_value) to a vector
				if(!checkStateOOB(state)) {
					size_t localIndex =
						getLocalIndexFromGlobalState(state, this->rank);
					this->states[2 * localIndex + 1] = this->states[2 * i];
				} else {
#ifdef CONTROLLEDX_DEBUG_LOGS
					appendDebugLog(
						"ControlledX: State |", state, "> out of bounds!\n");
#endif
					// pair (state, intended_value)
					statesOOB.push_back({state, this->states[2 * i]});
				}
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}

	manageDistr(statesOOB, applyReceivedOpsPauliX);
	updateStates();
}

void QubitLayerMPI::controlledZ(int controlQubit, int targetQubit)
{
	// Executes pauliZ if control qubit is |1>
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			if(state & MASK(controlQubit)) {
				state& MASK(targetQubit)
					? this->states[2 * i + 1] = -this->states[2 * i]
					: this->states[2 * i + 1] = this->states[2 * i];
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}
	updateStates();
}

void QubitLayerMPI::rotationX(int targetQubit, double angle)
{
	// PRECISION_TYPE hadamard_const = 1 / sqrt(2);
	PRECISION_TYPE rotationX_const1 = cos(angle);
	complex<PRECISION_TYPE> rotationX_const2 = -1i * sin(angle);

	vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesOOB;

	auto applyReceivedOpsRotationX =
		[&](const vector<complex<PRECISION_TYPE>>& receivedOps) {
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				this->states[2 * receivedOps[i].real() + 1] +=
					rotationX_const2 * receivedOps[i + 1];
			}
		};

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			(state & MASK(targetQubit))
				? this->states[2 * i + 1] -= rotationX_const1 * this->states[2 * i]
				: this->states[2 * i + 1] += rotationX_const1 * this->states[2 * i];
		}
	}

	size_t limit = LOCKSTEP_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsRotationX);
			statesOOB.clear();
			limit += LOCKSTEP_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			state = state ^ MASK(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, this->rank);
				this->states[2 * localIndex + 1] +=
					rotationX_const2 * this->states[2 * i];
			} else {
				statesOOB.push_back({state, this->states[2 * i]});
			}
		}
	}

	manageDistr(statesOOB, applyReceivedOpsRotationX);

	updateStates();
}

void QubitLayerMPI::sqrtPauliX(int targetQubit)
{
	vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesOOB;

	static const PRECISION_TYPE halfConst = (PRECISION_TYPE)1 / (PRECISION_TYPE)2;
	static const complex<PRECISION_TYPE> localConst = {halfConst, halfConst};
	static const complex<PRECISION_TYPE> remoteConst = {halfConst, -halfConst};

	auto applyReceivedOpsSqrtPauliX =
		[&](const vector<complex<PRECISION_TYPE>>& receivedOps) {
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				this->states[2 * receivedOps[i].real() + 1] +=
					remoteConst * receivedOps[i + 1];
			}
		};

	/** 
	 * TODO: experimentar meter esta operação num único loop
	 */
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			this->states[2 * i + 1] += localConst * this->states[2 * i];
		}
	}

	size_t limit = LOCKSTEP_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsSqrtPauliX);
			statesOOB.clear();
			limit += LOCKSTEP_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			state = state ^ MASK(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, this->rank);
				this->states[2 * localIndex + 1] +=
					remoteConst * this->states[2 * i];
			} else {
				statesOOB.push_back({state, this->states[2 * i]});
			}
		}
	}

	manageDistr(statesOOB, applyReceivedOpsSqrtPauliX);

	updateStates();
}

void QubitLayerMPI::sqrtPauliY(int targetQubit)
{
	vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesOOB;

	static const PRECISION_TYPE halfConst = (PRECISION_TYPE)1 / (PRECISION_TYPE)2;
	static const complex<PRECISION_TYPE> auxConst1 = {halfConst, halfConst};
	static const complex<PRECISION_TYPE> auxConst2 = {-halfConst, -halfConst};

	auto applyReceivedOpsSqrtPauliY =
		[&](const vector<complex<PRECISION_TYPE>>& receivedOps) {
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				uint64_t state = this->globalLowerBound + receivedOps[i].real();

				(state & MASK(targetQubit))
					? this->states[2 * receivedOps[i].real() + 1] +=
					  auxConst1 * receivedOps[i + 1]
					: this->states[2 * receivedOps[i].real() + 1] +=
					  auxConst2 * receivedOps[i + 1];
			}
		};

	/** 
	 * TODO: experimentar meter esta operação num único loop
	 */
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			this->states[2 * i + 1] += auxConst1 * this->states[2 * i];
		}
	}

	size_t limit = LOCKSTEP_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsSqrtPauliY);
			statesOOB.clear();
			limit += LOCKSTEP_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			state = state ^ MASK(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, this->rank);
				(state & MASK(targetQubit)) ? this->states[2 * localIndex + 1] +=
											  auxConst1 * this->states[2 * i]
											: this->states[2 * localIndex + 1] +=
											  auxConst2 * this->states[2 * i];
			} else {
				statesOOB.push_back({state, this->states[2 * i]});
			}
		}
	}

	manageDistr(statesOOB, applyReceivedOpsSqrtPauliY);

	updateStates();
}

void QubitLayerMPI::sGate(int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;

			state& MASK(targetQubit)
				? this->states[2 * i + 1] = 1i * this->states[2 * i]
				: this->states[2 * i + 1] = this->states[2 * i];
		}
	}
	updateStates();
}

void QubitLayerMPI::tGate(int targetQubit)
{
	static const complex<PRECISION_TYPE> tConst =
		exp((1i * (PRECISION_TYPE)M_PI) / (PRECISION_TYPE)4);

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;

			state& MASK(targetQubit)
				? this->states[2 * i + 1] = tConst * this->states[2 * i]
				: this->states[2 * i + 1] = this->states[2 * i];
		}
	}
	updateStates();
}

void QubitLayerMPI::hadamard(int targetQubit)
{
#ifdef HADAMARD_DEBUG_LOGS
	appendDebugLog("--- HADAMARD ---\n\n");
#endif
	// PRECISION_TYPE hadamard_const = 1 / sqrt(2);
	static const PRECISION_TYPE hadamard_const = 1 / sqrt(2);

	vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesOOB;

	auto applyReceivedOpsHadamard =
		[&](const vector<complex<PRECISION_TYPE>>& receivedOps) {
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				this->states[2 * receivedOps[i].real() + 1] +=
					hadamard_const * receivedOps[i + 1];
			}
		};

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			(state & MASK(targetQubit))
				? this->states[2 * i + 1] -= hadamard_const * this->states[2 * i]
				: this->states[2 * i + 1] += hadamard_const * this->states[2 * i];
		}
	}

	size_t limit = LOCKSTEP_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsHadamard);
			statesOOB.clear();
			limit += LOCKSTEP_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			state = state ^ MASK(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, this->rank);
				this->states[2 * localIndex + 1] +=
					hadamard_const * this->states[2 * i];
			} else {
				// #ifdef HADAMARD_DEBUG_LOGS
				// 				dynamic_bitset state2 = state;
				// 				appendDebugLog("Hadamard: State |",
				// 							   state2.printBitset(),
				// 							   "> ",
				// 							   state,
				// 							   " out of bounds!\n");
				// #endif
				// pair (state, intended_value)
				statesOOB.push_back({state, this->states[2 * i]});
			}
		}
	}

	manageDistr(statesOOB, applyReceivedOpsHadamard);

	updateStates();

#ifdef HADAMARD_DEBUG_LOGS
	appendDebugLog("\n--- END HADAMARD ---\n\n");
#endif
}

void QubitLayerMPI::pauliZ(int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;

			state& MASK(targetQubit) ? this->states[2 * i + 1] = -this->states[2 * i]
									 : this->states[2 * i + 1] = this->states[2 * i];

#ifdef PAULIZ_DEBUG_LOGS
			appendDebugLog("State vector before update: ", getStateVector());
#endif
		}
	}
	updateStates();

#ifdef PAULIZ_DEBUG_LOGS
	appendDebugLog("State vector after update: ", printStateVector());
#endif
}

void QubitLayerMPI::pauliY(int targetQubit)
{
#ifdef PAULIY_DEBUG_LOGS
	appendDebugLog("--- PAULI Y ---\n\n");
#endif
	// vector of (stateOOB, value) pairs
	vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesOOB;

	auto applyReceivedOpsPauliY =
		[&](const vector<complex<PRECISION_TYPE>>& receivedOps) {
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				this->states[2 * receivedOps[i].real() + 1].real() == 0
					? this->states[2 * receivedOps[i].real() + 1] =
						  receivedOps[i + 1] * 1i
					: this->states[2 * receivedOps[i].real() + 1] =
						  receivedOps[i + 1] * -1i;
			}
		};

	size_t limit = LOCKSTEP_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsPauliY);

			statesOOB.clear();
			limit += LOCKSTEP_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			// if |0>, scalar 1i applies to |1>
			// if |1>, scalar -1i aclear();
			// probabily room for optimization here
			state = state ^ MASK(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, this->rank);
				state& MASK(targetQubit)
					? this->states[2 * localIndex + 1] = this->states[2 * i] * 1i
					: this->states[2 * localIndex + 1] = this->states[2 * i] * -1i;
			} else {
#ifdef PAULIY_DEBUG_LOGS
				appendDebugLog(
					"PauliY: State |", state.printBitset(), "> out of bounds!\n");
#endif
				// pair (state, intended_value)
				statesOOB.push_back({state, this->states[2 * i]});
			}
		}
	}

	manageDistr(statesOOB, applyReceivedOpsPauliY);

	updateStates();

#ifdef PAULIY_DEBUG_LOGS
	appendDebugLog("--- PAULI Y ---\n\n");
#endif
}

void QubitLayerMPI::pauliX(int targetQubit)
{
#ifdef PAULIX_DEBUG_LOGS
	appendDebugLog("--- PAULI X ---\n\n");
#endif
	// vector of (stateOOB, amplitude) pairs
	vector<tuple<uint64_t, complex<PRECISION_TYPE>>> statesOOB;
	// statesOOB.reserve(LOCKSTEP_THRESHOLD);

	auto applyReceivedOpsPauliX =
		[&](const vector<complex<PRECISION_TYPE>>& receivedOps) {
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1];
			}
		};

	size_t limit = LOCKSTEP_THRESHOLD;
	for(size_t i = 0; (i < this->states.size() / 2); i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsPauliX);
			statesOOB.clear();
			limit += LOCKSTEP_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalLowerBound + i;
			state = state ^ MASK(targetQubit);

			// if a state is OOB, store tuple (state, intended_value) to a vector
			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, this->rank);
				this->states[2 * localIndex + 1] = this->states[2 * i];
			} else {
				// pair (state, amplitude) ATENCAO
				statesOOB.push_back({state, this->states[2 * i]});
			}
		}
	}

	manageDistr(statesOOB, applyReceivedOpsPauliX);

	updateStates();
#ifdef PAULIX_DEBUG_LOGS
	appendDebugLog("--- END PAULI X ---\n\n");
#endif
}

bool QubitLayerMPI::checkZeroState(size_t i) { return this->states[i * 2] != 0i; }

void QubitLayerMPI::updateStates()
{
	for(size_t i = 0; i < this->states.size(); i += 2) {
		this->states[i] = this->states[i + 1];
		this->states[i + 1] = 0i;
	}
}

string QubitLayerMPI::printStateVector()
{
	stringstream stateVector;
	for(size_t i = 0; i < 10; i++) {
		stateVector << this->states[i];

		if(i % 2 == 1)
			stateVector << " | ";
	}
	stateVector << "\n";
	return stateVector.str();
}

vector<size_t> QubitLayerMPI::calculateLayerAlloc()
{
	size_t layerSize = pow(2, this->numQubits);

	if(layerSize / 2 < (size_t)this->size) {
		cerr << "Known allocation bug: too many processes for " << this->numQubits
			 << " qubits" << endl;
		exit(EXIT_FAILURE);
	}

	size_t quocient = layerSize / this->size;
	int remainder = layerSize % this->size;

	vector<size_t> result(this->size, quocient);

	// Spreads the remainder through the vector
	int i = 0;
	while(remainder != 0) {
		result[i] += 1;
		--remainder;
		++i;
	}

	// Duplicate the size of each number
	for(size_t i = 0; i < result.size(); i++) result[i] *= 2;

	return result;
}

vector<size_t> QubitLayerMPI::calculateLayerLimits(vector<size_t> layerAllocs)
{
	vector<uint64_t> layerLimits(layerAllocs.size());

	uint64_t aux = 0;
	for(size_t i = 0; i < layerLimits.size(); i++) {
		aux += layerAllocs[i] / 2;
		layerLimits[i] = aux;
	}

	return layerLimits;
}

size_t QubitLayerMPI::getLocalIndexFromGlobalState(uint64_t globalState,
												   int targetProcess)
{
	if(targetProcess == 0)
		return globalState;

	return (globalState - this->layerLimits[targetProcess - 1]);
	// dynamic_bitset result = 0;

	// for(size_t i = 0; i < ::layerAllocs.size(); ++i) {
	// 	if(i == (size_t)node)
	// 		break;
	// 	result += (::layerAllocs[i] / 2);
	// }

	// return (receivedIndex - result).to_ullong();
}

vector<complex<PRECISION_TYPE>> QubitLayerMPI::distributeAndGatherStatesOOB(
	vector<tuple<uint64_t, complex<PRECISION_TYPE>>>& statesAndAmplitudesOOB)
{
#ifdef DISTR_STATES_OOB
	appendDebugLog("\n\t--- DISTR_STATES_OOB ---\n");
#endif

	vector<vector<complex<PRECISION_TYPE>>> localStatesAmplitudesToSend(
		this->size, vector<complex<PRECISION_TYPE>>());

	/**
	 * Restructure statesOOB into vector(node => {localIndex, ampl})
	 * Vector index points to the target node
	 */
	for(size_t i = 0; i < statesAndAmplitudesOOB.size(); i++) {
		uint64_t* state = &get<0>(statesAndAmplitudesOOB[i]);
		complex<PRECISION_TYPE>* amplitude = &get<1>(statesAndAmplitudesOOB[i]);

		int node = getNodeOfState(*state);
		uint64_t nodeLocalIndex = this->getLocalIndexFromGlobalState(*state, node);

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
			MPI_Isend(&end, 1, MPI_BYTE, targetNode, 0, MPI_COMM_WORLD, &req);
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

	for(int targetNode = 0; targetNode < this->size; targetNode++) {
		if(targetNode == this->rank)
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

int QubitLayerMPI::getNodeOfState(const uint64_t& state)
{
	size_t i = 0;
	for(; i < this->layerLimits.size(); i++) {
		if(state < this->layerLimits[i]) {
			return i;
		}
	}

	return i;
}

std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS>
QubitLayerMPI::gatherResults(PRECISION_TYPE* finalResults)
{
	PRECISION_TYPE* receivedResults =
		new PRECISION_TYPE[MAX_NUMBER_QUBITS * this->size];

	MPI_Gather(finalResults,
			   this->numQubits,
			   MPI_DOUBLE,
			   receivedResults,
			   numQubits,
			   MPI_DOUBLE,
			   0,
			   MPI_COMM_WORLD);

	std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS> gatheredResults = {0};

	if(this->rank == 0) {
		for(unsigned int i = 0; i < numQubits * this->size; i++) {
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

QubitLayerMPI::QubitLayerMPI(unsigned int numQubits, int rank, int size)
{
	this->numQubits = numQubits;
	this->rank = rank;
	this->size = size;

	vector<size_t> layerAllocs = this->calculateLayerAlloc();
	this->layerLimits = this->calculateLayerLimits(layerAllocs);

	// populate vector with all (0,0)
	this->states = vector<complex<PRECISION_TYPE>>(layerAllocs[this->rank], 0);

	// Initialze state vector as |0...0>
	if(this->rank == 0)
		this->states[0] = 1;

	// calculate global indexes
	uint64_t sum = 0;
	for(int i = 0; i < this->rank; i++) {
		sum += layerAllocs[i] / 2;
	}

	this->globalLowerBound = sum;
	this->globalUpperBound = sum + (layerAllocs[this->rank] / 2) - 1;
}
