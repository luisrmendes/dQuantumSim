#include "_utils.h"
#include "_macros.h"
#include <iostream>

using namespace std;

vector<dynamic_bitset> calculateLayerLimits(vector<size_t> layerAllocs) {
	vector<dynamic_bitset> layerLimits(layerAllocs.size());

	dynamic_bitset aux = 0;
	for (size_t i = 0; i < layerLimits.size(); i++) {
		aux += layerAllocs[i] / 2;
		layerLimits[i] = aux;
	}

	return layerLimits;
}

stringstream getFormattedQubitLayer(vector<complex<PRECISION_TYPE>> states)
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

vector<size_t> calculateLayerAlloc(int qubitCount, int nodeCount)
{
	vector<size_t> result;
	size_t layerSize = pow(2, qubitCount);

	if(layerSize / 2 < (size_t)nodeCount) {
		cerr << "Known allocation bug, too many processes for few qubits" << endl;
		exit(EXIT_FAILURE);
	}

	unsigned long long quocient = layerSize / nodeCount;
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

	// Check if layer alloc size is bigger than std_max
	for(size_t i = 0; i < result.size(); ++i){
		if (result[i] >= result.max_size()) {
			cerr << "Exceeded max_size of vector allocation at node " << i << endl;
			exit(EXIT_FAILURE); 
		}
	}

	return result;
}
