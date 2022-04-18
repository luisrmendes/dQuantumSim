#ifndef UTILSMPI_H
#define UTILSMPI_H

#include "dynamic_bitset.h"
#include "macros.h"
#include <vector>

extern int rank;
extern int size;
extern std::vector<size_t> layerAllocs;
extern std::vector<dynamic_bitset> layerLimits;

size_t getLocalIndexFromGlobalState(dynamic_bitset receivedIndex, int node);

void instructionsHandlerMPI(std::vector<unsigned int>& instructions,
							int rank,
							int size);

void gatherResultsMPI(int rank,
					  int size,
					  unsigned int numQubits,
					  PRECISION_TYPE* finalResults);

#endif
