#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

std::vector<unsigned int> sourceParser(char* fileName);

int getNumTargetQubitsOfGate(std::string gate);

unsigned int getCodeOfInstruction(std::string line);

#endif
