/*!
 * @file
 *
 * Main Window
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

#include "analyser.h"
#include "analysistools.h"
#include "mainwindow.h"
#include "reseed.h"
#include "resizecatcher.h"
#include "ui_mainwindow.h"
#include "globals.h"
#include "test.h"
#include "imagesequence.h"
#include "custommemorymanager.h"

#include <QActionGroup>
#include <QComboBox>
#include <QDataStream>
#include <QDebug>
#include <QDesktopServices>
#include <QDialogButtonBox>
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
#include <QThread>
#include <QTimer>
#include <QXmlStreamReader>
#include <QButtonGroup>
#include <QProgressDialog>

#include <cmath>
#include <math.h>

//not defined in all versions
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.7071067811865475
#endif

MainWindow *mainWindow;
SimManager *simulationManager;
CustomMemoryManager *customMemoryManager;

/*!
 * \brief QLabel class to allow changes in colour attached to timer (used in genome words GUI)
 */
class SysLabel: public QLabel
{
public:
    using QLabel::QLabel;
    void changeColour(const QColor &colour)
    {
        QPalette palette = this->palette();
        palette.setColor(QPalette::WindowText, colour);
        setPalette(palette);
    }
    void changeColourTimer(const QColor &colour, int timeout = 1000)
    {
        QColor defaultColour = Qt::white;
        changeColour(colour);
        QTimer::singleShot(timeout, this, [this, defaultColour]()
        {
            changeColour(defaultColour);
        });
    }
};


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
    autoFromCommand = false;

    simulationManager = new SimManager;
    customMemoryManager = new CustomMemoryManager;
    //RJG - load default environment image to allow program to run out of box (quicker for testing)
    //RJG - need to do this here, not from SimManager constructor as functions to load image require simManager exists
    QStringList f(":/REvoSim_default_env.png");
    simulationManager->env = new ImageSequence(f, simulationManager->simulationSettings->environmentChangeRate);
    //RJG - now we can set up run - this is dependent on having an environment, so this needs to be called in the current order
    simulationManager->setupRun();

    //Output version, but also date compiled for clarity
    setWindowTitle(QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) + " - compiled - " + __DATE__);
    setWindowIcon(QIcon (":/icon.png"));

    //Install filter to catch resize events to central widget and deliver to mainwindow (handle dock resizes)
    auto *resizeCatcher = new ResizeCatcher(this);
    ui->centralWidget->installEventFilter(resizeCatcher);

    //RJG - Set up scroll area height based on text size
    int textHeight = ui->LabelBatch->height();
    ui->scrollArea->setMaximumHeight(textHeight + 30);

    // Create Main Menu
    createMainMenu();

    // Create Main Toolbar
    createMainToolbar();

    //RJG - set up settings docker.
    simulationSettingsDock = createSimulationSettingsDock();
    //RJG - second settings docker.
    organismSettingsDock = createOrganismSettingsDock();
    //RJG - Third settings docker
    outputSettingsDock = createOutputSettingsDock();
    //ENF - Interactions settings docker
    interactionSettingsDock = createInteractionSettingsDock();
    //RJG - Logging settings docker
    logSettingsDock = createLogSettingsDock();

    //RJG - Make docks tabbed
    tabifyDockWidget(organismSettingsDock, simulationSettingsDock);
    tabifyDockWidget(simulationSettingsDock, interactionSettingsDock);
    tabifyDockWidget(outputSettingsDock, logSettingsDock);

    //Hide docks by default
    organismSettingsDock->hide();
    simulationSettingsDock->hide();
    outputSettingsDock->hide();
    interactionSettingsDock->hide();
    logSettingsDock->hide();

    //Add Genome Comparison UI Dock
    ui->genomeComparisonDock->hide();
    genoneComparison = new GenomeComparison;
    auto *genomeLayout = new QVBoxLayout;
    genomeLayout->addWidget(genoneComparison);
    ui->genomeComparisonContent->setLayout(genomeLayout);

    //RJG - Hide test log until needed
    ui->testLog->document()->setDefaultStyleSheet("h1 {text-align: center;} h2 {text-align: left;} h3 {text-align: center;} p {text-align: left; font-size: small;} p.error {font color=#FF00FF;}");
    ui->testLog->setHtml("<h1>REvoSim test log </h1><p>Below you can find the output of the REvoSim tests. The long pieces of text - e.g. masks and playing fields - are output as MD5 checksums for space and clarity. If the text is the same, the checksum will be too. Any tests that fail will appear in bright green font with a test failed message at the front.</p>");
    ui->testLog->setVisible(false);

    //RJG - Set batch variables - do this here as visualisation checks if batch is running
    batchRunning = false;
    batchRuns = 0;
    batchIterations = 0;
    batchTargetRuns = 0;

    //Create scenes, add to the GVs
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

    populationImage = new QImage(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY, QImage::Format_Indexed8);
    QVector <QRgb> clut(256);
    for (int ic = 0; ic < 256; ic++) clut[ic] = qRgb(ic, ic, ic);
    populationImage->setColorTable(clut);
    populationImage->fill(0);

    environmentImage = new QImage(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY, QImage::Format_RGB32);
    environmentImage->fill(0);

    populationImageColour = new QImage(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY, QImage::Format_RGB32);
    environmentImage->fill(0);

    environmentItem->setPixmap(QPixmap::fromImage(*environmentImage));
    populationItem->setPixmap(QPixmap::fromImage(*populationImage));

    //ARTS - Population Window dropdown must be after settings dock setup
    // 0 = Population count
    // 1 = Mean fitness
    // 2 = Visualisation system 1
    // 3 = Visualisation system 2
    // 4 = Settles
    // 5 = Breed/Settle Fails (R=Breed, G=Settle)
    // 6 unused
    // 7 = Species
    // 8 = Interactions
    // 9 = Breed List
    // 10 = Pathogens - coding
    // 11 Pathogens - non coding
    // 12 Stolen energy

    ui->populationWindowComboBox->addItem("Population count", QVariant(0));
    ui->populationWindowComboBox->addItem("Mean fitness", QVariant(1));
    ui->populationWindowComboBox->addItem("Visualisation system 1", QVariant(2));
    ui->populationWindowComboBox->addItem("Visualisation system 2", QVariant(3));
    //ui->populationWindowComboBox->addItem("Gene Frequencies", QVariant(4));
    //ui->populationWindowComboBox->addItem("Breed Attempts",QVariant(5));
    //ui->populationWindowComboBox->addItem("Breed Fails",QVariant(6));
    ui->populationWindowComboBox->addItem("Settles", QVariant(4));
    ui->populationWindowComboBox->addItem("Breed/Settle Fails (R=Breed, G=Settle)", QVariant(5));
    //ui->populationWindowComboBox->addItem("Breed Fails 2",QVariant(9));
    ui->populationWindowComboBox->addItem("Species", QVariant(7));
    ui->populationWindowComboBox->addItem("Interactions", QVariant(8));
    ui->populationWindowComboBox->addItem("Breed List", QVariant(9));
    ui->populationWindowComboBox->addItem("Pathogens - word 1", QVariant(10));
    ui->populationWindowComboBox->addItem("Pathogens - word 2", QVariant(11));
    ui->populationWindowComboBox->addItem("Stolen energy", QVariant(12));

    //ARTS -Population Window dropdown set current index. Note this value is the index not the data value.
    ui->populationWindowComboBox->setCurrentIndex(2);

    //RJG - Environment window dropdown - only one option here, others are created alongside the linkages
    ui->environmentWindowComboBox->addItem("Environment", QVariant(0));
    ui->environmentWindowComboBox->setCurrentIndex(0);

    pauseFlag = false;
    stopFlag = false;

    finishRun();//sets up enabling
    nextRefresh = 0;
    lastReport = 0;
    report();

    showMaximized();

    //RJG - add shortcut for tests
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T), this, SLOT(doTests()));

}

/*!
 * \brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete simulationManager;
    delete mainWindow;
}

/*!
 * \brief MainWindow::createMainMenu
 */
void MainWindow::createMainMenu()
{
    QObject::connect(ui->actionSave_settings, SIGNAL (triggered()), this, SLOT (saveSettings()));
    QObject::connect(ui->actionLoad_settings, SIGNAL (triggered()), this, SLOT (loadSettings()));
    QObject::connect(ui->actionCount_peaks, SIGNAL(triggered()), this, SLOT(writePeakCounts()));
    QObject::connect(ui->actionStart_Sim, SIGNAL(triggered()), this, SLOT(startSimulation()));
    QObject::connect(ui->actionRun_for, SIGNAL(triggered()), this, SLOT(runForNSimulation()));
    QObject::connect(ui->actionBatch, SIGNAL(triggered()), this, SLOT(startBatchSimulation()));
    QObject::connect(ui->actionPause_Sim, SIGNAL(triggered()), this, SLOT(pauseSimulation()));
    QObject::connect(ui->actionStop_Sim, SIGNAL(triggered()), this, SLOT(stopSimulation()));
    QObject::connect(ui->actionReset, SIGNAL (triggered()), this, SLOT (resetSimulation()));
    QObject::connect(ui->actionReseed, SIGNAL (triggered()), this, SLOT (launchReseedDialog()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveSimulation()));
    QObject::connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(loadSimulation()));
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
    logButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_log_button.png")), QString("Logging"), this);
    testButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_test_button.png")), QString("Tests"), this);
    aboutButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_about_button.png")), QString("About"), this);

    //ARTS - Toolbar default settings & docker toggles defaults
    startButton->setEnabled(false);
    runForButton->setEnabled(false);
    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);
    reseedButton->setEnabled(false);
    runForBatchButton->setEnabled(false);
    genomeComparisonButton->setCheckable(true);
    logButton->setCheckable(true);
    testButton->setCheckable(true);
    settingsButton->setCheckable(true);

    //Tootip Help
    startButton->setToolTip(tr("<font>Use this button to start a basic manual simulation run.</font>"));
    runForButton->setToolTip(tr("<font>Use this button to start a simulation that will stop after '<i>n</i>' iterations.</font>"));
    runForBatchButton->setToolTip(tr("<font>Use this button to start a batched simulation that will stop after '<i>x</i>' runs of '<i>n</i>' iterations.</font>"));
    pauseButton->setToolTip(tr("<font>Use this button to pause any simulation run or batched run. Press again to unpause.</font>"));
    stopButton->setToolTip(tr("<font>Use this button stop any simulation or batched run.</font>"));
    resetButton->setToolTip(tr("<font>Use this button reset the simulation.</font>"));
    reseedButton->setToolTip(tr("<font>Use this button reseed the simulation with a custom starting genome.</font>"));
    genomeComparisonButton->setToolTip(tr("<font>Use this button to open the Genome Comparison Dock.</font>"));
    logButton->setToolTip(tr("<font>This launches a dialogue to allow you to modify the REvoSim running log.</font>"));
    testButton->setToolTip(tr("<font>Use this button to open and then run the REvoSim test suite.</font>"));
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
    ui->toolBar->addAction(logButton);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(testButton);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(aboutButton);

    //RJG - Connect button signals to slot.
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
    QObject::connect(genomeComparisonButton, SIGNAL(triggered(bool)), this, SLOT(updateGenomeComparisonDockVisibility(bool)));
    QObject::connect(settingsButton, SIGNAL(triggered()), this, SLOT(updateSettingsDockVisibility()));
    QObject::connect(logButton, SIGNAL(triggered()), this, SLOT(updateOutputDockVisibility()));
    QObject::connect(testButton, SIGNAL(triggered()), this, SLOT(doTests()));
    QObject::connect(aboutButton, SIGNAL (triggered()), this, SLOT (on_actionAbout_triggered()));

    connect(ui->actionSpeciation_logging, &QAction::toggled, mainWindow, [ = ](const bool & i)
    {
        if (simulationManager->iteration == 0 && i) speciesModeChanged(SPECIES_MODE_PHYLOGENY_AND_METRICS, true);
        if (simulationManager->iteration != 0 && i && speciesMode != SPECIES_MODE_PHYLOGENY_AND_METRICS)
        {
            ui->actionSpeciation_logging->setChecked(false);
            QMessageBox::information(nullptr, tr("Error"), tr("Speciation logging requires species mode to be phylogeny and metrics, and this can't be changed during a run. Sorry."));
        }
    });
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

    QLabel *environment_label = new QLabel("Environmental Settings");
    environment_label->setStyleSheet("font-weight: bold");
    environmentSettingsGrid->addWidget(environment_label, 0, 1, 1, 2);

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
    environmentRateSpin->setValue(simulationManager->simulationSettings->environmentChangeRate);
    environmentSettingsGrid->addWidget(environmentRateLabel, 2, 1);
    environmentSettingsGrid->addWidget(environmentRateSpin, 2, 2);
    //RJG - Note in order to use a lamda not only do you need to use C++11, but there are two valueChanged signals for spinbox - and int and a string. Need to cast it to an int
    connect(environmentRateSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i )
    {
        simulationManager->simulationSettings->environmentChangeRate = i;
        simulationManager->env->modifyChangeRate(i);
    });

    QLabel *environment_mode_label = new QLabel("Environment mode:");
    environmentSettingsGrid->addWidget(environment_mode_label, 3, 1, 1, 2);

    auto environmentModeGrid = new QGridLayout;
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
    environmentModeGrid->addWidget(environmentModeStaticButton, 1, 1, 1, 1);
    environmentModeGrid->addWidget(environmentModeOnceButton, 1, 2, 1, 1);
    environmentModeGrid->addWidget(environmentModeLoopButton, 2, 1, 1, 1);
    environmentModeGrid->addWidget(environmentModeBounceButton, 2, 2, 1, 1);
    connect(environmentModeButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), mainWindow, [ = ](const int &i)
    {
        environmentModeChanged(i, false);
    });
    environmentSettingsGrid->addLayout(environmentModeGrid, 4, 1, 1, 2);

    interpolateCheckbox = new QCheckBox("Interpolate between images");
    interpolateCheckbox->setChecked(simulationManager->simulationSettings->environmentInterpolate);
    interpolateCheckbox->setToolTip("<font>Turning this ON will interpolate the environment between individual images.</font>");
    environmentSettingsGrid->addWidget(interpolateCheckbox, 5, 1, 1, 2);
    connect(interpolateCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->environmentInterpolate = i;
    });

    toroidalCheckbox = new QCheckBox("Toroidal environment");
    toroidalCheckbox->setChecked(simulationManager->simulationSettings->toroidal);
    toroidalCheckbox->setToolTip("<font>Turning this ON will allow dispersal of progeny in an unbounded warparound environment. Progeny leaving one side of the population window will immediately reappear on the opposite side.</font>");
    environmentSettingsGrid->addWidget(toroidalCheckbox, 6, 1, 1, 2);
    connect(toroidalCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->toroidal = i;
    });

    // Simulation Size Settings
    auto *simulationSizeSettingsGrid = new QGridLayout;

    QLabel *simulation_size_label = new QLabel("Simulation size");
    simulation_size_label->setStyleSheet("font-weight: bold");
    simulationSizeSettingsGrid->addWidget(simulation_size_label, 0, 1, 1, 2);

    QLabel *gridX_label = new QLabel("Grid Size:");
    gridX_label->setToolTip("<font>Number of grid cells on each axis.</font>");
    gridXSpin = new QSpinBox;
    gridXSpin->setMinimum(1);
    gridXSpin->setMaximum(256);
    gridXSpin->setValue(simulationManager->simulationSettings->gridX);
    gridXSpin->setToolTip("<font>Number of grid cells on each axis.</font>");
    simulationSizeSettingsGrid->addWidget(gridX_label, 2, 1);
    simulationSizeSettingsGrid->addWidget(gridXSpin, 2, 2);
    connect(gridXSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), mainWindow, [ = ](const int &i)
    {
        int oldRows = simulationManager->simulationSettings->gridX;
        simulationManager->simulationSettings->gridX = i;
        simulationManager->simulationSettings->gridY = i;
        redoImages(oldRows, simulationManager->simulationSettings->gridY);
    });

    QLabel *slots_label = new QLabel("Slots:");
    slots_label->setToolTip("<font>Number of slots per grid cell.</font>");
    slotsSpin = new QSpinBox;
    slotsSpin->setMinimum(1);
    slotsSpin->setMaximum(256);
    slotsSpin->setValue(simulationManager->cellSettingsMaster->slotsPerSquare);
    slotsSpin->setToolTip("<font>Number of slots per grid cell.</font>");
    simulationSizeSettingsGrid->addWidget(slots_label, 4, 1);
    simulationSizeSettingsGrid->addWidget(slotsSpin, 4, 2);
    connect(slotsSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), mainWindow, [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->slotsPerSquare = i;
        redoImages(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY);
    });

    // Simulation Settings
    auto *simulationSettingsGrid = new QGridLayout;

    QLabel *simulation_settings_label = new QLabel("Simulation settings");
    simulation_settings_label->setStyleSheet("font-weight: bold");
    simulationSettingsGrid->addWidget(simulation_settings_label, 0, 1, 1, 2);



    QLabel *target_label = new QLabel("Fitness target:");
    target_label->setToolTip("<font>Target value effects the fitness landscape. See manual for more details.</font>");
    targetSpin = new QSpinBox;
    targetSpin->setMinimum(1);
    targetSpin->setMaximum(96 * MAX_GENOME_WORDS);
    targetSpin->setValue(simulationManager->cellSettingsMaster->target);
    targetSpin->setToolTip("<font>Target value effects the fitness landscape. See manual for more details.</font>");
    simulationSettingsGrid->addWidget(target_label, 1, 1);
    simulationSettingsGrid->addWidget(targetSpin, 1, 2);
    connect(targetSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), mainWindow,  [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->target = i;
    });

    QLabel *energy_label = new QLabel("Energy input:");
    energy_label->setToolTip("<font>Energy level given to new organisms.</font>");
    energySpin = new QSpinBox;
    energySpin->setMinimum(1);
    energySpin->setMaximum(20000);
    energySpin->setValue(simulationManager->cellSettingsMaster->food);
    energySpin->setToolTip("<font>Energy level given to new organisms.</font>");
    simulationSettingsGrid->addWidget(energy_label, 2, 1);
    simulationSettingsGrid->addWidget(energySpin, 2, 2);
    connect(energySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->food = i;
    });

    QLabel *settleTolerance_label = new QLabel("Settle tolerance:");
    settleTolerance_label->setToolTip("<font>Defines the range of environments an organism can settle into.</font>");
    settleToleranceSpin = new QSpinBox;
    settleToleranceSpin->setMinimum(1);
    settleToleranceSpin->setMaximum(30 * MAX_GENOME_WORDS);
    settleToleranceSpin->setValue(simulationManager->cellSettingsMaster->settleTolerance);
    settleToleranceSpin->setToolTip("<font>Defines the range of environments an organism can settle into.</font>");
    simulationSettingsGrid->addWidget(settleTolerance_label, 3, 1);
    simulationSettingsGrid->addWidget(settleToleranceSpin, 3, 2);
    connect(settleToleranceSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->settleTolerance = i;
    });

    recalculateFitnessCheckbox = new QCheckBox("Recalculate fitness");
    recalculateFitnessCheckbox->setChecked(simulationManager->simulationSettings->recalculateFitness);
    recalculateFitnessCheckbox->setToolTip("<font>Turning on recalculates the fitness of each organism every iteration.</font>");
    simulationSettingsGrid->addWidget(recalculateFitnessCheckbox, 4, 1, 1, 1);
    connect(recalculateFitnessCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->recalculateFitness = i;
    });

    noSelectionFitnessCheckbox = new QCheckBox("No selection");
    recalculateFitnessCheckbox->setChecked(simulationManager->cellSettingsMaster->noSelection);
    noSelectionFitnessCheckbox->setToolTip("<font>Turning on distributes food evenly irrespective of fitness.</font>");
    simulationSettingsGrid->addWidget(noSelectionFitnessCheckbox, 4, 2, 1, 1);
    connect(noSelectionFitnessCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->cellSettingsMaster->noSelection = i;
    });

    //Phylogeny Settings
    auto *phylogenySettingsGrid = new QGridLayout;

    QLabel *phylogeny_settings_label = new QLabel("Phylogeny settings");
    phylogeny_settings_label->setStyleSheet("font-weight: bold");
    phylogenySettingsGrid->addWidget(phylogeny_settings_label, 0, 1, 1, 1);

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
    phylogenyGrid->addWidget(phylogenyOffButton, 1, 1, 1, 1);
    phylogenyGrid->addWidget(basicPhylogenyButton, 1, 2, 1, 1);
    phylogenyGrid->addWidget(phylogenyButton, 2, 1, 1, 1);
    phylogenyGrid->addWidget(phylogenyAndMetricsButton, 2, 2, 1, 1);
    connect(phylogenyButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), mainWindow, [ = ](const int &i)
    {
        speciesModeChanged(i, false);
    });
    phylogenySettingsGrid->addLayout(phylogenyGrid, 1, 1, 1, 2);

    //Linkages Settings
    auto *linkagesSettingsGrid = new QGridLayout;

    QLabel *linkages_settings_label = new QLabel("Linkages");
    linkages_settings_label->setStyleSheet("font-weight: bold");
    linkagesSettingsGrid->addWidget(linkages_settings_label, 0, 1, 1, 1);

    QPushButton *addLinkagesButton = new QPushButton("&Link a variable to an image mask");
    addLinkagesButton->setObjectName("addLinkagesButton");
    addLinkagesButton->setToolTip("<font>REvoSim allows you to link settings to the values of an 8 bit input image.</font>");
    linkagesSettingsGrid->addWidget(addLinkagesButton, 1, 1, 1, 1);
    connect(addLinkagesButton, SIGNAL (clicked()), this, SLOT(addLinkage()));

    QLabel *linkagesListLabel = new QLabel("Current linkages - double click to edit.");
    linkagesSettingsGrid->addWidget(linkagesListLabel, 2, 1, 1, 1);
    linkagesList = new QListWidget;
    linkagesList->setWordWrap(true);
    QObject::connect(linkagesList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(linkagesListClicked()));
    linkagesSettingsGrid->addWidget(linkagesList, 3, 1, 1, 1);

    //ARTS - Dock Grid Layout
    settingsGrid->addLayout(environmentSettingsGrid, 0, 1);
    settingsGrid->addLayout(simulationSizeSettingsGrid, 1, 1);
    settingsGrid->addLayout(simulationSettingsGrid, 2, 1);
    settingsGrid->addLayout(phylogenySettingsGrid, 3, 1);
    settingsGrid->addLayout(linkagesSettingsGrid, 4, 1);

    QWidget *settingsLayoutWidget = new QWidget;
    settingsLayoutWidget->setLayout(settingsGrid);
    settingsLayoutWidget->setMinimumWidth(300);
    simulationSettingsDock->setWidget(settingsLayoutWidget);
    simulationSettingsDock->adjustSize();

    return simulationSettingsDock;
}

/*!
 * \brief MainWindow::createInteractionSettingsDock
 * \return QDockWidget interactionSettingsDock
 */
