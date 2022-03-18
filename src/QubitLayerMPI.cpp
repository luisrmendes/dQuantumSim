#include "QubitLayerMPI.h"
#include "debug.h"
#include "distrEngine.h"
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

vector<double> QubitLayerMPI::calculateFinalResults()
{
	vector<double> finalResults(this->states.size() / 2, 0);
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		finalResults[i] = pow(abs(this->states[i * 2]), 2);
	}

	return finalResults;
}

void QubitLayerMPI::measureQubits(double* resultArr, vector<double> finalResults)
{
	// // Sum all qubits states of the qubit layer
	// for(size_t i = 0; i < this->states.size(); i++) {
	// 	cout << this->states[i] << endl;
	// }

	// // iniciar o array; 0 -> not searched, 1 -> searched
	// bool qubit_search_flags[this->numQubits] = {0};

	// // popular o array com os indices dos qubits
	// for(unsigned int i = 0; i < this->numQubits * 2; i += 2) {
	// 	resultArr[i] = (i / 2) + 1;
	// 	resultArr[i + 1] = 0;
	// }

	// // medir qubit 1
	// for(size_t i = 0; i < this->numQubits; i++) {
	// 	dynamic_bitset localStartIndex = getLocalStartIndex();
	// 	size_t start_offset = 0;
	// 	// descobrir qubit 1 a 1 no estado
	// 	while(!localStartIndex.test(i)) {
	// 		localStartIndex += 1;
	// 		start_offset++;
	// 	}

	// 	size_t jump = 2 ^ i;

	// 	for(size_t j = start_offset; j < this->states.size(); j += jump) {
	// 		for(size_t k = j; k < jump; k++) {
	// 			resultArr[(k * 2) + 1] += this->states[k].real();
	// 		}
	// 	}
	// }

	// Sum all qubits states of the qubit layer
	unsigned int resultsSize = this->numQubits * 2;

	// popular o array com os indices dos qubits
	for(unsigned int i = 0; i < resultsSize; i += 2) {
		resultArr[i] = (i / 2) + 1;
		resultArr[i + 1] = 0;
	}

	dynamic_bitset localStartIndex = getLocalStartIndex();
	for(size_t i = 0; i < this->states.size() / 2; i++) {

		for(unsigned int k = 0; k < this->numQubits; k++) {
			if(localStartIndex.test(k)) {
				resultArr[(k * 2) + 1] += finalResults[i];
			}
		}

		localStartIndex += 1;
	}
}

bool QubitLayerMPI::checkStateOOB(dynamic_bitset state)
{
	// true if OOB
	// size_t lowerBound = ::rank * (this->states.size() / 2);
	// size_t upperBound = (::rank + 1) * (this->states.size() / 2);
	// bool isBigger = state > this->globalEndIndex;
	// bool isLess = state < this->globalStartIndex;

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

void QubitLayerMPI::toffoli(int controlQubit1, int controlQubit2, int targetQubit)
{
	// Executes pauliX if both control qubits are set to |1>
	// vector of (stateOTB, value) pairs
	vector<tuple<dynamic_bitset, complex<double>>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;
			if(state.test(controlQubit1) && state.test(controlQubit2)) {
				state.flip(targetQubit);

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

	sendStatesOOB(statesOOB);
	vector<complex<double>> receivedOps = receiveStatesOOB();

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// operacao especifica ao pauliX
		this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1];
	}

	updateStates();
}

void QubitLayerMPI::controlledX(int controlQubit, int targetQubit)
{
	// Executes pauliX if control qubit is |1>

	// vector of (stateOOB, value) pairs
	vector<tuple<dynamic_bitset, complex<double>>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;
			if(state.test(controlQubit)) {
				state.flip(targetQubit);

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

	sendStatesOOB(statesOOB);
	vector<complex<double>> receivedOps = receiveStatesOOB();

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// operacao especifica ao pauliX
		this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1];
	}

	updateStates();
}

void QubitLayerMPI::controlledZ(int controlQubit, int targetQubit)
{
	// Executes pauliZ if control qubit is |1>
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;
			if(state.test(controlQubit)) {
				state.test(targetQubit)
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
	constexpr double hadamard_const = 1 / 1.414213562373095;

	vector<tuple<dynamic_bitset, complex<double>>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;
			(state.test(targetQubit))
				? this->states[2 * i + 1] -= hadamard_const * this->states[2 * i]
				: this->states[2 * i + 1] += hadamard_const * this->states[2 * i];
		}
	}

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;
			state.flip(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);
