#!/bin/bash
#SBATCH --nodes 2
#SBATCH --ntasks 2
#SBATCH --time=00:30:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415/PA01/src/mpi_packetCheck
