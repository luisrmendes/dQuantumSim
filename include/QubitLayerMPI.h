#ifndef QUBITLAYERMPI_H
#define QUBITLAYERMPI_H

#include "constants.h"
#include <bitset>
#include <complex>
#include <vector>

typedef std::vector<std::complex<double>> qubitLayer;

class QubitLayerMPI
{
  private:
	qubitLayer states;
	int rank;
	int size;
	std::vector<unsigned int> layerAllocs;
	unsigned int globalStartIndex;
	unsigned int globalEndIndex;

  public:
	/**
	 * Initializes state vector with (0,0)
	 * @param qubitCount Number of qubits 
	 */
	QubitLayerMPI(std::vector<unsigned int> layerAllocs, int rank, int size);
	qubitLayer getStates() { return this->states; }
	void setStates(qubitLayer states) { this->states = states; }
	std::vector<unsigned int> getLayerAllocs() { return this->layerAllocs; }
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
	 * Gets the node that posesses the state
	 * @param state 
	 * @returns The node that contains the state
	 */
	int getNodeOfState(unsigned long state);

	/**
	 * Handles states Out Of Bounds for each process.
	 * Sends operation and exit messages,
	 * Receives and returns operations messages.
	 * @param statesOOB Vector of (state, intended_value) operations to send
	 * @returns Vector with (state, intended_value) received operations
	 */
	std::vector<std::complex<double>>
	handlerStatesOOB(std::vector<std::complex<double>> statesOOB);

	/**
	 * Checks if state is Out Of Bounds of the state layer
	 * vector of the process
	 * @param state bitset of the state to check
	 * @return true if state is OOB, else false
	 */
	bool checkStateOOB(std::bitset<numQubitsMPI> state);

	void measureQubits(double* result);

	unsigned int getLocalStartIndex();

	unsigned int getLocalIndexFromReceivedOp(unsigned int receivedIndex);
};

#endif
