#ifndef DISTRENGINE_H
#define DISTRENGINE_H

#include <complex>
#include <vector>

extern int rank;
extern int size;
extern std::vector<unsigned long long> layerAllocs;

/**
 * Handles states Out Of Bounds for each process.
 * Sends operation and exit messages,
 * Receives and returns operations messages.
 * @param statesOOB Vector of (state, intended_value) operations to send
 * @returns Vector with (state, intended_value) received operations
 */
void sendStatesOOB(std::vector<std::complex<double>> statesOOB);

/**
 * Gets the node that posesses the state
 * @param state 
 * @returns The node that contains the state
 */
int getNodeOfState(unsigned long long state);

#endif