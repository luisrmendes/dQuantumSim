#ifndef _UTILS_HPP
#define _UTILS_HPP

#include "constants.hpp"
#include <cstdint>
#include <vector>

/**
 * WARN: 3 qubits (vector size 16) +4 processes  
 *
 * Calculates the size of the quantum layer vector for each node
 * @param qubitCount Number of qubits to simulate
 * @param nodeCount Number of nodes running in the mpi environment
 * @returns Size of the layer for each node ordered by node rank
 */
std::vector<std::size_t> calculateLayerAlloc(int qubitCount, int nodeCount);

/**
 * Returns a vector with the start global index for each node/process.
 * The return vector is offset by 1 regarding the rank, i.e. first value
 *  of vector has the start global index of the rank 1 (the second rank).
 * @param layerAllocs Vector containing the vector layer size for each node
 * @returns Vector of dynamic_bitsets
 */
std::vector<uint64_t> calculateLayerLimits(std::vector<std::size_t> layerAllocs);

#endif
