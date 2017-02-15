/******************************************************************************
* FILE: mpi_packetCheck.c
* DESCRIPTION:
*   MPI tutorial example code: Simple pingpong program
* AUTHOR: Evan Su
* LAST REVISED: 02/15/17
******************************************************************************/
#include "mpi.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#define  MASTER		0
#define  SLAVE      1
#define ARRAYSIZE   100

int main (int argc, char *argv[])
{
int   numtasks, taskid, len;
char hostname[MPI_MAX_PROCESSOR_NAME];
int number;
int numberArray[ARRAYSIZE];
int iteration = 25;
int index,indexS;
float averageTime = 0;
struct timeval startTime, endTime, diffTime, totalTime;
//preprocessing
timerclear(&totalTime);
timerclear(&diffTime);

//paralle start
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Get_processor_name(hostname, &len);


//printf ("Hello from task %d on %s!\n", taskid, hostname);
for(indexS = 0; indexS < ARRAYSIZE; indexS++)
{
	timerclear(&totalTime);
	timerclear(&diffTime);

	if (taskid == MASTER)
	{
		
		for(index = 0; index < iteration ; index++)
		{
			gettimeofday(&startTime, NULL); //start clock
			MPI_Send(numberArray, indexS, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(numberArray, indexS, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			gettimeofday(&endTime, NULL); //end clock

			timersub(&endTime, &startTime, &diffTime); //calc diff time
			timeradd(&diffTime, &totalTime, &totalTime); //acculmate diff time

		}
		averageTime = (totalTime.tv_sec * 1000000 + totalTime.tv_usec)/ (iteration*2);
		//printf("total iterations:%d",iteration *2);
		printf("Time: %10.0f microseconds for %d\n", averageTime, indexS); //average printout
	}
	else
	{
		for(index = 0; index < iteration ; index++)
		{
			MPI_Recv(numberArray, indexS, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(numberArray, indexS, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	}
}

MPI_Finalize();
printf("Task done\n");
return 0;



}

