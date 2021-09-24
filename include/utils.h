#ifndef UTILS_H
#define UTILS_H

#include <vector>

/**
 * BUG: 3 qubits (vector size 16) +4 processes  
 *
 * Calculates the size of the quantum layer vector for each node
 * @param qubitCount Number of qubits to simulate
 * @param nodeCount Number of nodes running in the mpi environment
 * @returns Size of the layer for each node ordered by node rank
 */
std::vector<int> calculateLayerAlloc(int qubitCount, int nodeCount);

void instructionsHandlerMPI(std::vector<unsigned int>& instructions,
							int rank,
							int size);

#endif