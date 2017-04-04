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
	    masterCode(atoi(argv[1]),argv[2]);


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
 *  Detail: The master node will create an image char array.
 *          The image array is filled using calc pixel.
 *          The image array is then written to a file.
 *  
 */
void masterCode(int buckets, char* fileName)
{
    struct bucket * bucketArray;
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
    int bucketIndex, nextIndex;
    struct bucketNode * newNode;
    FILE *fin;
    struct timeval startTime, endTime, diffTime;
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
        result = fscanf(fin,"%d",&unsortedArray[indexIn]);
    }
    fclose(fin);
    //make buckets
    smallBuckets = (int **)malloc(sizeof(int*)*numBucket);
    for(indexOut = 0; indexOut < numBucket; indexOut++ )
    {
        smallBuckets[indexOut] = (int *)malloc(sizeof(int)*2*rowSize);
        smallBuckets[indexOut][0] = 0;
    }
    
    MPI_Barrier(MPI_COMM_WORLD); //sync 1
    //fill buckets
    
    for(indexOut = 0; indexOut < size; indexOut++ )
    {
        bucketIndex = unsortedArray[indexOut]/(MAXINT/ buckets);
        nextIndex = smallBuckets[bucketIndex][0] + 1;
        smallBuckets[bucketIndex][0]++;
        //smallBuckets[bucketIndex][nextIndex] = unsortedArray[indexOut]; 
        
    }
    
    MPI_Barrier(MPI_COMM_WORLD);//sync 2

    MPI_Barrier(MPI_COMM_WORLD);//sync 3
    
    //print buckets
    for(indexOut = 0; indexOut < buckets; indexOut++ )
    {
        printf("Tucket %d:", indexOut);
        for(indexIn = 0; indexIn < smallBuckets[indexOut][0]; indexIn++)
        {
            printf("%d ", smallBuckets[indexOut][indexIn+1]);
        }
        printf("\n");
    }
    free(sendBuffer);
    for(indexOut = 0; indexOut < numBucket; indexOut++)
    {
        free(smallBuckets[indexOut]);
    }
    free(smallBuckets);
    /*
    unsortedArray = (int *) malloc(arraySize * sizeof(int));
    //read the list
    for(index = 0; index < arraySize; index++)
    {
        result = fscanf(fin,"%d", &unsortedArray[index]);
    }
    
    fclose(fin);
    //make buckets
    bucketArray = makeBucket(numBucket);
    //start time
    gettimeofday(&startTime, NULL);
    //fill buckets
    for(index = 0; index < arraySize; index++)
    {
        bucketNumber = unsortedArray[index]/(1000 / numBucket);
        //push number
        newNode = (struct bucketNode *)malloc(sizeof(struct bucketNode));
        newNode->next = bucketArray[bucketNumber].front;
        newNode->data = unsortedArray[index];
        bucketArray[bucketNumber].front =  newNode;

    }
    */

    //print buckets
    /*
    for(index = 0; index < numBucket; index++)
    {
        printBucket(&bucketArray[index],index);
    }
    */
    //sort buckets
    /*
    for(index = 0; index < numBucket; index++)
    {
        sortBucket(&bucketArray[index]);
    }
    */
    //prints the sorted bucket
    /*
    for(index = 0; index < numBucket; index++)
    {
        printBucket(&bucketArray[index],index);
    }
    */
    /*
    //stop time
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 

    //prints result
    printf("%f,",elapsedTime );
    //free memory
    deleteBucket(bucketArray, numBucket);
    free(unsortedArray);
    */
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
    int size;
    int * unsortedArray;
    int indexIn, indexOut;
    int **smallBuckets;
    int bucketIndex, nextIndex;
    MPI_Status status;
    MPI_Recv(&size, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    unsortedArray = (int*)malloc(sizeof(int)*size);
    MPI_Recv(unsortedArray, size, MPI_INT, MASTER, ARRAYTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //for(indexOut = 0; indexOut < size; indexOut++)
    //{
    //    printf("%d ", unsortedArray[indexOut]);
    //}
    //printf("\n");
    
    //make buckets
    smallBuckets = (int **)malloc(sizeof(int*)*buckets);
    for(indexOut = 0; indexOut < buckets; indexOut++ )
    {
        smallBuckets[indexOut] = (int *)malloc(sizeof(int)*2*size);
        smallBuckets[indexOut][0] = 0;
    }
    
    MPI_Barrier(MPI_COMM_WORLD); //sync 1
    //fill buckets
    
    for(indexOut = 0; indexOut < size; indexOut++ )
    {
        bucketIndex = unsortedArray[indexOut]/(MAXINT/ buckets);
        nextIndex = smallBuckets[bucketIndex][0] + 1;
        smallBuckets[bucketIndex][0]++;
        
        //printf("%d %d %d\n", unsortedArray[indexOut], bucketIndex, nextIndex);
        //printf("%d\n",smallBuckets[bucketIndex][nextIndex]);
        smallBuckets[bucketIndex][nextIndex] = unsortedArray[indexOut]; 
        
    }
    //print buckets
    for(indexOut = 0; indexOut < buckets; indexOut++ )
    {
        printf("bucket %d:", indexOut);
        for(indexIn = 0; indexIn < smallBuckets[indexOut][0]; indexIn++)
        {
            printf("%d ", smallBuckets[indexOut][indexIn+1]);
        }
        printf("\n");
    }
    //all to all    
    MPI_Barrier(MPI_COMM_WORLD);//sync 2
    
    MPI_Barrier(MPI_COMM_WORLD);//sync 3
    free(unsortedArray);

    for(indexOut = 0; indexOut < buckets; indexOut++)
    {
        free(smallBuckets[indexOut]);
    }
    free(smallBuckets);
    //printf("%d got\n", size);
    //printf("hello from slave");
}


