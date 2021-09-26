#ifndef UTILSMPI_H
#define UTILSMPI_H

#include <vector>

void instructionsHandlerMPI(std::vector<unsigned int>& instructions,
							int rank,
							int size);

void gatherResults(int rank, int size, double* finalResults);

#endif