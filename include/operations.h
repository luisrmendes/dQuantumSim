#pragma once
#ifndef OPERATION_H
#define OPERATION_H

#include <complex>
#include <variant>
#include <vector>

/**
 * Applies the gate operation (correct name?)
 * Fixed to size 2 vector (qubit state) and 2x2 matrix (single qubit gates)
 */
std::vector<std::complex<double>>
gateOperation(std::vector<std::complex<double>> state, std::complex<double>* mat);

/**
 * Applies tensor multiplication between a qubit and a matrix
 * for gate operations 
 */
std::vector<std::complex<double>>
tensorMultiplication(std::vector<std::complex<double>> state1, double* mat);

#endif