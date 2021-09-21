#pragma once
#ifndef QUBITLAYERMPI_H
#define QUBITLAYERMPI_H

#include "macros.h"
#include "mpi.h"
#include "utils.h"
#include <bitset>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <filesystem>

constexpr int numQubitsMPI = 4;

typedef std::vector<std::complex<double>> qubitLayer;

class QubitLayerMPI
{
  private:
	qubitLayer states;
	int rank;
	int size;
	std::stringstream debugLog;
	std::stringstream resultLog;

  public:
	/**
	 * Initializes state vector with (0,0)
	 * @param qubitCount Number of qubits 
	 */
	QubitLayerMPI(size_t qLayerSize, int rank, int size);
	qubitLayer getStates() { return this->states; }
	void setStates(qubitLayer states) { this->states = states; }
	std::stringstream& getDebugLog() { return this->debugLog; }
	std::stringstream& getResultLog() { return this->resultLog; }
	void updateStates();
	std::string getStateVector();

	void pauliX(int targetQubit);
	void pauliY(int targetQubit);
	void pauliZ(int targetQubit);
	void hadamard(int targetQubit);
	void controlledZ(int controlQubit, int targetQubit);
	void controlledX(int controlQubit, int targetQubit);
	void toffoli(int controlQubit1, int controlQubit2, int targetQubit);

	/**
	 * Prints the state layer vector with the adequate format
	 */
	void printStateVector();

	/**
	 * Appends info to the debug log of each process
	 * @param args Variable list of arguments to print
	 */
	template <typename... T>
	void appendDebugLog(const T&... args);

	/**
	 * Appends info to the result log of each process
	 * @param args Variable list of arguments to print
	 */
	template <typename... T>
	void appendResultLog(const T&... args);

	/**
	 * Displays qubit values according to processes rank by outputting the 
	 * result log of each process.
	 */
	void measure();

	/**
	 * Creates log file and directory, outputs the debug log of each process 
	 * to a log file in directory "logs" and to stdout if argument is true. 
	 * @param to_stdout if true, outputs to stdout
	 */
	void outputDebugLogs(bool to_stdout);

	/**
	 * Returns true if state has non-zero real component
	 * @param i State vector iterator position
	 */
	bool checkZeroState(int i);

	/**
	 * Gets the node that posesses the state
	 * @param state 
	 * @returns The node that contains the state
	 */
	int getNodeOfState(unsigned long state);

	/**
	 * Handles states Out Of Bounds for each process.
	 * Sends operation and exit messages,
	 * Receives and returns operations messages.
	 * @param statesOOB Vector of (state, intended_value) operations to send
	 * @returns Vector with (state, intended_value) received operations
	 */
	std::vector<std::complex<double>>
	handlerStatesOOB(std::vector<std::complex<double>> statesOOB);

	/**
	 * Checks if state is Out Of Bounds of the state layer
	 * vector of the process
	 * @param state bitset of the state to check
	 * @return true if state is OOB, else false
	 */
	bool checkStateOOB(std::bitset<numQubitsMPI> state);
};

#endif
