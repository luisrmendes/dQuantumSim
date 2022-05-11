#include "matrixOperations.h"
#include <array>

using namespace std;

vector<complex<double>> tensorMultiplication(vector<complex<double>> state1,
											 vector<complex<double>> state2)
{
	vector<complex<double>> result;

	for(auto it = state1.begin(); it != state1.end(); ++it) {
		for(auto it2 = state2.begin(); it2 != state2.end(); ++it2) {
			result.push_back(*it2 * *it);
		}
	}

	return result;
}

vector<complex<double>> tensorMultiplication(vector<complex<double>> state1,
											 array<complex<double>, 2> state2)
{
	vector<complex<double>> result;

	for(auto it = state1.begin(); it != state1.end(); ++it) {
		for(auto it2 = state2.begin(); it2 != state2.end(); ++it2) {
			result.push_back(*it2 * *it);
		}
	}

	return result;
}

vector<complex<double>>
matrixMultiplication(vector<complex<double>> state, complex<double>* mat, size_t dim)
{
	vector<complex<double>> result(dim, 0);

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
