#ifndef DEBUG_H
#define DEBUG_H

#include <sstream>

extern std::stringstream debugLog;
extern std::stringstream resultLog;

/**
 * Creates log file and directory, outputs the debug log of each process 
 * to a log file in directory "logs" and to stdout if argument is true. 
 * @param to_stdout if true, outputs to stdout
 */
void outputDebugLogs(int rank, int size, bool to_stdout);

void outputResult(int rank, int size);

/**
 * Appends info to the debug log of each process
 * @param args Variable list of arguments to print
 */
template <typename... T>
void appendDebugLog(const T&... args)
{
	((debugLog << args), ...);
}

/**
 * Appends info to the result log of each process
 * @param args Variable list of arguments to print
 */
template <typename... T>
void appendResultLog(const T&... args)
{
	((resultLog << args), ...);
}

#endif