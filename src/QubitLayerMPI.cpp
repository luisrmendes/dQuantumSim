#include "QubitLayerMPI.h"
#include "debug.h"
#include "distrEngine.h"
#include "dynamic_bitset.h"
#include "macros.h"
#include "mpi.h"
#include "utils.h"
#include "utilsMPI.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <tuple>

#define MASK(N) (0x1 << N)

using namespace std;

void QubitLayerMPI::measureQubits(double* resultArr)
{
	// Sum all qubits states of the qubit layer
	long localStartIndex = getLocalStartIndex();
	size_t j = 0;
	size_t resultsSize = this->numQubits * 2;

	// popular o vetor com os indices dos qubits
	for(unsigned int i = 0; i < resultsSize; i += 2) {
		resultArr[i] = (i / 2) + 1;
		resultArr[i + 1] = 0;
	}

	while(j < this->states.size()) {
		double result = pow(abs(this->states[j]), 2); // not sure...
		decltype(localStartIndex) state = (localStartIndex / 2);

		for(unsigned int k = 0; k < this->numQubits; k++) {
			if(state & MASK(k))
				resultArr[(k * 2) + 1] += result;
		}

		localStartIndex += 2;
		j += 2;
	}
}

bool QubitLayerMPI::checkStateOOB(dynamic_bitset state)
{
	// true if OOB
	// size_t lowerBound = ::rank * (this->states.size() / 2);
	// size_t upperBound = (::rank + 1) * (this->states.size() / 2);
	return state < this->globalStartIndex || state > this->globalEndIndex;
}

#ifdef MEASURE_STATE_VALUES_DEBUG_LOGS
#include <bitset>
constexpr int numQubitsMPI = 32;
void QubitLayerMPI::measure()
{
	int localStartIndex = getLocalStartIndex();
	size_t j = 0;

	while(j < this->states.size()) {
		float result = pow(abs(this->states[j]), 2); // not sure...

		appendDebugLog("Node ",
					   ::rank,
					   ": |",
					   bitset<numQubitsMPI>(localStartIndex / 2),
					   "> -> ",
					   result,
					   "\n");

		localStartIndex += 2;
		j += 2;
	}
	appendDebugLog("\n");
}
#endif

// void QubitLayerMPI::toffoli(int controlQubit1, int controlQubit2, int targetQubit)
// {
// 	// Executes pauliX if both control qubits are set to |1>
// 	// vector of (stateOTB, value) pairs
// 	vector<complex<double>> statesOOB;

// 	for(size_t i = 0; i < this->states.size() / 2; i++) {
// 		if(checkZeroState(i)) {
// 			unsigned long long state = i + this->globalStartIndex;
// 			if((state & MASK(controlQubit1)) && (state & MASK(controlQubit2))) {
// 				state = state ^ MASK(targetQubit);

// 				// if a state is OTB, store tuple (state, intended_value) to a vector
// 				if(!checkStateOOB(state)) {
// 					int localIndex = getLocalIndexFromGlobalState(state);
// 					this->states[2 * localIndex + 1] = this->states[2 * i];
// 				} else {

// #ifdef TOFFOLI_DEBUG_LOGS
// 					appendDebugLog("toffoli: State |", state, "> out of bounds!\n");
// #endif

// 					// pair (state, intended_value)
// 					statesOOB.push_back(state);
// 					statesOOB.push_back(this->states[2 * i]);
// 				}
// 			} else {
// 				this->states[2 * i + 1].real(this->states[2 * i].real());
// 			}
// 		}
// 	}

// 	sendStatesOOB(statesOOB);
// 	vector<complex<double>> receivedOps = receiveStatesOOB();

// 	for(size_t i = 0; i < receivedOps.size(); i += 2) {
// 		// calcula o index local do state recebido
// 		int localIndex = getLocalIndexFromGlobalState(receivedOps[i].real());

