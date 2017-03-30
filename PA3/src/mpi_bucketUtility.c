/******************************************************************************
* FILE: mpi_mandelUtility.c
* DESCRIPTION:
*   Implementations of utility functions for mandelbrot
* AUTHOR: Fred Harris (original code)
*         Evan Su (modified for c usage)
* LAST REVISED: 03/13/17
******************************************************************************/
//libraries and complier directives
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi_bucketUtility.h"
#ifdef _WIN32
  #define WRITE_FLAGS "wb"
#else
  #define WRITE_FLAGS "w"
#endif


//this function calculates the mandelbrot for a single pixel.
int sortBucket(struct bucket * b)
{
   struct bucketNode * currentPtr, *holderPtr, *unsorted, *previousPtr;

   unsorted = b->front;
   b->front = NULL;
   while(unsorted != NULL)
   {
        holderPtr = unsorted;
        unsorted = unsorted -> next;
        holderPtr-> next = NULL;
        if(b->front == NULL)
        {
            //
            b->front = holderPtr;
        }
        else
        {
            currentPtr = b-> front;
            previousPtr = NULL;
            while(currentPtr != NULL && (currentPtr -> data) < (holderPtr -> data) )
            {
                previousPtr = currentPtr;
                currentPtr = currentPtr -> next;
            }
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

   return 0;
}


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

void deleteBucket(struct bucket * bucketArray, int size)
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
