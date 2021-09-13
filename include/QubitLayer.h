#pragma once
#ifndef QUBITLAYER_H
#define QUBITLAYER_H

#include <bitset>
#include <complex>
#include <iostream>
#include <vector>

constexpr int numQubits = 3;

typedef std::vector<std::complex<double>> qubitLayer;

class QubitLayer
{
  private:
	qubitLayer states;
	int qubitCount;

  public:
	/**
	 * Starts circuit with all qubits |0>, id est |000> == {1,0}
	 * Initializes state vector with (0,0)
	 * @param qubitCount Number of qubits 
	 */
	QubitLayer(size_t qLayerSize);
	qubitLayer getStates() { return this->states; }
	void setStates(qubitLayer states) { this->states = states; }
	int getQubitCount() { return this->qubitCount; }
	void updateStates();
	void printStateVector();
	void measure();

	/**
	 * Returns true if state has non-zero real component
	 * @param i State vector iterator position
	 */
	bool checkZeroState(int i);
	void pauliX(int targetQubit);
	void pauliY(int targetQubit);
	void pauliZ(int targetQubit);
	void hadamard(int targetQubit);

	/**
	 * Executes pauliZ if control qubit is |1>
	 */
	void controlledZ(int controlQubit, int targetQubit);

	/**
	 * Executes pauliX if control qubit is |1>
	 */
	void controlledX(int controlQubit, int targetQubit);

	/**
	 * Executes pauliX if both control qubits are set to |1>
	 */
	void toffoli(int controlQubit1, int controlQubit2, int targetQubit);
};

#endif