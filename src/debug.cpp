#include "debug.h"
#include "mpi.h"
#include <filesystem>
#include <fstream>

using namespace std;

stringstream debugLog;
stringstream resultLog;

void outputResult(int rank, int size)
{
	// Print logs orderly
	for(int i = 0; i < size; i++) {
		if(rank == i)
			cout << resultLog.str() << endl;
		else
			MPI_Barrier(MPI_COMM_WORLD);
	}
}

void outputDebugLogs(int rank, int size, bool to_stdout)
{
	// remove all previous logs
	if(rank == 1) {
		filesystem::remove_all("logs");
		filesystem::create_directory("logs");
	}
	MPI_Barrier(MPI_COMM_WORLD);

	// create global log file
	string globalLogFileName = "logs/globalLog.txt";
	fstream globalLogFile;

	// create log file with name according to rank
	string nodeFileName = "logs/log";
	nodeFileName.append(to_string(rank));
	nodeFileName.append(".txt");
	fstream nodeLogFile;

	// Write to each log file
	nodeLogFile.open(nodeFileName, ios::app);

	if(!nodeLogFile) {
		cout << "file does not exist" << endl;
	} else {
		// Print logs orderly
		for(int i = 0; i < size; i++) {
			if(rank == i)
				nodeLogFile << debugLog.str() << "\n";
			else
				MPI_Barrier(MPI_COMM_WORLD);
		}
		nodeLogFile.close();
	}

	// Write to global log file
	globalLogFile.open(globalLogFileName, ios::app);

	if(!globalLogFile) {
		cout << "file does not exist" << endl;
	} else {
		globalLogFile << debugLog.str() << "\n";
		globalLogFile.close();
	}

	if(to_stdout) {
		// Print logs orderly
		for(int i = 0; i < size; i++) {
			if(rank == i)
				cout << debugLog.str() << endl;
			else
				MPI_Barrier(MPI_COMM_WORLD);
		}
	}
}