// 		// operacao especifica ao pauliX
// 		this->states[2 * localIndex + 1] = receivedOps[i + 1];
// 	}

// 	updateStates();
// }

// void QubitLayerMPI::controlledX(int controlQubit, int targetQubit)
// {
// 	// Executes pauliX if control qubit is |1>

// 	// vector of (stateOTB, value) pairs
// 	vector<complex<double>> statesOOB;

// 	for(size_t i = 0; i < this->states.size() / 2; i++) {
// 		if(checkZeroState(i)) {
// 			unsigned long long state = i + this->globalStartIndex;
// 			if(state & MASK(controlQubit)) {
// 				state = state ^ MASK(targetQubit);

// 				// if a state is OTB, store tuple (state, intended_value) to a vector
// 				if(!checkStateOOB(state)) {
// 					int localIndex = getLocalIndexFromGlobalState(state);
// 					this->states[2 * localIndex + 1] = this->states[2 * i];
// 				} else {

// #ifdef CONTROLLEDX_DEBUG_LOGS
// 					appendDebugLog(
// 						"ControlledX: State |", state, "> out of bounds!\n");
// #endif

// 					// pair (state, intended_val
// 					statesOOB.push_back(state);
// 					statesOOB.push_back(this->states[2 * i]);
// 				}
// 			} else {
// 				this->states[2 * i + 1].real(this->states[2 * i].real());
// 			}
// 		}
// 	}

// 	sendStatesOOB(statesOOB);
// 	vector<complex<double>> receivedOps = receiveStatesOOB();

// 	for(size_t i = 0; i < receivedOps.size(); i += 2) {
// 		// calcula o index local do state recebido
// 		int localIndex = getLocalIndexFromGlobalState(receivedOps[i].real());

// 		// operacao especifica ao pauliX
// 		this->states[2 * localIndex + 1] = receivedOps[i + 1];
// 	}

// 	updateStates();
// }

// void QubitLayerMPI::controlledZ(int controlQubit, int targetQubit)
// {
// 	// Executes pauliZ if control qubit is |1>
// 	for(size_t i = 0; i < this->states.size() / 2; i++) {
// 		if(checkZeroState(i)) {
// 			unsigned long long state = i + this->globalStartIndex;
// 			if(state & MASK(controlQubit)) {
// 				(state & MASK(targetQubit)) == 1
// 					? this->states[2 * i + 1] = -this->states[2 * i]
// 					: this->states[2 * i + 1] = this->states[2 * i];
// 			} else {
// 				this->states[2 * i + 1].real(this->states[2 * i].real());
// 			}
// 		}
// 	}
// 	updateStates();
// }

// void QubitLayerMPI::hadamard(int targetQubit)
// {
// #ifdef HADAMARD_DEBUG_LOGS
// 	appendDebugLog("CALLING HADAMARD\n\n");
// #endif

// 	constexpr double hadamard_const = 1 / 1.414213562373095;

// 	vector<complex<double>> statesOOB;

// 	for(size_t i = 0; i < this->states.size() / 2; i++) {
// 		if(checkZeroState(i)) {
// 			unsigned long long state = i + this->globalStartIndex;
// 			(state & MASK(targetQubit))
// 				? this->states[2 * i + 1] -= hadamard_const * this->states[2 * i]
// 				: this->states[2 * i + 1] += hadamard_const * this->states[2 * i];
// 		}
// 	}

// 	for(size_t i = 0; i < this->states.size() / 2; i++) {
// 		if(checkZeroState(i)) {
// 			unsigned long long state = i + this->globalStartIndex;
// 			state = state ^ MASK(targetQubit);

