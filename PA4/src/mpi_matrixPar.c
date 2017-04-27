/******************************************************************************
* FILE: mpi_matrixSeq.c
* DESCRIPTION:
*   Calculates the matrix sequential. 
* AUTHOR: Evan Su
* LAST REVISED: 04/5/17
******************************************************************************/
//libraries
#include "mpi.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

//define constants
#define  MASTER        0
#define  SLAVE         1
#define  TAG           0
#define  SECTOMICRO    1000000
#define  PRINT         0
#define  PRINTMATRIX   1
#define  RANGE         100
#define  TRUE          1
#define  FALSE         0

/*function declarations*******************************************************/
void masterCode(int, int, int, char *, char *);
void slaveCode(int, int);
void matrixMultipleSquare(int **, int**, int**, int);
void matrixInitCannon(int ,int, int, int, 
        int *, int *, int **, int**);

void printMatrix(int **, int );

void rotateRow(int , int, int, int, int *, int*, int **);
void rotateCol(int , int, int, int, int *, int*, int **);

/*
 *  Function name: makeMatrix
 *  
 *  Brief: make Matrix
 */
int ** makeMatrix(int);


/*
 *  Function name: freeMatrix
 *  
 *  Brief: frees Matrix memory
 */
void freeMatrix(int**, int);


/*
 *  Function name: fillMatrix
 *  
 *  Brief: fills matrixA and B with numbers
 */  
void fillMatrix(int**, int**, int);


/*
 *  Function name: readMatrix
 *  
 *  Brief: fills matrixA and matrixB from files
 */
void readMatrix(int**, int**, int, FILE *, FILE *);


int calcUpNeighbor(int, int, int);
int calcDownNeighbor(int, int, int);
int calcLeftNeighbor(int, int, int);
int calcRightNeighbor(int, int, int);

/*function declarations*******************************************************/




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
 *  Parameters:
 *        int argc: number of command line arguements
 *
 *        char *argv[]: command line arguements
 *              argv[1]: size of matrix
 *              argv[2]: file name A
 *              argv[3]: file name B
 *          
 *////<><<><><><><><<><<
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
    
    //determines the square root of the number of processes
    //assumes the number of task is 0 < task <=36 and is can evenly
    //square root
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

    //checks for valid size
    if(validSize == FALSE)
    {
        printf("invalid size\n");
        printf("size: %d", length);
    }
    else if (taskid == MASTER)
    {
        //calls master code
        masterCode(atoi(argv[1]), taskid, length, argv[2], argv[3]);


    }
    else
    {
        //slave code
        slaveCode( taskid, length);
    }

    MPI_Finalize();
    return 0;



}

/*
 *  Function name: masterCode
 *  
 *  Brief: matrix multiplication for the master node
 *  
 *  Detail: Makes matrixs based on size entered 
 *          If the size is greater than zero, the matrix
 *          are generated by the program
 *          else read from file 
 *          
 *          matrix multiple the two files
 *          
 *          prints the results
 *
 *  Parameters:
 *        int size: length of matrix
 *
 *        char * fileA: name of file of matrix 1 
 *
 *        char * fileB: name of file of matrix 2
 *
 *  notes: the function will only read from file if size is 0 or less.
 *///<><><<><><><><