QDockWidget *MainWindow::createInteractionSettingsDock()
{
    interactionSettingsDock = new QDockWidget("Interactions", this);
    interactionSettingsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    interactionSettingsDock->setFeatures(QDockWidget::DockWidgetMovable);
    interactionSettingsDock->setFeatures(QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, interactionSettingsDock);

    auto *interactionGrid = new QGridLayout;
    interactionGrid->setAlignment(Qt::AlignTop);

    // Interaction Settings
    auto *interactionSettingsGrid = new QGridLayout;

    QLabel *interactions_settings_label = new QLabel("Interaction settings");
    interactions_settings_label->setStyleSheet("font-weight: bold");
    interactionSettingsGrid->addWidget(interactions_settings_label, 0, 1, 1, 2);

    BlockInteractionsRadio = new QRadioButton("Genome blocks");
    BlockInteractionsRadio->setToolTip("<font>Select to turn on interactions using the blocks system.</font>");
    XORRadio = new QRadioButton("Genome XOR");
    XORRadio->setToolTip("<font>Select to enable interactions using the XOR system.</font>");

    auto *interactionsButtonGroup = new QButtonGroup;
    interactionsButtonGroup->addButton(BlockInteractionsRadio, 1);
    interactionsButtonGroup->addButton(XORRadio, 0);
    if (simulationManager->cellSettingsMaster->interactBlocks) BlockInteractionsRadio->setChecked(true);
    else XORRadio->setChecked(true);

    auto *interactionsGrid = new QGridLayout;
    interactionsGrid->addWidget(XORRadio, 1, 1, 1, 1);
    interactionsGrid->addWidget(BlockInteractionsRadio, 1, 2, 1, 1);
    interactionSettingsGrid->addLayout(interactionsGrid, 1, 1, 1, 2);

    connect(interactionsButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [ = ](const bool & i)
    {
        if (i == 0) simulationManager->cellSettingsMaster->interactBlocks = false;
        else simulationManager->cellSettingsMaster->interactBlocks = true;
    });

    QLabel *interactions_label = new QLabel("Interactions:");
    interactions_label->setToolTip("<font>Sets the number of interactions attempted by each organism during settling.</font>");
    interactionsSpin = new QSpinBox;
    interactionsSpin->setMinimum(0);
    interactionsSpin->setMaximum(100);
    interactionsSpin->setValue(simulationManager->cellSettingsMaster->interactions);
    interactionsSpin->setToolTip("<font>Sets the number of interactions attempted by each organism during settling.</font>");
    interactionSettingsGrid->addWidget(interactions_label, 2, 1);
    interactionSettingsGrid->addWidget(interactionsSpin, 2, 2);
    connect(interactionsSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->interactions = i;
    });

    FitnessChangeRadio = new QRadioButton("Interactions Change Fitness");
    FitnessChangeRadio->setToolTip("<font>Select to change fitness using interactions.</font>");
    EnergyChangeRadio = new QRadioButton("Interactions Change Energy");
    EnergyChangeRadio->setToolTip("<font>Select to change energy using interactions.</font>");
    NoInteractionTypeRadio = new QRadioButton("Interactions Do Nothing");
    NoInteractionTypeRadio->setToolTip("<font>Select to change nothing using interactions.</font>");

    auto *FitnessEnergyButtonGroup = new QButtonGroup;
    FitnessEnergyButtonGroup->addButton(FitnessChangeRadio, 0);
    FitnessEnergyButtonGroup->addButton(EnergyChangeRadio, 1);
    FitnessEnergyButtonGroup->addButton(NoInteractionTypeRadio, 2);
    if (simulationManager->cellSettingsMaster->interactFitness) FitnessChangeRadio->setChecked(true);
    else if (simulationManager->cellSettingsMaster->interactEnergy) EnergyChangeRadio->setChecked(true);
    else NoInteractionTypeRadio->setChecked(true);

    auto *interactionMethodsGrid = new QGridLayout;
    interactionMethodsGrid->addWidget(FitnessChangeRadio, 1, 1, 1, 1);
    interactionMethodsGrid->addWidget(EnergyChangeRadio, 2, 1, 1, 1);
    interactionMethodsGrid->addWidget(NoInteractionTypeRadio, 3, 1, 1, 1);
    interactionSettingsGrid->addLayout(interactionMethodsGrid, 3, 1, 1, 2);

    connect(FitnessEnergyButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [ = ](const int &i)
    {
        if (i == 0)
        {
            simulationManager->cellSettingsMaster->interactFitness = true;
            simulationManager->cellSettingsMaster->interactEnergy = false;
        }
        if (i == 1)
        {
            simulationManager->cellSettingsMaster->interactFitness = false;
            simulationManager->cellSettingsMaster->interactEnergy = true;
        }
        if (i == 2)
        {
            simulationManager->cellSettingsMaster->interactFitness = false;
            simulationManager->cellSettingsMaster->interactEnergy = false;
        }
    });

    QPushButton *editInteractionsGridButton = new QPushButton("&Edit the interactions grid");
    editInteractionsGridButton->setObjectName("editInteractionsGridButton");
    editInteractionsGridButton ->setToolTip("<font>This launches a dialogue allowing you to edit the interactiosn grid.</font>");
    interactionSettingsGrid->addWidget(editInteractionsGridButton, 6, 1, 1, 2);
    connect(editInteractionsGridButton, SIGNAL (clicked()), this, SLOT(editInteractionsGrid()));

    QLabel *predationDelta_label = new QLabel("Min. Predator Score (Energy Only):");
    predationDelta_label->setToolTip("<font>Sets the minimum difference in predator score required for direct energy theft to occur.</font>");
    predationDeltaSpin = new QSpinBox;
    predationDeltaSpin->setMinimum(0);
    predationDeltaSpin->setMaximum(100);
    predationDeltaSpin->setValue(simulationManager->cellSettingsMaster->minDeltaPredatorness);
    predationDeltaSpin->setToolTip("<font>Sets the minimum difference in predator score required for direct energy theft to occur.</font>");
    interactionSettingsGrid->addWidget(predationDelta_label, 7, 1);
    interactionSettingsGrid->addWidget(predationDeltaSpin, 7, 2);
    connect(predationDeltaSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->minDeltaPredatorness = i;
    });

    QLabel *predationEfficiency_label = new QLabel("Predation Efficiency (Energy Only):");
    predationEfficiency_label->setToolTip("<font>Sets the conversion factor (/10) for energy transfer up trophic levels.</font>");
    predationEfficiencySpin = new QSpinBox;
    predationEfficiencySpin->setMinimum(0);
    predationEfficiencySpin->setMaximum(10);
    predationEfficiencySpin->setValue(simulationManager->cellSettingsMaster->predationEfficiency);
    predationEfficiencySpin->setToolTip("<font>Sets the conversion factor (/10) for energy transfer up trophic levels.</font>");
    interactionSettingsGrid->addWidget(predationEfficiency_label, 8, 1);
    interactionSettingsGrid->addWidget(predationEfficiencySpin, 8, 2);
    connect(predationEfficiencySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->predationEfficiency = i;
    });

    interactRestrictCheckbox = new QCheckBox("Restrict interactions");
    interactRestrictCheckbox->setChecked(simulationManager->simulationSettings->predationRestriction);
    interactRestrictCheckbox->setToolTip("<font>Turn on/off restrictions to the way that trophic levels can interact.</font>");
    interactionSettingsGrid->addWidget(interactRestrictCheckbox, 9, 1, 1, 2);
    connect(interactRestrictCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->predationRestriction = i;
    });

    //RJG - pathogen settings.
    QLabel *pathogen_settings_label = new QLabel("Pathogen settings");
    pathogen_settings_label->setStyleSheet("font-weight: bold");
    interactionSettingsGrid->addWidget(pathogen_settings_label, 10, 1, 1, 2);

    pathogensCheckbox = new QCheckBox("Pathogens layer");
    pathogensCheckbox->setChecked(simulationManager->cellSettingsMaster->pathOn);
    pathogensCheckbox->setToolTip("<font>Turn on/off the 'Pathogens' layer.</font>");
    interactionSettingsGrid->addWidget(pathogensCheckbox, 11, 1, 1, 2);
    connect(pathogensCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->cellSettingsMaster->pathOn = i;
    });

    QLabel *pathogen_mutate_label = new QLabel("Pathogen mutation:");
    pathogen_mutate_label->setToolTip("<font>Select the 'Pathogen Mutation' rate. Min = 1; Max = 255.</font>");
    pathogenMutateSpin = new QSpinBox;
    pathogenMutateSpin->setMinimum(1);
    pathogenMutateSpin->setMaximum(255);
    pathogenMutateSpin->setValue(simulationManager->cellSettingsMaster->pathogenMutate);
    pathogenMutateSpin->setToolTip("<font>Select the 'Pathogen Mutation' rate. Min = 1; Max = 255.</font>");
    interactionSettingsGrid->addWidget(pathogen_mutate_label, 12, 1);
    interactionSettingsGrid->addWidget(pathogenMutateSpin, 12, 2);
    connect(pathogenMutateSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->pathogenMutate = i;
    });

    QLabel *pathogen_frequency_label = new QLabel("Pathogen frequency:");
    pathogen_frequency_label->setToolTip("<font>Select the 'Pathogen Fequency' rate. Min = 1; Max = 1000.</font>");
    pathogenFrequencySpin = new QSpinBox;
    pathogenFrequencySpin->setMinimum(1);
    pathogenFrequencySpin->setMaximum(1000);
    pathogenFrequencySpin->setValue(simulationManager->cellSettingsMaster->pathogenFrequency);
    pathogenFrequencySpin->setToolTip("<font>Select the 'Pathogen Fequency' rate. Min = 1; Max = 1000.</font>");
    interactionSettingsGrid->addWidget(pathogen_frequency_label, 13, 1);
    interactionSettingsGrid->addWidget(pathogenFrequencySpin, 13, 2);
    connect(pathogenFrequencySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->pathogenFrequency = i;
    });


    //RJG - Variable breed only in development branch, as is obligate and facultative sexual
    QLabel *pathogen_mode_label = new QLabel("Pathogen mode:");
    interactionSettingsGrid->addWidget(pathogen_mode_label, 14, 1, 1, 2);

    pathogenDriftRadio = new QRadioButton("Drift");
    pathogenDriftRadio->setToolTip("<font>Select to use allow pathogens to drift.</font>");
    pathogenEvolveRadio = new QRadioButton("Evolve");
    pathogenEvolveRadio->setToolTip("<font>Select to have pathogens evolve for higher virulence.</font>");

    auto *pathogenButtonGroup = new QButtonGroup;
    pathogenButtonGroup->addButton(pathogenDriftRadio, 0);
    pathogenButtonGroup->addButton(pathogenEvolveRadio, 1);
    pathogenDriftRadio->setChecked(true);
    auto *pathogenModeGrid = new QGridLayout;

    pathogenModeGrid->addWidget(pathogenDriftRadio, 1, 1, 1, 1);
    pathogenModeGrid->addWidget(pathogenEvolveRadio, 1, 2, 1, 1);

    interactionSettingsGrid->addLayout(pathogenModeGrid, 15, 1, 1, 2);

    connect(pathogenButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [ = ](const int &i)
    {
        if (i == 0) simulationManager->simulationSettings->pathogenMode = PATH_MODE_DRIFT;
        else simulationManager->simulationSettings->pathogenMode = PATH_MODE_EVOLVE;
    });

    //ENF - Biological cropping
    QLabel *cropping_settings_label = new QLabel("Cropping settings");
    cropping_settings_label->setStyleSheet("font-weight: bold");
    interactionSettingsGrid->addWidget(cropping_settings_label, 16, 1, 1, 2);

    QLabel *cropping_frequency_label = new QLabel("Cropping frequency:");
    cropping_frequency_label->setToolTip("<font>Select the 'Cropping' rate. Min = 0; Max = 100.</font>");
    croppingFrequencySpin = new QSpinBox;
    croppingFrequencySpin->setMinimum(0);
    croppingFrequencySpin->setMaximum(100);
    croppingFrequencySpin->setValue(simulationManager->cellSettingsMaster->croppingFrequency);
    croppingFrequencySpin->setToolTip("<font>Select the 'Cropping' rate. Min = 0; Max = 100.</font>");
    interactionSettingsGrid->addWidget(cropping_frequency_label, 17, 1);
    interactionSettingsGrid->addWidget(croppingFrequencySpin, 17, 2);
    connect(croppingFrequencySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->croppingFrequency = i;
    });

    //PG - HGT settings
    QLabel *hgt_settings_label = new QLabel("HGT settings");
    hgt_settings_label->setStyleSheet("font-weight: bold");
    interactionSettingsGrid->addWidget(hgt_settings_label, 18, 1, 1, 2);

    variablehgtprobCheckbox = new QCheckBox("Variable HGT probability");
    variablehgtprobCheckbox->setChecked(simulationManager->simulationSettings->variableHgtProb);
    variablehgtprobCheckbox->setToolTip("<font>Turn on/off variable transformation (HGT).</font>");
    interactionSettingsGrid->addWidget(variablehgtprobCheckbox, 19, 1, 1, 2);
    connect(variablehgtprobCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->variableHgtProb = i;
    });

    hgtCheckbox = new QCheckBox("Transformation activated");
    hgtCheckbox->setChecked(simulationManager->cellSettingsMaster->hgtTransform);
    hgtCheckbox->setToolTip("<font>Turn on/off transformation (HGT).</font>");
    interactionSettingsGrid->addWidget(hgtCheckbox, 19, 2, 1, 2);
    connect(hgtCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->cellSettingsMaster->hgtTransform = i;
    });

    //PG - link together so only one hgt setting can be selected at a time
    connect(variablehgtprobCheckbox, &QCheckBox::stateChanged, [ = ](int state) {
        if (state == Qt::Checked) {
            // Uncheck the other checkbox
            hgtCheckbox->setChecked(false);
        }
    });

    connect(hgtCheckbox, &QCheckBox::stateChanged, [ = ](int state) {
        if (state == Qt::Checked) {
            // Uncheck the other checkbox
            variablehgtprobCheckbox->setChecked(false);
        }
    });

    QLabel *transfer_chance_label = new QLabel("Transfer probability - 1 in ");
    transfer_chance_label->setToolTip("<font>Set the HGT transfer chance per cell per generation.</font>");
    transferchanceSpin = new QSpinBox();
    transferchanceSpin->setMinimum(100);
    transferchanceSpin->setMaximum(10000000);
    interactionSettingsGrid->addWidget(transfer_chance_label, 20, 1);
    interactionSettingsGrid->addWidget(transferchanceSpin, 20, 2);

    variablehgtlenCheckbox = new QCheckBox("Variable HGT length");
    variablehgtlenCheckbox->setChecked(simulationManager->simulationSettings->variableHgtLen);
    variablehgtlenCheckbox->setToolTip("<font>Turn on/off variable transformation (HGT).</font>");
    interactionSettingsGrid->addWidget(variablehgtlenCheckbox, 22, 1, 1, 2);
    connect(variablehgtlenCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->variableHgtLen = i;
    });

    randomlengthCheckbox = new QCheckBox("Random transfer length");
    randomlengthCheckbox->setToolTip("<font>Turn on/off nonSpatial settling of offspring.</font>");
    interactionSettingsGrid->addWidget(randomlengthCheckbox, 22, 2, 1, 2);
    randomlengthCheckbox->setChecked(simulationManager->simulationSettings->hgtrandomlength);
    connect(randomlengthCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->hgtrandomlength = i;
    });

    //PG - link together so only one hgt setting can be selected at a time
    connect(variablehgtlenCheckbox, &QCheckBox::stateChanged, [ = ](int state) {
        if (state == Qt::Checked) {
            // Uncheck the other checkbox
            randomlengthCheckbox->setChecked(false);
        }
    });

    connect(randomlengthCheckbox, &QCheckBox::stateChanged, [ = ](int state) {
        if (state == Qt::Checked) {
            // Uncheck the other checkbox
            variablehgtlenCheckbox->setChecked(false);
        }
    });

    QLabel *transfer_length_label = new QLabel("Transfer length: ");
    transfer_length_label->setToolTip("<font>Set the HGT transfer length.</font>");
    transferlengthSpin = new QSpinBox;
    transferlengthSpin->setMinimum(1);
    transferlengthSpin->setValue(simulationManager->cellSettingsMaster->hgtTransferLength);
    transferlengthSpin->setToolTip("<font>Set the HGT transfer length.</font>");
    interactionSettingsGrid->addWidget(transfer_length_label, 23, 1);
    interactionSettingsGrid->addWidget(transferlengthSpin, 23, 2);
    connect(transferlengthSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->hgtTransferLength = i;
    });

    //- make sure transfer length inputted is less than the transformable genome size
    connect(transferlengthSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        if (i > (simulationManager->hgtSystem->returnUseGenomeWordsCount() *32))
        {
            transferlengthSpin->clear();
        }
    });

    connect(hgt_word_edit_1, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        if (transferlengthSpin->value() > (simulationManager->hgtSystem->returnUseGenomeWordsCount() *32))
        {

            transferlengthSpin->clear();
        }
    });



    //PG - link together so only one hgt setting can be selected at a time
    connect(variablehgtlenCheckbox, &QCheckBox::stateChanged, [ = ](int state) {
        if (state == Qt::Checked) {
            // Uncheck the other checkbox
            randomlengthCheckbox->setChecked(false);
        }
    });

    connect(randomlengthCheckbox, &QCheckBox::stateChanged, [ = ](int state) {
        if (state == Qt::Checked) {
            // Uncheck the other checkbox
            variablehgtlenCheckbox->setChecked(false);
        }
    });

    variablehgtidCheckbox = new QCheckBox("Variable HGT ID matching");
    variablehgtidCheckbox->setChecked(simulationManager->simulationSettings->variableHgtId);
    variablehgtidCheckbox->setToolTip("<font>Turn on/off variable transformation (HGT).</font>");
    interactionSettingsGrid->addWidget(variablehgtidCheckbox, 24, 1, 1, 2);
    connect(variablehgtidCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->variableHgtId = i;
    });

    hgtidCheckbox = new QCheckBox("ID matching enabled");
    hgtidCheckbox->setToolTip("<font>Turn on/off max Difference to hgt.</font>");
    interactionSettingsGrid->addWidget(hgtidCheckbox, 24, 2, 1, 2);
    hgtidCheckbox->setChecked(simulationManager->simulationSettings->hgtId);
    connect(hgtidCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->hgtId = i;
    });

    //PG - link together so only one hgt setting can be selected at a time
    connect(hgtidCheckbox, &QCheckBox::stateChanged, [ = ](int state) {
        if (state == Qt::Checked) {
            // Uncheck the other checkbox
            variablehgtidCheckbox->setChecked(false);
        }
    });

    connect(variablehgtidCheckbox, &QCheckBox::stateChanged, [ = ](int state) {
        if (state == Qt::Checked) {
            // Uncheck the other checkbox
            hgtidCheckbox->setChecked(false);
        }
    });

    QLabel *transfer_max_difference_label = new QLabel("Max difference for transformation ");
    transfer_max_difference_label->setToolTip("<font>Set the HGT transfer chance per cell per generation.</font>");
    transfermaxdifferenceSpin = new QSpinBox();
    transfermaxdifferenceSpin->setMinimum(1);
    transfermaxdifferenceSpin->setMaximum(32);
    interactionSettingsGrid->addWidget(transfer_max_difference_label, 25, 1);
    interactionSettingsGrid->addWidget(transfermaxdifferenceSpin, 25, 2);
    connect(transfermaxdifferenceSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->simulationSettings->maxDifferenceHgt = i;
    });




    // PG- to add in when functions sorted, currently only non-Synonoymous works.
    QLabel *hgt_mode_label = new QLabel("Transformation placement:");
    interactionSettingsGrid->addWidget(hgt_mode_label, 26, 1, 1, 2);
    connect(transferchanceSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster-> hgtTransferChance = i;
    });
    hgtSynonoymousRadio = new QRadioButton("Same position");
    hgtSynonoymousRadio->setToolTip("<font>Transfer segment placed in recipent at same location as donor genome.</font>");
    hgtNonSynonoymousRadio = new QRadioButton("Shifted position");
    hgtNonSynonoymousRadio->setToolTip("<font>Transfer segment placed in recipent at different location than donor.</font>");

    auto *hgtButtonGroup = new QButtonGroup;
    hgtButtonGroup->addButton(hgtSynonoymousRadio, 0);
    hgtButtonGroup->addButton(hgtNonSynonoymousRadio, 1);
    hgtSynonoymousRadio->setChecked(true);
    auto *HgtModeGrid = new QGridLayout;

    HgtModeGrid->addWidget(hgtSynonoymousRadio, 1, 1, 1, 1);
    HgtModeGrid->addWidget(hgtNonSynonoymousRadio, 1, 2, 1, 1);

    interactionSettingsGrid->addLayout(HgtModeGrid, 27, 1, 1, 2);

    connect(hgtButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [ = ](const int &i)
    {
        if (i == 0) simulationManager->simulationSettings->hgtMode = HGT_SYNOYMOUS;
        else simulationManager->simulationSettings->hgtMode = HGT_NON_SYNOYMOUS;
    });

    //ENF - Genome seeding settings
    /*QLabel *seeding_settings_label = new QLabel("Genome seeding settings");
    seeding_settings_label->setStyleSheet("font-weight: bold");
    interactionSettingsGrid->addWidget(pathogen_settings_label, 16, 1, 1, 2);

    QLabel *seeding_mode_label = new QLabel("Seeding mode:");
    interactionSettingsGrid->addWidget(seeding_mode_label, 17, 1, 1, 2);

    seedingDefaultRadio = new QRadioButton("Default");
    seedingDefaultRadio->setToolTip("<font>Select to use the default genome seeding system where a single genome is found that fits the central environment pixel.</font>");
    seeding3TierRadio = new QRadioButton("3-Tier");
    seeding3TierRadio->setToolTip("<font>Select to seed 3 trophic levels of organisms with fixed genomes.</font>");
    seeding5TierRadio = new QRadioButton("5-Tier");
    seeding5TierRadio->setToolTip("<font>Select to seed 5 trophic levels of organisms with fixed genomes.</font>");

    auto *seedingButtonGroup = new QButtonGroup;
    seedingButtonGroup->addButton(seedingDefaultRadio, 0);
    seedingButtonGroup->addButton(seeding3TierRadio, 1);
    seedingButtonGroup->addButton(seeding5TierRadio, 2);
    seedingDefaultRadio->setChecked(true);
    auto *seedingModeGrid = new QGridLayout;

    seedingModeGrid->addWidget(seedingDefaultRadio, 1, 1, 1, 1);
    seedingModeGrid->addWidget(seeding3TierRadio, 1, 2, 1, 1);
    seedingModeGrid->addWidget(seeding5TierRadio, 1, 3, 1, 1);

    interactionSettingsGrid->addLayout(seedingModeGrid, 18, 1, 1, 2);

    connect(seedingButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [ = ](const int &i)
    {
        if (i == 0) simulationManager->simulationSettings->trophicSeedMode = TROPHIC_SEED_DEFAULT;
        else if (i == 1) simulationManager->simulationSettings->trophicSeedMode = TROPHIC_SEED_3TIER;
        else simulationManager->simulationSettings->trophicSeedMode = TROPHIC_SEED_5TIER;
    });*/

    //ARTS - Dock Grid Layout
    interactionGrid->addLayout(interactionSettingsGrid, 0, 1);

    QWidget *interactionLayoutWidget = new QWidget;
    interactionLayoutWidget->setLayout(interactionGrid);
    interactionLayoutWidget->setMinimumWidth(300);
    interactionSettingsDock->setWidget(interactionLayoutWidget);
    interactionSettingsDock->adjustSize();

    return interactionSettingsDock;
}

/*!
 * \brief MainWindow::createOutputSettingsDock
 * \return QDockWidget outputSettingsDock
 */
