#include "QubitLayerMPI.h"
#include "debug.h"
#include "macros.h"
#include "mpi.h"
#include "utils.h"
#include <algorithm>
#include <map>

using namespace std;

unsigned int QubitLayerMPI::getLocalIndexFromReceivedOp(unsigned int receivedIndex)
{
	unsigned int result = 0;

	for(unsigned int i = 0; i < this->layerAllocs.size(); ++i) {
		if(i == (unsigned int)this->rank)
			break;

		result += (layerAllocs[i] / 2);
	}

	return receivedIndex - result;
}

unsigned int QubitLayerMPI::getLocalStartIndex()
{
	unsigned int result = 0;

	for(int i = 0; i < rank; i++) {
		result += layerAllocs[i];
	}

	return result;
}

void QubitLayerMPI::measureQubits(double* resultArr)
{
	// Sum all qubits states of the qubit layer
	int localStartIndex = getLocalStartIndex();
	size_t j = 0;
	size_t resultsSize = numQubitsMPI * 2;

	// popular o vetor com os indices dos qubits
	for(unsigned int i = 0; i < resultsSize; i += 2) {
		resultArr[i] = (i / 2) + 1;
		resultArr[i + 1] = 0;
	}

	while(j < this->states.size()) {
		double result = pow(abs(this->states[j]), 2); // not sure...
		bitset<numQubitsMPI> state(localStartIndex / 2);
		for(unsigned int k = 0; k < numQubitsMPI; k++) {
			if(state.test(k))
				resultArr[(k * 2) + 1] += result;
		}

		localStartIndex += 2;
		j += 2;
	}
}

bool QubitLayerMPI::checkStateOOB(bitset<numQubitsMPI> state)
{
	size_t lowerBound = this->rank * (this->states.size() / 2);
	size_t upperBound = (this->rank + 1) * (this->states.size() / 2);

	return state.to_ulong() < lowerBound || state.to_ulong() >= upperBound;
}

vector<complex<double>>
QubitLayerMPI::handlerStatesOOB(vector<complex<double>> statesOOB)
{
	// Receives the vector statesOOB that contains (state, intended_value) pairs,
	// Searches for each state the node that owns the state,
	// Sends a message for the respective node, the state and value for the operation,
	// Sends -1 for every node that had no intended message
	// Receives every message, applies the operation of a received message if its not -1

	// Create vector with all ranks to keep track ranks that have no intended operations
	vector<int> ranks;
	for(int i = 0; i < size; i++) {
		ranks.push_back(i);
	}

	// send messages for all states in the vector
	int node = -1;
	vector<complex<double>> msgToSend;

#ifdef HANDLER_STATES_DEBUG
	if(statesOOB.size() != 0) {
		appendDebugLog(rank, size, "Wants to send this: \n");
		for(size_t i = 0; i < statesOOB.size(); i += 2) {
			appendDebugLog(rank,
						   size,
						   "\t|",
						   bitset<numQubitsMPI>(statesOOB[i].real()),
						   "> value: ",
						   statesOOB[i + 1],
						   "\n");
		}
		appendDebugLog(rank, size, "\n");
	}

#endif

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
	if(statesOOB.size() != 0) {
		appendDebugLog(rank, size, "mapMsgToSend:\n");
		for(auto it = mapMsgToSend.begin(); it != mapMsgToSend.end(); ++it) {
			appendDebugLog(rank, size, "Node ", it->first, "\n");
			for(size_t i = 0; i < it->second.size(); ++i) {
				if(i % 2 == 0)
					appendDebugLog(rank,
								   size,
								   "\t ",
								   it->second[i],
								   " => |",
								   bitset<numQubitsMPI>(it->second[i].real()),
								   ">\n");
				else
					appendDebugLog(rank, size, "\t ", it->second[i], "\n");
			}
		}
	}
	appendDebugLog(rank, size, "\n");
#endif

	for(auto it = mapMsgToSend.begin(); it != mapMsgToSend.end(); ++it) {
		// Erase the rank that has a intended operation
		// probabily a better way to do this
		ranks.erase(remove(ranks.begin(), ranks.end(), it->first), ranks.end());

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
		if(ranks[i] == this->rank)
			continue;

		MPI_Send(&end, 1, MPI_DOUBLE_COMPLEX, ranks[i], 0, MPI_COMM_WORLD);
	}

	// Receber todas as mensagens

	// Constroi um vetor com as operacoes recebidas
	vector<complex<double>> receivedOperations;

	MPI_Status status;
	complex<double> msg[MPI_RECV_BUFFER_SIZE];
	msg[0] = 0;
	for(int node = 0; node < this->size; node++) {
		// exceto a dele proprio
		if(node == this->rank)
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
#ifdef HANDLER_STATES_DEBUG
	if(receivedOperations.size() != 0) {
		appendDebugLog(rank, size, "Has received this: \n");
		for(size_t i = 0; i < receivedOperations.size(); i += 2) {
			appendDebugLog(rank,
						   size,
						   "\t|",
						   bitset<numQubitsMPI>(receivedOperations[i].real()),
						   "> value: ",
						   receivedOperations[i + 1],
						   "\n");
		}
		appendDebugLog(rank, size, "\n");
	}
#endif

	return receivedOperations;
}

int QubitLayerMPI::getNodeOfState(unsigned long state)
{
	return floor(state / (this->states.size() / 2));
}

void QubitLayerMPI::measure()
{
	int localStartIndex = getLocalStartIndex();
	size_t j = 0;

	while(j < this->states.size()) {
		float result = pow(abs(this->states[j]), 2); // not sure...

		appendDebugLog(rank,
					   size,
					   "Node ",
					   rank,
					   ": |",
					   bitset<numQubitsMPI>(localStartIndex / 2),
					   "> -> ",
					   result,
					   "\n");

		localStartIndex += 2;
		j += 2;
	}
	appendDebugLog(rank, size, "\n");
}

void QubitLayerMPI::toffoli(int controlQubit1, int controlQubit2, int targetQubit)
{
	// Executes pauliX if both control qubits are set to |1>
	// vector of (stateOTB, value) pairs
	vector<complex<double>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			if(state.test(controlQubit1) && state.test(controlQubit2)) {
				state.flip(targetQubit);

				// if a state is OTB, store tuple (state, intended_value) to a vector
				if(!checkStateOOB(state)) {
					int localIndex =
						state.to_ulong() - (rank * (this->states.size() / 2));
					this->states[2 * localIndex + 1] = this->states[2 * i];
				} else {

#ifdef TOFFOLI_DEBUG_LOGS
					appendDebugLog("toffoli: State |", state, "> out of bounds!\n");
#endif

					// pair (state, intended_value)
					statesOOB.push_back(state.to_ulong());
					statesOOB.push_back(this->states[2 * i]);
				}
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}

	vector<complex<double>> receivedOps = handlerStatesOOB(statesOOB);

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// calcula o index local do state recebido
		int localIndex =
			receivedOps[i].real() - (this->rank * (this->states.size() / 2));

		// operacao especifica ao pauliX
		this->states[2 * localIndex + 1] = receivedOps[i + 1];
	}

	updateStates();
}

