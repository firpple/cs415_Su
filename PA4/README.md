# Author
Evan Su

# PA4: "Parallel Matrix Multiplication"
MPI programs that measures the exectution time for matrix multiplication.
## Files in PA4

All Source code is located in the src folder.

Batch scripts and makefile is located in the build folder.

More scripts are found in the scripts folder within the build folder. Use at your own risk as these scripts are not throughly tested and the output may be incorrect.

bin contains all raw data that was collected in the tests

# Dependencies, Building, and Running

## Dependency Instructions

Quoted from user cs415-615
>These projects depend on the MPI Library and SLIURM Running environment preinstalled in [h1.cse.unr.edu](h1.cse.unr.edu).  There are no instructions included on how to install these suites on your personal machines.
>If you are using a Windows machine, you may SSH into your respective UNR accounts through [PuTTY] (http://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html).
>
>You may enter h1 off campus by first logging into your account through ubuntu.cse.unr.edu.  (Please note that keystrokes will be a tiny bit slower, but it's still manageable).
>```bash
>ssh username@ubuntu.cse.unr.edu
>ssh username@h1.cse.unr.edu
>```

## Building and Running
The program can be build two ways: Manually compile them with mpicc or use the makefile.
The program must be ran using sbatch. 
### mpicc Instructions
```bash
cd build
mpicc -Wall -o mpi_MatrixSequential ../src/mpi_MatrixSeq.c -lpmi
mpicc -Wall -o mpi_MatrixParallel ../src/mpi_MatrixPar.c -lpmi


#running the program
#<>
#format for manual run
#srun -n<cores> mpi_MatrixSequential <size> <matrix 1> <matrix 2>
#srun -n<cores> mpi_MatrixParallel <size> <matrix 1> <matrix 2>
#size must be set to 0 in order for program to read from file
#otherwise, the numbers will be automaticly generated.
#<>
srun -n1 mpi_MatrixSequential 1020 matrix/MatrixA matrix/MatrixB
srun -n4 mpi_MatrixParallel 1020 matrix/MatrixA matrix/MatrixB


#or
#<>
#Run batch scripts

./makeBatch.sh
#<>
sbatch scripts/SBMatrixSeqS1020
sbatch scripts/SBMatrixParN4S1020

#or
./runBatch.sh
#runBatch.sh must be modified so that it will check for your user name.
```


To clean the files, run this command:
```bash
rm mpi_MatrixSequential mpi_MatrixParallel
```


### Makefile Instructions
The makefile works as expected and must be updated with new files added in.

```bash
cd build
make
./makeBatch.sh
./runBatch.sh
```



To clean the files you've created, there is an extra target, `clean`.
These are for convienience only, you may add/remove them as necessary.
For best results run the scripts manually and separately. 

```bash
make clean
```

## Note on *LimitBreak.sh
These scripts are experiemental, use them at your own risk.
