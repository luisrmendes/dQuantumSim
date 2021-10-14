#ifndef QUBITLAYERMPI_H
#define QUBITLAYERMPI_H

#include "constants.h"
#include <complex>
#include <vector>

extern int rank;
extern int size;
extern std::vector<unsigned long long> layerAllocs;

typedef std::vector<std::complex<double>> qubitLayer;

class QubitLayerMPI
{
  private:
	qubitLayer states;
	unsigned int numQubits;
	unsigned long long globalStartIndex;
	unsigned long long globalEndIndex;

  public:
	/**
	 * Initializes state vector with (0,0)
	 * @param numQubits Number of qubits 
	 */
	QubitLayerMPI(unsigned int numQubits);
	qubitLayer getStates() { return this->states; }
	void setStates(qubitLayer states) { this->states = states; }
	void updateStates();
	std::string getStateVector();

	void pauliX(int targetQubit);
	void pauliY(int targetQubit);
	void pauliZ(int targetQubit);
	void hadamard(int targetQubit);
	void controlledZ(int controlQubit, int targetQubit);
	void controlledX(int controlQubit, int targetQubit);
	void toffoli(int controlQubit1, int controlQubit2, int targetQubit);

	/**
	 * Prints the state layer vector with the adequate format
	 */
	void printStateVector();

	/**
	 * Displays qubit values according to processes rank by outputting the 
	 * result log of each process.
	 */
	void measure();

	/**
	 * Returns true if state has non-zero real component
	 * @param i State vector iterator position
	 */
	bool checkZeroState(int i);

	/**
	 * Checks if state is Out Of Bounds of the state layer
	 * vector of the process
	 * @param state bitset of the state to check
	 * @return true if state is OOB, else false
	 */
	bool checkStateOOB(unsigned long long state);

	void measureQubits(double* result);

	unsigned long long getLocalStartIndex();

	unsigned long long
	getLocalIndexFromGlobalState(unsigned long long receivedIndex);
};

#endif
