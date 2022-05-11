#pragma once
#ifndef MATRIXOPERATIONS_H
#define MATRIXOPERATIONS_H

#include <complex>
#include <variant>
#include <vector>
#include <array>

std::vector<std::complex<double>>
tensorMultiplication(std::vector<std::complex<double>> state1,
					 std::vector<std::complex<double>> state2);

std::vector<std::complex<double>>
tensorMultiplication(std::vector<std::complex<double>> state1,
					 std::array<std::complex<double>, 2> state2);

/**
 * Applies the matrix multiplication
 * @param state The state of a qubit
 * @param mat The matrix of the single qubit gate operation (Pauli-X, Pauli-Y etc)
 * @param dim The Dimension of mat (dim*dim)
 */
std::vector<std::complex<double>> matrixMultiplication(
	std::vector<std::complex<double>> state, std::complex<double>* mat, size_t dim);

#endif