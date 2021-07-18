#include "MultiQubit.h"

using namespace std;

void MultiQubit::printState()
{
	cout << "State of entangled qubits :" << endl;
	for(unsigned int i = 0; i < this->state.size(); i++) {
		cout << noshowpos << state[i].real();
		if(state[i].imag() != 0)
			cout << showpos << state[i].imag() << "i";
		cout << endl;
	}
	cout << endl;
}