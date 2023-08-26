#ifndef ENVIRONMENTSETTINGS_H
#define ENVIRONMENTSETTINGS_H

#include <QMainWindow>

class MainWindow; //Forward decleration to avoid circular dependencies in header

class EnvironmentSettings
{
public:
    EnvironmentSettings(MainWindow *mainWin);

    int x, y, generations;
    QString savePath;
    bool batch = false;
    MainWindow *mainWindowPointer;

    struct settingsRussell
    {
        int buffer;
        int nSeed;
        int maxSize;
        int sizeVelocity;
        int maxAcceleration;
        int maxColourVelocity;
        int numbGenerations;

        double converge;
        double factor;
        double maximumVelocity;

        bool periodic;
        bool blur;

        double na, ma;
    };

    settingsRussell russellEnvironmentSettings;

    struct settingsMark
    {
        int objectcount;
        double maxSize, minsize;
        double maximumVelocity, maxsizeVelocity, maxcolvel, maxtightvel, maxtight, mintight, speedfactor;
        double veltweak, sizetweak, coltweak, tighttweak;
        int iter_to_accel;
        int iter_reset;
    };
    settingsMark markEnvironmentSettings;

    struct settingsNoise
    {
        int nMin;
        int nMax;
    };
    settingsNoise noiseEnvironmentSettings;

    struct settingsColour
    {

        int colourRed;
        int colourGreen;
        int colourBlue;
    };
    settingsColour colourEnvironmentSettings;

    struct settingsMakeStack
    {
        QString fileName;
    };
    settingsMakeStack makeStackSettings;
};

#endif // ENVIRONMENTSETTINGS_H
