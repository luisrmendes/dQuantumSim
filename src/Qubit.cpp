#include "Qubit.h"
#include <iostream>

using namespace std;

Qubit::Qubit(int id) { this->id = id; }

void Qubit::printState()
{
	for(unsigned int i=0; i<this->state.size(); i++) {
		cout << state[i] << endl;
	}
	cout << endl;
}