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

int *inputTable;
int *multiplicationTable;
int sSize;


static int counter = 0;

void insertInTable(int a, int b, int c, int *multiplicationTable, int *inputTable, int tableSize);

int findPlace(int a, int *inputTable, int tableSize);

void customSum (int *, int *, int *, MPI_Datatype *);

int findPlace(int a, int *inputTable, int tableSize) {
    int position = -1;
    for(int i = 0; i < tableSize; i++) {
        if(a == inputTable[i]) {
            position = i;
        }
    }
    return position;

}

void insertInTable(int a, int b, int c, int *multiplicationTable, int *inputTable, int tableSize) {
    int aPos = -1, bPos = -1;
    for(int i = 0; i < tableSize; i++ ) {
        if(a == inputTable[i]) {
            aPos = i;
        }
        if(b == inputTable[i]) {
            bPos = i;
        }
    }
    if(aPos == -1 && a == b) {
        aPos = counter;
        bPos = counter;
        inputTable[counter++] = a;
    } else {
        if (aPos == -1) {
            aPos = counter;
            inputTable[counter++] = a;
        }
        if (bPos == -1) {
            bPos = counter;
            inputTable[counter++] = b;
        }
    }
    multiplicationTable[aPos + tableSize * bPos] = c;
}


void customSum(int *in, int *inout, int *len, MPI_Datatype *dtype) {
    *inout = multiplicationTable[*in + sSize * *inout];
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);


    int nVariables;
    int progSize;
    int fSize;
    int *setF = NULL;
    int *setT = NULL;

    if(world_rank == 0) { // Process 0 reads input
        FILE *inputFile;
        if((inputFile = fopen(argv[1], "r")) == NULL) {
            printf("Couldn´t open %s", argv[1]);
            return 1;
        }

        fscanf(inputFile, "%i %i %i %i", &nVariables, &progSize, &sSize, &fSize);

        multiplicationTable = malloc(sizeof(int) * sSize * sSize);

        inputTable = malloc(sizeof(int) * sSize);
        for(int i = 0; i < sSize; i++) {
            inputTable[i] = -1; //TODO: make it possible to use -1 vallues.
        }

        for(int i = 0; i < sSize*sSize; i++) {
            int a, b, c;
            fscanf(inputFile, "%i %i %i", &a, &b, &c);
            insertInTable(a, b, c, multiplicationTable, inputTable, sSize);
        }

        setF = (int*) malloc(sizeof(int) * fSize);
        for(int i = 0; i < fSize; i++) {
            fscanf(inputFile, "%d", &setF[i]);
        }

        setT = (int*) malloc(sizeof(int) * 3 * progSize);
        for(int i = 0; i < progSize * 3; i++) {
            fscanf(inputFile, "%i", &setT[i]);
        }
        fclose(inputFile);
    }

    MPI_Bcast(&progSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int partSize = progSize/world_size;

    MPI_Bcast(&sSize, 1, MPI_INT,0,MPI_COMM_WORLD);

    if(world_rank != 0) {
        multiplicationTable = (int*) malloc(sizeof(int) * sSize*sSize);
        inputTable = (int*) malloc(sizeof(int) * sSize);
    }
    MPI_Bcast(multiplicationTable, sSize*sSize, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(inputTable, sSize, MPI_INT, 0, MPI_COMM_WORLD);

    int *local = (int*) malloc(sizeof(int) * partSize * 3);
    MPI_Scatter(setT, partSize * 3, MPI_INT, local, partSize * 3, MPI_INT, 0, MPI_COMM_WORLD);

    int *myValues = (int*) malloc(sizeof(int) * partSize);

    for(int i = 0; i < partSize; i++) {
        int i_i = local[i * 3];
        myValues[i] = local[i * 3 + (argv[2][i_i] - '0') + 1]; //argv[2]: inputstring
       //printf("%i ", myValues[i]);
    }

    int localResult = myValues[0];
    int currentPlace = 1;

    while(currentPlace != partSize) {
        int a = findPlace(localResult, inputTable, sSize) ;
        int b = findPlace(myValues[currentPlace++], inputTable,sSize);
        localResult = multiplicationTable[a + sSize * b];
    }

    int globalResult;
    MPI_Op op;
    MPI_Op_create( (MPI_User_function *)customSum, 1, &op );

    MPI_Reduce(&localResult, &globalResult, 1, MPI_INT, op, 0, MPI_COMM_WORLD);

    MPI_Op_free(&op);
    MPI_Finalize();

    if(world_rank == 0) {
        printf("Value %i \n", globalResult);

        for(int i = 0; i < fSize; i++) {
            if(globalResult == setF[i]) {
                printf("True\n");
                return 0;
            }
        }
        printf("False\n");
    }

    return 0;
}
