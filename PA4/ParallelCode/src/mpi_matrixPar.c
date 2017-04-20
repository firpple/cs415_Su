/******************************************************************************
* FILE: mpi_matrixSeq.c
* DESCRIPTION:
*   Calculates the matrix sequential. 
* AUTHOR: ME
* LAST REVISED: 04/5/17
******************************************************************************/
//libraries
#include "mpi.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

//define constants
#define  MASTER        0
#define  SLAVE      1
#define  TAG        0
#define  SECTOMICRO 1000000
#define  PRINT      0
#define     PRINTMATRIX    0
#define  RANGE        100
#define  TRUE        1
#define  FALSE        0
//function declarations
void masterCode(int, int, int);
void slaveCode(int,int, int);
void matrixMultipleSquare(int **, int**, int**, int);
void matrixInitCannon(int ,int, int, int, 
        int *, int *, int **, int**);

void printMatrix(int **, int );

void rotateRow(int , int, int, int, int *, int*, int **);
void rotateCol(int , int, int, int, int *, int*, int **);


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
    int length;
    int validSize = TRUE;
    //parallel start
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Get_processor_name(hostname, &len);
    
    //determines the squareroot of the number of workers
    //only goes up to 6^2
    switch(numtasks)
    {
        case 4:
            length = 2;
            break;
        case 9:
            length = 3;
            break;
        case 16:
            length = 4;
            break;
        case 25:
            length = 5;
            break;
        case 36:
            length = 6;
            break;
        default:
            validSize = FALSE;
            break;
    }
    //checks if the number of workers are correct
    if(validSize == FALSE)
    {
        printf("invalid size\n");
        printf("size: %d", length);
    }
    else if (taskid == MASTER)
    {
        //master code
        //mastercode(length of matrix, rank, length of worker mesh)
        masterCode(atoi(argv[1]), taskid, length);


    }
    else
    {
        //slave code
        //slavecode(length of matrix, rank, length of worker mesh)
        slaveCode(atoi(argv[1]), taskid, length);
    }

    MPI_Finalize();
    return 0;



}

/*
 *  Function name: masterCode
 *  
 *  Brief: Matrix multiplaction code for the master node
 *  
 *  Detail: 
 *          
 */