// 			if(!checkStateOOB(state)) {
// 				int localIndex = getLocalIndexFromGlobalState(state);
// #ifdef HADAMARD_DEBUG_LOGS
// 				appendDebugLog("Hadamard: Operation on state |",
// 							   state,
// 							   ">, local index ",
// 							   localIndex,
// 							   "\n");
// 				for(size_t i = 0; i < this->states.size(); i++) {
// 					appendDebugLog(this->states[i], "\n");
// 				}
// #endif
// 				this->states[2 * localIndex + 1] +=
// 					hadamard_const * this->states[2 * i];
// 			} else {

// #ifdef HADAMARD_DEBUG_LOGS
// 				appendDebugLog("Hadamard: State |", state, "> out of bounds!\n");
// #endif
// 				// pair (state, intended_value)
// 				statesOOB.push_back(state);
// 				statesOOB.push_back(this->states[2 * i]);
// 			}
// 		}
// 	}

// #ifdef HADAMARD_DEBUG_LOGS
// 	appendDebugLog("\n");
// #endif

// 	sendStatesOOB(statesOOB);
// 	vector<complex<double>> receivedOps = receiveStatesOOB();

// #ifdef HADAMARD_DEBUG_LOGS
// 	for(size_t i = 0; i < receivedOps.size(); ++i) {
// 		appendDebugLog(receivedOps[i], "\n");
// 	}
// #endif

// 	for(size_t i = 0; i < receivedOps.size(); i += 2) {
// 		// calcula o index local do state recebido
// 		int localIndex = getLocalIndexFromGlobalState(receivedOps[i].real());
// #ifdef HADAMARD_DEBUG_LOGS
// 		appendDebugLog("Local index: ", localIndex, "\n");
// #endif

// 		this->states[2 * localIndex + 1] += hadamard_const * receivedOps[i + 1];
// 	}

// 	updateStates();
// }

// void QubitLayerMPI::pauliZ(int targetQubit)
// {
// 	for(size_t i = 0; i < this->states.size() / 2; i++) {
// 		if(checkZeroState(i)) {
// 			unsigned long long state = i + this->globalStartIndex;

// 			(state & MASK(targetQubit)) == 1
// 				? this->states[2 * i + 1] = -this->states[2 * i]
// 				: this->states[2 * i + 1] = this->states[2 * i];

// #ifdef PAULIZ_DEBUG_LOGS
// 			appendDebugLog("State vector before update: ", getStateVector());
// #endif
// 		}
// 	}
// 	updateStates();

// #ifdef PAULIZ_DEBUG_LOGS
// 	appendDebugLog("State vector after update: ", getStateVector());
// #endif
// }

// void QubitLayerMPI::pauliY(int targetQubit)
// {
// 	// vector of (stateOTB, value) pairs
// 	vector<complex<double>> statesOOB;

// 	for(size_t i = 0; i < this->states.size() / 2; i++) {
// 		if(checkZeroState(i)) {
// 			unsigned long long state = i + this->globalStartIndex;
// 			// if |0>, scalar 1i applies to |1>
// 			// if |1>, scalar -1i aclear();

// testSum();
// testSub();
// testCompressBitsets();
// testNormalizeBitsets();

// cout << endl;plies to |0>
// 			// probabily room for optimization here
// 			state = state ^ MASK(targetQubit);

// 			if(!checkStateOOB(state)) {
// 				int localIndex = getLocalIndexFromGlobalState(state);

// 				(state & MASK(targetQubit)) == 0
// 					? this->states[2 * localIndex + 1] = this->states[2 * i] * 1i
// 					: this->states[2 * localIndex + 1] = this->states[2 * i] * -1i;

// 			} else {

// #ifdef PAULIY_DEBUG_LOGS
// 				appendDebugLog("PauliY: State |", state, "> out of bounds!\n");

// #endif

// 				// pair (state, intended_value)
// 				statesOOB.push_back(state);
// 				statesOOB.push_back(this->states[2 * i]);
// 			}
// 		}
// 	}

// 	sendStatesOOB(statesOOB);
// 	vector<complex<double>> receivedOps = receiveStatesOOB();

