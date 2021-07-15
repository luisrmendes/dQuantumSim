#include "Qubit.h"
#include <iostream>

Qubit::Qubit(int id) { this->id = id; }

void Qubit::printState()
{
    std::cout << "Qubit " << this->id << " state: " << std::endl;
	for(unsigned int i = 0; i < 1; i++) {
		for(unsigned int j = 0; j < 2; j++) {
			std::cout << state[i][j] << std::endl;
		}
	}
}