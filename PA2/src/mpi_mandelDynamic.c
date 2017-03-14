/******************************************************************************
* FILE: mpi_mandelDynamic.c
* DESCRIPTION:
*   Calculates the mandelbrot image using dynamic task assignment. 
* AUTHOR: Evan Su
* LAST REVISED: 03/13/17
******************************************************************************/
//libraries
#include "mpi.h"
#include "mpi_mandelUtility.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

//Define constants
#define  MASTER		0
#define  SLAVE      1
#define  TAG        0
#define  ROWNUMTAG        1
#define  ROWARRTAG        2
#define  SECTOMICRO 1000000
#define  NOTDONE    0
#define  DONE       1
#define  ALLDONE    -1
#define  BADPIXEL   -1

//Function declarations
void masterCode(int, int, int, int);
void slaveCode(int, int, int, int);

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
 */
int main (int argc, char *argv[])
{
    //argc checker
    if(argc < 2)
    {
        //there are not enough arugments
        //note: this will not check if the arguements are in the correct positions
        printf("not enough arguments\n");
        printf("proper usage:\n srun mpi_mandelSeq <width> <height> <mpi arguements>\n");
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
	    masterCode(displayWidth, displayHeight,taskid,numtasks);


    }
    else
    {
        //slave code
	    slaveCode(displayWidth, displayHeight,taskid,numtasks);
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
 *          Row numbers are sent to each of the slaves.
 *          Master waits for slaves to give back the row
 *          Masters sends a uncomplete row to slave when it recieved one
 *          Repeat until all rows are completed
 *          The image array is then written to a file.
 *  
 */
void masterCode(int width, int height, int rank, int nodes)
{
    //initialize variables
    char **image;
    int indexOut;
    int rowNumber, nextRow;
    //int masterRow;
    int finish = NOTDONE;
    //finish = DONE;
    int findIteration;
    //struct complex number;
    struct timeval startTime, endTime, diffTime;
    float elapsedTime = 0;
    MPI_Status status;

    //creates image array
    image = (char**)malloc(height* sizeof(char*));
    for(indexOut = 0; indexOut < height; indexOut++)
    {
        image[indexOut] = (char*)malloc(width * sizeof(char));
        //sets the begging of each row to invalid pixel
        image[indexOut][0] = BADPIXEL;
        
    }
    
    //synchronize
    //barrier
    MPI_Barrier(MPI_COMM_WORLD);
	gettimeofday(&startTime, NULL); //start clock

    //assigns tasks all slaves to a row
    nextRow = 0;
    for(indexOut = 1; indexOut < nodes; indexOut++)
    {        
        MPI_Send(&nextRow, 1, MPI_INT, indexOut ,ROWNUMTAG, MPI_COMM_WORLD);        
        nextRow++;
        if(nextRow >= height)
        {
            nextRow = 0;
        }
    }    
    //wait for last row;
    while(finish == NOTDONE)
    {
        //recv from slave
        MPI_Recv(&rowNumber, 1, MPI_INT, MPI_ANY_SOURCE ,ROWNUMTAG, MPI_COMM_WORLD, &status);
        MPI_Recv(image[rowNumber], width, MPI_BYTE, status.MPI_SOURCE ,ROWARRTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        //assign new row
        MPI_Send(&nextRow, 1, MPI_INT, status.MPI_SOURCE ,ROWNUMTAG, MPI_COMM_WORLD);
        
        //calculates next row to be sent;
        
        nextRow++;
        if(nextRow >= height)
        {
            nextRow = 0;
        }
        findIteration = 0;
        //finds the next valid row
        while(image[nextRow][0] != BADPIXEL && findIteration < 2)
        {
            nextRow++;
            if(nextRow >= height)
            {
                nextRow = 0;
                findIteration++;
            }
        }
        if(findIteration > 1)
        {
            //if there is no more rows, computations are done
            finish = DONE;
        }
        
    }
    //finish recieve data from slaves

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


    //free memory
    for(indexOut = 0; indexOut < height; indexOut++)
    {
        free(image[indexOut]);
    }
    free(image);


    //stops all remaining slaves;
    nextRow = ALLDONE;
    for(indexOut = 1; indexOut < nodes; indexOut++)
    {        
        MPI_Send(&nextRow, 1, MPI_INT, indexOut ,ROWNUMTAG, MPI_COMM_WORLD);
    }    

}

/*
 *  Function name: slaveCode
 *  
 *  Brief: Mandelbrot code for the slave node
 *  
 *  Detail: Creates a working buffer
 *          Recives a row number
 *          Calculates all the pixels in the given row
 *          sends the row and row number back to master
 *          Repeats this process until it recieves an invalid row.
 *  
 */
void slaveCode(int width, int height, int rank, int nodes)
{   
    //variables
    int indexIn;
    char *workingBuffer;
    int workingRow;
    int finish= NOTDONE;
    struct complex number;


    //creates image row buffer
    workingBuffer = (char*)malloc(width* sizeof(char*));

    //synchronize
    MPI_Barrier(MPI_COMM_WORLD);
    //works until the computations are complete.
    while(finish == NOTDONE)
    {
        //wait for instructions
        MPI_Recv(&workingRow, 1, MPI_INT, MASTER, ROWNUMTAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //check if it is a valid row or done message
        if(workingRow == ALLDONE)
        {
            //all calculations are complete
            finish = DONE;
        }
        else
        {
            //begin calculations for image
            for(indexIn = 0; indexIn < width; indexIn++)
            {
                number.real = -2. + workingRow/((float)height)*4.;
                number.imag = -2. + indexIn/((float)width)*4.;
                workingBuffer[indexIn] = (char) (calc_Pixel(number) % 256);
            }
            //sends row and row numberback       
            MPI_Send(&workingRow, 1, MPI_INT, MASTER ,ROWNUMTAG, MPI_COMM_WORLD);
            MPI_Send(workingBuffer, width, MPI_BYTE, MASTER ,ROWARRTAG, MPI_COMM_WORLD);
        }
    }
    //free memory 
    free(workingBuffer);

}


