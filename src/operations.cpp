#include "operations.h"
#include "Qubit.h"

using namespace std;

vector<std::complex<double>> gateOperation(vector<std::complex<double>> state,
										   std::complex<double>* mat)
{
	vector<std::complex<double>> result = {0, 0};

	for(unsigned int i = 0; i < 4; i++) {
		if(i < 2)
			result[0] += mat[i] * state[i % 2];
		else
			result[1] += mat[i] * state[i % 2];
	}

	return result;
}