void masterCode(int size, int rank, int length, char * fileA, char * fileB)
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
    int tileLength;
    FILE *finA, *finB;
    int scanResult;
    srand(0);




    if(size > 0)
    {
        tileLength = size/length;
        //make matrix
	    matrixA = makeMatrix(size);
	    matrixB = makeMatrix(size);
	    matrixC = makeMatrix(size);

        //make tiles
        tileA = makeMatrix(tileLength);
        tileB = makeMatrix(tileLength);
        tileC = makeMatrix(tileLength);
        fillMatrix(matrixA, matrixB, size);
    }
    else
    {
        //open files
        finA = fopen(fileA,"r");
        finB = fopen(fileB,"r");

        //scans the size
        scanResult = fscanf(finA, "%d", &size);
        scanResult = fscanf(finB, "%d", &size);

        if(scanResult == 0)
        {
            //issues with the scan
            printf("read error\n");
            fclose(finA);
            fclose(finB);
            //exits
            return;
        }

        tileLength = size/length;
        //make matrix
        matrixA = makeMatrix(size);
        matrixB = makeMatrix(size);
        matrixC = makeMatrix(size);

        //make tiles
        tileA = makeMatrix(tileLength);
        tileB = makeMatrix(tileLength);
        tileC = makeMatrix(tileLength);

        //fill matrix from file
        readMatrix(matrixA, matrixB, size, finA, finB);
        
        //close files
        fclose(finA);
        fclose(finB);
    }

    //make comm buffers
    sendBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);
    recvBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);

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
                    tileA[indexIn][indexSub] = matrixA[indexIn][indexSub];
                    tileB[indexIn][indexSub] = matrixB[indexIn][indexSub];
                }
            }
        }
        else
        {
            //slaves

            //sends the length of the matrix
            MPI_Send(&tileLength, 1, MPI_INT, indexOut, TAG, MPI_COMM_WORLD);

            //calculates the coordinate            
            sendRow = indexOut/length;
            sendCol = indexOut%length;

            //fills send buffer
            for(indexIn = 0; indexIn < tileLength; indexIn++)
            {
                for(indexSub = 0; indexSub < tileLength; indexSub++)
                {
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
                    sendBuffer[indexIn*tileLength + indexSub] = 
                            matrixB[sendRow*tileLength + indexIn]
                            [sendCol*tileLength + indexSub];
                }
            }

            //sends tile A
            MPI_Send(sendBuffer, tileLength*tileLength, 
                    MPI_INT, indexOut, TAG, MPI_COMM_WORLD);
        }
    }


    //calculates the row and col of the master node
    row = rank/length;
    col = rank%length;

    MPI_Barrier(MPI_COMM_WORLD);//barrier start time
    //start time
    gettimeofday(&startTime, NULL);
    
    matrixInitCannon(row,col,length,tileLength,sendBuffer,recvBuffer, tileA, tileB);
    //runs cannons
    up = calcUpNeighbor(row, col, length);
    down = calcDownNeighbor(row, col, length);
    left = calcLeftNeighbor(row, col, length);
    right = calcRightNeighbor(row, col, length);
    
    
    matrixMultipleSquare(tileA, tileB, tileC, tileLength);

    for(indexOut = 0; indexOut < length -1; indexOut ++)
    {
        rotateRow(left, right, length, tileLength, sendBuffer, recvBuffer, tileA);
        rotateCol(up, down, length, tileLength, sendBuffer, recvBuffer, tileB);
        
        matrixMultipleSquare(tileA, tileB, tileC, tileLength);
    }

    MPI_Barrier(MPI_COMM_WORLD);//barrier end time
    //stop time
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &diffTime); //calc diff time
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 

    //prints result in microseconds
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
	
    freeMatrix(matrixA, tileLength);
	matrixA = NULL;

    freeMatrix(matrixB, tileLength);
	matrixB = NULL;

    freeMatrix(matrixC, tileLength);
	matrixC = NULL;

	//frees tiles
    freeMatrix(tileA, tileLength);
	tileA = NULL;

    freeMatrix(tileB, tileLength);
	tileB = NULL;

    freeMatrix(tileC, tileLength);
	tileC = NULL;

    //free send buffers
    free(sendBuffer);
    free(recvBuffer);
}

/*
 *  Function name: slaveCode
 *  
 *  Brief: bucketsort code for the slave node
 *  
 *  Detail: 
 *  
 */
