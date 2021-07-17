#pragma once
#ifndef OPERATION_H
#	define OPERATION_H

#include <vector>

using namespace std;

vector<double> pauli_X(vector<double> state);

vector<double> gateOperation(vector<double> state, double* mat);

/**
 * Applies tensor multiplication between a qubit and a matrix
 * for gate operations 
 */
vector<double> tensorMultiplication(vector<double> state1, double* mat);

/**
 * Applies tensor multiplication between two qubits
 */
vector<double> tensorMultiplication(vector<double> state1, vector<double> state2);


#endif