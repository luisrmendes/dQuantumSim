#include "QubitLayerMPI.h"
#include "_macros.h"
#include "_utils.h"
#include "debug.h"
#include "distrEngine.h"
#include "flags.h"
#include "mpi.h"
#include "utilsMPI.h"
#include <future>

using namespace std;

vector<PRECISION_TYPE> QubitLayerMPI::calculateFinalResults()
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

	vector<PRECISION_TYPE> finalResults(this->states.size() / 2);
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		finalResults[i] = pow(abs(this->states[i * 2]), 2);
	}

	return finalResults;
}

void QubitLayerMPI::measureQubits(vector<dynamic_bitset> layerLimits,
								  PRECISION_TYPE* resultArr,
								  vector<PRECISION_TYPE> finalResults)
{
	dynamic_bitset localStartIndex;
	if(::rank == 0)
		localStartIndex = 0;
	else
		localStartIndex = ::layerLimits[::rank - 1];

	auto func = [&](unsigned int numQubits,
					dynamic_bitset localStartIndex,
					size_t start,
					size_t end) {
		array<PRECISION_TYPE, MAX_NUMBER_QUBITS> results;
		results.fill(0);

		for(size_t i = start; i < end; i++) {
			if(finalResults[i] == 0) {
				localStartIndex += 1;
				continue;
			}
			for(unsigned int j = 0; j < numQubits; j++) {
				if(localStartIndex.test(j)) {
					results[j] += finalResults[i];
				}
			}
			localStartIndex += 1;
		}

		return results;
	};

	// decide if multithreading of singlethreading
	if(finalResults.size() < 100) {
		if(::rank == 0)
			cout << "\tSingle Threaded\n\n";

		array<PRECISION_TYPE, MAX_NUMBER_QUBITS> results_aux =
			func(this->numQubits, localStartIndex, 0, finalResults.size());

		for(unsigned int i = 0; i < MAX_NUMBER_QUBITS; i++) {
			resultArr[i] += results_aux[i];
		}
	} else {
		if(::rank == 0)
			cout << "\tMulti Threaded\n\n";

		// unsigned int numThreads = std::thread::hardware_concurrency();
		const unsigned int numThreads = 2;
		const size_t section_size = finalResults.size() / numThreads;

		future<array<PRECISION_TYPE, MAX_NUMBER_QUBITS>> future_thread_1;
		future<array<PRECISION_TYPE, MAX_NUMBER_QUBITS>> future_thread_2;

		// launch threads
		future_thread_1 = async(
			launch::async, func, this->numQubits, localStartIndex, 0, section_size);
		future_thread_2 = async(launch::async,
								func,
								this->numQubits,
								localStartIndex + section_size,
								section_size,
								finalResults.size());

		// array of results
		array<PRECISION_TYPE, MAX_NUMBER_QUBITS> all_results[numThreads];

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

void QubitLayerMPI::toffoli(int controlQubit1, int controlQubit2, int targetQubit)
{
	// Executes pauliX if both control qubits are set to |1>
	// vector of (stateOTB, value) pairs
	vector<tuple<dynamic_bitset, complex<PRECISION_TYPE>>> statesOOB;

	size_t aux = 0;
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
		aux++;
		if(aux == LOCK_STEP_DISTR_THRESHOLD) {
			sendStatesOOB(statesOOB);
			statesOOB.clear();
			vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				// operacao especifica ao pauliX
				this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1];
			}
		}
	}

	sendStatesOOB(statesOOB);
	vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();

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
	vector<tuple<dynamic_bitset, complex<PRECISION_TYPE>>> statesOOB;

	size_t aux = 0;
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

		aux++;
		if(aux == LOCK_STEP_DISTR_THRESHOLD) {
			sendStatesOOB(statesOOB);
			statesOOB.clear();
			vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				// operacao especifica ao pauliX
				this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1];
			}
		}
	}

	sendStatesOOB(statesOOB);
	vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();

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
	constexpr PRECISION_TYPE hadamard_const = 1 / 1.414213562373095;

	vector<tuple<dynamic_bitset, complex<PRECISION_TYPE>>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;
			(state.test(targetQubit))
				? this->states[2 * i + 1] -= hadamard_const * this->states[2 * i]
				: this->states[2 * i + 1] += hadamard_const * this->states[2 * i];
		}
	}

	size_t aux = 0;
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
		aux++;
		if(aux == LOCK_STEP_DISTR_THRESHOLD) {
			sendStatesOOB(statesOOB);
			statesOOB.clear();
			vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				this->states[2 * receivedOps[i].real() + 1] +=
					hadamard_const * receivedOps[i + 1];
			}
			aux = 0;
		}
	}