void slaveCode(int rank, int length)
{
    int indexIn, indexOut;
    int **tileA, **tileB, **tileC;
    int *sendBuffer, *recvBuffer;
    int row, col;
    int up, down, left, right;
    int tileLength;

    //receive matrix length
    MPI_Recv(&tileLength, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //tileLength = size/length;
    //make tiles
	
    tileA = makeMatrix(tileLength);
    tileB = makeMatrix(tileLength);
    tileC = makeMatrix(tileLength);

    sendBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);
    recvBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);


    //recv data
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

    MPI_Barrier(MPI_COMM_WORLD);//barrier start time

    //initalizes cannons
    row = rank/length;
    col = rank%length;

    matrixInitCannon(row,col,length,tileLength,sendBuffer,recvBuffer, tileA, tileB);

    //runs cannons
    up = calcUpNeighbor(row, col, length);
    down = calcDownNeighbor(row, col, length);
    left = calcLeftNeighbor(row, col, length);
    right = calcRightNeighbor(row, col, length);

    matrixMultipleSquare(tileA, tileB, tileC, tileLength);

    for(indexOut = 0; indexOut < length -1; indexOut ++)
    {
        rotateRow(left, right, length, tileLength, sendBuffer, recvBuffer, tileA);
        rotateCol(up, down, length, tileLength, sendBuffer, recvBuffer, tileB);
        
        matrixMultipleSquare(tileA, tileB, tileC, tileLength);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);//barrier end time

    //prints tiles
    if(PRINTMATRIX)
    {
        //new line the csv time printout
        printf("\n");

        //prints rank
        printf("Rank: %d\n", rank);

        //matrix A
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

	//free matrix
    freeMatrix(tileA, tileLength);
	tileA = NULL;

    freeMatrix(tileB, tileLength);
	tileB = NULL;

    freeMatrix(tileC, tileLength);
	tileC = NULL;

	//frees com buffers
    free(sendBuffer);
    free(recvBuffer);
}

/*
 *  Function name: matrixMultipleSquare
 *  
 *  Brief: matrix multiplication two matrixes and stores into the results
 *  
 *  Detail: matrix multiples matrixA and matrixB. Stores result in matrixResults
 *          by adding the values to the matrixResults.
 *
 *  Parameters:
 *        int ** matrixA: first matrix multplicand
 *
 *        int ** matrixB: second matrix multplicand
 *
 *        int ** matrixResult: product matrix
 *
 *        int length: length of the matrix
 *
 *  Notes: The matrix must be initilized and pointing to a 2d array
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
                //adds the results to the matrixresult
                matrixResult[indexOut][indexIn] += 
                        matrixA[indexOut][indexSub]*matrixB[indexSub][indexIn];
            }
        }
    }
}

/*
 *  Function name: printMatrix
 *  
 *  Brief: prints the matrix
 *  
 *  Detail: prints matrix using a for loop
 *
 *  Parameters:
 *        int **matrix: matrix to print
 *
 *        int length: length of the matrix
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

void matrixInitCannon(int row,int col, int length, int matrixLength,
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

void rotateRow(int left, int right, int length, int matrixLength, int * sendBuffer, int * recvBuffer, int **matrix)
{
    int indexIn, indexOut;
    MPI_Request reqSend, reqRecv;
    MPI_Status statRecv;
    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            sendBuffer[indexOut* matrixLength + indexIn] = matrix[indexOut][indexIn];
        }
    }
    //printf("sending to %d, recv from: %d", left, right);
    

    MPI_Isend(sendBuffer, matrixLength*matrixLength, 
            MPI_INT, left, TAG, MPI_COMM_WORLD, &reqSend);

    MPI_Irecv(recvBuffer, matrixLength*matrixLength, 
            MPI_INT, right, TAG, MPI_COMM_WORLD, &reqRecv);

    
    MPI_Wait(&reqRecv, &statRecv);
    MPI_Wait(&reqSend, &statRecv);

    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            matrix[indexOut][indexIn] = recvBuffer[indexOut* matrixLength + indexIn];
        }
    }
}
void rotateCol(int up, int down, int length, int matrixLength, int * sendBuffer, int * recvBuffer, int **matrix)
{
    int indexIn, indexOut;
    MPI_Request reqSend, reqRecv;
    MPI_Status statRecv;
    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            sendBuffer[indexOut* matrixLength + indexIn] = matrix[indexOut][indexIn];
        }
    }
    //printf("sending to %d, recv from: %d", left, right);
    

    MPI_Isend(sendBuffer, matrixLength*matrixLength, 
            MPI_INT, up, TAG, MPI_COMM_WORLD, &reqSend);

    MPI_Irecv(recvBuffer, matrixLength*matrixLength, 
            MPI_INT, down, TAG, MPI_COMM_WORLD, &reqRecv);

    
    MPI_Wait(&reqRecv, &statRecv);    
    MPI_Wait(&reqSend, &statRecv);
    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            matrix[indexOut][indexIn] = recvBuffer[indexOut* matrixLength + indexIn];
        }
    }
}
/*
 *  Function name: makeMatrix
 *  
 *  Brief: make Matrix
 *  
 *  Detail: makes the matrix of size*size using calloc and malloc
 *
 *  Parameters:
 *        int size: length of the matrix
 */
