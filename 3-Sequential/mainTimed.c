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

    char files[7][16] = {"branch1000.txt",
                         "branch2000.txt",
                         "branch4000.txt",
                         "branch8000.txt",
                         "branch16000.txt",
                         "branch32000.txt",
                         "branch64000.txt"};

    char input[7][16] = {"input1000.txt",
                         "input2000.txt",
                         "input4000.txt",
                         "input8000.txt",
                         "input16000.txt",
                         "input32000.txt",
                         "input64000.txt"};

    for(int k = 0; k < 7; k++) {

        // READING THE RULES
        FILE *inputFile;
        if ((inputFile = fopen(files[k], "r")) == NULL) {
            printf("Couldn´t open %s", files[k]);
            return 1;
        }

        int progSize;
        int sSize;
        int fSize;

        fscanf(inputFile, "%*i %i %i %i", &progSize, &sSize, &fSize);

        int *multiplicationTable = malloc(sizeof(int) * sSize * sSize);

        int *inputTable = malloc(sizeof(int) * sSize);
        for (int i = 0; i < sSize; i++) {
            inputTable[i] = -1;
        }

        for (int i = 0; i < sSize * sSize; i++) {
            int a, b, c;
            fscanf(inputFile, "%i %i %i", &a, &b, &c);
            insertInTable(a, b, c, multiplicationTable, inputTable, sSize);
        }

        int *setF = (int *) malloc(sizeof(int) * fSize);

        for (int i = 0; i < fSize; i++) {
            fscanf(inputFile, "%d", &setF[i]);
        }

        int **setT = (int **) malloc(sizeof(int *) * progSize);
        for (int i = 0; i < progSize; i++) {
            setT[i] = (int *) malloc(sizeof(int) * 3);
            fscanf(inputFile, "%i %i %i", &setT[i][0], &setT[i][1], &setT[i][2]);
        }

        fclose(inputFile);

        FILE *stringFile;
        if ((stringFile = fopen(input[k], "r")) == NULL) {
            printf("Couldn´t open %s", input[k]);
            return 1;
        }
        char * boolInput = malloc(sizeof(char)*progSize + 1);
        fscanf(stringFile, "%s", boolInput);
        fclose(stringFile);

        // Time the program
        double start, finish;
        GET_TIME(start);

        int *values = (int *) malloc(sizeof(int) * progSize);
        for (int i = 0; i < progSize; i++) {
            int i_i = setT[i][0];
            values[i] = setT[i][(boolInput[i_i] - '0') + 1];
        }

        int currentVal = values[0];
        int currentPlace = 1;

        while (currentPlace != progSize) {
            int a = findPlace(currentVal, inputTable, sSize);
            int b = findPlace(values[currentPlace++], inputTable, sSize);
            currentVal = multiplicationTable[a + sSize * b];
        }

        GET_TIME(finish);
        printf("For file %s: %e s\n", files[k], (finish - start));

        free(multiplicationTable);
        free(inputTable);
        free(setF);
        free(setT);
        free(values);
        free(boolInput);
        counter = 0;
    }
    return 0;
}