// 	for(size_t i = 0; i < receivedOps.size(); i += 2) {
// 		// calcula o index local do state recebido
// 		int localIndex = getLocalIndexFromGlobalState(receivedOps[i].real());

// 		this->states[2 * localIndex + 1].real() == 0
// 			? this->states[2 * localIndex + 1] = receivedOps[i + 1] * 1i
// 			: this->states[2 * localIndex + 1] = receivedOps[i + 1] * -1i;
// 	}

// 	updateStates();
// }

void QubitLayerMPI::pauliX(int targetQubit)
{
#ifdef PAULIX_DEBUG_LOGS
	appendDebugLog("--- PAULI X ---\n");
#endif

	// vector of (stateOTB, value) pairs
	vector<tuple<dynamic_bitset, complex<double>>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = (this->globalStartIndex + i);
			state.flip(targetQubit);
			// if a state is OOB, store tuple (state, intended_value) to a vector
			if(!checkStateOOB(state)) {
#ifdef PAULIX_DEBUG_LOGS
				appendDebugLog("State |",
							   state.to_ullong(),
							   "> in bounds = ",
							   this->states[2 * i],
							   "\n");
#endif
				size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);
				this->states[2 * localIndex + 1] = this->states[2 * i];
			} else {
#ifdef PAULIX_DEBUG_LOGS
				appendDebugLog("State |", state.to_ullong(), "> out of bounds!\n");
#endif
				// pair (state, intended_value) ATENCAO
				statesOOB.push_back({state, this->states[2 * i]});
				// statesOOB.push_back(this->states[2 * i]);
			}
		}
	}

	sendStatesOOB(statesOOB);
	vector<complex<double>> receivedOps = receiveStatesOOB();

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// calcula o index local do state recebido

		// unsigned long long localIndex =
		// 	getLocalIndexFromGlobalState(receivedOps[i].real());

		// cout << receivedOps[i].real() << endl;
		// cout << localIndex << endl;

#ifdef PAULIX_DEBUG_LOGS
		appendDebugLog("Local Index = ", localIndex, "\n");
#endif
		// operacao especifica ao pauliX
		this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1];
	}
	updateStates();

#ifdef PAULIX_DEBUG_LOGS
	appendDebugLog("--- END PAULI X ---\n\n");
#endif
}

bool QubitLayerMPI::checkZeroState(size_t i)
{
	return this->states[i * 2].real() != 0 || this->states[i * 2].imag() != 0;
}

void QubitLayerMPI::updateStates()
{
	for(size_t i = 0; i < this->states.size(); i += 2) {
		this->states[i] = this->states[i + 1];
		this->states[i + 1] = {0, 0};
	}
}

string QubitLayerMPI::getStateVector()
{
	stringstream stateVector;
	for(size_t i = 0; i < this->states.size(); i++) {
		stateVector << this->states[i];

		if(i % 2 == 1)
			stateVector << " | ";
	}
	stateVector << "\n";
	return stateVector.str();
}

void QubitLayerMPI::printStateVector()
{
	for(size_t i = 0; i < this->states.size(); i++) {
		cout << this->states[i];

		if(i % 2 == 1)
			cout << " | ";
	}
	cout << endl << endl;
}

QubitLayerMPI::QubitLayerMPI(unsigned int numQubits)
{
	this->numQubits = numQubits;

	// populate vector with all (0,0)
	unsigned long long i = 0;
	while(i < ::layerAllocs[::rank]) {
		this->states.push_back(0);
		++i;
	}

	// Initialze state vector as |0...0>
	if(::rank == 0)
		this->states[0] = 1;

	// calculate global indexes
	dynamic_bitset sum = 0;
	for(int i = 0; i < ::rank; i++) {
		sum += ::layerAllocs[i] / 2;
	}

	this->globalStartIndex = sum;
	this->globalEndIndex = sum + (::layerAllocs[::rank] / 2) - 1;
}
