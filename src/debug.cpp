#include "debug.h"
#include <filesystem>

using namespace std;

string globalLogFileName = "logs/globalLog.txt";
string nodeFileName;
fstream globalLogFile;
fstream nodeLogFile;
stringstream debugLog;
stringstream resultLog;

void openAndCleanDebugFiles(int rank, int size)
{
	/**
	 * remove all previous logs 
	 * TODO: pode dar problemas quando correr em maquinas
	 * diferentes sem ser processos na mesma 
	 */
	if(rank == 0) {
		filesystem::remove_all("logs");
		filesystem::create_directory("logs");
	}

	return;
}
