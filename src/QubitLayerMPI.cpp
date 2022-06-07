#include "QubitLayerMPI.h"
#include "_macros.h"
#include "_utils.h"
#include "debug.h"
#include "distrEngine.h"
#include "dynamic_bitset.h"
#include "flags.h"
#include "mpi.h"
#include "utilsMPI.h"
#include <array>
#include <functional>
#include <future>

#define MASK(N) (0x1ull << N)

using namespace std;

void QubitLayerMPI::manageDistr(
	vector<StateAndAmplitude>& statesOOB,
	function<void(vector<StateAndAmplitude>&)> operationFunc)
{

	// sendStatesOOB(statesOOB);
	// vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();
	// operationFunc(receivedOps);
	// MPI_Barrier(MPI_COMM_WORLD);
	vector<StateAndAmplitude> receivedOps = distributeAndGatherStatesOOB(statesOOB);
	operationFunc(receivedOps);
	// MPI_Barrier(MPI_COMM_WORLD);
}

void QubitLayerMPI::calculateStateProbabilities()
{
	// pre-calculate sub-iteration times
	// size_t num_sub_iterations = (this->states.size()/2) / iteration_size;
	// if(num_sub_iterations == 0) {
	// 	for(size_t i = 0; i < this->states.size(); i += 2) {
	// 		results[i / 2] = pow(abs(this->states[i]), 2);
	// 	}
	// } else {
	// 	cout << num_sub_iterations << endl;
	// 	for(uint64_t i = 0; i < num_sub_iterations; i++) {
	// 		for(size_t j = 0; j < iteration_size; j++) {
	// 			results[j + (iteration_size * i)] = pow(abs(this->states.at(j * 2)), 2);
	// 		}
	// 		this->states.erase(this->states.begin(),
	// 						   this->states.begin() + iteration_size * 2);
	// 		this->states.shrink_to_fit();
	// 	}
	// }

	for(size_t i = 0; i < this->states.size(); i += 2) {
		this->states[i] = pow(abs(this->states[i]), 2);
	}
}

void QubitLayerMPI::measureQubits(vector<uint64_t> layerLimits,
								  PRECISION_TYPE* results)
{
	uint64_t localStartIndex;
	if(::rank == 0)
		localStartIndex = 0;
	else
		localStartIndex = ::layerLimits[::rank - 1];

	// auto get_results = [&](unsigned int numQubits,
	// 					   uint64_t localStartIndex,
	// 					   size_t start,
	// 					   size_t end) {
	// 	array<PRECISION_TYPE, MAX_NUMBER_QUBITS> results = {0};

	// 	for(size_t i = start; i < end; i++) {
	// 		if(finalResults[i] == 0) {
	// 			localStartIndex += 1;
	// 			continue;
	// 		}
	// 		for(unsigned int j = 0; j < numQubits; j++) {
	// 			if(localStartIndex & MASK(j)) {
	// 				results[j] += finalResults[i];
	// 			}
	// 		}
	// 		localStartIndex += 1;
	// 	}

	// 	return results;
	// };

	// decide if multithreading of singlethreading

	if(::rank == 0)
		cout << "\tSingle Threaded\n\n";

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

	// } else {
	// 	if(::rank == 0)
	// 		cout << "\tMulti Threaded\n\n";

	// 	// unsigned int numThreads = std::thread::hardware_concurrency();
	// 	const unsigned int numThreads = 2;
	// 	const size_t section_size = finalResults.size() / numThreads;

	// 	future<array<PRECISION_TYPE, MAX_NUMBER_QUBITS>> future_thread_1;
	// 	future<array<PRECISION_TYPE, MAX_NUMBER_QUBITS>> future_thread_2;

	// 	// launch threads
	// 	future_thread_1 = async(launch::async,
	// 							get_results,
	// 							this->numQubits,
	// 							localStartIndex,
	// 							0,
	// 							section_size);
	// 	future_thread_2 = async(launch::async,
	// 							get_results,
	// 							this->numQubits,
	// 							localStartIndex + section_size,
	// 							section_size,
	// 							finalResults.size());

	// 	// array of results
	// 	array<PRECISION_TYPE, MAX_NUMBER_QUBITS> all_results[numThreads];

	// 	// gather results
	// 	all_results[0] = future_thread_1.get();
	// 	all_results[1] = future_thread_2.get();

	// 	for(unsigned int i = 0; i < numQubits; i++) {
	// 		resultArr[i] += all_results[0][i];
	// 		resultArr[i] += all_results[1][i];
	// 	}
	// }
}

bool QubitLayerMPI::checkStateOOB(uint64_t state)
{
	// true if OOB
	// size_t lowerBound = ::rank * (this->states.size() / 2);
	// size_t upperBound = (::rank + 1) * (this->states.size() / 2);
	// bool isBigger = state > this->globalEndIndex;
	// bool isLess = state < this->globalStartIndex;

	return state < this->globalStartIndex || state > this->globalEndIndex;
}

