#ifndef LOGSIMULATION_H
#define LOGSIMULATION_H

#include "simsettings.h"
#include "analyser.h"

#include <QString>
#include <QTextStream>
#include <QDir>

class LogSimulation
{
public:
    LogSimulation(SimSettings *settings);
    void CreateReplacementText();
    QString printSettings();
    QString printHeaderText();
    QString printDefaultHeaderText();
    QString printLogIterationText();
    QString printDefaultLogIterationText();
    QString printLogSpeciesText();
    QString printDefaultLogSpeciesText();
    QString writeHeaderFromLogText(QString text);

    void writeRunData(QString globalSavePath, int batchRuns);
    void writeLog(QString globalSavePath, int batchRuns, int logType);
    void setHeaderTextFromGUI(QString text);
    void setIterationTextFromGUI(QString text);
    void setSpeciestTextFromGUI(QString text);

    bool csvOutput;
    QHash <QString, QString> replacementText;

    void writeLogXML(QString globalSavePath);
    void loadLogXML(QString fileName);

    QString validateLog(QString text, int logType);

private:
    QString headerText;
    QString logIterationText;
    QString logSpeciesText;

    QString processLogTextSpecies(QString text);
    QString processLogTextGeneral(QString text);
    QString validateString(QString text);

    QString writeCustomLogHeader();
    QString writeFitnessLogHeader();
    QString writeRecombinationLogHeader();
    QString writeDisparityLogHeader(bool fullDetails = false);
    QString writeSpeciationLogHeader();
    QString writeMutationLogHeader();

    QString writeCustomLog();
    QString writeFitnessLog();
    QString writeRecombinationLog();
    QString writeDisparityLog(bool fullDetails = false);
    QString writeGridSpecies();
    QString writeSpeciationLog();
    QString writeMutationLog();
    QString GetFrequenciesLogString(Species species, bool original);

    SimSettings *simSettings;
};

#endif // LOGSIMULATION_H
