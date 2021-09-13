#pragma once
#ifndef STATEACCESS_H
#define STATEACCESS_H

#include <bitset>
#include <complex>
#include <iostream>
#include <vector>

typedef std::vector<std::complex<double>> qubitStates;

class StateAccess
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
	StateAccess(int qubitCount);
	qubitStates getStates() { return this->states; }
	int getQubitCount() { return this->qubitCount; }
	void printStateVector();
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

	size_t calculateJump(int targetQubit);

	/**
	 * Returns true if state has non-zero real component
	 * @param i State vector iterator position
	 */
	bool checkZeroState(int i);
};

#endif