#ifdef HADAMARD_DEBUG_LOGS
	appendDebugLog("\n");
#endif

	// // if (statesOOB.size() != 0)
	// future<void> sendStatesFuture;
	// sendStatesFuture = async(launch::async, move(sendStatesOOB), move(statesOOB));

	// future<vector<complex<double>>> receivedOpsFuture;
	// receivedOpsFuture = async(launch::async, move(receiveStatesOOB));

	// sendStatesFuture.get();
	// // MPI_Barrier(MPI_COMM_WORLD);
	// vector<complex<double>> receivedOps = receivedOpsFuture.get();

	sendStatesOOB(statesOOB);
	vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();

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
	appendDebugLog("State vector after update: ", printStateVector());
#endif
}

void QubitLayerMPI::pauliY(int targetQubit)
{
#ifdef PAULIY_DEBUG_LOGS
	appendDebugLog("--- PAULI Y ---\n\n");
#endif
	// vector of (stateOOB, value) pairs
	vector<tuple<dynamic_bitset, complex<PRECISION_TYPE>>> statesOOB;

	size_t aux = 0;
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			dynamic_bitset state = this->globalStartIndex + i;
			// if |0>, scalar 1i applies to |1>
			// if |1>, scalar -1i aclear();
			// probabily room for optimization here
			state.flip(targetQubit);

			if(!checkStateOOB(state)) {
				size_t localIndex = getLocalIndexFromGlobalState(state, ::rank);
#ifdef USING_DOUBLE
				state.test(targetQubit)
					? this->states[2 * localIndex + 1] = this->states[2 * i] * 1i
					: this->states[2 * localIndex + 1] = this->states[2 * i] * -1i;
#elif USING_FLOAT
				state.test(targetQubit)
					? this->states[2 * localIndex + 1] = this->states[2 * i] * 1if
					: this->states[2 * localIndex + 1] = this->states[2 * i] * -1if;
#endif

			} else {
#ifdef PAULIY_DEBUG_LOGS
				appendDebugLog(
					"PauliY: State |", state.printBitset(), "> out of bounds!\n");
#endif
				// pair (state, intended_value)
				statesOOB.push_back({state, this->states[2 * i]});
			}
		}
		aux++;
		if(aux == LOCK_STEP_DISTR_THRESHOLD) {
			sendStatesOOB(statesOOB);
			statesOOB.clear();
			vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
#ifdef USING_DOUBLE
				this->states[2 * receivedOps[i].real() + 1].real() == 0
					? this->states[2 * receivedOps[i].real() + 1] =
						  receivedOps[i + 1] * 1i
					: this->states[2 * receivedOps[i].real() + 1] =
						  receivedOps[i + 1] * -1i;
#elif USING_FLOAT
				this->states[2 * receivedOps[i].real() + 1].real() == 0
					? this->states[2 * receivedOps[i].real() + 1] =
						  receivedOps[i + 1] * 1if
					: this->states[2 * receivedOps[i].real() + 1] =
						  receivedOps[i + 1] * -1if;
#endif
			}
			aux = 0;
		}
	}

	sendStatesOOB(statesOOB);
	vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
#ifdef USING_DOUBLE
		this->states[2 * receivedOps[i].real() + 1].real() == 0
			? this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1] * 1i
			: this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1] * -1i;
#elif USING_FLOAT
		this->states[2 * receivedOps[i].real() + 1].real() == 0
			? this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1] * 1if
			: this->states[2 * receivedOps[i].real() + 1] =
				  receivedOps[i + 1] * -1if;
#endif
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
	vector<tuple<dynamic_bitset, complex<PRECISION_TYPE>>> statesOOB;

	size_t aux = 0;
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
		aux++;
		if(aux == LOCK_STEP_DISTR_THRESHOLD) {
			sendStatesOOB(statesOOB);
			statesOOB.clear();
			vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();
			for(size_t i = 0; i < receivedOps.size(); i += 2) {
				// operacao especifica ao pauliX
				this->states[2 * receivedOps[i].real() + 1] = receivedOps[i + 1];
			}
			aux = 0;
		}
	}

	sendStatesOOB(statesOOB);
	vector<complex<PRECISION_TYPE>> receivedOps = receiveStatesOOB();

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
	dynamic_bitset sum = 0;
	for(int i = 0; i < ::rank; i++) {
		sum += ::layerAllocs[i] / 2;
	}

	this->globalStartIndex = sum;
	this->globalEndIndex = sum + (::layerAllocs[::rank] / 2) - 1;
}
