#pragma once
#ifndef QUBITLAYER_H
#define QUBITLAYER_H

#include <bitset>
#include <complex>
#include <iostream>
#include <vector>

typedef std::vector<std::complex<float>> qubitStates;

class QubitLayer
{
  private:
	qubitStates states;
	int qubitCount;

  public:
	/**
	 * Initializes state vector with (0,0), calculates 
	 * vector size with input and output values
	 * @param qubitCount Number of qubits 
	 */
	QubitLayer(int qubitCount);
	qubitStates getStates() { return this->states; }
	int getQubitCount() { return this->qubitCount; }
	void printStates();
	void pauliX(int targetQubit);
	void updateStates();
	void measure();
};

#endif
