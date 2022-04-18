#include "mpi.h"

#define LOCK_STEP_DISTR_THRESHOLD 100000
#define MPI_RECV_BUFFER_SIZE	  262145
#define MPI_MAX_BUFFER			  1000
#define MAX_NUMBER_QUBITS		  100 // used in the arrays on measuring results

#if USING_DOUBLE
#define MPI_PRECISION_TYPE_COMPLEX                                                  \
	OMPI_PREDEFINED_GLOBAL(MPI_Datatype, ompi_mpi_dblcplex)
#define MPI_PRECISION_TYPE OMPI_PREDEFINED_GLOBAL(MPI_Datatype, ompi_mpi_double)
#define PRECISION_TYPE	   double

#elif USING_FLOAT
#define MPI_PRECISION_TYPE_COMPLEX                                                  \
	OMPI_PREDEFINED_GLOBAL(MPI_Datatype, ompi_mpi_c_float_complex)
#define MPI_PRECISION_TYPE OMPI_PREDEFINED_GLOBAL(MPI_Datatype, ompi_mpi_float)
#define PRECISION_TYPE	   float
#endif