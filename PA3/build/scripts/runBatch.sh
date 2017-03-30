#!/bin/bash
for((i = 100; i <=100000000; i *= 10))
do

for((k = 2; k <=32 ; k *= 2))
do

sbatch SBBuckSeqB${k}S${i}

done

done
