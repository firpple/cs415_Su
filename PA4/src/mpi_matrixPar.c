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
#define	 PRINTMATRIX	1
#define  RANGE		100
#define  TRUE		1
#define  FALSE		0
//function declarations
void masterCode(int, int);
void slaveCode(int,int, int);
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
	int length;
	int validSize = TRUE;
    //parallel start
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Get_processor_name(hostname, &len);
	
	switch(numtasks)
	{
		case 4:
			length = 2;
			break;
		case 9:
			length = 3;
			break;
		case 16:
			length = 4;
			break;
		case 25:
			length = 5;
			break;
		case 36:
			length = 6;
			break;
		default:
			validSize = FALSE;
			break;
	}
	if(validSize == FALSE)
	{
		printf("invalid size\n");
		printf("size: %d", length);
	}
    else if (taskid == MASTER)
    {
	    //master code
	    masterCode(atoi(argv[1]), length);


    }
    else
    {
        //slave code
        //this code does nothing, placeholder for parallel.
	    slaveCode(atoi(argv[1]), taskid, length);
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
void masterCode(int size, int length)
{
    struct timeval startTime, endTime, diffTime;
    float elapsedTime = 0;
	int **matrixA, **matrixB, **matrixC;
	int **tileA, **tileB, **tileC;
	int sendRow, sendCol;
	int *sendBuffer, *recvBuffer;
	int indexIn, indexOut, indexSub;
	int counter = 0;
	int tileLength = size/length;

    //make matrix
	matrixA = (int **)malloc(sizeof(int*) * size);
	matrixB = (int **)malloc(sizeof(int*) * size);
	matrixC = (int **)malloc(sizeof(int*) * size);
	for (indexIn = 0; indexIn < size; indexIn ++)
	{
		matrixA[indexIn] = (int*)calloc(size ,sizeof(int));
		matrixB[indexIn] = (int*)calloc(size ,sizeof(int));		
		matrixC[indexIn] = (int*)calloc(size ,sizeof(int));
	}
	
	//make tiles
	tileA = (int **)malloc(sizeof(int*) * tileLength);
	tileB = (int **)malloc(sizeof(int*) * tileLength);
	tileC = (int **)malloc(sizeof(int*) * tileLength);
	for (indexIn = 0; indexIn < tileLength; indexIn ++)
	{
		tileA[indexIn] = (int*)calloc(tileLength, sizeof(int));
		tileB[indexIn] = (int*)calloc(tileLength, sizeof(int));		
		tileC[indexIn] = (int*)calloc(tileLength, sizeof(int));
	}
	//make comm buffers
	sendBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);
	recvBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);

    //fill matrix
	srand(0);
    for(indexOut = 0; indexOut < size; indexOut++)
	{
		for(indexIn = 0; indexIn < size; indexIn++)
		{
			//matrixA[indexOut][indexIn] = (indexIn + indexOut)%100;
			//matrixB[indexOut][indexIn] = (indexIn + indexOut)%100;
			
			matrixA[indexOut][indexIn] = rand() %RANGE;
			matrixB[indexOut][indexIn] = rand() %RANGE;
			matrixC[indexOut][indexIn] = 0;
		}
	}
    
    //gives everyone data
	//for goes from 0 to max num of slaves
	for(indexOut = 0; indexOut < length*length; indexOut++)
	{
		if(indexOut == 0)
		{
			//master
			for(indexIn = 0; indexIn < tileLength; indexIn++)
			{
				for(indexSub = 0; indexSub < tileLength; indexSub++)
				{
					tileA[indexIn][indexSub] = matrixA[indexIn][indexSub];
					tileB[indexIn][indexSub] = matrixB[indexIn][indexSub];
				}
			}



		}
		else
		{
			//slaves
			//calculates the coordinate			
			sendRow = indexOut/length;
			sendCol = indexOut%length;
			//fills send buffer
			for(indexIn = 0; indexIn < tileLength; indexIn++)
			{
				for(indexSub = 0; indexSub < tileLength; indexSub++)
				{
					sendBuffer[indexIn*tileLength + indexSub] = 
							matrixA[sendRow*tileLength + indexIn]
							[sendCol*tileLength + indexSub];
					printf("row:%d col%d", sendRow, sendCol);
				}
			}

			//sends tile A
			MPI_Send(sendBuffer, tileLength*tileLength, 
					MPI_INT, indexOut, TAG, MPI_COMM_WORLD);
			//<><><
		}
	}



    //start time
    gettimeofday(&startTime, NULL);

    //matrix multiplication
	matrixMultipleSquare(matrixA, matrixB, matrixC, size);
    //stop time
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 

    //prints result
	//time
	printf("%f,",elapsedTime );
	//print matrixs
	
	if(PRINTMATRIX)
	{
		printf("\n");
		//matrix A
		printf("matrix A:\n");
		printMatrix(matrixA, size);
		//matrix B
		printf("matrix B:\n");
		printMatrix(matrixB, size);
		//matrix C
		printf("matrix C:\n");
		printMatrix(matrixC, size);
	}
	
	//prints tiles
	if(PRINTMATRIX)
	{
		printf("\n");
		//matrix A
		printf("Rank: %d\n", 0);
		printf("Tile A:\n");
		printMatrix(tileA, tileLength);
		//matrix B
		printf("Tile B:\n");
		printMatrix(tileB, tileLength);
		//matrix C
		printf("Tile C:\n");
		printMatrix(tileC, tileLength);
	}

	//free memory
	for(indexIn = 0; indexIn < size; indexIn++)
	{
		free(matrixA[indexIn]);
		free(matrixB[indexIn]);
		free(matrixC[indexIn]);
	}
	free(matrixA);
	free(matrixB);
	free(matrixC);
	//printf("freed matrix\n");

	for(indexIn = 0; indexIn < length; indexIn++)
	{
		free(tileA[indexIn]);
		free(tileB[indexIn]);
		free(tileC[indexIn]);
	}
	free(tileA);
	free(tileB);
	free(tileC);
	//printf("freed tiles\n");
	free(sendBuffer);
	free(recvBuffer);
}

