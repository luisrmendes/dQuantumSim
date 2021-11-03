#ifndef UTILSMPI_H
#define UTILSMPI_H

#include "dynamic_bitset.h"
#include <vector>

extern int rank;
extern int size;
extern std::vector<unsigned long long> layerAllocs;

unsigned long long getLocalStartIndex();

unsigned long long getLocalIndexFromGlobalState(dynamic_bitset receivedIndex);

void instructionsHandlerMPI(std::vector<unsigned int>& instructions,
							int rank,
							int size);

void gatherResultsMPI(int rank,
					  int size,
					  unsigned int numQubits,
					  double* finalResults);

#endif
