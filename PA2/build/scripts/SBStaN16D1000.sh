#!/bin/bash
#SBATCH -o StaN16_1000J%J.out
#SBATCH --nodes 2
#SBATCH --ntasks 16
#SBATCH --time=00:05:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA2/build/mpi_mandelStatic 1000 1000
