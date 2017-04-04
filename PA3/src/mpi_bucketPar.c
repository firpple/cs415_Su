/******************************************************************************
* FILE: mpi_bucketSeq.c
* DESCRIPTION:
*   Calculates the bucketsort image sequential. 
* AUTHOR: Evan Su
* LAST REVISED: 03/13/17
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
	    masterCode(atoi(argv[1]),argv[2]);


    }
    else
    {
        //slave code
        //this code does nothing, placeholder for parallel.
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
 *  Detail: The master node will create an image char array.
 *          The image array is filled using calc pixel.
 *          The image array is then written to a file.
 *  
 */
void masterCode(int buckets, char* fileName)
{
    struct bucket * bucketArray;
    struct bucket * bucketPtr;
    int numBucket = buckets;
    int arraySize;
    int *unsortedArray;
    int *sendBuffer;
    int index;
    int result;
    int bucketNumber;
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
        fclose(fin);
        rowSize = -1;
        for(indexOut = 1; indexOut < buckets; indexOut++)
        {
            MPI_Send(&rowSize, 1, MPI_INT, indexOut, TAG, MPI_COMM_WORLD);
        }
        //printf("rowSize: %d\n", rowSize);
        return;
    }
    
    rowSize = arraySize/numBucket;
    sendBuffer = (int*)malloc(sizeof(int)*rowSize);
    //send rows out
    for(indexOut = 1; indexOut < numBucket; indexOut++)
    {
        MPI_Send(&rowSize, 1, MPI_INT, indexOut, TAG, MPI_COMM_WORLD);
        for(indexIn = 0; indexIn < rowSize; indexIn++)
        {
            result = fscanf(fin,"%d",&sendBuffer[indexIn]);
        }
        MPI_Send(sendBuffer, rowSize, MPI_INT, indexOut, ARRAYTAG, MPI_COMM_WORLD);
    }
    //
    size = arraySize - rowSize* (numBucket- 1);
    unsortedArray = (int*)malloc(sizeof(int)*size);
    for(indexOut = 0; indexOut < size; indexOut ++)
    {
        result = fscanf(fin,"%d",&unsortedArray[indexOut]);
        printf("{%d}", indexOut);
    }
    fclose(fin);
    //make buckets
    bucketPtr = (struct bucket*)malloc(sizeof(struct bucket));
    bucketPtr -> front = NULL;
    smallBuckets = (int **)malloc(sizeof(int*)*numBucket);
    for(indexOut = 0; indexOut < numBucket; indexOut++ )
    {
        smallBuckets[indexOut] = (int *)malloc(sizeof(int)*2*rowSize);
        smallBuckets[indexOut][0] = 0;
    }
    
    sendBuckets = (int *)malloc(sizeof(int)*numBucket * 2* rowSize);
    recvBuckets = (int *)malloc(sizeof(int)*numBucket * 2* rowSize);

    MPI_Barrier(MPI_COMM_WORLD); //sync 1
    //printf("started time\n");
    //start time
    gettimeofday(&startTime, NULL);
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
        for(indexIn = 0; indexIn < recvBuckets[indexOut*2*rowSize]; indexIn++)
        {
            //printf("%d ", recvBuckets[indexOut*2*rowSize + indexIn +1]);
            newNode = (struct bucketNode *)malloc(sizeof(struct bucketNode));
            newNode->next = bucketPtr->front;
            newNode->data = recvBuckets[indexOut*2*rowSize + indexIn +1];
            bucketPtr->front =  newNode;
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);//sync 2
    
    gettimeofday(&sortTime, NULL);
    sortBucket(bucketPtr);
    //printBucket(bucketPtr, 0);

    MPI_Barrier(MPI_COMM_WORLD);//sync 3
    //stop time
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 

    //prints result
    printf("%f,",elapsedTime );

    //sort time
    timersub(&endTime, &sortTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 

    //prints result
    printf("%f,",elapsedTime );
    
    //print buckets

    printBucket(bucketPtr, 0);
    /*for(indexOut = 0; indexOut < buckets; indexOut++ )
    {
        //printf("Tucket %d:", indexOut);
        for(indexIn = 0; indexIn < smallBuckets[indexOut][0]; indexIn++)
        {
        //    printf("%d ", smallBuckets[indexOut][indexIn+1]);
        }
        //printf("\n");
    }

    for(indexOut = 0; indexOut < numBucket; indexOut++ )
    {
        //printf("zucket %d:", indexOut);
        
        for(indexIn = 0; indexIn < recvBuckets[indexOut*2*rowSize]; indexIn++)
        {
        //    printf("%d ", recvBuckets[indexOut*2*rowSize + indexIn +1]);
        }
        //printf("\n");
    }
*/


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
 *  Detail: The slave node does nothing in sequential calculations
 *  
 */
void slaveCode(int buckets, char* fileName, int rank)
{
    
    struct bucket * bucketPtr;
    struct bucketNode * newNode;
    int size;
    int * unsortedArray;
    int indexIn, indexOut;
    int **smallBuckets;
    int *recvBuckets;
    int *sendBuckets;
    int currentIndex;
    int bucketIndex, nextIndex;
    int numBucket = buckets;
    MPI_Status status;


    MPI_Recv(&size, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    unsortedArray = (int*)malloc(sizeof(int)*size);
    MPI_Recv(unsortedArray, size, MPI_INT, MASTER, ARRAYTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    
    //make buckets
    bucketPtr = (struct bucket*)malloc(sizeof(struct bucket));
    bucketPtr -> front = NULL;
    smallBuckets = (int **)malloc(sizeof(int*)*buckets);
    for(indexOut = 0; indexOut < buckets; indexOut++ )
    {
        smallBuckets[indexOut] = (int *)malloc(sizeof(int)*2*size);
        smallBuckets[indexOut][0] = 0;
    }
    sendBuckets = (int *)malloc(sizeof(int)*buckets*2*size);
    recvBuckets = (int *)malloc(sizeof(int)*buckets*2*size);
    
    MPI_Barrier(MPI_COMM_WORLD); //sync 1
    //fill buckets
    
    for(indexOut = 0; indexOut < size; indexOut++ )
    {
        bucketIndex = (int)(unsortedArray[indexOut]/(MAXINT/ (float) buckets));
        nextIndex = smallBuckets[bucketIndex][0] + 1;
        //smallBuckets[bucketIndex][0]++;
        
        //printf("%d %d %d\n", unsortedArray[indexOut], bucketIndex, nextIndex);
        //printf("%d\n",smallBuckets[bucketIndex][nextIndex]);
        //smallBuckets[bucketIndex][nextIndex] = unsortedArray[indexOut]; 
        
    }
    
//MPI_Recv(unsortedArray, size, MPI_INT, MASTER, ARRAYTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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

    MPI_Barrier(MPI_COMM_WORLD);//sync 2
    
    //put in big bucket;
    for(indexOut = 0; indexOut < numBucket; indexOut++ )
    {
        for(indexIn = 0; indexIn < recvBuckets[indexOut*2*size]; indexIn++)
        {
            //printf("%d ", recvBuckets[indexOut*2*rowSize + indexIn +1]);
            newNode = (struct bucketNode *)malloc(sizeof(struct bucketNode));
            newNode->next = bucketPtr->front;
            newNode->data = recvBuckets[indexOut*2*size + indexIn +1];
            bucketPtr->front =  newNode;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);//sync 3
    
    sortBucket(bucketPtr);
    printBucket(bucketPtr, rank);
    

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
    //printf("%d got\n", size);
    //printf("hello from slave");
}


