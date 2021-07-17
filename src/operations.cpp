#include "operations.h"
#include "Qubit.h"

using namespace std;

vector<double> gateOperation(vector<double> state, std::variant<double, std::complex<double>>* mat)
{
    vector<double> result = {0, 0};

    struct VisitMatrix {
        void operator()(double& mat, double state, double& result) { result = result + (mat * state); }
        void operator()(std::complex<double>& mat, double state, std::complex<double>& result) { result = result + (mat * state); }
    };

    for (unsigned int i = 0; i < 4; i++) {
        if (i<2) 
            visit(VisitMatrix(), mat[i], state[i % 2], &result[0]);
        else
            visit(VisitMatrix(), mat[i], state[i % 2], &result[1]);
    }

    return result;
}

vector<double> gateOperation(vector<double> state, double* mat)
{
    vector<double> result = {0, 0};

    for (unsigned int i = 0; i < 4; i++) {
        if (i<2) 
            result[0] += mat[i] * state[i % 2];
        else
            result[1] += mat[i] * state[i % 2];
    }

    return result;
}

vector<double> tensorMultiplication(vector<double> state1, vector<double> state2)
{
    vector<double> result;

    for (auto it = state2.begin(); it != state2.end(); ++it) {
        for (auto it2 = state1.begin(); it2 != state1.end(); ++it2) {
            result.push_back(*it2 * *it); 
        }
    }

    return result;
}
