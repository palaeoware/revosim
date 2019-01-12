/*!
 * @file
 * Main Window
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

#include <cmath>

#include "analyser.h"
#include "analysistools.h"
#include "mainwindow.h"
#include "reseed.h"
#include "resizecatcher.h"
#include "ui_mainwindow.h"
#include "globals.h"

#include <QActionGroup>
#include <QComboBox>
#include <QDataStream>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QDebug>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QGraphicsPixmapItem>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <QThread>
#include <QXmlStreamReader>
#include <QButtonGroup>

//not defined in all versions
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.7071067811865475
#endif

SimManager *simulationManager;
MainWindow *mainWindow;

/*!
 * \brief The Sleeper class
 */
class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs)
    {
        QThread::usleep(usecs);
    }
    static void msleep(unsigned long msecs)
    {
        QThread::msleep(msecs);
    }
    static void sleep(unsigned long secs)
    {
        QThread::sleep(secs);
    }
};

/*!
 * \brief MainWindow::MainWindow
 * \param parent
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    analyser = new Analyser; // so can delete next time!
    ui->setupUi(this);
    mainWindow = this;

    // Output version, but also date compiled for clarity
    setWindowTitle(QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) + " - compiled - " + __DATE__);
    setWindowIcon(QIcon (":/icon.png"));

    //Install filter to catch resize events to central widget and deliver to mainwindow (handle dock resizes)
    auto *resizeCatcher = new ResizeCatcher(this);
    ui->centralWidget->installEventFilter(resizeCatcher);

    // Create Main Menu
    createMainMenu();

    // Create Main Toolbar
    createMainToolbar();

    //RJG - set up settings docker.
    simulationSettingsDock = createSimulationSettingsDock();
    //----RJG - second settings docker.
    organismSettingsDock = createOrganismSettingsDock();
    //RJG - Third settings docker
    outputSettingsDock = createOutputSettingsDock();

    //RJG - Make docks tabbed
    tabifyDockWidget(organismSettingsDock, simulationSettingsDock);
    tabifyDockWidget(simulationSettingsDock, outputSettingsDock);

    //Hide docks by default
    organismSettingsDock->hide();
    simulationSettingsDock->hide();
    outputSettingsDock->hide();

    //Add Genome Comparison UI Dock
    ui->genomeComparisonDock->hide();
    genoneComparison = new GenomeComparison;
    auto *genomeLayout = new QVBoxLayout;
    genomeLayout->addWidget(genoneComparison);
    ui->genomeComparisonContent->setLayout(genomeLayout);

    //create scenes, add to the GVs
    environmentScene = new EnvironmentScene;
    ui->GV_Environment->setScene(environmentScene);
    environmentScene->mainWindow = this;

    populationScene = new PopulationScene;
    populationScene->mainWindow = this;
    ui->GV_Population->setScene(populationScene);

    //add images to the scenes
    environmentItem = new QGraphicsPixmapItem();
    environmentScene->addItem(environmentItem);
    environmentItem->setZValue(0);

    populationItem = new QGraphicsPixmapItem();
    populationScene->addItem(populationItem);

    populationImage = new QImage(gridX, gridY, QImage::Format_Indexed8);
    QVector <QRgb> clut(256);
    for (int ic = 0; ic < 256; ic++) clut[ic] = qRgb(ic, ic, ic);
    populationImage->setColorTable(clut);
    populationImage->fill(0);

    environmentImage = new QImage(gridX, gridY, QImage::Format_RGB32);
    environmentImage->fill(0);

    populationImageColour = new QImage(gridX, gridY, QImage::Format_RGB32);
    environmentImage->fill(0);

    environmentItem->setPixmap(QPixmap::fromImage(*environmentImage));
    populationItem->setPixmap(QPixmap::fromImage(*populationImage));

    //ARTS - Population Window dropdown must be after settings dock setup
    // 0 = Population count
    // 1 = Mean fitness
    // 2 = Coding genome as colour
    // 3 = NonCoding genome as colour
    // 4 = Gene Frequencies
    // 5 = Breed Attempts
    // 6 = Breed Fails
    // 7 = Settles
    // 8 = Settle Fails (R=Breed, G=Settle)
    // 9 = Breed Fails 2
    // 10 = Species
    ui->populationWindowComboBox->addItem("Population Count", QVariant(0));
    ui->populationWindowComboBox->addItem("Mean Fitness", QVariant(1));
    ui->populationWindowComboBox->addItem("Coding Genome as Colour", QVariant(2));
    ui->populationWindowComboBox->addItem("Non-Coding Genome as Colour", QVariant(3));
    ui->populationWindowComboBox->addItem("Gene Frequencies", QVariant(4));
    //ui->populationWindowComboBox->addItem("Breed Attempts",QVariant(5));
    //ui->populationWindowComboBox->addItem("Breed Fails",QVariant(6));
    ui->populationWindowComboBox->addItem("Settles", QVariant(7));
    ui->populationWindowComboBox->addItem("Breed/Settle Fails (R=Breed, G=Settle)", QVariant(8));
    //ui->populationWindowComboBox->addItem("Breed Fails 2",QVariant(9));
    ui->populationWindowComboBox->addItem("Species", QVariant(10));

    //ARTS -Population Window dropdown set current index. Note this value is the index not the data value.
    ui->populationWindowComboBox->setCurrentIndex(2);

    simulationManager = new SimManager;

    pauseFlag = false;

    //RJG - load default environment image to allow program to run out of box (quicker for testing)
    environmentFiles.append(":/REvoSim_default_env.png");
    currentEnvironmentFile = 0;
    simulationManager->loadEnvironmentFromFile(1);

    finishRun();//sets up enabling
    simulationManager->setupRun();
    nextRefresh = 0;
    report();

    //RJG - Set batch variables
    batchRunning = false;
    batchRuns = 0;
    batchIterations = 0;
    batchTargetRuns = 0;

    showMaximized();

    //RJG - seed pseudorandom numbers
    qsrand(static_cast<uint>(QTime::currentTime().msec()));

    //RJG - Now load randoms into program - portable rand is just plain pseudorandom number - initially used in makelookups (called from simmanager contructor) to write to randoms array
    int seedoffset = simulationManager->portableRandom();
    QFile rfile(":/randoms.dat");
    if (!rfile.exists())
        QMessageBox::warning(this, "Oops", "Error loading randoms. Please do so manually.");
    rfile.open(QIODevice::ReadOnly);

    rfile.seek(seedoffset);

    //RJG - overwrite pseudorandoms with genuine randoms
    qint64 i = rfile.read(reinterpret_cast<char *>(randoms), 65536);
    if (i != static_cast<qint64>(65536))
        QMessageBox::warning(this, "Oops", "Failed to read 65536 bytes from file - random numbers may be compromised - try again or restart program");
}

/*!
 * \brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete simulationManager;
}

/**
 * @brief MainWIndow::createMainMenu
 */
void MainWindow::createMainMenu()
{
    QObject::connect(ui->actionStart_Sim, SIGNAL(triggered()), this, SLOT(startSimulation()));
    QObject::connect(ui->actionRun_for, SIGNAL(triggered()), this, SLOT(runForNSimulation()));
    QObject::connect(ui->actionBatch, SIGNAL(triggered()), this, SLOT(startBatchSimulation()));
    QObject::connect(ui->actionPause_Sim, SIGNAL(triggered()), this, SLOT(pauseSimulation()));
    QObject::connect(ui->actionStop_Sim, SIGNAL(triggered()), this, SLOT(stopSimulation()));
    QObject::connect(ui->actionReset, SIGNAL (triggered()), this, SLOT (resetSimulation()));
    QObject::connect(ui->actionReseed, SIGNAL (triggered()), this, SLOT (launchReseedDialog()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveSimulation()));
    QObject::connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(loadSimulation()));
    QObject::connect(ui->actionSaveSettings, SIGNAL (triggered()), this, SLOT (saveSettings()));
    QObject::connect(ui->actionLoadSettings, SIGNAL (triggered()), this, SLOT (loadSettings()));
    QObject::connect(ui->actionCount_peaks, SIGNAL(triggered()), this, SLOT(writePeakCounts()));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exitProgram()));
}

/*!
 * \brief MainWindow::createMainToolbar
 */
void MainWindow::createMainToolbar()
{
    //ARTS - Toolbar buttons & docker toggles
    startButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_button_green.png")), QString("Run"), this);
    runForButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_n_button_green.png")), QString("Run for..."), this);
    runForBatchButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_batch_button_green.png")), QString("Batch..."), this);
    pauseButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_pause_button_orange.png")), QString("Pause"), this);
    stopButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_stop_button_red.png")), QString("Stop"), this);
    resetButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_reset_button_red.png")), QString("Reset"), this);
    reseedButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_reset_known_genome_button_red.png")), QString("Reseed"), this);
    genomeComparisonButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_genome_button.png")), QString("Genome"), this);
    settingsButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_settings_1_button.png")), QString("Settings"), this);
    aboutButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_about_button.png")), QString("About"), this);

    //ARTS - Toolbar default settings & docker toggles defaults
    startButton->setEnabled(false);
    runForButton->setEnabled(false);
    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);
    reseedButton->setEnabled(false);
    runForBatchButton->setEnabled(false);
    genomeComparisonButton->setCheckable(true);
    settingsButton->setCheckable(true);

    //Tootip Help
    startButton->setToolTip(tr("<font>Use this button to start a basic manual simulation run.</font>"));
    runForButton->setToolTip(tr("<font>Use this button to start a simulation that will stop after '<i>n</i>' iterations.</font>"));
    runForBatchButton->setToolTip(tr("<font>Use this button to start a batched simulation that will stop after '<i>x</i>' runs of '<i>n</i>' iterations.</font>"));
    pauseButton->setToolTip(tr("<font>Use this button to pause any simulation run or batched run. Press again to unpause.</font>"));
    stopButton->setToolTip(tr("<font>Use this button stop any simulation or batched run.</font>"));
    resetButton->setToolTip(tr("<font>Use this button reset the simulation.</font>"));
    reseedButton->setToolTip(tr("<font>Use this button reseed the simulation with a custom starting genome.</font>"));
    genomeComparisonButton->setToolTip(tr("<font>Use this button to open the Genome Comparission Dock.</font>"));
    settingsButton->setToolTip(tr("<font>Use this button to open the Settings Dock.</font>"));
    aboutButton->setToolTip(tr("<font>Use this button to view information about this program.</font>"));

    //ARTS - Toolbar layout
    ui->toolBar->setIconSize(QSize(25, 25));
    ui->toolBar->addAction(startButton);
    ui->toolBar->addAction(runForButton);
    ui->toolBar->addAction(runForBatchButton);
    ui->toolBar->addAction(pauseButton);
    ui->toolBar->addAction(stopButton);
    ui->toolBar->addAction(resetButton);
    ui->toolBar->addAction(reseedButton);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(genomeComparisonButton);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(settingsButton);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(aboutButton);

    //RJG - Connect button signals to slots.
    //Note for clarity:
    //Reset = start again with random individual.
    //Reseed = start again with user defined genome
    QObject::connect(startButton, SIGNAL(triggered()), this, SLOT(startSimulation()));
    QObject::connect(runForButton, SIGNAL(triggered()), this, SLOT(runForNSimulation()));
    QObject::connect(pauseButton, SIGNAL(triggered()), this, SLOT(pauseSimulation()));
    QObject::connect(stopButton, SIGNAL(triggered()), this, SLOT(stopSimulation()));
    QObject::connect(resetButton, SIGNAL(triggered()), this, SLOT(resetSimulation()));
    QObject::connect(reseedButton, SIGNAL(triggered()), this, SLOT(launchReseedDialog()));
    QObject::connect(runForBatchButton, SIGNAL(triggered()), this, SLOT(startBatchSimulation()));
    QObject::connect(genomeComparisonButton, SIGNAL(triggered(bool)), this, SLOT(updateGenomeComparisonDockVisability(bool)));
    QObject::connect(settingsButton, SIGNAL(triggered()), this, SLOT(updateSettingsDockVisability()));
    QObject::connect(aboutButton, SIGNAL (triggered()), this, SLOT (on_actionAbout_triggered()));
}

/*!
 * \brief MainWindow::createSimulationSettingsDock
 * \return QDockWidget simulationSettingsDock
 */
