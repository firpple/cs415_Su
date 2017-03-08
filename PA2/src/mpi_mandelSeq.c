/******************************************************************************
* FILE: mpi_packetCheck.c
* DESCRIPTION:
*   MPI Send and recieved. The program outputs the ellapsed time between mpi 
*   send and mpi receive for increasing message sizes. The ellapsed time is
*   measured in microseconds
* AUTHOR: Evan Su
* LAST REVISED: 02/19/17
******************************************************************************/
#include "mpi.h"
#include "mpi_mandelUltility.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#define  MASTER		0
#define  SLAVE      1
#define  TAG        0
#define  ARRAYSIZE  10000
#define  ITERATIONS 50
#define  SECTOMICRO 1000000


void masterCode();
void slaveCode();

int main (int argc, char *argv[])
{
    //preprocessing
    int numtasks, taskid, len;
    int numberArray[ARRAYSIZE];
    int index,indexS;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    float elapsedTime = 0;
    struct timeval startTime, endTime, diffTime;
    timerclear(&diffTime);

    //parallel start
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Get_processor_name(hostname, &len);

    //the printf here is only used for debugging
    //printf ("Hello from task %d on %s!\n", taskid, hostname);

    //Iterator indexs is for message size
    timerclear(&diffTime);

    if (taskid == MASTER)
    {
	    //master code
        //
	    gettimeofday(&startTime, NULL); //start clock
        //send to slave
	    masterCode();

	    timersub(&endTime, &startTime, &diffTime); //calc diff time
        //converts time struct to float
	    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 
        //prints result in CSV format
	    printf("%f,",elapsedTime );
        printf("\n");
    }
    else
    {
        //slave code
        //this code does nothing, placeholder for parallel.
	    slaveCode();
    }

    MPI_Finalize();
    return 0;



}


void masterCode()
{
    printf("hello from master");
}

void slaveCode()
{
    printf("hello from slave");
}


