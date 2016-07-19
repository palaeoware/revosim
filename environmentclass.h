#ifndef ENVIRONMENTCLASS_H
#define ENVIRONMENTCLASS_H

#define PI 3.141592654

#include <QMessageBox>
#include <QString>
#include "math.h"

//Definitions of images size
//#define GRID_X 100
//#define GRID_Y 100

class environmentclass
{

public:
    environmentclass();
    virtual void regenerate();

    bool accessSave();
    void save(int generations);

    void path(QString files_directory,  bool doSave);

    //Hard limits of 1000 - right now.
    quint8 environment[1000][1000][3];

protected:
    QString dir;
    bool saveMe;

private:

};

class russellenvironment : public environmentclass
{
public:
    russellenvironment();
    void regenerate();

private:
    typedef struct
    {
            double n,m;
            double colour[3];
            double size;
            double nv, mv;
    } seed;

    seed seeds[1000];

    void laplace();
    void readSettings();

    int Rand8();
    int buffer;
    int nseed;

    int maxsize;

    int sizevel;

    float maxvel;
    int minvel;

    int maxacc;

    int maxcvel;
    bool periodic;
    bool blur;
    double converge;
    double factor;
    int numbGenerations;

    float na, ma;

};

#endif // ENVIRONMENTCLASS_H