QDockWidget *MainWindow::createOutputSettingsDock()
{
    outputSettingsDock = new QDockWidget("Output settings", this);
    outputSettingsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    outputSettingsDock->setFeatures(QDockWidget::DockWidgetMovable);
    outputSettingsDock->setFeatures(QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::LeftDockWidgetArea, outputSettingsDock);

    auto *outputSettingsGrid = new QGridLayout;
    outputSettingsGrid->setAlignment(Qt::AlignTop);

    //ARTS - Output Save Path
    auto *savePathGrid = new QGridLayout;
    QLabel *savePathLabel = new QLabel("Output save path:");
    savePathLabel->setObjectName("savePathLabel");
    savePathGrid->addWidget(savePathLabel, 1, 1, 1, 2);
    QString programPath(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    programPath.append("/");
    globalSavePath = new QLineEdit(programPath);
    globalSavePath->setToolTip("<font>System save path where all logs, images, and output data is saved.</font>");
    savePathGrid->addWidget(globalSavePath, 2, 1, 1, 2);
    QPushButton *changePathButton = new QPushButton("&Change");
    changePathButton->setToolTip("<font>System save path where all logs, images, and output data is saved.</font>");
    changePathButton->setObjectName("changePathButton");
    savePathGrid->addWidget(changePathButton, 3, 1, 1, 2);
    connect(changePathButton, SIGNAL (clicked()), this, SLOT(udpateGlobalPath()));

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
    refreshRateSpin->setMaximum(100000);
    refreshRateSpin->setValue(refreshRate);
    refreshRateSpin->setToolTip("<font>Number of iteration between each logging and screen data refresh event.</font>");
    pollingRateGrid->addWidget(refreshRateLabel, 2, 1);
    pollingRateGrid->addWidget(refreshRateSpin, 2, 2);
    connect(refreshRateSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), mainWindow, [ = ](const int &i)
    {
        refreshRate = i;
    });

    //ARTS - Logging: Population & Environment
    auto *imagesGrid = new QGridLayout;

    QLabel *imagesLabel = new QLabel("Image logging");
    imagesLabel->setObjectName("imagesLabel");
    imagesGrid->addWidget(imagesLabel, 1, 1, 1, 2);

    savePopulationCount = new QCheckBox("Population count");
    savePopulationCount->setToolTip("<font>Turn on/off to log the 'Population Count' as an image.</font>");
    imagesGrid->addWidget(savePopulationCount, 2, 1, 1, 1);
    saveMeanFitness = new QCheckBox("Mean fitness");
    saveMeanFitness->setToolTip("<font>Turn on/off to log the 'Mean Fitness' as an image.</font>");
    imagesGrid->addWidget(saveMeanFitness, 2, 2, 1, 1);
    saveVisSysOneAsColour = new QCheckBox("Visualisation system 1");
    saveVisSysOneAsColour->setToolTip("<font>Turn on/off to log the 'Coding Genome' as an image.</font>");
    imagesGrid->addWidget(saveVisSysOneAsColour, 3, 1, 1, 1);
    saveVisSysTwoAsColour = new QCheckBox("Visualisation system 2");
    saveVisSysTwoAsColour->setToolTip("<font>Turn on/off to log the 'Non-coding Genome' as an image.</font>");
    imagesGrid->addWidget(saveVisSysTwoAsColour, 3, 2, 1, 1);
    saveSpecies = new QCheckBox("Species");
    saveSpecies->setToolTip("<font>Turn on/off to log the 'Species' as an image.</font>");
    imagesGrid->addWidget(saveSpecies, 4, 1, 1, 1);
    saveSettles = new QCheckBox("Settles");
    saveSettles->setToolTip("<font>Turn on/off to log the 'Settles' as an image.</font>");
    imagesGrid->addWidget(saveSettles, 4, 2, 1, 1);
    saveFailsSettles = new QCheckBox("Breed/Settle Fails");
    saveFailsSettles->setToolTip("<font>Turn on/off to log the 'Breed/Settle Fails' as an image.</font>");
    imagesGrid->addWidget(saveFailsSettles, 5, 1, 1, 1);
    saveEnvironment = new QCheckBox("Environment");
    saveEnvironment->setToolTip("<font>Turn on/off to log the 'Environment' as an image.</font>");
    imagesGrid->addWidget(saveEnvironment, 5, 2, 1, 1);

    QCheckBox *saveAllImagesCheckbox = new QCheckBox("All");
    saveAllImagesCheckbox->setObjectName("saveAllImagesCheckbox");
    saveAllImagesCheckbox->setToolTip("<font>Toggle all/none of the above checkboxes.</font>");
    imagesGrid->addWidget(saveAllImagesCheckbox, 6, 1, 1, 1);
    QObject::connect(saveAllImagesCheckbox, SIGNAL (toggled(bool)), this, SLOT(saveAllCheckboxStateChanged(bool)));

    //ARTS - Logging to text file
    auto *fileLoggingGrid = new QGridLayout;

    QLabel *outputSettingsLabel = new QLabel("End run log");
    outputSettingsLabel->setObjectName("outputSettingsLabel");
    fileLoggingGrid->addWidget(outputSettingsLabel, 1, 1, 1, 2);

    autoWriteLogCheckbox = new QCheckBox("Automatically create end run log for batch runs");
    autoWriteLogCheckbox->setChecked(true);
    autoWriteLogCheckbox->setToolTip("<font>Turn on/off this option to automatically write detailed log after batch runs</font>");
    fileLoggingGrid->addWidget(autoWriteLogCheckbox, 2, 1, 1, 2);

    QPushButton *write_nwk = new QPushButton("Write data (including tree) for current run");
    write_nwk->setToolTip("<font>Use this option to manually trigger the writing of a detailed log after a run.</font>");
    fileLoggingGrid->addWidget(write_nwk, 3, 1, 1, 2);
    connect(write_nwk, SIGNAL (clicked()), this, SLOT(writeRunData()));

    excludeWithoutDescendantsCheckbox = new QCheckBox("Exclude species without descendents");
    excludeWithoutDescendantsCheckbox->setChecked(allowExcludeWithDescendants);
    excludeWithoutDescendantsCheckbox->setToolTip("<font>Use this option to exclude species without descendents.</font>");
    fileLoggingGrid->addWidget(excludeWithoutDescendantsCheckbox, 4, 1, 1, 1);
    connect(excludeWithoutDescendantsCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        allowExcludeWithDescendants = i;
    });

    QLabel *minSpeciesSizeLabel = new QLabel("Minimum species size:");
    minSpeciesSizeLabel->setToolTip("<font>Selects the minimum number of organisms needed to define a species. Min = 0; Max = 1000000.</font>");

    minSpeciesSizeSpin = new QSpinBox;
    minSpeciesSizeSpin->setToolTip("<font>Selects the minimum number of organisms needed to define a species. Min = 0; Max = 1000000.</font>");
    minSpeciesSizeSpin->setMinimum(0);
    minSpeciesSizeSpin->setMaximum(1000000);
    minSpeciesSizeSpin->setValue(static_cast<int>(simulationManager->simulationSettings->minSpeciesSize));
    fileLoggingGrid->addWidget(minSpeciesSizeLabel, 5, 1);
    fileLoggingGrid->addWidget(minSpeciesSizeSpin, 5, 2);
    connect(minSpeciesSizeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->simulationSettings->minSpeciesSize = static_cast<quint64>(i);
    });


    autoWriteLogIndividualsCheckbox = new QCheckBox("Automatically create individuals log for batch runs");
    autoWriteLogIndividualsCheckbox->setChecked(false);
    autoWriteLogIndividualsCheckbox->setToolTip("<font>Turn on/off this option to automatically write the individuals log after batch runs</font>");
    fileLoggingGrid->addWidget(autoWriteLogIndividualsCheckbox, 6, 1, 1, 2);

    QPushButton *dumpIndividualsButton = new QPushButton("Write data on every individual for current run");
    dumpIndividualsButton ->setToolTip("<font>Use this option to write key data for every living individual to file.</font>");
    fileLoggingGrid->addWidget(dumpIndividualsButton, 7, 1, 1, 2);
    connect(dumpIndividualsButton, SIGNAL (clicked()), this, SLOT(writeIndividualsData()));

    //ARTS - Advanced
    auto *advancedLoggingGrid = new QGridLayout;

    QLabel *advancedSettingsLabel = new QLabel("Advanced");
    advancedSettingsLabel->setObjectName("advancedSettingsLabel");
    advancedLoggingGrid->addWidget(advancedSettingsLabel, 1, 1, 1, 2);

    guiCheckbox = new QCheckBox("Don't update GUI on refresh/poll");
    guiCheckbox->setChecked(simulationManager->simulationSettings->gui);
    guiCheckbox->setToolTip("<font>Note: If you turn off GUI update you cannot log the population/environment windows using saved images.</font>");
    advancedLoggingGrid->addWidget(guiCheckbox, 2, 1, 1, 2);
    QObject::connect(guiCheckbox, SIGNAL (toggled(bool)), this, SLOT(guiCheckboxStateChanged(bool)));

    //ARTS - Dock Grid Layout
    outputSettingsGrid->addLayout(savePathGrid, 1, 1, 1, 2);
    outputSettingsGrid->addLayout(pollingRateGrid, 2, 1, 1, 2);
    outputSettingsGrid->addLayout(imagesGrid, 3, 1, 1, 2);
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

    QLabel *org_settings_label = new QLabel("Organism settings");
    org_settings_label->setStyleSheet("font-weight: bold");
    organismSettingsGrid->addWidget(org_settings_label, 1, 1, 1, 2);

    QLabel *mutate_label = new QLabel("Chance of mutation:");
    mutate_label->setToolTip("<font>Selects the chance of mutation. Min = 0; Max = 255.</font>");
    mutateSpin = new QSpinBox;
    mutateSpin->setMinimum(0);
    mutateSpin->setMaximum(255);
    mutateSpin->setValue(simulationManager->cellSettingsMaster->mutate);
    mutateSpin->setToolTip("<font>Selects the chance of mutation. Min = 0; Max = 255.</font>");
    organismSettingsGrid->addWidget(mutate_label, 2, 1);
    organismSettingsGrid->addWidget(mutateSpin, 2, 2);
    connect(mutateSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->mutate = i;
    });

    variableMutationCheckbox = new QCheckBox("Variable mutation");
    variableMutationCheckbox->setToolTip("<font>Turn on/off to toggle variable mutation. This overrides the selected mutation rate (above).</font>");
    organismSettingsGrid->addWidget(variableMutationCheckbox, 3, 1, 1, 2);
    variableMutationCheckbox->setChecked(simulationManager->cellSettingsMaster->variableMutate);
    connect(variableMutationCheckbox, &QCheckBox::stateChanged, mainWindow, [ = ](const bool & i)
    {
        simulationManager->cellSettingsMaster->variableMutate = i;
        mutateSpin->setEnabled(!i);
    });

    QLabel *startAge_label = new QLabel("Start age:");
    startAge_label->setToolTip("<font>Sets the starting age for organisms. Min = 1; Max = 1000.</font>");
    startAgeSpin = new QSpinBox;
    startAgeSpin->setMinimum(1);
    startAgeSpin->setMaximum(1000);
    startAgeSpin->setValue(simulationManager->cellSettingsMaster->startAge);
    startAgeSpin->setToolTip("<font>Sets the starting age for organisms. Min = 1; Max = 1000.</font>");
    organismSettingsGrid->addWidget(startAge_label, 4, 1);
    organismSettingsGrid->addWidget(startAgeSpin, 4, 2);
    connect(startAgeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->startAge = i;
    });

    QLabel *breed_settings_label = new QLabel("Breed settings");
    breed_settings_label->setStyleSheet("font-weight: bold");
    organismSettingsGrid->addWidget(breed_settings_label, 5, 1, 1, 2);

    QLabel *breedThreshold_label = new QLabel("Breed threshold:");
    breedThreshold_label->setToolTip("<font>Sets the 'Breed Threshold'. Min = 0; Max = 5000.</font>");
    breedThresholdSpin = new QSpinBox;
    breedThresholdSpin->setMinimum(1);
    breedThresholdSpin->setMaximum(5000);
    breedThresholdSpin->setValue(simulationManager->cellSettingsMaster->breedThreshold);
    breedThresholdSpin->setToolTip("<font>Sets the 'Breed Threshold'. Min = 0; Max = 5000.</font>");
    organismSettingsGrid->addWidget(breedThreshold_label, 6, 1);
    organismSettingsGrid->addWidget(breedThresholdSpin, 6, 2);
    connect(breedThresholdSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->breedThreshold = i;
    });

    QLabel *breedCost_label = new QLabel("Breed cost:");
    breedCost_label->setToolTip("<font>Sets the 'Breed Cost'. Min = 0; Max = 10000.</font>");
    breedCostSpin = new QSpinBox;
    breedCostSpin->setMinimum(1);
    breedCostSpin->setMaximum(10000);
    breedCostSpin->setValue(simulationManager->cellSettingsMaster->breedCost);
    breedCostSpin->setToolTip("<font>Sets the 'Breed Cost'. Min = 0; Max = 10000.</font>");
    organismSettingsGrid->addWidget(breedCost_label, 7, 1);
    organismSettingsGrid->addWidget(breedCostSpin, 7, 2);
    connect(breedCostSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->breedCost = i;
    });

    QLabel *maxDifferenceLabel = new QLabel("Max difference to breed:");
    maxDifferenceLabel->setToolTip("<font>Sets the maximum difference between organisms to allow breeding. Min = 1; Max = 31.</font>");
    maxDifferenceSpin = new QSpinBox;
    maxDifferenceSpin->setMinimum(1);
    maxDifferenceSpin->setMaximum(31);
    maxDifferenceSpin->setValue(simulationManager->simulationSettings->maxDifference);
    maxDifferenceSpin->setToolTip("<font>Sets the maximum difference between organisms to allow breeding. Min = 1; Max = 31.</font>");
    organismSettingsGrid->addWidget(maxDifferenceLabel, 8, 1);
    organismSettingsGrid->addWidget(maxDifferenceSpin, 8, 2);
    connect(maxDifferenceSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->simulationSettings->maxDifference = i;
    });

    breedDifferenceCheckbox = new QCheckBox("Use max diff to breed");
    breedDifferenceCheckbox->setToolTip("<font>Turn on/off the maximum difference between organisms test.</font>");
    organismSettingsGrid->addWidget(breedDifferenceCheckbox, 9, 1, 1, 2);
    breedDifferenceCheckbox->setChecked(simulationManager->cellSettingsMaster->breedDifference);
    connect(breedDifferenceCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->cellSettingsMaster->breedDifference = i;
    });

    breedSpeciesCheckbox = new QCheckBox("Breed only within species");
    breedSpeciesCheckbox->setToolTip("<font>Turn on/off breeding only within the same species.</font>");
    organismSettingsGrid->addWidget(breedSpeciesCheckbox, 10, 1, 1, 2);
    breedSpeciesCheckbox->setChecked(simulationManager->cellSettingsMaster->breedSpecies);
    connect(breedSpeciesCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->cellSettingsMaster->breedSpecies = i;
    });

    MultiBreedListCheckbox = new QCheckBox("Multiple Breed Lists");
    MultiBreedListCheckbox->setChecked(simulationManager->cellSettingsMaster->multiBreedList);
    MultiBreedListCheckbox->setToolTip("<font>Turn on/off multiple breed lists per cell.</font>");
    organismSettingsGrid->addWidget(MultiBreedListCheckbox, 11, 1, 1, 2);
    connect(MultiBreedListCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->cellSettingsMaster->multiBreedList = i;
    });

    //RJG - Variable breed only in development branch, as is obligate and facultative sexual
    QLabel *breed_mode_label = new QLabel("Breed mode:");
    organismSettingsGrid->addWidget(breed_mode_label, 12, 1, 1, 2);

    obligateSexualRadio = new QRadioButton("Obligate");
    obligateSexualRadio->setToolTip("<font>Select to use obligate sexual breeding.</font>");
    facultativeSexualRadio = new QRadioButton("Facultative");
    facultativeSexualRadio->setToolTip("<font>Select to use facultative sexual breeding.</font>");
    asexualRadio = new QRadioButton("Asexual");
    asexualRadio->setToolTip("<font>Select to use asexual breeding.</font>");
    variableBreedRadio = new QRadioButton("Variable");
    variableBreedRadio->setToolTip("<font>Select to use variable breeding.</font>");

    auto *breedingButtonGroup = new QButtonGroup;
    breedingButtonGroup->addButton(obligateSexualRadio, 0);
    breedingButtonGroup->addButton(facultativeSexualRadio, 1);
    breedingButtonGroup->addButton(asexualRadio, 2);
    breedingButtonGroup->addButton(variableBreedRadio, 3);
    obligateSexualRadio->setChecked(simulationManager->cellSettingsMaster->obligateSexual);
    asexualRadio->setChecked(simulationManager->cellSettingsMaster->asexual);
    variableBreedRadio->setChecked(simulationManager->cellSettingsMaster->variableBreed);

    auto *breedModeGrid = new QGridLayout;

    breedModeGrid->addWidget(obligateSexualRadio, 1, 1, 1, 1);
    breedModeGrid->addWidget(facultativeSexualRadio, 1, 2, 1, 1);
    breedModeGrid->addWidget(asexualRadio, 2, 1, 1, 1);
    breedModeGrid->addWidget(variableBreedRadio, 2, 2, 1, 1);

    organismSettingsGrid->addLayout(breedModeGrid, 13, 1, 1, 2);

    connect(breedingButtonGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), [ = ](const int &i)
    {
        if (i == 0)
        {
            simulationManager->cellSettingsMaster->obligateSexual = true;
            simulationManager->cellSettingsMaster->facultativeSexual = false;
            simulationManager->cellSettingsMaster->asexual = false;
            simulationManager->cellSettingsMaster->variableBreed = false;
        }
        if (i == 1)
        {
            simulationManager->cellSettingsMaster->obligateSexual = false;
            simulationManager->cellSettingsMaster->facultativeSexual = true;
            simulationManager->cellSettingsMaster->asexual = false;
            simulationManager->cellSettingsMaster->variableBreed = false;
        }
        if (i == 2)
        {
            simulationManager->cellSettingsMaster->obligateSexual = false;
            simulationManager->cellSettingsMaster->facultativeSexual = false;
            simulationManager->cellSettingsMaster->asexual = true;
            simulationManager->cellSettingsMaster->variableBreed = false;
        }
        if (i == 3)
        {
            simulationManager->cellSettingsMaster->obligateSexual = false;
            simulationManager->cellSettingsMaster->facultativeSexual = false;
            simulationManager->cellSettingsMaster->asexual = false;
            simulationManager->cellSettingsMaster->variableBreed = true;
        }
    });

    QLabel *settle_settings_label = new QLabel("Settle settings");
    settle_settings_label->setStyleSheet("font-weight: bold");
    organismSettingsGrid->addWidget(settle_settings_label, 14, 1, 1, 2);

    QLabel *dispersal_label = new QLabel("Dispersal:");
    dispersal_label->setToolTip("<font>Set the maximum dispersal of offspring from parent. Min = 1; Max = 200. Note that this is a divisor - smaller numbers allow more dispersal.</font>");
    dispersalSpin = new QSpinBox;
    dispersalSpin->setMinimum(1);
    dispersalSpin->setMaximum(200);
    dispersalSpin->setValue(simulationManager->cellSettingsMaster->dispersal);
    dispersalSpin->setToolTip("<font>Set the maximum dispersal of offspring from parent. Min = 1; Max = 200. Note that this is a divisor - smaller numbers allow more dispersal.</font>");
    organismSettingsGrid->addWidget(dispersal_label, 15, 1);
    organismSettingsGrid->addWidget(dispersalSpin, 15, 2);
    connect(dispersalSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [ = ](const int &i)
    {
        simulationManager->cellSettingsMaster->dispersal = i;
    });

    nonspatialCheckbox = new QCheckBox("Nonspatial settling");
    nonspatialCheckbox->setToolTip("<font>Turn on/off nonSpatial settling of offspring.</font>");
    organismSettingsGrid->addWidget(nonspatialCheckbox, 16, 1, 1, 2);
    nonspatialCheckbox->setChecked(simulationManager->simulationSettings->nonspatial);
    connect(nonspatialCheckbox, &QCheckBox::stateChanged, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->nonspatial = i;
    });

    QLabel *word_settings_label = new QLabel("Genome Words");
    word_settings_label->setStyleSheet("font-weight: bold");
    word_settings_label->setToolTip("Set systems to genome words below. Systems may need to be enabled elsewhere in GUI. Impossible strings will result in red label.");
    organismSettingsGrid->addWidget(word_settings_label, 17, 1, 1, 2);

    QLabel *genomesize_label = new QLabel("Genome size:");
    genomesize_label->setToolTip("<font>Genome size (in 32 bit words).</font>");
    genomeSizeSpin = new QSpinBox;
    genomeSizeSpin->setMinimum(1);
    genomeSizeSpin->setMaximum(32);
    genomeSizeSpin->setValue(simulationManager->simulationSettings->genomeSize);
    genomeSizeSpin->setToolTip("<font>Genome size (in 32 bit words).</font>");
    organismSettingsGrid->addWidget(genomesize_label, 18, 1);
    organismSettingsGrid->addWidget(genomeSizeSpin, 18, 2);
    connect(genomeSizeSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), mainWindow,  [ = ](const int &i)
    {
        System system("Temp");
        int max = 0;
        foreach (QString s, wordsInUse)
            if (system.returnMaxFromString(s) > max)max = system.returnMaxFromString(s);
        if (i < (max + 1))
        {
            genomeSizeSpin->setValue(max + 1);
            return;
        }
        simulationManager->simulationSettings->genomeSize = i;
    });

    //RJG - scroll area has its own widget with a layout applied - do this in function below
    QScrollArea *scrollArea = createWordsScrollArea();
    organismSettingsGrid->addWidget(scrollArea, 19, 1, 1, 2);

    QWidget *org_settings_layout_widget = new QWidget;
    org_settings_layout_widget->setLayout(organismSettingsGrid);
    organismSettingsDock->setWidget(org_settings_layout_widget);

    return organismSettingsDock;
}

/*!
 * \brief MainWindow::createWordsScrollArea
 * \return QScrollArea scrollArea
 */
