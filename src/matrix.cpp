#include "matrix.h"

using namespace std;

void generateRandomMatrix(
	double* mat, size_t dim, int minvalue, int maxvalue, int seed)
{
	// random_device rd;
	// mt19937 rng(rd());
	mt19937 rng(seed);
	uniform_int_distribution<> distr(minvalue, maxvalue);

	size_t i;

	for(i = 0; i < dim * dim; i++) {
		mat[i] = distr(rng);
	}
}

void printMatrix(double* mat, size_t dim)
{
	cout << left << setw(9) << setprecision(3);
	size_t i, j;
	for(i = 0; i < dim; i++) {
		for(j = 0; j < dim; j++)
			cout << left << setw(9) << mat[i * dim + j] << " ";
		cout << endl;
	}
	cout << endl;
}

void resetMatrix(double* A, size_t dim)
{
	memset(A, 0, dim * dim * sizeof(double));
}