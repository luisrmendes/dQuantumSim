#ifndef _MACROS_H
#define _MACROS_H
#include "mpi.h"

#define PRECISION_TYPE double

// 2^64 = 18446744073709551616
// 2^32 =
// 2^16 = 65536

/* MPI_RECV_BUFFER_SIZE must always be double LOCK_STEP_DISTR_THRESHOLD */
#define LOCK_STEP_DISTR_THRESHOLD 100000
#define MPI_RECV_BUFFER_SIZE	  200000
// #define MPI_RECV_BUFFER_BYTE_SIZE 4800000 // MPI_RECV_BUFFER_SIZE * 8 * 3 (StatesAndAmplitude)
#define MAX_NUMBER_QUBITS 100 // used in the arrays on measuring results

#endif