void QubitLayerMPI::controlledX(int controlQubit, int targetQubit)
{
	// Executes pauliX if control qubit is |1>

	// vector of (stateOTB, value) pairs
	vector<complex<double>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			if(state.test(controlQubit)) {
				state.flip(targetQubit);

				// if a state is OTB, store tuple (state, intended_value) to a vector
				if(!checkStateOOB(state)) {
					int localIndex =
						state.to_ulong() - (rank * (this->states.size() / 2));
					this->states[2 * localIndex + 1] = this->states[2 * i];
				} else {

#ifdef CONTROLLEDX_DEBUG_LOGS
					appendDebugLog(
						"ControlledX: State |", state, "> out of bounds!\n");
#endif

					// pair (state, intended_value)
					statesOOB.push_back(state.to_ulong());
					statesOOB.push_back(this->states[2 * i]);
				}
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}

	vector<complex<double>> receivedOps = handlerStatesOOB(statesOOB);

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// calcula o index local do state recebido
		int localIndex =
			receivedOps[i].real() - (this->rank * (this->states.size() / 2));

		// operacao especifica ao pauliX
		this->states[2 * localIndex + 1] = receivedOps[i + 1];
	}

	updateStates();
}

void QubitLayerMPI::controlledZ(int controlQubit, int targetQubit)
{
	// Executes pauliZ if control qubit is |1>
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			if(state.test(controlQubit)) {
				state[targetQubit] == 1
					? this->states[2 * i + 1] = -this->states[2 * i]
					: this->states[2 * i + 1] = this->states[2 * i];
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}
	updateStates();
}

bool QubitLayerMPI::checkZeroState(int i)
{
	return this->states[i * 2].real() != 0 || this->states[i * 2].imag() != 0;
}

void QubitLayerMPI::hadamard(int targetQubit)
{
#ifdef HADAMARD_DEBUG_LOGS
	appendDebugLog(rank, size, "CALLING HADAMARD\n\n");
#endif

	vector<complex<double>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			state.test(targetQubit)
				? this->states[2 * i + 1] -= (1 / sqrt(2)) * this->states[2 * i]
				: this->states[2 * i + 1] += (1 / sqrt(2)) * this->states[2 * i];
		}
	}

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			state.flip(targetQubit);

			if(!checkStateOOB(state)) {
				int localIndex =
					state.to_ulong() - (rank * (this->states.size() / 2));
#ifdef HADAMARD_DEBUG_LOGS
				appendDebugLog(rank,
							   size,
							   "Hadamard: Operation on state |",
							   state,
							   ">, local index ",
							   localIndex,
							   "\n");
				for(size_t i = 0; i < this->states.size(); i++) {
					appendDebugLog(rank, size, this->states[i], "\n");
				}
#endif
				this->states[2 * localIndex + 1] +=
					(1 / sqrt(2)) * this->states[2 * i];
			} else {

#ifdef HADAMARD_DEBUG_LOGS
				appendDebugLog(
					rank, size, "Hadamard: State |", state, "> out of bounds!\n");
#endif
				// pair (state, intended_value)
				statesOOB.push_back(state.to_ulong());
				statesOOB.push_back(this->states[2 * i]);
			}
		}
	}