QScrollArea *MainWindow::createWordsScrollArea()
{
    //RJG - create scroll area, apply layout to a widget that scroll area views
    QScrollArea *scrollArea = new QScrollArea(this);

    auto *scrollAreaGrid = new QGridLayout;
    scrollArea->setWidgetResizable(true);
    scrollAreaGrid->setAlignment(Qt::AlignTop);

    //RJG - Fitness system
    SysLabel *fitness_word_label = new SysLabel("Fitness: ");
    scrollAreaGrid->addWidget(fitness_word_label, 0, 0);
    fitness_word_edit = new QLineEdit;
    scrollAreaGrid->addWidget(fitness_word_edit, 0, 1);
    fitness_word_edit->setText(simulationManager->environmentalFitnessSytem->returnGenomeWordInUseString());
    wordsInUse.insert("Environment", simulationManager->environmentalFitnessSytem->returnGenomeWordInUseString());

    fitnessMessageTimer.setSingleShot(true); //RJG - trigger only once per start() call
    connect(&fitnessMessageTimer, &QTimer::timeout, this, [&]()
    {
        if (!autoFromCommand)
            if (QMessageBox::question(nullptr, "Update fitness target?", "Would you like to update the fitness target to a value suitable for this number of genome words?",
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
                targetSpin->setValue(simulationManager->environmentalFitnessSytem->returnUseGenomeWordsCount() * 66);
    });

    connect(fitness_word_edit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->environmentalFitnessSytem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            fitness_word_label->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->environmentalFitnessSytem->setGenomeWordsFromString(s, genomeSizeSpin->value())) fitness_word_label->changeColour(Qt::red);
        else
        {
            fitness_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Environment", s);
            fitnessMessageTimer.start(1000);
        }
    });

    //RJG - Breed system
    SysLabel *breed_word_label = new SysLabel("Breed: ");
    scrollAreaGrid->addWidget(breed_word_label, 1, 0);
    breed_word_edit = new QLineEdit;
    scrollAreaGrid->addWidget(breed_word_edit, 1, 1);
    breed_word_edit->setText(simulationManager->breedSystem->returnGenomeWordInUseString());
    wordsInUse.insert("Breed", simulationManager->breedSystem->returnGenomeWordInUseString());
    connect(breed_word_edit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->breedSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            breed_word_label->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->breedSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) breed_word_label->changeColour(Qt::red);
        else
        {
            breed_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Breed", s);
        }
    });

    //RJG - muttate system
    SysLabel *mutate_word_label = new SysLabel("Mutate: ");
    scrollAreaGrid->addWidget(mutate_word_label, 2, 0);
    mutate_word_edit = new QLineEdit;
    scrollAreaGrid->addWidget(mutate_word_edit, 2, 1);
    mutate_word_edit->setText(simulationManager->mutationSystem->returnGenomeWordInUseString());
    wordsInUse.insert("Mutate", simulationManager->mutationSystem->returnGenomeWordInUseString());
    connect(mutate_word_edit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->mutationSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            mutate_word_label->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->mutationSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) mutate_word_label->changeColour(Qt::red);
        else
        {
            mutate_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Mutate", s);
        }
    });

    //RJG - Variable mutate
    SysLabel *variable_mutate_word_label = new SysLabel("Variable Mutate: ");
    scrollAreaGrid->addWidget(variable_mutate_word_label, 3, 0);
    variable_mutate_word_edit = new QLineEdit;
    scrollAreaGrid->addWidget(variable_mutate_word_edit, 3, 1);
    variable_mutate_word_edit->setText(simulationManager->variableMutateSystem->returnGenomeWordInUseString());
    wordsInUse.insert("VariableMutate", simulationManager->variableMutateSystem->returnGenomeWordInUseString());
    connect(variable_mutate_word_edit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->variableMutateSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            variable_mutate_word_label->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->variableMutateSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) variable_mutate_word_label->changeColour(Qt::red);
        else
        {
            variable_mutate_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("VariableMutate", s);
        }
    });

    //RJG - variable breed
    SysLabel *variable_breed_word_label = new SysLabel("Variable Breed: ");
    scrollAreaGrid->addWidget(variable_breed_word_label, 4, 0);
    variable_breed_word_edit = new QLineEdit;
    scrollAreaGrid->addWidget(variable_breed_word_edit, 4, 1);
    variable_breed_word_edit->setText(simulationManager->variableBreedSystem->returnGenomeWordInUseString());
    wordsInUse.insert("breed", simulationManager->variableBreedSystem->returnGenomeWordInUseString());
    connect(variable_breed_word_edit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->variableBreedSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            variable_breed_word_label->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->variableBreedSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) variable_breed_word_label->changeColour(Qt::red);
        else
        {
            variable_breed_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("VariableBreed", s);
        }
    });

    //RJG - pathogens
    SysLabel *pathogen_word_label = new SysLabel("Pathogens: ");
    scrollAreaGrid->addWidget(pathogen_word_label, 5, 0);
    pathogen_word_edit = new QLineEdit;
    scrollAreaGrid->addWidget(pathogen_word_edit, 5, 1);
    pathogen_word_edit->setText(simulationManager->pathogensSystem->returnGenomeWordInUseString());
    wordsInUse.insert("Pathogens", simulationManager->pathogensSystem->returnGenomeWordInUseString());
    connect(pathogen_word_edit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->pathogensSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            pathogen_word_label->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->pathogensSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) pathogen_word_label->changeColour(Qt::red);
        else
        {
            pathogen_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Pathogens", s);
        }
    });

    //RJG Species ID system
    SysLabel *species_ID_word_label = new SysLabel("Species ID: ");
    scrollAreaGrid->addWidget(species_ID_word_label, 6, 0);
    species_ID_word_edit = new QLineEdit;
    scrollAreaGrid->addWidget(species_ID_word_edit, 6, 1);
    species_ID_word_edit->setText(simulationManager->speciesIDSytem->returnGenomeWordInUseString());
    wordsInUse.insert("SpeciesID", simulationManager->speciesIDSytem->returnGenomeWordInUseString());
    connect(species_ID_word_edit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->speciesIDSytem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            species_ID_word_label->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->speciesIDSytem->setGenomeWordsFromString(s, genomeSizeSpin->value())) species_ID_word_label->changeColour(Qt::red);
        else
        {
            species_ID_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("SpeciesID", s);
        }
    });

    //RJG - interactions
    SysLabel *interactions_word_label = new SysLabel("Interactions: ");
    scrollAreaGrid->addWidget(interactions_word_label, 7, 0);
    interactions_word_edit = new QLineEdit;
    scrollAreaGrid->addWidget(interactions_word_edit, 7, 1);
    interactions_word_edit->setText(simulationManager->interactionSystem->returnGenomeWordInUseString());
    wordsInUse.insert("Interactions", simulationManager->interactionSystem->returnGenomeWordInUseString());
    connect(interactions_word_edit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->interactionSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            interactions_word_label->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->interactionSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) interactions_word_label->changeColour(Qt::red);
        else
        {
            interactions_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Interactions", s);
        }
    });


    //RJG - visualisation
    SysLabel *visualisations_word_label = new SysLabel("Visualisation: ");
    scrollAreaGrid->addWidget(visualisations_word_label, 8, 0);
    visualisations_word_edit = new QLineEdit;
    scrollAreaGrid->addWidget(visualisations_word_edit, 8, 1);
    visualisations_word_edit->setText(simulationManager->visualisationSystem->returnGenomeWordInUseString());
    wordsInUse.insert("Visualisation", simulationManager->visualisationSystem->returnGenomeWordInUseString());
    connect(visualisations_word_edit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->visualisationSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            visualisations_word_label->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->visualisationSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) visualisations_word_label->changeColour(Qt::red);
        else
        {
            visualisations_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Visualisations", s);
        }
    });

    //RJG - visualisation
    SysLabel *visualisations_word_label_2 = new SysLabel("Visualisation 2: ");
    scrollAreaGrid->addWidget(visualisations_word_label_2, 9, 0);
    visualisations_word_edit_2 = new QLineEdit;
    scrollAreaGrid->addWidget(visualisations_word_edit_2, 9, 1);
    visualisations_word_edit_2->setText(simulationManager->visualisationSystem2->returnGenomeWordInUseString());
    wordsInUse.insert("Visualisation2", simulationManager->visualisationSystem2->returnGenomeWordInUseString());
    connect(visualisations_word_edit_2, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->visualisationSystem2->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            visualisations_word_label_2->changeColour(Qt::red);
            return;
        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->visualisationSystem2->setGenomeWordsFromString(s, genomeSizeSpin->value())) visualisations_word_label_2->changeColour(Qt::red);
        else
        {
            visualisations_word_label_2->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Visualisations2", s);
        }
    });

    SysLabel *hgt_word_label = new SysLabel("Transformable words");
    scrollAreaGrid->addWidget(hgt_word_label, 11, 0);
    hgt_word_edit_1 = new QLineEdit;
    scrollAreaGrid->addWidget(hgt_word_edit_1, 11, 1);
    hgt_word_edit_1->setText(simulationManager->hgtSystem->returnGenomeWordInUseString());
    wordsInUse.insert("Hgt", simulationManager->hgtSystem->returnGenomeWordInUseString());

    connect(hgt_word_edit_1, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->hgtSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            hgt_word_label->changeColour(Qt::red);
            return;

        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->hgtSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) hgt_word_label->changeColour(Qt::red);
        else
        {
            hgt_word_label->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Hgt", s);
        }
        //qDebug()<<simulationManager->hgtSystem->returnGenomeWordInUseString();
    });


    SysLabel *hgt_word_label_1 = new SysLabel("Variable HGT probability: ");
    scrollAreaGrid->addWidget(hgt_word_label_1, 12, 0);
    variable_hgt_word_edit_1 = new QLineEdit;
    scrollAreaGrid->addWidget(variable_hgt_word_edit_1, 12, 1);
    variable_hgt_word_edit_1->setText(simulationManager->variableHgtProbSystem->returnGenomeWordInUseString());
    wordsInUse.insert("Hgtprobability", simulationManager->variableHgtProbSystem->returnGenomeWordInUseString());
    connect(variable_hgt_word_edit_1, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->variableHgtProbSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            hgt_word_label_1->changeColour(Qt::red);
            return;

        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->variableHgtProbSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) hgt_word_label_1->changeColour(Qt::red);
        else
        {
            hgt_word_label_1->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Hgtprobability", s);
        }
        qDebug()<<simulationManager->variableHgtProbSystem->returnGenomeWordInUseString();
    });

    SysLabel *hgt_word_label_2 = new SysLabel("Variable HGT Transfer Length: ");
    scrollAreaGrid->addWidget(hgt_word_label_2, 13, 0);
    variable_hgt_word_edit_2 = new QLineEdit;
    scrollAreaGrid->addWidget(variable_hgt_word_edit_2, 13, 1);
    variable_hgt_word_edit_2->setText(simulationManager->variableHgtLenSystem->returnGenomeWordInUseString());
    wordsInUse.insert("Hgttransferlength", simulationManager->variableHgtLenSystem->returnGenomeWordInUseString());
    connect(variable_hgt_word_edit_2, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->variableHgtLenSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            hgt_word_label_2->changeColour(Qt::red);
            return;

        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->variableHgtLenSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) hgt_word_label_2->changeColour(Qt::red);
        else
        {
            hgt_word_label_2->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Hgttransferlength", s);
        }
    });

    SysLabel *hgt_word_label_3 = new SysLabel("Variable HGT ID Matching: ");
    scrollAreaGrid->addWidget(hgt_word_label_3, 14, 0);
    variable_hgt_word_edit_3 = new QLineEdit;
    scrollAreaGrid->addWidget(variable_hgt_word_edit_3, 14, 1);
    variable_hgt_word_edit_3->setText(simulationManager->variableHgtIdSystem->returnGenomeWordInUseString());
    wordsInUse.insert("Hgttransferid", simulationManager->variableHgtIdSystem->returnGenomeWordInUseString());
    connect(variable_hgt_word_edit_3, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        int max = simulationManager->variableHgtIdSystem->returnMaxFromString(s);
        if (max < 0 || max > MAX_GENOME_WORDS)
        {
            hgt_word_label_3->changeColour(Qt::red);
            return;

        }
        //+1 here due to zero counting - i.e. if we want to be using word 3, we need to be using 4 words.
        else if (max + 1 > genomeSizeSpin->value())genomeSizeSpin->setValue(max + 1);

        if (!simulationManager->variableHgtIdSystem->setGenomeWordsFromString(s, genomeSizeSpin->value())) hgt_word_label_3->changeColour(Qt::red);
        else
        {
            hgt_word_label_3->changeColourTimer(Qt::darkGreen, 1000);
            wordsInUse.insert("Hgttransferid", s);
        }
    });

    //RJG - need to have a widget as part of this stack so the scroll area can allow you to scroll over the widget
    QWidget *widget = new QWidget();
    scrollArea->setWidget(widget);
    widget->setLayout(scrollAreaGrid);
    return scrollArea;
}



/*!
 * \brief MainWindow::createSimulationSettingsDock
 * \return QDockWidget simulationSettingsDock
 */
QDockWidget *MainWindow::createLogSettingsDock()
{
    logSettingsDock = new QDockWidget("Running log", this);
    logSettingsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    logSettingsDock->setFeatures(QDockWidget::DockWidgetMovable);
    logSettingsDock->setFeatures(QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::LeftDockWidgetArea, logSettingsDock);

    auto *logSettingsGrid = new QGridLayout;
    logSettingsGrid->setAlignment(Qt::AlignTop);

    loggingCheckbox = new QCheckBox("Write to file");
    loggingCheckbox->setChecked(simulationManager->simulationSettings->logging);
    loggingCheckbox->setToolTip("<font>Turn on/off this option to write to a text log file every refresh/poll.</font>");
    logSettingsGrid->addWidget(loggingCheckbox, 0, 1);
    connect(loggingCheckbox, &QCheckBox::stateChanged, mainWindow, [ = ](const bool & i)
    {
        simulationManager->simulationSettings->logging = i;
    });

    QPushButton *validateButton = new QPushButton("Validate logs");
    validateButton->setToolTip("<font>Click here to validate the logs - possible errors will appear in red.</font>");
    validateButton->setObjectName("validateButton");
    logSettingsGrid->addWidget(validateButton, 1, 1);
    connect(validateButton, &QPushButton::clicked, logSettingsDock, [ = ]()
    {
        QString validatedText;
        validatedText = simulationManager->simulationLog->validateLog(mainWindow->headerTextEdit->toHtml(), LOG_HEADER);
        mainWindow->headerTextEdit->setHtml(validatedText);

        validatedText = simulationManager->simulationLog->validateLog(mainWindow->iterationTextEdit->toHtml(), LOG_ITERATION);
        mainWindow->iterationTextEdit->setHtml(validatedText);

        validatedText = simulationManager->simulationLog->validateLog(mainWindow->logTextEdit->toHtml(), LOG_SPECIES);
        mainWindow->logTextEdit->setHtml(validatedText);
    });

    QPushButton *instructionsButton = new QPushButton("Instructions");
    instructionsButton->setObjectName("instructionsButton");
    instructionsButton->setToolTip("<font>Click here for logging instructions.</font>");
    logSettingsGrid->addWidget(instructionsButton, 0, 2);
    connect(instructionsButton, &QPushButton::clicked, logSettingsDock, [ = ]()
    {
        QString outString;
        QTextStream out(&outString);
        out << "This docker allows you to enter text that is modified and then output for REvoSim's log.\n\n"
            "Keywords within *stars* are replaced with outputs for further analysis when the log is written (e.g. *iteration* is replaced with the current iteration number). A full list of keywords is available in the REvoSim documentation.\n\n"
            "You can define header text (or output this from either of the other pieces of log text: unrecognised keywords will be coloured in red.\n\n"
            "You can then add per-iteration, or per-species, per-iteration outputs. "
            "More details are included in the REvoSim documentation.";
        /*QStringList list;
         QHashIterator<QString, QString> i(simulationManager->simulationLog->replacementText);
         while (i.hasNext())
         {
             i.next();
             list.append(i.key());
         }
         //sort the list
         list.sort();
         out << list.join("\n"); //and append with newline separators into the string;*/
        QMessageBox::information(this, tr("REvoSim - Logging system"), outString, Qt::NonModal);
    });

    QPushButton *commandLineLog = new QPushButton("Command line log file");
    commandLineLog->setObjectName("commandLineLog");
    commandLineLog->setToolTip("<font>Click here to output an XML file that can be loaded from command line.</font>");
    logSettingsGrid->addWidget(commandLineLog, 1, 2);
    connect(commandLineLog, &QPushButton::clicked, logSettingsDock, [ = ]()
    {
        simulationManager->simulationLog->setHeaderTextFromGUI(mainWindow->headerTextEdit->toHtml());
        simulationManager->simulationLog->setIterationTextFromGUI(mainWindow->iterationTextEdit->toHtml());
        simulationManager->simulationLog->setSpeciestTextFromGUI(mainWindow->logTextEdit->toHtml());
        doSavePath();
        simulationManager->simulationLog->writeLogXML(getSavePath());
    });

    QLabel *logSettingsLabel = new QLabel("Header text");
    logSettingsLabel->setStyleSheet("font-weight: bold");
    logSettingsGrid->addWidget(logSettingsLabel, 3, 1, 1, 2);

    QLabel *logSettingsInstructionsLabel = new QLabel("This will be printed at the start of the log.");
    logSettingsInstructionsLabel->setWordWrap(true);
    logSettingsGrid->addWidget(logSettingsInstructionsLabel, 4, 1, 1, 2);

    headerTextEdit = new QTextEdit(logSettingsDock);
    headerTextEdit->setMaximumHeight(150);
    headerTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    logSettingsGrid->addWidget(headerTextEdit, 5, 1, 1, 2);

    QLabel *itSettingsLabel = new QLabel("Iteration text");
    itSettingsLabel->setStyleSheet("font-weight: bold");
    logSettingsGrid->addWidget(itSettingsLabel, 6, 1, 1, 2);

    QLabel *logIterationInstructionsLabel = new QLabel("The text below will be printed every iteration.");
    logSettingsGrid->addWidget(logIterationInstructionsLabel, 7, 1, 1, 2);

    iterationTextEdit = new QTextEdit(logSettingsDock);
    iterationTextEdit->setMaximumHeight(200);
    iterationTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    logSettingsGrid->addWidget(iterationTextEdit, 9, 1, 1, 2);

    QPushButton *headerFromILogButton = new QPushButton("Write header from iteration log");
    headerFromILogButton->setObjectName("headerFromILogButton");
    headerFromILogButton->setToolTip("<font>Write the log header from the tags place in the iteration text box.</font>");
    logSettingsGrid->addWidget(headerFromILogButton, 8, 1, 1, 2);
    connect(headerFromILogButton, &QPushButton::clicked, logSettingsDock, [ = ]()
    {
        headerTextEdit->setText(simulationManager->simulationLog->writeHeaderFromLogText(iterationTextEdit->toPlainText()));
    });

    QLabel *spSettingsLabel = new QLabel("Species text");
    spSettingsLabel->setStyleSheet("font-weight: bold");
    logSettingsGrid->addWidget(spSettingsLabel, 10, 1, 1, 2);

    QLabel *logSpeciesInstructionsLabel = new QLabel("The text below will be every iteration for every species.");
    logSettingsGrid->addWidget(logSpeciesInstructionsLabel, 11, 1, 1, 2);

    logTextEdit = new QTextEdit(logSettingsDock);
    logTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    logSettingsGrid->addWidget(logTextEdit, 13, 1, 1, 2);

    QPushButton *headerFromSLogButton = new QPushButton("Write header from species log");
    headerFromSLogButton->setObjectName("headerFromSLogButton");
    headerFromSLogButton->setToolTip("<font>Write the log header from the tags place in the species text box.</font>");
    logSettingsGrid->addWidget(headerFromSLogButton, 12, 1, 1, 2);
    connect(headerFromSLogButton, &QPushButton::clicked, logSettingsDock, [ = ]()
    {
        simulationManager->simulationLog->CreateReplacementText(); //has to be reset because of reasons (changes to genome length)
        headerTextEdit->setText(simulationManager->simulationLog->writeHeaderFromLogText(logTextEdit->toPlainText()));
    });

    //RJG - placed here so other objects already exist
    QPushButton *defaultLogsButton = new QPushButton("v2.0.0 log");
    defaultLogsButton->setObjectName("defaultLogsButton");
    defaultLogsButton->setToolTip("<font>This will output the text required for the default REvoSim logs from v2.0.0.</font>");
    logSettingsGrid->addWidget(defaultLogsButton, 2, 1);
    connect(defaultLogsButton, &QPushButton::clicked, logSettingsDock, [ = ]()
    {
        simulationManager->simulationLog->csvOutput = false;
        headerTextEdit->clear();
        iterationTextEdit->clear();
        logTextEdit->clear();

        if (simulationManager->simulationLog->csvOutput)
        {
            logTextEdit->setText(simulationManager->simulationLog->printDefaultLogSpeciesText());
            headerFromSLogButton->click();
            QString header = headerTextEdit->toPlainText();
            header.prepend("*printSettings*\n\n");
            headerTextEdit->setText(header);
        }
        else
        {
            logTextEdit->setText(simulationManager->simulationLog->printDefaultLogSpeciesText());
            iterationTextEdit->setText(simulationManager->simulationLog->printDefaultLogIterationText());
            headerTextEdit->setText(simulationManager->simulationLog->printDefaultHeaderText());

        }
    });


    //RJG - placed here so other objects already exist
    QPushButton *defaultLogsButtonCSV = new QPushButton("v2.0.0 CSV log");
    defaultLogsButtonCSV->setObjectName("defaultLogsButtonCSV");
    defaultLogsButtonCSV->setToolTip("<font>This will output the text required for the default REvoSim logs from v2.0.0.</font>");
    logSettingsGrid->addWidget(defaultLogsButtonCSV, 2, 2);
    connect(defaultLogsButtonCSV, &QPushButton::clicked, logSettingsDock, [ = ]()
    {
        simulationManager->simulationLog->csvOutput = true;
        headerTextEdit->clear();
        iterationTextEdit->clear();
        logTextEdit->clear();

        logTextEdit->setText(simulationManager->simulationLog->printDefaultLogSpeciesText());
        headerFromSLogButton->click();
        QString header = headerTextEdit->toPlainText();
        header.prepend("*printSettings*\n\n");
        headerTextEdit->setText(header);
    });

    QWidget *logSettingsLayoutWidget = new QWidget;
    logSettingsLayoutWidget ->setLayout(logSettingsGrid);
    logSettingsDock->setWidget(logSettingsLayoutWidget);

    return logSettingsDock;
}

/*!
 * \brief MainWindow::udpateGlobalPath
 *
 * Change the save globalSavePath for various stuff.
 */
void MainWindow::udpateGlobalPath()
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
 * Adds a null loop to the simulation simulationManager->iteration run when pause button/command is descendantsd
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
    refreshEnvironment();
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

    QString environmentSceneValue = QString("%1/%2").arg(simulationManager->env->returnCurrentFileNumber() + 1).arg(simulationManager->env->returnFileListCount());
    ui->LabelEnvironment->setText(environmentSceneValue);
}

/*!
 * \brief MainWindow::on_actionReseed_triggered
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
 * \brief MainWindow::startSimulation()
 *
 * Triggered simulation "Run" action.
 */
void MainWindow::startSimulation()
{

    if (simulationManager->env->returnCurrentFileNumber() == -1)
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
        if (ui->actionGo_Slow->isChecked())Sleeper::msleep(30);

        //ARTS - set Stop flag to returns true if reached end... but why? It will fire the finishRun() function at the end.
        if (simulationManager->iterate(simulationManager->simulationSettings->environmentMode, simulationManager->simulationSettings->environmentInterpolate)) stopFlag = true;

        if (!simulationManager->aliveCount) simulationDead();
    }
    finishRun();
}

/*!
 * \brief MainWindow::startSimulation
 *
 * Triggered simulation "Run For..." action.
 *
 * simulationManager->iterations - if 0 (default) it pops up a dialog
 */
void MainWindow::runForNSimulation(int iterations)
{
    if (simulationManager->env->returnCurrentFileNumber() == -1)
    {
        QMessageBox::critical(nullptr, "", "Cannot start simulation without environment");
        if (!loadEnvironmentFiles())
        {
            return;
        }
    }


    bool ok = false;
    int i;
    int numIterations;
    if (iterations == 0) //normal usage - no simulationManager->iterations passed - do a dialog
    {

        numIterations = QInputDialog::getInt(this, "", tr("Iterations: "), 1000, 1, 10000000, 1, &ok);
        i = numIterations;
        if (!ok) return;
    }
    else // iteration count passed (from command line) - use this
    {
        numIterations = i = iterations;
    }

    ui->LabelBatch->setText(tr("1/1"));

    runSetUp();

    int count = 0;

    while (!stopFlag && i > 0 && count < 1000)
    {
        while (pauseFlag)
        {
            waitUntilPauseSignalIsEmitted();
            pauseFlag = false;
        }

        report();
        qApp->processEvents();

        if (simulationManager->iterate(simulationManager->simulationSettings->environmentMode, simulationManager->simulationSettings->environmentInterpolate))stopFlag = true;
        if (!simulationManager->aliveCount)
        {
            simulationDead();
            //RJG - need to reset the number of iterations if everything dies
            numIterations = i = iterations;
            //RJG - but we probably want to avoid infinite runs if nothing can live
            count ++;
        }
        i--;
    }


    if (autoWriteLogCheckbox->isChecked()) writeRunData();
    if (autoWriteLogIndividualsCheckbox->isChecked()) writeIndividualsData();

    if (iterations == 0) // do 'finished' popups only if no iteration count was passed, i.e. NOT if command line run
    {
        //ARTS Show finish message and run FinshRun()
        if (!stopFlag)
        {
            QMessageBox::information(nullptr, tr("Run For... Finished"), tr("The run for %1 iterations has finished.").arg(numIterations));
            finishRun();
        }
        else if (count == 1000)
        {
            QMessageBox::information(nullptr, tr("Simulations dead"), tr("It looks like no simulations could survive with these settings. We tried 1000 times. Change setting?"));
            finishRun();
        }
        else
        {
            QMessageBox::information(nullptr, tr("Run For... Stopped"), tr("The run for %1 iterations has been stopped at iteration %2.").arg(
                                         numIterations).arg(numIterations - i));
            finishRun();
        }
    }
    else
    {
        if (count == 1000) qInfo() << "REvoSim tried 1000 simulations, and none stuck. "
                                       "It looks like your settings are not conducive to the run surviving. Please try again with different simulation settings.";
        else qInfo() << "The run of" << numIterations << "iterations has finished.";
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
    int environmentStart = simulationManager->env->returnCurrentFileNumber();
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
    if (index != -1)   // -1 for not found
    {
        environmentComboBox->setCurrentIndex(index);
    }
    QString environmentLabel = QString(tr("Would you like the environment to repeat with each batch?"));
    form.addRow(environmentLabel, environmentComboBox);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
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
            simulationManager->env->setCurrentFileNumber(environmentStart);
            simulationManager->env->reset(0);
        }

        //And run...
        ui->LabelBatch->setText(tr("%1/%2").arg((batchRuns + 1)).arg(batchTargetRuns));

        if (simulationManager->env->returnCurrentFileNumber() == -1)
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

            simulationManager->iterate(simulationManager->simulationSettings->environmentMode, simulationManager->simulationSettings->environmentInterpolate);
            if (!simulationManager->aliveCount) break;
            i--;
        }

        if (autoWriteLogCheckbox->isChecked()) writeRunData();
        if (autoWriteLogIndividualsCheckbox->isChecked()) writeIndividualsData();
        batchRuns++;

        if (!stopFlag && batchRuns < batchTargetRuns)
        {
            resetSimulation();
        }

        //RJG - Reset masks
        //simulationManager->MakeLookups();
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

void MainWindow::simulationDead()
{
    nextRefresh = 1;
    report();

    QString messageText("It looks like nothing is alive in your simulation. This could be due to the settings you are using, or just bad luck. The simulation will terminate and reseed for you.");

    if (!autoFromCommand)
    {
        QMessageBox *warningBox = new QMessageBox;
        //Delete when closed so no memory leak
        warningBox->setAttribute(Qt::WA_DeleteOnClose, true);
        warningBox->setWindowTitle(tr("Stopping simulation"));
        warningBox->setText(messageText);
        warningBox->show();
        //Close after ten minutes.
        QTimer::singleShot(6000000, warningBox, SLOT(close()));
    }
    else qInfo() << messageText;

    stopFlag = true;

    resetSimulation();
}


