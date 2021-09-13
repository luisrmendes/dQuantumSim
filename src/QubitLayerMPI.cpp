#include "QubitLayerMPI.h"

using namespace std;


void QubitLayerMPI::measure(int rank)
{
	int i = rank * this->states.size();
	size_t j = 0;

	while(j < this->states.size()) {
		float result = pow(abs(this->states[i]), 2); // not sure...
		cout << "|" << bitset<numQubits>(i / 2) << "> -> " << result << endl;

		i += 2;
		j += 2;
	}
}

void QubitLayerMPI::toffoli(int controlQubit1, int controlQubit2, int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubits> state = i;
			if(state.test(controlQubit1) && state.test(controlQubit2)) {
				bitset<numQubits> state = i;
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
			bitset<numQubits> state = i;
			if(state.test(controlQubit)) {
				bitset<numQubits> state = i;
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
			bitset<numQubits> state = i;
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
			bitset<numQubits> state = i;
			if(state.test(targetQubit)) {
				this->states[2 * i + 1] -= (1 / sqrt(2)) * this->states[2 * i];
			} else {
				this->states[2 * i + 1] += (1 / sqrt(2)) * this->states[2 * i];
			}
		}
	}
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubits> state = i;
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
			bitset<numQubits> state = i;
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
			bitset<numQubits> state = i;
			// if |0>, scalar 1i applies to |1>
			// if |1>, scalar -1i aaplies to |0>
			// probabily room for optimization here
			bitset<numQubits> flippedState = state.flip(targetQubit);
			state[targetQubit] == 0 ? this->states[2 * flippedState.to_ulong() + 1] =
										  this->states[2 * i] * 1i
									: this->states[2 * flippedState.to_ulong() + 1] =
										  this->states[2 * i] * -1i;
		}
	}
	updateStates();
}

void QubitLayerMPI::pauliX(int targetQubit, int rank)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubits> state = i;
			state.flip(targetQubit);

			int lowerBound = rank * (this->states.size() / 2);
			int upperBound = (rank + 1) * (this->states.size() / 2);

			if(state.to_ulong() < lowerBound || state.to_ulong() > upperBound) {
				cout << "State out of bounds " << endl;
				int node = getNodeOfState(state.to_ulong());
			}

			this->states[2 * state.to_ulong() + 1] = this->states[2 * i];
		}
	}
	updateStates();
}

void QubitLayerMPI::measure()
{
	for(size_t i = 0; i < this->states.size(); i += 2) {
		float result = pow(abs(this->states[i]), 2); // not sure...

		cout << "|" << bitset<numQubits>(i / 2) << "> -> " << result << endl;
	}
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

QubitLayerMPI::QubitLayerMPI(size_t qLayerSize, int rank)
{
    this->rank = rank;
	// populate vector with all (0,0)
	size_t i = 0;
	while(i < qLayerSize) {
		this->states.push_back(0);
		++i;
	}
}