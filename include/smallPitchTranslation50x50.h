#ifndef SMALLPITCHTRANSLATION50x50_H
#define SMALLPITCHTRANSLATION50x50_H

#include "smallPitchTranslationBase.h"

class SmallPitchTranslation50x50 : SmallPitchTranslationBase
{
public:
    SmallPitchTranslation50x50() :
        classGrid(80, vector<int>(52)),
        sensorToRocRowGrid(160, vector<int>(160)),
        sensorToRocColGrid(160, vector<int>(160))
    {
        buildClassGrid();
        buildRocToSensorTranslationGrid();
    };
    bool isRegularSizedPixel(int col, int row);
    bool isCentralSmallPixel(int col, int row);
    bool isCornerSmallPixel(int col, int row);
    bool isSmallPixel(int col, int row);
    bool isBeginningRegularSizedPixel(int col, int row);
    void fromROCToSensorCoords(unsigned int *col, unsigned int *row);

private:
    vector<vector<int>> classGrid; //= vector<vector<int>>(80, vector<int>(52, 1));
    vector<vector<int>> sensorToRocRowGrid; //= vector<vector<int>>(80, vector<int>(312));
    vector<vector<int>> sensorToRocColGrid; // = vector<vector<int>>(80, vector<int>(312));
    void buildClassGrid();
    void buildRocToSensorTranslationGrid();
    void buildSensorToRocTranslationGrid() { }
};

#endif // SMALLPITCHTRANSLATION50x50_H