QDockWidget *MainWindow::createSimulationSettingsDock()
{
    simulationSettingsDock = new QDockWidget("Simulation", this);
    simulationSettingsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    simulationSettingsDock->setFeatures(QDockWidget::DockWidgetMovable);
    simulationSettingsDock->setFeatures(QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, simulationSettingsDock);

    auto *settingsGrid = new QGridLayout;
    settingsGrid->setAlignment(Qt::AlignTop);

    // Environment Settings
    auto *environmentSettingsGrid = new QGridLayout;

    QLabel *environmentLabel = new QLabel("Environmental Settings");
    environmentLabel->setStyleSheet("font-weight: bold");
    environmentSettingsGrid->addWidget(environmentLabel, 0, 1, 1, 2);

    QPushButton *changeEnvironmentFilesButton = new QPushButton("&Change Environment Files");
    changeEnvironmentFilesButton->setObjectName("changeEnvironmentFilesButton");
    changeEnvironmentFilesButton->setToolTip("<font>REvoSim allows you to customise the environment by loading one or more image files.</font>");
    environmentSettingsGrid->addWidget(changeEnvironmentFilesButton, 1, 1, 1, 2);
    connect(changeEnvironmentFilesButton, SIGNAL (clicked()), this, SLOT(loadEnvironmentFiles()));

    QLabel *environmentRateLabel = new QLabel("Environment refresh rate:");
    environmentRateLabel->setToolTip("<font>This is the rate of change for the selected environmental images.</font>");
    environmentRateSpin = new QSpinBox;
    environmentRateSpin->setToolTip("<font>This is the rate of change for the selected environmental images.</font>");
    environmentRateSpin->setMinimum(0);
    environmentRateSpin->setMaximum(100000);
    environmentRateSpin->setValue(environmentChangeRate);
    environmentSettingsGrid->addWidget(environmentRateLabel, 2, 1);
    environmentSettingsGrid->addWidget(environmentRateSpin, 2, 2);
    //RJG - Note in order to use a lamda not only do you need to use C++11, but there are two valueChanged signals for spinbox - and int and a string. Need to cast it to an int
    connect(environmentRateSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i )
    {
        environmentChangeRate = i;
    });

    QLabel *environment_modeLabel = new QLabel("Environment mode:");
    environmentSettingsGrid->addWidget(environment_modeLabel, 3, 1, 1, 2);

    auto *environmentModeGrid = new QGridLayout;
    environmentModeStaticButton = new QRadioButton("Static");
    environmentModeStaticButton->setToolTip("<font>'Static' uses a single environment image.</font>");
    environmentModeOnceButton = new QRadioButton("Once");
    environmentModeOnceButton->setToolTip("<font>'Once' uses each image only once, simulation stops after last image.</font>");
    environmentModeLoopButton = new QRadioButton("Loop");
    environmentModeLoopButton->setToolTip("<font>'Loop' uses each image in order in a loop</font>");
    environmentModeBounceButton = new QRadioButton("Bounce");
    environmentModeBounceButton->setToolTip("<font>'Bounce' rebounds between the first and last image in a loop.</font>");
    auto *environmentModeButtonGroup = new QButtonGroup;
    environmentModeButtonGroup->addButton(environmentModeStaticButton, ENV_MODE_STATIC);
    environmentModeButtonGroup->addButton(environmentModeOnceButton, ENV_MODE_ONCE);
    environmentModeButtonGroup->addButton(environmentModeLoopButton, ENV_MODE_LOOP);
    environmentModeButtonGroup->addButton(environmentModeBounceButton, ENV_MODE_BOUNCE);
    environmentModeLoopButton->setChecked(true);
    environmentModeGrid->addWidget(environmentModeStaticButton, 1, 1, 1, 2);
    environmentModeGrid->addWidget(environmentModeOnceButton, 1, 2, 1, 2);
    environmentModeGrid->addWidget(environmentModeLoopButton, 2, 1, 1, 2);
    environmentModeGrid->addWidget(environmentModeBounceButton, 2, 2, 1, 2);
    connect(environmentModeButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [ = ](const int &i)
    {
        environmentModeChanged(i, false);
    });
    environmentSettingsGrid->addLayout(environmentModeGrid, 4, 1, 1, 2);

    interpolateCheckbox = new QCheckBox("Interpolate between images");
    interpolateCheckbox->setChecked(environmentInterpolate);
    interpolateCheckbox->setToolTip("<font>Turning this ON will interpolate the environment between individual images.</font>");
    environmentSettingsGrid->addWidget(interpolateCheckbox, 5, 1, 1, 2);
    connect(interpolateCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        environmentInterpolate = i;
    });

    toroidalCheckbox = new QCheckBox("Toroidal environment");
    toroidalCheckbox->setChecked(toroidal);
    toroidalCheckbox->setToolTip("<font>Turning this ON will allow dispersal of progeny in an unbounded warparound environment. Progeny leaving one side of the population window will immediately reappear on the opposite side.</font>");
    environmentSettingsGrid->addWidget(toroidalCheckbox, 6, 1, 1, 2);
    connect(toroidalCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        toroidal = i;
    });

    // Simulation Size Settings
    auto *simulationSizeSettingsGrid = new QGridLayout;

    QLabel *simulation_sizeLabel = new QLabel("Simulation size");
    simulation_sizeLabel->setStyleSheet("font-weight: bold");
    simulationSizeSettingsGrid->addWidget(simulation_sizeLabel, 0, 1, 1, 2);

    QLabel *gridXLabel = new QLabel("Grid X:");
    gridXLabel->setToolTip("<font>Number of grid cells on the <i>x</i> axis.</font>");
    gridXSpin = new QSpinBox;
    gridXSpin->setMinimum(1);
    gridXSpin->setMaximum(256);
    gridXSpin->setValue(gridX);
    gridXSpin->setToolTip("<font>Number of grid cells on the <i>x</i> axis.</font>");
    simulationSizeSettingsGrid->addWidget(gridXLabel, 2, 1);
    simulationSizeSettingsGrid->addWidget(gridXSpin, 2, 2);
    connect(gridXSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        int oldRows = gridX;
        gridX = i;
        redoImages(oldRows, gridY);
    });

    QLabel *gridYLabel = new QLabel("Grid Y:");
    gridYLabel->setToolTip("<font>Number of grid cells on the <i>y</i> axis.</font>");
    gridYSpin = new QSpinBox;
    gridYSpin->setMinimum(1);
    gridYSpin->setMaximum(256);
    gridYSpin->setValue(gridY);
    gridYSpin->setToolTip("<font>Number of grid cells on the <i>y</i> axis.</font>");
    simulationSizeSettingsGrid->addWidget(gridYLabel, 3, 1);
    simulationSizeSettingsGrid->addWidget(gridYSpin, 3, 2);
    connect(gridYSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        int oldColumns = gridY;
        gridY = i;
        redoImages(gridX, oldColumns);
    });

    QLabel *slotsLabel = new QLabel("Slots:");
    slotsLabel->setToolTip("<font>Number of slots per grid cell.</font>");
    slotsSpin = new QSpinBox;
    slotsSpin->setMinimum(1);
    slotsSpin->setMaximum(256);
    slotsSpin->setValue(slotsPerSquare);
    slotsSpin->setToolTip("<font>Number of slots per grid cell.</font>");
    simulationSizeSettingsGrid->addWidget(slotsLabel, 4, 1);
    simulationSizeSettingsGrid->addWidget(slotsSpin, 4, 2);
    connect(slotsSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        slotsPerSquare = i;
        redoImages(gridX, gridY);
    });

    // Simulation Settings
    auto *simulationSettingsGrid = new QGridLayout;

    QLabel *simulationSettingsLabel = new QLabel("Simulation settings");
    simulationSettingsLabel->setStyleSheet("font-weight: bold");
    simulationSettingsGrid->addWidget(simulationSettingsLabel, 0, 1, 1, 2);

    QLabel *targetLabel = new QLabel("Fitness target:");
    targetLabel->setToolTip("<font>Target value effects the fitness landscape. See manual for more details.</font>");
    targetSpin = new QSpinBox;
    targetSpin->setMinimum(1);
    targetSpin->setMaximum(96);
    targetSpin->setValue(target);
    targetSpin->setToolTip("<font>Target value effects the fitness landscape. See manual for more details.</font>");
    simulationSettingsGrid->addWidget(targetLabel, 1, 1);
    simulationSettingsGrid->addWidget(targetSpin, 1, 2);
    connect(targetSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        target = i;
    });

    QLabel *energyLabel = new QLabel("Energy input:");
    energyLabel->setToolTip("<font>Energy level given to new organisms.</font>");
    energySpin = new QSpinBox;
    energySpin->setMinimum(1);
    energySpin->setMaximum(20000);
    energySpin->setValue(food);
    energySpin->setToolTip("<font>Energy level given to new organisms.</font>");
    simulationSettingsGrid->addWidget(energyLabel, 2, 1);
    simulationSettingsGrid->addWidget(energySpin, 2, 2);
    connect(energySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        food = i;
    });

    QLabel *settleToleranceLabel = new QLabel("Settle tolerance:");
    settleToleranceLabel->setToolTip("<font>Defines the range of environments an organism can settle into.</font>");
    settleToleranceSpin = new QSpinBox;
    settleToleranceSpin->setMinimum(1);
    settleToleranceSpin->setMaximum(30);
    settleToleranceSpin->setValue(settleTolerance);
    settleToleranceSpin->setToolTip("<font>Defines the range of environments an organism can settle into.</font>");
    simulationSettingsGrid->addWidget(settleToleranceLabel, 3, 1);
    simulationSettingsGrid->addWidget(settleToleranceSpin, 3, 2);
    connect(settleToleranceSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        settleTolerance = i;
    });

    recalculateFitnessCheckbox = new QCheckBox("Recalculate fitness");
    recalculateFitnessCheckbox->setChecked(recalculateFitness);
    recalculateFitnessCheckbox->setToolTip("<font>Turning on recalculates the fitness of each organism every iteration.</font>");
    simulationSettingsGrid->addWidget(recalculateFitnessCheckbox, 4, 1, 1, 2);
    connect(recalculateFitnessCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        recalculateFitness = i;
    });

    //Phylogeny Settings
    auto *phylogenySettingsGrid = new QGridLayout;

    QLabel *phylogenySettingsLabel = new QLabel("Phylogeny settings");
    phylogenySettingsLabel->setStyleSheet("font-weight: bold");
    phylogenySettingsGrid->addWidget(phylogenySettingsLabel, 0, 1, 1, 1);

    auto *phylogenyGrid = new QGridLayout;
    phylogenyOffButton = new QRadioButton("Off");
    phylogenyOffButton->setToolTip("<font>Turns phylogeny logging off.</font>");
    basicPhylogenyButton = new QRadioButton("Basic");
    basicPhylogenyButton->setToolTip("<font>Turns on basic phylogeny logging.</font>");
    phylogenyButton = new QRadioButton("Phylogeny");
    phylogenyButton->setToolTip("<font>Turns on phylogeny logging.</font>");
    phylogenyAndMetricsButton = new QRadioButton("Phylogeny and metrics");
    phylogenyAndMetricsButton->setToolTip("<font>Turns on phylogeny and metrics logging.</font>");
    auto *phylogenyButtonGroup = new QButtonGroup;
    phylogenyButtonGroup->addButton(phylogenyOffButton, SPECIES_MODE_NONE);
    phylogenyButtonGroup->addButton(basicPhylogenyButton, SPECIES_MODE_BASIC);
    phylogenyButtonGroup->addButton(phylogenyButton, SPECIES_MODE_PHYLOGENY);
    phylogenyButtonGroup->addButton(phylogenyAndMetricsButton, SPECIES_MODE_PHYLOGENY_AND_METRICS);
    basicPhylogenyButton->setChecked(true);
    phylogenyGrid->addWidget(phylogenyOffButton, 1, 1, 1, 2);
    phylogenyGrid->addWidget(basicPhylogenyButton, 1, 2, 1, 2);
    phylogenyGrid->addWidget(phylogenyButton, 2, 1, 1, 2);
    phylogenyGrid->addWidget(phylogenyAndMetricsButton, 2, 2, 1, 2);
    connect(phylogenyButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [ = ](const int &i)
    {
        speciesModeChanged(i, false);
    });
    phylogenySettingsGrid->addLayout(phylogenyGrid, 1, 1, 1, 2);

    //ARTS - Dock Grid Layout
    settingsGrid->addLayout(environmentSettingsGrid, 0, 1);
    settingsGrid->addLayout(simulationSizeSettingsGrid, 1, 1);
    settingsGrid->addLayout(simulationSettingsGrid, 2, 1);
    settingsGrid->addLayout(phylogenySettingsGrid, 3, 1);

    QWidget *settingsLayoutWidget = new QWidget;
    settingsLayoutWidget->setLayout(settingsGrid);
    settingsLayoutWidget->setMinimumWidth(300);
    simulationSettingsDock->setWidget(settingsLayoutWidget);
    simulationSettingsDock->adjustSize();

    return simulationSettingsDock;
}

/*!
 * \brief MainWindow::createOutputSettingsDock
 * \return QDockWidget outputSettingsDock
 */
QDockWidget *MainWindow::createOutputSettingsDock()
{
    outputSettingsDock = new QDockWidget("Output", this);
    outputSettingsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    outputSettingsDock->setFeatures(QDockWidget::DockWidgetMovable);
    outputSettingsDock->setFeatures(QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, outputSettingsDock);

    auto *outputSettingsGrid = new QGridLayout;
    outputSettingsGrid->setAlignment(Qt::AlignTop);

    //ARTS - Output Save Path
    auto *savePathGrid = new QGridLayout;
    QLabel *savePathLabel = new QLabel("Output save path:");
    savePathLabel->setObjectName("savePathLabel");
    savePathGrid->addWidget(savePathLabel, 1, 1, 1, 2);
    QString program_path(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    program_path.append("/");
    globalSavePath = new QLineEdit(program_path);
    globalSavePath->setToolTip("<font>System globalSavePath where all logs, images, and output data is saved.</font>");
    savePathGrid->addWidget(globalSavePath, 2, 1, 1, 2);
    QPushButton *changePathButton = new QPushButton("&Change");
    changePathButton->setToolTip("<font>System globalSavePath where all logs, images, and output data is saved.</font>");
    changePathButton->setObjectName("changePathButton");
    savePathGrid->addWidget(changePathButton, 3, 1, 1, 2);
    connect(changePathButton, SIGNAL (clicked()), this, SLOT(updateGlobalPath()));

    //ARTS - Refresh/Polling Rate
    auto *pollingRateGrid = new QGridLayout;
    QLabel *pollingRateLabel = new QLabel("Refresh/Polling Rate");
    pollingRateLabel->setObjectName("pollingRateLabel");
    pollingRateGrid->addWidget(pollingRateLabel, 1, 1, 1, 2);

    refreshRate = 50;
    QLabel *refreshRateLabel = new QLabel("Refresh/polling rate:");
    refreshRateLabel->setObjectName("refreshRateLabel");
    refreshRateLabel->setToolTip("<font>Number of iteration between each logging and screen data refresh event.</font>");
    refreshRateSpin = new QSpinBox;
    refreshRateSpin->setMinimum(1);
    refreshRateSpin->setMaximum(10000);
    refreshRateSpin->setValue(refreshRate);
    refreshRateSpin->setToolTip("<font>Number of iteration between each logging and screen data refresh event.</font>");
    pollingRateGrid->addWidget(refreshRateLabel, 2, 1);
    pollingRateGrid->addWidget(refreshRateSpin, 2, 2);
    connect(refreshRateSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        refreshRate = i;
    });

    //ARTS - Logging: Population & Environment
    auto *images_grid = new QGridLayout;

    QLabel *imagesLabel = new QLabel("Logging: Population/Enivronment");
    imagesLabel->setObjectName("imagesLabel");
    images_grid->addWidget(imagesLabel, 1, 1, 1, 2);

    savePopulationCount = new QCheckBox("Population count");
    savePopulationCount->setToolTip("<font>Turn on/off to log the 'Population Count' as an image.</font>");
    images_grid->addWidget(savePopulationCount, 2, 1, 1, 1);
    saveMeanFitness = new QCheckBox("Mean fitness");
    saveMeanFitness->setToolTip("<font>Turn on/off to log the 'Mean Fitness' as an image.</font>");
    images_grid->addWidget(saveMeanFitness, 2, 2, 1, 1);
    saveCodingGenomeAsColour = new QCheckBox("Coding genome");
    saveCodingGenomeAsColour->setToolTip("<font>Turn on/off to log the 'Coding Genome' as an image.</font>");
    images_grid->addWidget(saveCodingGenomeAsColour, 3, 1, 1, 1);
    saveNonCodingGenomeAsColour = new QCheckBox("Noncoding genome");
    saveNonCodingGenomeAsColour->setToolTip("<font>Turn on/off to log the 'Noncoding Genome' as an image.</font>");
    images_grid->addWidget(saveNonCodingGenomeAsColour, 3, 2, 1, 1);
    saveSpecies = new QCheckBox("Species");
    saveSpecies->setToolTip("<font>Turn on/off to log the 'Species' as an image.</font>");
    images_grid->addWidget(saveSpecies, 4, 1, 1, 1);
    saveGeneFrequencies = new QCheckBox("Gene frequencies");
    saveGeneFrequencies->setToolTip("<font>Turn on/off to log the 'Gene Frequencies' as an image.</font>");
    images_grid->addWidget(saveGeneFrequencies, 4, 2, 1, 1);
    saveSettles = new QCheckBox("Settles");
    saveSettles->setToolTip("<font>Turn on/off to log the 'Settles' as an image.</font>");
    images_grid->addWidget(saveSettles, 5, 1, 1, 1);
    saveFailsSettles = new QCheckBox("Breed/Settle Fails");
    saveFailsSettles->setToolTip("<font>Turn on/off to log the 'Breed/Settle Fails' as an image.</font>");
    images_grid->addWidget(saveFailsSettles, 5, 2, 1, 1);
    saveEnvironment = new QCheckBox("Environment");
    saveEnvironment->setToolTip("<font>Turn on/off to log the 'Environment' as an image.</font>");
    images_grid->addWidget(saveEnvironment, 6, 1, 1, 1);

    QCheckBox *saveAllImagesCheckbox = new QCheckBox("All");
    saveAllImagesCheckbox->setObjectName("saveAllImagesCheckbox");
    saveAllImagesCheckbox->setToolTip("<font>Toggle all/none of the above checkboxes.</font>");
    images_grid->addWidget(saveAllImagesCheckbox, 6, 2, 1, 1);
    QObject::connect(saveAllImagesCheckbox, SIGNAL (toggled(bool)), this, SLOT(saveAllCheckboxStateChanged(bool)));

    //ARTS - Logging to text file
    auto *fileLoggingGrid = new QGridLayout;

    QLabel *outputSettingsLabel = new QLabel("Logging: To Text File(s)");
    outputSettingsLabel->setObjectName("outputSettingsLabel");
    fileLoggingGrid->addWidget(outputSettingsLabel, 1, 1, 1, 2);

    loggingCheckbox = new QCheckBox("Write Log Files");
    loggingCheckbox->setChecked(logging);
    loggingCheckbox->setToolTip("<font>Turn on/off this option to write to a text log file every refresh/poll.</font>");
    fileLoggingGrid->addWidget(loggingCheckbox, 2, 1, 1, 2);
    connect(loggingCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        logging = i;
    });

    autowriteLogCheckbox = new QCheckBox("Automatically create detailed log on batch runs");
    autowriteLogCheckbox->setChecked(true);
    autowriteLogCheckbox->setToolTip("<font>Turn on/off this option to automatically write detailed log after batch runs</font>");
    fileLoggingGrid->addWidget(autowriteLogCheckbox, 3, 1, 1, 2);

    QPushButton *write_nwk = new QPushButton("Write data (including tree) for current run");
    write_nwk->setToolTip("<font>Use this option to manually trigger the writing of a detailed log after a run.</font>");
    fileLoggingGrid->addWidget(write_nwk, 4, 1, 1, 2);
    connect(write_nwk, SIGNAL (clicked()), this, SLOT(writeRunData()));

    excludeWithoutDescendantsCheckbox = new QCheckBox("Exclude species without descendents");
    excludeWithoutDescendantsCheckbox->setChecked(allowExcludeWithDescendants);
    excludeWithoutDescendantsCheckbox->setToolTip("<font>Use this option to exclude species without descendents.</font>");
    fileLoggingGrid->addWidget(excludeWithoutDescendantsCheckbox, 5, 1, 1, 1);
    connect(excludeWithoutDescendantsCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        allowExcludeWithDescendants = i;
    });

    QLabel *minSpeciesSizeLabel = new QLabel("Minimum species size:");
    minSpeciesSizeLabel->setToolTip("<font>Selects the minimum number of organisms needed to define a species. Min = 0; Max = 1000000.</font>");
    auto *minSpeciesSizeSpin = new QSpinBox;
    minSpeciesSizeSpin->setToolTip("<font>Selects the minimum number of organisms needed to define a species. Min = 0; Max = 1000000.</font>");
    minSpeciesSizeSpin->setMinimum(0);
    minSpeciesSizeSpin->setMaximum(1000000);
    minSpeciesSizeSpin->setValue(static_cast<int>(minSpeciesSize));
    fileLoggingGrid->addWidget(minSpeciesSizeLabel, 6, 1);
    fileLoggingGrid->addWidget(minSpeciesSizeSpin, 6, 2);
    connect(minSpeciesSizeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        minSpeciesSize = static_cast<quint64>(i);
    });

    //ARTS - Advanced
    auto *advancedLoggingGrid = new QGridLayout;

    QLabel *advancedSettingsLabel = new QLabel("Advanced");
    advancedSettingsLabel->setObjectName("advancedSettingsLabel");
    advancedLoggingGrid->addWidget(advancedSettingsLabel, 1, 1, 1, 2);

    guiCheckbox = new QCheckBox("Don't update GUI on refresh/poll");
    guiCheckbox->setChecked(gui);
    guiCheckbox->setToolTip("<font>Note: If you turn off GUI update you cannot log the population/environment windows using saved images.</font>");
    advancedLoggingGrid->addWidget(guiCheckbox, 2, 1, 1, 2);
    QObject::connect(guiCheckbox, SIGNAL (toggled(bool)), this, SLOT(guiCheckboxStateChanged(bool)));

    //ARTS - Dock Grid Layout
    outputSettingsGrid->addLayout(savePathGrid, 1, 1, 1, 2);
    outputSettingsGrid->addLayout(pollingRateGrid, 2, 1, 1, 2);
    outputSettingsGrid->addLayout(images_grid, 3, 1, 1, 2);
    outputSettingsGrid->addLayout(fileLoggingGrid, 4, 1, 1, 2);
    outputSettingsGrid->addLayout(advancedLoggingGrid, 5, 1, 1, 2);

    QWidget *outputSettingsLayoutWidget = new QWidget;
    outputSettingsLayoutWidget->setLayout(outputSettingsGrid);
    outputSettingsDock->setWidget(outputSettingsLayoutWidget);

    return outputSettingsDock;
}

