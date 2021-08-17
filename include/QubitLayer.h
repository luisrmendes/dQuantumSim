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
	void updateStates();
	void printStateVector();
	void measure();
	void pauliX(int targetQubit);

	/**
	 * TODO: Pauli Y
	 */
	void pauliY(int targetQubit);

	/**
	 * TODO: Pauli Z
	 */
	void pauliZ(int targetQubit);

	/**
	 * TODO: Hadamard
	 */
	void hadamard(int targetQubit);
};

#endif
