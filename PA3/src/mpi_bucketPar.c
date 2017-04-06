/******************************************************************************
* FILE: mpi_bucketSeq.c
* DESCRIPTION:
*   Calculates the bucketsort image sequential. 
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
#define  ARRAYTAG   1
#define  SECTOMICRO 1000000
#define  MAXINT     1000
#define  PRINT      0
#define SORTONLY 	1
//function declarations
void masterCode(int, char*);
void slaveCode(int, char*, int);

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
	    masterCode(atoi(argv[1]),argv[2]);


    }
    else
    {
        //slave code
	    slaveCode(atoi(argv[1]),argv[2], taskid);
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
 *  
 */
void masterCode(int buckets, char* fileName)
{
    //struct bucket * bucketArray;
    struct bucket * bucketPtr;
    int numBucket = buckets;
    int arraySize;
    int *unsortedArray;
    int *sendBuffer;
    //int index;
    int result;
    //int bucketNumber;
    int rowSize;
    int indexIn, indexOut;
    int size;
    int ** smallBuckets;
    int * recvBuckets;
    int * sendBuckets;
    int bucketIndex, nextIndex;
    int currentIndex;
    struct bucketNode * newNode;
    FILE *fin;
    struct timeval startTime, endTime, diffTime;
    struct timeval sortTime;
    float elapsedTime = 0;

    fin = fopen(fileName, "r");
    //get array
    //read the size of list
    result = fscanf(fin,"%d",&arraySize);
    if(result == 0)
    {
        //if fscanf was unable to read the file, send a kill command 
        //to all processes
        fclose(fin);
        rowSize = -1;
        for(indexOut = 1; indexOut < buckets; indexOut++)
        {
            MPI_Send(&rowSize, 1, MPI_INT, indexOut, TAG, MPI_COMM_WORLD);
        }
        return;
    }
    
    rowSize = arraySize/numBucket;
    sendBuffer = (int*)malloc(sizeof(int)*rowSize);
    //send rows out
    for(indexOut = 1; indexOut < numBucket; indexOut++)
    {
        //sends the size of the buffer to the slave
        MPI_Send(&rowSize, 1, MPI_INT, indexOut, TAG, MPI_COMM_WORLD);
        for(indexIn = 0; indexIn < rowSize; indexIn++)
        {
            //reads the numbers into a buffer
            result = fscanf(fin,"%d",&sendBuffer[indexIn]);
        }
        //sends the buffer to the slave
        MPI_Send(sendBuffer, rowSize, MPI_INT, indexOut, ARRAYTAG, MPI_COMM_WORLD);
    }
    //the remaining rows are for the master
    size = arraySize - rowSize* (numBucket- 1);
    unsortedArray = (int*)malloc(sizeof(int)*size);
    for(indexOut = 0; indexOut < size; indexOut ++)
    {
        //reads numbers into the master's set
        result = fscanf(fin,"%d",&unsortedArray[indexOut]);
    }
    fclose(fin);
    //make buckets
    bucketPtr = makeBucket(1);
    smallBuckets = (int **)malloc(sizeof(int*)*numBucket);
    for(indexOut = 0; indexOut < numBucket; indexOut++ )
    {
        smallBuckets[indexOut] = (int *)malloc(sizeof(int)*2*rowSize);
        smallBuckets[indexOut][0] = 0;
    }
    
    sendBuckets = (int *)malloc(sizeof(int)*numBucket * 2* rowSize);
    recvBuckets = (int *)malloc(sizeof(int)*numBucket * 2* rowSize);

    MPI_Barrier(MPI_COMM_WORLD); //sync begin sorting
    //start time
    gettimeofday(&startTime, NULL);
    MPI_Barrier(MPI_COMM_WORLD); //sync timing has begun
    //fill small buckets
    for(indexOut = 0; indexOut < size; indexOut++ )
    {
        bucketIndex = (int)(unsortedArray[indexOut]/(MAXINT/ (float) buckets));
        nextIndex = smallBuckets[bucketIndex][0] + 1;
        smallBuckets[bucketIndex][0]++;
        smallBuckets[bucketIndex][nextIndex] = unsortedArray[indexOut]; 
        
    }
    //ready buckets to send by transfering them to a 1d array
    currentIndex = 0;
    for(indexOut = 0; indexOut < numBucket; indexOut++ )
    {
        for(indexIn = 0; indexIn < 2*rowSize; indexIn++ )
        {
            sendBuckets[currentIndex] = smallBuckets[indexOut][indexIn];
            currentIndex++;
        }
    }
    //all to all
    MPI_Alltoall(sendBuckets,2*rowSize,MPI_INT,
                 recvBuckets, 2*rowSize, MPI_INT,
                 MPI_COMM_WORLD);
    //put in big bucket;
    for(indexOut = 0; indexOut < numBucket; indexOut++ )
    {
        for(indexIn = 0; indexIn < recvBuckets[((indexOut +1)%numBucket)*2*rowSize]; indexIn++)
        {
            newNode = (struct bucketNode *)malloc(sizeof(struct bucketNode));
            newNode->next = bucketPtr->front;
            newNode->data = recvBuckets[((indexOut +1)%numBucket)*2*rowSize + indexIn +1];
            bucketPtr->front =  newNode;
        }
    }
    
    if(PRINT)
    {
        printBucket(bucketPtr, 0);
    }
    //start sorting
    gettimeofday(&sortTime, NULL);
    sortBucket(&bucketPtr[0]);
	gettimeofday(&endTime, NULL);
	
	if(SORTONLY)
	{
        //calculates sort time exculsively.        
        timersub(&endTime, &sortTime, &diffTime); //calc diff time
        //converts time struct to float
        elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 
    	printf("%f,",elapsedTime );
	}    

    MPI_Barrier(MPI_COMM_WORLD);//sync sorting done
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
    
    //print buckets
    if(PRINT)
    {
        printBucket(bucketPtr, 0);
    }


    //free memory
    free(sendBuffer);
    for(indexOut = 0; indexOut < numBucket; indexOut++)
    {
        free(smallBuckets[indexOut]);
    }
    free(smallBuckets);
    free(sendBuckets);
    free(recvBuckets);
    deleteBucket(bucketPtr, 1);
}