/*!
 * \brief MainWindow::createOrganismSettingsDock
 * \return QDockWidget organismSettingsDock
 */
QDockWidget *MainWindow::createOrganismSettingsDock()
{
    organismSettingsDock = new QDockWidget("Organism", this);
    organismSettingsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    organismSettingsDock->setFeatures(QDockWidget::DockWidgetMovable);
    organismSettingsDock->setFeatures(QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, organismSettingsDock);

    auto *organismSettingsGrid = new QGridLayout;
    organismSettingsGrid->setAlignment(Qt::AlignTop);

    QLabel *organismSettingsLabel = new QLabel("Organism settings");
    organismSettingsLabel->setStyleSheet("font-weight: bold");
    organismSettingsGrid->addWidget(organismSettingsLabel, 1, 1, 1, 2);

    QLabel *mutateLabel = new QLabel("Chance of mutation:");
    mutateLabel->setToolTip("<font>Selects the chance of mutation. Min = 0; Max = 255.</font>");
    mutateSpin = new QSpinBox;
    mutateSpin->setMinimum(0);
    mutateSpin->setMaximum(255);
    mutateSpin->setValue(mutate);
    mutateSpin->setToolTip("<font>Selects the chance of mutation. Min = 0; Max = 255.</font>");
    organismSettingsGrid->addWidget(mutateLabel, 2, 1);
    organismSettingsGrid->addWidget(mutateSpin, 2, 2);
    connect(mutateSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        mutate = i;
    });

    QLabel *startAgeLabel = new QLabel("Start age:");
    startAgeLabel->setToolTip("<font>Sets the starting age for organisms. Min = 1; Max = 1000.</font>");
    startAgeSpin = new QSpinBox;
    startAgeSpin->setMinimum(1);
    startAgeSpin->setMaximum(1000);
    startAgeSpin->setValue(startAge);
    startAgeSpin->setToolTip("<font>Sets the starting age for organisms. Min = 1; Max = 1000.</font>");
    organismSettingsGrid->addWidget(startAgeLabel, 4, 1);
    organismSettingsGrid->addWidget(startAgeSpin, 4, 2);
    connect(startAgeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        startAge = i;
    });

    QLabel *breedSettingsLabel = new QLabel("Breed settings");
    breedSettingsLabel->setStyleSheet("font-weight: bold");
    organismSettingsGrid->addWidget(breedSettingsLabel, 5, 1, 1, 2);

    QLabel *breedThresholdLabel = new QLabel("Breed threshold:");
    breedThresholdLabel->setToolTip("<font>Sets the 'Breed Threshold'. Min = 0; Max = 5000.</font>");
    breedThresholdSpin = new QSpinBox;
    breedThresholdSpin->setMinimum(1);
    breedThresholdSpin->setMaximum(5000);
    breedThresholdSpin->setValue(breedThreshold);
    breedThresholdSpin->setToolTip("<font>Sets the 'Breed Threshold'. Min = 0; Max = 5000.</font>");
    organismSettingsGrid->addWidget(breedThresholdLabel, 6, 1);
    organismSettingsGrid->addWidget(breedThresholdSpin, 6, 2);
    connect(breedThresholdSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        breedThreshold = i;
    });

    QLabel *breedCostLabel = new QLabel("Breed cost:");
    breedCostLabel->setToolTip("<font>Sets the 'Breed Cost'. Min = 0; Max = 10000.</font>");
    breedCostSpin = new QSpinBox;
    breedCostSpin->setMinimum(1);
    breedCostSpin->setMaximum(10000);
    breedCostSpin->setValue(breedCost);
    breedCostSpin->setToolTip("<font>Sets the 'Breed Cost'. Min = 0; Max = 10000.</font>");
    organismSettingsGrid->addWidget(breedCostLabel, 7, 1);
    organismSettingsGrid->addWidget(breedCostSpin, 7, 2);
    connect(breedCostSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        breedCost = i;
    });

    QLabel *maxDifferenceLabel = new QLabel("Max difference to breed:");
    maxDifferenceLabel->setToolTip("<font>Sets the maximum difference between organisms to allow breeding. Min = 1; Max = 31.</font>");
    maxDifferenceSpin = new QSpinBox;
    maxDifferenceSpin->setMinimum(1);
    maxDifferenceSpin->setMaximum(31);
    maxDifferenceSpin->setValue(maxDifference);
    maxDifferenceSpin->setToolTip("<font>Sets the maximum difference between organisms to allow breeding. Min = 1; Max = 31.</font>");
    organismSettingsGrid->addWidget(maxDifferenceLabel, 8, 1);
    organismSettingsGrid->addWidget(maxDifferenceSpin, 8, 2);
    connect(maxDifferenceSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        maxDifference = i;
    });

    breedDifferenceCheckbox = new QCheckBox("Use max diff to breed");
    breedDifferenceCheckbox->setToolTip("<font>Turn on/off the maximum difference between organisms test.</font>");
    organismSettingsGrid->addWidget(breedDifferenceCheckbox, 9, 1, 1, 1);
    breedDifferenceCheckbox->setChecked(breedDifference);
    connect(breedDifferenceCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        breedDifference = i;
    });

    breedSpeciesCheckbox = new QCheckBox("Breed only within species");
    breedSpeciesCheckbox->setToolTip("<font>Turn on/off breeding only within the same species.</font>");
    organismSettingsGrid->addWidget(breedSpeciesCheckbox, 10, 1, 1, 1);
    breedSpeciesCheckbox->setChecked(breedSpecies);
    connect(breedSpeciesCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        breedSpecies = i;
    });

    QLabel *breed_modeLabel = new QLabel("Breed mode:");
    organismSettingsGrid->addWidget(breed_modeLabel, 11, 1, 1, 2);
    sexualRadio = new QRadioButton("Sexual");
    sexualRadio->setToolTip("<font>Select to use 'Sexual' breeding.</font>");
    asexualRadio = new QRadioButton("Asexual");
    asexualRadio->setToolTip("<font>Select to use 'Asexual' breeding.</font>");
    auto *breedingButtonGroup = new QButtonGroup;
    breedingButtonGroup->addButton(sexualRadio, 0);
    breedingButtonGroup->addButton(asexualRadio, 1);
    sexualRadio->setChecked(sexual);
    asexualRadio->setChecked(asexual);
    organismSettingsGrid->addWidget(sexualRadio, 12, 1, 1, 2);
    organismSettingsGrid->addWidget(asexualRadio, 13, 1, 1, 2);
    connect(breedingButtonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [ = ](const int &i)
    {
        if (i == 0)
        {
            sexual = true;
            asexual = false;
        }
        if (i == 1)
        {
            sexual = false;
            asexual = true;
        }
    });

    QLabel *settleSettingsLabel = new QLabel("Settle settings");
    settleSettingsLabel->setStyleSheet("font-weight: bold");
    organismSettingsGrid->addWidget(settleSettingsLabel, 15, 1, 1, 2);

    QLabel *dispersalLabel = new QLabel("Dispersal:");
    dispersalLabel->setToolTip("<font>Set the maximum dispersal of offspring from parent. Min = 1; Max = 200.</font>");
    dispersalSpin = new QSpinBox;
    dispersalSpin->setMinimum(1);
    dispersalSpin->setMaximum(200);
    dispersalSpin->setValue(dispersal);
    dispersalSpin->setToolTip("<font>Set the maximum dispersal of offspring from parent. Min = 1; Max = 200.</font>");
    organismSettingsGrid->addWidget(dispersalLabel, 16, 1);
    organismSettingsGrid->addWidget(dispersalSpin, 16, 2);
    connect(dispersalSpin, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        dispersal = i;
    });

    nonspatialCheckbox = new QCheckBox("Nonspatial settling");
    nonspatialCheckbox->setToolTip("<font>Turn on/off nonspatial settling of offspring.</font>");
    organismSettingsGrid->addWidget(nonspatialCheckbox, 17, 1, 1, 2);
    nonspatialCheckbox->setChecked(nonspatial);
    connect(nonspatialCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        nonspatial = i;
    });

    QWidget *orgnismSettingsLayoutWidget = new QWidget;
    orgnismSettingsLayoutWidget->setLayout(organismSettingsGrid);
    organismSettingsDock->setWidget(orgnismSettingsLayoutWidget);

    return organismSettingsDock;
}

/*!
 * \brief MainWindow::updateGlobalPath
 *
 * Change the save globalSavePath for various stuff.
 */
void MainWindow::updateGlobalPath()
{
    QString dirname = QFileDialog::getExistingDirectory(this, "Select directory in which files should be saved.");
    if (dirname.length() != 0)
    {
        dirname.append("/");
        globalSavePath->setText(dirname);
    }

}

/*!
 * \brief MainWindow::waitUntilPauseSignalIsEmitted
 * \return QEventLoop
 *
 * Adds a null loop to the simulation iteration run when pause button/command is descendantsd
 * this loop is removed on the next tiggered() signal send from either one.
 */
int MainWindow::waitUntilPauseSignalIsEmitted()
{
    QEventLoop loop;
    QObject::connect(pauseButton, SIGNAL(triggered()), &loop, SLOT(quit()));
    QObject::connect(ui->actionPause_Sim, SIGNAL(triggered()), &loop, SLOT(quit()));
    return loop.exec();
}

/*!
 * \brief MainWindow::on_actionAbout_triggered
 *
 * Tiggered action that opens a new About dialog.
 */
void MainWindow::on_actionAbout_triggered()
{
    About adialogue;
    adialogue.exec();
}

/*!
 * \brief MainWindow::resetSimulation
 *
 * Resets the simulation (i.e. fill the centre pixel with a genome, then sets up a run).
 */
void MainWindow::resetSimulation()
{
    // Reset the information bar
    resetInformationBar();

    //RJG - This resets all the species logging stuff as well as setting up the run
    simulationManager->setupRun();
    nextRefresh = 0;

    //ARTS - Update views based on the new reset simulation
    refreshPopulations();
}

/*!
 * \brief MainWindow::resetInformationBar
 *
 * Resets the bottom information bar
 */
void MainWindow::resetInformationBar()
{
    //ARTS - reset the bottom information bar
    ui->LabelBatch->setText(tr("1/1"));
    ui->LabelIteration->setText(tr("0"));
    ui->LabelIterationsPerHour->setText(tr("0.00"));
    ui->LabelCritters->setText(tr("0"));
    ui->LabelSpeed->setText(tr("0.00"));
    ui->LabelFitness->setText(tr("0.00%"));
    ui->LabelSpecies->setText(tr("-"));

    QString environmentSceneValue;
    environmentSceneValue.sprintf("%d/%d", currentEnvironmentFile + 1, environmentFiles.count());
    ui->LabelEnvironment->setText(environmentSceneValue);
}

/*!
 * \brief MainWindow::launchReseedDialog
 *
 * Reseed provides options to either reset using a random genome, or a user defined one - drawn
 * from the genome comparison docker.
 */
void MainWindow::launchReseedDialog()
{
    Reseed reseed_dialogue;
    reseed_dialogue.exec();
    resetSimulation();
}

/*!
 * \brief MainWindow::changeEvent
 * \param e
 */
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/*!
 * \brief MainWindow::startSimulation
 *
 * Triggered simulation "Run" action.
 */
