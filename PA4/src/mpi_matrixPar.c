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
#include <math.h>

//define constants
#define  MASTER        0
#define  SLAVE         1
#define  TAG           0
#define  SECTOMICRO    1000000
#define  PRINT         0
#define  PRINTMATRIX   0
#define  PRINTMASTER   1
#define  PRINTTIME     1
#define  RANGE         100
#define  TRUE          1
#define  FALSE         0

/*function declarations*******************************************************/

/*
 *  Function name: masterCode
 *  
 *  Brief: matrix multiplication for the master node
 */
void masterCode(int, int, int, char *, char *);


/*
 *  Function name: slaveCode
 *  
 *  Brief: matrix multiplication for the slave node
 */
void slaveCode(int, int);


/*
 *  Function name: matrixMultipleSquare
 *  
 *  Brief: matrix multiplication two matrixes and stores into the results
 */
void matrixMultipleSquare(int **, int**, int**, int);


/*
 *  Function name: printMatrix
 *  
 *  Brief: prints the matrix
 */
void printMatrix(int **, int );


/*
 *  Function name: matrixInitCannon
 *  
 *  Brief: initializes the tiles of all processes for Cannon's algorithm
 */
void matrixInitCannon(int ,int, int, int, 
        int *, int *, int **, int**);


/*
 *  Function name: rotateRow
 *  
 *  Brief: rotates tiles to the left
 */
void rotateRow(int , int, int, int, int *, int*, int **);


/*
 *  Function name: rotateCol
 *  
 *  Brief: rotates tiles upward
 */
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

/*
 *  Function name: calc*Neighbor
 *  
 *  Brief: calculates the id of the neighbor *
 */
int calcUpNeighbor(int, int, int);
int calcDownNeighbor(int, int, int);
int calcLeftNeighbor(int, int, int);
int calcRightNeighbor(int, int, int);


/*
 *  Function name: consolidateMatrixMaster
 *  
 *  Brief: consolidate all resulting tiles into matrix
 */
void consolidateMatrixMaster(int tileLength, int meshLength, int ** tile, int** matrix);


/*
 *  Function name: consolidateMatrixSlave
 *  
 *  Brief: consolidate all tiles into master matrix
 */
