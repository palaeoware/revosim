/**
 * @file
 * Header: Main Window
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "about.h"
#include "environmentscene.h"
#include "genomecomparison.h"
#include "populationscene.h"
#include "simmanager.h"

#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QMainWindow>
#include <QProgressBar>
#include <QRadioButton>
#include <QShortcut>
#include <QSpinBox>
#include <QStandardPaths>

extern MainWindow *mainWindow;

namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Ui::MainWindow *ui;

    void resize();
    void refreshEnvironment();
    void setStatusBarText(const QString &text);
    void statusProgressBar(QProgressBar *prBar, bool add);
    void createMainToolbar();
    void createMainMenu();
    void updateGUIFromVariables();
    void processAppEvents();
    bool genomeComparisonAdd();
    QDockWidget *createSimulationSettingsDock();
    QDockWidget *createOutputSettingsDock();
    QDockWidget *createOrganismSettingsDock();

    int refreshRate{};
    GenomeComparison *genoneComparison;

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    Analyser *analyser;

    void closeEvent(QCloseEvent *e);
    void report();
    void runSetUp();
    void finishRun();
    void refreshPopulations();
    void resetInformationBar();
    void resetSquare(int n, int m);
    void resizeImageObjects();
    void writeLog();
    void calculateSpecies();
    int scaleFails(int fails, float generations);
    int waitUntilPauseSignalIsEmitted();
    QString handleAnalysisTool(int code);
    QString printSettings();

    bool stopFlag{};
    bool pauseFlag;
    int nextRefresh;

    //RJG - GUI stuff
    EnvironmentScene *environmentScene;
    PopulationScene *populationScene;
    QDockWidget *simulationSettingsDock;
    QDockWidget *organismSettingsDock;
    QDockWidget *outputSettingsDock;

    //RJG - GUI buttons and settings docker options which need to be accessible via slots; especially for load settings function
    QAction *startButton{};
    QAction *stopButton{};
    QAction *pauseButton{};
    QAction *runForButton{};
    QAction *resetButton{};
    QAction *reseedButton{};
    QAction *runForBatchButton{};
    QAction *genomeComparisonButton{};
    QAction *settingsButton{};
    QAction *aboutButton{};

    //RJG - Save images checkboxes
    QCheckBox *guiCheckbox{};
    QCheckBox *savePopulationCount{};
    QCheckBox *saveMeanFitness{};
    QCheckBox *saveCodingGenomeAsColour{};
    QCheckBox *saveSpecies{};
    QCheckBox *saveNonCodingGenomeAsColour{};
    QCheckBox *saveGeneFrequencies{};
    QCheckBox *saveSettles{};
    QCheckBox *saveFailsSettles{};
    QCheckBox *saveEnvironment{};
    QCheckBox *interpolateCheckbox{};

    //RJG - other checkboxes
    QCheckBox *recalculateFitnessCheckbox{};
    QCheckBox *toroidalCheckbox{};
    QCheckBox *nonspatialCheckbox{};
    QCheckBox *breedDifferenceerenceCheckbox{};
    QCheckBox *breedSpeciesCheckbox{};
    QCheckBox *excludeWithoutDescendantsCheckbox{};
    QCheckBox *loggingCheckbox{};
    QCheckBox *autowriteLogCheckbox{};

    //RJG - radios and spins
    QRadioButton *phylogenyOffButton{};
    QRadioButton *basicPhylogenyButton{};
    QRadioButton *phylogenyButton{};
    QRadioButton *phylogenyAndMetricsButton{};
    QRadioButton *sexualRadio{};
    QRadioButton *asexualRadio{};
    QRadioButton *environmentModeBounceButton{};
    QRadioButton *environmentModeLoopButton{};
    QRadioButton *environmentModeOnceButton{};
    QRadioButton *environmentModeStaticButton{};

    QSpinBox *mutateSpin{};
    QSpinBox *refreshRateSpin{};
    QSpinBox *maxDifferenceerenceSpin{};
    QSpinBox *breedThresholdSpin{};
    QSpinBox *targetSpin{};
    QSpinBox *environmentRateSpin{};
    QSpinBox *gridXSpin{};
    QSpinBox *gridYSpin{};
    QSpinBox *settleToleranceSpin{};
    QSpinBox *slotsSpin{};
    QSpinBox *startAgeSpin{};
    QSpinBox *dispersalSpin{};
    QSpinBox *energySpin{};
    QSpinBox *breedCostSpin{};

    //RJG - global save globalSavePath for all outputs
    QLineEdit *globalSavePath{};

    //RJG - options for batching
    bool batchRunning;
    quint64 batchRuns;
    quint64 batchIterations;
    quint64 batchTargetRuns;

    //Now some things settable by the ui
    QTime timer;
    QImage *populationImage;
    QImage *environmentImage;
    QImage *populationImageColour;
    QGraphicsPixmapItem *populationItem;
    QGraphicsPixmapItem *environmentItem;

private slots:
    void guiCheckboxStateChanged(bool);
    void saveAllCheckboxStateChanged(bool);
    void writeRunData();
    void updateSettingsDockVisability();
    void updateGenomeComparisonDockVisability(bool checked);
    void redoImages(int oldRows, int oldColumns);
    void speciesModeChanged(int changeSpeciesMode, bool updateGUI);
    void environmentModeChanged(int changeEnvironmentMode, bool updateGUI);
    void loadSettings();
    void saveSettings();
    void updateGlobalPath();
    bool loadEnvironmentFiles();
    void writePeakCounts();
    void startSimulation();
    void runForNSimulation();
    void resetSimulation();
    void launchReseedDialog();
    void startBatchSimulation();
    void pauseSimulation();
    void stopSimulation();
    void saveSimulation();
    void loadSimulation();
    void exitProgram();

    // Note: on_actionXXX function are autoconnected by QT
    void on_actionLoad_Random_Numbers_triggered(); // auto
    void on_populationWindowComboBox_currentIndexChanged(int index); // auto
    void on_actionCode_on_GitHub_triggered(); // auto
    void on_actionAbout_triggered(); //auto
    void on_actionOnline_User_Manual_triggered(); // auto
    void on_actionSettings_Dock_triggered(); // auto
};

#endif // MAINWINDOW_H