/*!
 * \brief MainWindow::pauseSimulation
 *
 * Pause function to halt the simulation mid run and allow restart at same point.
 * Note this disables the Stop button as the Stop function runs outside the iterations loop,
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
        //RJG - Restart time is in development only
        restartTimer();
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

    if (simulationManager->simulationSettings->logging && speciesMode == SPECIES_MODE_NONE)
    {
        if (!autoFromCommand) QMessageBox::warning(this, "Be aware", "Species tracking is off, so the log files won't show species information");
        else qInfo() << "Species tracking is off, so the log files won't show species information";
    }

    doSavePath();

    restartTimer();

    nextRefresh = refreshRate;

    if (loggingCheckbox->isChecked()) writeLog();
}

void MainWindow::doSavePath()
{
    //Sort out globalSavePath
    QString savePathString(getSavePath());
    if (!savePathString.endsWith(QDir::separator())) globalSavePath->setText(savePathString + QDir::separator());
    QDir globalSavePathStr(getSavePath());
    if (!globalSavePathStr.exists())
    {
        if (!autoFromCommand)QMessageBox::warning(this, "Error!", "The program doesn't think the save directory exists, so is going to default back to the direcctory in which the executable is.");
        else qInfo() << "Error - The program doesn't think the save directory exists, so is going to default back to the direcctory in which the executable is.";
        QString programPath(QCoreApplication::applicationDirPath());
        programPath.append(QDir::separator());
        globalSavePath->setText(programPath);
        globalSavePathStr.setPath(programPath);
    }
    //RJG - Set up save directory
    if (!getSavePath().endsWith(QString(PRODUCTNAME) + "_output" + QDir::separator()))
    {
        if (!globalSavePathStr.mkpath(QString(PRODUCTNAME) + "_output" + QDir::separator()))
        {
            if (!autoFromCommand) QMessageBox::warning(this, "Error", "Cant save images. Permissions issue?");
            else qInfo() << "Error - Cant save images. Permissions issue?";
            return;
        }
        globalSavePath->setText(globalSavePathStr.absolutePath() + QDir::separator() + QString(PRODUCTNAME) + "_output" + QDir::separator());
    }
}

void MainWindow::restartTimer()
{
    timer.restart();
    timingqueue.clear();
    timingqueue.append(0); //first timing item is 0
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

    if (getSavePath().endsWith("REvoSim_output/"))globalSavePath->setText(getSavePath().remove("REvoSim_output/"));
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
    if (simulationManager->aliveCount == 0) return;
    if (--nextRefresh > 0) return;

    nextRefresh = refreshRate;

    QString s;
    QTextStream sout(&s);

    float atime = (timePerFrame());

    quint64 totalfit = 0;
    for (int n2 = 0; n2 < simulationManager->simulationSettings->gridX; n2++)
        for (int m2 = 0; m2 < simulationManager->simulationSettings->gridY; m2++)
        {
            totalfit += static_cast<quint64>(totalFitness[n2][m2]);
        }

    double t = static_cast<double>(totalfit);
    t /= static_cast<double>(simulationManager->aliveCount);
    t /= static_cast<double>(simulationManager->cellSettingsMaster->settleTolerance);
    t *= 100; //now %age

    QString out;

    QTextStream o(&out);
    o << simulationManager->iteration; //need to use to avoid int64 descendants
    ui->LabelIteration->setText(out);
    sout << "Iterations: " << out << ". ";

    if (atime > 0) out = QString("%1k").arg((3600000 / static_cast<double>(atime)) / 1000, 0, 'f', 1);
    else out = "-";
    ui->LabelIterationsPerHour->setText(out);
    sout << "Iterations/hour: " << out << ". ";

    if (simulationManager->env->returnCurrentFileNumber() >= simulationManager->env->returnFileListCount()) out = QString("Finished (%1)").arg(simulationManager->env->returnFileListCount());
    else out = QString("%1/%2").arg(simulationManager->env->returnCurrentFileNumber() + 1).arg(simulationManager->env->returnFileListCount());
    ui->LabelEnvironment->setText(out);
    sout << "Environment: " << out << ". ";

    out = QString("%1%").arg(t, 0, 'f', 2);
    ui->LabelFitness->setText(out);
    sout << "Mean fitness: " << out << ". ";

    if (atime > 0)out = QString("%1").arg(static_cast<double>(atime), 0, 'f', 2);
    else out = "-";
    ui->LabelSpeed->setText(out);
    sout << "Milliseconds/iteration: " << out << ". ";

    out = QString("%1").arg(simulationManager->aliveCount);
    ui->LabelCritters->setText(out);
    sout << "Organisms: " << out << ". ";

    calculateSpecies();
    out = "-";
    if (speciesMode != SPECIES_MODE_NONE)
    {
        int g5 = 0, g50 = 0;
        for (int i = 0; i < oldSpeciesList->count(); i++)
        {
            if ((*oldSpeciesList)[i].size > 5) g5++;
            if ((*oldSpeciesList)[i].size > 50) g50++;
        }
        out = QString("%1 (>5:%2 >50:%3)").arg(oldSpeciesList->count()).arg(g5).arg(g50);
    }
    ui->LabelSpecies->setText(out);
    sout << "Species: " << out << ".";

    //RJG - Write stats to console if running from command line (doesn't work on windows due to OS limitations)
    if (autoFromCommand)
    {
        QDebug terminalOut = qInfo();
        //RJG - dont want quotation marks around the output
        terminalOut.noquote();
        terminalOut << s;
    }

    //do species stuff
    if (!simulationManager->simulationSettings->gui)refreshPopulations();
    if (!simulationManager->simulationSettings->gui)refreshEnvironment();

    writeLog();

    //reset the breedAttempts and breedFails arrays
    for (int n2 = 0; n2 < simulationManager->simulationSettings->gridX; n2++)
        for (int m2 = 0; m2 < simulationManager->simulationSettings->gridY; m2++)
        {
            //breedAttempts[n2][m2]=0;
            breedFails[n2][m2] = 0;
            settles[n2][m2] = 0;
            settlefails[n2][m2] = 0;
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
    float ffails = static_cast<float>(fails) / generations;

    ffails *= 100.0; // a fudge factor no less! Well it's only visualization...
    ffails = static_cast<float>(pow(static_cast<double>(ffails), 0.8));

    if (ffails > static_cast<float>(255.0)) ffails = static_cast<float>(255.0);

    return static_cast<int>(ffails);
}

/*!
 * \brief MainWindow::refreshPopulationstimePerFrame
 *
 * Works out a time per iteration from the queue of time values, recording a new one too
 *
 */

float MainWindow::timePerFrame()
{
    int thistime = timer.elapsed();

    timingqueue.append(thistime); //add ms since timer start to queue
    while (timingqueue.length() > 10) timingqueue.dequeue();  //remove from head to keep 10 values

    if (timingqueue.length() == 1) return -1;

    float timeelapsed = static_cast<float>(thistime - timingqueue.head());
    timeelapsed /= static_cast<float>(timingqueue.length() - 1);
    return timeelapsed / static_cast<float>(refreshRate);
}

/*!
 * \brief MainWindow::on_populationWindowComboBox_currentIndexChanged
 * \param index
 *
 * Catches any changes to the population window dropdown.
 */
void MainWindow::on_populationWindowComboBox_currentIndexChanged(int index)
{
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
    QString globalSavePathStr(getSavePath());
    if (!globalSavePathStr.endsWith(QDir::separator()))globalSavePathStr.append(QDir::separator());
    if (batchRunning)
    {
        globalSavePathStr.append(QString("Images_run_%1").arg(batchRuns, 4, 10, QChar('0')));
        globalSavePathStr.append(QDir::separator());
    }
    QDir save_dir(globalSavePathStr);

    //ARTS - Check to see what the mode is from the Population Window QComboBox; return the data as int.
    int currentSelectedMode = ui->populationWindowComboBox->itemData(ui->populationWindowComboBox->currentIndex()).toInt();

    // (0) Population Count
    if (currentSelectedMode == 0 || savePopulationCount->isChecked())
    {
        //Popcount
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {
                int count = 0;
                for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
                {
                    if (critters[n][m][c].age) count++;
                }
                count *= 2;
                if (count > 255) count = 255;
                populationImage->setPixel(n, m, static_cast<uint>(count));
            }
        if (currentSelectedMode == 0)populationItem->setPixmap(QPixmap::fromImage(*populationImage));
        if (savePopulationCount->isChecked())
            if (save_dir.mkpath("population/"))
                populationImage->save(QString(save_dir.path() + "/population/REvoSim_population_it_%1.png").arg(simulationManager->iteration, 7, 10, QChar('0')));
    }

    // (1) Fitness
    if (currentSelectedMode == 1 || saveMeanFitness->isChecked())
    {
        //Popcount
        int multiplier = 255 / (2 * simulationManager->cellSettingsMaster->settleTolerance);
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {
                int count = 0;
                for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
                {
                    if (critters[n][m][c].age) count++;
                }
                if (count == 0)
                    populationImage->setPixel(n, m, 0);
                else
                {
                    int clampedValue = static_cast<uint>((totalFitness[n][m] * static_cast<uint>(multiplier)) / static_cast<uint>(count));
                    if (clampedValue > 255) clampedValue = 255;
                    populationImage->setPixel(n, m, clampedValue);
                }
            }
        if (currentSelectedMode == 1)populationItem->setPixmap(QPixmap::fromImage(*populationImage));
        if (saveMeanFitness->isChecked())
            if (save_dir.mkpath("fitness/"))
                populationImage->save(QString(save_dir.path() + "/fitness/REvoSim_mean_fitness_it_%1.png").arg(simulationManager->iteration, 7, 10, QChar('0')));
    }

    // (2) Visual system 1
    if (currentSelectedMode == 2 || saveVisSysOneAsColour->isChecked())
    {
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
                populationImageColour->setPixelColor(n, m, simulationManager->visualisationSystem->returnRGB(n, m));

        if (currentSelectedMode == 2) populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
        if (saveVisSysOneAsColour->isChecked())
            if (save_dir.mkpath("VS1/"))
                populationImageColour->save(QString(save_dir.path() + "/VS1/REvoSim_VS1_it_%1.png").arg(simulationManager->iteration, 7, 10, QChar('0')));
    }

    // (3) Visual system 2
    if (currentSelectedMode == 3 || saveVisSysTwoAsColour->isChecked())
    {
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
                populationImageColour->setPixelColor(n, m, simulationManager->visualisationSystem2->returnRGB(n, m));

        if (currentSelectedMode == 3)populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
        if (saveVisSysTwoAsColour->isChecked())
            if (save_dir.mkpath("VS2/"))
                populationImageColour->save(QString(save_dir.path() + "/VS2/REvoSim_VS2_it_%1.png").arg(simulationManager->iteration, 7, 10, QChar('0')));
    }
    /*
    // (4) Breed Attempts
    //RJG - No save option as no longer in the menu as an option.
    if (currentSelectedMode == 4)
    {
        //Popcount
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {
                int value = (breedAttempts[n][m] * 10) / refreshRate;
                if (value > 255) value = 255;
                populationImage->setPixel(n, m, static_cast<uint>(value));
            }
        populationItem->setPixmap(QPixmap::fromImage(*populationImage));
    }

    // (5) Breed Fails
    //RJG - No save option as no longer in the menu as an option.
    if (currentSelectedMode == 5)
    {
        //Popcount
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
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
    */
    // (4) Settles
    if (currentSelectedMode == 4 || saveSettles->isChecked())
    {
        //Popcount

        // Workout min and max values
        int max = -1;
        int min = -1;

        // Get min and max values
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
        {
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {
                int value = (settles[n][m] * 10) / refreshRate;
                if (min < 0) min = value;
                if (max < 0) max = value;
                if (value < min) min = value;
                if (value > max) max = value;
            }
        }

        // Scale each value to 0-255
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
        {
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {
                int value = (settles[n][m] * 10) / refreshRate;

                // Scale values ( x - min(x) ) * (smax - smin) / ( max(x) - min(x) ) + smin
                if (max - min > 0) value = (value - min) * 255 / (max - min);
                else value = 0;

                populationImage->setPixel(n, m, static_cast<uint>(value));
            }
        }

        if (currentSelectedMode == 4)populationItem->setPixmap(QPixmap::fromImage(*populationImage));
        if (saveSettles->isChecked())
            if (save_dir.mkpath("settles/"))
                populationImage->save(QString(save_dir.path() + "/settles/REvoSim_settles_it_%1.png").arg(simulationManager->iteration, 7, 10, QChar('0')));
    }

    // (5) Breed/Settle Fails (R=Breed, G=Settle)
    //RJG - this now combines breed fails (red) and settle fails (green)
    if (currentSelectedMode == 5 || saveFailsSettles->isChecked())
    {
        //work out average per simulationManager->iteration
        float generations = static_cast<float>(simulationManager->iteration - static_cast<quint64>(lastReport));

        //Make image
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {
                int r = scaleFails(breedFails[n][m], generations);
                int g = scaleFails(settlefails[n][m], generations);
                populationImageColour->setPixel(n, m, qRgb(r, g, 0));
            }

        if (currentSelectedMode == 5) populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
        if (saveFailsSettles->isChecked())
            if (save_dir.mkpath("breed_settle_fails/"))
                populationImageColour->save(QString(save_dir.path() + "/breed_settle_fails/REvoSim_breed_settle_fails_it_%1.png").arg(simulationManager->iteration, 7, 10, QChar('0')));
    }
    /*
        // (8) Breed Fails 2
        if (currentSelectedMode == 8)
        {
            //Popcount
            for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
                for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
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
    */
    // (7) Species
    if (currentSelectedMode == 7 || saveSpecies->isChecked())   //do visualisation if necessary
    {
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {

                if (totalFitness[n][m] == 0) populationImageColour->setPixel(n, m, 0); //black if square is empty
                else
                {
                    quint64 thisspecies = 0;
                    for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
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

        if (currentSelectedMode == 7)populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
        if (saveSpecies->isChecked())
            if (save_dir.mkpath("species/"))
                populationImageColour->save(QString(save_dir.path() + "/species/REvoSim_species_it_%1.png").arg(simulationManager->iteration, 7, 10, QChar('0')));
    }

    // (10) Interactions
    if (currentSelectedMode == 8)
    {
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
        {
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {
                if (totalFitness[n][m] == 0) populationImage->setPixel(n, m, 0); //black if square is empty
                else
                {
                    unsigned int fitnessSource = 0;
                    for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
                    {
                        if (critters[n][m][c].age > 0)
                        {
                            fitnessSource = static_cast<uint>(((critters[n][m][c].fitness) + 1) * 51 / ((critters[n][m][c].environmentalFitness) + 1)); // Hardcoded multiplier of 51.
                            if (fitnessSource > 255) fitnessSource = 255;
                            break;
                        }
                    }
                    populationImage->setPixel(n, m, fitnessSource);
                }
            }
        }
        populationItem->setPixmap(QPixmap::fromImage(*populationImage));
    }

    // (11) Breed List
    if (currentSelectedMode == 9)
    {
        int BreedListHistLength = 1 + (32 * simulationManager->simulationSettings->genomeSize);
        if (BreedListHistLength == 1) return;

        int BreedListHist[BreedListHistLength];
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
        {
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {
                int Rchannel = 0, Gchannel = 0, Bchannel = 0;

                for (int c = 0; c < BreedListHistLength; c++) BreedListHist[c] = 0;
                for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
                    if (critters[n][m][c].age > 0) BreedListHist[critters[n][m][c].genomeBitCountWithJitter]++;
                int MaxIndex = 0;
                int MaxButOneIndex = 0;
                int MaxButTwoIndex = 0;
                for (int c = 0; c < BreedListHistLength; c++)
                {
                    if (BreedListHist[c] > BreedListHist[MaxIndex])
                    {
                        MaxButTwoIndex = MaxButOneIndex;
                        MaxButOneIndex = MaxIndex;
                        MaxIndex = c;
                    }
                    else if (BreedListHist[c] > BreedListHist[MaxButOneIndex])
                    {
                        MaxButTwoIndex = MaxButOneIndex;
                        MaxButOneIndex = c;
                    }
                    else if (BreedListHist[c] > BreedListHist[MaxButTwoIndex])
                        MaxButTwoIndex = c;
                }
                Rchannel = static_cast<int>((MaxIndex * 255) / (BreedListHistLength - 1));
                Gchannel = static_cast<int>((MaxButOneIndex * 255) / (BreedListHistLength - 1));
                Bchannel = static_cast<int>((MaxButTwoIndex * 255) / (BreedListHistLength - 1));

                populationImageColour->setPixel(n, m, qRgb(Rchannel, Gchannel, Bchannel));
            }
        }
        populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
    }

    // (12) Pathogen - "coding" genome as colour
    if (currentSelectedMode == 10)
    {
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
                populationImageColour->setPixelColor(n, m, simulationManager->pathogensSystem->returnRGB(n, m, 0));
        populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
    }

    // (13) Pathogen - "non-coding" genome as colour
    if (currentSelectedMode == 11)
    {
        //RJG - work out genome - depends on mode
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
                populationImageColour->setPixelColor(n, m, simulationManager->pathogensSystem->returnRGB(n, m, 1));
        populationItem->setPixmap(QPixmap::fromImage(*populationImageColour));
    }

    // (14) Energy Theft
    if (currentSelectedMode == 12)   //do visualisation if necessary
    {
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
        {
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            {

                if (totalFitness[n][m] == 0) populationImageColour->setPixel(n, m, 0); //black if square is empty
                else
                {
                    quint64 proportionEnergyStolen = 0;
                    for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
                    {
                        if (critters[n][m][c].age > 0)
                        {
                            proportionEnergyStolen = static_cast<uint>((critters[n][m][c].stolenEnergy) * 255 / ((critters[n][m][c].lifetimeEnergy) + 1)); // Hardcoded multiplier of 255.
                            if (proportionEnergyStolen > 255) proportionEnergyStolen = 255;
                            break;
                        }
                    }

                    populationImage->setPixel(n, m, proportionEnergyStolen);
                }
            }
            populationItem->setPixmap(QPixmap::fromImage(*populationImage));
        }
    }

    lastReport = static_cast<int>(simulationManager->iteration);
}

/*!
 * \brief MainWindow::refreshEnvironment
 *
 * Refreshes the environment window.
 */
void MainWindow::refreshEnvironment()
{
    QString globalSavePathStr(getSavePath());
    if (!globalSavePathStr.endsWith(QDir::separator()))globalSavePathStr.append(QDir::separator());
    if (batchRunning)
    {
        globalSavePathStr.append(QString("Images_run_%1").arg(batchRuns, 4, 10, QChar('0')));
        globalSavePathStr.append(QDir::separator());
    }
    QDir save_dir(globalSavePathStr);

    int currentSelectedMode = ui->environmentWindowComboBox->itemData(ui->environmentWindowComboBox->currentIndex()).toInt();

    ImageSequence *render = simulationManager->env;
    if (currentSelectedMode != 0)
        for (auto &l : simulationManager->linkages)
            if (l.variable == ui->environmentWindowComboBox->currentText())
                render = l.imageSequence;

    for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
        for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
        {
            quint8 *environment = render->returnRGB(n, m);
            if (currentSelectedMode == 0) environmentImage->setPixel(n, m, qRgb(environment[0], environment[1], environment[2]));
            //RJG - Linkage therefore only use red channel
            else environmentImage->setPixel(n, m, qRgb(environment[0], environment[0], environment[0]));
        }

    environmentItem->setPixmap(QPixmap::fromImage(*environmentImage));
    if (saveEnvironment->isChecked())
        if (save_dir.mkpath("environment/"))
            environmentImage->save(QString(save_dir.path() + "/environment/REvoSim_environment_it_%1.png").arg(simulationManager->iteration, 7, 10, QChar('0')));
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
    if (!autoFromCommand)
    {
        if (dontUpdate
                && QMessageBox::question(nullptr, "Heads up", "If you don't update the GUI, images will also not be saved. OK?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
        {
            guiCheckbox->setChecked(false);
            return;
        }
    }

    simulationManager->simulationSettings->gui = dontUpdate;
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
    saveVisSysOneAsColour->setChecked(all);
    saveVisSysTwoAsColour->setChecked(all);
    saveSpecies->setChecked(all);
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
    if (batchRunning)simulationManager->simulationLog->writeRunData(getSavePath(), batchRuns);
    else simulationManager->simulationLog->writeRunData(getSavePath(), -1);
}


/*!
 * \brief MainWindow::writeRunData
 *
 * At end of run in run for/batch mode, or on click when a run is going, this allows user to
 * output the final log, along with the tree for the run.
 */
void MainWindow::writeIndividualsData()
{

    doSavePath();
    simulationManager->simulationLog->writeLog(getSavePath(), -1, LOG_DUMP_INDIVIDUALS);
    if (getSavePath().endsWith("REvoSim_output/"))globalSavePath->setText(getSavePath().remove("REvoSim_output/"));
}


/*!
 * \brief MainWindow::simulationManager->simulationSettings->environmentModeChanged
 * \param changesimulationManager->simulationSettings->environmentMode
 * \param updateGUI
 *
 * Sets the environment mode on change.
 */
void MainWindow::environmentModeChanged(int changeEnvironmentMode, bool updateGUI)
{
    int newEnvironmentMode = ENV_MODE_STATIC;
    if (changeEnvironmentMode == ENV_MODE_ONCE)
        newEnvironmentMode = ENV_MODE_ONCE;
    if (changeEnvironmentMode == ENV_MODE_LOOP)
        newEnvironmentMode = ENV_MODE_LOOP;
    if (changeEnvironmentMode == ENV_MODE_BOUNCE)
        newEnvironmentMode = ENV_MODE_BOUNCE;

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

    simulationManager->simulationSettings->environmentMode = static_cast<quint8>(newEnvironmentMode);
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
    if (changeSpeciesMode == SPECIES_MODE_PHYLOGENY) newSpeciesMode = SPECIES_MODE_PHYLOGENY;
    if (changeSpeciesMode == SPECIES_MODE_PHYLOGENY_AND_METRICS) newSpeciesMode = SPECIES_MODE_PHYLOGENY_AND_METRICS;
    if (changeSpeciesMode == SPECIES_MODE_BASIC) newSpeciesMode = SPECIES_MODE_BASIC;

    //some changes not allowed
    if (simulationManager->iteration != static_cast<quint64>(0))
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
    for (int c = 0; c < SLOTS_PER_GRID_SQUARE; c++)
        critters[n][m][c].age = 0;

    totalFitness[n][m] = 0;

    breedAttempts[n][m] = 0;
    breedSuccess[n][m] = 0;
    breedFails[n][m] = 0;
    settles[n][m] = 0;
    settlefails[n][m] = 0;
}

/*!
 * \brief MainWindow::resizeImageObjects
 */
void MainWindow::resizeImageObjects()
{
    delete populationImage;
    delete environmentImage;
    delete populationImageColour;

    populationImage = new QImage(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY, QImage::Format_Indexed8);
    QVector<QRgb> clut(256);
    for (int ic = 0; ic < 256; ic++) clut[ic] = qRgb(ic, ic, ic);
    populationImage->setColorTable(clut);

    environmentImage = new QImage(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY, QImage::Format_RGB32);

    populationImageColour = new QImage(simulationManager->simulationSettings->gridX, simulationManager->simulationSettings->gridY, QImage::Format_RGB32);
}

/*!
 * \brief MainWindow::redoImages
 * \param oldRows
 * \param oldColumns
 */
void MainWindow::redoImages(int oldRows, int oldColumns)
{
    //check that the maxUsed's are in the new range
    for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
        for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
            if (maxUsed[n][m] >= simulationManager->cellSettingsMaster->slotsPerSquare) maxUsed[n][m] = simulationManager->cellSettingsMaster->slotsPerSquare - 1;

    //If either rows or cols are bigger - make sure age is set to 0 in all critters in new bit!
    if (simulationManager->simulationSettings->gridX > oldRows)
    {
        for (int n = oldRows; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = 0; m < simulationManager->simulationSettings->gridY; m++)
                resetSquare(n, m);
    }
    if (simulationManager->simulationSettings->gridY > oldColumns)
    {
        for (int n = 0; n < simulationManager->simulationSettings->gridX; n++)
            for (int m = oldColumns; m < simulationManager->simulationSettings->gridY; m++)
                resetSquare(n, m);
    }

    resizeImageObjects();
    refreshPopulations();
    refreshEnvironment();
    resize();
}

/*!
 * \brief MainWindow::updateSettingsDockVisibility
 *
 * Triggered action to open/close the Settings Dock.
 */
void MainWindow::updateSettingsDockVisibility()
{
    if (simulationSettingsDock->isVisible())
    {
        simulationSettingsDock->hide();
        organismSettingsDock->hide();
        interactionSettingsDock->hide();
        settingsButton->setChecked(false);
        ui->actionSettings_Dock->setChecked(false);
    }
    else
    {
        simulationSettingsDock->show();
        organismSettingsDock->show();
        interactionSettingsDock->show();
        settingsButton->setChecked(true);
        ui->actionSettings_Dock->setChecked(true);
    }
}

/*!
 * \brief MainWindow::updateOutputDockVisibility
 *
 * Triggered action to open/close the Settings Dock.
 */
void MainWindow::updateOutputDockVisibility()
{
    if (outputSettingsDock->isVisible())
    {
        outputSettingsDock->hide();
        logSettingsDock->hide();
        logButton->setChecked(false);
        ui->actionOutput_Dock->setChecked(false);
    }
    else
    {
        outputSettingsDock->show();
        logSettingsDock->show();
        logButton->setChecked(true);
        ui->actionOutput_Dock->setChecked(true);
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

    QString count_peaks_file(getSavePath());
    if (!count_peaks_file.endsWith(QDir::separator()))count_peaks_file.append(QDir::separator());
    count_peaks_file.append(QString(PRODUCTNAME) + "_count_peaks.txt");
    QFile outputfile(count_peaks_file);
    outputfile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&outputfile);

    out << "REvoSim Peak Counting ";
    QDateTime t(QDateTime::currentDateTime());
    out << t.toString(Qt::ISODate) << "\n\n===================\n\n";
    out << "\nBelow is a histogram showing fitnesses for a large number of randomly selected genomes under the user-defined RGB levels.\n";
    out << "\n\n===================\n\n";
    out << peaks;

    outputfile.close();
}

/*!
 * \brief MainWindow::addLinkage
 * \return
 * Launch dialogue to implement a link between 8 bit rasters and any simulation settings
 */
void MainWindow::addLinkage()
{
    int count = 0;
    for (auto &l : simulationManager->linkages) if (!l.set)count++;
    if (count == 0)
    {
        if (!autoFromCommand) QMessageBox::warning(this, "All linkages assigned", "All currently implemented linkages are assigned - if you would like more to be added, please email RJG");
        else qInfo() << "All currently implemented linkages are assigned - if you would like more to be added, please email RJG";
        return;
    }

    //RJG - New linkage to send to dialogue (this allows same dialogue to be used for editing)
    linkageClass localLinkage;
    linkagesDialog d(&localLinkage, simulationManager, this);

    //RJG - execute dilaogue, and assign linkage if required
    if (d.exec() != QDialog::Accepted) return;

    //RJG set the linkages in sim manager - overwrite the temp one stored in constructor
    QString variableToSet(localLinkage.variable);
    simulationManager->linkages.insert(variableToSet, localLinkage);
    linkagesGUI();

}

/*!
 * \brief MainWindow::linkagesGUI
 * \return
 * Update mainwindow GUI to reflect linkages, and also set if they are on or not in simulationManager
 */
void MainWindow::linkagesGUI()
{
    int count = 0;
    linkagesList->clear();

    int currentIndex = ui->environmentWindowComboBox->currentIndex();

    //RJG - Environment window dropdown - only one option here, others are created alongside the linkages
    ui->environmentWindowComboBox->clear();
    ui->environmentWindowComboBox->addItem("Environment", QVariant(0));
    for (auto &l : simulationManager->linkages)
        if (l.set)
        {
            count++;
            QString outString;
            QTextStream out(&outString);
            QStringList files(l.imageSequence->returnFileList());
            QFileInfo file(files[0]);
            out << "Linkage " << count << ": variable " << l.variable << " is defined by image stack in the folder " << file.path() << "\n\n";
            linkagesList->addItem(outString);
            //RJG - add to visualisation combo box
            ui->environmentWindowComboBox->addItem(l.variable, QVariant(count));
        }
    if (count > 0) simulationManager->simulationSettings->linkagesOn = true;
    else simulationManager->simulationSettings->linkagesOn = false;
    //RJG - visualise environment by default
    if (currentIndex > ui->environmentWindowComboBox->count())currentIndex = 0;
    ui->environmentWindowComboBox->setCurrentIndex(currentIndex);
    refreshEnvironment();
}

void MainWindow::linkagesListClicked()
{
    QString var(linkagesList->currentItem()->text());
    int end = var.indexOf(" is defined");
    var = var.mid(20, end - 20);

    linkageClass editLinkage;
    editLinkage = simulationManager->linkages.value(var);
    linkagesDialog d(&editLinkage, this);
    if (d.exec() != QDialog::Accepted) return;

    //RJG set the linkages in sim manager
    //QString variableToSet(editLinkage.variable);
    simulationManager->linkages.insert(var, editLinkage);
    linkagesGUI();
}

/*!
 * \brief MainWindow::editInteractionsGrid
 * \return
 * Launch dialogue to implement edit of interactions grid
 */
void MainWindow::editInteractionsGrid()
{
    QDialog interactionsDialogue(this);
    interactionsDialogue.setWindowTitle("REvoSim interactions");
    QGridLayout interactionsGrid(&interactionsDialogue);

    QLabel lab("Please enter your a priori interactions grid here:");
    interactionsGrid.addWidget(&lab, 0, 0, 1, 4, Qt::AlignJustify);

    // Add spin boxes
    QList <QSpinBox *> spins;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            QSpinBox *spin = new QSpinBox(&interactionsDialogue);
            spin->setMinimum(-32);
            spin->setMaximum(32);
            spin->setValue(simulationManager->simulationSettings->a_priori_interaction[i][j]);
            interactionsGrid.addWidget(spin, i + 1, j);
            spins << spin;
        }
    }

    // Add standard buttons
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &interactionsDialogue);
    interactionsGrid.addWidget(&buttonBox, 6, 1, 1, 4);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &interactionsDialogue, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &interactionsDialogue, SLOT(reject()));

    if (interactionsDialogue.exec() == QDialog::Accepted)
    {
        int count = 0;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
            {
                simulationManager->simulationSettings->a_priori_interaction[i][j] = spins[count]->value();
                count++;
            }

        //Call makeinteractionlookups in simulationmanager to update the interactions array from the a priori interactions grid
        simulationManager->makeInteractionLookups();
    }
}

