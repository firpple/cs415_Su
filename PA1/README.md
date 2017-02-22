# Author
Evan Su

# PA1: "Ping Pong"

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
The program can be build two ways: Manually compile them with mpicc or use the makefile
The program must be ran using sbatch. 
### mpicc Instructions
```bash
mpicc -Wall -o mpi_pingpong src/mpi_pingpong.c -lpmi
mpicc -Wall -o mpi_packetCheck src/mpi_packetCheck.c -lpmi
sbatch One_box.sh
sbatch Two_box.sh
sbatch Timing.sh
```


To clean the files, run this command:
```bash
rm mpi_pingpong mpi_packetCheck
```


### Makefile Instructions
The makefile works as expected and must be updated with new files added in.

```bash
make
sbatch One_box.sh
sbatch Two_box.sh
sbatch Timing.sh
```



To clean the files you've created, there is an extra target, `clean`..
There is a run target that will automatically run sbatch all the scripts, `run`
These are for convienience only, you may add/remove them as necessary.
For best results run the scripts manually and separately. 

```bash
make clean
make run
```
