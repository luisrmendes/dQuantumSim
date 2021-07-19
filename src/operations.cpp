#include "operations.h"
#include "Qubit.h"

using namespace std;

vector<std::complex<double>> gateOperation(vector<std::complex<double>> state,
										   std::complex<double>* mat,
										   size_t dim)
{
	// Dimension of the gate matrix must be equal to the size of the state
	if(dim != state.size()) {
		cout << "Qubit state size does not match gate matrix dimension" << endl;
		exit(-1);
	}

	vector<std::complex<double>> result(dim, 0);

	int j = -1;

	for(unsigned int i = 0; i < dim * dim; i++) {
		if(i % dim == 0)
			j++;
		result[j] += state[i % dim] * mat[i];
	}

	return result;

	// vector<std::complex<double>> result = {0, 0};

	// for(unsigned int i = 0; i < 4; i++) {
	// 	if(i < 2)
	// 		result[0] += mat[i] * state[i % 2];
	// 	else
	// 		result[1] += mat[i] * state[i % 2];
	// }

	// return result;
}
