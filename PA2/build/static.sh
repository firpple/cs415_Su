#!/bin/bash
#SBATCH --nodes 1
#SBATCH --ntasks 8
#SBATCH --time=00:05:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA2/build/mpi_mandelStatic 5000 6000
