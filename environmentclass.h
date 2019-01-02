#ifndef ENVIRONMENTCLASS_H
#define ENVIRONMENTCLASS_H

#define PI 3.141592654

#include <QMessageBox>
#include <QString>
#include "math.h"

//Definitions of images size
//#define GRID_X 100
//#define GRID_Y 100

//RJG This is the base class that a bunch of the environmental generators inheret, which includes save and random number functions
class environmentclass
{

public:
    environmentclass();
    virtual ~environmentclass();

    virtual void regenerate();

    void save(int generations);

    void path(QString files_directory,  bool doSave);

    //Hard limits of 1000 - right now.
    quint8 environment[1000][1000][3];

    bool error=false;

protected:
    QString dir;
    bool saveMe;
    quint8 Rand8();

private:

};

#endif // ENVIRONMENTCLASS_H
