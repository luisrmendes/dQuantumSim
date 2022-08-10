#include "parser.hpp"
#include <fstream>
#include <iostream>

using namespace std;

unsigned int getCodeOfInstruction(std::string line)
{
	if(line == "x")
		return 1;
	if(line == "y")
		return 2;
	if(line == "z")
		return 3;
	if(line == "h")
		return 4;
	if(line == "cx")
		return 5;
	if(line == "cz")
		return 6;
	if(line == "ccx")
		return 7;
	if(line == "sqrtx")
		return 8;
	if(line == "sqrty")
		return 9;
	if(line == "s")
		return 10;
	if(line == "t")
		return 11;
	if(line == "qreg")
		return 12;
	else
		return -1;
}

int getNumTargetQubitsOfGate(std::string gate)
{
	if(gate == "x" || gate == "y" || gate == "z" || gate == "h" || gate == "sqrtx" ||
	   gate == "sqrty" || gate == "s" || gate == "t") {
		return 1;
	} else if(gate == "cx" || gate == "cz") {
		return 2;
	} else if(gate == "ccx")
		return 3;
	else {
		return -1;
	}
}

vector<unsigned int> sourceParser(char* fileName)
{
	string line;
	ifstream myfile(fileName);

	if(!myfile.is_open()) {
		cerr << "Error opening file " << fileName << endl;
		exit(EXIT_FAILURE);
	}

	vector<unsigned int> instructions;

	string delim = " ";
	size_t end = 0U;
	size_t start = 0U;

	auto getNumberOfString = [](string input) {
		string result;
		for(size_t i = 0; i < input.length(); i++) {
			if(isdigit(input[i]))
				result += input[i];
		}
		return stoi(result);
	};

	// parse the first line, get the first work, ignore comments
	while(end == std::string::npos || (line[0] == '#') ||
		  line.substr(start, end - start) != "qreg") {
		// cout << "Parser => skipping line \"" << line << "\"" << endl;
		getline(myfile, line);
		end = line.find(delim);
	}

	start = end + delim.length();
	end = line.find(delim);
	string secondWord = line.substr(start, end - start);

	instructions.push_back(getNumberOfString(secondWord));

	// parse next lines
	while(getline(myfile, line)) {
		delim = ";";
		start = 0U;
		end = line.find(delim);

		if(end == std::string::npos || (line[0] == '#'))
			continue;

		string wholeLine = line.substr(start, end - start);

		string delimSpace = " ";
		end = line.find(delimSpace);
		string firstWord = wholeLine.substr(start, end - start);
		string secondWord = wholeLine.substr(end - start + 1, wholeLine.length());

		int numTargetQubits = getNumTargetQubitsOfGate(firstWord);
		if(numTargetQubits == -1) {
			cerr << "Parser => skipping instruction \"" << firstWord << "\"" << endl;
			continue;
		}

		instructions.push_back(getCodeOfInstruction(firstWord));

		start = 0U;
		for(int i = 0; i < numTargetQubits; i++) {
			string delimComma = ",";
			end = secondWord.find(delimComma, start);
			string qubitArgs = secondWord.substr(start, end - start);

			instructions.push_back(getNumberOfString(qubitArgs));
			start = end + delimComma.length();
		}
	}

	return instructions;
}
