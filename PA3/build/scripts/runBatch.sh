#!/bin/bash
for m in 1 2 3 4 5
do

    for((i = 100; i <=100000000; i *= 10))
    do

        for((k = 2; k <=32 ; k *= 2))
        do

            sbatch SBBuckSeqB${k}S${i}
            
            sleep 1m
        done

    done

done
