/******************************************************************************
* FILE: mpi_bucketUtility.h
* DESCRIPTION:
*   Declarations of utility functions for bucket sort
* AUTHOR: Evan Su
* LAST REVISED: 04/5/17
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
struct bucket* makeBucket(int);
void deleteBucket(struct bucket *, int);
#endif
