/*
    Copyright (c) 2019 Daniel Notland

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software
    is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int automatonSize;
    int *grid = NULL;
    int transformation[512];
    int nStates;
    double local_start, local_finish, local_elapsed, elapsed;


    if(world_rank == 0) {

        nStates = atoi(argv[3]);

        // READING THE RULES
        FILE *rules;
        if ((rules = fopen(argv[1], "r")) == NULL) {
            printf("Couldn´t open %s", argv[1]);
            return 1;
        }

        int c = 0;
        while (fscanf(rules, "%*s %i", &transformation[c++]) != EOF) {}
        fclose(rules);

        // READING INIT CONFIG
        FILE *init;

        if ((init = fopen(argv[2], "r")) == NULL) {
            printf("Couldn´t open %s", argv[2]);
            return 1;
        }

        fscanf(init, "%d", &automatonSize);
        grid = (int*) malloc(sizeof(int) * automatonSize * automatonSize);

        char *boolString = malloc(sizeof(char) * automatonSize + 1);
        for (int i = 0; i < automatonSize; i++) {
            fscanf(init, "%s", boolString);
            for (int j = 0; j < automatonSize; j++) {
                grid[i * automatonSize + j] = boolString[j] - '0';
            }
        }
        fclose(init);
    }

    MPI_Barrier(MPI_COMM_WORLD); // Startline for the timer
    local_start = MPI_Wtime();


    MPI_Bcast(&automatonSize, 1, MPI_INT, 0, MPI_COMM_WORLD); // Distributing size of automaton
    int partSize = (automatonSize * automatonSize)/world_size;

    MPI_Bcast(&transformation, 512, MPI_INT, 0, MPI_COMM_WORLD); // Distributing transformation rules

    MPI_Bcast(&nStates, 1, MPI_INT, 0, MPI_COMM_WORLD); // Distributing number of steps

    int *currentSequence = (int*) malloc(sizeof(int) * partSize);
    int *nextSequence = (int *) malloc(sizeof(int) * partSize);

    int *upper = (int *) malloc(sizeof(int) * automatonSize);
    int *lower = (int *) malloc(sizeof(int) * automatonSize);


    for(int state = 0; state < nStates; state++) {
        MPI_Scatter(grid, partSize, MPI_INT, currentSequence, partSize, MPI_INT, 0, MPI_COMM_WORLD); //Distributing vector

        /*if(world_rank == 0) { // Printing current grid
            printf("\nRound %i\n",state+1);
            for(int i = 0; i < automatonSize*automatonSize; i++) {
                if(i % automatonSize == 0) {
                    printf("\n");
                }
                printf("%i ", grid[i]);
            }
            printf("\n");
        }*/

        for(int i = 0; i < automatonSize; i++) { // Distributing vectors
            if(world_rank == 0 && world_size > 1) {
                MPI_Send(&currentSequence[partSize - automatonSize + i], 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&lower[i], 1, MPI_INT, world_rank + 1, 0 , MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if(world_size > 1) {
                MPI_Recv(&upper[i], 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if(world_rank != world_size - 1) {
                    MPI_Send(&currentSequence[partSize - automatonSize + i], 1, MPI_INT, world_rank + 1, 0,
                             MPI_COMM_WORLD);
                }

                MPI_Send(&currentSequence[i], 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD);

                if(world_rank != world_size - 1) {
                    MPI_Recv(&lower[i], 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD,
                             MPI_STATUS_IGNORE);
                }
            }
        }

        for(int i = 0; i < partSize; i++) { // Finding neighbors

            int left;
            if(i % automatonSize == 0) {
                left = 0;
            } else  {
                left = currentSequence[i-1];
            }

            int mid = currentSequence[i];


            int right;
            if((i+1) % automatonSize == 0) {
                right = 0;
            } else {
                right = currentSequence[i+1];
            }




            int upperLeft;
            if(i % automatonSize == 0) {
                upperLeft = 0;
            }
            else if(i - automatonSize - 1 >= 0) {
                upperLeft = currentSequence[i-automatonSize-1];
            } else if (world_rank == 0) {
                upperLeft = 0;
            }
            else {
                upperLeft = upper[(i-1) % automatonSize];
            }

            int upperMid;
            if(i - automatonSize >= 0) {
                upperMid = currentSequence[i-automatonSize];
            } else if(world_rank == 0) {
                upperMid = 0;
            }  else {
                upperMid = upper[i % automatonSize];
            }

            int upperRight;
            if( (i+1) % automatonSize == 0) {
                upperRight = 0;
            } else if(i - automatonSize + 1 >= 0) {
                upperRight = currentSequence[i - automatonSize + 1];
            }
            else if(world_rank == 0) {
                    upperRight = 0;
                }
            else {
                upperRight = upper[(i+1) % automatonSize];
            }


            int lowerLeft;
            if(i % automatonSize == 0) {
                lowerLeft = 0;
            } else if(i + automatonSize - 1 < partSize) {
                lowerLeft = currentSequence[i+automatonSize-1];
            } else if(world_rank == world_size - 1)
            {
                lowerLeft = 0;
            }else {
                lowerLeft = lower[(i-1) % automatonSize];
            }

            int lowerMid;

            if(i + automatonSize < partSize) {
                lowerMid = currentSequence[i+automatonSize];
            } else if(world_rank == world_size - 1) {
                lowerMid = 0;
            }
            else {
                lowerMid = lower[i % automatonSize];
            }

            int lowerRight;
            if((i+1) % automatonSize == 0) {
                lowerRight = 0;
            } else if(i + automatonSize + 1 < partSize) {
                lowerRight = currentSequence[i+automatonSize+1];
            } else if(world_rank == world_size - 1 ) {
                lowerRight = 0;
            }else {
                lowerRight = lower[(i+1) % automatonSize];
            }

            int place = 256 * upperLeft + 128 * upperMid + 64 * upperRight +
                    32 * left + 16 * mid + 8 * right + 4 * lowerLeft + 2 *
                    lowerMid + lowerRight;

            nextSequence[i] = transformation[place];
        }
        MPI_Gather(nextSequence, partSize, MPI_INT, grid, partSize, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Getting the longest runningtime
    local_finish = MPI_Wtime();
    local_elapsed = local_finish - local_start;
    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if(world_rank == 0) {
        printf("\nElapsed time for file: = %e s\n", elapsed);

    }

    MPI_Finalize();
    return 0;
}
