#pragma once
#ifndef MATRIX_H
#define MATRIX_H

#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>

void generateRandomMatrix(
	double* mat, std::size_t dim, int minvalue, int maxvalue, int seed);

void printMatrix(double* mat, std::size_t dim);

void resetMatrix(double* A, std::size_t dim);

#endif
