#ifndef RUSSELLENVIRONMENT_H
#define RUSSELLENVIRONMENT_H


//RJG - This is Russell's environmental class which inherets environment class (above).
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
            bool initialised=false;
    } seed;

    seed seeds[1000];

    void laplace();
    void readSettings();

    int buffer;
    int nseed;

    int maxsize;

    int sizevel;

    float maxvel;
    int maxacc;

    int maxcvel;
    bool periodic;
    bool blur;
    double converge;
    double factor;
    int numbGenerations;

    float na, ma;

};


#endif // RUSSELLENVIRONMENT_H
