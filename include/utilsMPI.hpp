#ifndef UTILSMPI_HPP
#define UTILSMPI_HPP

#include "constants.hpp"
#include <array>
#include <cstdint>
#include <vector>

/**
 * @brief Distribute instructions read from rank 0 to all ranks.
 * Must only be used if MPI does not share environment folder across machines
 * 
 * @param instructions 
 * @return std::vector<unsigned int> 
 */
std::vector<unsigned int>
instructionsHandlerMPI(std::vector<unsigned int>& instructions);



#endif
