#include "smallPitchTranslation25x100.h"

void SmallPitchTranslation25x100::buildRocToSensorTranslationGrid() {
    int newRow, newCol;

    for (int row = 0; row < 80; row++) {
        for (int col = 0; col < 52; col++) {
            rocToSensorRowGrid[row][col] = -1;
            rocToSensorColGrid[row][col] = -1;
        }
    }

    for (int row = 0; row < 80; row++) {
        for (int col = 0; col < 52; col++) {
            newRow = row;

            if      (col % 4 == 1)
                newCol = 6*(col+1)-1;
            else if (col % 4 == 2 )
                newCol = 6*col;
            else if (col % 4 == 0)
                newCol = 6*(col+2)-2;
            else if (col % 4 == 3)
                newCol = 6*(col-1)+1;

            rocToSensorColGrid[row][col] = newCol;
            rocToSensorRowGrid[row][col] = newRow;
        }
    }
}

void SmallPitchTranslation25x100::buildSensorToRocTranslationGrid() {
    int newRow, newCol;

    for (int row = 0; row < 80; row++) {
        for (int col = 0; col < 312; col++) {
            sensorToRocRowGrid[row][col] = -1;
            sensorToRocColGrid[row][col] = -1;
        }
    }

    for (int row = 0; row < 80; row++) {
        for (int col = 0; col < 312; col++) {
            newRow = row;

            if (col%4 == 3)
                newCol = (col+1)/6-1;
            else if (col%4 == 0)
                newCol = col/6;
            else if (col%4 == 2)
                newCol = (col+2)/6-2;
            else if (col%4 == 1)
                newCol = (col-1)/6+1;
            else {
                newCol = -1;
                newRow = -1;
            }

            sensorToRocColGrid[row][col] = newCol;
            sensorToRocRowGrid[row][col] = newRow;
        }
    }
}

void SmallPitchTranslation25x100::buildClassGrid() {
    int val;

    for (int row = 0; row < 80; row++)
        for (int col = 0; col < 52; col++)
            classGrid[row][col] = 0;

    for (int row = 0; row < 80; row++) {
        for (int col = 0; col < 52; col++) {
            if      (col % 4 == 1)
                val = 10;
            else if (col % 4 == 2)
                val = 9;
            else if (col % 4 == 0)
                val = 1;
            else if (col % 4 == 3)
                val = 5;
            else
                val = 0;

            classGrid[row][col] = val;
        }
    }
}

bool SmallPitchTranslation25x100::isRegularSizedPixel(int col, int row) {
    return (classGrid.at(row).at(col) == 1);
}

bool SmallPitchTranslation25x100::isSmallPixel(int col, int row) {
    return (classGrid.at(row).at(col) == 10 || classGrid.at(row).at(col) == 9);
}

bool SmallPitchTranslation25x100::isCenteredSmallPixel(int col, int row) {
    return (classGrid.at(row).at(col) == 10);
}

bool SmallPitchTranslation25x100::isOffsetSmallPixel(int col, int row) {
    return (classGrid.at(row).at(col) == 9);
}

void SmallPitchTranslation25x100::fromROCToSensorCoords(unsigned int * col, unsigned int * row) {
    unsigned int newCol, newRow;
    newCol = rocToSensorColGrid[*row][*col];
    newRow = rocToSensorRowGrid[*row][*col];
    *col = newCol;
    *row = newRow;
}

void SmallPitchTranslation25x100::fromSensorToRocCoords(unsigned int * col, unsigned int * row) {
    unsigned int newCol, newRow;
    newCol = sensorToRocColGrid.at(*row).at(*col);
    newRow = sensorToRocRowGrid.at(*row).at(*col);
    *col = newCol;
    *row = newRow;
}
