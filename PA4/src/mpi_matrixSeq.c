/******************************************************************************
* FILE: mpi_matrixSeq.c
* DESCRIPTION:
*   Calculates the matrix sequential. 
* AUTHOR: Evan Su
* LAST REVISED: 04/5/17
******************************************************************************/
//libraries
#include "mpi.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

//define constants
#define  MASTER		0
#define  SLAVE      1
#define  TAG        0
#define  SECTOMICRO 1000000
#define  PRINT      0
#define SORTONLY 	0
//function declarations
void masterCode(int);
void slaveCode(int,int);
void matrixMultipleSquare(int **, int**, int**, int);

void printMatrix(int **, int );

//Main function
/*
 *  Function name: main
 *  
 *  Brief: Main driver for the program
 *  
 *  Detail: The main driver initializes parallel processes.
 *          The master calls the master function
 *          The slave calls the slave function
 *          Both functions will run until computations are over.
 *
 *  Note: The slave functions does nothing, it is a placeholder
 *        for parallel code. 
 *          
 */
int main (int argc, char *argv[])
{
    //argc checker
    if(argc < 2)
    {
        //there are not enough arugments
        //note: this will not check if the arguements are in the correct positions
        printf("not enough arguments\n");
        printf("proper usage:\n srun mpi_BucketSeq <filename> <mpi arguements>");
        return 0;
    }
    
    //preprocessing
    int numtasks, taskid, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    //parallel start
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Get_processor_name(hostname, &len);


    if (taskid == MASTER)
    {
	    //master code
	    masterCode(atoi(argv[1]));


    }
    else
    {
        //slave code
        //this code does nothing, placeholder for parallel.
	    slaveCode(atoi(argv[1]), taskid);
    }

    MPI_Finalize();
    return 0;



}

/*
 *  Function name: masterCode
 *  
 *  Brief: bucketsort code for the master node
 *  
 *  Detail: The master node will readin the numbers from the given file name
 *          The master node will send a portion of the numbers to each slave
 *          Then, the portion remaining is sorted using bucket sort
 *          
 */
void masterCode(int size)
{
    struct timeval startTime, endTime, diffTime;
    struct timeval sortTime;
    float elapsedTime = 0;
	int **matrixA, **matrixB, **matrixC;
	int indexIn, indexOut;

    //make matrix
	matrixA = (int **)malloc(sizeof(int*) * size);
	matrixB = (int **)malloc(sizeof(int*) * size);
	matrixC = (int **)malloc(sizeof(int*) * size);
	for (indexIn = 0; indexIn < size; indexIn ++)
	{
		matrixA[indexIn] = (int*)malloc(sizeof(int)* size);
		matrixB[indexIn] = (int*)malloc(sizeof(int)* size);		
		matrixC[indexIn] = (int*)malloc(sizeof(int)* size);
	}
	

    //fill matrix
    for(indexOut = 0; indexOut < size; indexOut++)
	{
		for(indexIn = 0; indexIn < size; indexIn++)
		{
			matrixA[indexOut][indexIn] = indexIn + indexOut;
			matrixB[indexOut][indexIn] = indexIn + indexOut;
		}
	}
    
    
    //start time
    gettimeofday(&startTime, NULL);

    //matrix multiplication
	
    //stop time
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 

    //prints result
	//time
	printf("%f,",elapsedTime );
	//print matrixs
	//matrix A
	printf("matrix A:\n");
	printMatrix(matrixA, size);
	//matrix B
	printf("matrix B:\n");
	printMatrix(matrixB, size);
	//matrix C
	printf("matrix C:\n");
	
}

/*
 *  Function name: slaveCode
 *  
 *  Brief: bucketsort code for the slave node
 *  
 *  Detail: The slave node does nothing in sequential calculations
 *  
 */
void slaveCode(int buckets, int rank)
{
    //printf("hello from slave");
}

void matrixMultipleSquare(int **matrixA, int**matrixB, int**matrixResult, int length)
{
	
}

void printMatrix(int **matrix, int length)
{

}