void masterCode(int size, int rank, int length)
{
    struct timeval startTime, endTime, diffTime;
    float elapsedTime = 0;
    int **matrixA, **matrixB, **matrixC;
    int **tileA, **tileB, **tileC;
    int sendRow, sendCol;
    int row, col;
    int up, down, left, right;
    int *sendBuffer, *recvBuffer;
    int indexIn, indexOut, indexSub;
    int tileLength = size/length;

    //make matrix
    matrixA = (int **)malloc(sizeof(int*) * size);
    matrixB = (int **)malloc(sizeof(int*) * size);
    matrixC = (int **)malloc(sizeof(int*) * size);
    for (indexIn = 0; indexIn < size; indexIn ++)
    {
        matrixA[indexIn] = (int*)calloc(size ,sizeof(int));
        matrixB[indexIn] = (int*)calloc(size ,sizeof(int));        
        matrixC[indexIn] = (int*)calloc(size ,sizeof(int));
    }
    
    //make tiles
    tileA = (int **)malloc(sizeof(int*) * tileLength);
    tileB = (int **)malloc(sizeof(int*) * tileLength);
    tileC = (int **)malloc(sizeof(int*) * tileLength);
    for (indexIn = 0; indexIn < tileLength; indexIn ++)
    {
        tileA[indexIn] = (int*)calloc(tileLength, sizeof(int));
        tileB[indexIn] = (int*)calloc(tileLength, sizeof(int));        
        tileC[indexIn] = (int*)calloc(tileLength, sizeof(int));
    }
    //make comm buffers
    sendBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);
    recvBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);

    //fill matrix
    srand(0);
    for(indexOut = 0; indexOut < size; indexOut++)
    {
        for(indexIn = 0; indexIn < size; indexIn++)
        {
            //matrixA[indexOut][indexIn] = (indexIn + indexOut)%100;
            //matrixB[indexOut][indexIn] = (indexIn + indexOut)%100;
            
            matrixA[indexOut][indexIn] = rand() %RANGE;
            matrixB[indexOut][indexIn] = rand() %RANGE;
            matrixC[indexOut][indexIn] = 0;
        }
    }
    
    //gives everyone data
    //for goes from 0 to max num of slaves
    for(indexOut = 0; indexOut < length*length; indexOut++)
    {
        if(indexOut == 0)
        {
            //master
            for(indexIn = 0; indexIn < tileLength; indexIn++)
            {
                for(indexSub = 0; indexSub < tileLength; indexSub++)
                {
                    //puts the matrix into master tile
                    tileA[indexIn][indexSub] = matrixA[indexIn][indexSub];
                    tileB[indexIn][indexSub] = matrixB[indexIn][indexSub];
                }
            }



        }
        else
        {
            //slaves
            //calculates the coordinate            
            sendRow = indexOut/length;
            sendCol = indexOut%length;
            //fills send buffer
            for(indexIn = 0; indexIn < tileLength; indexIn++)
            {
                for(indexSub = 0; indexSub < tileLength; indexSub++)
                {
                    //puts the matrixA into send buffer
                    sendBuffer[indexIn*tileLength + indexSub] = 
                            matrixA[sendRow*tileLength + indexIn]
                            [sendCol*tileLength + indexSub];
                }
            }

            //sends tile A
            MPI_Send(sendBuffer, tileLength*tileLength, 
                    MPI_INT, indexOut, TAG, MPI_COMM_WORLD);
            

            for(indexIn = 0; indexIn < tileLength; indexIn++)
            {
                for(indexSub = 0; indexSub < tileLength; indexSub++)
                {
                    //puts the matrixB into send buffer
                    sendBuffer[indexIn*tileLength + indexSub] = 
                            matrixB[sendRow*tileLength + indexIn]
                            [sendCol*tileLength + indexSub];
                }
            }

            //sends tile B
            MPI_Send(sendBuffer, tileLength*tileLength, 
                    MPI_INT, indexOut, TAG, MPI_COMM_WORLD);
        }
    }


    //calculates coornates
    //should always be 0, 0
    row = rank/length;
    col = rank%length;

    MPI_Barrier(MPI_COMM_WORLD);
    //start time
    gettimeofday(&startTime, NULL);
    //runs cannons
    matrixInitCannon(row,col,length,tileLength,sendBuffer,recvBuffer, tileA, tileB);
    //calcs neighbors
    up = ((row + length - 1)%length)*length + col;
    down = ((row + 1)%length)*length + col;
    left = row *length + (col + length - 1) %length;
    right = row *length + (col  + 1) %length;
    
    //multiply matrix
    matrixMultipleSquare(tileA, tileB, tileC, tileLength);

    for(indexOut = 0; indexOut < length -1; indexOut ++)
    {
        //rotates
        rotateRow(left, right, length, tileLength, sendBuffer, recvBuffer, tileA);
        rotateCol(up, down, length, tileLength, sendBuffer, recvBuffer, tileB);
        
        //matrix multiplication
        matrixMultipleSquare(tileA, tileB, tileC, tileLength);
    }

    
    if(PRINTMATRIX)
    {
        //matrixMultipleSquare(matrixA, matrixB, matrixC, size);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    //stop time
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 

    //prints result
    //time
    printf("%f,",elapsedTime );
    //print matrixs
    
    if(PRINTMATRIX)
    {
        printf("\n");
        //matrix A
        printf("matrix A:\n");
        printMatrix(matrixA, size);
        //matrix B
        printf("matrix B:\n");
        printMatrix(matrixB, size);
        //matrix C
        printf("matrix C:\n");
        printMatrix(matrixC, size);
    }
    
    //prints tiles
    if(PRINTMATRIX)
    {
        printf("\n");
        //matrix A
        printf("Rank: %d\n", 0);
        printf("Tile A:\n");
        printMatrix(tileA, tileLength);
        //matrix B
        printf("Tile B:\n");
        printMatrix(tileB, tileLength);
        //matrix C
        printf("Tile C:\n");
        printMatrix(tileC, tileLength);
    }

    //free memory
    for(indexIn = 0; indexIn < size; indexIn++)
    {
        free(matrixA[indexIn]);
        free(matrixB[indexIn]);
        free(matrixC[indexIn]);
    }
    free(matrixA);
    free(matrixB);
    free(matrixC);
    //printf("freed matrix\n");

    for(indexIn = 0; indexIn < tileLength; indexIn++)
    {
        free(tileA[indexIn]);
        free(tileB[indexIn]);
        free(tileC[indexIn]);
    }
    free(tileA);
    free(tileB);
    free(tileC);
    //printf("freed tiles\n");
    free(sendBuffer);
    free(recvBuffer);
}

