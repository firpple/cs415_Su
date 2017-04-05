#!/bin/bash

#BATCHFILES="~/cs415Workspace/pull2/cs415_Su/PA3/build/scripts"

cd ..
cd bin
#echo hello
for i in 1
do

    for batchfil in ~/cs415_Su/PA3/build/scripts/*
    do

        TEST=$(squeue -o"%.18i %.9P %.20j %.20u %.8T %.10M %.9l %.6D %R")

        while [[ "$TEST" =~ "evansu-" ]]
        do
            sleep 1s
            TEST=$(squeue -o"%.18i %.9P %.20j %.20u %.8T %.10M %.9l %.6D %R")
        done

        sbatch $batchfile
        #echo "here"
        echo $batchfil
        sleep 1s
    done

done

cd ..
cd build
