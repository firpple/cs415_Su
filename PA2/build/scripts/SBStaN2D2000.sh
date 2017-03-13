#!/bin/bash
#SBATCH -o StaN2_2000J%J.out
#SBATCH --nodes 1
#SBATCH --ntasks 2
#SBATCH --time=00:05:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA2/build/mpi_mandelStatic 2000 2000
