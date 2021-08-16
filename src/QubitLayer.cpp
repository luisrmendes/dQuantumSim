#include "QubitLayer.h"

using namespace std;

QubitLayer::QubitLayer(int qubitCount)
{
	// populate vector with all (0,0)
	int i = 0;
	while(i < qubitCount) {
		this->state.push_back(0);
        ++i;
	}
}

void QubitLayer::printStates()
{
	// populate vector with all (0,0)
	for(auto it = this->state.begin(); it != this->state.end(); ++it) {
		cout << *it << endl;
	}
}