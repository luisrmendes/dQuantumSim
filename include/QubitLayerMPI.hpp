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

	/**
	 * @brief Returns a vector with upper bound limits on the state vector segment
	 * of each process.
	 * 
	 * @param layerAllocs 
	 * @return std::vector<size_t> 
	 */
	std::vector<size_t> calculateLayerLimits(std::vector<size_t> layerAllocs);

	/**
	 * @brief Returns the local index of the state indexed in the global state vector context.
	 * 
	 * @param globalState 
	 * @param targetProcess 
	 * @return size_t 
	 */
	size_t getLocalIndexFromGlobalState(uint64_t globalState, int targetProcess);

	/**
	 * @brief Distributes out-of-bound states and gathers the incoming 
	 * out-of-bound states. This contains the main distribution mechanism 
	 * for all operations in the state vector.
	 * 
	 * @param statesAndAmplitudesOOB 
	 * @return std::vector<std::complex<PRECISION_TYPE>> 
	 */
	std::vector<std::complex<PRECISION_TYPE>> distributeAndGatherStatesOOB(
		std::vector<std::tuple<uint64_t, std::complex<PRECISION_TYPE>>>&
			statesAndAmplitudesOOB);

	/**
	 * @brief Gets the node that contains the state
	 * 
	 * @param state 
	 * @return int
	 */
	int getNodeOfState(const uint64_t& state);

  public:
	uint64_t globalLowerBound;
	uint64_t globalUpperBound;

	/**
	 * @brief Construct a qubit layer object. 
	 * Initializes state vector with (0,0).
	 * Calculates layer allocations size per process.
	 * Calculates layer limits and global upper and lower bounds.
	 * 
	 * @param numQubits 
	 * @param rank MPI rank
	 * @param size MPI size
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
	 * @brief Prints the state layer vector with the adequate format
	 */
	std::string printStateVector();

	/**
	 * @brief Displays qubit values according to processes rank by outputting the 
	 * result log of each process.
	 */
	void measure();

	/**
	 * @brief Returns true if state has non-zero real component.
	 * 
	 * @param i State vector iterator position
	 */
	bool checkZeroState(size_t i);

	/**
	 * @brief Measures qubit probabilities of state vector segment.
	 * Must receive an array from which the return values are placed.
	 * C-style array is used because MPI.
	 * 
	 * @param result 
	 */
	void measureQubits(PRECISION_TYPE* result);

	/**
	 * @brief Checks if state is Out Of Bounds of the state layer
	 * vector of the process.
	 * 
	 * @param state bitset of the state to check
	 * @return true if state is OOB, else false
	*/
	bool checkStateOOB(uint64_t state);

	/**
	 * @brief Calculates the square of each state amplitude, 
	 * creates a new vector with the results.
	 * 
	 * WARN: In this instance, program has two big vectors, may cause 
	 * segfault when using too much processes per node
	 * 
	 * TODO: How to avoid the spike in memory
	 */
	void calculateStateProbabilities();

	/**
	 * @brief Small wrapper for operation distribution.
	 * 
	 * TODO: Replace with the common code between operations
	 * 
	 * @param statesOOB 
	 * @param operationFunc 
	 */
	void manageDistr(
		std::vector<std::tuple<uint64_t, std::complex<PRECISION_TYPE>>>& statesOOB,
		const std::function<void(std::vector<std::complex<PRECISION_TYPE>>)>&
			operationFunc);

	/**
	 * @brief Gathers the qubit results from all processes. Receives an array 
	 * of qubitProbabilities of the current process - this is only used by the 
	 * rank 0 process that acts as a root for the gather process.
	 * 
	 * @param qubitProbabilities Array of qubit probabilities of the own process
	 * state vector segment
	 * @return std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS> 
	 */
	std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS>
	gatherResults(PRECISION_TYPE* qubitProbabilities);
};

#endif
