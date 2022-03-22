#include "QubitLayerMPI.h"
#include "debug.h"
#include "distrEngine.h"
#include "macros.h"
#include "mpi.h"
#include "utils.h"
#include "utilsMPI.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <future>
#include <map>
#include <thread>
#include <tuple>
#include <utility>

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

array<double, THREAD_ARRAY_SIZE> func(unsigned int numQubits,
									  dynamic_bitset localStartIndex,
									  size_t start,
									  size_t end)
{
	array<double, THREAD_ARRAY_SIZE> results;
	results.fill(0);

	// cout << "Start: " << start << " End: " << end << endl;

	for(size_t i = start; i < end; i++) {
		for(unsigned int j = 0; j < numQubits; j++) {
			if(localStartIndex.test(j)) {
				results[j] += finalResults[i];
			}
		}
		localStartIndex += 1;
	}

	// cout << "results[2]: " << results[2]
	// 		 << " finalResults[7]: " << finalResults[7] << endl;

	return results;
}

void QubitLayerMPI::measureQubits(double* resultArr)
{
	dynamic_bitset localStartIndex = getLocalStartIndex();

	for(unsigned int i = 0; i < numQubits; i++) {
		resultArr[i] = 0;
	}

	// decide if multithreading of singlethreading
	if(this->states.size() / 2 < 100) {
		cout << "\tSingle Threaded\n\n";
		for(size_t i = 0; i < this->states.size() / 2; i++) {
			for(unsigned int j = 0; j < this->numQubits; j++) {
				if(localStartIndex.test(j)) {
					resultArr[j] += finalResults[i];
				}
			}
			localStartIndex += 1;
		}
	} else {
		cout << "\tMulti Threaded\n\n";
		// unsigned int numThreads = std::thread::hardware_concurrency();
		const unsigned int numThreads = 2;
		const size_t section_size = (this->states.size() / 2) / numThreads;
		// cout << "states size: " << this->states.size() / 2 << endl;

		future<array<double, THREAD_ARRAY_SIZE>> future_thread_1;
		future<array<double, THREAD_ARRAY_SIZE>> future_thread_2;

		// launch threads
		future_thread_1 = async(launch::async,
								&func,
								move(this->numQubits),
								move(localStartIndex),
								move(0),
								move(section_size));
		future_thread_2 = async(launch::async,
								&func,
								move(this->numQubits),
								move(localStartIndex + section_size),
								move(section_size),
								move(finalResults.size()));

		// array of results
		array<double, THREAD_ARRAY_SIZE> all_results[numThreads];

		// gather results
		all_results[0] = future_thread_1.get();
		all_results[1] = future_thread_2.get();

		for(unsigned int i = 0; i < numQubits; i++) {
			resultArr[i] += all_results[0][i];
			resultArr[i] += all_results[1][i];
		}
	}

	// for(unsigned int i = 0; i < numQubits; i++) {
	// 	cout << resultArr[i] << endl;
	// }

	// dynamic_bitset localStartIndex = getLocalStartIndex();

	// // for(size_t i = 0; i < this->states.size() / 2; i++) {
	// // 	cout << finalResults[i] << endl;
	// // }

	// // unsigned int numThreads = std::thread::hardware_concurrency();
	// const unsigned int numThreads = 2;
	// const size_t section_size = (this->states.size() / 2) / numThreads;
	// cout << "states size: " << this->states.size() / 2 << endl;

	// // array of futures
	// future<array<double, THREAD_ARRAY_SIZE>> future_array[numThreads];

	// // launch threads
	// unsigned int section_increments = 0;
	// for(size_t i = 0; i < numThreads; i++) {
	// 	future_array[i] = async(launch::async,
	// 							&func,
	// 							move(this->numQubits),
	// 							move(localStartIndex),
	// 							move(section_increments),
	// 							move(section_increments + section_size));
	// 	section_increments += section_size;
	// 	localStartIndex += section_size;
	// }

	// // array of results
	// array<double, THREAD_ARRAY_SIZE> all_results[numThreads];

	// // gather results
	// for(size_t i = 0; i < numThreads; i++) {
	// 	all_results[i] = future_array[i].get();
	// }

	// // for(size_t i = 0; i < numThreads; i++) {
	// // 	cout << "array 1: " << endl;
	// // 	for(size_t j = 0; j < numQubits; j++) {
	// // 		cout << all_results[i][j] << endl;
	// // 	}
	// // 	cout << endl;
	// // }

	// // join results
	// // Sum all qubits states of the qubit layer
	// // // popular o array com os indices dos qubits
	// for(unsigned int i = 0; i < numQubits; i++) {
	// 	resultArr[i] = 0;
	// }

	// for(unsigned int i = 0; i < numQubits; i++) {
	// 	for(unsigned int j = 0; j < numThreads; j++) {
	// 		resultArr[i] += all_results[j][i];
	// 	}
	// }

	// for(unsigned int i = 0; i < numQubits; i++) {
	// 	cout << resultArr[i] << endl;
	// }

	// // decide if multithreading of singlethreading
	// if(this->states.size() / 2 < 1000) {
	// 	for(size_t i = 0; i < this->states.size() / 2; i++) {
	// 		for(unsigned int k = 0; k < this->numQubits; k++) {
	// 			if(localStartIndex.test(k)) {
	// 				resultArr[(k * 2) + 1] += finalResults[i];
	// 			}
	// 		}
	// 		localStartIndex += 1;
	// 	}
	// } else {
	// 	// unsigned int numThreads = std::thread::hardware_concurrency();
	// 	const unsigned int num_threads = 2;

	// 	// array of futures
	// 	future<array<double, THREAD_ARRAY_SIZE>> future_array[num_threads];

	// 	// size_t section_size = (this->states.size() / 2) / numThreads;

	// 	cout << "State size: " << (this->states.size() / 2)
	// 		 << " sectionsize: " << section_size << " i: " << endl;
	// }
	// }

	/* JUMP Attempt */
	// // Sum all qubits states of the qubit layer
	// dynamic_bitset localStartIndex = getLocalStartIndex();
	// for(size_t i = 0; i < finalResults.size(); i++) {
	// 	cout << localStartIndex.printBitset() << " -> " << finalResults[i] << endl;
	// 	localStartIndex += 1;
	// }

	// // popular o array com os indices dos qubits
	// for(unsigned int i = 0; i < this->numQubits * 2; i += 2) {
	// 	resultArr[i] = (i / 2) + 1;
	// 	resultArr[i + 1] = 0;
	// }

	// // medir qubit 1
	// for(size_t i = 0; i < ::layerAllocs.size(); i++) {
	// 	cout << ::layerAllocs[i] << endl;
	// }

	// for(size_t i = 0; i < 3; i++) {
	// 	size_t start_offset = 0;
	// 	dynamic_bitset localStartIndex = getLocalStartIndex();

	// 	// descobrir qubit 1 a 1 no estado
	// 	while(!localStartIndex.test(i)) {
	// 		localStartIndex += 1;
	// 		start_offset++;
	// 	}

	// 	// if(localStartIndex.getBitset().size() > i + 1) {
	// 	// 	continue;
	// 	// }

	// 	size_t jump = pow(2, i);

	// 	for(size_t j = start_offset; j < finalResults.size(); j++) {
	// 		for(size_t k = 0; k < jump && k < finalResults.size(); k++) {
	// 			if(::rank == 1)
	// 				cout << start_offset << " jump: " << jump << " "
	// 					 << resultArr[(i * 2)] << endl;
	// 			resultArr[(i * 2) + 1] += finalResults[k + j];
	// 		}
	// 		j += jump;
	// 	}
	// }
	// cout << " EXIT" << endl;
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
	// this->states.resize(::layerAllocs[::rank]);
	// fill(this->states.begin(), this->states.end(), 0);

	this->states = vector<complex<double>>(::layerAllocs[::rank], 0);

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
