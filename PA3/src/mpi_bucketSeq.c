/******************************************************************************
* FILE: mpi_bucketSeq.c
* DESCRIPTION:
*   Calculates the bucketsort sequential. 
* AUTHOR: Evan Su
* LAST REVISED: 04/5/17
******************************************************************************/
//libraries
#include "mpi.h"
#include "mpi_bucketUtility.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

//define constants
#define  MASTER		0
#define  SLAVE      1
#define  TAG        0
#define  SECTOMICRO 1000000
#define  PRINT      0
#define SORTONLY 	1
#define MAXSIZE 1000
//function declarations
void masterCode(int, char*);
void slaveCode(int, char*);

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
	    masterCode(atoi(argv[1]),atoi(argv[2]));


    }
    else
    {
        //slave code
        //this code does nothing, placeholder for parallel.
	    slaveCode(atoi(argv[1]),argv[2]);
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
void masterCode(int buckets, int genSize)
{
    struct bucket * bucketArray;
    int numBucket = buckets;
    int arraySize;
    int *unsortedArray;
    int index;
    int result;
    int bucketNumber;
    struct bucketNode * newNode;
    //FILE *fin;
    struct timeval startTime, endTime, diffTime;
    struct timeval sortTime;
    float elapsedTime = 0;

    //fin = fopen(fileName, "r");
    //get array
    //read the size of list
    result = genSize
    if(result == 0)
    {
        //fclose(fin);
        return;
    }
    unsortedArray = (int *) malloc(arraySize * sizeof(int));
    //read the list
    srand(0);
    for(index = 0; index < arraySize; index++)
    {
        unsortedArray[index] = rand()% MAXSIZE
    }
    
    //fclose(fin);
    //make buckets
    bucketArray = makeBucket(numBucket);
    //start time
    gettimeofday(&startTime, NULL);
    //fill buckets
    for(index = 0; index < arraySize; index++)
    {
        bucketNumber = (int)(unsortedArray[index]/(1000/ (float) numBucket));
        //push number
        newNode = (struct bucketNode *)malloc(sizeof(struct bucketNode));
        newNode->next = bucketArray[bucketNumber].front;
        newNode->data = unsortedArray[index];
        bucketArray[bucketNumber].front =  newNode;

    }
    if(PRINT)
    {
        for(index = 0; index < numBucket; index++)
        {
            printBucket(&bucketArray[index],index);
        }
    }    
    
    //sort buckets
    for(index = 0; index < numBucket; index++)
    {
		
    	gettimeofday(&sortTime, NULL);
        sortBucket(&bucketArray[index]);

		gettimeofday(&endTime, NULL);


		timersub(&endTime, &sortTime, &diffTime); //calc diff time
		//converts time struct to float
		elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 
		//prints sort time
		if(SORTONLY)
		{
			printf("%f,",elapsedTime );
		}
    }
    //stop time
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 

    //prints result
	if(!SORTONLY)
	{
    	printf("%f,",elapsedTime );
	}
    //calc sort time
    timersub(&endTime, &sortTime, &diffTime); //calc diff time
    //converts time struct to float

    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 

    //prints the sorted bucket
    if(PRINT)
    {
        for(index = 0; index < numBucket; index++)
        {
            printBucket(&bucketArray[index],index);
        }
    }    
    


    //free memory
    deleteBucket(bucketArray, numBucket);
    free(unsortedArray);
}

/*
 *  Function name: slaveCode
 *  
 *  Brief: bucketsort code for the slave node
 *  
 *  Detail: The slave node does nothing in sequential calculations
 *  
 */
void slaveCode(int buckets, char* fileName)
{
    //printf("hello from slave");
}


