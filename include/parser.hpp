#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

std::vector<unsigned int> sourceParser(char* fileName);

int checkValidInstruction(std::string line);

unsigned int getCodeOfInstruction(std::string line);

#endif
