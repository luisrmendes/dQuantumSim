#include "operations.h"
#include "Qubit.h"

using namespace std;

vector<std::complex<double>> matrixMultiplication(vector<std::complex<double>> state,
												  std::complex<double>* mat,
												  size_t dim)
{
	vector<std::complex<double>> result(dim, 0);

	for(unsigned int j = 0; j < dim; j++) {
		for(unsigned int i = 0; i < dim; i++) {
			result[j] += state[i] * mat[j * dim + i];
		}
	}

	// int j = -1;
	// for(unsigned int i = 0; i < dim * dim; i++) {
	// 	if(i % dim == 0)
	// 		j++;
	// 	result[j] += state[i % dim] * mat[i];
	// }

	return result;
}
