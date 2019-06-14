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

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int automatonSize;
    int *currentState = NULL;
    int transformation[8];
    int nStates;

    if(world_rank == 0) {
        nStates = atoi(argv[3]);

        // READING THE RULES
        FILE *rules;
        if ((rules = fopen(argv[1], "r")) == NULL) {
            printf("Couldn´t open %s", argv[1]);
            return 1;
        }

        int i = 0;
        while (fscanf(rules, "%*s %i", &transformation[i++]) != EOF) {}
        fclose(rules);

        // READING INIT CONFIG
        FILE *init;

        if ((init = fopen(argv[2], "r")) == NULL) {
            printf("Couldn´t open %s", argv[2]);
            return 1;
        }

        fscanf(init, "%i", &automatonSize);
        char *initTemp = (char *) malloc(sizeof(char) * automatonSize + 1);
        fscanf(init, "%s", initTemp);

        currentState = (int *) malloc(sizeof(int) * automatonSize);
        for(int i = 0; i  < automatonSize; i++) {
            currentState[i] = initTemp[i] - '0';
        }
        free(initTemp);

        fclose(init);
    }

    MPI_Bcast(&automatonSize, 1, MPI_INT, 0, MPI_COMM_WORLD); // Distributing size of automaton
    int partSize = automatonSize/world_size;

    MPI_Bcast(&transformation, 8, MPI_INT, 0, MPI_COMM_WORLD); // Distributing transformation rules

    MPI_Bcast(&nStates, 1, MPI_INT, 0, MPI_COMM_WORLD); // Distributing number of steps

    int *currentPart = (int *) malloc(sizeof(int) * partSize);
    int *nextPart = (int *) malloc(sizeof(int) * partSize);

    for(int state = 0; state < nStates; state++) {
        MPI_Scatter(currentState, partSize, MPI_INT, currentPart, partSize, MPI_INT, 0, MPI_COMM_WORLD); //Distributing vector

        int outerLeft = 0;
        int outerRight = 0;
        if(world_rank == 0 && world_size >  1) {
            MPI_Send(&currentPart[partSize - 1], 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&outerRight, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else if(world_rank == world_size - 1 && world_size > 1) {
            MPI_Send(&currentPart[0], 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&outerLeft, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        } else if(world_size > 1) {
            MPI_Recv(&outerLeft, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&currentPart[0], 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD);
            MPI_Send(&currentPart[partSize - 1], 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&outerRight, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        for(int cell = 0; cell < partSize; cell++) {
            int left;
            int right;

            if (cell == 0 && world_rank == 0) {
                left = 0;
            } else if (cell == 0) {
                left = outerLeft;
            } else {
                left = currentPart[cell-1];
            }

            int mid = currentPart[cell];


            if (cell == partSize - 1 && world_rank == world_size - 1) {
                right = 0;
            } else if (cell == partSize - 1) {
                right = outerRight;
            } else {
                right = currentPart[cell + 1];
            }

            nextPart[cell] = transformation[left * 4 + mid * 2 + right];
        }
        MPI_Gather(nextPart, partSize, MPI_INT, currentState, partSize, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    if(world_rank == 0) {
        for(int i = 0; i < automatonSize; i++) {
            printf("%i ", currentState[i]);
        }
        printf("\n");
    }
    return 0;
}
