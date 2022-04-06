#include "parser.h"
#include <fstream>
#include <iostream>

using namespace std;

unsigned int getCodeOfInstruction(std::string line)
{
	if(line == "pauliX")
		return 1;
	if(line == "pauliY")
		return 2;
	if(line == "pauliZ")
		return 3;
	if(line == "hadamard")
		return 4;
	if(line == "controlledX")
		return 5;
	if(line == "controlledZ")
		return 6;
	if(line == "toffoli")
		return 7;
	else
		return -1;
}

int checkValidInstruction(std::string line)
{
	if(line == "pauliX" || line == "pauliY" || line == "pauliZ" ||
	   line == "hadamard") {
		return 1;
	} else if(line == "controlledX" || line == "controlledZ") {
		return 2;
	} else if(line == "toffoli")
		return 3;
	else {
		cerr << "Instruction not recognized: " << line << endl;
		return -1;
	}
}

vector<unsigned int> sourceParser(char* fileName)
{
	string line;
	ifstream myfile(fileName);

	if(!myfile.is_open()) {
		perror("Error opening file!\n");
		exit(EXIT_FAILURE);
	}

	vector<unsigned int> instructions;

	string delim = " ";
	size_t end;
	size_t start = 0U;
	do {
		// parse first line (init)
		getline(myfile, line);
		end = line.find(delim);
	} while(end == std::string::npos || (line[0] == '#'));

	if(line.substr(start, end - start) != "init") {
		cerr << "First line of file must have init <number_of_qubits>" << endl;
		exit(EXIT_FAILURE);
	} else {
		start = end + delim.length();
		end = line.find(delim);
		instructions.push_back(stoi(line.substr(start, end - start)));
	}

	// parse next lines
	while(getline(myfile, line)) {
		start = 0U;
		end = line.find(delim);

		if(end == std::string::npos || (line[0] == '#'))
			continue;

		int iterations = checkValidInstruction(line.substr(start, end - start));

		if(iterations == -1) {
			cerr << "Instruction not recognized!" << endl;
			exit(EXIT_FAILURE);
		}

		instructions.push_back(
			getCodeOfInstruction(line.substr(start, end - start)));

		for(int i = 0; i < iterations; i++) {
			start = end + delim.length();
			end = line.find(delim, start);
			instructions.push_back(stoi(line.substr(start, end - start)));
		}
	}

	return instructions;
}
