#include "QubitLayer.h"

using namespace std;

bool QubitLayer::checkZeroState(int i) { return this->states[i].real() != 0; }

size_t QubitLayer::calculateJump(int targetQubit)
{
	size_t jump = pow(2, targetQubit - 1) * 2;

	return jump;
}

void QubitLayer::hadamard(int targetQubit)
{
	// calculate jump
	size_t jump = calculateJump(targetQubit);
	unsigned int jumpCounter = 0;

	for(size_t i = 0; i < this->states.size(); i += jump) {
		// check if real value is different than 0
		if(checkZeroState(i)) {
			// check if state is |0>
			if(jumpCounter % 2 == 0) {
				// |0> = 1/sqrt(2) |0>
				this->states[i + 1].real(this->states[i].real() * (1 / sqrt(2)));
				// |0> = 1/sqrt(2) |1>
				this->states[i + jump + 1].real(this->states[i].real() *
												(1 / sqrt(2)));
			}
			// if state is |1>
			else {
				// |1> = -1/sqrt(2) |1>
				this->states[i + 1].real(this->states[i].real() * (-1 / sqrt(2)));
				// |1> = 1/sqrt(2) |0>
				this->states[i - jump + 1].real(this->states[i].real() * (1 / sqrt(2)));
			}
		}
		jumpCounter++;
	}
	updateStates();
}

void QubitLayer::pauliZ(int targetQubit)
{
	size_t jump = calculateJump(targetQubit);
	unsigned int jumpCounter = 0;

	for(size_t i = 0; i < this->states.size(); i += jump) {
		if(checkZeroState(i)) {
			if(jumpCounter % 2 == 0) { // if state is |0>
				this->states[i + 1] = this->states[i];
			} else { // if state is |1>
				this->states[i + 1].real(this->states[i].real() * -1);
			}
		}
		jumpCounter++;
	}

	updateStates();
}

// TODO
void QubitLayer::pauliY(int targetQubit) { return; }

void QubitLayer::pauliX(int targetQubit)
{
	// calculate jump
	size_t jump = calculateJump(targetQubit);
	unsigned int jumpCounter = 0;

	for(size_t i = 0; i < this->states.size(); i += jump) {
		// check if real value is different than 0
		if(checkZeroState(i)) {
			// check if real value is 1
			if(this->states[i].real() != 0) {
				// check if state is |0>
				if(jumpCounter % 2 == 0) {
					// state |1> has value of previous |0>
					this->states[i + jump + 1].real(this->states[i].real());
				}
				// if state is |1>
				else {
					this->states[i - jump + 1].real(this->states[i].real());
				}
			}
		}
		jumpCounter++;
	}
    
    /** problema esta aqui, pauliX tem de atualizar os estados que já têm 1. 
     * Com esta implementação, com o jump, apenas se alteram os estados do qubit que se pretende.
     * É preciso ter em conta os estados que estão a (1,0), porque com o updateStates() passam
     * a (0,0), cancelando-se. É preciso mudar a lógica de mudar os estados
     */    
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
        if (i % 2 == 1) cout << " | ";
	}
    cout << endl;

	cout << "State vector size: " << this->states.size() << endl;
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