int ** makeMatrix(int size)
{
    int index;
    int **tempPtr;

    tempPtr = (int **)malloc(sizeof(int*) * size);    
    for (index = 0; index < size; index ++)
    {
        tempPtr[index] = (int*)calloc(size ,sizeof(int));
    }
    return tempPtr;
}

/*
 *  Function name: freeMatrix
 *  
 *  Brief: frees Matrix memory
 *  
 *  Detail: frees the matrix memory
 *  Parameters:
 *        int **matrix: matrix to free
 *
 *        int size: length of the matrix
 */
void freeMatrix(int ** matrix, int size)
{
    int index;
    for(index = 0; index < size; index++)
    {
        free(matrix[index]);
    }
    free(matrix);
}

/*
 *  Function name: fillMatrix
 *  
 *  Brief: fills matrixA and B with numbers
 *  
 *  Detail: fills matrixA and matrixB with rand()
 *
 *  Parameters:
 *        int **matrixA: matrix 1 to fill
 *
 *        int **matrixB: matrix 2 to fill
 *
 *        int size: length of the matrix
 *
 *  Notes:  srand must be seeded before the function call
 */
void fillMatrix(int** matrixA, int **matrixB, int size)
{
    int indexOut, indexIn;

    for(indexOut = 0; indexOut < size; indexOut++)
    {
        for(indexIn = 0; indexIn < size; indexIn++)
        {
            //fills matrix of A and B at [indexOut][indexIn] with rand numbers
            matrixA[indexOut][indexIn] = rand() %RANGE;
            matrixB[indexOut][indexIn] = rand() %RANGE;
        }
    }
}
/*
 *  Function name: readMatrix
 *  
 *  Brief: fills matrixA and matrixB from files
 *  
 *  Detail: matrixA is filled with numbers from finA 
 *          matrixB is filled with numberes from finB
 *
 *  Parameters:
 *        int **matrixA: matrix 1 to fill
 *
 *        int **matrixB: matrix 2 to fill
 *
 *        int size: length of the matrix
 *
 *        FILE * finA: pointer to the matrix 1 data
 *
 *        FILE * finB: pointer to the matrix 2 data
 *
 *  Notes:  finA and finB must point to a file before using this function
 */
void readMatrix(int** matrixA, int** matrixB, int size, FILE * finA, FILE * finB)
{
    int indexIn, indexOut;
    int scanResult;
    
    for(indexOut = 0; indexOut < size; indexOut ++)
    {
        for(indexIn = 0; indexIn < size; indexIn++)
        {
            //reads the numbers into matrixA and matrixB
            scanResult = fscanf(finA, "%d", &matrixA[indexOut][indexIn]);
            scanResult = fscanf(finB, "%d", &matrixB[indexOut][indexIn]);
            if(scanResult == 0)
            {
                //read error
                printf("read error\n");
            }
        }
    }
}



int calcUpNeighbor(int row, int col, int length)
{
    return ((row + length - 1)%length)*length + col;
}
int calcDownNeighbor(int row, int col, int length)
{
    return ((row + 1)%length)*length + col;
}
int calcLeftNeighbor(int row, int col, int length)
{
    return row *length + (col + length - 1) %length;
}
int calcRightNeighbor(int row, int col, int length)
{
    return row *length + (col  + 1) %length;
}

