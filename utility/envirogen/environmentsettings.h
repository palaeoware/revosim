#ifndef ENVIRONMENTSETTINGS_H
#define ENVIRONMENTSETTINGS_H

#include <QMainWindow>

class MainWindow; //Forward decleration to avoid circular dependencies in header

class EnvironmentSettings
{
public:
    EnvironmentSettings(MainWindow *mainWin);

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

    };
};

#endif // ENVIRONMENTSETTINGS_H
