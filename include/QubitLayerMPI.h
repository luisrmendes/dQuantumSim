#pragma once
#ifndef QUBITLAYERMPI_H
#define QUBITLAYERMPI_H

#include "mpi.h"
#include "utils.h"
#include <bitset>
#include <complex>
#include <iostream>
#include <vector>

constexpr int numQubitsMPI = 3;

typedef std::vector<std::complex<double>> qubitLayer;

class QubitLayerMPI
{
  private:
	qubitLayer states;
	int rank;
	int size;

  public:
	/**
	 * Starts circuit with all qubits |0>, id est |000> == {1,0}
	 * Initializes state vector with (0,0)
	 * @param qubitCount Number of qubits 
	 */
	QubitLayerMPI(size_t qLayerSize, int rank, int size);
	qubitLayer getStates() { return this->states; }
	void setStates(qubitLayer states) { this->states = states; }
	void updateStates();
	void printStateVector();

	/**
	 * Displays qubit values according to processes rank
	 */
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

	int getNodeOfState(unsigned long state);
};

#endif