/*!
* \brief MainWindow::loadEnvironmentFiles
* \return bool
*
* Action to allow new environmental files to be loaded from the local filesystem.
* Optional folder argument skips the dialog - used by command line system
*/
bool MainWindow::loadEnvironmentFiles(QString folder)
{
    QStringList files;
    //RJG - Select files
    if (folder == "")
    {
        QString desktop = QString(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
        files = QFileDialog::getOpenFileNames(this, "Select one or more image files to load as the environment for your simulation.", desktop, "Images (*.png *.bmp)");
    }
    else
    {
        //Get list of all image files from folder
        QDir d(folder);
        QStringList filters;
        filters << "*.bmp" << "*.png";
        d.setNameFilters(filters);
        d.setSorting(QDir::Name);
        files = d.entryList();

        //Add on the path
        for (int i = 0; i < files.count(); i++) files[i] = folder + "/" + files[i];
    }

    if (files.length() == 0)
    {
        qInfo() << "******* Failed to load environment files: assuming you're on command line, did you point to the correct folder? Defaulting to a static environment. *******";
        return false;
    }

    bool notsquare = false;
    bool different_size = false;
    for (int i = 0; i < files.length(); i++)
    {
        QImage LoadImage(files[i]);
        int x = LoadImage.width();
        int y = LoadImage.height();
        if (x != y) notsquare = true;
        if (x != 100 || y != 100) different_size = true;
    }
    if ((notsquare || different_size) && !autoFromCommand)
        QMessageBox::warning(
            this,
            "FYI",
            "For speed REvoSim currently has static arrays for the environment, which limits out of the box functionality to 100 x 100 square environments. "
            "It looks like some of your Environment images don't meet this requirement. Anything smaller than 100 x 100 will be stretched (irrespective of aspect ratio) to 100x100. Anything bigger, and we'll use the top left corner. Should you wish to use a different size environment, please email RJG or MDS."
        );

    delete (simulationManager->env);
    simulationManager->env = new ImageSequence(files, simulationManager->simulationSettings->environmentChangeRate);

    refreshEnvironment();

    //RJG - Reset for this new environment
    simulationManager->setupRun();

    //ARTS - Update the information bar
    QString environmentSceneValue = QString("%1/%2").arg(simulationManager->env->returnCurrentFileNumber() + 1).arg(simulationManager->env->returnFileListCount());
    ui->LabelEnvironment->setText(environmentSceneValue);

    return true;
}

/*!
 * \brief MainWindow::on_actionSave_triggered
 *
 * Action to save the current settings and simulation to an .revosim file.
 *
 */
void MainWindow::saveSimulation(QString filename)
{

    //RJG - create file
    if (filename.length() < 3) filename = QFileDialog::getSaveFileName(this, tr("Save file as..."), QString(getSavePath() + QString(PRODUCTNAME) + "_simulation.revosim"), "REvoSim files (*.revosim)");
    if (filename.length() == 0) return;
    if (!filename.endsWith(".revosim"))filename.append(".revosim");

    //RJG - save settings associated with this file
    QString XMLfileName(filename);
    XMLfileName.replace(".revosim", ".xml");
    saveSettings(XMLfileName);
    //This approach, because this won't work without custom >> and << operators - which is a faff
    //out << *simulationManager->simulationSettings;


    //RJG - otherwise, save the file
    QFile outfile(filename);
    if (!outfile.open(QIODevice::WriteOnly))
    {
        setStatusBarText("Error opening simulation file to write to.");
        return;
    }
    QDataStream out(&outfile);

    out << QString("REvoSim Format File");
    out << static_cast<quint8>(FILEVERSION);

    //Then to quote MDS, "serialise all my crap"
    //RJG - Environment system look ups / masks
    for (int i = 0; i < 256; i++)
        for (int j = 0; j < 3; j++)
            out << simulationManager->environmentalFitnessSytem->returnXOR(i, j);

    //RJG - The critters array
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
            for (int k = 0; k < simulationManager->cellSettings[i][j].slotsPerSquare; k++)
            {
                if ((critters[i][j][k]).age == 0) //its dead
                    out << static_cast<int>(0);
                else
                {
                    out << critters[i][j][k].age;
                    for (int l = 0; l < simulationManager->simulationSettings->genomeSize; l++) out << critters[i][j][k].genomeWords[l];
                    out << critters[i][j][k].fitness;
                    out << critters[i][j][k].energy;
                    out << critters[i][j][k].xPosition;
                    out << critters[i][j][k].yPosition;
                    out << critters[i][j][k].zPosition;
                    out << critters[i][j][k].speciesID;
                }
            }

    //RJG - Other arrays
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            quint8 *environment = simulationManager->env->returnRGB(i, j);
            out << environment[0];
            out << environment[1];
            out << environment[2];
        }

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            out << totalFitness[i][j];
            out << breedAttempts[i][j];
            out << breedFails[i][j];
            out << settles[i][j];
            out << settlefails[i][j];
            out << maxUsed[i][j];
        }


    /*
            //Ints
            out << simulationManager->simulationSettings->gridX;
            out << simulationManager->simulationSettings->gridY;
            out << settleTolerance;
            out << slotsPerSquare;
            out << startAge;
            out << dispersal;
            out << food;

            out << breedCost;
            out << pathogenMutate;
            out << pathogenFrequency;
            out << maxDifference;
            out << breedThreshold;
            out << target;
            out << simulationManager->simulationSettings->environmentChangeRate;
            out << simulationManager->simulationSettings->environmentMode;
            out << refreshRate;
            out << minSpeciesSize;
            out << speciesMode;

            //Bools
            out << recalculateFitness;
            out << toroidal;
            out << nonspatial;
            out << breedDifference;
            out << breedSpecies;
            out << pathOn;
            out << variableMutate;
            out << allowExcludeWithDescendants;
            out << obligateSexual;
            out << facultativeSexual;
            out << asexual;
            out << variableBreed;
            out << logging;
            out << csvoutput; //v2
            out << detailedrunninglogs; //v2
            out << environmentInterpolate;
            out << fitnessLoggingToFile;
            out << autoWriteLogCheckbox->isChecked();
            out << autoWriteIndividaulsLogCheckbox->isChecked();
            out << savePopulationCount->isChecked();
            out << saveMeanFitness->isChecked();
            out << saveVisSysOneAsColour->isChecked();
            out << saveSpecies->isChecked();
            out << saveVisSysTwoAsColour->isChecked();
            out << saveGeneFrequencies->isChecked();
            out << saveSettles->isChecked();
            out << saveFailsSettles->isChecked();
            out << saveEnvironment->isChecked();

            //Strings
            out << getSavePath();

            int vmode = 0;
            vmode = ui->populationWindowComboBox->itemData(ui->populationWindowComboBox->currentIndex()).toInt();
            out << vmode;

            //file list
            out << simulationManager->enviromentFiles.count();
            for (int i = 0; i < simulationManager->enviromentFiles.count(); i++)
                out << simulationManager->enviromentFiles[i];

            //now the big arrays
            for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
                for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
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
                            out << critters[i][j][k].speciesID;

                        }
                    }

            for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
                for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
                {
                    out << environment[i][j][0];
                    out << environment[i][j][1];
                    out << environment[i][j][2];
                }

            for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
                for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
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

            for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
                for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
                {
                    out << breedAttempts[i][j];
                    out << breedFails[i][j];
                    out << settles[i][j];
                    out << settlefails[i][j];
                    out << maxUsed[i][j];
                }

            //And some window state stuff
            out << saveState(); //window state
            out << ui->actionGenomeComparison->isChecked();

            //interpolate environment stuff
            out << environmentInterpolate;
            for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
                for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
                {
                    out << environmentLast[i][j][0];
                    out << environmentLast[i][j][1];
                    out << environmentLast[i][j][2];
                }
            for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
                for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
                {
                    out << environmentNext[i][j][0];
                    out << environmentNext[i][j][1];
                    out << environmentNext[i][j][2];
                }

            out << speciesSamples;
            out << speciesSensitivity;
            out << timeSliceConnect;

            //now the species archive
            out << oldSpeciesList->count();
            for (int j = 0; j < oldSpeciesList->count(); j++)
            {
                out << (*oldSpeciesList)[j].ID;
                out << (*oldSpeciesList)[j].type;
                out << (*oldSpeciesList)[j].originTime;
                out << (*oldSpeciesList)[j].parent;
                out << (*oldSpeciesList)[j].size;
                out << (*oldSpeciesList)[j].internalID;
                for (int i = 0; i < 64; i++) out << (*oldSpeciesList)[j].frequencies[i]; //added MDS - this drives change in file version number
            }

            out << archivedSpeciesLists.count();
            for (int i = 0; i < archivedSpeciesLists.count(); i++)
            {
                out << archivedSpeciesLists[i].count();
                for (int j = 0; j < archivedSpeciesLists[i].count(); j++)
                {
                    out << archivedSpeciesLists[i][j].ID;
                    out << archivedSpeciesLists[i][j].type;
                    out << archivedSpeciesLists[i][j].originTime;
                    out << archivedSpeciesLists[i][j].parent;
                    out << archivedSpeciesLists[i][j].size;
                    out << archivedSpeciesLists[i][j].internalID;
                }
            }
            out << nextSpeciesID;
            out << simulationManager->simulationSettings->lastSpeciesCalculated;

            //now random number array
            for (unsigned char random : randoms8)
                out << random;

            //MDS - write out the lastGenome and initialGenome hashes
            out << lastGenomes->count();
            QHash<quint64, genome_for_tracking>::iterator it;
            for (it = lastGenomes->begin(); it != lastGenomes->end(); ++it)
            {
                out << it.key();
                out << it.value().modal;
                for (int ii = 0; ii < 64; ii++) out << it.value().frequency[ii];
            }

            out << initialGenomes->count();
            for (it = initialGenomes->begin(); it != initialGenomes->end(); ++it)
            {
                out << it.key();
                out << it.value().modal;
                for (int ii = 0; ii < 64; ii++) out << it.value().frequency[ii];
            }

            outfile.close();
            */
}

/*!
 * \brief MainWindow::loadSimulation
 *
 * Load a REvoSim file of saved settings and simulations.
 *
 * \todo RJG - Fitness logging to file not sorted on load as yet.
 */
void MainWindow::loadSimulation(QString filename)
{
    //RJG - load  file
    if (filename.length() < 3) filename = QFileDialog::getOpenFileName(this, tr("Open file..."), "", "REvoSim files (*.revosim)");
    if (filename.length() == 0 || !filename.endsWith(".revosim"))
    {
        QMessageBox::warning(this, "", "Error opening simulation file - is this a valid .revosim file?");
        return;
    }

    QString XMLFileName(filename);
    XMLFileName.replace(".revosim", ".xml");

    //QFile settingsFile (XMLFileName);
    //if (!settingsFile.exists()) QMessageBox::warning(this, "", "Can't find matching .xml file to load settings. Will continue to load simulation, but will not change the settings to match saved state.");
    //else loadSettings(XMLFileName);

    if (!stopFlag) stopFlag = true;

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

    quint8 version;
    in >> version;
    if (version > FILEVERSION) QMessageBox::warning(this, "", "Version too high - will try to read, but may go horribly wrong.");


    //Then to quote MDS, "serialise all my crap"
    //RJG - Environment system look ups / masks
    for (int i = 0; i < 256; i++)
        for (int j = 0; j < 3; j++)
        {
            quint32 v;
            in >> v;
            simulationManager->environmentalFitnessSytem->setXOR(i, j, v);
        }


    int count = 0;
    //The critters array
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
            for (int k = 0; k < simulationManager->cellSettings[i][j].slotsPerSquare; k++)
            {
                in >> critters[i][j][k].age;
                if (critters[i][j][k].age > 0)
                {
                    if (critters[i][j][k].genomeWords == nullptr) critters[i][j][k].initialise((quint32 *)(&simulationManager->simulationSettings->reseedGenome), simulationManager->env->returnRGB(i, j), i, j, 0,
                                                                                                   nextSpeciesID, 0);
                    for (int l = 0; l < simulationManager->simulationSettings->genomeSize; l++) in >> critters[i][j][k].genomeWords[l];
                    in >> critters[i][j][k].fitness;
                    in >> critters[i][j][k].energy;
                    in >> critters[i][j][k].xPosition;
                    in >> critters[i][j][k].yPosition;
                    in >> critters[i][j][k].zPosition;
                    in >> critters[i][j][k].speciesID;

                    if (count < 50)
                    {
                        QString genome;
                        for (int l = 0; l < simulationManager->simulationSettings->genomeSize; l++) genome.append(simulationManager->printGenome(critters[i][j][k].genomeWords[l]));
                        count++;
                    }
                }
            }

    //RJG - Other arrays
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            quint8 environment[3];
            in >> environment[0];
            in >> environment[1];
            in >> environment[2];
            simulationManager->env->setRGB(i, j, environment);
        }

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            in >> totalFitness[i][j];
            in >> breedAttempts[i][j];
            in >> breedFails[i][j];
            in >> settles[i][j];
            in >> settlefails[i][j];
            in >> maxUsed[i][j];
        }

    infile.close();


    updateGUIFromVariables();
    nextRefresh = 0;
    resizeImageObjects();
    //RJG Oct 21 - this is currently causing a crash due to the species system when inserting into a hash - comment out for now
    report();
    resize();
    refreshEnvironment();
    refreshPopulations();


    /*
    //Ints
    in >> simulationManager->simulationSettings->gridX;
    in >> simulationManager->simulationSettings->gridY;
    in >> settleTolerance;
    in >> slotsPerSquare;
    in >> startAge;
    in >> dispersal;
    in >> food;
    in >> breedCost;
    in >> mutate;
    in >> pathogenMutate;
    in >> pathogenFrequency;
    in >> maxDifference;
    in >> breedThreshold;
    in >> target;
    in >> simulationManager->simulationSettings->environmentChangeRate;
    in >> simulationManager->simulationSettings->environmentMode;
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
    in >> pathOn;
    in >> variableMutate;
    in >> allowExcludeWithDescendants;
    in >> obligateSexual;
    in >> facultativeSexual;
    in >> asexual;
    in >> variableBreed;
    in >> logging;
    if (version >= 2) in >> csvoutput;
    if (version >= 2) in >> detailedrunninglogs;
    in >> gui;
    in >> environmentInterpolate;
    in >> fitnessLoggingToFile;
    bool inBool;
    in >> inBool;
    autoWriteLogCheckbox->setChecked(inBool);
    in >> inBool;
    autoWriteIndividualsLogCheckbox->setChecked(inBool);
    in >> inBool;
    savePopulationCount->setChecked(inBool);
    in >> inBool;
    saveMeanFitness->setChecked(inBool);
    in >> inBool;
    saveVisSysOneAsColour->setChecked(inBool);
    in >> inBool;
    saveSpecies->setChecked(inBool);
    in >> inBool;
    saveVisSysTwoAsColour->setChecked(inBool);
    in >> inBool;
    saveGeneFrequencies->setChecked(inBool);
    in >> inBool;
    saveSettles->setChecked(inBool);
    in >> inBool;
    saveFailsSettles->setChecked(inBool);
    in >> inBool;
    saveEnvironment->setChecked(inBool);

    //Strings
    QString loadPath;
    in >> loadPath;
    globalSavePath->setText(loadPath);

    // 0 = Static
    // 1 = Once
    // 2 = Looped (default)
    // 3 = Bounce
    simulationManager->simulationSettings->environmentMode = ENV_MODE_LOOP;
    in >> simulationManager->simulationSettings->environmentMode;
    if (simulationManager->simulationSettings->environmentMode == 0)
        simulationManager->simulationSettings->environmentModeStaticButton->setChecked(true);
    if (simulationManager->simulationSettings->environmentMode == 1)
        simulationManager->simulationSettings->environmentModeOnceButton->setChecked(true);
    if (simulationManager->simulationSettings->environmentMode == 2)
        simulationManager->simulationSettings->environmentModeLoopButton->setChecked(true);
    if (simulationManager->simulationSettings->environmentMode == 3)
        simulationManager->simulationSettings->environmentModeBounceButton->setChecked(true);

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
    if (index != -1)   // -1 for not found
    {
        ui->populationWindowComboBox->setCurrentIndex(index);
    }

    int count;

    //file list
    in >> count;
    simulationManager->enviromentFiles.clear();

    for (int i = 0; i < count; i++)
    {
        QString t;
        in >> t;
        simulationManager->enviromentFiles.append(t);
    }

    //now the big arrays
    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
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
                    in >> critters[i][j][k].speciesID;
                }
            }

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            in >> environment[i][j][0];
            in >> environment[i][j][1];
            in >> environment[i][j][2];
        }

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
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


    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            in >> breedAttempts[i][j];
            in >> breedFails[i][j];
            in >> settles[i][j];
            in >> settlefails[i][j];
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

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
        {
            in >> environmentLast[i][j][0];
            in >> environmentLast[i][j][1];
            in >> environmentLast[i][j][2];
        }

    for (int i = 0; i < simulationManager->simulationSettings->gridX; i++)
        for (int j = 0; j < simulationManager->simulationSettings->gridY; j++)
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
    oldSpeciesList->clear();

    int temp;
    in >> temp; //oldSpeciesList->count();
    for (int j = 0; j < temp; j++)
    {
        Species s;
        in >> s.ID;
        in >> s.type;
        in >> s.originTime;
        in >> s.parent;
        in >> s.size;
        in >> s.internalID;
        if (version >= 2)
            for (int i = 0; i < 64; i++) in >> s.frequencies[i];
        oldSpeciesList->append(s);
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
            in >> s.ID;
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
    in >> simulationManager->simulationSettings->lastSpeciesCalculated; //actually no - if we import this it will assume an 'a' object exists.
    //bodge
    simulationManager->simulationSettings->lastSpeciesCalculated--;

    //now random array
    for (unsigned char &random : randoms8)
        in >> random;

    if (version >= 2)
    {
        int hashcount;

        lastGenomes->clear();
        //read lastgenomes
        in >> hashcount;
        for (int i = 0; i < hashcount; i++)
        {
            genome_for_tracking g;
            quint64 key;
            in >> key;
            in >> g.modal;
            for (int ii = 0; ii < 64; ii++) in >> g.frequency[ii];
            lastGenomes->insert(key, g);
        }

        //read initalgenomes
        initialGenomes->clear();
        in >> hashcount;
        for (int i = 0; i < hashcount; i++)
        {
            genome_for_tracking g;
            quint64 key;
            in >> key;
            in >> g.modal;
            for (int ii = 0; ii < 64; ii++) in >> g.frequency[ii];
            initialGenomes->insert(key, g);
        }
    }

    infile.close();
    nextRefresh = 0;
    resizeImageObjects();
    report();
    resize();

    updateGUIFromVariables();
    */
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
    quint8 *environment = simulationManager->env->returnRGB(x, y);

    //---- Get genome colour
    if (totalFitness[x][y] != 0)
    {
        if (!genoneComparison->returnAll())
        {
            for (int c = 0; c < simulationManager->cellSettingsMaster->slotsPerSquare; c++)
                if (critters[x][y][c].age > 0) genoneComparison->addGenomeCritter(critters[x][y][c], environment);

            return true;
        }
        else
        {
            int c = simulationManager->genomeComparisonSystem->findModalGenome(x, y);
            if (c == -1) return false;
            genoneComparison->addGenomeCritter(critters[x][y][c], environment);

            return true;
        }
    }

    return false;
}

