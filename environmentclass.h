#ifndef ENVIRONMENTCLASS_H
#define ENVIRONMENTCLASS_H

#define PI 3.141592654
#include <QMessageBox>
#include <QString>
#include "math.h"


#define GRID_X 256
#define GRID_Y 256

class environmentclass
{

public:
    environmentclass();
    virtual void regenerate();
    virtual void settings();
    virtual void nseed_change(int value);
    virtual void maxsize_change(int value);
    virtual void sizevel_change(int value);
    virtual void maxvel_change(int value);
    virtual void maxcvel_change(int value);
    virtual void periodic_change(bool value);
    virtual void converge_change(double value);
    virtual void numbGenerations_change(int value);
    virtual void initialiseSeeds(int number);

    bool accessSave();
    void save(int generations);

    void path(QString files_directory,  bool doSave);

    quint8 environment[GRID_X][GRID_Y][3];

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
    void nseed_change(int value);
    void maxsize_change(int value);
    void sizevel_change(int value);
    void maxvel_change(int value);
    void maxcvel_change(int value);
    void periodic_change(bool value);
    void converge_change(double value);
    void numbGenerations_change(int value);
    void initialiseSeeds(int number);

private:
    typedef struct
    {
            int n,m;
            double colour[3];
            double size;
    } seed;

    seed seeds[1000];

    void laplace();

    int Rand8();
    int nseed;
    int maxsize;
    int sizevel;
    int maxvel;
    int maxcvel;
    bool periodic;
    double converge;
    int numbGenerations;

};

#endif // ENVIRONMENTCLASS_H
