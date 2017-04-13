#!/bin/bash
cd scripts
#sequential
for k in 2640 2520 2400 2280 2160 2040 1440 1020
do

#echo "hello"
cat > SBMatrixSeqS${k} << EOF
#!/bin/bash
#SBATCH -o MSeqS${k}_%J.out
#SBATCH --ntasks 1
#SBATCH --time=00:08:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_matrixSequential ${k}
EOF

done