void MainWindow::startSimulation()
{

    if (currentEnvironmentFile == -1)
    {
        QMessageBox::critical(nullptr, "", "Cannot start simulation without environment");
        if (!loadEnvironmentFiles())
        {
            return;
        }
    }

    runSetUp();

    ui->LabelBatch->setText(tr("1/1"));

    while (!stopFlag)
    {
        while (pauseFlag)
        {
            waitUntilPauseSignalIsEmitted();
            pauseFlag = false;
        }

        report();
        qApp->processEvents();
        if (ui->actionGo_Slow->isChecked()) Sleeper::msleep(30);

        //ARTS - set Stop flag to returns true if reached end... but why? It will fire the finishRun() function at the end.
        if (simulationManager->iterate(environmentMode, environmentInterpolate))stopFlag = true;
        if (!aliveCount) simulationDead();
    }

    finishRun();
}

void MainWindow::simulationDead()
{
    nextRefresh = 1;
    report();

    QMessageBox::warning(nullptr, "Stopping simulation",
                         "It looks like nothing is alive in your simulation. This could be due to the settings you are using, or just bad luck. The simulation will terminate and reseed for you.");
    stopFlag = true;

    resetSimulation();
}

/*!
 * \brief MainWindow::runForNSimulation
 *
 * Triggered simulation "Run For..." action.
 */
void MainWindow::runForNSimulation()
{
    if (currentEnvironmentFile == -1)
    {
        QMessageBox::critical(nullptr, "", "Cannot start simulation without environment");
        if (!loadEnvironmentFiles())
        {
            return;
        }
    }

    bool ok = false;
    int i;
    int n;
    n = QInputDialog::getInt(this, "", tr("Iterations: "), 1000, 1, 10000000, 1, &ok);
    i = n;
    if (!ok) return;

    ui->LabelBatch->setText(tr("1/1"));

    runSetUp();

    while (!stopFlag && i > 0)
    {
        while (pauseFlag)
        {
            waitUntilPauseSignalIsEmitted();
            pauseFlag = false;
        }

        report();
        qApp->processEvents();

        if (simulationManager->iterate(environmentMode, environmentInterpolate)) stopFlag = true;
        if (!aliveCount) simulationDead();
        i--;
    }

    if (autowriteLogCheckbox->isChecked())writeRunData();

    //ARTS Show finish message and run FinshRun()
    if (!stopFlag)
    {
        QMessageBox::information(nullptr, tr("Run For... Finished"), tr("The run for %1 iterations has finished.").arg(n));
        finishRun();
    }
    else
    {
        QMessageBox::information(nullptr, tr("Run For... Stopped"), tr("The run for %1 iterations has been stopped at iteration %2.").arg(n).arg(i));
        finishRun();
    }
}

/*!
 * \brief MainWindow::startBatchSimulation
 *
 * Batch - primarily intended to allow repeats of runs with the same settings, rather than allowing these to be changed between runs.
 */
void MainWindow::startBatchSimulation()
{
    //ARTS - set default vaules
    batchRunning = true;
    batchRuns = 0;
    int environmentStart = currentEnvironmentFile;

    bool repeatEnvironment;
    QString globalSavePathStr(globalSavePath->text());

    //ARTS - batch setup default and maxium values
    int maxIterations = 10000000;
    int defaultIterations = 1000;
    int maxRuns = 10000000;
    int defaultRuns = 1000;

    //ARTS - start of batch setup dialog form
    QDialog dialog(this);
    dialog.setMinimumSize(480, 150);
    dialog.setWindowTitle(tr("Batch Run Setup"));

    QFormLayout form(&dialog);
    // Add some text above the fields
    form.addRow(new QLabel("You may: 1) set the number of runs you require; 2) set the number of iterations per run; and 3) choose to repeat or not to repeat the environment each run."));

    auto *iterationsSpinBox = new QSpinBox(&dialog);
    iterationsSpinBox->setRange(2, maxIterations);
    iterationsSpinBox->setSingleStep(1);
    iterationsSpinBox->setValue(defaultIterations);
    QString iterationsLabel = QString(tr("How many iterations would you like each run to go for (max = %1)?")).arg(maxIterations);
    form.addRow(iterationsLabel, iterationsSpinBox);

    auto *runsSpinBox = new QSpinBox(&dialog);
    runsSpinBox->setRange(1, maxRuns);
    runsSpinBox->setSingleStep(2);
    runsSpinBox->setValue(defaultRuns);
    QString runsLabel = QString(tr("And how many runs (max = %1)?")).arg(maxRuns);
    form.addRow(runsLabel, runsSpinBox);

    auto *environmentComboBox = new QComboBox(&dialog);
    environmentComboBox->addItem("Yes", 1);
    environmentComboBox->addItem("No", 0);
    int index = environmentComboBox->findData(1);
    if (index != -1 )   // -1 for not found
    {
        environmentComboBox->setCurrentIndex(index);
    }
    QString environmentLabel = QString(tr("Would you like the environment to repeat with each batch?"));
    form.addRow(environmentLabel, environmentComboBox);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal,
                               &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    //ARTS - end of batch setup dialog form

    //ARTS - if OK has been pressed take values and update the defaults, else return false without running batch
    if (dialog.exec() == QDialog::Accepted)
    {
        batchIterations = static_cast<quint64>(iterationsSpinBox->value());
        batchTargetRuns = static_cast<quint64>(runsSpinBox->value());
        repeatEnvironment = environmentComboBox->itemData(environmentComboBox->currentIndex()) == 1;

        // Reset before starting batch run
        resetSimulation();

        ui->LabelBatch->setText(tr("%1/%2").arg(1).arg(batchTargetRuns));
    }
    else
    {
        return;
    }

    //ARTS - run the batch
    do
    {

        //RJG - Sort environment so it repeats
        if (repeatEnvironment)
        {
            currentEnvironmentFile = environmentStart;
            simulationManager->loadEnvironmentFromFile(environmentMode);
        }

        //And run...
        ui->LabelBatch->setText(tr("%1/%2").arg((batchRuns + 1)).arg(batchTargetRuns));

        if (currentEnvironmentFile == -1)
        {
            QMessageBox::critical(nullptr, "", "Cannot start simulation without environment");
            if (!loadEnvironmentFiles())
            {
                return;
            }
        }

        runSetUp();
        quint64 i = batchIterations;
        while (!stopFlag && i > 0)
        {
            while (pauseFlag)
            {
                waitUntilPauseSignalIsEmitted();
                pauseFlag = false;
            }

            report();
            qApp->processEvents();

            simulationManager->iterate(environmentMode, environmentInterpolate);
            if (!aliveCount) break;
            i--;
        }

        if (autowriteLogCheckbox->isChecked())writeRunData();

        batchRuns++;

        if (!stopFlag && batchRuns < batchTargetRuns)
        {
            resetSimulation();
        }

    }
    while (batchRuns < batchTargetRuns && !stopFlag);

    //ARTS Show finish message and reset batch counters
    if ((batchRuns) == batchTargetRuns)
    {
        QMessageBox::information(
            nullptr,
            tr("Batch Finished"),
            tr("The batch of %1 runs with %2 iterations has finished.")
            .arg(batchTargetRuns)
            .arg(batchIterations)
        );
    }
    else
    {
        QMessageBox::information(
            nullptr,
            tr("Batch Stopped"),
            tr("The batch of %1 runs with %2 iterations has been stopped at batch run number %3.")
            .arg(batchTargetRuns)
            .arg(batchIterations)
            .arg(batchRuns)
        );
    }

    globalSavePath->setText(globalSavePathStr);
    batchRuns = 0;
    batchRunning = false;
    finishRun();
}

/*!
 * \brief MainWindow::pauseSimulation
 *
 * Pause function to halt the simulation mid run and allow restart at same point.
 * Note this disables the Stop button as the Stop function runs outside the iteration loop,
 * so can not be triggered while paused.
 */
void MainWindow::pauseSimulation()
{
    if (pauseFlag)
    {
        pauseFlag = false;
        ui->actionStop_Sim->setEnabled(true);
        ui->actionPause_Sim->setText(tr("Pause"));
        ui->actionPause_Sim->setToolTip(tr("Pause"));
        stopButton->setEnabled(true);
        pauseButton->setText(tr("Pause"));
    }
    else
    {
        pauseFlag = true;
        ui->actionStop_Sim->setEnabled(false);
        ui->actionPause_Sim->setText(tr("Resume"));
        ui->actionPause_Sim->setToolTip(tr("Resume"));
        stopButton->setEnabled(false);
        pauseButton->setText(tr("Resume"));
    }
}

/*!
 * \brief MainWindow::stopSimulation
 *
 * Sets the stopFlag to be true on Stop button/command trigger.
 */
void MainWindow::stopSimulation()
{
    stopFlag = true;
}

/*!
 * \brief MainWindow::runSetUp
 *
 * Sets up the defaults for a simulation run.
 */
void MainWindow::runSetUp()
{
    stopFlag = false;

    // Run start action
    ui->actionStart_Sim->setEnabled(false);
    startButton->setEnabled(false);

    // Run for... start action
    ui->actionRun_for->setEnabled(false);
    runForButton->setEnabled(false);

    // Batched Run start action
    ui->actionBatch->setEnabled(false);
    runForBatchButton->setEnabled(false);

    // Pause action
    ui->actionPause_Sim->setEnabled(true);
    pauseButton->setEnabled(true);

    // Stop action
    ui->actionStop_Sim->setEnabled(true);
    stopButton->setEnabled(true);

    // Reset action
    ui->actionReset->setEnabled(false);
    resetButton->setEnabled(false);

    // Reseed action
    ui->actionReseed->setEnabled(false);
    reseedButton->setEnabled(false);

    if (logging && speciesMode == SPECIES_MODE_NONE)
        QMessageBox::warning(this, "Be aware", "Species tracking is off, so the log files won't show species information");

    //Sort out globalSavePath
    QString save_path_string(globalSavePath->text());
    if (!save_path_string.endsWith(QDir::separator())) globalSavePath->setText(save_path_string + QDir::separator());
    QDir globalSavePathStr(globalSavePath->text());
    if (!globalSavePathStr.exists())
    {
        QMessageBox::warning(
            nullptr,
            "Error!",
            "The program doesn't think the save directory exists, so is going to default back to the direcctory in which the executable is."
        );
        QString program_path(QCoreApplication::applicationDirPath());
        program_path.append(QDir::separator());
        globalSavePath->setText(program_path);
        globalSavePathStr.setPath(program_path);
    }
    //RJG - Set up save directory
    if (!globalSavePath->text().endsWith(QString(PRODUCTNAME) + "_output" + QDir::separator()))
    {
        if (!globalSavePathStr.mkpath(QString(PRODUCTNAME) + "_output" + QDir::separator()))
        {
            QMessageBox::warning(this, "Error", "Cant save images. Permissions issue?");
            return;
        }
        globalSavePath->setText(globalSavePathStr.absolutePath() + QDir::separator() + QString(PRODUCTNAME) + "_output" + QDir::separator());
    }

    timer.restart();
    nextRefresh = refreshRate;

    if (loggingCheckbox->isChecked())
        writeLog();
}

/*!
 * \brief MainWindow::finishRun
 *
 * Resets the buttons/commands back to a pre-run state.
 */
void MainWindow::finishRun()
{
    // Run start action
    ui->actionStart_Sim->setEnabled(true);
    startButton->setEnabled(true);

    // Run for... start action
    ui->actionRun_for->setEnabled(true);
    runForButton->setEnabled(true);

    // Batched Run start action
    ui->actionBatch->setEnabled(true);
    runForBatchButton->setEnabled(true);

    // Pause action
    ui->actionPause_Sim->setEnabled(false);
    pauseButton->setEnabled(false);

    // Stop action
    ui->actionStop_Sim->setEnabled(false);
    stopButton->setEnabled(false);

    // Reset action
    ui->actionReset->setEnabled(true);
    resetButton->setEnabled(true);

    // Reseed action
    ui->actionReseed->setEnabled(true);
    reseedButton->setEnabled(true);
}

/*!
 * \brief MainWindow::closeEvent
 *
 * Main close action.
 */
void MainWindow::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);

    exit(0);
}

/*!
 * \brief MainWindow::report
 *
 * Updates reports, and does logging.
 */
void MainWindow::report()
{
    if (--nextRefresh > 0)
        return;

    nextRefresh = refreshRate;

    QString s;
    QTextStream sout(&s);

    int time = timer.elapsed();
    float atime = static_cast<float>(time / refreshRate);
    timer.restart();
    double t = 0;
    for (int n2 = 0; n2 < gridX; n2++)
        for (int m2 = 0; m2 < gridY; m2++)
            t += totalFitness[n2][m2];
    t /= static_cast<double>(aliveCount);
    t /= static_cast<double>(settleTolerance);
    t *= 100; //now %age

    QString out;
    QTextStream o(&out);

    o << iteration; //need to use to avoid int64 descendants
    ui->LabelIteration->setText(out);

    out.sprintf("%.2fk", static_cast<double>((3600000 / atime) / 1000));
    ui->LabelIterationsPerHour->setText(out);

    //now back to sprintf for convenience
    if (currentEnvironmentFile >= environmentFiles.count())
        out.sprintf("Finished (%d)", environmentFiles.count());
    else
        out.sprintf("%d/%d", currentEnvironmentFile + 1, environmentFiles.count());
    ui->LabelEnvironment->setText(out);

    out.sprintf("%.2f%%", t);
    ui->LabelFitness->setText(out);

    out.sprintf("%.2f", static_cast<double>(atime));
    ui->LabelSpeed->setText(out);

    out.sprintf("%d", aliveCount);
    ui->LabelCritters->setText(out);

    calculateSpecies();
    out = "-";
    if (speciesMode != SPECIES_MODE_NONE)
    {
        int g5 = 0, g50 = 0;
        for (int i = 0; i < oldSpeciesList.count(); i++)
        {
            if (oldSpeciesList[i].size > 5) g5++;
            if (oldSpeciesList[i].size > 50) g50++;
        }
        out.sprintf("%d (>5:%d >50:%d)", oldSpeciesList.count(), g5, g50);
    }
    ui->LabelSpecies->setText(out);

    //do species stuff
    if (!gui)refreshPopulations();
    if (!gui)refreshEnvironment();

    writeLog();

    //reset the breedAttempts and breedFails arrays
    for (int n2 = 0; n2 < gridX; n2++)
        for (int m2 = 0; m2 < gridY; m2++)
        {
            //breedAttempts[n2][m2]=0;
            breedFails[n2][m2] = 0;
            settles[n2][m2] = 0;
            settleFails[n2][m2] = 0;
        }
}

/*!
 * \brief MainWindow::scaleFails
 * \param fails
 * \param generations
 * \return int
 *
 * Scales colour of fail count, correcting for generations, and scaling high values to something saner.
 */
int MainWindow::scaleFails(int fails, float generations)
{
    float ffails = (static_cast<float>(fails)) / generations;

    ffails *= 100.0; // a fudge factor no less! Well it's only visualization...
    ffails = static_cast<float>(pow(static_cast<double>(ffails), 0.8));

    if (ffails > static_cast<float>(255.0))
        ffails = static_cast<float>(255.0);

    return static_cast<int>(ffails);
}

/*!
 * \brief MainWindow::on_populationWindowComboBox_currentIndexChanged
 * \param index
 *
 * Catches any changes to the population window dropdown.
 */
void MainWindow::on_populationWindowComboBox_currentIndexChanged(int index)
{
    // 0 = Population count
    // 1 = Mean fitness
    // 2 = Coding genome as colour
    // 3 = NonCoding genome as colour
    // 4 = Gene Frequencies
    // 5 = Breed Attempts
    // 6 = Breed Fails
    // 7 = Settles
    // 8 = Breed/Settle Fails (R=Breed, G=Settle)
    // 9 = Breed Fails 2
    // 10 = Species
    int currentSelectedMode = ui->populationWindowComboBox->itemData(index).toInt();
    Q_UNUSED(currentSelectedMode);

    //view_mode_changed();
    refreshPopulations();
}

