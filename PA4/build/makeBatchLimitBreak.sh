#!/bin/bash
cd experimentalScripts

#4 cores
for ((k = 2640; k <= 4561; k += 240 ))
do

#echo "hello"
cat > SBMatrixParN4S${k} << EOF
#!/bin/bash
#SBATCH -o MParN4S${k}_%J.out
#SBATCH -N 1
#SBATCH --ntasks 4
#SBATCH --time=00:08:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA4/build/mpi_matrixParallel ${k} matrix/MatrixA matrix/MatrixB
EOF

done




#9 cores
for ((k = 2640; k <= 6120; k += 360 ))
do

#echo "hello"
cat > SBMatrixParN9S${k} << EOF
#!/bin/bash
#SBATCH -o MParN9S${k}_%J.out
#SBATCH -N 2
#SBATCH -n 9
#SBATCH --time=00:08:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA4/build/mpi_matrixParallel ${k} matrix/MatrixA matrix/MatrixB
EOF

done




#16 cores
for ((k = 2640; k <= 7681; k += 480))
do

#echo "hello"
cat > SBMatrixParN16S${k} << EOF
#!/bin/bash
#SBATCH -o MParN16S${k}_%J.out
#SBATCH -N 2
#SBATCH -n 16
#SBATCH --time=00:08:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA4/build/mpi_matrixParallel ${k} matrix/MatrixA matrix/MatrixB
EOF

done




#25 cores
for ((k = 2640; k <= 9001; k += 600))
do

#echo "hello"
cat > SBMatrixParN25S${k} << EOF
#!/bin/bash
#SBATCH -o MParN25S${k}_%J.out
#SBATCH -N 4
#SBATCH -n 25
#SBATCH --time=00:08:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA4/build/mpi_matrixParallel ${k} matrix/MatrixA matrix/MatrixB
EOF

done


