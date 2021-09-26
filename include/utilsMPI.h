#ifndef UTILSMPI_H
#define UTILSMPI_H

#include <vector>

void gatherResults(int rank, int size, std::vector<double> results);

#endif