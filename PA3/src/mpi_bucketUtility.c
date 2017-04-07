/******************************************************************************
* FILE: mpi_bucketUtility.c
* DESCRIPTION:
*   Implementations of utility functions for bucket sort
* AUTHOR: Evan Su (modified for c usage)
* LAST REVISED: 04/5/17
******************************************************************************/
//libraries and complier directives
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "mpi_bucketUtility.h"
#ifdef _WIN32
  #define WRITE_FLAGS "wb"
#else
  #define WRITE_FLAGS "w"
#endif

#define  SECTOMICRO 1000000
//sorts a single bucket
int sortBucket(struct bucket * b)
{
    struct bucketNode * currentPtr, *holderPtr, *unsorted, *previousPtr;

   unsorted = b->front;
   b->front = NULL;
   //sorts the list using insertion sort
   while(unsorted != NULL)
   {
        holderPtr = unsorted;
        unsorted = unsorted -> next;
        holderPtr-> next = NULL;
        if(b->front == NULL)
        {
            //insert on the front of list
            b->front = holderPtr;
        }
        else
        {
            //finds the next place to insert the item
            currentPtr = b-> front;
            previousPtr = NULL;
            //cycles through the sorted list
            while(currentPtr != NULL && (currentPtr -> data) < (holderPtr -> data) )
            {
                previousPtr = currentPtr;
                currentPtr = currentPtr -> next;
            }
            //inserts the element in the correct location
            if(currentPtr == b->front)
            {
                b->front = holderPtr;
                holderPtr -> next = currentPtr;
            }
            else
            {
                previousPtr -> next = holderPtr;
                holderPtr -> next = currentPtr;
            }
        }



   }
    free(sortArray);
	/*
    //bubble sort, currently disabled
    int size, holder;
    int index, indexIn;
    struct bucketNode * firstPtr;//, *secondPtr;
    int * sortArray;
    int debugcount = 0;
    
    struct timeval endTime, diffTime;
    struct timeval sortTime;
    float elapsedTime;
    

    //determine size
    size = 0;
    firstPtr = b-> front;
    while(firstPtr != NULL)
    {
        firstPtr = firstPtr -> next;
        size++;
    }
	sortArray = (int*) malloc(sizeof(int) * size);
	firstPtr = b->front;
    //pulls items from bucket to array
	for(index = 0; index <size; index++)
	{
		sortArray[index]= firstPtr -> data;
		//printf("%d ", sortArray[index]);
		firstPtr = firstPtr -> next; 
	}
	//printf("\n");

    //sorts array
    gettimeofday(&sortTime, NULL);
    for(index = 0; index < size; index++)
    {
        for(indexIn = 0; indexIn < size - 1; indexIn++)
		{
			if(sortArray[indexIn] > sortArray[indexIn + 1])
			{
				holder = sortArray[indexIn];
				sortArray[indexIn] = sortArray[indexIn + 1];
				sortArray[indexIn + 1] = holder;
                debugcount++;
			}
		}
    }
    gettimeofday(&endTime, NULL);
    //calculates sort time exculsively.        
    timersub(&endTime, &sortTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 
	//printf("[%f for count of %d,]",elapsedTime, debugcount );

    puts the items in the array to the buckets
	firstPtr = b->front;
	for(index = 0; index <size; index++)
	{
		firstPtr -> data = sortArray[index];
		//printf("%d ", sortArray[index]);
		firstPtr = firstPtr -> next; 
	}
	free(sortArray);*/
	
   return 0;
}

//prints a single bucket
void printBucket(struct bucket * currentBucket, int bucketNumber)
{

    struct bucketNode * ptrNode;
    ptrNode = currentBucket->front;
    printf("Bucket %d:",bucketNumber);
    while(ptrNode != NULL)
    {
        printf("%d ", ptrNode->data);
        ptrNode = ptrNode -> next;
    }
    printf("\n");
}
//makes an array of buckets
struct bucket* makeBucket(int numBucket)
{
    int index;
    struct bucket * bucketArray;
    
    bucketArray = (struct bucket *) malloc(numBucket*sizeof(struct bucket)); 
    for(index = 0; index < numBucket; index++)
    {
        bucketArray[index].front = NULL;
    }
    return bucketArray;
}
//delete an array of buckets
void deleteBucket(struct bucket * bucketArray, int numBucket)
{

    int index;
    struct bucketNode * ptrNode;
    for(index = 0; index < numBucket; index++)
    {
        ptrNode = bucketArray[index].front;
        while(ptrNode != NULL)
        {
            //frees each node that exist
            bucketArray[index].front= ptrNode->next;
            free(ptrNode);
            ptrNode = bucketArray[index].front;

        }
        //bucketArray[index]
    }
    free(bucketArray);
}
