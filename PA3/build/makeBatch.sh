#!/bin/bash
cd scripts
#sequential
for((i = 100; i <=10000000; i *= 10))
do

#makes 2 buckets
cat > SBBuckSeqB2S${i} << EOF
#!/bin/bash
#SBATCH -o BSeqB2S${i}_%J.out
#SBATCH --ntasks 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketSequential 2 ${i}
EOF

for((k = 4; k <=24 ; k += 4))
do
#echo "hello"
cat > SBBuckSeqB${k}S${i} << EOF
#!/bin/bash
#SBATCH -o BSeqB${k}S${i}_%J.out
#SBATCH --ntasks 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketSequential ${k} ${i}
EOF

done

done

#more sequential
for i in 100000000 200000000 400000000 800000000
do

#makes 2 buckets
cat > SBBuckSeqB2S${i} << EOF
#!/bin/bash
#SBATCH -o BSeqB2S${i}_%J.out
#SBATCH --ntasks 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketSequential 2 ${i}
EOF

for((k = 4; k <=24 ; k += 4))
do
#echo "hello"
cat > SBBuckSeqB${k}S${i} << EOF
#!/bin/bash
#SBATCH -o BSeqB${k}S${i}_%J.out
#SBATCH --ntasks 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketSequential ${k} ${i}
EOF

done

done


#parallel stuffs
for((i = 100; i <=10000000; i *= 10))
do

#2
cat > SBBuckParB2S${i} << EOF
#!/bin/bash
#SBATCH -o BParB2S${i}_%J.out
#SBATCH --ntasks 2
#SBATCH --nodes 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 2 ${i}
EOF
#4
cat > SBBuckParB4S${i} << EOF
#!/bin/bash
#SBATCH -o BParB4S${i}_%J.out
#SBATCH --ntasks 4
#SBATCH --nodes 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 4 ${i}
EOF
#8
cat > SBBuckParB8S${i} << EOF
#!/bin/bash
#SBATCH -o BParB8S${i}_%J.out
#SBATCH --ntasks 8
#SBATCH --nodes 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 8 ${i}
EOF
#12
cat > SBBuckParB12S${i} << EOF
#!/bin/bash
#SBATCH -o BParB12S${i}_%J.out
#SBATCH --ntasks 12
#SBATCH --nodes 2
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 12 ${i}
EOF
#16
cat > SBBuckParB16S${i} << EOF
#!/bin/bash
#SBATCH -o BParB16S${i}_%J.out
#SBATCH --ntasks 16
#SBATCH --nodes 2
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 16 ${i}
EOF
#20
cat > SBBuckParB20S${i} << EOF
#!/bin/bash
#SBATCH -o BParB20S${i}_%J.out
#SBATCH --ntasks 20
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 20 ${i}
EOF
#24
cat > SBBuckParB24S${i} << EOF
#!/bin/bash
#SBATCH -o BParB24S${i}_%J.out
#SBATCH --ntasks 24
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 24 ${i}
EOF

done


for i in 100000000 200000000 400000000 800000000
do

#2
cat > SBBuckParB2S${i} << EOF
#!/bin/bash
#SBATCH -o BParB2S${i}_%J.out
#SBATCH --ntasks 2
#SBATCH --nodes 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 2 ${i}
EOF
#4
cat > SBBuckParB4S${i} << EOF
#!/bin/bash
#SBATCH -o BParB4S${i}_%J.out
#SBATCH --ntasks 4
#SBATCH --nodes 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 4 ${i}
EOF
#8
cat > SBBuckParB8S${i} << EOF
#!/bin/bash
#SBATCH -o BParB8S${i}_%J.out
#SBATCH --ntasks 8
#SBATCH --nodes 1
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 8 ${i}
EOF
#12
cat > SBBuckParB12S${i} << EOF
#!/bin/bash
#SBATCH -o BParB12S${i}_%J.out
#SBATCH --ntasks 12
#SBATCH --nodes 2
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 12 ${i}
EOF
#16
cat > SBBuckParB16S${i} << EOF
#!/bin/bash
#SBATCH -o BParB16S${i}_%J.out
#SBATCH --ntasks 16
#SBATCH --nodes 2
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 16 ${i}
EOF
#20
cat > SBBuckParB20S${i} << EOF
#!/bin/bash
#SBATCH -o BParB20S${i}_%J.out
#SBATCH --ntasks 20
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 20 ${i}
EOF
#24
cat > SBBuckParB24S${i} << EOF
#!/bin/bash
#SBATCH -o BParB24S${i}_%J.out
#SBATCH --ntasks 24
#SBATCH --time=00:07:00
#SBATCH --mail-user=pikmin898@gmail.com
#SBATCH --mail-type=ALL

srun ~/cs415_Su/PA3/build/mpi_bucketParallel 24 ${i}
EOF

done





cd ..
#done
