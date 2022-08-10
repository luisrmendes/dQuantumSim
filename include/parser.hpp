#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

/**
 * @brief Convert quantum assembly instructions to integer codes.
 * Parses a subset of OpenQASM 2.0 specification.
 * 
 * @param fileName 
 * @return std::vector<unsigned int> 
 */
std::vector<unsigned int> sourceParser(char* fileName);

/**
 * @brief Returns the number of target qubits per instructions.
 * 
 * @param gate 
 * @return int 
 */
int getNumTargetQubitsOfGate(std::string gate);

/**
 * @brief Returns the integer code of the quantum assembly instruction.
 * 
 * @param line 
 * @return unsigned int 
 */
unsigned int getCodeOfInstruction(std::string line);

#endif
