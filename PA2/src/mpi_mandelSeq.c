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
        printf("not enough arguments\n");
        printf("proper usage:\n srun mpi_mandelSeq <width> <height> <mpi arguements>");
        return 0;
    }
    
    //preprocessing
    int numtasks, taskid, len;
    int displayWidth, displayHeight;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    float elapsedTime = 0;
    struct timeval startTime, endTime, diffTime;
    displayWidth = atoi(argv[1]);
    displayHeight = atoi(argv[2]);

    timerclear(&diffTime);

    //parallel start
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Get_processor_name(hostname, &len);

    //the printf here is only used for debugging
    //printf ("Hello from task %d on %s!\n", taskid, hostname);

    //Iterator indexs is for message size
    timerclear(&diffTime);

    if (taskid == MASTER)
    {
	    //master code
        //
	    gettimeofday(&startTime, NULL); //start clock
        //send to slave
	    masterCode(displayWidth, displayHeight);

	    timersub(&endTime, &startTime, &diffTime); //calc diff time
        //converts time struct to float
	    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 
        //prints result in CSV format
	    printf("%f,",elapsedTime );
        printf("\n");
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


void masterCode(int width, int height)
{
    char **image;
    int indexOut, indexIn;
    struct complex number;

    image = (char**)malloc(height* sizeof(char*));
    for(indexOut = 0; indexOut < height; indexOut++)
    {
        image[indexOut] = (char*)malloc(width * sizeof(char));
    }
    for(indexOut = 0; indexOut < height; indexOut++)
    {
        for(indexIn = 0; indexIn < width; indexIn++)
        {
            number.real = -2. + indexOut/((float)height)*4.;
            number.imag = -2. + indexIn/((float)width)*4.;
            image[indexOut][indexIn] = (char) (calc_Pixel(number) % 256);
            //printf("%d", image[indexOut][indexIn]);
        }
        //printf("\n");
    }
    
    pim_write_color4("outimage.pim", width, height,
                    (const unsigned char**)image,
                    (const unsigned char**)image, 
                    (const unsigned char**)image);

    for(indexOut = 0; indexOut < height; indexOut++)
    {
        free(image[indexOut]);
    }
    free(image);

    printf("hello from master");

}

void slaveCode(int width, int height)
{
    printf("hello from slave");
}


