#ifndef IMAGESEQUENCE_H
#define IMAGESEQUENCE_H

#include <QStringList>
#include <QImage>
#include "globals.h"

class ImageSequence
{
public:
    ImageSequence(QStringList files, int rate);
    bool regenerate(int eMode, bool interpolate);
    void modifyChangeRate(int rate);
    void reset(int counter);
    quint8 *returnRGB(int n, int m);
    void setRGB(int n, int m, quint8 *rgb);
    int returnCurrentFileNumber();
    int returnFileListCount();
    int returnChangeRate();
    QStringList returnFileList();
    void setCurrentFileNumber(int n);

private:
    void loadFromFile(int eMode);

    QStringList fileList;
    QImage burnInImage;

    int currentFile;
    int changeCounter;
    int nextChange;
    int changeRate;

    quint8 grid[GRID_X][GRID_Y][3];  //0 = red, 1 = green, 2 = blue
    quint8 gridLast[GRID_X][GRID_Y][3];  //Used for interpolation
    quint8 gridNext[GRID_X][GRID_Y][3];  //Used for interpolation

    bool changeForward;
};

#endif // IMAGESEQUENCE_H
