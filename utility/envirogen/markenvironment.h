#ifndef MARKENVIRONMENT_H
#define MARKENVIRONMENT_H

#include <QColor>
#define MAXOBJ 1000

class markenvironment : public environmentclass
{
public:
    markenvironment();
    void regenerate();


private:
    double RandFloat();
    quint8 add_and_limit(int oldvalue, int color, double dist, double maxdist, double tightness);
    void ReadSettings();
    double objxpos[MAXOBJ];
    double objypos[MAXOBJ];
    double objxvel[MAXOBJ];
    double objyvel[MAXOBJ];
    double objsize[MAXOBJ];
    double objsizevel[MAXOBJ];
    double objcolours[MAXOBJ][3];
    double objcolvel[MAXOBJ][3];
    double objtightness[MAXOBJ];
    double objtightvel[MAXOBJ];
    int objectcount;
    double maxsize, minsize;
    double maxvel, maxsizevel, maxcolvel, maxtightvel, maxtight, mintight, speedfactor;
    double veltweak, sizetweak, coltweak, tighttweak;
    int iter_to_accel;
    int iter_reset;
};


#endif // MARKENVIRONMENT_H