/*!
 * \brief MainWindow::refreshPopulations
 *
 * Rereshes the population window. Also runs the species identification code.
 */
void MainWindow::refreshPopulations()
{
    //RJG - make globalSavePath if required - this way as if user adds file name to globalSavePath, this will create a
    //subfolder with the same file name as logs.
    QString globalSavePathStr(globalSavePath->text());
    if (!globalSavePathStr.endsWith(QDir::separator()))
        globalSavePathStr.append(QDir::separator());
    if (batchRunning)
    {
        globalSavePathStr.append(QString("Images_run_%1").arg(batchRuns, 4, 10, QChar('0')));
        globalSavePathStr.append(QDir::separator());
    }
    QDir save_dir(globalSavePathStr);

    // 0 = Population count
    // 1 = Mean fitness
    // 2 = Coding genome as colour
    // 3 = NonCoding genome as colour
    // 4 = Gene Frequencies
    // 5 = Breed Attempts
    // 6 = Breed Fails
    // 7 = Settles
    // 8 = Settle Fails === Fails (R=Breed, G=Settle)
    // 9 = Breed Fails 2
    // 10 = Species

    //ARTS - Check to see what the mode is from the Population Window QComboBox; return the data as int.
    int currentSelectedMode = ui->populationWindowComboBox->itemData(ui->populationWindowComboBox->currentIndex()).toInt();

    // (0) Population Count
    //if (ui->actionPopulation_Count->isChecked()||savePopulationCount->isChecked())
    if (currentSelectedMode == 0 || savePopulationCount->isChecked())
    {
        //Popcount
        int bigcount = 0;
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {
                int count = 0;
                for (int c = 0; c < slotsPerSquare; c++)
                {
                    if (critters[n][m][c].age) count++;
                }
                bigcount += count;
                count *= 2;
                if (count > 255) count = 255;
                populationImage->setPixel(n, m, static_cast<uint>(count));
            }
        //if (ui->actionPopulation_Count->isChecked())populationItem->setPixmap(QPixmap::fromImage(*populationImage));
        if (currentSelectedMode == 0)
            populationItem->setPixmap(QPixmap::fromImage(*populationImage));
        if (savePopulationCount->isChecked())
            if (save_dir.mkpath("population/"))
                populationImage->save(QString(save_dir.path() + "/population/REvoSim_population_it_%1.png").arg(iteration, 7, 10, QChar('0')));
    }

    // (1) Fitness
    if (currentSelectedMode == 1 || saveMeanFitness->isChecked())
    {
        //Popcount
        int multiplier = 255 / settleTolerance;
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {
                int count = 0;
                for (int c = 0; c < slotsPerSquare; c++)
                {
                    if (critters[n][m][c].age) count++;
                }
                if (count == 0)
                    populationImage->setPixel(n, m, 0);
                else
                    populationImage->setPixel(n, m, static_cast<uint>((totalFitness[n][m] * static_cast<uint>(multiplier)) / static_cast<uint>(count)));

            }
        if (currentSelectedMode == 1)
            populationItem->setPixmap(QPixmap::fromImage(*populationImage));
        if (saveMeanFitness->isChecked())
            if (save_dir.mkpath("fitness/"))
                populationImageColour->save(QString(save_dir.path() + "/fitness/REvoSim_mean_fitness_it_%1.png").arg(iteration, 7, 10, QChar('0')));
    }

    // (2) Genome as colour
    if (currentSelectedMode == 2 || saveCodingGenomeAsColour->isChecked())
    {
        //find modal genome in each square, convert to colour
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {
                //data structure for mode
                quint64 genomes[SLOTS_PER_GRID_SQUARE];
                int counts[SLOTS_PER_GRID_SQUARE];
                int arraypos = 0; //pointer

                if (totalFitness[n][m] == 0) populationImageColour->setPixel(n, m, 0); //black if square is empty
                else
                {
                    //for each used slot
                    for (int c = 0; c < maxUsed[n][m]; c++)
                    {
                        if (critters[n][m][c].age > 0)
                        {
                            //If critter is alive

                            quint64 g = critters[n][m][c].genome;

                            //find genome frequencies
                            for (int i = 0; i < arraypos; i++)
                            {
                                if (genomes[i] == g)   //found it
                                {
                                    counts[i]++;
                                    goto gotcounts;
                                }
                            }
                            //didn't find it
                            genomes[arraypos] = g;
                            counts[arraypos++] = 1;
                        }
gotcounts:
                        ;
                    }

                    //find max frequency
                    int max = -1;
                    quint64 maxg = 0;

                    for (int i = 0; i < arraypos; i++)
                        if (counts[i] > max)
                        {
                            max = counts[i];
                            maxg = genomes[i];
                        }

                    //now convert first 32 bits to a colour
                    // r,g,b each counts of 11,11,10 bits
                    auto genome = static_cast<quint32>((maxg & (static_cast<quint64>(65536) * static_cast<quint64>(65536) - static_cast<quint64>(1))));
                    quint32 b = bitCounts[genome & 2047] * 23;
                    genome /= 2048;
                    quint32 g = bitCounts[genome & 2047] * 23;
                    genome /= 2048;
                    quint32 r = bitCounts[genome] * 25;
                    populationImageColour->setPixel(n, m, qRgb(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)));
                }

            }

        if (currentSelectedMode == 2)
            populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
        if (saveCodingGenomeAsColour->isChecked())
            if (save_dir.mkpath("coding/"))
                populationImageColour->save(QString(save_dir.path() + "/coding/REvoSim_coding_genome_it_%1.png").arg(iteration, 7, 10, QChar('0')));
    }

    // (3) Non-coding Genome
    if (currentSelectedMode == 3 || saveNonCodingGenomeAsColour->isChecked())
    {
        //find modal genome in each square, convert non-coding to colour
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {
                //data structure for mode
                quint64 genomes[SLOTS_PER_GRID_SQUARE];
                int counts[SLOTS_PER_GRID_SQUARE];
                int arraypos = 0; //pointer

                if (totalFitness[n][m] == 0)
                    populationImageColour->setPixel(n, m, 0); //black if square is empty
                else
                {
                    //for each used slot
                    for (int c = 0; c < maxUsed[n][m]; c++)
                    {
                        if (critters[n][m][c].age > 0)
                        {
                            //If critter is alive

                            quint64 g = critters[n][m][c].genome;

                            //find genome frequencies
                            for (int i = 0; i < arraypos; i++)
                            {
                                if (genomes[i] == g)   //found it
                                {
                                    counts[i]++;
                                    goto gotcounts2;
                                }
                            }
                            //didn't find it
                            genomes[arraypos] = g;
                            counts[arraypos++] = 1;
                        }
gotcounts2:
                        ;
                    }


                    //find max frequency
                    int max = -1;
                    quint64 maxg = 0;

                    for (int i = 0; i < arraypos; i++)
                        if (counts[i] > max)
                        {
                            max = counts[i];
                            maxg = genomes[i];
                        }

                    //now convert second 32 bits to a colour
                    // r,g,b each counts of 11,11,10 bits
                    auto genome = static_cast<quint32>((maxg / (static_cast<quint64>(65536) * static_cast<quint64>(65536))));
                    quint32 b = bitCounts[genome & 2047] * 23;
                    genome /= 2048;
                    quint32 g = bitCounts[genome & 2047] * 23;
                    genome /= 2048;
                    quint32 r = bitCounts[genome] * 25;
                    populationImageColour->setPixel(n, m, qRgb(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)));
                }
            }

        if (currentSelectedMode == 3)
            populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
        if (saveNonCodingGenomeAsColour->isChecked())
            if (save_dir.mkpath("non_coding/"))
                populationImageColour->save(QString(save_dir.path() + "/non_coding/REvoSim_non_coding_it_%1.png").arg(iteration, 7, 10, QChar('0')));
    }

    // (4) Gene Frequencies
    if (currentSelectedMode == 4 || saveGeneFrequencies->isChecked())
    {
        //Popcount
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {
                int count = 0;
                int gen0tot = 0;
                int gen1tot = 0;
                int gen2tot = 0;
                for (int c = 0; c < slotsPerSquare; c++)
                {
                    if (critters[n][m][c].age)
                    {
                        count++;
                        if (critters[n][m][c].genome & 1) gen0tot++;
                        if (critters[n][m][c].genome & 2) gen1tot++;
                        if (critters[n][m][c].genome & 4) gen2tot++;
                    }
                }
                if (count == 0) populationImageColour->setPixel(n, m, qRgb(0, 0, 0));
                else
                {
                    quint32 r = static_cast<quint32>(gen0tot * 256 / count);
                    quint32 g = static_cast<quint32>(gen1tot * 256 / count);
                    quint32 b = static_cast<quint32>(gen2tot * 256 / count);
                    populationImageColour->setPixel(n, m, qRgb(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b)));
                }
            }

        if (currentSelectedMode == 4)
            populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
        if (saveGeneFrequencies->isChecked())
            if (save_dir.mkpath("gene_freq/"))
                populationImageColour->save(QString(save_dir.path() + "/gene_freq/REvoSim_gene_freq_it_%1.png").arg(iteration, 7, 10, QChar('0')));
    }

    // (5) Breed Attempts
    //RJG - No save option as no longer in the menu as an option.
    if (currentSelectedMode == 5)
    {
        //Popcount
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {
                int value = (breedAttempts[n][m] * 10) / refreshRate;
                if (value > 255) value = 255;
                populationImage->setPixel(n, m, static_cast<uint>(value));
            }
        populationItem->setPixmap(QPixmap::fromImage(*populationImage));
    }

    // (6) Breed Fails
    //RJG - No save option as no longer in the menu as an option.
    if (currentSelectedMode == 6)
    {
        //Popcount
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {
                if (breedAttempts[n][m] == 0) populationImage->setPixel(n, m, 0);
                else
                {
                    int value = (breedFails[n][m] * 255) / breedAttempts[n][m];
                    populationImage->setPixel(n, m, static_cast<uint>(value));
                }
            }
        populationItem->setPixmap(QPixmap::fromImage(*populationImage));
    }

    // (7) Settles
    if (currentSelectedMode == 7 || saveSettles->isChecked())
    {
        //Popcount

        // Workout min and max values
        int max = -1;
        int min = -1;

        // Get min and max values
        for (int n = 0; n < gridX; n++)
        {
            for (int m = 0; m < gridY; m++)
            {
                int value = (settles[n][m] * 10) / refreshRate;
                //qDebug() << value;
                if (min < 0) min = value;
                if (max < 0) max = value;
                if (value < min) min = value;
                if (value > max) max = value;
            }
        }

        // Scale each value to 0-255
        for (int n = 0; n < gridX; n++)
        {
            for (int m = 0; m < gridY; m++)
            {
                int value = (settles[n][m] * 10) / refreshRate;

                // Scale values ( x - min(x) ) * (smax - smin) / ( max(x) - min(x) ) + smin
                if (max - min > 0)
                    value = (value - min) * 255 / (max - min);
                else
                    value = 0;

                //qDebug() << value;
                populationImage->setPixel(n, m, static_cast<uint>(value));
            }
        }

        if (currentSelectedMode == 7)populationItem->setPixmap(QPixmap::fromImage(*populationImage));
        if (saveSettles->isChecked())
            if (save_dir.mkpath("settles/"))
                populationImage->save(QString(save_dir.path() + "/settles/REvoSim_settles_it_%1.png").arg(iteration, 7, 10, QChar('0')));
    }

    // (8) Breed/Settle Fails
    //RJG - this now combines breed fails (red) and settle fails (green)
    if (currentSelectedMode == 8 || saveFailsSettles->isChecked())
    {
        //work out average per iteration
        float generations = static_cast<float>(iteration - static_cast<quint64>(lastReport));


        //Make image
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {
                int r = scaleFails(breedFails[n][m], generations);
                int g = scaleFails(settleFails[n][m], generations);
                populationImageColour->setPixel(n, m, qRgb(r, g, 0));
            }

        if (currentSelectedMode == 8)
            populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
        if (saveFailsSettles->isChecked())
            if (save_dir.mkpath("breed_settle_fails/"))
                populationImageColour->save(QString(save_dir.path() + "/breed_settle_fails/REvoSim_breed_settle_fails_it_%1.png").arg(iteration, 7, 10, QChar('0')));

    }

    // (9) Breed Fails 2
    if (currentSelectedMode == 9)
    {
        //Popcount
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {
                if (breedFails[n][m] == 0) populationImage->setPixel(n, m, 0);
                else
                {
                    int value = (breedFails[n][m]);
                    if (value > 255) value = 255;
                    populationImage->setPixel(n, m, static_cast<uint>(value));
                }
            }
        populationItem->setPixmap(QPixmap::fromImage(*populationImage));
    }

    // (10) Species
    if (currentSelectedMode == 10 || saveSpecies->isChecked())   //do visualisation if necessary
    {
        for (int n = 0; n < gridX; n++)
            for (int m = 0; m < gridY; m++)
            {

                if (totalFitness[n][m] == 0)
                    populationImageColour->setPixel(n, m, 0); //black if square is empty
                else
                {
                    quint64 thisspecies = 0;
                    for (int c = 0; c < slotsPerSquare; c++)
                    {
                        if (critters[n][m][c].age > 0)
                        {
                            thisspecies = critters[n][m][c].speciesID;
                            break;
                        }
                    }

                    populationImageColour->setPixel(n, m, speciesColours[thisspecies % 65536]);
                }
            }

        if (currentSelectedMode == 10)
            populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
        if (saveSpecies->isChecked())
            if (save_dir.mkpath("species/"))
                populationImageColour->save(QString(save_dir.path() + "/species/REvoSim_species_it_%1.png").arg(iteration, 7, 10, QChar('0')));

    }

    lastReport = iteration;
}

/*!
 * \brief MainWindow::refreshEnvironment
 *
 * Refreshes the environment window.
 */
void MainWindow::refreshEnvironment()
{
    QString globalSavePathStr(globalSavePath->text());
    if (!globalSavePathStr.endsWith(QDir::separator()))
        globalSavePathStr.append(QDir::separator());
    if (batchRunning)
    {
        globalSavePathStr.append(QString("Images_run_%1").arg(batchRuns, 4, 10, QChar('0')));
        globalSavePathStr.append(QDir::separator());
    }
    QDir save_dir(globalSavePathStr);

    for (int n = 0; n < gridX; n++)
        for (int m = 0; m < gridY; m++)
            environmentImage->setPixel(n, m, qRgb(environment[n][m][0], environment[n][m][1], environment[n][m][2]));

    environmentItem->setPixmap(QPixmap::fromImage(*environmentImage));
    if (saveEnvironment->isChecked())
        if (save_dir.mkpath("environment/"))
            environmentImage->save(QString(save_dir.path() + "/environment/REvoSim_environment_it_%1.png").arg(iteration, 7, 10, QChar('0')));
}

/*!
 * \brief MainWindow::resizeEvent
 *
 * resize windows on window size change event.
 */
void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    resize();
}

/*!
 * \brief MainWindow::resize
 *
 * Force and window resize and rescale of the graphic view
 */
void MainWindow::resize()
{
    ui->GV_Population->fitInView(populationItem, Qt::KeepAspectRatio);
    ui->GV_Environment->fitInView(environmentItem, Qt::KeepAspectRatio);
}

/*!
 * \brief MainWindow::guiCheckboxStateChanged
 * \param dontUpdate
 *
 * Sets the gui don't update flag on gui checkbox change.
 */
void MainWindow::guiCheckboxStateChanged(bool dontUpdate)
{
    if (dontUpdate && QMessageBox::question(
                nullptr,
                "Heads up",
                "If you don't update the GUI, images will also not be saved. OK?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::Yes
            ) == QMessageBox::No
       )
    {
        guiCheckbox->setChecked(false);
        return;
    }

    gui = dontUpdate;
}

