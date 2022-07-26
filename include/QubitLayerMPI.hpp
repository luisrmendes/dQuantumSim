#ifndef QUBITLAYERMPI_HPP
#define QUBITLAYERMPI_HPP

#include "constants.hpp"
#include <complex>
#include <functional>
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

  public:
	uint64_t globalLowerBound;
	uint64_t globalUpperBound;
	/**
	 * Initializes state vector with (0,0)
	 * @param numQubits Number of qubits 
	 */
	QubitLayerMPI(unsigned int numQubits);
	qubitLayer& getStates() { return this->states; }
	uint64_t getGlobalStartIndex() { return this->globalLowerBound; }
	uint64_t getGlobalEndIndex() { return this->globalUpperBound; }

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
	void rotationX(int targetQubit, double angle);
	void sqrtPauliX(int targetQubit);
	void sqrtPauliY(int targetQubit);
	void sGate(int targetQubit);
	void tGate(int targetQubit);


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

	void measureQubits(PRECISION_TYPE* result);

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

	void manageDistr(
		std::vector<std::tuple<uint64_t, std::complex<PRECISION_TYPE>>>& statesOOB,
		const std::function<void(std::vector<std::complex<PRECISION_TYPE>>)>&
			operationFunc);
};

#endif