void QubitLayerMPI::toffoli(int controlQubit1, int controlQubit2, int targetQubit)
{
	// Executes pauliX if both control qubits are set to |1>
	auto applyReceivedOpsPauliX = [&](vector<StateAndAmplitude>& receivedOps) {
		for(size_t i = 0; i < receivedOps.size(); i++) {
			this->states[2 * receivedOps[i].getState() + 1] = receivedOps[i].getAmplitude();
		}
	};

	vector<StateAndAmplitude> statesOOB;

	uint64_t limit = LOCK_STEP_DISTR_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsPauliX);

			statesOOB.clear();
			limit += LOCK_STEP_DISTR_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalStartIndex + i;
			if(state & MASK(controlQubit1) && state & MASK(controlQubit2)) {
				state = state ^ MASK(targetQubit);

				// if a state is OOB, store tuple (state, intended_value) to a vector
				if(!checkStateOOB(state)) {
					size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);
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
	auto applyReceivedOpsPauliX = [&](vector<StateAndAmplitude>& receivedOps) {
		for(size_t i = 0; i < receivedOps.size(); i++) {
			this->states[2 * receivedOps[i].getState() + 1] = receivedOps[i].getAmplitude();
		}
	};
	// vector of (stateOOB, value) pairs
	vector<StateAndAmplitude> statesOOB;

	uint64_t limit = LOCK_STEP_DISTR_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsPauliX);

			statesOOB.clear();
			limit += LOCK_STEP_DISTR_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalStartIndex + i;
			if(state & MASK(controlQubit)) {
				state = state ^ MASK(targetQubit);

				// if a state is OOB, store tuple (state, intended_value) to a vector
				if(!checkStateOOB(state)) {
					size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);
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
			uint64_t state = this->globalStartIndex + i;
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

void QubitLayerMPI::hadamard(int targetQubit)
{
#ifdef HADAMARD_DEBUG_LOGS
	appendDebugLog("--- HADAMARD ---\n\n");
#endif
	// PRECISION_TYPE hadamard_const = 1 / sqrt(2);
	constexpr PRECISION_TYPE hadamard_const = 0.7071067811865475244008;

	vector<StateAndAmplitude> statesOOB;

	auto applyReceivedOpsHadamard =
		[&](vector<StateAndAmplitude>& receivedOps) {
			for(size_t i = 0; i < receivedOps.size(); i++) {
				this->states[2 * receivedOps[i].getState() + 1] +=
					hadamard_const * receivedOps[i].getAmplitude();
			}
		};

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			uint64_t state = this->globalStartIndex + i;
			(state & MASK(targetQubit))
				? this->states[2 * i + 1] -= hadamard_const * this->states[2 * i]
				: this->states[2 * i + 1] += hadamard_const * this->states[2 * i];
		}
	}

	size_t limit = LOCK_STEP_DISTR_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsHadamard);
			statesOOB.clear();
			limit += LOCK_STEP_DISTR_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalStartIndex + i;
			state = state ^ MASK(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);
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
			uint64_t state = this->globalStartIndex + i;

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
	vector<StateAndAmplitude> statesOOB;

	auto applyReceivedOpsPauliY = [&](const vector<StateAndAmplitude>& receivedOps) {
		for(size_t i = 0; i < receivedOps.size(); i++) {
			this->states[2 * receivedOps[i].state + 1].real() == 0
				? this->states[2 * receivedOps[i].state + 1] =
					  receivedOps[i].amplitude * 1i
				: this->states[2 * receivedOps[i].state + 1] =
					  receivedOps[i].amplitude * -1i;
		}
	};

	size_t limit = LOCK_STEP_DISTR_THRESHOLD;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsPauliY);

			statesOOB.clear();
			limit += LOCK_STEP_DISTR_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalStartIndex + i;
			// if |0>, scalar 1i applies to |1>
			// if |1>, scalar -1i aclear();
			// probabily room for optimization here
			state = state ^ MASK(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);
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
	vector<StateAndAmplitude> statesOOB;
	// statesOOB.reserve(LOCK_STEP_DISTR_THRESHOLD);

	auto applyReceivedOpsPauliX = [&](vector<StateAndAmplitude>& receivedOps) {
		for(size_t i = 0; i < receivedOps.size(); i++) {
			this->states[2 * receivedOps[i].getState() + 1] = receivedOps[i].getAmplitude();
		}
	};
	size_t limit = LOCK_STEP_DISTR_THRESHOLD;
	for(size_t i = 0; (i < this->states.size() / 2); i++) {
		if(i == limit) {
			manageDistr(statesOOB, applyReceivedOpsPauliX);

			statesOOB.clear();
			limit += LOCK_STEP_DISTR_THRESHOLD;
		}
		if(checkZeroState(i)) {
			uint64_t state = this->globalStartIndex + i;
			state = state ^ MASK(targetQubit);

			// if a state is OOB, store tuple (state, intended_value) to a vector
			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);
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

QubitLayerMPI::QubitLayerMPI(unsigned int numQubits)
{
	this->numQubits = numQubits;

	// populate vector with all (0,0)
	this->states = vector<complex<PRECISION_TYPE>>(::layerAllocs[::rank], 0);

	// Initialze state vector as |0...0>
	if(::rank == 0)
		this->states[0] = 1;

	// calculate global indexes
	uint64_t sum = 0;
	for(int i = 0; i < ::rank; i++) {
		sum += ::layerAllocs[i] / 2;
	}

	this->globalStartIndex = sum;
	this->globalEndIndex = sum + (::layerAllocs[::rank] / 2) - 1;
}