/*
 *  Function name: slaveCode
 *  
 *  Brief: bucketsort code for the slave node
 *  
 *  Detail: The slave recieves the unsorted array with its size
 *          The slave puts bucket sorts the array
 *  
 */
void slaveCode(int buckets, char* fileName, int rank)
{
    
    struct bucket * bucketPtr;
    struct bucketNode * newNode;

    struct timeval endTime, diffTime;
    struct timeval sortTime;
	float elapsedTime;
    int size;
    int * unsortedArray;
    int indexIn, indexOut;
    int **smallBuckets;
    int *recvBuckets;
    int *sendBuckets;
    int currentIndex;
    int bucketIndex, nextIndex;
    int numBucket = buckets;
    //MPI_Status status;


    MPI_Recv(&size, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    unsortedArray = (int*)malloc(sizeof(int)*size);
    
    MPI_Recv(unsortedArray, size, MPI_INT, MASTER, ARRAYTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    
    //make buckets
    bucketPtr =  makeBucket(1);
    smallBuckets = (int **)malloc(sizeof(int*)*buckets);
    for(indexOut = 0; indexOut < buckets; indexOut++ )
    {
        smallBuckets[indexOut] = (int *)malloc(sizeof(int)*2*size);
        smallBuckets[indexOut][0] = 0;
    }
    sendBuckets = (int *)malloc(sizeof(int)*buckets*2*size);
    recvBuckets = (int *)malloc(sizeof(int)*buckets*2*size);
    
    MPI_Barrier(MPI_COMM_WORLD); //sync begin sorting
    MPI_Barrier(MPI_COMM_WORLD); //sync timing has begun
    //fill buckets
    
    for(indexOut = 0; indexOut < size; indexOut++ )
    {
        bucketIndex = (int)(unsortedArray[indexOut]/(MAXINT/ (float) buckets));
        nextIndex = smallBuckets[bucketIndex][0] + 1;
        smallBuckets[bucketIndex][0]++;
        smallBuckets[bucketIndex][nextIndex] = unsortedArray[indexOut]; 
        
    }
    
    //ready buckets to send
    currentIndex = 0;
    for(indexOut = 0; indexOut < buckets; indexOut++ )
    {
        for(indexIn = 0; indexIn < 2*size; indexIn++ )
        {
            sendBuckets[currentIndex] = smallBuckets[indexOut][indexIn];
            currentIndex++;
        }
    }
    //all to all    
    MPI_Alltoall(sendBuckets, 2*size, MPI_INT,
                 recvBuckets, 2*size, MPI_INT,
                 MPI_COMM_WORLD);

    
    //put in big bucket;
    for(indexOut = 0; indexOut < numBucket; indexOut++ )
    {
        for(indexIn = 0; indexIn < recvBuckets[((indexOut +1)%numBucket)*2*size]; indexIn++)
        {
            //printf("%d ", recvBuckets[indexOut*2*rowSize + indexIn +1]);
            newNode = (struct bucketNode *)malloc(sizeof(struct bucketNode));
            newNode->next = bucketPtr->front;
            newNode->data = recvBuckets[((indexOut +1)%numBucket)*2*size + indexIn +1];
            bucketPtr->front =  newNode;
        }
    }
    if(PRINT)
    {
        printBucket(bucketPtr, rank);
    }
	
	gettimeofday(&sortTime, NULL);   
    sortBucket(bucketPtr);
	gettimeofday(&endTime, NULL);


    timersub(&endTime, &sortTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 
    //prints result
	
	if(SORTONLY)
	{
    	printf("%f,",elapsedTime);
	}
    if(PRINT)
    {
        printBucket(bucketPtr, rank);
    }
    MPI_Barrier(MPI_COMM_WORLD);//sync 3 sorting done
    //free memory
    
    free(unsortedArray);

    for(indexOut = 0; indexOut < buckets; indexOut++)
    {
        free(smallBuckets[indexOut]);
    }
    free(smallBuckets);
    
    free(sendBuckets);
    free(recvBuckets);
    deleteBucket(bucketPtr, 1);
}


