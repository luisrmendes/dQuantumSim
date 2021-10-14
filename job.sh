#!/bin/bash -x
#SBATCH --account=<prpb97>
#SBATCH --nodes=2
#SBATCH --ntasks=2
#SBATCH --output=mpi-out.%j
#SBATCH --error=mpi-err.%j
#SBATCH --time=00:15:00
#SBATCH --partition=batch

srun ./simulator $1