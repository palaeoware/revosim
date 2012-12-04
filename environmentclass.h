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
    virtual void buff_change(int value);
    virtual void nseed_change(int value);
    virtual void maxsize_change(int value);
    virtual void sizevel_change(int value);
    virtual void maxvel_change(double value);
    virtual void maxcvel_change(int value);
    virtual void periodic_change(bool value);
    virtual void blur_change(bool value);
    virtual void converge_change(double value);
    virtual void numbGenerations_change(int value);
    virtual void initialiseSeeds(int number);
    virtual void fact_change(double value);


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
    void maxvel_change(double value);
    void maxcvel_change(int value);
    void periodic_change(bool value);
    void blur_change(bool value);
    void converge_change(double value);
    void buff_change (int value);
    void numbGenerations_change(int value);
    void initialiseSeeds(int number);
    void fact_change(double value);

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
