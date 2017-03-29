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


//this function calculates t *he mandelbrot for a single pixel.
int sortBucket(struct bucket b)
{
   int count, max_iter;
   printf("helloworld\n");
   return 0;
}
