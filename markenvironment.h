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
    quint8 Rand8();
    float RandFloat();
    quint8 add_and_limit(int oldvalue, int color, float dist, float maxdist, float tightness);
    void ReadSettings();
    float objxpos[MAXOBJ];
    float objypos[MAXOBJ];
    float objxvel[MAXOBJ];
    float objyvel[MAXOBJ];
    float objsize[MAXOBJ];
    float objsizevel[MAXOBJ];
    float objcolours[MAXOBJ][3];
    float objcolvel[MAXOBJ][3];
    float objtightness[MAXOBJ];
    float objtightvel[MAXOBJ];
    int objectcount;
    float maxsize, minsize;
    float maxvel,maxsizevel,maxcolvel,maxtightvel, maxtight, mintight, speedfactor;
    float veltweak, sizetweak, coltweak, tighttweak;
    int iter_to_accel;
    int iter_reset;
};


#endif // MARKENVIRONMENT_H
