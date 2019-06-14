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
#include "timer.h"

static int counter = 0;

void insertInTable(int a, int b, int c, int *multiplicationTable, int *inputTable, int tableSize);

int findPlace(int a, int *inputTable, int tableSize);

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

int main(int argc, char *argv[]) {

    // READING THE RULES
    FILE *inputFile;
    if((inputFile = fopen(argv[1], "r")) == NULL) {
        printf("Couldn´t open %s", argv[1]);
        return 1;
    }

    int progSize;
    int sSize;
    int fSize;

    fscanf(inputFile, "%*i %i %i %i", &progSize, &sSize, &fSize);

    int *multiplicationTable = malloc(sizeof(int) * sSize * sSize);

    int *inputTable = malloc(sizeof(int) * sSize);
    for(int i = 0; i < sSize; i++) {
        inputTable[i] = -1;
    }

    for(int i = 0; i < sSize*sSize; i++) {
        int a, b, c;
        fscanf(inputFile, "%i %i %i", &a, &b, &c);
        insertInTable(a, b, c, multiplicationTable, inputTable, sSize);
    }

    int *setF = (int*) malloc(sizeof(int) * fSize);

    for(int i = 0; i < fSize; i++) {
        fscanf(inputFile, "%d", &setF[i]);
    }

    int **setT = (int**) malloc(sizeof(int *) * progSize);
    for(int i = 0; i < progSize; i++) {
        setT[i] = (int*) malloc(sizeof(int) * 3);
        fscanf(inputFile, "%i %i %i", &setT[i][0], &setT[i][1], &setT[i][2]);
    }

    fclose(inputFile);

    int *values = (int*) malloc(sizeof(int) * progSize);
    for(int i = 0; i < progSize; i++) {
        int i_i = setT[i][0];
        values[i] = setT[i][(argv[2][i_i] - '0') + 1]; //argv[2]: inputstring
        //printf("%i ",values[i]);
    }

    //printf("\n");
      //  for(int i = 0; i < sSize*sSize; i++) {
     //       printf("%i ",multiplicationTable[i]);
     //   }
    //printf("\n");

    int currentVal = values[0];
    int currentPlace = 1;

    while(currentPlace != progSize) {
            int a = findPlace(currentVal, inputTable, sSize) ;
            int b = findPlace(values[currentPlace++], inputTable,sSize);
            currentVal = multiplicationTable[a + sSize * b];
    }

    printf("\nValue %i \n",currentVal);

    for(int i = 0; i < fSize; i++) {
        if(currentVal == setF[i]) {
            printf("True");
            return 0;
        }
    }
    printf("False");

    return 0;
}
