#ifndef SMALLPITCHTRANSLATION25x100_H
#define SMALLPITCHTRANSLATION25x100_H

#include "smallPitchTranslationBase.h"
#include <iostream>

class SmallPitchTranslation25x100 : SmallPitchTranslationBase
{
public:
    SmallPitchTranslation25x100() :
        classGrid(vector<vector<int>>(80, vector<int>(52))),
        sensorToRocRowGrid(vector<vector<int>>(80, vector<int>(312))),
        sensorToRocColGrid(vector<vector<int>>(80, vector<int>(312)))
    {
        buildClassGrid();
        buildSensorToRocTranslationGrid();
        buildRocToSensorTranslationGrid();
    }

    bool isRegularSizedPixel(int col, int row);
    bool isSmallPixel(int col, int row);
    bool isCenteredSmallPixel(int col, int row);
    bool isOffsetSmallPixel(int col, int row);
    void fromROCToSensorCoords(unsigned int *col, unsigned int *row);
    void fromSensorToRocCoords(unsigned int *col, unsigned int *row);

private:
    vector<vector<int>> classGrid; //= vector<vector<int>>(80, vector<int>(52, 1));
    vector<vector<int>> sensorToRocRowGrid; //= vector<vector<int>>(80, vector<int>(312));
    vector<vector<int>> sensorToRocColGrid; // = vector<vector<int>>(80, vector<int>(312));
    void buildClassGrid();
    void buildSensorToRocTranslationGrid();
    void buildRocToSensorTranslationGrid();
};

#endif // SMALLPITCHTRANSLATION25x100_H
