#pragma once
#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <complex>
#include <variant>
#include <vector>

/**
 * Applies the gate operation (correct name?)
 * Fixed to size 2 vector (qubit state) and 2x2 matrix (single qubit gates)
 * @param state The state of a qubit
 * @param mat The matrix of the single qubit gate operation (Pauli-X, Pauli-Y etc)
 */
std::vector<std::complex<double>>
gateOperation(std::vector<std::complex<double>> state, std::complex<double>* mat);

#endif