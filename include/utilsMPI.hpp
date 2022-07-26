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

std::vector<unsigned int>
instructionsHandlerMPI(std::vector<unsigned int>& instructions);

std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS>
gatherResultsMPI(unsigned int numQubits, PRECISION_TYPE* finalResults);

#endif
