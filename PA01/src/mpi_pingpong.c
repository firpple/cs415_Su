/******************************************************************************
* FILE: mpi_hello.c
* DESCRIPTION:
*   MPI tutorial example code: Simple hello world program
* AUTHOR: Blaise Barney
* LAST REVISED: 03/05/10
******************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define  MASTER		0
#define  SLAVE      1

int main (int argc, char *argv[])
{
int   numtasks, taskid, len;
char hostname[MPI_MAX_PROCESSOR_NAME];
int number;
int iterations = 0;
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
if (taskid == MASTER)
{
	number = -1;
	gettimeofday(&startTime, NULL); //start clock
	MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
	//printf("finish send\n");
	MPI_Recv(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	gettimeofday(&endTime, NULL); //end clock

	timersub(&endTime, &startTime, &diffTime); //calc diff time
	timeradd(&diffTime, &totalTime, &totalTime); //acculmate diff time
	iteration += 2;//increment number of iterations
	averageTime = (totalTime.tv_sec * 1000000 + totalTime.tv_usec)/iteration;
	printf("average travel time %f microseconds\n"); //average printout
	//printf("got reply\n");
}
else
{
	MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("recieved packet\n");
	MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
}
MPI_Finalize();
printf("Task done\n");
return 0;



}

