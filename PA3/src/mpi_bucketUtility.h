/******************************************************************************
* FILE: mpi_mandelUtility.h
* DESCRIPTION:
*   Declarations of utility functions for mandelbrot
* AUTHOR: Fred Harris (original code)
*         Evan Su (modified for c use)
* LAST REVISED: 03/13/17
******************************************************************************/

//compiling directives

#ifndef __PIMFUNCS_H__
#define __PIMFUNCS_H__

//complex data struct
struct bucketNode
{
    struct bucketNode * next;
    int data;
};
struct bucket
{
    struct bucketNode * front;
};


int sortBucket(struct bucket *);
void printBucket(struct bucket *, int);
struct bucket * makeBucket(int numBucket)
void deleteBucket();
#endif
