#ifndef UTILSMPI_HPP
#define UTILSMPI_HPP

#include "constants.hpp"
#include <vector>

/**
 * @brief Distribute instructions read from rank 0 to all ranks.
 * Must only be used if MPI does not share environment folder across machines
 * 
 * @param instructions 
 * @return std::vector<unsigned int> 
 */
std::vector<unsigned int>
broadcastInstructions(std::vector<unsigned int>& instructions);

#endif
