/******************************************************************************
* FILE: mpi_packetCheck.c
* DESCRIPTION:
*   MPI Send and recieved. The program outputs the ellapsed time between mpi 
*   send and mpi receive for increasing message sizes. The ellapsed time is
*   measured in microseconds
* AUTHOR: Evan Su
* LAST REVISED: 02/19/17
******************************************************************************/
#include "mpi.h"
#include "mpi_mandelUtility.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#define  MASTER		0
#define  SLAVE      1
#define  TAG        0
#define  SECTOMICRO 1000000


void masterCode(int, int);
void slaveCode(int, int);

int main (int argc, char *argv[])
{
    //argc checker
    if(argc < 2)
    {
        //there are not enough arugments
        //note: this will not check if the arguements are in the correct positions
        printf("not enough arguments\n");
        printf("proper usage:\n srun mpi_mandelSeq <width> <height> <mpi arguements>");
        return 0;
    }
    
    //preprocessing
    int numtasks, taskid, len;
    int displayWidth, displayHeight;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    displayWidth = atoi(argv[1]);
    displayHeight = atoi(argv[2]);


    //parallel start
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Get_processor_name(hostname, &len);


    if (taskid == MASTER)
    {
	    //master code
	    masterCode(displayWidth, displayHeight);


    }
    else
    {
        //slave code
        //this code does nothing, placeholder for parallel.
	    slaveCode(displayWidth, displayHeight);
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
void masterCode(int width, int height)
{
    //initialize variables
    char **image;
    int indexOut, indexIn;
    struct complex number;
    struct timeval startTime, endTime, diffTime;
    float elapsedTime = 0;

    //creates image array
    image = (char**)malloc(height* sizeof(char*));
    for(indexOut = 0; indexOut < height; indexOut++)
    {
        image[indexOut] = (char*)malloc(width * sizeof(char));
    }

	gettimeofday(&startTime, NULL); //start clock
    //begin calculations for image
    for(indexOut = 0; indexOut < height; indexOut++)
    {
        for(indexIn = 0; indexIn < width; indexIn++)
        {
            number.real = -2. + indexOut/((float)height)*4.;
            number.imag = -2. + indexIn/((float)width)*4.;
            image[indexOut][indexIn] = (char) (calc_Pixel(number) % 256);
        }
    }
    //finish calculations for image
	gettimeofday(&endTime, NULL); //end clock
    timersub(&endTime, &startTime, &diffTime); //calc diff time

    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 
    //prints result 
    printf("time:\t%f\n",elapsedTime );
    //this function will write the mandelbrot in cyan color
    pim_write_color4("outimage.pim", width, height,
                    (const unsigned char**)image,
                    (const unsigned char**)image, 
                    (const unsigned char**)image);

    for(indexOut = 0; indexOut < height; indexOut++)
    {
        free(image[indexOut]);
    }
    free(image);

    //printf("hello from master");

}

/*
 *  Function name: slaveCode
 *  
 *  Brief: Mandelbrot code for the slave node
 *  
 *  Detail: The slave node does nothing in sequential calculations
 *  
 */
void slaveCode(int width, int height)
{
    //printf("hello from slave");
}

