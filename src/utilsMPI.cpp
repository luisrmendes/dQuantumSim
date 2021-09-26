#include "utilsMPI.h"
#include "constants.h"
#include "mpi.h"
#include <iostream>

using namespace std;

void gatherResults(int rank,
				   int size,
				   std::vector<double> resultsVec,
				   double* finalResults)
{
	MPI_Status status;
	size_t resultsSize = numQubitsMPI * 2;
	double results[resultsSize];

	if(rank == 0) {
		double receivedResults[resultsSize];

		// popular o vetor com os indices dos qubits
		for(unsigned int i = 0; i < resultsSize; i += 2) {
			finalResults[i] = (i / 2) + 1;
			finalResults[i + 1] = resultsVec[i + 1];
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
				finalResults[i + 1] += receivedResults[i + 1];
			}
		}
		return;
	} else {
		copy(resultsVec.begin(), resultsVec.end(), results);
		MPI_Send(results, resultsSize, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		return;
	}
}