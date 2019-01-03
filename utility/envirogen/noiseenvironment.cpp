#include "environmentclass.h"
#include "noiseenvironment.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <time.h>

noiseenvironment::noiseenvironment()
{
    qsrand(time(NULL));
    ReadSettings();
}


void noiseenvironment::ReadSettings()
{
    nMin = MainWin->ui->noiseMin->value();
    nMax = MainWin->ui->noiseMax->value();
}

void noiseenvironment::regenerate()
{
    ReadSettings();

    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
        {
            for (int i = 0; i < 3; i++)
            {
                quint8 noiseVal;

                do
                {
                    noiseVal = Rand8();
                }
                while (noiseVal < nMin || noiseVal > nMax);

                environment[n][m][i] = noiseVal;
            }
        }

}
