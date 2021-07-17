#pragma once
#ifndef OPERATION_H
#	define OPERATION_H

#include <vector>

using namespace std;

/**
 * Applies tensor multiplication in two qubits
 */
vector<double> tensorMultiplication(vector<double> state1, vector<double> state2);

/**
 * Applies tensor multiplication between a qubit and a matrix
 */
vector<double> tensorMultiplication(vector<double> state1, double* state2);

#endif