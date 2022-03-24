#ifndef DISTRENGINE_H
#define DISTRENGINE_H

#include "dynamic_bitset.h"
#include <complex>
#include <vector>

extern int rank;
extern int size;
extern std::vector<size_t> layerAllocs;

/**
 * Handles states Out Of Bounds for each process.
 * Sends operation and exit messages,
 * Receives and returns operations messages.
 * @param statesOOB Vector of (state, intended_value) operations to send
 * @returns Vector with (state, intended_value) received operations
 */
void sendStatesOOB(
	std::vector<std::tuple<dynamic_bitset, std::complex<double>>> statesOOB);

std::vector<std::complex<double>> receiveStatesOOB();

/**
 * Gets the node that posesses the state
 * @param state 
 * @returns The node that contains the state
 */
int getNodeOfState(dynamic_bitset state);

#endif