/*!
 * \brief MainWindow::on_actionFitness_logging_to_File_triggered
 */
void MainWindow::on_actionFitness_logging_to_File_triggered()
{
    simulationManager->simulationSettings->fitnessLoggingToFile = ui->actionFitness_logging_to_File->isChecked();
}

/*!
 * \brief MainWindow::calculateSpecies
 */
void MainWindow::calculateSpecies()
{
    if (speciesMode == SPECIES_MODE_NONE) return; //do nothing!

    if (simulationManager->iteration != simulationManager->simulationSettings->lastSpeciesCalculated)
    {
        delete analyser;  //replace old analyser object with new
        analyser = new Analyser;
        analyser->speciationLogging = ui->actionSpeciation_logging->isChecked();

        //New species analyser
        analyser->groupsGenealogicalTracker_v3();

        //Makre sure this is updated
        simulationManager->simulationSettings->lastSpeciesCalculated = simulationManager->iteration;
    }
}

/*!
 * \brief MainWindow::writeLog
 *
 * Writes main ongoing log
 */
void MainWindow::writeLog()
{
    int batchNumber;
    if (batchRunning)batchNumber = batchRuns;
    else batchNumber = -1;
    if (simulationManager->simulationSettings->logging)
    {
        simulationManager->simulationLog->setHeaderTextFromGUI(mainWindow->headerTextEdit->toPlainText());
        simulationManager->simulationLog->setIterationTextFromGUI(mainWindow->iterationTextEdit->toPlainText());
        simulationManager->simulationLog->setSpeciestTextFromGUI(mainWindow->logTextEdit->toPlainText());
        simulationManager->simulationLog->writeLog(getSavePath(), batchNumber, LOG_CUSTOM);
    }
    if (ui->actionRecombination_logging->isChecked())simulationManager->simulationLog->writeLog(getSavePath(), batchNumber, LOG_RECOMBINATION);
    if (ui->actionFitness_logging_to_File->isChecked())simulationManager->simulationLog->writeLog(getSavePath(), batchNumber, LOG_FITNESS);
    if (ui->actionVariable_mutation_logging->isChecked())simulationManager->simulationLog->writeLog(getSavePath(), batchNumber, LOG_MUTATION);
    if (ui->actionSpeciation_logging->isChecked() && analyser->speciationLogText.length() > 0)simulationManager->simulationLog->writeLog(getSavePath(), batchNumber, LOG_SPECIATION);
    if (ui->actionDisparity_logging->isChecked())simulationManager->simulationLog->writeLog(getSavePath(), batchNumber, LOG_DISPARITY);
    if (ui->actionVariable_hgt_prob_logging->isChecked())simulationManager->simulationLog->writeLog(getSavePath(), batchNumber, LOG_HGT_PROB);
    if (ui->actionVariable_hgt_len_logging->isChecked())simulationManager->simulationLog->writeLog(getSavePath(), batchNumber, LOG_HGT_LEN);
    if (ui->actionVariable_hgt_id_logging->isChecked())simulationManager->simulationLog->writeLog(getSavePath(), batchNumber, LOG_HGT_ID);
}


/*!
 * \brief MainWindow::getSavePath()
 *
 * Returns the save path.
 */
QString MainWindow::getSavePath()
{
    return globalSavePath->text();
}

/*!
 * \brief MainWindow::getBatchRuns()
 *
 * Returns the save path.
 */
quint64 MainWindow::getBatchRuns()
{
    return batchRuns;
}

/*!
 * \brief MainWindow::setStatusBarText
 * \param text
 *
 * Sets the status bar text.
 */
void MainWindow::setStatusBarText(const QString &text)
{
    if (!autoFromCommand) ui->statusBar->showMessage(text);
    else if (text.length() > 0) qInfo() << text;
}

/**
 * @brief MainWindow::statusProgressBar
 * @param prBar
 * @param add
 */
void MainWindow::statusProgressBar(QProgressBar *prBar, bool add)
{
    if (add)
    {
        ui->statusBar->clearMessage();
        if (!autoFromCommand) setStatusBarText("Calculating species...");
        else qInfo() << "Calculating species...";
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
    QString file = QFileDialog::getOpenFileName(this, "Select random number file", "", "*.*");

    if (file.length() == 0) return;

    int seedOffset;
    seedOffset = QInputDialog::getInt(this, "Seed value", "Byte offset to start reading from (will read 65536 bytes)");

    //now read in values to array
    QFile rfile(file);
    rfile.open(QIODevice::ReadOnly);

    rfile.seek(seedOffset);

    qint64 i = rfile.read(reinterpret_cast<char *>(randoms8), 65536);
    if (i != 65536)QMessageBox::warning(this, "Oops", "Failed to read 65536 bytes from file - random numbers may be compromised - try again or restart program");
    else QMessageBox::information(this, "Success", "New random numbers read successfully");
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
    //RJG - changed to return string 04/04/18 as analysis docker will be removed.

    //Is there a valid input file?
    AnalysisTools a;
    QString OutputString;

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
            OutputString = a.makeNewick(rootSpecies);
        else
            OutputString = "Species tracking is not enabled.";
        break;

    case ANALYSIS_TOOL_CODE_WRITE_DATA:
        if (phylogenyAndMetricsButton->isChecked()) OutputString = a.writeData(rootSpecies);
        else
            OutputString = "Species tracking is not enabled, or is set to phylogeny only.";
        break;

    default:
        QMessageBox::warning(this, "Error", "No handler for analysis tool");
        return QString("Error, No handler for analysis tool");

    }

    //write result to screen
    //ui->plainTextEdit->clear();
    //ui->plainTextEdit->appendPlainText(OutputString);

    return OutputString;
}

/*!
 * \brief MainWindow::intToBool
 *
 * Conversion function as reading int's to bools doesn't work!
 */
bool MainWindow::intToBool(int i)
{
    if (i > 0) return true;
    else return false;
}

/*!
 * \brief MainWindow::loadSettings
 *
 * Save and load settings (but not critter info, masks etc.).
 */
void MainWindow::loadSettings(QString fileName, bool calledFromCommandLine)
{
    QString settingsFilename;
    if (fileName.length() > 0)settingsFilename = fileName;
    else settingsFilename = QFileDialog::getOpenFileName(this, tr("Open File"), getSavePath(), "XML files (*.xml)");
    QFile settingsFile(settingsFilename);
    if (settingsFilename.length() < 3 || !settingsFile.open(QIODevice::ReadOnly))
    {
        if (calledFromCommandLine) qInfo() << "******* Failed to load settings file. Will run using defaults. *******";
        else setStatusBarText("Error opening file.");
        return;
    }

    QXmlStreamReader settingsFileIn(&settingsFile);

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
            {
                simulationManager->simulationSettings->gridX = settingsFileIn.readElementText().toInt();
            }
            if (settingsFileIn.name() == "gridY")
            {
                simulationManager->simulationSettings->gridY = settingsFileIn.readElementText().toInt();
            }
            if (settingsFileIn.name() == "settleTolerance")
                simulationManager->cellSettingsMaster->settleTolerance = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "slotsPerSquare")
                simulationManager->cellSettingsMaster->slotsPerSquare = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "startAge")
                simulationManager->cellSettingsMaster->startAge = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "dispersal")
                simulationManager->cellSettingsMaster->dispersal = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "food")
                simulationManager->cellSettingsMaster->food = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "breedCost")
                simulationManager->cellSettingsMaster->breedCost = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "mutate")
                simulationManager->cellSettingsMaster->mutate = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "interactrate")
                simulationManager->cellSettingsMaster->interactions = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "croppingRate")
                simulationManager->cellSettingsMaster->croppingFrequency = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "predationefficiency")
                simulationManager->cellSettingsMaster->predationEfficiency = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "minpredatorscore")
                simulationManager->cellSettingsMaster->minDeltaPredatorness = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "pathogenMutate")
                simulationManager->cellSettingsMaster->pathogenMutate = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "pathogenFrequency")
                simulationManager->cellSettingsMaster->pathogenFrequency = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "maxDifference")
                simulationManager->simulationSettings->maxDifference = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "breedThreshold")
                simulationManager->cellSettingsMaster->breedThreshold = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "target")
                simulationManager->cellSettingsMaster->target = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "environmentChangeRate")
                simulationManager->simulationSettings->environmentChangeRate = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "refreshRate")
                refreshRate = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "environmentMode")
                environmentModeChanged(settingsFileIn.readElementText().toInt(), true);
            if (settingsFileIn.name() == "pathogenMode")
                simulationManager->simulationSettings->pathogenMode = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "interactions")
                simulationManager->cellSettingsMaster->interactions = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "predationEfficiency")
                simulationManager->cellSettingsMaster->predationEfficiency = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "minDeltaPredatorness")
                simulationManager->cellSettingsMaster->minDeltaPredatorness = settingsFileIn.readElementText().toInt();

            //No Gui options for the remaining settings as yet.
            if (settingsFileIn.name() == "speciesSamples")
                simulationManager->simulationSettings->speciesSamples = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "speciesSensitivity")
                simulationManager->simulationSettings->speciesSensitivity = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "timeSliceConnect")
                simulationManager->simulationSettings->timeSliceConnect = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "minSpeciesSize")
                simulationManager->simulationSettings->minSpeciesSize = static_cast<quint64>(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "speciesMode")
                speciesMode = static_cast<quint64>(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "genomeSize")
                simulationManager->simulationSettings->genomeSize = settingsFileIn.readElementText().toInt();

            //Bools
            if (settingsFileIn.name() == "recalculateFitness")
                simulationManager->simulationSettings->recalculateFitness = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "noSelection")
                simulationManager->cellSettingsMaster->noSelection = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "toroidal")
                simulationManager->simulationSettings->toroidal = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "nonspatial")
                simulationManager->simulationSettings->nonspatial = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "breedDifference")
                simulationManager->cellSettingsMaster->breedDifference = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "breedSpecies")
                simulationManager->cellSettingsMaster->breedSpecies = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "pathOn")
                simulationManager->cellSettingsMaster->pathOn = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "variableMutate")
                simulationManager->cellSettingsMaster->variableMutate = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "allowExcludeWithDescendants")
                allowExcludeWithDescendants = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "obligateSexual")
                simulationManager->cellSettingsMaster->obligateSexual = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "facultativeSexual")
                simulationManager->cellSettingsMaster->facultativeSexual = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "asexual")
                simulationManager->cellSettingsMaster->asexual = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "variableBreed")
                simulationManager->cellSettingsMaster->variableBreed = settingsFileIn.readElementText().toInt();
            if (settingsFileIn.name() == "logging")
                simulationManager->simulationSettings->logging = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "csvoutput")
                simulationManager->simulationLog->csvOutput = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "gui")
                simulationManager->simulationSettings->gui = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "environmentInterpolate")
                simulationManager->simulationSettings->environmentInterpolate = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "interactBlocks")
                simulationManager->cellSettingsMaster->interactBlocks = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "multiBreedList")
                simulationManager->cellSettingsMaster->multiBreedList = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "interactFitness")
                simulationManager->cellSettingsMaster->interactFitness = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "interactEnergy")
                simulationManager->cellSettingsMaster->interactEnergy = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "interactWithinSpecies")
                simulationManager->cellSettingsMaster->interactWithinSpecies = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "predationRestriction")
                simulationManager->simulationSettings->predationRestriction = intToBool(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "v2log")
            {
                logTextEdit->setText(simulationManager->simulationLog->printDefaultLogSpeciesText());
                iterationTextEdit->setText(simulationManager->simulationLog->printDefaultLogIterationText());
                headerTextEdit->setText(simulationManager->simulationLog->printDefaultHeaderText());
            }
            if (settingsFileIn.name() == "randomReseedBeforeGenetic")
                simulationManager->simulationSettings->randomReseedBeforeGenetic = intToBool(settingsFileIn.readElementText().toInt());

            //No gui options for below
            if (settingsFileIn.name() == "fitnessLoggingToFile")
                simulationManager->simulationSettings->fitnessLoggingToFile = intToBool(settingsFileIn.readElementText().toInt());
            //Only GUI options
            if (settingsFileIn.name() == "autowrite")
                autoWriteLogCheckbox->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "autowriteIndividuals")
                autoWriteLogIndividualsCheckbox->setChecked(settingsFileIn.readElementText().toInt());
            if (settingsFileIn.name() == "savePopulationCount")
                savePopulationCount->setChecked(intToBool(settingsFileIn.readElementText().toInt()));
            if (settingsFileIn.name() == "saveMeanFitness")
                saveMeanFitness->setChecked(intToBool(settingsFileIn.readElementText().toInt()));
            if (settingsFileIn.name() == "saveVisSysOneAsColour")
                saveVisSysOneAsColour->setChecked(intToBool(settingsFileIn.readElementText().toInt()));
            if (settingsFileIn.name() == "saveSpecies")
                saveSpecies->setChecked(intToBool(settingsFileIn.readElementText().toInt()));
            if (settingsFileIn.name() == "saveVisSysTwoAsColour")
                saveVisSysTwoAsColour->setChecked(intToBool(settingsFileIn.readElementText().toInt()));
            if (settingsFileIn.name() == "saveSettles")
                saveSettles->setChecked(intToBool(settingsFileIn.readElementText().toInt()));
            if (settingsFileIn.name() == "saveFailsSettles")
                saveFailsSettles->setChecked(intToBool(settingsFileIn.readElementText().toInt()));
            if (settingsFileIn.name() == "saveEnvironment")
                saveEnvironment->setChecked(intToBool(settingsFileIn.readElementText().toInt()));

            //Strings
            if (settingsFileIn.name() == "globalSavePath")
                globalSavePath->setText(settingsFileIn.readElementText());
            if (settingsFileIn.name() == "headerTextEdit")
                simulationManager->simulationLog->setHeaderTextFromGUI(settingsFileIn.readElementText());
            if (settingsFileIn.name() == "iterationTextEdit")
                simulationManager->simulationLog->setIterationTextFromGUI(settingsFileIn.readElementText());
            if (settingsFileIn.name() == "logTextEdit")
                simulationManager->simulationLog->setSpeciestTextFromGUI(settingsFileIn.readElementText());

            for (auto &s : simulationManager->systemsList)
                if (settingsFileIn.name() == s->returnName().replace(" ", "_"))
                    s->setGenomeWordsFromString(settingsFileIn.readElementText(), simulationManager->simulationSettings->genomeSize);
        }
    }
    // Error
    if (settingsFileIn.hasError())
        setStatusBarText("There seems to have been an error reading in the XML file. Not all settings will have been loaded.");
    else
        setStatusBarText("Loaded settings file");

    settingsFile.close();

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
    gridXSpin->setValue(simulationManager->simulationSettings->gridX);
    genomeSizeSpin->setValue(simulationManager->simulationSettings->genomeSize);
    settleToleranceSpin->setValue(simulationManager->cellSettingsMaster->settleTolerance);
    slotsSpin->setValue(simulationManager->cellSettingsMaster->slotsPerSquare);
    startAgeSpin->setValue(simulationManager->cellSettingsMaster->startAge);
    dispersalSpin->setValue(simulationManager->cellSettingsMaster->dispersal);
    energySpin->setValue(simulationManager->cellSettingsMaster->food);
    breedCostSpin->setValue(simulationManager->cellSettingsMaster->breedCost);
    mutateSpin->setValue(simulationManager->cellSettingsMaster->mutate);
    maxDifferenceSpin->setValue(simulationManager->simulationSettings->maxDifference);
    breedThresholdSpin->setValue(simulationManager->cellSettingsMaster->breedThreshold);
    targetSpin->setValue(simulationManager->cellSettingsMaster->target);
    environmentRateSpin->setValue(simulationManager->simulationSettings->environmentChangeRate);
    refreshRateSpin->setValue(refreshRate);
    interactionsSpin->setValue(simulationManager->cellSettingsMaster->interactions);
    croppingFrequencySpin->setValue(simulationManager->cellSettingsMaster->croppingFrequency);
    predationDeltaSpin->setValue(simulationManager->cellSettingsMaster->minDeltaPredatorness);
    predationEfficiencySpin->setValue(simulationManager->cellSettingsMaster->predationEfficiency);
    pathogenMutateSpin->setValue(simulationManager->cellSettingsMaster->pathogenMutate);
    pathogenFrequencySpin->setValue(simulationManager->cellSettingsMaster->pathogenFrequency);
    if (simulationManager->simulationSettings->pathogenMode == PATH_MODE_DRIFT)pathogenDriftRadio->setChecked(true);
    else pathogenEvolveRadio->setChecked(true);

    // Add speciesMode
    speciesModeChanged(speciesMode, true);
    // Add simulationManager->simulationSettings->environmentMode
    environmentModeChanged(simulationManager->simulationSettings->environmentMode, true);

    //Bools
    recalculateFitnessCheckbox->setChecked(simulationManager->simulationSettings->recalculateFitness);
    noSelectionFitnessCheckbox->setChecked(simulationManager->cellSettingsMaster->noSelection);
    toroidalCheckbox->setChecked(simulationManager->simulationSettings->toroidal);
    nonspatialCheckbox->setChecked(simulationManager->simulationSettings->nonspatial);
    breedDifferenceCheckbox->setChecked(simulationManager->cellSettingsMaster->breedDifference);
    breedSpeciesCheckbox->setChecked(simulationManager->cellSettingsMaster->breedSpecies);
    pathogensCheckbox->setChecked(simulationManager->cellSettingsMaster->pathOn);
    MultiBreedListCheckbox->setChecked(simulationManager->cellSettingsMaster->multiBreedList);
    variableMutationCheckbox->setChecked(simulationManager->cellSettingsMaster->variableMutate);
    excludeWithoutDescendantsCheckbox->setChecked(allowExcludeWithDescendants);
    obligateSexualRadio->setChecked(simulationManager->cellSettingsMaster->obligateSexual);
    facultativeSexualRadio->setChecked(simulationManager->cellSettingsMaster->facultativeSexual);
    asexualRadio->setChecked(simulationManager->cellSettingsMaster->asexual);
    variableBreedRadio->setChecked(simulationManager->cellSettingsMaster->variableBreed);
    loggingCheckbox->setChecked(simulationManager->simulationSettings->logging);
    guiCheckbox->setChecked(simulationManager->simulationSettings->gui);
    interpolateCheckbox->setChecked(simulationManager->simulationSettings->environmentInterpolate);
    minSpeciesSizeSpin->setValue(simulationManager->simulationSettings->minSpeciesSize);

    if (simulationManager->cellSettingsMaster->interactBlocks) BlockInteractionsRadio->setChecked(true);
    else XORRadio->setChecked(true);

    if (simulationManager->cellSettingsMaster->interactFitness) FitnessChangeRadio->setChecked(true);
    else if (simulationManager->cellSettingsMaster->interactEnergy) EnergyChangeRadio->setChecked(true);
    else NoInteractionTypeRadio->setChecked(true);

    headerTextEdit->setHtml(simulationManager->simulationLog->printHeaderText());
    iterationTextEdit->setHtml(simulationManager->simulationLog->printLogIterationText());
    logTextEdit->setHtml(simulationManager->simulationLog->printLogSpeciesText());

    fitness_word_edit->setText(simulationManager->environmentalFitnessSytem->returnGenomeWordInUseString());
    breed_word_edit->setText(simulationManager->breedSystem->returnGenomeWordInUseString());
    mutate_word_edit->setText(simulationManager->mutationSystem->returnGenomeWordInUseString());
    variable_mutate_word_edit->setText(simulationManager->variableMutateSystem->returnGenomeWordInUseString());
    variable_breed_word_edit->setText(simulationManager->variableBreedSystem->returnGenomeWordInUseString());
    pathogen_word_edit->setText(simulationManager->pathogensSystem->returnGenomeWordInUseString());
    species_ID_word_edit->setText(simulationManager->speciesIDSytem->returnGenomeWordInUseString());
    interactions_word_edit->setText(simulationManager->interactionSystem->returnGenomeWordInUseString());
    visualisations_word_edit->setText(simulationManager->visualisationSystem->returnGenomeWordInUseString());
    visualisations_word_edit_2->setText(simulationManager->visualisationSystem2->returnGenomeWordInUseString());
}

/*!
 * \brief MainWindow::saveSettings
 */
void MainWindow::saveSettings(QString fileName)
{
    QString settingsFilename(fileName);
    if (settingsFilename.length() < 3) settingsFilename = QFileDialog::getSaveFileName(this, tr("Save file as..."), QString(getSavePath() + QString(PRODUCTNAME) + "_settings.xml"));
    if (!settingsFilename.endsWith(".xml"))settingsFilename.append(".xml");
    QFile settingsFile(settingsFilename);
    if (!settingsFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        setStatusBarText("Error opening settings file to write to.");
        return;
    }

    QString saveMessage;

    //RJG - assume if file list is one it's default, otherwise custom - add warning this is not saved
    if (simulationManager->simulationSettings->linkagesOn) saveMessage.append("Note that save settings will not save your linkages. ");
    if (simulationManager->env->returnFileListCount() > 1) saveMessage.append("Note that save settings will not save your environmental images. ");
    if (simulationManager->cellSettingsMaster->interactBlocks)saveMessage.append("Save settings will not save a customised a priori interactions grid. ");

    QXmlStreamWriter settingsFileOut(&settingsFile);
    settingsFileOut.setAutoFormatting(true);
    settingsFileOut.setAutoFormattingIndent(-2);

    settingsFileOut.writeStartDocument();

    settingsFileOut.writeStartElement("revosim");

    //Ints
    settingsFileOut.writeStartElement("gridX");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->gridX));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("gridY");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->gridY));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("settleTolerance");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->settleTolerance));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("slotsPerSquare");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->slotsPerSquare));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("startAge");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->startAge));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("dispersal");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->dispersal));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("food");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->food));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("breedCost");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->breedCost));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("mutate");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->mutate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("pathogenMutate");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->pathogenMutate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("pathogenFrequency");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->pathogenFrequency));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("maxDifference");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->maxDifference));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("breedThreshold");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->breedThreshold));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("target");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->target));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("environmentChangeRate");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->environmentChangeRate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("environmentMode");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->environmentMode));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("refreshRate");
    settingsFileOut.writeCharacters(QString("%1").arg(refreshRate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("speciesSamples");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->speciesSamples));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("speciesSensitivity");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->speciesSensitivity));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("timeSliceConnect");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->timeSliceConnect));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("minSpeciesSize");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->minSpeciesSize));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("speciesMode");
    settingsFileOut.writeCharacters(QString("%1").arg(speciesMode));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("genomeSize");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->genomeSize));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("pathogenMode");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->pathogenMode));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("predationEfficiency");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->predationEfficiency));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("minDeltaPredatorness");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->minDeltaPredatorness));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("interactions");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->interactions));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("croppingRate");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->croppingFrequency));
    settingsFileOut.writeEndElement();

    //Bools
    settingsFileOut.writeStartElement("recalculateFitness");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->recalculateFitness));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("noSelection");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->noSelection));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("toroidal");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->toroidal));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("nonspatial");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->nonspatial));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("breedDifference");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->breedDifference));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("breedSpecies");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->breedSpecies));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("multiBreedList");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->multiBreedList));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("pathOn");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->pathOn));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("variableMutate");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->variableMutate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("allowExcludeWithDescendants");
    settingsFileOut.writeCharacters(QString("%1").arg(allowExcludeWithDescendants));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("obligateSexual");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->obligateSexual));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("facultativeSexual");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->facultativeSexual));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("asexual");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->asexual));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("variableBreed");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->variableBreed));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("logging");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->logging));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("csvoutput");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationLog->csvOutput));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("gui");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->gui));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("environmentInterpolate");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->environmentInterpolate));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("fitnessLoggingToFile");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->fitnessLoggingToFile));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("autowrite");
    settingsFileOut.writeCharacters(QString("%1").arg(autoWriteLogCheckbox->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("autowriteIndividuals");
    settingsFileOut.writeCharacters(QString("%1").arg(autoWriteLogIndividualsCheckbox->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("savePopulationCount");
    settingsFileOut.writeCharacters(QString("%1").arg(savePopulationCount->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveMeanFitness");
    settingsFileOut.writeCharacters(QString("%1").arg(saveMeanFitness->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveVisSysOneAsColour");
    settingsFileOut.writeCharacters(QString("%1").arg(saveVisSysOneAsColour->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveSpecies");
    settingsFileOut.writeCharacters(QString("%1").arg(saveSpecies->isChecked()));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("saveVisSysTwoAsColour");
    settingsFileOut.writeCharacters(QString("%1").arg(saveVisSysTwoAsColour->isChecked()));
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

    settingsFileOut.writeStartElement("randomReseedBeforeGenetic");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->randomReseedBeforeGenetic));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("interactBlocks");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->interactBlocks));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("interactFitness");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->interactFitness));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("interactEnergy");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->interactEnergy));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("interactWithinSpecies");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->cellSettingsMaster->interactWithinSpecies));
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("predationRestriction");
    settingsFileOut.writeCharacters(QString("%1").arg(simulationManager->simulationSettings->predationRestriction));
    settingsFileOut.writeEndElement();

    //Strings
    settingsFileOut.writeStartElement("globalSavePath");
    settingsFileOut.writeCharacters(getSavePath());
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("headerTextEdit");
    settingsFileOut.writeCharacters(mainWindow->headerTextEdit->toHtml());
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("iterationTextEdit");
    settingsFileOut.writeCharacters(mainWindow->iterationTextEdit->toHtml());
    settingsFileOut.writeEndElement();

    settingsFileOut.writeStartElement("logTextEdit");
    settingsFileOut.writeCharacters(mainWindow->logTextEdit->toHtml());
    settingsFileOut.writeEndElement();

    for (auto &s : simulationManager->systemsList)
    {
        settingsFileOut.writeStartElement(s->returnName().replace(" ", "_"));
        settingsFileOut.writeCharacters(s->returnGenomeWordInUseString());
        settingsFileOut.writeEndElement();
    }

    settingsFileOut.writeEndElement();

    settingsFileOut.writeEndDocument();

    settingsFile.close();

    saveMessage.append("File saved");
    setStatusBarText(saveMessage);
}

