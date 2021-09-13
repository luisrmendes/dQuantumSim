#pragma once
#ifndef QUBITLAYERMPI_H
#define QUBITLAYERMPI_H

#include <bitset>
#include <complex>
#include <iostream>
#include <vector>
#include "utils.h"

constexpr int numQubits = 3;

typedef std::vector<std::complex<double>> qubitLayer;

class QubitLayerMPI
{
  private:
	qubitLayer states;
    int rank;

  public:
	/**
	 * Starts circuit with all qubits |0>, id est |000> == {1,0}
	 * Initializes state vector with (0,0)
	 * @param qubitCount Number of qubits 
	 */
	QubitLayerMPI(size_t qLayerSize, int rank);
	qubitLayer getStates() { return this->states; }
	void setStates(qubitLayer states) { this->states = states; }
	void updateStates();
	void printStateVector();
	
	/**
	 * Displays qubit values on a single process not using MPI
	 */
	void measure();

	/**
	 * Returns true if state has non-zero real component
	 * @param i State vector iterator position
	 */
	bool checkZeroState(int i);
	void pauliX(int targetQubit, int rank);
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

	/**
	 * Displays qubit values according to processes rank
	 */
	void measure(int rank);
};

#endif
