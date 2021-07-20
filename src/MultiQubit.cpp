#include "MultiQubit.h"

using namespace std;

void MultiQubit::controlledNot()
{
	complex<double> mat[] {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 1,
		0, 0, 1, 0
	};

	this->state = matrixMultiplication(this->state, mat, 4);
}

MultiQubit MultiQubit::tensorMultiplication(Qubit q1, Qubit q2)
{
	vector<complex<double>> result;

	vector<complex<double>> state1 = q1.getState();
	vector<complex<double>> state2 = q2.getState();

	for(auto it = state1.begin(); it != state1.end(); ++it) {
		for(auto it2 = state2.begin(); it2 != state2.end(); ++it2) {
			result.push_back(*it2 * *it);
		}
	}

	MultiQubit entangledQubit(result);
	return entangledQubit;
}

void MultiQubit::printState()
{
	cout << "State of entangled qubits:" << endl;
	for(unsigned int i = 0; i < this->state.size(); i++) {
		cout << noshowpos << state[i].real();
		if(state[i].imag() != 0)
			cout << showpos << state[i].imag() << "i";
		cout << endl;
	}
	cout << endl;
}