/*
 *  Function name: slaveCode
 *  
 *  Brief: matrix calculation for the slave node
 *  
 *  Detail: 
 *  
 */
void slaveCode(int size, int rank, int length)
{
    //printf("hello from slave");
    int indexIn, indexOut;
    int **tileA, **tileB, **tileC;
    int *sendBuffer, *recvBuffer;
    int row, col;

    int up, down, left, right;
    int tileLength;
    tileLength = size/length;
    //make tiles
    tileA = (int **)malloc(sizeof(int*) * tileLength);
    tileB = (int **)malloc(sizeof(int*) * tileLength);
    tileC = (int **)malloc(sizeof(int*) * tileLength);
    for (indexIn = 0; indexIn < tileLength; indexIn ++)
    {
        tileA[indexIn] = (int*)calloc(tileLength, sizeof(int));
        tileB[indexIn] = (int*)calloc(tileLength, sizeof(int));        
        tileC[indexIn] = (int*)calloc(tileLength, sizeof(int));
    }
    sendBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);
    recvBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);
    //recv tile data
    MPI_Recv(recvBuffer, tileLength*tileLength, 
            MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for(indexOut = 0; indexOut < tileLength; indexOut++)
    {
        for(indexIn = 0; indexIn < tileLength; indexIn++)
        {
            tileA[indexOut][indexIn] = recvBuffer[indexOut*tileLength + indexIn];
        }
    }
    MPI_Recv(recvBuffer, tileLength*tileLength, 
            MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for(indexOut = 0; indexOut < tileLength; indexOut++)
    {
        for(indexIn = 0; indexIn < tileLength; indexIn++)
        {
            tileB[indexOut][indexIn] = recvBuffer[indexOut*tileLength + indexIn];
        }
    }

    row = rank/length;
    col = rank%length;
    //begins calculations
    MPI_Barrier(MPI_COMM_WORLD);// started time
    //runs cannons
    //initalizes cannons
    matrixInitCannon(row,col,length,tileLength,sendBuffer,recvBuffer, tileA, tileB);
    up = ((row + length - 1)%length)*length + col;
    down = ((row + 1)%length)*length + col;
    left = row *length + (col + length - 1) %length;
    right = row *length + (col  + 1) %length;

    matrixMultipleSquare(tileA, tileB, tileC, tileLength);

    for(indexOut = 0; indexOut < length -1; indexOut ++)
    {
        rotateRow(left, right, length, tileLength, sendBuffer, recvBuffer, tileA);
        rotateCol(up, down, length, tileLength, sendBuffer, recvBuffer, tileB);
        
        matrixMultipleSquare(tileA, tileB, tileC, tileLength);
    }
    //matrix caluculation stops
    MPI_Barrier(MPI_COMM_WORLD);//stop time
    //prints tiles
    if(PRINTMATRIX)
    {
        printf("\n");
        //matrix A
        printf("Rank: %d\n", rank);
        printf("Tile A:\n");
        printMatrix(tileA, tileLength);
        //matrix B
        printf("Tile B:\n");
        printMatrix(tileB, tileLength);
        //matrix C
        printf("Tile C:\n");
        printMatrix(tileC, tileLength);
    }

    //free memory
    for(indexIn = 0; indexIn < tileLength; indexIn++)
    {
        free(tileA[indexIn]);
        free(tileB[indexIn]);
        free(tileC[indexIn]);
    }
    free(tileA);
    free(tileB);
    free(tileC);
    free(sendBuffer);
    free(recvBuffer);
}


/*
 *  Function name: matrixMultipleSquare
 *  
 *  Brief: matrix multiplication with two square arrays
 *  
 *  Detail: matrix multiplys the two matrixs and stores into results matrix
 *          When the matrix is stored, results is added into the resulted matrix
 *  
 */
void matrixMultipleSquare(int **matrixA, int**matrixB, int**matrixResult, int length)
{
    int indexIn, indexOut, indexSub;
    for(indexOut = 0; indexOut < length; indexOut++)
    {
        for(indexIn = 0; indexIn < length; indexIn++)
        {
            for(indexSub = 0; indexSub < length; indexSub++)
            {
                matrixResult[indexOut][indexIn] += 
                    matrixA[indexOut][indexSub]*matrixB[indexSub][indexIn];
            }
        }
    }
}
/*
 *  Function name: print matrix
 *  
 *  Brief: matrix prints
 *  
 *  Detail: 
 *  
 */
void printMatrix(int **matrix, int length)
{
    int indexIn, indexOut;
    for(indexOut = 0; indexOut < length; indexOut++)
    {
        for(indexIn = 0; indexIn < length; indexIn++)
        {
            printf("%8d", matrix[indexOut][indexIn]);
        }
        printf("\n");
    }
}

/*
 *  Function name: matrixInitCannon
 *  
 *  Brief: initalizes the matrix for canons alogorithm
 *  
 *  Detail: 
 *  
 *  Notes:  This functions has 8 arguements. 2 are used for 
 *          speedup(sendBuffer and recvBuffer).
 */
void matrixInitCannon(int row, int col, int length, int matrixLength,
        int * sendBuffer, int * recvBuffer, int **matrixA, int**matrixB)
{
    int index;
    int up, down, left, right;
    up = ((row + length - 1)%length)*length + col;
    down = ((row + 1)%length)*length + col;
    left = row *length + (col + length - 1) %length;
    right = row *length + (col  + 1) %length;

    //printf("me: %d, U:%d, D:%d, L:%d, R:%d\n", row*length +col, up, down, left, right);
    for(index = 0; index < row; index++)
    {
        rotateRow(left, right, length, matrixLength, sendBuffer, recvBuffer, matrixA);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for(index = 0; index < col; index++)
    {
        rotateCol(up, down, length, matrixLength, sendBuffer, recvBuffer, matrixB);
    }
}
/*
 *  Function name: rotateRow
 *  
 *  Brief: rotates tiles to the left
 *  
 *  Detail: 
 *  
 *  Notes:  This functions has 7 arguements. 2 are used for 
 *          speedup(sendBuffer and recvBuffer).
 */
void rotateRow(int left, int right, int length, int matrixLength, 
        int * sendBuffer, int * recvBuffer, int **matrix)
{
    int indexIn, indexOut;
    MPI_Request reqSend, reqRecv;
    MPI_Status statRecv;
    //matrix transfered into buffer
    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            sendBuffer[indexOut* matrixLength + indexIn] = matrix[indexOut][indexIn];
        }
    }
    //sends the tiles
    MPI_Isend(sendBuffer, matrixLength*matrixLength, 
            MPI_INT, left, TAG, MPI_COMM_WORLD, &reqSend);

    MPI_Irecv(recvBuffer, matrixLength*matrixLength, 
            MPI_INT, right, TAG, MPI_COMM_WORLD, &reqRecv);

    
    MPI_Wait(&reqRecv, &statRecv);
    MPI_Wait(&reqSend, &statRecv);

    //buffer transfered into matrix
    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            matrix[indexOut][indexIn] = recvBuffer[indexOut* matrixLength + indexIn];
        }
    }
}

