#!/bin/bash
#SBATCH -o Seq500J%J.out
#SBATCH --ntasks 1
#SBATCH --time=00:05:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA2/build/mpi_mandelSeq 500 500