/*!
 * \brief MainWindow::saveAllCheckboxStateChanged
 * \param all
 *
 * Checks/unchecks all state checkboxes on check/uncheck of the All checkbox.
 */
void MainWindow::saveAllCheckboxStateChanged(bool all)
{
    savePopulationCount->setChecked(all);
    saveMeanFitness->setChecked(all);
    saveCodingGenomeAsColour->setChecked(all);
    saveNonCodingGenomeAsColour->setChecked(all);
    saveSpecies->setChecked(all);
    saveGeneFrequencies->setChecked(all);
    saveSettles->setChecked(all);
    saveFailsSettles->setChecked(all);
    saveEnvironment->setChecked(all);
}

/*!
 * \brief MainWindow::writeRunData
 *
 * At end of run in run for/batch mode, or on click when a run is going, this allows user to
 * output the final log, along with the tree for the run.
 */
void MainWindow::writeRunData()
{

    QString FinalLoggingFile(globalSavePath->text());
    if (!FinalLoggingFile.endsWith(QDir::separator()))
        FinalLoggingFile.append(QDir::separator());
    FinalLoggingFile.append(QString(PRODUCTNAME) + "_end_run_log");
    if (batchRunning)
        FinalLoggingFile.append(QString("_run_%1").arg(batchRuns, 4, 10, QChar('0')));
    FinalLoggingFile.append(".txt");
    QFile outputfile(FinalLoggingFile);
    outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&outputfile);

    out << "End run log ";
    QDateTime t(QDateTime::currentDateTime());
    out << t.toString(Qt::ISODate) << "\n\n===================\n\n" << printSettings() <<
        "\n\n===================\n";
    out << "\nThis log features the tree from a finished run, in Newick format, and then data for all the species that have existed with more individuals than minimum species size. The exact data provided depends on the phylogeny tracking mode selected in the GUI.\n";
    out << "\n\n===================\n\n";
    out << "Tree:\n\n" << handleAnalysisTool(ANALYSIS_TOOL_CODE_MAKE_NEWICK);
    out << "\n\nSpecies data:\n\n";
    out << handleAnalysisTool(ANALYSIS_TOOL_CODE_WRITE_DATA);
    outputfile.close();
}

/*!
 * \brief MainWindow::environmentModeChanged
 * \param changeEnvironmentMode
 * \param updateGUI
 *
 * Sets the environment mode on change.
 */
void MainWindow::environmentModeChanged(int changeEnvironmentMode, bool updateGUI)
{
    int new_environment_mode = ENV_MODE_STATIC;

    if (changeEnvironmentMode == ENV_MODE_ONCE)
        new_environment_mode = ENV_MODE_ONCE;
    if (changeEnvironmentMode == ENV_MODE_LOOP)
        new_environment_mode = ENV_MODE_LOOP;
    if (changeEnvironmentMode == ENV_MODE_BOUNCE)
        new_environment_mode = ENV_MODE_BOUNCE;

    if (updateGUI)
    {
        if (changeEnvironmentMode == ENV_MODE_STATIC)
            environmentModeStaticButton->setChecked(true);
        if (changeEnvironmentMode == ENV_MODE_ONCE)
            environmentModeOnceButton->setChecked(true);
        if (changeEnvironmentMode == ENV_MODE_LOOP)
            environmentModeLoopButton->setChecked(true);
        if (changeEnvironmentMode == ENV_MODE_BOUNCE)
            environmentModeBounceButton->setChecked(true);
    }

    environmentMode = static_cast<quint8>(new_environment_mode);
}

/*!
 * \brief MainWindow::speciesModeChanged
 * \param changeSpeciesMode
 *
 * Sets the species mode on change.
 */
void MainWindow::speciesModeChanged(int changeSpeciesMode, bool updateGUI)
{
    int newSpeciesMode = SPECIES_MODE_NONE;

    if (changeSpeciesMode == SPECIES_MODE_PHYLOGENY)
        newSpeciesMode = SPECIES_MODE_PHYLOGENY;
    if (changeSpeciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS)
        newSpeciesMode = SPECIES_MODE_PHYLOGENY_AND_METRICS;
    if (changeSpeciesMode == SPECIES_MODE_BASIC)
        newSpeciesMode = SPECIES_MODE_BASIC;

    //some changes not allowed
    if (iteration != static_cast<quint64>(0))
    {
        //already running. Can switch tracking off - but not on
        //detailed tracking can be switched on/off at any point
        if (speciesMode == SPECIES_MODE_NONE)
        {
            if (newSpeciesMode != SPECIES_MODE_NONE)
            {
                QMessageBox::warning(this, "Error", "Turning on species logging is not allowed mid-simulation");
                phylogenyOffButton->setChecked(true);
                return;
            }
        }

        if (speciesMode == SPECIES_MODE_BASIC)
        {
            if (newSpeciesMode == SPECIES_MODE_PHYLOGENY || newSpeciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS)
            {
                QMessageBox::warning(this, "Error", "Turning on phylogeny tracking is not allowed mid-simulation");
                basicPhylogenyButton->setChecked(true);
                return;
            }
        }
        //all other combinations are OK - hopefully
    }

    if (updateGUI)
    {
        if (changeSpeciesMode == SPECIES_MODE_BASIC)
            basicPhylogenyButton->setChecked(true);
        if (changeSpeciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS)
            phylogenyAndMetricsButton->setChecked(true);
        if (changeSpeciesMode == SPECIES_MODE_PHYLOGENY)
            phylogenyButton->setChecked(true);
        if (changeSpeciesMode == SPECIES_MODE_NONE)
            phylogenyOffButton->setChecked(true);
    }

    speciesMode = static_cast<quint8>(newSpeciesMode);
}

/*!
 * \brief MainWindow::resetSquare
 * \param n
 * \param m
 *
 * Resets the grid square.
 */
void MainWindow::resetSquare(int n, int m)
{
    //grid expanded - make sure everything is zeroed in new slots
    for (int c = 0; c < slotsPerSquare; c++) critters[n][m][c].age = 0;

    totalFitness[n][m] = 0;

    breedAttempts[n][m] = 0;
    breedFails[n][m] = 0;
    settles[n][m] = 0;
    settleFails[n][m] = 0;
}

/*!
 * \brief MainWindow::resizeImageObjects
 */
void MainWindow::resizeImageObjects()
{
    delete populationImage;
    delete environmentImage;
    delete populationImageColour;
    populationImage = new QImage(gridX, gridY, QImage::Format_Indexed8);
    QVector <QRgb> clut(256);
    for (int ic = 0; ic < 256; ic++)
        clut[ic] = qRgb(ic, ic, ic);
    populationImage->setColorTable(clut);

    environmentImage = new QImage(gridX, gridY, QImage::Format_RGB32);

    populationImageColour = new QImage(gridX, gridY, QImage::Format_RGB32);
}

/*!
 * \brief MainWindow::redoImages
 * \param oldRows
 * \param oldColumns
 */
void MainWindow::redoImages(int oldRows, int oldColumns)
{
    //check that the maxUsed's are in the new range
    for (int n = 0; n < gridX; n++)
        for (int m = 0; m < gridY; m++)
            if (maxUsed[n][m] >= slotsPerSquare) maxUsed[n][m] = slotsPerSquare - 1;

    //If either rows or cols are bigger - make sure age is set to 0 in all critters in new bit!
    if (gridX > oldRows)
    {
        for (int n = oldRows; n < gridX; n++) for (int m = 0; m < gridY; m++)
                resetSquare(n, m);
    }
    if (gridY > oldColumns)
    {
        for (int n = 0; n < gridX; n++) for (int m = oldColumns; m < gridY; m++)
                resetSquare(n, m);
    }

    resizeImageObjects();

    refreshPopulations();
    refreshEnvironment();
    resize();
}

/*!
 * \brief MainWindow::updateSettingsDockVisability
 *
 * Triggered action to open/close the Settings Dock.
 */
void MainWindow::updateSettingsDockVisability()
{
    if (simulationSettingsDock->isVisible())
    {
        simulationSettingsDock->hide();
        organismSettingsDock->hide();
        outputSettingsDock->hide();
        settingsButton->setChecked(false);
        ui->actionSettings_Dock->setChecked(false);
    }
    else
    {
        simulationSettingsDock->show();
        organismSettingsDock->show();
        outputSettingsDock->show();
        settingsButton->setChecked(true);
        ui->actionSettings_Dock->setChecked(true);
    }
}

/*!
 * \brief MainWindow::writePeakCounts
 *
 * Trigers the Count Peaks action.
 */
void MainWindow::writePeakCounts()
{

    QString peaks(handleAnalysisTool(ANALYSIS_TOOL_CODE_COUNT_PEAKS));
    //Count peaks returns empty string if error.
    if (peaks.length() < 5)
        return;

    QString count_peaks_file(globalSavePath->text());
    if (!count_peaks_file.endsWith(QDir::separator()))
        count_peaks_file.append(QDir::separator());
    count_peaks_file.append(QString(PRODUCTNAME) + "_count_peaks.txt");
    QFile outputfile(count_peaks_file);
    outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&outputfile);

    out << "REvoSim Peak Counting ";
    QDateTime t(QDateTime::currentDateTime());
    out << t.toString(Qt::ISODate) << "\n\n===================\n\n";
    out << "\nBelow is a histogram showing the different fitnesses for all potential 32-bit organisms in REvoSim under the user-defined RGB levels.\n";
    out << "\n\n===================\n\n";
    out << peaks;

    outputfile.close();
}

/*!
 * \brief MainWindow::loadEnvironmentFiles
 * \return bool
 *
 * Action to allow new environmental files to be loaded from the local filesystem.
 */
bool MainWindow::loadEnvironmentFiles()
{
    //Select files
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more image files to load in simulation environment...",
                            "",
                            "Images (*.png *.bmp)"
                        );

    if (files.length() == 0)
        return false;

    bool notsquare = false;
    bool different_size = false;

    for (int i = 0; i < files.length(); i++)
    {
        QImage LoadImage(files[i]);
        int x = LoadImage.width();
        int y = LoadImage.height();
        if (x != y)notsquare = true;
        if (x != 100 || y != 100)different_size = true;
    }
    if (notsquare || different_size)
        QMessageBox::warning(
            this,
            "FYI",
            "For speed REvoSim currently has static arrays for the environment, which limits out of the box functionality to 100 x 100 square environments. "
            "It looks like some of your Environment images don't meet this requirement. Anything smaller than 100 x 100 will be stretched (irrespective of aspect ratio) to 100x100. Anything bigger, and we'll use the top left corner. Should you wish to use a different size environment, please email "
            + QString(EMAIL) + " with this message or go to " + QString(GITURL) + QString(GITREPOSITORY) + QString(GITISSUE) + " and raise a feature request."
        );

    environmentFiles = files;
    currentEnvironmentFile = 0;
    simulationManager->loadEnvironmentFromFile(environmentMode);
    refreshEnvironment();

    //RJG - Reset for this new environment
    simulationManager->setupRun();

    //ARTS - Update the information bar
    QString environmentSceneValue;
    environmentSceneValue.sprintf("%d/%d", currentEnvironmentFile + 1, environmentFiles.count());
    ui->LabelEnvironment->setText(environmentSceneValue);

    return true;
}

/*!
 * \brief MainWindow::saveSimulation
 *
 * Action to save the current settings and simulation to an .revosim file.
 *
 */
void MainWindow::saveSimulation()
{
    QString filename = QFileDialog::getSaveFileName(
                           this,
                           "Save file",
                           "",
                           "REvoSim files (*.revosim)"
                       );

    if (filename.length() == 0)
        return;

    //Otherwise - serialise all my crap
    QFile outfile(filename);
    outfile.open(QIODevice::WriteOnly | QIODevice::Text);

    QDataStream out(&outfile);

    out << QString("REvoSim Format File");
    out << static_cast<int>(FILEVERSION);

    //Ints
    out << gridX;
    out << gridY;
    out << settleTolerance;
    out << slotsPerSquare;
    out << startAge;
    out << dispersal;
    out << food;
    out << breedCost;
    out << mutate;
    out << maxDifference;
    out << breedThreshold;
    out << target;
    out << environmentChangeRate;
    out << environmentMode;
    out << refreshRate;
    out << speciesSamples;
    out << speciesSensitivity;
    out << timeSliceConnect;
    out << minSpeciesSize;
    out << speciesMode;

    //Bools
    out << recalculateFitness;
    out << toroidal;
    out << nonspatial;
    out << breedDifference;
    out << breedSpecies;
    out << allowExcludeWithDescendants;
    out << sexual;
    out << asexual;
    out << logging;
    out << gui;
    out << environmentInterpolate;
    out << fitnessLoggingToFile;
    out << autowriteLogCheckbox->isChecked();
    out << savePopulationCount->isChecked();
    out << saveMeanFitness->isChecked();
    out << saveCodingGenomeAsColour->isChecked();
    out << saveSpecies->isChecked();
    out << saveNonCodingGenomeAsColour->isChecked();
    out << saveGeneFrequencies->isChecked();
    out << saveSettles->isChecked();
    out << saveFailsSettles->isChecked();
    out << saveEnvironment->isChecked();

    //Strings
    out << globalSavePath->text();

    int vmode = 0;
    vmode = ui->populationWindowComboBox->itemData(ui->populationWindowComboBox->currentIndex()).toInt();
    out << vmode;

    //file list
    out << environmentFiles.count();
    for (int i = 0; i < environmentFiles.count(); i++)
        out << environmentFiles[i];

    //now the big arrays
    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
            for (int k = 0; k < slotsPerSquare; k++)
            {
                if ((critters[i][j][k]).age == 0) //its dead
                    out << static_cast<int>(0);
                else
                {
                    out << critters[i][j][k].age;
                    out << critters[i][j][k].genome;
                    out << critters[i][j][k].fitness;
                    out << critters[i][j][k].energy;
                    out << critters[i][j][k].xPosition;
                    out << critters[i][j][k].yPosition;
                    out << critters[i][j][k].zPosition;
                }
            }

    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            out << environment[i][j][0];
            out << environment[i][j][1];
            out << environment[i][j][2];
        }

    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            out << totalFitness[i][j];
        }

    for (auto &xormask : xorMasks)
        for (int j = 0; j < 3; j++)
        {
            out << xormask[j];
        }

    //ARTS - Genome Comparison Saving
    out << genoneComparison->saveComparison();

    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            out << breedAttempts[i][j];
            out << breedFails[i][j];
            out << settles[i][j];
            out << settleFails[i][j];
            out << maxUsed[i][j];
        }

    //And some window state stuff
    out << saveState(); //window state
    out << ui->actionGenomeComparison->isChecked();

    //interpolate environment stuff
    out << environmentInterpolate;
    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            out << environmentLast[i][j][0];
            out << environmentLast[i][j][1];
            out << environmentLast[i][j][2];
        }
    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            out << environmentNext[i][j][0];
            out << environmentNext[i][j][1];
            out << environmentNext[i][j][2];
        }

    out << speciesSamples;
    out << speciesSensitivity;
    out << timeSliceConnect;

    //now the species archive
    out << oldSpeciesList.count();
    for (int j = 0; j < oldSpeciesList.count(); j++)
    {
        out << oldSpeciesList[j].id;
        out << oldSpeciesList[j].type;
        out << oldSpeciesList[j].originTime;
        out << oldSpeciesList[j].parent;
        out << oldSpeciesList[j].size;
        out << oldSpeciesList[j].internalID;
    }

    out << archivedSpeciesLists.count();
    for (int i = 0; i < archivedSpeciesLists.count(); i++)
    {
        out << archivedSpeciesLists[i].count();
        for (int j = 0; j < archivedSpeciesLists[i].count(); j++)
        {
            out << archivedSpeciesLists[i][j].id;
            out << archivedSpeciesLists[i][j].type;
            out << archivedSpeciesLists[i][j].originTime;
            out << archivedSpeciesLists[i][j].parent;
            out << archivedSpeciesLists[i][j].size;
            out << archivedSpeciesLists[i][j].internalID;
        }
    }
    out << nextSpeciesID;
    out << lastSpeciesCalculated;

    //now random number array
    for (unsigned char random : randoms)
        out << random;

    outfile.close();
}

