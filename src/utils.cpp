#include "utils.h"
#include "constants.h"
#include "mpi.h"
#include <cmath>
#include <complex>
#include <iostream>
#include <vector>

using namespace std;

stringstream getFormattedQubitLayer(vector<complex<double>> states)
{
	stringstream result;
	for(size_t i = 0; i < states.size(); i++) {
		result << states[i];

		if(i % 2 == 1)
			result << " | ";
	}
	result << "\n\n";

	return result;
}

vector<unsigned int> calculateLayerAlloc(int qubitCount, int nodeCount)
{
	vector<unsigned int> result;
	size_t layerSize = pow(2, qubitCount);

	if(layerSize / 2 < (size_t)nodeCount) {
		cerr << "Known allocation bug, too many processes for few qubits" << endl;
		exit(-1);
	}

	int quocient = layerSize / nodeCount;
	int remainder = layerSize % nodeCount;

	for(int i = 0; i < nodeCount; ++i) {
		result.push_back(quocient);
	}

	// Spreads the remainder through the vector
	int i = 0;
	while(remainder != 0) {
		result[i] += 1;
		--remainder;
		++i;
	}

	// Duplicate the size of each number
	for(size_t i = 0; i < result.size(); ++i) {
		result[i] *= 2;
	}

	return result;
}
