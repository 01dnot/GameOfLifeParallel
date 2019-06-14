#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"


int main(int argc, char *argv[]) {
    char files[7][14] = {"init1024.txt",
                          "init2048.txt",
                          "init4096.txt",
                          "init8192.txt",
                          "init16384.txt",
                          "init32768.txt",
                          "init65536.txt"};

    for(int k = 0; k < 7; k++) {

        int nStates = atoi(argv[3]);

        // READING THE RULES
        FILE *rules;
        if ((rules = fopen(argv[1], "r")) == NULL) {
            printf("Couldn´t open %s", argv[1]);
            return 1;
        }

        int transformation[512];
        int c = 0;

        while (fscanf(rules, "%*s %i", &transformation[c++]) != EOF) {}
        fclose(rules);

        // READING INIT CONFIG
        FILE *init;

        if ((init = fopen(files[k], "r")) == NULL) {
            printf("Couldn´t open %s", files[k]);
            return 1;
        }

        int automatonSize;
        fscanf(init, "%d", &automatonSize);

        int **grid = (int **) malloc(sizeof(int *) * automatonSize);
        for (int i = 0; i < automatonSize; i++) {
            grid[i] = (int *) malloc(sizeof(int) * automatonSize);
        }

        char *boolString = malloc(sizeof(char) * automatonSize + 1);
        for (int i = 0; i < automatonSize; i++) {
            fscanf(init, "%s", boolString);
            for (int j = 0; j < automatonSize; j++) {
                grid[i][j] = boolString[j] - '0';
            }
        }

        fclose(init);


        int **nextSequence = (int **) malloc(sizeof(int *) * automatonSize);
        for (int i = 0; i < automatonSize; i++) {
            nextSequence[i] = (int *) malloc(sizeof(int) * automatonSize);
        }

        // Time the program
        double start, finish;
        GET_TIME(start);


        for (int state = 0; state < nStates; state++) {
            for (int x = 0; x < automatonSize; x++) {
                for (int y = 0; y < automatonSize; y++) {
                    int upperLeft = x - 1 < 0 || y - 1 < 0 ? 0 : grid[x - 1][y - 1];
                    int upperMid = x - 1 < 0 ? 0 : grid[x - 1][y];
                    int upperRight = x - 1 < 0 || y + 1 >= automatonSize ? 0 : grid[x - 1][y + 1];

                    int left = y - 1 < 0 ? 0 : grid[x][y - 1];
                    int mid = grid[x][y];
                    int right = y + 1 >= automatonSize ? 0 : grid[x][y + 1];

                    int downLeft = x + 1 >= automatonSize || y - 1 < 0 ? 0 : grid[x + 1][y - 1];
                    int downMid = x + 1 >= automatonSize ? 0 : grid[x + 1][y];
                    int downRight = x + 1 >= automatonSize || y + 1 >= automatonSize ? 0 : grid[x + 1][y + 1];

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

            for (int i = 0; i < automatonSize; i++) {
                for (int j = 0; j < automatonSize; j++) {
                    grid[i][j] = nextSequence[i][j];
                }
            }
        }

        GET_TIME(finish);
        printf("For file %s: %e s\n", files[k], (finish - start));
    }
    return 0;
}