/*
 *  Function name: rotateCol
 *  
 *  Brief: rotates tiles to the up
 *  
 *  Detail: 
 *  
 *  Notes:  This functions has 7 arguements. 2 are used for 
 *          speedup(sendBuffer and recvBuffer).
 */
void rotateCol(int up, int down, int length, int matrixLength, int * sendBuffer, int * recvBuffer, int **matrix)
{
    int indexIn, indexOut;
    MPI_Request reqSend, reqRecv;
    MPI_Status statRecv;
    //matrix transfered into buffer
    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            sendBuffer[indexOut* matrixLength + indexIn] = matrix[indexOut][indexIn];
        }
    }
    //sends the tiles
    MPI_Isend(sendBuffer, matrixLength*matrixLength, 
            MPI_INT, up, TAG, MPI_COMM_WORLD, &reqSend);

    MPI_Irecv(recvBuffer, matrixLength*matrixLength, 
            MPI_INT, down, TAG, MPI_COMM_WORLD, &reqRecv);

    
    MPI_Wait(&reqRecv, &statRecv);    
    MPI_Wait(&reqSend, &statRecv);
    
    //buffer transfered into matrix
    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            matrix[indexOut][indexIn] = recvBuffer[indexOut* matrixLength + indexIn];
        }
    }
}

