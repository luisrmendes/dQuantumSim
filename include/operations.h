#pragma once
#ifndef OPERATION_H
#define OPERATION_H

#include <complex>
#include <variant>
#include <vector>

std::vector<std::complex<double>>
gateOperation(std::vector<std::complex<double>> state, std::complex<double>* mat);

/**
 * Applies tensor multiplication between a qubit and a matrix
 * for gate operations 
 */
std::vector<double> tensorMultiplication(std::vector<double> state1, double* mat);

/**
 * Applies tensor multiplication between two qubits
 * Only applies to size 2 vectors (qubit state)
 */
std::vector<std::complex<double>>
tensorMultiplication(std::vector<std::complex<double>> state1,
					 std::vector<std::complex<double>> state2);

#endif