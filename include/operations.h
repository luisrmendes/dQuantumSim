#pragma once
#ifndef OPERATION_H
#	define OPERATION_H

#include <vector>
#include <variant>
#include <complex>

std::vector<std::complex<double>> gateOperation(std::vector<std::complex<double>> state, std::complex<double>* mat);

/**
 * Applies tensor multiplication between a qubit and a matrix
 * for gate operations 
 */
std::vector<double> tensorMultiplication(std::vector<double> state1, double* mat);

/**
 * Applies tensor multiplication between two qubits
 */
std::vector<double> tensorMultiplication(std::vector<double> state1, std::vector<double> state2);


#endif