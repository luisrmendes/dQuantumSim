#include "utilsMPI.h"
#include "constants.h"
#include "mpi.h"
#include <iostream>

using namespace std;

void gatherResults(int rank, int size, std::vector<double> resultsVec)
{
	MPI_Status status;
	size_t resultsSize = numQubitsMPI * 2;
	double results[resultsSize];

	if(rank == 0) {
		double receivedResults[resultsSize];

		// popular o vetor com os indices dos qubits
		for(unsigned int i = 0; i < resultsSize; i += 2) {
			results[i] = (i / 2) + 1;
			results[i + 1] = resultsVec[i + 1];
		}

		for(int node = 1; node < size; node++) {
			MPI_Recv(&receivedResults,
					 resultsSize,
					 MPI_DOUBLE,
					 node,
					 0,
					 MPI_COMM_WORLD,
					 &status);

			for(size_t i = 0; i < resultsSize; i += 2) {
				results[i + 1] += receivedResults[i + 1];
			}
		}
	} else {
		copy(resultsVec.begin(), resultsVec.end(), results);
		MPI_Send(results, resultsSize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	if(rank == 0) {
		cout << "Results: " << endl;
		for(size_t i = 0; i < resultsSize; i += 2) {
			cout << "Qubit " << i / 2 << " -> " << results[i + 1] << endl;
		}
		cout << endl;
	}
}