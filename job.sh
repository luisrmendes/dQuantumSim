#!/bin/bash -x
#SBATCH --account=prpb97
#SBATCH --nodes=800
#SBATCH --ntasks=1600
#SBATCH --output=mpi-out.%j
#SBATCH --error=mpi-err.%j
#SBATCH --time=00:30:00
#SBATCH --partition=batch

srun ./simulator $1