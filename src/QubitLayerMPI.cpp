#include "QubitLayerMPI.h"

using namespace std;

bool QubitLayerMPI::checkStateOOB(bitset<numQubitsMPI> state)
{
	long unsigned lowerBound = this->rank * (this->states.size() / 2);
	long unsigned upperBound = (this->rank + 1) * (this->states.size() / 2);

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
	if(statesOOB.size() != 0) {
		for(size_t i = 0; i < statesOOB.size(); i += 2) {

			int node = getNodeOfState(statesOOB[i].real());

			// cout << "Process " << rank << " sending to node " << node << " state "
			// 	 << statesOOB[i].real() << endl;

			// Erase the rank that has a intended operation
			ranks.erase(ranks.begin() + node);

			// Convert vector into an array acceptable for MPI
			complex<double> msg[statesOOB.size()];
			copy(statesOOB.begin(), statesOOB.end(), msg);

			// Send the array to the intended node, MPI_TAG = tamanho da mensagem
			MPI_Send(msg,
					 statesOOB.size(),
					 MPI_DOUBLE_COMPLEX,
					 node,
					 statesOOB.size(),
					 MPI_COMM_WORLD);
		}
	}

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
	int MPI_RECV_BUFFER_SIZE = 10;
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
				// cout << msg[i] << endl;
				receivedOperations.push_back(msg[i]);
			}
		}
	}

	return receivedOperations;
}

int QubitLayerMPI::getNodeOfState(unsigned long state)
{
	int node = floor(state / (this->states.size() / 2));

	return node;
}

void QubitLayerMPI::measure()
{
	int i = this->rank * this->states.size();
	size_t j = 0;

	while(j < this->states.size()) {
		float result = pow(abs(this->states[j]), 2); // not sure...

		cout << "Node " << this->rank << ": ";

		cout << "|" << bitset<numQubitsMPI>(i / 2) << "> -> " << result << endl;

		i += 2;
		j += 2;
	}
}

void QubitLayerMPI::toffoli(int controlQubit1, int controlQubit2, int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			if(state.test(controlQubit1) && state.test(controlQubit2)) {
				bitset<numQubitsMPI> state = i;
				state.flip(targetQubit);
				this->states[2 * state.to_ulong() + 1] = this->states[2 * i];
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}
	updateStates();
}

void QubitLayerMPI::controlledX(int controlQubit, int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			if(state.test(controlQubit)) {
				bitset<numQubitsMPI> state = i;
				state.flip(targetQubit);
				this->states[2 * state.to_ulong() + 1] = this->states[2 * i];
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}
	updateStates();
}

void QubitLayerMPI::controlledZ(int controlQubit, int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			if(state.test(controlQubit)) {
				state[targetQubit] != 0
					? this->states[2 * i + 1].real(this->states[2 * i].real() * -1)
					: this->states[2 * i + 1].real(this->states[2 * i].real());
			} else {
				this->states[2 * i + 1].real(this->states[2 * i].real());
			}
		}
	}
	updateStates();
}

bool QubitLayerMPI::checkZeroState(int i)
{
	return (this->states[i * 2].real() != 0) || this->states[i * 2].imag() != 0;
}

void QubitLayerMPI::hadamard(int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			if(state.test(targetQubit)) {
				this->states[2 * i + 1] -= (1 / sqrt(2)) * this->states[2 * i];
			} else {
				this->states[2 * i + 1] += (1 / sqrt(2)) * this->states[2 * i];
			}
		}
	}
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			state.flip(targetQubit);
			this->states[2 * state.to_ulong() + 1] +=
				(1 / sqrt(2)) * this->states[2 * i];
		}
	}
	updateStates();
}

void QubitLayerMPI::pauliZ(int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			state[targetQubit] != 0
				? this->states[2 * i + 1].real(this->states[2 * i].real() * -1)
				: this->states[2 * i + 1].real(this->states[2 * i].real());
		}
	}
	updateStates();
}

void QubitLayerMPI::pauliY(int targetQubit)
{
	// vector of (stateOTB, value) pairs
	vector<complex<double>> statesOOB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			// if |0>, scalar 1i applies to |1>
			// if |1>, scalar -1i applies to |0>
			// probabily room for optimization here
			bitset<numQubitsMPI> flippedState = state.flip(targetQubit);

			if(!checkStateOOB(flippedState)) {
				state[targetQubit] == 0
					? this->states[2 * flippedState.to_ulong() + 1] =
						  this->states[2 * i] * 1i
					: this->states[2 * flippedState.to_ulong() + 1] =
						  this->states[2 * i] * -1i;

			} else {
				cout << "Process " << rank << " says : State |" << state
					 << "> out of bounds!" << endl;

				// pair (state, intended_value)
				statesOOB.push_back(flippedState.to_ulong());
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
				int localIndex =
					state.to_ulong() - (rank * (this->states.size() / 2));
				this->states[2 * localIndex + 1] = this->states[2 * i];

			} else {
				cout << "Process " << rank << " says : State |" << state
					 << "> out of bounds!" << endl;

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

void QubitLayerMPI::printStateVector()
{
	for(size_t i = 0; i < this->states.size(); i++) {
		cout << this->states[i];
		if(i % 2 == 1)
			cout << " | ";
	}
	cout << endl << endl;
}

QubitLayerMPI::QubitLayerMPI(size_t qLayerSize, int rank, int size)
{
	this->rank = rank;
	this->size = size;
	// populate vector with all (0,0)
	size_t i = 0;
	while(i < qLayerSize) {
		this->states.push_back(0);
		++i;
	}
}