#include "utils.h"

using namespace std;

vector<int> calculateLayerAlloc(int qubitCount, int nodeCount)
{
	vector<int> result;
	size_t layerSize = pow(2, qubitCount);

	int quocient = layerSize / nodeCount;
	int remainder = layerSize % nodeCount;

	for(size_t i = 0; i < qubitCount; ++i) {
		result.push_back(quocient);
	}

    // Spreads the remainder through the vector
	if(remainder != 0) {
		for(size_t i = 0; i < result.size(); ++i) {
			result[i] += 1;
			remainder--;
		}
	}

    // Duplicate the size of each number
    for(size_t i = 0; i < result.size(); ++i) {
		result[i] *= 2;
	} 

	return result;
}