/*
 *  Function name: slaveCode
 *  
 *  Brief: bucketsort code for the slave node
 *  
 *  Detail: The slave node does nothing in sequential calculations
 *  
 */
void slaveCode(int size, int rank, int length)
{
    //printf("hello from slave");
	int indexIn, indexOut, indexSub;
	int **tileA, **tileB, **tileC;
	int *sendBuffer, *recvBuffer;
	int tileLength;
	tileLength = size/length;
	//make tiles
	tileA = (int **)malloc(sizeof(int*) * tileLength);
	tileB = (int **)malloc(sizeof(int*) * tileLength);
	tileC = (int **)malloc(sizeof(int*) * tileLength);
	for (indexIn = 0; indexIn < tileLength; indexIn ++)
	{
		tileA[indexIn] = (int*)calloc(tileLength, sizeof(int));
		tileB[indexIn] = (int*)calloc(tileLength, sizeof(int));		
		tileC[indexIn] = (int*)calloc(tileLength, sizeof(int));
	}
	sendBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);
	recvBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);
	//recv data
	MPI_Recv(recvBuffer, tileLength*tileLength, 
			MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	for(indexOut = 0; indexOut < tileLength; indexOut++)
	{
		for(indexIn = 0; indexIn < tileLength; indexIn++)
		{
			tileA[indexOut][indexIn] = recvBuffer[indexOut*length + indexIn];
		}
	}



	//prints tiles
	if(PRINTMATRIX)
	{
		printf("\n");
		//matrix A
		printf("Rank: %d\n", rank);
		printf("Tile A:\n");
		printMatrix(tileA, tileLength);
		//matrix B
		printf("Tile B:\n");
		printMatrix(tileB, tileLength);
		//matrix C
		printf("Tile C:\n");
		printMatrix(tileC, tileLength);
	}

	//free memory
	for(indexIn = 0; indexIn < tileLength; indexIn++)
	{
		free(tileA[indexIn]);
		free(tileB[indexIn]);
		free(tileC[indexIn]);
	}
	free(tileA);
	free(tileB);
	free(tileC);
	free(sendBuffer);
	free(recvBuffer);
}

void matrixMultipleSquare(int **matrixA, int**matrixB, int**matrixResult, int length)
{
	int indexIn, indexOut, indexSub;
	for(indexOut = 0; indexOut < length; indexOut++)
	{
		for(indexIn = 0; indexIn < length; indexIn++)
		{
			for(indexSub = 0; indexSub < length; indexSub++)
			{
				matrixResult[indexOut][indexIn] += matrixA[indexOut][indexSub]*matrixB[indexSub][indexIn];
			}
		}
	}
}

void printMatrix(int **matrix, int length)
{
	int indexIn, indexOut;
	for(indexOut = 0; indexOut < length; indexOut++)
	{
		for(indexIn = 0; indexIn < length; indexIn++)
		{
			printf("%8d", matrix[indexOut][indexIn]);
		}
		printf("\n");
	}
}
