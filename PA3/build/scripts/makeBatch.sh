#!/bin/bash
for((i = 100; i <=100000000; i *= 10))
do

for((k = 2; k <=32 ; k *= 2))
do
#echo "hello"
cat > SBBuckSeqB${k}S${i} << EOF
#!/bin/bash
#SBATCH -o BSeqB${k}S${i}_%J.out
#SBATCH --ntasks 1
#SBATCH --time=00:05:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketSequential ${k} ~/cs415_Su/PA3/build/unsorted/unsort_N${i}
EOF

done

done
