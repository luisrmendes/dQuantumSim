#include "QubitLayerMPI.h"

using namespace std;

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
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			// if |0>, scalar 1i applies to |1>
			// if |1>, scalar -1i aaplies to |0>
			// probabily room for optimization here
			bitset<numQubitsMPI> flippedState = state.flip(targetQubit);
			state[targetQubit] == 0 ? this->states[2 * flippedState.to_ulong() + 1] =
										  this->states[2 * i] * 1i
									: this->states[2 * flippedState.to_ulong() + 1] =
										  this->states[2 * i] * -1i;
		}
	}
	updateStates();
}

void QubitLayerMPI::pauliX(int targetQubit)
{
	// vector of (stateOTB, value) pairs
	vector<complex<double>> statesOTB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			state.flip(targetQubit);

			long unsigned lowerBound = this->rank * (this->states.size() / 2);
			long unsigned upperBound = (this->rank + 1) * (this->states.size() / 2);

			// if a state is OTB, store tuple (state, intended_value) to a vector
			if(state.to_ulong() < lowerBound || state.to_ulong() >= upperBound) {
				cout << "Process " << rank << " says : State |" << state
					 << "> out of bounds!" << endl;

				// pair (state, intended_value)
				statesOTB.push_back(state.to_ulong());
				statesOTB.push_back(this->states[2 * i]);
			} else {
				this->states[2 * state.to_ulong() + 1] = this->states[2 * i];
			}
		}
	}

	// Create vector with all ranks
	vector<int> ranks;
	for(long unsigned int i = 0; i < size; i++) {
		ranks.push_back(i);
	}

	// send messages for all states in the vector
	if(statesOTB.size() != 0) {
		// int value = 1; // means pauliX
		for(size_t i = 0; i < statesOTB.size(); i += 2) {
			// calculate node for the intended message
			int node = getNodeOfState(statesOTB[i].real());
			ranks.erase(ranks.begin() + node);

			// Convert vector into an array acceptable for MPI
			complex<double> msg[statesOTB.size()];
			copy(statesOTB.begin(), statesOTB.end(), msg);

			MPI_Send(msg,
					 statesOTB.size(),
					 MPI_DOUBLE_COMPLEX,
					 node,
					 statesOTB.size(),
					 MPI_COMM_WORLD);

			// cout << endl
			// 	 << "Process " << rank << " sent to " << node << endl
			// 	 << "\tstate: " << msg[0] << endl
			// 	 << "\tvalue: " << msg[1] << endl
			// 	 << endl;
		}
	}

	complex<double> end = -1;

	// Se o processo tinha coisas para enviar, envia -1 a todos os restantes
	if(statesOTB.size() != 0) {
		for(int i = 0; i < ranks.size(); i++) {
			// exceto a ele proprio
			if(ranks[i] == this->rank)
				continue;

			// cout << "Process " << ranks[i] << " sending exit!" << endl;
			MPI_Send(&end, 1, MPI_DOUBLE_COMPLEX, ranks[i], 0, MPI_COMM_WORLD);
		}
	}
	// Se o processo não tinha nada para enviar, envia -1 a todos
	else {
		for(int node = 0; node < this->size; node++) {
			// exceto a ele proprio
			if(node == this->rank)
				continue;

			// cout << "Process " << node << " sending exit!" << endl;
			MPI_Send(&end, 1, MPI_DOUBLE_COMPLEX, node, 0, MPI_COMM_WORLD);
		}
	}

	MPI_Status status;
	complex<double> msg[9];
	msg[0] = 0;

	// Receber todas as mensagens
	for(int node = 0; node < this->size; node++) {
		// exceto a dele proprio
		if(node == this->rank)
			continue;

		MPI_Recv(
			&msg, 9, MPI_DOUBLE_COMPLEX, node, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		// Se mensagem for especial
		if(status.MPI_TAG != 0) {
			cout << "Sou o processo " << this->rank << ", Special packet from "
				 << node << ": " << endl;
			for(int i = 0; i < status.MPI_TAG; i++) {
				cout << msg[i] << endl;
			}
			// calculate local index of state
			int localIndex =
				msg[0].real() - (this->rank * (this->states.size() / 2));
			cout << "local index: " << localIndex << endl;
			this->states[2 * localIndex + 1] = msg[1];
			printStateVector();
		} else {
			// cout << "Exit packet from " << node << ": " << msg[0] << endl;
		}
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