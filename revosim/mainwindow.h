/**
 * @file
 * Header: Main Window
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
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
#include "linkagesdialog.h"
#include <math.h>
#include <bitset>

#include <QActionGroup>
#include <QAction>
#include <QCheckBox>
#include <QMainWindow>
#include <QRadioButton>
#include <QShortcut>
#include <QSpinBox>
#include <QStandardPaths>
#include <QProgressBar>
#include <QQueue>
#include <QHash>
#include <QTextBrowser>
#include <QListWidget>
#include <QScrollArea>
#include <QTimer>

extern MainWindow *mainWindow;

namespace Ui {
class MainWindow;
}

/*!
 * \brief The MainWindow class
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    friend class test; //RJG make friend a test class so it can access private functions to test them

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
    float timePerFrame();

    QDockWidget *createSimulationSettingsDock();
    QDockWidget *createOutputSettingsDock();
    QDockWidget *createOrganismSettingsDock();
    QDockWidget *createInteractionSettingsDock();
    QDockWidget *createLogSettingsDock();
    QScrollArea *createWordsScrollArea();

    int refreshRate{};
    GenomeComparison *genoneComparison;

    void setOptionsFromParser(QHash<QString, QString> *options);

    //RJG - different main window statuses
    bool batchRunning;
    bool autoFromCommand;

    QString getSavePath();
    quint64 getBatchRuns();

    QString handleAnalysisTool(int code);
    //RJG - to do - moving this here so can access from logger, but ultimately will want better solution
    Analyser *analyser;

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    void closeEvent(QCloseEvent *e);
    void report();
    void runSetUp();
    void finishRun();
    void refreshPopulations();
    void resetInformationBar();
    void resetSquare(int n, int m);
    void resizeImageObjects();
    void writeLog();
    void simulationDead();
    void calculateSpecies();
    void restartTimer();
    void doSavePath();
    int scaleFails(int fails, float generations);
    int waitUntilPauseSignalIsEmitted();

    bool stopFlag{};
    bool pauseFlag;
    int nextRefresh;
    int lastReport;
    QHash<QString, QString> wordsInUse;

    //RJG - GUI stuff
    EnvironmentScene *environmentScene;
    PopulationScene *populationScene;
    QDockWidget *simulationSettingsDock;
    QDockWidget *organismSettingsDock;
    QDockWidget *outputSettingsDock;
    QDockWidget *interactionSettingsDock;
    QDockWidget *logSettingsDock;
    QListWidget *linkagesList;

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
    QAction *logButton{};
    QAction *testButton{};
    QAction *aboutButton{};

    //RJG - Save images checkboxes
    QCheckBox *guiCheckbox{};
    QCheckBox *savePopulationCount{};
    QCheckBox *saveMeanFitness{};
    QCheckBox *saveVisSysOneAsColour{};
    QCheckBox *saveSpecies{};
    QCheckBox *saveVisSysTwoAsColour{};
    QCheckBox *saveSettles{};
    QCheckBox *saveFailsSettles{};
    QCheckBox *saveEnvironment{};
    QCheckBox *interpolateCheckbox{};

    //RJG - other checkboxes
    QCheckBox *recalculateFitnessCheckbox{};
    QCheckBox *noSelectionFitnessCheckbox{};
    QCheckBox *toroidalCheckbox{};
    QCheckBox *nonspatialCheckbox{};
    QCheckBox *breedDifferenceCheckbox{};
    QCheckBox *breedSpeciesCheckbox{};
    QCheckBox *interactRestrictCheckbox{};
    QCheckBox *pathogensCheckbox{};
    QCheckBox *MultiBreedListCheckbox{};
    QCheckBox *variableMutationCheckbox{};
    QCheckBox *excludeWithoutDescendantsCheckbox{};
    QCheckBox *loggingCheckbox{};
    QCheckBox *autoWriteLogCheckbox{};
    QCheckBox *autoWriteLogIndividualsCheckbox{};
    QCheckBox *hgtCheckbox{};

    //RJG - radios and spins
    QRadioButton *phylogenyOffButton{};
    QRadioButton *basicPhylogenyButton{};
    QRadioButton *phylogenyButton{};
    QRadioButton *phylogenyAndMetricsButton{};
    QRadioButton *obligateSexualRadio{};
    QRadioButton *obligateAsexualRadio{};
    QRadioButton *facultativeSexualRadio{};
    QRadioButton *asexualRadio{};
    QRadioButton *variableBreedRadio{};
    QRadioButton *environmentModeBounceButton{};
    QRadioButton *environmentModeLoopButton{};
    QRadioButton *environmentModeOnceButton{};
    QRadioButton *environmentModeStaticButton{};
    QRadioButton *BlockInteractionsRadio{};
    QRadioButton *XORRadio{};
    QRadioButton *FitnessChangeRadio{};
    QRadioButton *EnergyChangeRadio{};
    QRadioButton *NoInteractionTypeRadio{};
    QRadioButton *pathogenDriftRadio{};
    QRadioButton *pathogenEvolveRadio{};
    QRadioButton *seedingDefaultRadio{};
    QRadioButton *seeding3TierRadio{};
    QRadioButton *seeding5TierRadio{};
    QRadioButton *hgtSynonoymousRadio{};
    QRadioButton *hgtNonSynonoymousRadio{};

    QSpinBox *minSpeciesSizeSpin{};
    QSpinBox *mutateSpin{};
    QSpinBox *refreshRateSpin{};
    QSpinBox *maxDifferenceSpin{};
    QSpinBox *breedThresholdSpin{};
    QSpinBox *targetSpin{};
    QSpinBox *environmentRateSpin{};
    QSpinBox *gridXSpin{};
    QSpinBox *settleToleranceSpin{};
    QSpinBox *genomeSizeSpin{};
    QSpinBox *slotsSpin{};
    QSpinBox *startAgeSpin{};
    QSpinBox *dispersalSpin{};
    QSpinBox *energySpin{};
    QSpinBox *breedCostSpin{};
    // Only in development branch (as is variable breed above)
    QSpinBox *pathogenMutateSpin{};
    QSpinBox *pathogenFrequencySpin{};
    QSpinBox *croppingFrequencySpin{};

    //RJG - text edits for logs
    QTextEdit *headerTextEdit;
    QTextEdit *iterationTextEdit;
    QTextEdit *logTextEdit;

    //RJG - line edits for systems
    QLineEdit *fitness_word_edit;
    QLineEdit *breed_word_edit;
    QLineEdit *mutate_word_edit;
    QLineEdit *variable_mutate_word_edit;
    QLineEdit *variable_breed_word_edit;
    QLineEdit *pathogen_word_edit;
    QLineEdit *species_ID_word_edit;
    QLineEdit *interactions_word_edit;
    QLineEdit *visualisations_word_edit;
    QLineEdit *visualisations_word_edit_2;


    // Interactions
    QSpinBox *interactionsSpin{};
    QSpinBox *predationDeltaSpin{};
    QSpinBox *predationEfficiencySpin{};

    //RJG - global save globalSavePath for all outputs
    QLineEdit *globalSavePath{};

    //RJG - options for batching
    quint64 batchRuns;
    quint64 batchIterations;
    quint64 batchTargetRuns;

    //Now some things settable by the ui
    QElapsedTimer timer;
    QQueue<int> timingqueue;  //records times for last few refreshes - used to calculate a moving-average iterations/second value

    QImage *populationImage;
    QImage *environmentImage;
    QImage *populationImageColour;
    QGraphicsPixmapItem *populationItem;
    QGraphicsPixmapItem *environmentItem;

    QTimer fitnessMessageTimer;

    bool intToBool(int i);
    bool boolStringToBool(QString s);

private slots:
    void guiCheckboxStateChanged(bool);
    void saveAllCheckboxStateChanged(bool);
    void writeRunData();
    void writeIndividualsData();
    void updateSettingsDockVisibility();
    void updateOutputDockVisibility();
    void redoImages(int oldRows, int oldColumns);
    void speciesModeChanged(int changeSpeciesMode, bool updateGUI);
    void environmentModeChanged(int changeEnvironmentMode, bool updateGUI);
    void loadSettings(QString fileName = "", bool calledFromCommandLine = false);
    void saveSettings(QString fileName = "");
    bool loadEnvironmentFiles(QString folder = "");
    void addLinkage();
    void linkagesGUI();
    void udpateGlobalPath();
    void writePeakCounts();
    void startSimulation();
    void runForNSimulation(int iterations = 0);
    void resetSimulation();
    void launchReseedDialog();
    void startBatchSimulation();
    void pauseSimulation();
    void stopSimulation();
    void updateGenomeComparisonDockVisibility(bool checked);
    void saveSimulation(QString filename = "");
    void loadSimulation(QString filename = "");
    void exitProgram();
    void doTests();
    void editInteractionsGrid();
    void linkagesListClicked();
    void toggleTestUI();

    // Note: on_actionXXX function are autoconnected by QT
    void on_actionFitness_logging_to_File_triggered();
    void on_actionLoad_Random_Numbers_triggered();
    void on_populationWindowComboBox_currentIndexChanged(int index);
    void on_actionCode_on_GitHub_triggered();
    void on_actionReportBugIssueFeatureRequest_triggered();
    void on_actionOnline_User_Manual_triggered();
    void on_actionSettings_Dock_triggered();
    void on_actionAbout_triggered();
    void on_actionGenomeComparison_triggered();
    void on_actionOutput_Dock_triggered();
};

#endif // MAINWINDOW_H
