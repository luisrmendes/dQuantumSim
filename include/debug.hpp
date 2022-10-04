#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <fstream>
#include <iostream>

extern int rank;
extern int size;

/**
 * Creates log file and directory, outputs the debug log of each process 
 * to a log file in directory "logs" and to stdout if argument is true. 
 * @param to_stdout if true, outputs to stdout
 */
void openAndCleanDebugFiles();

/**
 * Appends info to the debug log of each process
 * @param args Variable list of arguments to print
 */
template <typename... T>
void appendDebugLog(const T&... args)
{
    std::fstream nodeLogFile;
    std::string nodeFileName = "logs/log";
    nodeFileName.append(std::to_string(::rank));
    nodeFileName.append(".txt");

    nodeLogFile.open(nodeFileName, std::ios::app);
    if(!nodeLogFile)
        std::cerr << "file does not exist" << std::endl;
    ((nodeLogFile << args), ...);
    nodeLogFile.close();

    // std::fstream globalLogFile;
    // for(int i = 0; i < size; i++) {
    // 	if(rank == i) {
    // 		globalLogFile.open(globalLogFileName, std::ios::app);
    // 		if(!globalLogFile)
    // 			std::cerr << "file does not exist" << std::endl;
    // 		((globalLogFile << args), ...);
    // 		globalLogFile.close();
    // 	} else
    // 		MPI_Barrier(MPI_COMM_WORLD);
    // }
}

/**
 * Appends info to the result log of each process
 * @param args Variable list of arguments to print
 */
template <typename... T>
void appendResultLog(const T&... args)
{
    std::fstream resultLog;
    ((resultLog << args), ...);
}

#endif
