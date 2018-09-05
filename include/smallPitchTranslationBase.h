#ifndef SMALLPITCHTRANSLATIONBASE_H
#define SMALLPITCHTRANSLATIONBASE_H

#include <vector>

using std::vector;


class SmallPitchTranslationBase
{
public:
    virtual bool isRegularSizedPixel(int col, int row) = 0;
    virtual bool isSmallPixel(int col, int row) = 0;
    virtual void fromROCToSensorCoords(unsigned int *col, unsigned int *row) = 0;

protected:
//    vector<vector<int>> classGrid = vector<vector<int>>(80, vector<int>(52));
    int rocToSensorRowGrid[80][52];
    int rocToSensorColGrid[80][52];
    virtual void buildClassGrid() = 0;
    virtual void buildSensorToRocTranslationGrid() = 0;
    virtual void buildRocToSensorTranslationGrid() = 0;
};

#endif // SMALLPITCHTRANSLATIONBASE_H
