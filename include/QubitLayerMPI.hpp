#ifndef QUBITLAYERMPI_HPP
#define QUBITLAYERMPI_HPP

#include "constants.hpp"
#include <complex>
#include <functional>
#include <vector>

typedef std::vector<std::complex<PRECISION_TYPE>> qubitLayer;

class QubitLayerMPI
{
  private:
	qubitLayer states;
	unsigned int numQubits;
	int rank;
	int size;
	std::vector<size_t> layerLimits;

	/**
	 * @brief Returns a vector with the size of state vector segment allocation
	 * per each process represented in the index. Takes in account the input and
	 * output states.
	 * 
	 * @return std::vector<size_t> 
	 */
	std::vector<size_t> calculateLayerAlloc();

	std::vector<size_t> calculateLayerLimits(std::vector<size_t> layerAllocs);

	size_t getLocalIndexFromGlobalState(uint64_t receivedIndex, int node);
	/**
	 * Sends statesOOB and returns received statesOOB
	 */
	std::vector<std::complex<PRECISION_TYPE>> distributeAndGatherStatesOOB(
		std::vector<std::tuple<uint64_t, std::complex<PRECISION_TYPE>>>&
			statesAndAmplitudesOOB);

	/**
	 * Gets the node that posesses the state
	 * @param state 
	 * @returns The node that contains the state
	 */
	int getNodeOfState(const uint64_t& state);

  public:
	uint64_t globalLowerBound;
	uint64_t globalUpperBound;

	/**
	 * Initializes state vector with (0,0)
	 * @param numQubits Number of qubits 
	 */
	QubitLayerMPI(unsigned int numQubits, int rank, int size);
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

	/**
	 * @brief 
	 * 
	 * @param numQubits 
	 * @param finalResults 
	 * @return std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS> 
	 */
	std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS>
	gatherResults(PRECISION_TYPE* finalResults);
};

#endif