#ifdef HADAMARD_DEBUG_LOGS
	appendDebugLog(rank, size, "\n");
#endif

	vector<complex<double>> receivedOps = handlerStatesOOB(statesOOB);

#ifdef HADAMARD_DEBUG_LOGS
	for(size_t i = 0; i < receivedOps.size(); ++i) {
		appendDebugLog(rank, size, receivedOps[i], "\n");
	}
#endif

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// calcula o index local do state recebido
		int localIndex = getLocalIndexFromReceivedOp(receivedOps[i].real());
#ifdef HADAMARD_DEBUG_LOGS
		appendDebugLog(rank, size, "Local index: ", localIndex, "\n");
#endif

		this->states[2 * localIndex + 1] += (1 / sqrt(2)) * receivedOps[i + 1];
	}

	updateStates();
}

void QubitLayerMPI::pauliZ(int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);

			state[targetQubit] == 1 ? this->states[2 * i + 1] = -this->states[2 * i]
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
	// vector of (stateOTB, value) pairs
	vector<complex<double>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			// if |0>, scalar 1i applies to |1>
			// if |1>, scalar -1i applies to |0>
			// probabily room for optimization here
			state.flip(targetQubit);

			if(!checkStateOOB(state)) {
				int localIndex =
					state.to_ulong() - (rank * (this->states.size() / 2));

				state[targetQubit] == 0
					? this->states[2 * localIndex + 1] = this->states[2 * i] * 1i
					: this->states[2 * localIndex + 1] = this->states[2 * i] * -1i;

			} else {

#ifdef PAULIY_DEBUG_LOGS
				appendDebugLog("PauliY: State |", state, "> out of bounds!\n");

#endif

				// pair (state, intended_value)
				statesOOB.push_back(state.to_ulong());
				statesOOB.push_back(this->states[2 * i]);
			}
		}
	}

	vector<complex<double>> receivedOps = handlerStatesOOB(statesOOB);

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// calcula o index local do state recebido
		int localIndex =
			receivedOps[i].real() - (this->rank * (this->states.size() / 2));

		this->states[2 * localIndex + 1].real() == 0
			? this->states[2 * localIndex + 1] = receivedOps[i + 1] * 1i
			: this->states[2 * localIndex + 1] = receivedOps[i + 1] * -1i;
	}

	updateStates();
}

void QubitLayerMPI::pauliX(int targetQubit)
{
	// vector of (stateOTB, value) pairs
	vector<complex<double>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i + (rank * this->states.size() / 2);
			state.flip(targetQubit);

			// if a state is OTB, store tuple (state, intended_value) to a vector
			if(!checkStateOOB(state)) {
#ifdef PAULIX_DEBUG_LOGS
				appendDebugLog(rank,
							   size,
							   "State ",
							   state,
							   " in bounds = ",
							   this->states[2 * i],
							   "\n");
#endif
				int localIndex =
					state.to_ulong() - (rank * (this->states.size() / 2));
				this->states[2 * localIndex + 1] = this->states[2 * i];

			} else {

#ifdef PAULIX_DEBUG_LOGS
				appendDebugLog(
					rank, size, "PauliX: State |", state, "> out of bounds!\n");
#endif

				// pair (state, intended_value)
				statesOOB.push_back(state.to_ulong());
				statesOOB.push_back(this->states[2 * i]);
			}
		}
	}

	vector<complex<double>> receivedOps = handlerStatesOOB(statesOOB);

	for(size_t i = 0; i < receivedOps.size(); i += 2) {
		// calcula o index local do state recebido
		int localIndex = getLocalIndexFromReceivedOp(receivedOps[i].real());

#ifdef PAULIX_DEBUG_LOGS
		appendDebugLog(rank, size, "Local Index = ", localIndex, "\n");
#endif
		// operacao especifica ao pauliX
		this->states[2 * localIndex + 1] = receivedOps[i + 1];
	}

	updateStates();
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

QubitLayerMPI::QubitLayerMPI(vector<unsigned int> layerAllocs, int rank, int size)
{
	this->rank = rank;
	this->size = size;
	this->layerAllocs = layerAllocs;
	// populate vector with all (0,0)
	unsigned int i = 0;
	while(i < layerAllocs[rank]) {
		this->states.push_back(0);
		++i;
	}
}