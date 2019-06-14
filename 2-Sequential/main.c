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


int main(int argc, char *argv[]) {

    int nStates = atoi(argv[3]);

    // READING THE RULES
    FILE *rules;
    if((rules = fopen(argv[1], "r")) == NULL) {
        printf("Couldn´t open %s", argv[1]);
        return 1;
    }

    int transformation[512];
    int c = 0;

    while(fscanf(rules, "%*s %i", &transformation[c++]) != EOF) {}
    fclose(rules);

    // READING INIT CONFIG
    FILE *init;

    if((init = fopen(argv[2], "r")) == NULL) {
        printf("Couldn´t open %s", argv[2]);
        return 1;
    }

    int automatonSize;
    fscanf(init, "%d", &automatonSize);

    int **grid = (int**) malloc(sizeof(int*) * automatonSize);
    for(int i = 0; i < automatonSize; i++) {
        grid[i] = (int *) malloc(sizeof(int) * automatonSize);
    }

    char *boolString = malloc(sizeof(char) * automatonSize + 1);
    for(int i = 0; i < automatonSize; i++) {
        fscanf(init, "%s", boolString);
        for(int j = 0; j < automatonSize; j++) {
            grid[i][j] = boolString[j] - '0';
        }
    }
    free(boolString);


    fclose(init);


    int **nextSequence = (int**) malloc(sizeof(int*) * automatonSize);
    for(int i = 0; i < automatonSize; i++) {
        nextSequence[i] = (int *) malloc(sizeof(int) * automatonSize);
    }

    for(int i = 0; i < automatonSize; i++) {
        for(int j = 0; j < automatonSize; j++) {
            printf("%i ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");


    for(int state = 0; state < nStates; state++) {
        for(int x = 0; x < automatonSize; x++) {
            for(int y = 0; y < automatonSize; y++) {
                int upperLeft = x - 1 < 0 || y - 1 < 0 ? 0 : grid[x-1][y-1];
                int upperMid = x - 1 < 0 ? 0 : grid[x-1][y];
                int upperRight = x - 1 < 0 || y + 1 >= automatonSize ? 0 : grid[x-1][y+1];

                int left = y - 1 < 0 ? 0 : grid[x][y-1];
                int mid = grid[x][y];
                int right = y + 1 >= automatonSize ? 0 : grid[x][y+1];

                int downLeft = x + 1 >= automatonSize || y - 1 < 0 ? 0 : grid[x+1][y-1];
                int downMid = x + 1 >= automatonSize ? 0 : grid[x+1][y];
                int downRight = x + 1 >= automatonSize || y + 1 >= automatonSize ? 0 : grid[x+1][y+1];

                nextSequence[x][y] = transformation[256 * upperLeft +
                                                    128 * upperMid +
                                                    64 * upperRight +
                                                    32 * left +
                                                    16 * mid +
                                                    8 * right +
                                                    4 * downLeft +
                                                    2 * downMid +
                                                    downRight];
            }
        }

        for(int i = 0; i < automatonSize; i++) {
            for(int j = 0; j < automatonSize; j++) {
                grid[i][j] = nextSequence[i][j];
            }
        }
        printf("Round %i\n", state+2);
        for(int i = 0; i < automatonSize; i++) {
            for(int j = 0; j < automatonSize; j++) {
                printf("%i ", grid[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    printf("Final state:\n");
    for(int x = 0; x < automatonSize; x++) {
        for(int y = 0; y < automatonSize; y++) {
            printf("%i ", grid[x][y]);
        }
        printf("\n");
    }
    free(grid);
    free(nextSequence);

    return 0;
}
