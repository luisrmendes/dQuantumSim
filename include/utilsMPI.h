#ifndef UTILSMPI_H
#define UTILSMPI_H

#include "_macros.h"
#include <array>
#include <vector>

extern int rank;
extern int size;
extern std::vector<size_t> layerAllocs;
extern std::vector<uint64_t> layerLimits;

size_t getLocalIndexFromGlobalState(uint64_t receivedIndex, int node);

void instructionsHandlerMPI(std::vector<unsigned int>& instructions,
							int rank,
							int size);

std::array<PRECISION_TYPE, MAX_NUMBER_QUBITS>
gatherResultsMPI(unsigned int numQubits, PRECISION_TYPE* finalResults);

#endif
