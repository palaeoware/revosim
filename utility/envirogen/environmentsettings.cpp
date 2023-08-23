#include "environmentsettings.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

EnvironmentSettings::EnvironmentSettings(MainWindow *mainWin)
{

    x = mainWin->ui->spinSize->value();
    y = mainWin->ui->spinSize->value();
    savePath = mainWin->ui->path->toPlainText(); //Note this will need to be overwritten in many instances by the output of set up save directory
    russellEnvironmentSettings.nSeed = mainWin->ui->numSeed->value();
    russellEnvironmentSettings.buffer = mainWin->ui->buffSpin->value();
    russellEnvironmentSettings.maxSize = mainWin->ui->sMax->value();
    russellEnvironmentSettings.sizeVelocity = mainWin->ui->sMin->value();
    russellEnvironmentSettings.maximumVelocity = mainWin->ui->velMax2->value();
    russellEnvironmentSettings.periodic = mainWin->ui->periodic->isChecked();
    russellEnvironmentSettings.blur = mainWin->ui->blurChk->isChecked();
    russellEnvironmentSettings.converge = mainWin->ui->convergeAt->value();
    russellEnvironmentSettings.numbGenerations = mainWin->ui->numGenerations->value();
    russellEnvironmentSettings.factor = mainWin->ui->factorSpin->value();
    russellEnvironmentSettings.maxAcceleration = 1;
}
