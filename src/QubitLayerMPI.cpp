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
		float result = pow(abs(this->states[i]), 2); // not sure...
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
	vector<int> statesOTB;

	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubitsMPI> state = i;
			state.flip(targetQubit);

			int lowerBound = this->rank * (this->states.size() / 2);
			int upperBound = (this->rank + 1) * (this->states.size() / 2);

			// cout << "lower: " << lowerBound << endl;
			// cout << "upper: " << upperBound << endl;
			// cout << "State: " << state.to_ulong() << endl;

			int value;

			// if a state is OTB, store those states to a vector
			if(state.to_ulong() < lowerBound || state.to_ulong() >= upperBound) {
				cout << "Process " << rank << "State out of bounds " << endl;
				// construir um array com (state, valor)
				int msg;
				msg = state.to_ulong();
				// msg[1] = this->states[2 * i];
				statesOTB.push_back(msg);
			} else {
				this->states[2 * state.to_ulong() + 1] = this->states[2 * i];
			}
		}
	}

	// send messages for all states in the vector
	if(statesOTB.size() != 0) {
		// int value = 1; // means pauliX
		for(size_t i = 0; i < statesOTB.size(); i++) {
			int node = getNodeOfState(statesOTB[i]);

			MPI_Send(&statesOTB[i], 1, MPI_INT, node, 0, MPI_COMM_WORLD);
			cout << "Rank " << rank << " has sent to " << node << endl;
		}
	}

	// Mensagem de fim para todas as nodes
	int end[2];
	end[0] = -1;
	end[1] = -1;
	for(int node = 0; node < this->size; node++) {
		if(node == this->rank)
			continue;
		MPI_Send(&end, 1, MPI_INT, node, 0, MPI_COMM_WORLD);
	}

	// Receber todas as mensagens até receber mensagem de fim
	MPI_Status status;
	int msg;
	while(msg != -1) {
		MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		cout << "Node " << status.MPI_SOURCE << " value: " << msg << endl;
		if(msg != -1) {
			this->states[2 * msg + 1] = 1;
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