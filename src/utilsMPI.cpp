#include "utilsMPI.hpp"
#include "constants.hpp"
#include "debug.hpp"
#include "debugLogFlags.hpp"
#include "mpi.h"

using namespace std;

vector<unsigned int> broadcastInstructions(vector<unsigned int>& instructions)
{
    size_t num_instructions = instructions.size();
    unsigned int* instructions_arr = &instructions[0];

    MPI_Bcast(&num_instructions, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(instructions_arr, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    vector<unsigned int> returnInstructions(instructions_arr,
                                            instructions_arr + num_instructions);

#ifdef INSTRUCTIONS_HANDLER_LOGS
    appendDebugLog("Instructions received: \n");
    for(size_t i = 0; i < instructions.size(); i++) {
        appendDebugLog(instructions[i], " ");
    }
    appendDebugLog("\n\n");
#endif

    return returnInstructions;
}