/*!
 * \brief MainWindow::exit
 *
 * Action to exit the program.
 */
void MainWindow::exitProgram()
{
    if (pauseFlag)pauseSimulation();
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


/**
 * @brief MainWindow::on_actionBugIssueFeatureRequest_triggered
 */
void MainWindow::on_actionReportBugIssueFeatureRequest_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(GITURL) + QString(GITREPOSITORY) + QString(GITISSUE)));
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
    updateSettingsDockVisibility();
}

/*!
 * \brief MainWindow::on_actionSettings_Dock_triggered
 */
void MainWindow::on_actionOutput_Dock_triggered()
{
    updateOutputDockVisibility();
}


/*!
 * \brief MainWindow::on_actionGenomeComparison_triggered
 */
void MainWindow::on_actionGenomeComparison_triggered()
{
    updateGenomeComparisonDockVisibility(ui->actionGenomeComparison->isChecked());
}


/*!
 * \brief MainWindow::updateGenomeComparisonDockVisibility
 * \param checked
 *
 * Action to open the Genome Comparison Dock.
 */
void MainWindow::updateGenomeComparisonDockVisibility(bool checked)
{
    ui->genomeComparisonDock->setVisible(checked);
    ui->actionGenomeComparison->setChecked(checked);
    genomeComparisonButton->setChecked(checked);
}

void MainWindow::processAppEvents()
{
    qApp->processEvents();
}

bool MainWindow::boolStringToBool(QString s)
{
    if (s == "1") return true;
    else return false;
}

void MainWindow::setOptionsFromParser(QHash<QString, QString> *options)
{
    if (!options->empty()) qInfo() << "Launching software, setting options from command line";

    //MDS - maxthreads
    if (options->contains("maxthreads"))
        simulationManager->SetProcessorCount(options->value("maxthreads").toInt());
    else
        simulationManager->SetProcessorCount(-1); //use system-determined thread count


    //do this first as it resets stuff
    if (options->contains("settings"))
    {
        loadSettings(options->value("settings"), true);
    }
    if (options->contains("x")) simulationManager->simulationSettings->gridX = options->value("x").toInt();
    //Do  r here as then load environment below calls setuprun, which will update the environmental counter
    if (options->contains("r")) simulationManager->simulationSettings->environmentChangeRate = options->value("r").toInt();
    if (options->contains("e")) loadEnvironmentFiles(options->value("e"));
    if (options->contains("opt_log"))
    {
        simulationManager->simulationLog->loadLogXML(options->value("opt_log"));
        //RJG if you load file, assume you want to log
        simulationManager->simulationSettings->logging = true;
    }
    if (options->contains("a")) simulationManager->cellSettingsMaster->startAge = options->value("a").toInt();
    if (options->contains("b")) simulationManager->cellSettingsMaster->breedThreshold = options->value("b").toInt();
    if (options->contains("c")) simulationManager->cellSettingsMaster->breedCost = options->value("c").toInt();
    if (options->contains("d")) simulationManager->simulationSettings->maxDifference = options->value("d").toInt();
    //e done above
    if (options->contains("f")) simulationManager->cellSettingsMaster->breedDifference = boolStringToBool(options->value("f"));
    if (options->contains("g")) simulationManager->cellSettingsMaster->breedSpecies = boolStringToBool(options->value("g"));

    if (options->contains("i")) simulationManager->cellSettingsMaster->dispersal = options->value("i").toInt();
    if (options->contains("j")) globalSavePath->setText(options->value("j") + "/");
    if (options->contains("k"))
    {
        if (options->value("k").toLower() == "normal" || options->value("k").toLower() == "both")
            simulationManager->simulationSettings->logging = true;
        if (options->value("k").toLower() == "phylogeny" || options->value("k").toLower() == "both")
            autoWriteLogCheckbox->setChecked(true);
        if (options->value("k").toLower() == "normal") autoWriteLogCheckbox->setChecked(false);
    }
    if (options->contains("l")) allowExcludeWithDescendants = boolStringToBool(options->value("l"));
    if (options->contains("m"))
    {
        if (options->value("m").toLower() == "static") environmentModeChanged(ENV_MODE_STATIC, true);
        if (options->value("m").toLower() == "bounce") environmentModeChanged(ENV_MODE_BOUNCE, true);
        if (options->value("m").toLower() == "loop") environmentModeChanged(ENV_MODE_LOOP, true);
        if (options->value("m").toLower() == "once") environmentModeChanged(ENV_MODE_ONCE, true);
    }
    if (options->contains("n")) simulationManager->cellSettingsMaster->food = options->value("n").toInt();
    if (options->contains("o")) simulationManager->cellSettingsMaster->settleTolerance = options->value("o").toInt();
    if (options->contains("p"))
    {
        if (options->value("p").toLower() == "off" || options->value("p").toLower() == "none" )
            speciesModeChanged(SPECIES_MODE_NONE, true);
        if (options->value("p").toLower() == "basic")
            speciesModeChanged(SPECIES_MODE_BASIC, true);
        if (options->value("p").toLower() == "phylogeny")
            speciesModeChanged(SPECIES_MODE_PHYLOGENY, true);
        if (options->value("p").toLower() == "metrics")
            speciesModeChanged(SPECIES_MODE_PHYLOGENY_AND_METRICS, true);
    }
    if (options->contains("q")) simulationManager->simulationSettings->recalculateFitness = boolStringToBool(options->value("q"));
    if (options->contains("s")) simulationManager->cellSettingsMaster->slotsPerSquare = options->value("s").toInt();
    if (options->contains("t")) simulationManager->simulationSettings->toroidal = boolStringToBool(options->value("t"));
    if (options->contains("u")) simulationManager->cellSettingsMaster->mutate = options->value("u").toInt();
    if (options->contains("v")) simulationManager->simulationLog->csvOutput = boolStringToBool(options->value("v"));
    if (options->contains("w")) simulationManager->simulationSettings->environmentInterpolate = boolStringToBool(options->value("w"));
    if (options->contains("z")) simulationManager->simulationSettings->genomeSize = options->value("z").toInt();
    if (options->contains("interactblocks")) simulationManager->cellSettingsMaster->interactBlocks = boolStringToBool(options->value("interactblocks"));
    if (options->contains("multibreedlist")) simulationManager->cellSettingsMaster->multiBreedList = boolStringToBool(options->value("multibreedlist"));
    if (options->contains("interactrate")) simulationManager->cellSettingsMaster->interactions = options->value("interactrate").toInt();
    if (options->contains("cropping")) simulationManager->cellSettingsMaster->croppingFrequency = options->value("cropping").toInt();
    if (options->contains("interactfitness")) simulationManager->cellSettingsMaster->interactFitness = boolStringToBool(options->value("interactfitness"));
    if (options->contains("interactenergy")) simulationManager->cellSettingsMaster->interactEnergy = boolStringToBool(options->value("interactenergy"));
    if (options->contains("v2log"))
    {
        logTextEdit->setHtml(simulationManager->simulationLog->printDefaultLogSpeciesText());
        iterationTextEdit->setHtml(simulationManager->simulationLog->printDefaultLogIterationText());
        headerTextEdit->setHtml(simulationManager->simulationLog->printDefaultHeaderText());
        simulationManager->simulationLog->setHeaderTextFromGUI(mainWindow->headerTextEdit->toHtml());
        simulationManager->simulationLog->setIterationTextFromGUI(mainWindow->iterationTextEdit->toHtml());
        simulationManager->simulationLog->setSpeciestTextFromGUI(mainWindow->logTextEdit->toHtml());
        simulationManager->simulationSettings->logging = true;
    }
    if (options->contains("minpredatorscore")) simulationManager->cellSettingsMaster->minDeltaPredatorness = options->value("minpredatorscore").toInt();
    if (options->contains("predationefficiency")) simulationManager->cellSettingsMaster->predationEfficiency = options->value("predationefficiency").toInt();

    if (options->contains("nonspatial")) simulationManager->simulationSettings->nonspatial = boolStringToBool(options->value("nonspatial"));
    if (options->contains("polling"))
    {
        qInfo() << "Setting polling to " << options->value("polling").toInt();
        refreshRate = options->value("polling").toInt();
    }
    if (options->contains("minspeciessize"))
    {
        qInfo() << "Minspec is " << options->value("minspeciessize").toInt();
        simulationManager->simulationSettings->minSpeciesSize = options->value("minspeciessize").toULongLong();
    }
    if (options->contains("fitnesstarget")) simulationManager->cellSettingsMaster->target = options->value("fitnesstarget").toInt();

    if (options->contains("breed"))
    {
        simulationManager->cellSettingsMaster->obligateSexual = false;
        simulationManager->cellSettingsMaster->facultativeSexual = false;
        simulationManager->cellSettingsMaster->variableBreed = false;
        simulationManager->cellSettingsMaster->asexual = false;

        bool error = false;
        //Default to obligate
        if (options->value("breed").toLower() == "facultative") simulationManager->cellSettingsMaster->facultativeSexual = true;
        else if (options->value("breed").toLower() == "variable")
        {
            simulationManager->cellSettingsMaster->variableBreed = true;
            ui->actionRecombination_logging->setChecked(true);
        }
        else if (options->value("breed").toLower() == "asexual") simulationManager->cellSettingsMaster->asexual = true;
        else
        {
            simulationManager->cellSettingsMaster->obligateSexual = true;
            if (options->value("breed").toLower() != "obligate") error = true;
        }

        if (error) qInfo() << "Didn't understand your breed tag. Defaulting to obligate.";
        else qInfo() << "Breed is set to " << options->value("breed").toLower();
    }

    if (options->contains("li_population")) savePopulationCount->setChecked(boolStringToBool(options->value("li_population")));
    if (options->contains("li_fitness")) saveMeanFitness->setChecked(boolStringToBool(options->value("li_fitness")));
    if (options->contains("li_coding")) saveVisSysOneAsColour->setChecked(boolStringToBool(options->value("li_coding")));
    if (options->contains("li_noncoding")) saveVisSysTwoAsColour->setChecked(boolStringToBool(options->value("li_noncoding")));
    if (options->contains("li_species")) saveSpecies->setChecked(boolStringToBool(options->value("li_species")));
    if (options->contains("li_settles")) saveSettles->setChecked(boolStringToBool(options->value("li_settles")));
    if (options->contains("li_fails")) saveFailsSettles->setChecked(boolStringToBool(options->value("li_fails")));
    if (options->contains("li_environment")) saveEnvironment->setChecked(boolStringToBool(options->value("li_environment")));

    if (options->contains("sys_fitness"))simulationManager->environmentalFitnessSytem->setGenomeWordsFromString(options->value("sys_fitness"), simulationManager->simulationSettings->genomeSize);
    if (options->contains("sys_breed"))simulationManager->breedSystem->setGenomeWordsFromString(options->value("sys_breed"), simulationManager->simulationSettings->genomeSize);
    if (options->contains("sys_mutate"))simulationManager->mutationSystem->setGenomeWordsFromString(options->value("sys_mutate"), simulationManager->simulationSettings->genomeSize);
    if (options->contains("sys_var_mutate"))simulationManager->variableMutateSystem->setGenomeWordsFromString(options->value("sys_var_mutate"), simulationManager->simulationSettings->genomeSize);
    if (options->contains("sys_var_breed"))simulationManager->variableBreedSystem->setGenomeWordsFromString(options->value("sys_var_breed"), simulationManager->simulationSettings->genomeSize);
    if (options->contains("sys_pathogens"))simulationManager->pathogensSystem->setGenomeWordsFromString(options->value("sys_pathogens"), simulationManager->simulationSettings->genomeSize);
    if (options->contains("sys_species_ID"))simulationManager->speciesIDSytem->setGenomeWordsFromString(options->value("sys_species_ID"), simulationManager->simulationSettings->genomeSize);
    if (options->contains("sys_interactions"))simulationManager->interactionSystem->setGenomeWordsFromString(options->value("sys_interactions"), simulationManager->simulationSettings->genomeSize);
    if (options->contains("sys_visualisation"))simulationManager->visualisationSystem->setGenomeWordsFromString(options->value("sys_visualisation"), simulationManager->simulationSettings->genomeSize);
    if (options->contains("sys_visualisation2"))simulationManager->visualisationSystem2->setGenomeWordsFromString(options->value("sys_visualisation2"), simulationManager->simulationSettings->genomeSize);

    //RJG - linkages require the image sequence
    if (options->contains("L1_variable") && options->contains("L1_imageSequence"))
    {
        QHash<QString, linkageClass>::iterator i = simulationManager->linkages.find(options->value("L1_variable"));
        bool flag = true;
        if (i == simulationManager->linkages.end())
        {
            qInfo() << "Linked variable not recognised -  do you have a typo? No linkage will be set.";
            flag = false;
        }

        QDir d(options->value("L1_imageSequence"));
        QStringList filters = {"*.bmp", "*.png"};
        d.setNameFilters(filters);
        d.setSorting(QDir::Name);
        QStringList files = d.entryList();

        if (files.length() == 0)
        {
            qInfo() << "No images loaded for linkage - is the path to the folder correct? Linkage will not be loaded.";
            flag = false;
        }

        if (flag)
        {
            //RJG - set linkage to on
            i->set = true;
            simulationManager->simulationSettings->linkagesOn = true;

            //RJG - Add path to images
            for (int i = 0; i < files.count(); i++) files[i] = options->value("L1_imageSequence") + files[i];

            //RJG - sort out rate
            int rate = 100;
            if (options->contains("L1_change_rate")) rate = options->value("L1_change_rate").toInt();
            //RJG - image sequence will be a nullptr from the constructor when launched from command line
            i->imageSequence = new ImageSequence(files, rate);

            //RJG - set mode if requested (default in constructor is static)
            if (options->contains("L1_mode"))
            {
                if (options->value("L1_mode").toLower() == "static") i->mode = ENV_MODE_STATIC;
                if (options->value("L1_mode").toLower() == "bounce") i->mode = ENV_MODE_BOUNCE;
                if (options->value("L1_mode").toLower() == "loop") i->mode = ENV_MODE_LOOP;
                if (options->value("L1_mode").toLower() == "once") i->mode = ENV_MODE_ONCE;
            }

            //RJG - and interpolate if required
            if (options->contains("L1_interpolate")) i->interpolate = boolStringToBool(options->value("L1_interpolate"));

            qInfo() << "Linkage " << i->variable << " set, with change rate of " << i->imageSequence->returnChangeRate() << " mode " << i->mode << " and interpolate as " << i->interpolate <<
                    " controlled by masks: " << i->imageSequence->returnFileList();
        }
    }
    else if (options->contains("L1_variable") || options->contains("L1_imageSequence")) qInfo() << "Don't have enough information to set up linkage 1 - need minimum of lined variable and image sequence.";

    if (options->contains("L2_variable") && options->contains("L2_imageSequence"))
    {
        QHash<QString, linkageClass>::iterator i = simulationManager->linkages.find(options->value("L2_variable"));
        bool flag = true;
        if (i == simulationManager->linkages.end())
        {
            qInfo() << "Linked variable not recognised -  do you have a typo? No linkage will be set.";
            flag = false;
        }

        QDir d(options->value("L2_imageSequence"));
        QStringList filters = {"*.bmp", "*.png"};
        d.setNameFilters(filters);
        d.setSorting(QDir::Name);
        QStringList files = d.entryList();

        if (files.length() == 0)
        {
            qInfo() << "No images loaded for linkage - is the path to the folder correct? Linkage will not be loaded.";
            flag = false;
        }

        if (flag)
        {
            //RJG - set linkage to on
            i->set = true;
            simulationManager->simulationSettings->linkagesOn = true;

            //RJG - Add path to images
            for (int i = 0; i < files.count(); i++) files[i] = options->value("L2_imageSequence") + files[i];

            //RJG - sort out rate
            int rate = 100;
            if (options->contains("L2_change_rate")) rate = options->value("L2_change_rate").toInt();
            //RJG - image sequence will be a nullptr from the constructor when launched from command line
            i->imageSequence = new ImageSequence(files, rate);

            //RJG - set mode if requested (default in constructor is static)
            if (options->contains("L2_mode"))
            {
                if (options->value("L2_mode").toLower() == "static") i->mode = ENV_MODE_STATIC;
                if (options->value("L2_mode").toLower() == "bounce") i->mode = ENV_MODE_BOUNCE;
                if (options->value("L2_mode").toLower() == "loop") i->mode = ENV_MODE_LOOP;
                if (options->value("L2_mode").toLower() == "once") i->mode = ENV_MODE_ONCE;
            }

            //RJG - and interpolate if required
            if (options->contains("L2_interpolate")) i->interpolate = boolStringToBool(options->value("L2_interpolate"));

            qInfo() << "Linkage " << i->variable << " set, with change rate of " << i->imageSequence->returnChangeRate() << " mode " << i->mode << " and interpolate as " << i->interpolate <<
                    " controlled by masks: " << i->imageSequence->returnFileList();
        }
    }
    else if (options->contains("L2_variable") || options->contains("L2_imageSequence")) qInfo() << "Don't have enough information to set up linkage 2 - need minimum of lined variable and image sequence.";

    if (options->contains("noselection")) simulationManager->cellSettingsMaster->noSelection = boolStringToBool(options->value("noselection"));
    if (options->contains("variablemutate"))
    {
        simulationManager->cellSettingsMaster->variableMutate = boolStringToBool(options->value("variablemutate"));
        if (simulationManager->cellSettingsMaster->variableMutate) qInfo() << "Varible mutate is on";
        else qInfo() << "Varible mutate is off";
        ui->actionVariable_mutation_logging->setChecked(simulationManager->cellSettingsMaster->variableMutate);
    }

    if (options->contains("pathogens"))
    {
        qInfo() << "Setting pathogens to "  <<  boolStringToBool(options->value("pathogens"));
        simulationManager->cellSettingsMaster->pathOn = boolStringToBool(options->value("pathogens"));
    }

    if (options->contains("pathogenmutate"))
    {
        qInfo() << "Setting pathogen mutate to " << options->value("pathogenmutate").toInt();
        simulationManager->cellSettingsMaster->pathogenMutate = options->value("pathogenmutate").toInt();
    }

    if (options->contains("pathogenfrequency"))
    {
        qInfo() << "Setting pathogen frequency to " << options->value("pathogenfrequency").toInt();
        simulationManager->cellSettingsMaster->pathogenFrequency = options->value("pathogenfrequency").toInt();
    }

    if (options->contains("pathogenevolve"))
    {
        //  PATH_MODE_EVOLVE is 1 - so if this is 1, we are good
        qInfo() << "Setting pathogen evolve to " << boolStringToBool(options->value("pathogenevolve"));
        simulationManager->simulationSettings->pathogenMode =  boolStringToBool(options->value("pathogenevolve"));
    }

    if (options->contains("customlogging"))
    {
        bool customLogging = boolStringToBool(options->value("customlogging"));
        if (customLogging)
        {
            qInfo() << "Custom logging enabled";
            ui->actionFitness_logging_to_File->setChecked(true);
            ui->actionRecombination_logging->setChecked(true);
            ui->actionVariable_mutation_logging->setChecked(true);
            ui->actionSpeciation_logging->setChecked(true);
            ui->actionVariable_hgt_prob_logging->setChecked(true);
            ui->actionVariable_hgt_len_logging->setChecked(true);
            ui->actionVariable_hgt_id_logging->setChecked(true);
        }
    }

    if (options->contains("disparityLogging"))
    {
        qInfo() << "Disparity logging enabled";
        ui->actionDisparity_logging->setChecked(true);
    }

    updateGUIFromVariables();

    if (options->contains("auto"))
    {
        autoFromCommand = true;

        if (options->contains("nogui"))
        {
            simulationManager->simulationSettings->gui = true;
        }

        //RJG - if launching from command line it makes more sense for default location to be with executable, I think
        if (!options->contains("j"))
        {
            QString programPath(QCoreApplication::applicationDirPath());
            programPath.append(QDir::separator());
            globalSavePath->setText(programPath);
        }

        qInfo().noquote() << "After parsing the command line, REvoSim has the settings shown below.\n\n" << simulationManager->simulationLog->printSettings();

        simulationManager->setupRun();

        int autoFromCommandValue = options->value("auto").toInt();
        if (autoFromCommandValue == 0) // run for ever
        {
            qInfo() << "Output files will be placed in " << getSavePath();
            qInfo() << "Auto-starting sim, which will run indefinitely";
            startSimulation();
        }
        else
        {
            qInfo() << "Output files will be placed in" << getSavePath();
            qInfo() << "Auto-starting sim - will run for" << autoFromCommandValue << "iterations.";
            runForNSimulation(autoFromCommandValue);
        }

        qInfo() << "Quitting REvoSim";
        exit(0); //qApp.quit not working - something to do with threads. This does the job though
    }

    delete options;
}


void MainWindow::toggleTestUI()
{

    if (!ui->testLog->isVisible())
    {
        ui->testLog->setVisible(true);
        ui->frame_2->setVisible(false);
    }
    else
    {
        ui->testLog->setVisible(false);
        ui->frame_2->setVisible(true);
        return;
    }
}

void MainWindow::doTests()
{
    toggleTestUI();

    int testCount = 10;
    int testStart = 0;

    QStringList items;
    items << tr("All");
    for (int i = 0; i < testCount; i++) items << QString::number(i);

    bool ok;
    QString item = QInputDialog::getItem(this, tr("REvoSim tests"), tr("Which test?"), items, 0, false, &ok);

    setStatusBarText("Starting tests");
    QProgressDialog progress("Doing tests...", "Cancel", 0, testCount, this);
    progress.setWindowModality(Qt::WindowModal);
    qApp->processEvents();

    if (ok && !item.isEmpty())
    {
        if (item != "All")
        {
            testCount = item.toInt() + 1;
            testStart = item.toInt();
            progress.close();
        }
        else
        {
            progress.setValue(0);
            progress.show();
            qApp->processEvents();
        }
    }
    else
    {
        ui->testLog->setVisible(false);
        ui->frame_2->setVisible(true);
        setStatusBarText("Tests cancelled");
        return;
    }

    test testObject;
    for (int testNumber = testStart; testNumber < testCount; testNumber++)
    {
        if (item == "All")
        {
            progress.setValue(testNumber);
            if (progress.wasCanceled()) break;
            qApp->processEvents();
        }

        ui->testLog->setAlignment(Qt::AlignLeft);
        ui->testLog->append(QString("<h2>Test %1</h2>").arg(testNumber));

        QString testResult;
        if (testObject.callTest(testNumber, testResult))
        {
            testResult.prepend("<p>");
            testResult.append("</p>");
            //Replace new lines with breaks - do it here because this makes use of a load of print functions that return \n
            testResult.replace("\n", "<br />");
            ui->testLog->append(testResult);
        }
        else
        {
            testResult.prepend("<p><font color=\"Lime\">********Test Failed ********\n");
            testResult.append("</font></p>");
            testResult.replace("\n", "<br />");
            ui->testLog->append(testResult);
        }

        ui->testLog->setAlignment(Qt::AlignCenter);
        ui->testLog->append("<p>*****************************************</p>");

    }

    if (item != "All") progress.close();

    setStatusBarText("Tests done.");
}
