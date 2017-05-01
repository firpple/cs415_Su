#!/bin/bash

#BATCHFILES="~/cs415Workspace/pull2/cs415_Su/PA3/build/scripts"

cd ..
cd bin
#echo hello
for i in 1 2 3 4 5
do
    
    #for batchfil in ~/cs415Workspace/pull2/cs415_Su/PA4/build/scripts/SB*
    for batchfil in ~/cs415_Su/PA4/build/experimentalScripts/*
    do

        #echo "try to batch $sbatchfil"
        TEST=$(squeue -o"%.18i %.9P %.20j %.20u %.8T %.10M %.9l %.6D %R")

        while [[ "$TEST" =~ "evansu" ]]
        do
            sleep 1s
            TEST=$(squeue -o"%.18i %.9P %.20j %.20u %.8T %.10M %.9l %.6D %R")
        done

        sbatch $batchfil
        #echo "here"
        echo "submited this batch:"
        echo $batchfil
        echo $i
        echo "."
        sleep 1s
    done

done

cd ..
cd build
