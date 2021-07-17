#include "operations.h"
#include "Qubit.h"

vector<double> tensorMultiplication(vector<double> state1, vector<double> state2) {
    vector<double> result;

    for (vector<double>::iterator it = state2.begin(); it != state2.end(); ++it) {
        for (vector<double>::iterator it2 = state1.begin(); it2 != state1.end(); ++it2) {
            result.push_back(*it2 * *it); 
        }
    }

    return result;
}

