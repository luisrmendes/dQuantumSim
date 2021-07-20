#pragma once
#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <complex>
#include <variant>
#include <vector>

/**
 * Applies the matrix multiplication
 * @param state The state of a qubit
 * @param mat The matrix of the single qubit gate operation (Pauli-X, Pauli-Y etc)
 * @param dim The Dimension of mat (dim*dim)
 */
std::vector<std::complex<double>> matrixMultiplication(
	std::vector<std::complex<double>> state, std::complex<double>* mat, size_t dim);

#endif