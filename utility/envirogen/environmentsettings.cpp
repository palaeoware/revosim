#include "environmentsettings.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

EnvironmentSettings::EnvironmentSettings(MainWindow *mainWin)
{

    //All environments
    x = mainWin->ui->spinSize->value();
    y = mainWin->ui->spinSize->value();
    generations = mainWin->ui->numGenerations->value();
    savePath = mainWin->ui->path->toPlainText(); //Note this will need to be overwritten in many instances by the output of set up save directory
    mainWindowPointer = mainWin;

    //Russell environment
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

    //Mark environment
    markEnvironmentSettings.objectcount = mainWin->ui->ME_Objects->value();
    markEnvironmentSettings.maxSize = (double)(mainWin->ui->ME_maxSize->value());
    markEnvironmentSettings.minsize = (double)(mainWin->ui->ME_MinSize->value());
    markEnvironmentSettings.maximumVelocity = mainWin->ui->ME_MaxVel->value();
    markEnvironmentSettings.maxsizeVelocity = mainWin->ui->ME_maxSizeVel->value();
    markEnvironmentSettings.maxcolvel = mainWin->ui->ME_MaxColVel->value();
    markEnvironmentSettings.maxtightvel = mainWin->ui->ME_MaxTightVel->value();
    markEnvironmentSettings.maxtight = mainWin->ui->ME_MaxTight->value();
    markEnvironmentSettings.mintight = mainWin->ui->ME_MinTight->value();
    markEnvironmentSettings.speedfactor = mainWin->ui->ME_SpeedFactor->value();
    markEnvironmentSettings.tighttweak = mainWin->ui->ME_MaxTightAccel->value();
    markEnvironmentSettings.coltweak = mainWin->ui->ME_MaxColAccel->value();
    markEnvironmentSettings.veltweak = mainWin->ui->ME_MaxAccel->value();
    markEnvironmentSettings.sizetweak = mainWin->ui->ME_maxSizeAccel->value();
    markEnvironmentSettings.iter_reset = mainWin->ui->ME_Iterations->value();
    markEnvironmentSettings.toroidal = mainWin->ui->ME_Toroidal->isChecked();

    //Noise environment
    noiseEnvironmentSettings.nMin = mainWin->ui->noiseMin->value();
    noiseEnvironmentSettings.nMax = mainWin->ui->noiseMax->value();

    //Colour environment
    colourEnvironmentSettings.colourRed = mainWin->ui->spinRed->value();
    colourEnvironmentSettings.colourGreen = mainWin->ui->spinGreen->value();
    colourEnvironmentSettings.colourBlue = mainWin->ui->spinBlue->value();

    //Makestacksettings
    makeStackSettings.fileName = mainWin->ui->sFromImTxt->toPlainText();
}
