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

int main(int argc, char *argv[]) {
    char files[11][8] = {"k10.txt", "k11.txt", "k12.txt", "k13.txt", "k14.txt", "k15.txt", "k16.txt", "k17.txt", "k18.txt", "k19.txt", "k20.txt"};

    for(int k = 10; k < 21; k++) {

        int nStates = atoi(argv[3]);

        // READING THE RULES
        FILE *rules;
        if ((rules = fopen(argv[1], "r")) == NULL) {
            printf("Couldn´t open %s", argv[1]);
            return 1;
        }
        int c = 0;
        int transformation[8];
        while (fscanf(rules, "%*s %i", &transformation[c++]) != EOF) {}
        fclose(rules);

        // READING INIT CONFIG
        FILE *init;

        if ((init = fopen(files[k-10], "r")) == NULL) {
            printf("Couldn´t open %s", files[k-10]);
            return 1;
        }
        int automatonSize;
        fscanf(init, "%i", &automatonSize);

        char *initTemp = (char *) malloc(sizeof(char) * automatonSize + 1);
        fscanf(init, "%s", initTemp);
        fclose(init);
        int *currentState = (int *) malloc(sizeof(int) * automatonSize);
        for (int i = 0; i < automatonSize; i++) {
            currentState[i] = initTemp[i] - '0';
        }
        free(initTemp);

        // Time the program
        double start, finish;
        GET_TIME(start);

        int *nextState = (int *) malloc(sizeof(int) * automatonSize);

        for (int state = 0; state < nStates; state++) {
            for (int cell = 0; cell < automatonSize; cell++) {
                int left = cell == 0 ? 0 : currentState[cell - 1];
                int right = cell == automatonSize - 1 ? 0 : currentState[cell + 1];
                int mid = currentState[cell];
                nextState[cell] = transformation[left * 4 + mid * 2 + right];
            }
            for (int i = 0; i < automatonSize; i++) {
                currentState[i] = nextState[i];
            }
        }

        GET_TIME(finish);
        printf("Elapsed time for k = %i: %e s\n", k, (finish - start));

        /*for(int i = 0; i < automatonSize; i++) {
            printf("%i ", currentState[i]);
        }
        printf("\n");*/
    }
    return 0;
}