#ifndef UTILSMPI_H
#define UTILSMPI_H

#include "dynamic_bitset.h"
#include <vector>

extern int rank;
extern int size;
extern std::vector<unsigned long long> layerAllocs;

dynamic_bitset getLocalStartIndex();

size_t getLocalIndexFromGlobalState(dynamic_bitset receivedIndex, int node);

void instructionsHandlerMPI(std::vector<unsigned int>& instructions,
							int rank,
							int size);

void gatherResultsMPI(int rank,
					  int size,
					  unsigned int numQubits,
					  double* finalResults);

#endif
