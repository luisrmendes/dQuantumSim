#include "matrix.h"

void generateRandomMatrix(double* mat, std::size_t dim, int minvalue, int maxvalue, int seed) {

	// random_device rd;
	// mt19937 rng(rd());
	std::mt19937 rng(seed);
	std::uniform_int_distribution<> distr(minvalue, maxvalue);

	std::size_t i;

	for (i = 0; i < dim * dim; i++) {
		mat[i] = distr(rng);
	}
}

void printMatrix(double* mat, std::size_t dim) {
	std::cout << std::left << std::setw(9) << std::setprecision(3);
	std::size_t i, j;
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++)
			std::cout << std::left << std::setw(9) << mat[i * dim + j] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void resetMatrix(double *A, std::size_t dim) {
	memset(A, 0, dim * dim * sizeof(double));
}