void consolidateMatrixSlave( int tileLength, int meshLength, int rank, int** tile);



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
 *
 *  Parameters:
 *        int argc: number of command line arguements
 *
 *        char *argv[]: command line arguements
 *              argv[1]: size of matrix
 *              argv[2]: file name A
 *              argv[3]: file name B
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
    
    //determines the square root of the number of processes
    //assumes the number of task is 0 < task <=36 and is can evenly
    //square root
    /*switch(numtasks)
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
    }*/
    length = (int) sqrt(numtasks);

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
 *          Sends a tile to each slave
 *          
 *          matrix multiple the two matrix using cannons algorithm
 *          
 *          prints the results
 *
 *  Parameters:
 *        int size: length of matrix
 *
 *        int rank: rank of the node
 *
 *        int length: length of matrixMesh
 *
 *        char * fileA: name of file of matrix 1 
 *
 *        char * fileB: name of file of matrix 2
 *
 *  notes: the function will only read from file if size is 0 or less.
 */
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



    //*****************************************************/
    //***Read Matrix **************************************/
    //*****************************************************/

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

    
    //*****************************************************/
    //***Start Matrix Send*********************************/
    //*****************************************************/
    //gives everyone data
    //for goes from 0 to max num of slaves
    for(indexOut = 0; indexOut < length*length; indexOut++)
    {
        if(indexOut == 0)
        {
            //master
            //copys the first tile of the matrix to the master's
            //tile
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

            //fills send buffer with tile A
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

            //fills send buffer with tile B
            for(indexIn = 0; indexIn < tileLength; indexIn++)
            {
                for(indexSub = 0; indexSub < tileLength; indexSub++)
                {
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


    //calculates the row and col of the master node
    row = rank/length;
    col = rank%length;
    
    //*****************************************************/
    //***Start Matrix Multiply*****************************/
    //*****************************************************/

    MPI_Barrier(MPI_COMM_WORLD);//barrier start time
    //start time
    gettimeofday(&startTime, NULL);
    
    //runs cannons
    matrixInitCannon(row,col,length,tileLength,sendBuffer,recvBuffer, tileA, tileB);

    //calculates neighbors
    up = calcUpNeighbor(row, col, length);
    down = calcDownNeighbor(row, col, length);
    left = calcLeftNeighbor(row, col, length);
    right = calcRightNeighbor(row, col, length);
    
    //matrix multiply
    matrixMultipleSquare(tileA, tileB, tileC, tileLength);

    //Rotates matrix and multiply
    for(indexOut = 0; indexOut < length -1; indexOut ++)
    {
        rotateRow(left, right, length, tileLength, sendBuffer, recvBuffer, tileA);
        rotateCol(up, down, length, tileLength, sendBuffer, recvBuffer, tileB);
        
        matrixMultipleSquare(tileA, tileB, tileC, tileLength);
    }

    MPI_Barrier(MPI_COMM_WORLD);//barrier end time

    //stop time
    gettimeofday(&endTime, NULL);
    //calc diff time
    timersub(&endTime, &startTime, &diffTime);
    //converts time struct to float
    elapsedTime = (diffTime.tv_sec * SECTOMICRO + diffTime.tv_usec); 


    //*****************************************************/
    //***Print Results*************************************/
    //*****************************************************/

    //gathers all the C tiles into matrixC
    consolidateMatrixMaster(tileLength, length, tileC, matrixC);

    if(PRINTTIME)
    {
        //prints result in microseconds
        printf("%f,",elapsedTime );
    }

    //print matrixs
    if(PRINTMATRIX || PRINTMASTER)
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
	
    //free matrix
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
 *  Brief: matrix multiplication for the slave node
 *  
 *  Detail: Makes matrixs based on size recieved from master 
 *          matrix multiple the two matrix using cannons algorithm
 *          prints the results
 *
 *  Parameters:
 *        int rank: rank of the node
 *
 *        int length: length of matrixMesh
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

    //make tiles
    tileA = makeMatrix(tileLength);
    tileB = makeMatrix(tileLength);
    tileC = makeMatrix(tileLength);

    //make comm buffers
    sendBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);
    recvBuffer = (int *)malloc(sizeof(int) * tileLength * tileLength);


    //*****************************************************/
    //***Recieve Matrix Data*******************************/
    //*****************************************************/

    //recv data of tile A
    MPI_Recv(recvBuffer, tileLength*tileLength, 
            MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //transfer data to tile A
    for(indexOut = 0; indexOut < tileLength; indexOut++)
    {
        for(indexIn = 0; indexIn < tileLength; indexIn++)
        {
            tileA[indexOut][indexIn] = recvBuffer[indexOut*tileLength + indexIn];
        }
    }

    //recv data of tile B
    MPI_Recv(recvBuffer, tileLength*tileLength, 
            MPI_INT, MASTER, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //transfer data to tile B
    for(indexOut = 0; indexOut < tileLength; indexOut++)
    {
        for(indexIn = 0; indexIn < tileLength; indexIn++)
        {
            tileB[indexOut][indexIn] = recvBuffer[indexOut*tileLength + indexIn];
        }
    }
    //*****************************************************/
    //***Matrix Multiply***********************************/
    //*****************************************************/
    MPI_Barrier(MPI_COMM_WORLD);//barrier start time

    //calculates coordinates of self
    row = rank/length;
    col = rank%length;

    //calculates neighbors
    up = calcUpNeighbor(row, col, length);
    down = calcDownNeighbor(row, col, length);
    left = calcLeftNeighbor(row, col, length);
    right = calcRightNeighbor(row, col, length);

    //initalizes cannons
    matrixInitCannon(row,col,length,tileLength,sendBuffer,recvBuffer, tileA, tileB);

    //matrix multiply
    matrixMultipleSquare(tileA, tileB, tileC, tileLength);

    //rotates tiles and matrix multiply
    for(indexOut = 0; indexOut < length -1; indexOut ++)
    {
        rotateRow(left, right, length, tileLength, sendBuffer, recvBuffer, tileA);
        rotateCol(up, down, length, tileLength, sendBuffer, recvBuffer, tileB);
        
        matrixMultipleSquare(tileA, tileB, tileC, tileLength);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);//barrier end time


    //*****************************************************/
    //***Print Results*************************************/
    //*****************************************************/

    //sends the C tile back to the master for printing
    consolidateMatrixSlave(tileLength, length, rank, tileC);

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


/*
 *  Function name: matrixInitCannon
 *  
 *  Brief: initializes the tiles of all processes for Cannon's algorithm
 *  
 *  Detail: shifts tiles left process' row number of times
 *          shitfs tiles up process' col number of times
 *
 *  Parameters:
 *        int row: row coordinate of process
 *
 *        int col: col coordinate of process
 *
 *        int length: length of matrixMesh
 *
 *        int matrixLength: length of matrix tile
 *
 *        int * sendBuffer: array used for sending 
 *
 *        int * recvBuffer: array used for receiving
 *
 *        int ** matrixA: tile A for rotating
 *
 *        int ** matrixB: tile B for rotating
 *
 *  notes: Send and recv buffer must be initialized to 
 *         size of matrixLength*matrixLength
 */
void matrixInitCannon(int row,int col, int length, int matrixLength,
        int * sendBuffer, int * recvBuffer, int **matrixA, int**matrixB)
{
    int index;
    int up, down, left, right;

    //calculates neighbors
    up = calcUpNeighbor(row, col, length);
    down = calcDownNeighbor(row, col, length);
    left = calcLeftNeighbor(row, col, length);
    right = calcRightNeighbor(row, col, length);

    //The printf below is saved for debugging purposes.
    //printf("me: %d, U:%d, D:%d, L:%d, R:%d\n", row*length +col, up, down, left, right);

    //rotates tile A left row number of times 
    for(index = 0; index < row; index++)
    {
        rotateRow(left, right, length, matrixLength, sendBuffer, recvBuffer, matrixA);
    }

    MPI_Barrier(MPI_COMM_WORLD);//waits for row rotate to complete

    //rotates tile B up row number of times
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
 *  Detail: transfer 2d matrix into 1d array of sendbuffer
 *          sends the matrix to the left neighbor
 *          recieves the new matrix from the right neighbor
 *          copies recved matrix into original matrix.
 *
 *  Parameters:
 *        int left: left neighbor's id number
 *
 *        int right: right neighbor's id number
 *
 *        int length: length of matrixMesh
 *
 *        int matrixLength: length of matrix tile
 *
 *        int * sendBuffer: array used for sending 
 *
 *        int * recvBuffer: array used for receiving
 *
 *        int ** matrix: tile matrix for rotating
 *
 *  notes: Send and recv buffer must be initialized to 
 *         size of matrixLength*matrixLength
 */
void rotateRow(int left, int right, int length, int matrixLength, 
        int * sendBuffer, int * recvBuffer, int **matrix)
{
    int indexIn, indexOut;
    MPI_Request reqSend, reqRecv;
    MPI_Status statRecv;

    //copies the matrix into the sendbuffer
    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            sendBuffer[indexOut* matrixLength + indexIn] = matrix[indexOut][indexIn];
        }
    }
    //printf("sending to %d, recv from: %d", left, right);
    
    //sends the matrix to the left
    MPI_Isend(sendBuffer, matrixLength*matrixLength, 
            MPI_INT, left, TAG, MPI_COMM_WORLD, &reqSend);

    //recv matrix from the right
    MPI_Irecv(recvBuffer, matrixLength*matrixLength, 
            MPI_INT, right, TAG, MPI_COMM_WORLD, &reqRecv);

    //wait for sends and recv to complete
    MPI_Wait(&reqRecv, &statRecv);
    MPI_Wait(&reqSend, &statRecv);

    //copies the recvbuffer to the matrix
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
 *  Brief: rotates tiles upward
 *  
 *  Detail: transfer 2d matrix into 1d array of sendbuffer
 *          sends the matrix to the up neighbor
 *          recieves the new matrix from the down neighbor
 *          copies recved matrix into original matrix.
 *
 *  Parameters:
 *        int up: up neighbor's id number
 *
 *        int down: down neighbor's id number
 *
 *        int length: length of matrixMesh
 *
 *        int matrixLength: length of matrix tile
 *
 *        int * sendBuffer: array used for sending 
 *
 *        int * recvBuffer: array used for receiving
 *
 *        int ** matrix: tile matrix for rotating
 *
 *  notes: Send and recv buffer must be initialized to 
 *         size of matrixLength*matrixLength
 */
void rotateCol(int up, int down, int length, int matrixLength, 
        int * sendBuffer, int * recvBuffer, int **matrix)
{
    int indexIn, indexOut;
    MPI_Request reqSend, reqRecv;
    MPI_Status statRecv;
    
    //copies the matrix into the sendbuffer
    for(indexOut = 0; indexOut < matrixLength; indexOut++)
    {
        for(indexIn = 0; indexIn < matrixLength; indexIn++)
        {
            sendBuffer[indexOut* matrixLength + indexIn] = matrix[indexOut][indexIn];
        }
    }


    //printf("sending to %d, recv from: %d", left, right);
    

    //sends the matrix to up
    MPI_Isend(sendBuffer, matrixLength*matrixLength, 
            MPI_INT, up, TAG, MPI_COMM_WORLD, &reqSend);

    //recv matrix from the down
    MPI_Irecv(recvBuffer, matrixLength*matrixLength, 
            MPI_INT, down, TAG, MPI_COMM_WORLD, &reqRecv);

    
    //wait for sends and recv to complete
    MPI_Wait(&reqRecv, &statRecv);    
    MPI_Wait(&reqSend, &statRecv);

    //copies the recvbuffer to the matrix
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


/*
 *  Function name: calcUpNeighbor
 *  
 *  Brief: calculates the id of the neighbor up
 *  
 *  Detail: uses rotate formula to calculate neighbor
 *
 *  Parameters:
 *        int row: row coordinate of calling process
 *
 *        int col: col coordinate of calling process
 *
 *        int length: length of the mesh
 *
 */
int calcUpNeighbor(int row, int col, int length)
{
    return ((row + length - 1)%length)*length + col;
}

/*
 *  Function name: calcDownNeighbor
 *  
 *  Brief: calculates the id of the neighbor down
 *  
 *  Detail: uses rotate formula to calculate neighbor
 *
 *  Parameters:
 *        int row: row coordinate of calling process
 *
 *        int col: col coordinate of calling process
 *
 *        int length: length of the mesh
 *
 */
int calcDownNeighbor(int row, int col, int length)
{
    return ((row + 1)%length)*length + col;
}

/*
 *  Function name: calcLeftNeighbor
 *  
 *  Brief: calculates the id of the neighbor Left
 *  
 *  Detail: uses rotate formula to calculate neighbor
 *
 *  Parameters:
 *        int row: row coordinate of calling process
 *
 *        int col: col coordinate of calling process
 *
 *        int length: length of the mesh
 *
 */
int calcLeftNeighbor(int row, int col, int length)
{
    return row *length + (col + length - 1) %length;
}

/*
 *  Function name: calcRightNeighbor
 *  
 *  Brief: calculates the id of the neighbor right
 *  
 *  Detail: uses rotate formula to calculate neighbor
 *
 *  Parameters:
 *        int row: row coordinate of calling process
 *
 *        int col: col coordinate of calling process
 *
 *        int length: length of the mesh
 *
 */
int calcRightNeighbor(int row, int col, int length)
{
    return row *length + (col  + 1) %length;
}


/*
 *  Function name: consolidateMatrixMaster
 *  
 *  Brief: consolidate all resulting tiles into matrix
 *  
 *  Detail: Copies the master's tile to the first section
 *          Recieve tiles from slaves row by row.
 *
 *  Parameters:
 *        int tileLength: length of tile matrix
 *
 *        int meshLength: length of mesh
 *
 *        int ** tile: tile to consolidate
 *
 *        int ** matrix: matrix to consolidate into
 *
 *  Note: This function must be called by master process only
 *        This function only works in parallel with consolidateMatrixSlave
 */
void consolidateMatrixMaster(int tileLength, int meshLength, int ** tile, int** matrix)
{
    int indexIn, indexOut;
    int row, col;

    //puts masters tile in matrix
    for(indexOut = 0; indexOut < tileLength; indexOut++)
    {
        for(indexIn = 0; indexIn < tileLength; indexIn++)
        {
            matrix[indexOut][indexIn] = tile[indexOut][indexIn]; 
        }
    }

    //puts recieve tile and puts in matrix
    for(indexOut = 1; indexOut < meshLength * meshLength; indexOut++)
    {
        //calculates slave's coordinates
        row = indexOut/meshLength;
        col = indexOut%meshLength;
        
        for(indexIn = 0; indexIn < tileLength; indexIn++)
        {
            //receive tile row into the correct matrix row
            MPI_Recv(&matrix[row* tileLength + indexIn][col* tileLength], tileLength, 
                    MPI_INT, indexOut, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        MPI_Barrier(MPI_COMM_WORLD);//waits for tile to complete send and recv
    }    
}


/*
 *  Function name: consolidateMatrixSlave
 *  
 *  Brief: consolidate all tiles into master matrix
 *  
 *  Detail: Waits for master to be ready to recv tile
 *          Sends tile to master row by row
 *
 *  Parameters:
 *        int tileLength: length of tile matrix
 *
 *        int meshLength: length of mesh
 *
 *        int rank: rank of calling process
 *
 *        int ** tile: tile to consolidate
 *
 *  Note: This function must be called by master process only
 *        This function only works in parallel with consolidateMatrixMaster
 */
void consolidateMatrixSlave( int tileLength, int meshLength, int rank, int** tile)
{
    
    int indexIn, indexOut;

    for(indexOut = 1; indexOut < meshLength * meshLength; indexOut++)
    {
        //sends tile only if the "turn" is the current process
        if(rank == indexOut)
        {
            //sends tile row by row
            for(indexIn = 0; indexIn < tileLength; indexIn++)
            {
                MPI_Send(tile[indexIn], tileLength, 
                        MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
                
            }
        }
        //waits for communication from other tiles to finish
        MPI_Barrier(MPI_COMM_WORLD);
    }    
}
