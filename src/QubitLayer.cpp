#include "QubitLayer.h"

using namespace std;

bool QubitLayer::checkZeroState(int i)
{
	return (this->states[i * 2].real() != 0) || this->states[i * 2].imag() != 0;
}

size_t QubitLayer::calculateJump(int targetQubit)
{
	size_t jump = pow(2, targetQubit - 1) * 2;

	return jump;
}

void QubitLayer::hadamard(int targetQubit)
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
			this->states[2 * state.to_ulong() + 1] -=
				(1 / sqrt(2)) * this->states[2 * i];
		}
	}
	updateStates();
}

void QubitLayer::pauliZ(int targetQubit)
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

void QubitLayer::pauliY(int targetQubit)
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

void QubitLayer::pauliX(int targetQubit)
{
	for(size_t i = 0; i < this->states.size() / 2; i++) {
		if(checkZeroState(i)) {
			bitset<numQubits> state = i;
			state.flip(targetQubit);
			this->states[2 * state.to_ulong() + 1] = this->states[2 * i];
		}
	}
	updateStates();
}

void QubitLayer::measure()
{
	for(size_t i = 0; i < this->states.size(); i += 2) {
		float result = pow(abs(this->states[i]), 2); // not sure...

		// TODO: proper string formatter
		cout << "|" << bitset<3>(i / 2) << "> -> " << result << endl;
	}
}

void QubitLayer::updateStates()
{
	for(size_t i = 0; i < this->states.size(); i += 2) {
		this->states[i] = this->states[i + 1];
		this->states[i + 1] = {0, 0};
	}
}

void QubitLayer::printStateVector()
{
	for(size_t i = 0; i < this->states.size(); i++) {
		cout << this->states[i];
		if(i % 2 == 1)
			cout << " | ";
	}
	cout << endl << endl;
}

QubitLayer::QubitLayer(int qubitCount)
{
	this->qubitCount = qubitCount;
	// calculate total size
	size_t v_size = 2 * pow(2, qubitCount);

	// populate vector with all (0,0), except (1,0) for |00>
	size_t i = 0;
	++i;
	this->states.push_back(1);
	while(i < v_size) {
		this->states.push_back(0);
		++i;
	}
}