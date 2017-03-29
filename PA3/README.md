# Author
Evan Su

# PA2: "Embarrassing Parallel Mandelbrot"
MPI programs that measures the time for the mandelbrot image to calucalate.
## Files in PA1

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
mpicc -Wall -c mpi_mandelUtility.c -lpmi
mpicc -Wall -o mpi_mandelSeq ../src/mpi_mandelSeq.c mpi_mandelUtility.o -lpmi
mpicc -Wall -o mpi_mandelStatic ../src/mpi_mandelStatic.c mpi_mandelUtility.o -lpmi
mpicc -Wall -o mpi_mandelDynamic ../src/mpi_mandelDynamic.c mpi_mandelUtility.o -lpmi
sbatch dynamic.sh
sbatch sequential.sh
sbatch static.sh
```


To clean the files, run this command:
```bash
rm mpi_mandelSeq mpi_mandelStatic mpi_mandelDynamic mpi_mandelUtility.o
```


### Makefile Instructions
The makefile works as expected and must be updated with new files added in.

```bash
cd build
make
sbatch dynamic.sh
sbatch sequential.sh
sbatch static.sh
```



To clean the files you've created, there is an extra target, `clean`..
There is a run target that will automatically run sbatch all the scripts, `run`
These are for convienience only, you may add/remove them as necessary.
For best results run the scripts manually and separately. 

```bash
make clean
make run
```
