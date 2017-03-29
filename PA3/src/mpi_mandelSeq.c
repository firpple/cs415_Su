/******************************************************************************
* FILE: mpi_mandelSeq.c
* DESCRIPTION:
*   Calculates the mandelbrot image sequential. 
* AUTHOR: Evan Su
* LAST REVISED: 03/13/17
******************************************************************************/
//libraries
#include "mpi.h"
#include "mpi_mandelUtility.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

//define constants
#define  MASTER		0
#define  SLAVE      1
#define  TAG        0
#define  SECTOMICRO 1000000

//function declarations
void masterCode(char*);
void slaveCode(char*);

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
	    masterCode(argv[1]);


    }
    else
    {
        //slave code
        //this code does nothing, placeholder for parallel.
	    slaveCode(argv[1]);
    }

    MPI_Finalize();
    return 0;



}

/*
 *  Function name: masterCode
 *  
 *  Brief: Mandelbrot code for the master node
 *  
 *  Detail: The master node will create an image char array.
 *          The image array is filled using calc pixel.
 *          The image array is then written to a file.
 *  
 */
void masterCode(char* fileName)
{
    struct bucket * bucketArray;
    int numBucket = 10;
    int arraySize;
    int *unsortedArray;
    int index;
    int result;
    int bucketNumber;
    struct bucketNode * newNode;
    struct bucketNode * ptrNode;
    FILE *fin;
    fin = fopen(fileName, "r");
    //get array
    result = fscanf(fin,"%d",&arraySize);
    unsortedArray = (int *) malloc(arraySize * sizeof(int));

    for(index = 0; index < arraySize; index++)
    {
        result = fscanf(fin,"%d", &unsortedArray[index]);
    }

    for(index = 0; index < arraySize; index++)
    {
        //printf("%d ", unsortedArray[index]);
    }
    printf("\n");
    //make buckets
    bucketArray = (struct bucket *) malloc(numBucket*sizeof(struct bucket)); 
    for(index = 0; index < numBucket; index++)
    {
        bucketArray[index].front = NULL;
    }
    //fill buckets
    for(index = 0; index < arraySize; index++)
    {
        bucketNumber = unsortedArray[index]/(1000 / numBucket);
        printf("%d %d\n",bucketNumber,unsortedArray[index]);
        //push number
        newNode = (struct bucketNode *)malloc(sizeof(struct bucketNode));
        newNode->next = bucketArray[bucketNumber].front;
        newNode->data = unsortedArray[index];
        bucketArray[bucketNumber].front =  newNode;

    }

    for(index = 0; index < numBucket; index++)
    {
        ptrNode = bucketArray[index].front;
        printf("Bucket %d:",index);
        while(ptrNode != NULL)
        {
            printf("%d ", ptrNode->data);
            ptrNode = ptrNode -> next;
        }
        printf("\n");
    }
    //sort buckets


    free(bucketArray);
    free(unsortedArray);
}

/*
 *  Function name: slaveCode
 *  
 *  Brief: Mandelbrot code for the slave node
 *  
 *  Detail: The slave node does nothing in sequential calculations
 *  
 */
void slaveCode(char* fileName)
{
    //printf("hello from slave");
}


