#!/bin/bash
#SBATCH -o DynN2_500J%J.out
#SBATCH --nodes 1
#SBATCH --ntasks 8
#SBATCH --time=00:05:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA2/build/mpi_mandelDynamic 8000 8000