/*!
 * \brief MainWindow::loadSimulation
 *
 * Load a REvoSim file of saved settings and simulations.
 *
 * \todo RJG - Fitness logging to file not sorted on load as yet.
 */
void MainWindow::loadSimulation()
{
    QString filename = QFileDialog::getOpenFileName(
                           this,
                           "Save file",
                           "",
                           "REvoSim files (*.revosim)"
                       );

    if (filename.length() == 0)
        return;

    if (!stopFlag)
        stopFlag = true;

    //Otherwise - serialise all my crap
    QFile infile(filename);
    infile.open(QIODevice::ReadOnly);

    QDataStream in(&infile);

    QString strtemp;
    in >> strtemp;
    if (strtemp != "REvoSim Format File")
    {
        QMessageBox::warning(this, "", "Not an REvoSim Format File.");
        return;
    }

    int version;
    in >> version;
    if (version > FILEVERSION)
    {
        QMessageBox::warning(this, "", "Version too high - will try to read, but may go horribly wrong.");
    }

    //Ints
    in >> gridX;
    in >> gridY;
    in >> settleTolerance;
    in >> slotsPerSquare;
    in >> startAge;
    in >> dispersal;
    in >> food;
    in >> breedCost;
    in >> mutate;
    in >> maxDifference;
    in >> breedThreshold;
    in >> target;
    in >> environmentChangeRate;
    in >> environmentMode;
    in >> refreshRate;
    in >> speciesSamples;
    in >> speciesSensitivity;
    in >> timeSliceConnect;
    in >> minSpeciesSize;
    in >> speciesMode;

    //Bools
    in >> recalculateFitness;
    in >> toroidal;
    in >> nonspatial;
    in >> breedDifference;
    in >> breedSpecies;
    in >> allowExcludeWithDescendants;
    in >> sexual;
    in >> asexual;
    in >> logging;
    in >> gui;
    in >> environmentInterpolate;
    in >> fitnessLoggingToFile;
    bool in_bool;
    in >> in_bool;
    autowriteLogCheckbox->setChecked(in_bool);
    in >> in_bool;
    savePopulationCount->setChecked(in_bool);
    in >> in_bool;
    saveMeanFitness->setChecked(in_bool);
    in >> in_bool;
    saveCodingGenomeAsColour->setChecked(in_bool);
    in >> in_bool;
    saveSpecies->setChecked(in_bool);
    in >> in_bool;
    saveNonCodingGenomeAsColour->setChecked(in_bool);
    in >> in_bool;
    saveGeneFrequencies->setChecked(in_bool);
    in >> in_bool;
    saveSettles->setChecked(in_bool);
    in >> in_bool;
    saveFailsSettles->setChecked(in_bool);
    in >> in_bool;
    saveEnvironment->setChecked(in_bool);

    //Strings
    QString load_path;
    in >> load_path;
    globalSavePath->setText(load_path);

    // 0 = Static
    // 1 = Once
    // 2 = Looped (default)
    // 3 = Bounce
    environmentMode = ENV_MODE_LOOP;
    in >> environmentMode;
    if (environmentMode == 0)
        environmentModeStaticButton->setChecked(true);
    if (environmentMode == 1)
        environmentModeOnceButton->setChecked(true);
    if (environmentMode == 2)
        environmentModeLoopButton->setChecked(true);
    if (environmentMode == 3)
        environmentModeBounceButton->setChecked(true);

    // 0 = Population count
    // 1 = Mean fitnessFails (R-Breed, G=Settle)
    // 2 = Coding genome as colour
    // 3 = NonCoding genome as colour
    // 4 = Gene Frequencies
    // 5 = Breed Attempts
    // 6 = Breed Fails
    // 7 = Settles
    // 8 = Settle Fails
    // 9 = Breed Fails 2
    // 10 = Species
    int vmode;
    in >> vmode;
    int index = ui->populationWindowComboBox->findData(vmode);
    if ( index != -1 )   // -1 for not found
    {
        ui->populationWindowComboBox->setCurrentIndex(index);
    }

    int count;

    //file list
    in >> count;
    environmentFiles.clear();

    for (int i = 0; i < count; i++)
    {
        QString t;
        in >> t;
        environmentFiles.append(t);
    }

    //now the big arrays
    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
            for (int k = 0; k < slotsPerSquare; k++)
            {
                in >> critters[i][j][k].age;
                if (critters[i][j][k].age > 0)
                {
                    in >> critters[i][j][k].genome;
                    in >> critters[i][j][k].fitness;
                    in >> critters[i][j][k].energy;
                    in >> critters[i][j][k].xPosition;
                    in >> critters[i][j][k].yPosition;
                    in >> critters[i][j][k].zPosition;
                }
            }

    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            in >> environment[i][j][0];
            in >> environment[i][j][1];
            in >> environment[i][j][2];
        }

    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            in >> totalFitness[i][j];
        }

    for (auto &xormask : xorMasks)
        for (int j = 0; j < 3; j++)
        {
            in >> xormask[j];
        }

    //Genome Comparison Loading
    QByteArray Temp;
    in >> Temp;
    genoneComparison->loadComparison(Temp);


    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            in >> breedAttempts[i][j];
            in >> breedFails[i][j];
            in >> settles[i][j];
            in >> settleFails[i][j];
            in >> maxUsed[i][j];
        }

    in >> Temp;
    restoreState(Temp); //window state

    bool btemp;
    in >> btemp;
    ui->actionGenomeComparison->setChecked(btemp);

    //interpolate environment stuff
    environmentInterpolate = true;
    in >> environmentInterpolate;

    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            in >> environmentLast[i][j][0];
            in >> environmentLast[i][j][1];
            in >> environmentLast[i][j][2];
        }

    for (int i = 0; i < gridX; i++)
        for (int j = 0; j < gridY; j++)
        {
            in >> environmentNext[i][j][0];
            in >> environmentNext[i][j][1];
            in >> environmentNext[i][j][2];
        }

    in >> speciesSamples;
    in >> speciesSensitivity;
    in >> timeSliceConnect;

    //now the species archive
    archivedSpeciesLists.clear();
    oldSpeciesList.clear();

    int temp;
    in >> temp; //oldSpeciesList.count();
    for (int j = 0; j < temp; j++)
    {
        Species s;
        in >> s.id;
        in >> s.type;
        in >> s.originTime;
        in >> s.parent;
        in >> s.size;
        in >> s.internalID;
        oldSpeciesList.append(s);
    }

    in >> temp; //archivedSpeciesLists.count();

    for (int i = 0; i < temp; i++)
    {
        int temp2;
        in >> temp2; //archivedSpeciesLists.count();
        QList<Species> ql;
        for (int j = 0; j < temp2; j++)
        {
            Species s;
            in >> s.id;
            in >> s.type;
            in >> s.originTime;
            in >> s.parent;
            in >> s.size;
            in >> s.internalID;
            ql.append(s);
        }
        archivedSpeciesLists.append(ql);
    }
    in >> nextSpeciesID;
    in >> lastSpeciesCalculated; //actually no - if we import this it will assume an 'a' object exists.
    //bodge
    lastSpeciesCalculated--;

    //now random array
    for (unsigned char &random : randoms)
        in >> random;

    infile.close();
    nextRefresh = 0;
    resizeImageObjects();
    report();
    resize();

    updateGUIFromVariables();
}

/*!
 * \brief MainWindow::genomeComparisonAdd
 * \return bool
 *
 * Adds a selected cell's genome to the Genome Comparison Dock
 */
bool MainWindow::genomeComparisonAdd()
{
    int x = populationScene->selectedX;
    int y = populationScene->selectedY;

    //---- Get genome colour
    if (totalFitness[x][y] != 0)
    {
        for (int c = 0; c < slotsPerSquare; c++)
        {
            if (critters[x][y][c].age > 0)
            {
                genoneComparison->addGenomeCritter(critters[x][y][c], environment[x][y]);
                return true;
            }
        }
    }
    return false;
}

/*!
 * \brief MainWindow::calculateSpecies
 */
void MainWindow::calculateSpecies()
{
    if (speciesMode == SPECIES_MODE_NONE)
        return; //do nothing!

    if (iteration != lastSpeciesCalculated)
    {
        delete analyser;  //replace old analyser object with new
        analyser = new Analyser;

        //New species analyser
        analyser->groupsGenealogicalTracker();

        //Makre sure this is updated
        lastSpeciesCalculated = iteration;
    }
}

/*!
 * \brief MainWindow::writeLog
 *
 * Writes main ongoing log
 */
void MainWindow::writeLog()
{
    //RJG - write main ongoing log
    if (logging)
    {
        speciesLoggingFile = globalSavePath->text();
        if (!speciesLoggingFile.endsWith(QDir::separator()))
            speciesLoggingFile.append(QDir::separator());
        speciesLoggingFile.append(QString(PRODUCTNAME) + "_log");
        if (batchRunning)
            speciesLoggingFile.append(QString("_run_%1").arg(batchRuns, 4, 10, QChar('0')));
        speciesLoggingFile.append(".txt");
        QFile outputfile(speciesLoggingFile);

        if (iteration == 0)
        {
            outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&outputfile);
            out << "New run ";
            QDateTime t(QDateTime::currentDateTime());
            out << t.toString(Qt::ISODate) << "\n\n";
            out << "===================\n\n";
            out << printSettings();
            out << "\n\n";
            out << "===================\n";
            out << "\nFor each iteration, this log features:\n\n";
            out << "- [I] Iteration Number\n";
            out << "- [P] Population Grid Data:\n";
            out << "-- Number of living digital organisms\n";
            out << "-- Mean fitness of living digital organisms\n";
            out << "-- Number of entries on the breed list\n";
            out << "-- Number of failed breed attempts\n";
            out << "-- Number of species\n";
            out << "- [S] Species Data:\n";
            out << "-- Species id\n";
            out << "-- Species origin (iterations)\n";
            out << "-- Species parent\n";
            out << "-- Species current size (number of individuals)\n";
            out << "-- Species current genome (for speed this is the genome of a randomly sampled individual, not the modal organism)\n\n";
            out << "**Note that this excludes species with less individuals than Minimum species size, but is not able to exlude species without descendants, which can only be achieved with the end-run log.**\n\n";
            out << "===================\n\n";
            outputfile.close();
        }

        outputfile.open(QIODevice::Append | QIODevice::Text);
        QTextStream out(&outputfile);

        out << "[I] " << iteration << "\n";

        int gridNumberAlive = 0, gridTotalFitness = 0, gridBreedEntries = 0, gridBreedFails = 0;
        for (int i = 0; i < gridX; i++)
            for (int j = 0; j < gridY; j++)
            {
                gridTotalFitness += totalFitness[i][j];
                //----RJG: Manually count breed stufffor grid
                gridBreedEntries += breedAttempts[i][j];
                gridBreedFails += breedFails[i][j];
                //----RJG: Manually count number alive thanks to maxUsed descendants
                for  (int k = 0; k < slotsPerSquare; k++)if (critters[i][j][k].fitness)gridNumberAlive++;
            }
        double meanFitness = double(gridTotalFitness) / double(gridNumberAlive);

        out << "[P] " << gridNumberAlive << "," << meanFitness << "," << gridBreedEntries << "," <<
            gridBreedFails << "," << oldSpeciesList.count() << "\n";

        //----RJG: And species details for each iteration
        for (int i = 0; i < oldSpeciesList.count(); i++)
        {
            //----RJG: Unable to exclude species without descendants, for obvious reasons.
            if (quint64(oldSpeciesList[i].size) > minSpeciesSize)
            {
                out << "[S] ";
                out << (oldSpeciesList[i].id) << ",";
                out << oldSpeciesList[i].originTime << ",";
                out << oldSpeciesList[i].parent << ",";
                out << oldSpeciesList[i].size << ",";
                //---- RJG - output binary genome if needed
                for (int j = 0; j < 63; j++)if (tweakers64[63 - j] & oldSpeciesList[i].type) out << "1";
                    else out << "0";
                if (tweakers64[0] & oldSpeciesList[i].type) out << "1";
                else out << "0";
                out << "\n";
            }
        }
        out << "\n";
        outputfile.close();
    }
}

/*!
 * \brief MainWindow::setStatusBarText
 * \param text
 *
 * Sets the status bar text.
 */
void MainWindow::setStatusBarText(const QString &text)
{
    ui->statusBar->showMessage(text);
}

void MainWindow::statusProgressBar(QProgressBar *prBar, bool add)
{
    if (add)
    {
        ui->statusBar->clearMessage();
        setStatusBarText("Calculating species...");
        ui->statusBar->addPermanentWidget(prBar);
    }
    else
    {
        ui->statusBar->removeWidget(prBar);
        ui->statusBar->clearMessage();
    }
}

/*!
 * \brief MainWindow::on_actionLoad_Random_Numbers_triggered
 *
 * Action to load custom random number files
 */
void MainWindow::on_actionLoad_Random_Numbers_triggered()
{
    //RJG - have added randoms to resources and into constructor, load on launch to ensure true randoms are loaded by default.
    //Select files
    QString file = QFileDialog::getOpenFileName(
                       this,
                       "Select random number file",
                       "",
                       "*.*"
                   );

    if (file.length() == 0) return;

    int seedoffset;
    seedoffset = QInputDialog::getInt(this, "Seed value", "Byte offset to start reading from (will read 65536 bytes)");

    //now read in values to array
    QFile rfile(file);
    rfile.open(QIODevice::ReadOnly);

    rfile.seek(seedoffset);

    qint64 i = rfile.read(reinterpret_cast<char *>(randoms), static_cast<qint64>(65536));
    if (i != 65536)
        QMessageBox::warning(this, "Oops", "Failed to read 65536 bytes from file - random numbers may be compromised - try again or restart program");
    else
        QMessageBox::information(this, "Success", "New random numbers read successfully");
}

/*!
 * \brief MainWindow::handleAnalysisTool
 * \param code
 * \return text
 *
 * Handle analyses at end of run for data.
 */
QString MainWindow::handleAnalysisTool(int code)
{
    //Tidied up a bit - MDS 14/9/2017
    //RJG - changed to return string 04/04/18 as analysis docker will be removed.

    //Is there a valid input file?
    AnalysisTools a;
    QString OutputString, FilenameString;

    switch (code)
    {
    case ANALYSIS_TOOL_CODE_COUNT_PEAKS:
    {
        bool ok;
        int red = QInputDialog::getInt(this, "Count peaks...", "Red level?", 128, 0, 255, 1, &ok);
        if (!ok)
            return QString("");
        int green = QInputDialog::getInt(this, "Count peaks...", "Green level?", 128, 0, 255, 1, &ok);
        if (!ok)
            return QString("");;
        int blue = QInputDialog::getInt(this, "Count peaks...", "Blue level?", 128, 0, 255, 1, &ok);
        if (!ok)
            return QString("");
        OutputString = a.countPeaks(red, green, blue);
        break;
    }

    case ANALYSIS_TOOL_CODE_MAKE_NEWICK:
        if (phylogenyButton->isChecked() || phylogenyAndMetricsButton->isChecked())
            OutputString = a.makeNewick(rootSpecies, minSpeciesSize, allowExcludeWithDescendants);
        else
            OutputString = "Species tracking is not enabled.";
        break;

    case ANALYSIS_TOOL_CODE_WRITE_DATA:
        if (phylogenyAndMetricsButton->isChecked())
            OutputString = a.writeData(rootSpecies, minSpeciesSize, allowExcludeWithDescendants);
        else
            OutputString = "Species tracking is not enabled, or is set to phylogeny only.";
        break;

    default:
        QMessageBox::warning(this, "Error", "No handler for analysis tool");
        return QString("Error, No handler for analysis tool");

    }

    return OutputString;
}

