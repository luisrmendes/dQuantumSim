#ifndef UTILSMPI_HPP
#define UTILSMPI_HPP

#include "constants.hpp"
#include <array>
#include <cstdint>
#include <vector>

extern int rank;
extern int size;
extern std::vector<std::size_t> layerAllocs;
extern std::vector<uint64_t> layerLimits;

std::size_t getLocalIndexFromGlobalState(uint64_t receivedIndex, int node);

/**
 * @brief Distribute instructions read from rank 0 to all ranks.
 * Must only be used if MPI does not share environment folder across machines
 * 
 * @param instructions 
 * @return std::vector<unsigned int> 
 */
std::vector<unsigned int>
instructionsHandlerMPI(std::vector<unsigned int>& instructions);

/**
 * @brief 
 * 
 * @param numQubits 
 * @param finalResults 
 * @return std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS> 
 */
std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS>
gatherResultsMPI(unsigned int numQubits, PRECISION_TYPE* finalResults);

#endif
