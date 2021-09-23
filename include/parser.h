#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

std::vector<unsigned int> sourceParser(char* fileName);
int checkValidInstruction(std::string line);
unsigned int getCodeOfInstruction(std::string line);

#endif