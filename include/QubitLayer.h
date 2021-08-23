#pragma once
#ifndef QUBITLAYER_H
#define QUBITLAYER_H

#include <bitset>
#include <complex>
#include <iostream>
#include <vector>

constexpr int numQubits = 3;

typedef std::vector<std::complex<double>> qubitStates;

class QubitLayer
{
  private:
	qubitStates states;
	int qubitCount;

  public:
	/**
	 * Starts circuit with all qubits |0>, id est |000> == {1,0}
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

	void pauliZ(int targetQubit);

	/**
	 * TODO: Hadamard
	 */
	void hadamard(int targetQubit);

	size_t calculateJump(int targetQubit);

	/**
	 * Returns true if state has non-zero real component
	 * @param i State vector iterator position
	 */
	bool checkZeroState(int i);
};

#endif
