#ifndef DISTRENGINE_H
#define DISTRENGINE_H

#include "_macros.h"
#include <complex>
#include <vector>

extern int rank;
extern int size;
extern std::vector<size_t> layerAllocs;
extern std::vector<uint64_t> layerLimits;

/**
 * Handles states Out Of Bounds for each process.
 * Sends operation and exit messages,
 * Receives and returns operations messages.
 * @param statesOOB Vector of (state, intended_value) operations to send
 * @returns Vector with (state, intended_value) received operations
 */
void sendStatesOOB(
	std::vector<std::tuple<uint64_t, std::complex<PRECISION_TYPE>>> statesOOB);

std::vector<std::complex<PRECISION_TYPE>> receiveStatesOOB();

/**
 * Gets the node that posesses the state
 * @param state 
 * @returns The node that contains the state
 */
int getNodeOfState(uint64_t state);

/**
 * WARN: 3 qubits (vector size 16) +4 processes  
 *
 * Calculates the size of the quantum layer vector for each node
 * @param qubitCount Number of qubits to simulate
 * @param nodeCount Number of nodes running in the mpi environment
 * @returns Size of the layer for each node ordered by node rank
 */
std::vector<size_t> calculateLayerAlloc(int qubitCount, int nodeCount);

/**
 * Sends statesOOB and returns received statesOOB
 */
std::vector<std::complex<PRECISION_TYPE>> distributeAndGatherStatesOOB(
	std::vector<std::tuple<uint64_t, std::complex<PRECISION_TYPE>>>
		statesAndAmplitudesOOB);

#endif