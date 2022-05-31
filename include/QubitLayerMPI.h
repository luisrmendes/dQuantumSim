#ifndef QUBITLAYERMPI_H
#define QUBITLAYERMPI_H

#include "_macros.h"
#include <complex>
#include <vector>

extern int rank;
extern int size;
extern std::vector<size_t> layerAllocs;

typedef std::vector<std::complex<PRECISION_TYPE>> qubitLayer;

class QubitLayerMPI
{
  private:
	qubitLayer states;
	unsigned int numQubits;
	uint64_t globalStartIndex;
	uint64_t globalEndIndex;

  public:
	/**
	 * Initializes state vector with (0,0)
	 * @param numQubits Number of qubits 
	 */
	QubitLayerMPI(unsigned int numQubits);
	qubitLayer& getStates() { return this->states; }
	uint64_t getGlobalStartIndex() { return this->globalStartIndex; }
	uint64_t getGlobalEndIndex() { return this->globalEndIndex; }

	void setStates(qubitLayer states) { this->states = states; }
	void clearStates()
	{
		this->states.clear();
		this->states.shrink_to_fit();
	}
	void updateStates();

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
	std::string printStateVector();

	/**
	 * Displays qubit values according to processes rank by outputting the 
	 * result log of each process.
	 */
	void measure();

	/**
	 * Returns true if state has non-zero real component
	 * @param i State vector iterator position
	 */
	bool checkZeroState(size_t i);

	void measureQubits(std::vector<uint64_t> layerLimits,
					   PRECISION_TYPE* result);

	/**
	 * Checks if state is Out Of Bounds of the state layer
	 * vector of the process
	 * @param state bitset of the state to check
	 * @return true if state is OOB, else false
	*/
	bool checkStateOOB(uint64_t state);

	/**
	 * Calculates the square of each state amplitude, 
	 * creates a new vector with the results.
	 * 
	 * WARN: In this instance, program has two big vectors, may cause 
	 * segfault when using too much processes per node
	 * 
	 * TODO: How to avoid the spike in memory
	 */
	void calculateStateProbabilities();
};

#endif
