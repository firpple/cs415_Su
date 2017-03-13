#!/bin/bash
#SBATCH -o StaN4_2000J%J.out
#SBATCH --nodes 1
#SBATCH --ntasks 4
#SBATCH --time=00:05:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA2/build/mpi_mandelStatic 2000 2000
