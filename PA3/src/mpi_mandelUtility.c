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
#include "mpi_mandelUtility.h"
#ifdef _WIN32
  #define WRITE_FLAGS "wb"
#else
  #define WRITE_FLAGS "w"
#endif


//this function calculates the mandelbrot for a single pixel.
int sortBucket(struct bucket * b)
{
   int valueHolder;
   //printf("helloworld\n");
   struct bucketNode * currentPtr, * endOfListPtr, *holderPtr, *unsorted, *previousPtr;

   //endOfListPtr = b -> front;
   unsorted = b->front;
   b->front = NULL;
   while(unsorted != NULL)
   {
        //
        //holderPtr = endOfListPtr -> next;
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