#ifdef HADAMARD_DEBUG_LOGS
				appendDebugLog("Hadamard: Operation on state |",
							   state.printBitset(),
							   ">, local index ",
							   localIndex,
							   "\n");
				for(size_t i = 0; i < this->states.size(); i++) {
					appendDebugLog(this->states[i], "\n");
				}
#endif
				this->states[2 * localIndex + 1] +=
					hadamard_const * this->states[2 * i];
			} else {
#ifdef HADAMARD_DEBUG_LOGS
				appendDebugLog(
					"Hadamard: State |", state.printBitset(), "> out of bounds!\n");
#endif
				// pair (state, intended_value)
				statesOOB.push_back({state, this->states[2 * i]});
			}
		}
	}
#ifdef HADAMARD_DEBUG_LOGS
	appendDebugLog("\n");
#endif
	sendStatesOOB(statesOOB);
	vector<complex<double>> receivedOps = receiveStatesOOB();

#ifdef HADAMARD_DEBUG_LOGS
	for(size_t i = 0; i < receivedOps.size(); ++i) {
		appendDebugLog(receivedOps[i], "\n");
	}
#endif
	for(size_t i = 0; i < receivedOps.size(); i += 2) {
#ifdef HADAMARD_DEBUG_LOGS
		appendDebugLog("Local index: ", receivedOps[i].real(), "\n");
#endif
		this->states[2 * receivedOps[i].real() + 1] +=
			hadamard_const * receivedOps[i + 1];
	}

	updateStates();

#ifdef HADAMARD_DEBUG_LOGS
	appendDebugLog("--- END HADAMARD ---\n\n");
#endif
}

void QubitLayerMPI::pauliZ(int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;

			state.test(targetQubit) ? this->states[2 * i + 1] = -this->states[2 * i]
									: this->states[2 * i + 1] = this->states[2 * i];

#ifdef PAULIZ_DEBUG_LOGS
			appendDebugLog("State vector before update: ", getStateVector());
#endif
		}
	}
	updateStates();

#ifdef PAULIZ_DEBUG_LOGS
	appendDebugLog("State vector after update: ", getStateVector());
#endif
}

void QubitLayerMPI::pauliY(int targetQubit)
{
#ifdef PAULIY_DEBUG_LOGS
	appendDebugLog("--- PAULI Y ---\n\n");
#endif
	// vector of (stateOOB, value) pairs
	vector<tuple<dynamic_bitset, complex<double>>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;
			// if |0>, scalar 1i applies to |1>
			// if |1>, scalar -1i aclear();
			// probabily room for optimization here
			state.flip(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);

				state.test(targetQubit)
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

	sendStatesOOB(statesOOB);
	vector<complex<double>> receivedOps = receiveStatesOOB();

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		this->states[2 * receivedOps[i].real() + 1].real() == 0
			? this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1] * 1i
			: this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1] * -1i;
	}

	updateStates();

#ifdef PAULIY_DEBUG_LOGS
	appendDebugLog("--- PAULI Y ---\n\n");
#endif
}

void QubitLayerMPI::pauliX(int targetQubit)
{
#ifdef PAULIX_DEBUG_LOGS
	appendDebugLog("--- PAULI X ---\n");
#endif

	// vector of (stateOOB, value) pairs
	vector<tuple<dynamic_bitset, complex<double>>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;
			state.flip(targetQubit);

			// if a state is OOB, store tuple (state, intended_value) to a vector
			if(!checkStateOOB(state)) {
#ifdef PAULIX_DEBUG_LOGS
				appendDebugLog("State |",
							   state.printBitset(),
							   "> in bounds = ",
							   this->states[2 * i],
							   "\n");
#endif
				size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);
				this->states[2 * localIndex + 1] = this->states[2 * i];
			} else {
#ifdef PAULIX_DEBUG_LOGS
				appendDebugLog("State |", state.printBitset(), "> out of bounds!\n");
#endif
				// pair (state, intended_value) ATENCAO
				statesOOB.push_back({state, this->states[2 * i]});
			}
		}
	}

	sendStatesOOB(statesOOB);
	vector<complex<double>> receivedOps = receiveStatesOOB();

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
#ifdef PAULIX_DEBUG_LOGS
		appendDebugLog("Local Index = ", receivedOps[i].real(), "\n");
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
	for(size_t i = 0; i < 10; i++) {
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