/*!
 * \brief MainWindow::printSettings
 * \return text
 */
QString MainWindow::printSettings()
{
    QString settings;
    QTextStream settingsOut(&settings);

    settingsOut << "REvoSim settings:\n\n";

    settingsOut << "- Integers:\n";
    settingsOut << "-- Grid X: " << gridX << "\n";
    settingsOut << "-- Grid Y: " << gridY << "\n";
    settingsOut << "-- Settle tolerance: " << settleTolerance << "\n";
    settingsOut << "-- Start age: " << startAge << "\n";
    settingsOut << "-- Disperal: " << dispersal << "\n";
    settingsOut << "-- Food: " << food << "\n";
    settingsOut << "-- Breed cost: " << breedCost << "\n";
    settingsOut << "-- Mutate: " << mutate << "\n";
    settingsOut << "-- Max diff to breed: " << maxDifference << "\n";
    settingsOut << "-- Breed threshold: " << breedThreshold << "\n";
    settingsOut << "-- Slots per square: " << slotsPerSquare << "\n";
    settingsOut << "-- Fitness target: " << target << "\n";
    settingsOut << "-- Environmental change rate: " << environmentChangeRate << "\n";
    settingsOut << "-- Minimum species size:" << minSpeciesSize << "\n";
    settingsOut << "-- Environment mode:" << environmentMode << "\n";
    settingsOut << "-- Speices mode:" << speciesMode << "\n";

    settingsOut << "\n- Bools:\n";
    settingsOut << "-- Recalculate fitness: " << recalculateFitness << "\n";
    settingsOut << "-- Toroidal environment: " << toroidal << "\n";
    settingsOut << "-- Interpolate environment: " << environmentInterpolate << "\n";
    settingsOut << "-- Nonspatial setling: " << nonspatial << "\n";
    settingsOut << "-- Enforce max diff to breed:" << breedDifference << "\n";
    settingsOut << "-- Only breed within species:" << breedSpecies << "\n";
    settingsOut << "-- Exclude species without descendants:" << allowExcludeWithDescendants << "\n";
    settingsOut << "-- Breeding: ";
    if (sexual)
        settingsOut << "sexual" << "\n";
    else if (asexual)
        settingsOut << "asexual" << "\n";
    else
        settingsOut << "variable" << "\n";

    return settings;
}

/*!
 * \brief MainWindow::loadSettings
 *
 * Save and load settings (but not critter info, masks etc.).
 */
void MainWindow::loadSettings()
{
    QString settings_filename = QFileDialog::getOpenFileName(this, tr("Open File"), globalSavePath->text(), "XML files (*.xml)");
    if (settings_filename.length() < 3)
        return;
    QFile settings_file(settings_filename);
    if (!settings_file.open(QIODevice::ReadOnly))
    {
        setStatusBarText("Error opening file.");
        return;
    }

    QXmlStreamReader settingsFileIn(&settings_file);

    while (!settingsFileIn.atEnd() && !settingsFileIn.hasError())
    {

        /* Read next element.*/
        QXmlStreamReader::TokenType token = settingsFileIn.readNext();
        /* If token is just StartDocument, we'll go to next.*/

        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement)
        {
            //Ints
            if (settingsFileIn.name() == "revosim")
                continue;
            if (settingsFileIn.name() == "gridX")
                gridX = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "gridY")
                gridY = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "settleTolerance")
                settleTolerance = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "slotsPerSquare")
                slotsPerSquare = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "startAge")
                startAge = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "dispersal")
                dispersal = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "food")
                food = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "breedCost")
                breedCost = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "mutate")
                mutate = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "maxDifference")
                maxDifference = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "breedThreshold")
                breedThreshold = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "target")
                target = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "environmentChangeRate")
                environmentChangeRate = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "refreshRate")
                refreshRate = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "environmentMode")
                environmentModeChanged(settingsFileIn.readElementText().toInt(), true);
            //No Gui options for the remaining settings as yet.
            if (settingsFileIn.name() == "speciesSamples")
                speciesSamples = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "speciesSensitivity")
                speciesSensitivity = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "timeSliceConnect")
                timeSliceConnect = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "minSpeciesSize")
                minSpeciesSize = static_cast<quint64>(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "speciesMode")
                minSpeciesSize = static_cast<quint64>(settingsFileIn.readElementText().toInt());

            //Bools
            if (settingsFileIn.name() == "recalculateFitness")
                recalculateFitness = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "toroidal")
                toroidal = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "nonspatial")
                nonspatial = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "breedDifference")
                breedDifference = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "breedSpecies")
                breedSpecies = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "allowExcludeWithDescendants")
                allowExcludeWithDescendants = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "sexual")
                sexual = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "asexual")
                asexual = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "logging")
                logging = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "gui")
                gui = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "environmentInterpolate")
                environmentInterpolate = settingsFileIn.readElementText().toInt();
            //No gui options for below
            if (settingsFileIn.name() == "fitnessLoggingToFile")
                fitnessLoggingToFile = settingsFileIn.readElementText().toInt();
            //Only GUI options
            if (settingsFileIn.name() == "autowrite")
                autowriteLogCheckbox->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "savePopulationCount")
                savePopulationCount->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "saveMeanFitness")
                saveMeanFitness->setChecked( settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "saveCodingGenomeAsColour")
                saveCodingGenomeAsColour->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "saveSpecies")
                saveSpecies->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "saveNonCodingGenomeAsColour")
                saveNonCodingGenomeAsColour->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "saveGeneFrequencies")
                saveGeneFrequencies->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "saveSettles")
                saveSettles->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "saveFailsSettles")
                saveFailsSettles->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "saveEnvironment")
                saveEnvironment->setChecked(settingsFileIn.readElementText().toInt());

            //Strings
            if (settingsFileIn.name() == "globalSavePath")
                globalSavePath->setText(settingsFileIn.readElementText());
        }
    }
    // Error
    if (settingsFileIn.hasError())
        setStatusBarText("There seems to have been an error reading in the XML file. Not all settings will have been loaded.");
    else
        setStatusBarText("Loaded settings file");

    settings_file.close();

    updateGUIFromVariables();
}

/*!
 * \brief MainWindow::updateGUIFromVariables
 *
 * Call this from either load settings, or plain load. Updates gui from those variables held as simulation globals
 */
void MainWindow::updateGUIFromVariables()
{
    //Ints
    gridXSpin->setValue(gridX);
    gridYSpin->setValue(gridY);
    settleToleranceSpin->setValue(settleTolerance);
    slotsSpin->setValue(slotsPerSquare);
    startAgeSpin->setValue(startAge);
    dispersalSpin->setValue(dispersal);
    energySpin->setValue(food);
    breedCostSpin->setValue(breedCost);
    mutateSpin->setValue(mutate);
    maxDifferenceSpin->setValue(maxDifference);
    breedThresholdSpin->setValue(breedThreshold);
    targetSpin->setValue(target);
    environmentRateSpin->setValue(environmentChangeRate);
    refreshRateSpin->setValue(refreshRate);
    // Add speciesMode
    speciesModeChanged(speciesMode, true);
    // Add environmentMode
    environmentModeChanged(environmentMode, true);

    //Bools
    recalculateFitnessCheckbox->setChecked(recalculateFitness);
    toroidalCheckbox->setChecked(toroidal);
    nonspatialCheckbox->setChecked(nonspatial);
    breedDifferenceCheckbox->setChecked(breedDifference);
    breedSpeciesCheckbox->setChecked(breedSpecies);
    excludeWithoutDescendantsCheckbox->setChecked(allowExcludeWithDescendants);
    sexualRadio->setChecked(sexual);
    asexualRadio->setChecked(asexual);
    loggingCheckbox->setChecked(logging);
    guiCheckbox->setChecked(gui);
    interpolateCheckbox->setChecked(environmentInterpolate);
}

/*!
 * \brief MainWindow::saveSettings
 */
void MainWindow::saveSettings()
{
    QString settings_filename = QFileDialog::getSaveFileName(
                                    this,
                                    tr("Save file as..."),
                                    QString(globalSavePath->text() + QString(PRODUCTNAME) + "Settings.xml")
                                );
    if (!settings_filename.endsWith(".xml"))
        settings_filename.append(".xml");
    QFile settings_file(settings_filename);
    if (!settings_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        setStatusBarText("Error opening settings file to write to.");
        return;
    }

    QXmlStreamWriter settingsFileOut(&settings_file);
    settingsFileOut.setAutoFormatting(true);
    settingsFileOut.setAutoFormattingIndent(-2);

    settingsFileOut.writeStartDocument();

    settingsFileOut.writeStartElement("revosim");

    //Ints
    settingsFileOut.writeStartElement("gridX");
    settingsFileOut.writeCharacters(QString("%1").arg(gridX));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("gridY");
    settingsFileOut.writeCharacters(QString("%1").arg(gridY));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("settleTolerance");
    settingsFileOut.writeCharacters(QString("%1").arg(settleTolerance));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("slotsPerSquare");
    settingsFileOut.writeCharacters(QString("%1").arg(slotsPerSquare));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("startAge");
    settingsFileOut.writeCharacters(QString("%1").arg(startAge));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("dispersal");
    settingsFileOut.writeCharacters(QString("%1").arg(dispersal));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("food");
    settingsFileOut.writeCharacters(QString("%1").arg(food));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("breedCost");
    settingsFileOut.writeCharacters(QString("%1").arg(breedCost));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("mutate");
    settingsFileOut.writeCharacters(QString("%1").arg(mutate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("maxDifference");
    settingsFileOut.writeCharacters(QString("%1").arg(maxDifference));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("breedThreshold");
    settingsFileOut.writeCharacters(QString("%1").arg(breedThreshold));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("target");
    settingsFileOut.writeCharacters(QString("%1").arg(target));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("environmentChangeRate");
    settingsFileOut.writeCharacters(QString("%1").arg(environmentChangeRate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("environmentMode");
    settingsFileOut.writeCharacters(QString("%1").arg(environmentMode));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("refreshRate");
    settingsFileOut.writeCharacters(QString("%1").arg(refreshRate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("speciesSamples");
    settingsFileOut.writeCharacters(QString("%1").arg(speciesSamples));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("speciesSensitivity");
    settingsFileOut.writeCharacters(QString("%1").arg(speciesSensitivity));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("timeSliceConnect");
    settingsFileOut.writeCharacters(QString("%1").arg(timeSliceConnect));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("minSpeciesSize");
    settingsFileOut.writeCharacters(QString("%1").arg(minSpeciesSize));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("speciesMode");
    settingsFileOut.writeCharacters(QString("%1").arg(speciesMode));
    settingsFileOut.writeEndElement();

    //Bools
    settingsFileOut.writeStartElement("recalculateFitness");
    settingsFileOut.writeCharacters(QString("%1").arg(recalculateFitness));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("toroidal");
    settingsFileOut.writeCharacters(QString("%1").arg(toroidal));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("nonspatial");
    settingsFileOut.writeCharacters(QString("%1").arg(nonspatial));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("breedDifference");
    settingsFileOut.writeCharacters(QString("%1").arg(breedDifference));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("breedSpecies");
    settingsFileOut.writeCharacters(QString("%1").arg(breedSpecies));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("allowExcludeWithDescendants");
    settingsFileOut.writeCharacters(QString("%1").arg(allowExcludeWithDescendants));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("sexual");
    settingsFileOut.writeCharacters(QString("%1").arg(sexual));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("asexual");
    settingsFileOut.writeCharacters(QString("%1").arg(asexual));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("logging");
    settingsFileOut.writeCharacters(QString("%1").arg(logging));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("gui");
    settingsFileOut.writeCharacters(QString("%1").arg(gui));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("environmentInterpolate");
    settingsFileOut.writeCharacters(QString("%1").arg(environmentInterpolate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("fitnessLoggingToFile");
    settingsFileOut.writeCharacters(QString("%1").arg(fitnessLoggingToFile));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("autowrite");
    settingsFileOut.writeCharacters(QString("%1").arg(autowriteLogCheckbox->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("savePopulationCount");
    settingsFileOut.writeCharacters(QString("%1").arg(savePopulationCount->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveMeanFitness");
    settingsFileOut.writeCharacters(QString("%1").arg(saveMeanFitness->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveCodingGenomeAsColour");
    settingsFileOut.writeCharacters(QString("%1").arg(saveCodingGenomeAsColour->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveSpecies");
    settingsFileOut.writeCharacters(QString("%1").arg(saveSpecies->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveNonCodingGenomeAsColour");
    settingsFileOut.writeCharacters(QString("%1").arg(saveNonCodingGenomeAsColour->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveGeneFrequencies");
    settingsFileOut.writeCharacters(QString("%1").arg(saveGeneFrequencies->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveSettles");
    settingsFileOut.writeCharacters(QString("%1").arg(saveSettles->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveFailsSettles");
    settingsFileOut.writeCharacters(QString("%1").arg(saveFailsSettles->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveEnvironment");
    settingsFileOut.writeCharacters(QString("%1").arg(saveEnvironment->isChecked()));
    settingsFileOut.writeEndElement();

    //Strings
    settingsFileOut.writeStartElement("globalSavePath");
    settingsFileOut.writeCharacters(globalSavePath->text());
    settingsFileOut.writeEndElement();

    settingsFileOut.writeEndElement();

    settingsFileOut.writeEndDocument();

    settings_file.close();

    setStatusBarText("File saved");
}

/*!
 * \brief MainWindow::exit
 *
 * Action to exit the program.
 */
void MainWindow::exitProgram()
{
    if (pauseFlag)
        pauseSimulation();
    stopSimulation();
    QApplication::quit();
}

/*!
 * \brief MainWindow::on_actionCode_on_GitHub_triggered
 *
 * Trigger to open url to gihub repository.
 */
void MainWindow::on_actionCode_on_GitHub_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(GITURL) + QString(GITREPOSITORY)));
}

/*!
 * \brief MainWindow::on_actionOnline_User_Manual_triggered
 *
 * Trigger to open url to online documentation
 */
void MainWindow::on_actionOnline_User_Manual_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(READTHEDOCS)));
}

/*!
 * \brief MainWindow::on_actionSettings_Dock_triggered
 */
void MainWindow::on_actionSettings_Dock_triggered()
{
    updateSettingsDockVisability();
}

/*!
 * \brief MainWindow::updateGenomeComparisonDockVisability
 * \param checked
 *
 * Action to open the Genome Comparison Dock.
 */
void MainWindow::updateGenomeComparisonDockVisability(bool checked)
{
    ui->genomeComparisonDock->setVisible(checked);
    ui->actionGenomeComparison->setChecked(checked);
    genomeComparisonButton->setChecked(checked);
}

/**
 * @brief MainWindow::processAppEvents
 */
void MainWindow::processAppEvents()
{
    qApp->processEvents();
}

/**
 * @brief MainWindow::on_actionBugIssueFeatureRequest_triggered
 */
void MainWindow::on_actionBugIssueFeatureRequest_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(GITURL) + QString(GITREPOSITORY) + QString(GITISSUE)));
}
