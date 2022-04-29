#ifndef UTILSMPI_H
#define UTILSMPI_H

#include "_macros.h"
#include <vector>

extern int rank;
extern int size;
extern std::vector<size_t> layerAllocs;
extern std::vector<uint64_t> layerLimits;

size_t getLocalIndexFromGlobalState(uint64_t receivedIndex, int node);

void instructionsHandlerMPI(std::vector<unsigned int>& instructions,
							int rank,
							int size);

void gatherResultsMPI(int rank,
					  int size,
					  unsigned int numQubits,
					  PRECISION_TYPE* finalResults);

#endif
