#include "smallPitchTranslation50x50.h"

//SmallPitchTranslation50x50::SmallPitchTranslation()
//{
//    buildClassGrid50x50();
//    buildTranslationGrid50x50();
//}

void SmallPitchTranslation50x50::buildRocToSensorTranslationGrid() {
    int newRow, newCol;

    for (int row = 0; row < 80; row++) {
        for (int col = 0; col < 52; col++) {
            rocToSensorRowGrid[row][col] = -1;
            rocToSensorColGrid[row][col] = -1;
        }
    }

    for (int row = 0; row < 80; row++) {
        for (int col = 0; col < 52; col++) {

            if (col <= 3) continue;

            if ((col-3) % 6 == 2 || (col-3) % 6 == 3) {

                if      ((col-3) % 6 == 2) newCol = 3*3 + ((col-3)/6)*18 + 8;
                else if ((col-3) % 6 == 3) newCol = 3*3 + ((col-3)/6)*18 + 9;

                if (row >= 78) continue;

                if      (row % 6 == 1) newRow = (row/6)*12 + 4;
                else if (row % 6 == 2) newRow = (row/6)*12 + 5;
                else if (row % 6 == 3) newRow = (row/6)*12 + 6;
                else if (row % 6 == 4) newRow = (row/6)*12 + 7;
                else
                    continue;
            }
            else if ((col-3) % 6 == 1 || (col-3) % 6 == 4) {

                if      ((col-3) % 6 == 1) newCol = 3*3 + ((col-3)/6)*18 + 7;
                else if ((col-3) % 6 == 4) newCol = 3*3 + ((col-3)/6)*18 + 10;

                if (row >= 78) continue;

                if      (row % 6 == 0) newRow = (row/6)*12 + 4;
                else if (row % 6 == 2) newRow = (row/6)*12 + 5;
                else if (row % 6 == 3) newRow = (row/6)*12 + 6;
                else if (row % 6 == 5) newRow = (row/6)*12 + 7;
                else
                    continue;
            }
            else
                continue;

            rocToSensorRowGrid[row][col] = newRow;
            rocToSensorColGrid[row][col] = newCol;
        }
    }
}

void SmallPitchTranslation50x50::buildClassGrid() {
    int val;

    for (int row = 0; row < 80; row++)
        for (int col = 0; col < 52; col++)
            classGrid[row][col] = 0;

    for (int row = 0; row < 80; row++) {
        for (int col = 0; col < 52; col++) {
            if (((col-3) % 6 == 2 || (col-3) % 6 == 3) && col >= 3) {
                if (row % 6 == 2 || row % 6 == 3)
                    val = 10;
                else if (row % 6 == 1 || row % 6 == 4)
                    val = 9;
                else if (row % 6 == 0 || row % 6 == 5)
                    val = 3;
            }
            else if (((col-3) % 6 == 1 || (col-3) % 6 == 4) && col >= 3) {
                if (row % 6 == 2 || row % 6 == 3)
                    val = 8;
                else if (row % 6 == 1 || row % 6 == 4)
                    val = 4;
                else if (row % 6 == 0 || row % 6 == 5)
                    val = 7;
            }
            else if (((col-3) % 6 == 0 || (col-3) % 6 == 5) && col >= 3) {
                if (row % 6 == 0 || row % 6 == 1 || row % 6 == 4 || row % 6 == 5)
                    val = 1;
                else if (row % 6 == 2 || row % 6 == 3)
                    val = 5;
            }
            else if (col < 3)
                val = 1;
            else
                val = 0;

            classGrid[row][col] = val;
        }
    }
}

bool SmallPitchTranslation50x50::isRegularSizedPixel(int col, int row) {
    return (classGrid[row][col] == 1);
}

bool SmallPitchTranslation50x50::isCentralSmallPixel(int col, int row) {
    return (classGrid[row][col] == 10);
}

bool SmallPitchTranslation50x50::isCornerSmallPixel(int col, int row) {
    return (classGrid[row][col] == 7);
}

bool SmallPitchTranslation50x50::isSmallPixel(int col, int row) {
    return (classGrid[row][col] == 10 ||
            classGrid[row][col] == 9  ||
            classGrid[row][col] == 8  ||
            classGrid[row][col] == 7  );
}

bool SmallPitchTranslation50x50::isBeginningRegularSizedPixel(int col, int row) {
    return (/*classGrid50x50[row][col] == 1 &&*/ col < 3);
}

void SmallPitchTranslation50x50::fromROCToSensorCoords(unsigned int * col, unsigned int * row) {
    unsigned int newCol, newRow;
    newCol = rocToSensorColGrid[*row][*col];
    newRow = rocToSensorRowGrid[*row][*col];
    *col = newCol;
    *row = newRow;
}

