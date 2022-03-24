#ifndef _UTILS_H
#define _UTILS_H

#include <complex>
#include <sstream>
#include <vector>

std::stringstream getFormattedQubitLayer(std::vector<std::complex<double>> states);

/**
 * WARN: 3 qubits (vector size 16) +4 processes  
 *
 * Calculates the size of the quantum layer vector for each node
 * @param qubitCount Number of qubits to simulate
 * @param nodeCount Number of nodes running in the mpi environment
 * @returns Size of the layer for each node ordered by node rank
 */
std::vector<size_t> calculateLayerAlloc(int qubitCount, int nodeCount);

#endif
