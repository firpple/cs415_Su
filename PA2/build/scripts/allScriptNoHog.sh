#!/bin/bash

for i in 1
do
    echo "Welcome $i times"

    for file in SB*.sh

    do
        echo "Ran this file"

        echo ${file##*/}

        sbatch ${file}

        sleep 3m

    done  

done

  
