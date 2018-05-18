/*!
 * @file
 * Main Window
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2018 by Mark Sutton, Russell Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "reseed.h"
#include "analyser.h"
#include "resizecatcher.h"

#include <QTextStream>
#include <QInputDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGraphicsPixmapItem>
#include <QDockWidget>
#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QFormLayout>
#include <QStringList>
#include <QMessageBox>
#include <QActionGroup>
#include <QDataStream>
#include <QStringList>
#include <QFile>
#include <QXmlStreamReader>
#include <QDesktopServices>
#include <QPushButton>

#include "analysistools.h"
#include "version.h"
#include "math.h"

#ifndef M_SQRT1_2 //not defined in all versions
#define M_SQRT1_2 0.7071067811865475
#endif

SimManager *TheSimManager;
MainWindow *MainWin;

#include <QThread>

/*

To do for paper:

To remove prior to release, but after all changes are complete:
-- Dual reseed
-- Remove all custom logging options (RJG is using these for research and projects, so needs to keep them in master fork).
-- Variable breeding  (RJG is using these for research and projects, so needs to keep them in master fork).
-- Pathogens Layer  (RJG is using these for research and projects, so needs to keep them in master fork).

To do in revisions:
-- Further comment code
-- Standardise case throughout the code, and also variable names in a sensible fashion
-- Rename "generations" variable "iterations", which is more correct
-- Settles vis - does this work at all?
-- Fails vis - check green scaling

To do long term:
-- Add variable mutation rate depent on population density:
---- Count number of filled slots (do as percentage of filled slots)
---- Use percentage to dictate probability of mutation (between 0 and 1), following standard normal distribution
---- But do this both ways around so really full mutation rate can be either very high, or very low

*/

/*!
 * \brief The Sleeper class
 */
class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};

/*!
 * \brief MainWindow::MainWindow
 * \param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    a = new Analyser; // so can delete next time!
    ui->setupUi(this);
    MainWin=this;

    //RJG - Output version, but also date compiled for clarity
    QString version;
    version.sprintf("%d.%d.%d",MAJORVERSION,MINORVERSION,PATCHVERSION);
    setWindowTitle(QString(PRODUCTNAME)+" v"+version+" - compiled - "+__DATE__);
    setWindowIcon(QIcon (":/icon.png"));

    //Install filter to catch resize events to central widget and deliver to mainwindow (handle dock resizes)
    ResizeCatcher *rescatch = new ResizeCatcher(this);
    ui->centralWidget->installEventFilter(rescatch);

    // Create Main Toolbar
    createMainToolbar();

    QObject::connect(ui->actionSave_settings, SIGNAL (triggered()), this, SLOT (save_settings()));
    QObject::connect(ui->actionLoad_settings, SIGNAL (triggered()), this, SLOT (load_settings()));
    QObject::connect(ui->actionCount_peaks, SIGNAL(triggered()), this, SLOT(on_actionCount_Peaks_triggered()));

    //RJG - set up settings docker.
    simulationSettingsDock = createSimulationSettingsDock();
    //----RJG - second settings docker.
    organismSettingsDock = createOrganismSettingsDock();
    //RJG - Third settings docker
    outputSettingsDock = createOutputSettingsDock();

    //RJG - Make docks tabbed
    tabifyDockWidget(organismSettingsDock,simulationSettingsDock);
    tabifyDockWidget(simulationSettingsDock,outputSettingsDock);

    //Hide docks by default
    organismSettingsDock->hide();
    simulationSettingsDock->hide();
    outputSettingsDock->hide();

    //Add Genome Comparison UI Dock
    ui->genomeComparisonDock->hide();
    genoneComparison = new GenomeComparison;
    QVBoxLayout *genomeLayout = new QVBoxLayout;
    genomeLayout->addWidget(genoneComparison);
    ui->genomeComparisonContent->setLayout(genomeLayout);

    //create scenes, add to the GVs
    envscene = new EnvironmentScene;
    ui->GV_Environment->setScene(envscene);
    envscene->mw=this;

    popscene = new PopulationScene;
    popscene->mw=this;
    ui->GV_Population->setScene(popscene);

    //add images to the scenes
    env_item= new QGraphicsPixmapItem();
    envscene->addItem(env_item);
    env_item->setZValue(0);

    pop_item = new QGraphicsPixmapItem();
    popscene->addItem(pop_item);

    pop_image=new QImage(gridX, gridY, QImage::Format_Indexed8);
    QVector <QRgb> clut(256);
    for (int ic=0; ic<256; ic++) clut[ic]=qRgb(ic,ic,ic);
    pop_image->setColorTable(clut);
    pop_image->fill(0);

    env_image=new QImage(gridX, gridY, QImage::Format_RGB32);
    env_image->fill(0);

    pop_image_colour=new QImage(gridX, gridY, QImage::Format_RGB32);
    env_image->fill(0);

    env_item->setPixmap(QPixmap::fromImage(*env_image));
    pop_item->setPixmap(QPixmap::fromImage(*pop_image));

    //ARTS - Population Window dropdown must be after settings dock setup
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
    ui->populationWindowComboBox->addItem("Population count",QVariant(0));
    ui->populationWindowComboBox->addItem("Mean fitnessFails (R-Breed, G=Settle)",QVariant(1));
    ui->populationWindowComboBox->addItem("Coding genome as colour",QVariant(2));
    ui->populationWindowComboBox->addItem("NonCoding genome as colour",QVariant(3));
    ui->populationWindowComboBox->addItem("Gene Frequencies",QVariant(4));
    //ui->populationWindowComboBox->addItem("Breed Attempts",QVariant(5));
    //ui->populationWindowComboBox->addItem("Breed Fails",QVariant(6));
    ui->populationWindowComboBox->addItem("Settles",QVariant(7));
    ui->populationWindowComboBox->addItem("Settle Fails",QVariant(8));
    //ui->populationWindowComboBox->addItem("Breed Fails 2",QVariant(9));
    ui->populationWindowComboBox->addItem("Species",QVariant(10));

    //ARTS -Population Window dropdown set current index. Note this value is the index not the data value.
    ui->populationWindowComboBox->setCurrentIndex(2);

    TheSimManager = new SimManager;

    pauseflag = false;

    //RJG - load default environment image to allow program to run out of box (quicker for testing)
    EnvFiles.append(":/EvoSim_default_env.png");
    CurrentEnvFile=0;
    TheSimManager->loadEnvironmentFromFile(1);

    FinishRun();//sets up enabling
    TheSimManager->SetupRun();
    NextRefresh=0;
    Report();

    //RJG - Set batch variables
    batch_running=false;
    runs=-1;
    batch_iterations=-1;
    batch_target_runs=-1;

    showMaximized();

    //RJG - seed pseudorandom numbers
    qsrand(QTime::currentTime().msec());
    //RJG - Now load randoms into program - portable rand is just plain pseudorandom number - initially used in makelookups (called from simmanager contructor) to write to randoms array
    int seedoffset = TheSimManager->portable_rand();
    QFile rfile(":/randoms.dat");
    if (!rfile.exists()) QMessageBox::warning(this,"Oops","Error loading randoms. Please do so manually.");
    rfile.open(QIODevice::ReadOnly);

    rfile.seek(seedoffset);

    //RJG - overwrite pseudorandoms with genuine randoms
    int i=rfile.read((char *)randoms,65536);
    if (i!=65536) QMessageBox::warning(this,"Oops","Failed to read 65536 bytes from file - random numbers may be compromised - try again or restart program");

    //RJG - fill cumulative_normal_distribution with numbers for variable breeding
    //These are a cumulative standard normal distribution from -3 to 3, created using the math.h complementary error function
    //Then scaled to zero to 32 bit rand max, to allow for probabilities within each iteration through a random number
    float x=-3., inc=(6./32.);
    for(int cnt=0;cnt<33;cnt++)
    {
        double NSDF=(0.5 * erfc(-(x) * M_SQRT1_2));
        cumulative_normal_distribution[cnt]=4294967296*NSDF;
        x+=inc;
    }

    //RJG - fill pathogen probability distribution as required so pathogens can kill critters
    //Start with linear, may want to change down the line.
    for(int cnt=0;cnt<65;cnt++)
        pathogen_prob_distribution[cnt]=(4294967296/2)+(cnt*(4294967295/128));
}

/*!
 * \brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete TheSimManager;
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
    ui->toolBar->setIconSize(QSize(25,25));
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

    //RJG - Connect button signals to slot.
    //Note for clarity:
    //Reset = start again with random individual.
    //Reseed = start again with user defined genome
    QObject::connect(startButton, SIGNAL(triggered()), this, SLOT(on_actionStart_Sim_triggered()));
    QObject::connect(runForButton, SIGNAL(triggered()), this, SLOT(on_actionRun_for_triggered()));
    QObject::connect(pauseButton, SIGNAL(triggered()), this, SLOT(on_actionPause_Sim_triggered()));
    QObject::connect(stopButton, SIGNAL(triggered()), this, SLOT(on_actionStop_Sim_triggered()));
    QObject::connect(resetButton, SIGNAL(triggered()), this, SLOT(on_actionReset_triggered()));
    QObject::connect(reseedButton, SIGNAL(triggered()), this, SLOT(on_actionReseed_triggered()));
    QObject::connect(runForBatchButton, SIGNAL(triggered()), this, SLOT(on_actionBatch_triggered()));
    QObject::connect(genomeComparisonButton, SIGNAL(triggered(bool)), this, SLOT(on_actionGenomeComparison_triggered(bool)));
    QObject::connect(settingsButton, SIGNAL(triggered()), this, SLOT(actionSettings_triggered()));
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

    QGridLayout *settings_grid = new QGridLayout;
    settings_grid->setAlignment(Qt::AlignTop);

    // Environment Settings
    QGridLayout *environmentSettingsGrid = new QGridLayout;

    QLabel *environment_label= new QLabel("Environmental Settings");
    environment_label->setStyleSheet("font-weight: bold");
    environmentSettingsGrid->addWidget(environment_label,0,1,1,2);

    QPushButton *changeEnvironmentFilesButton = new QPushButton("&Change Environment Files");
    changeEnvironmentFilesButton->setObjectName("changeEnvironmentFilesButton");
    changeEnvironmentFilesButton->setToolTip("<font>REvoSim allows you to customise the environment by loading one or more image files.</font>");
    environmentSettingsGrid->addWidget(changeEnvironmentFilesButton,1,1,1,2);
    connect(changeEnvironmentFilesButton, SIGNAL (clicked()), this, SLOT(actionEnvironment_Files_triggered()));

    QLabel *environment_rate_label = new QLabel("Environment refresh rate:");
    environment_rate_label->setToolTip("<font>This is the rate of change for the selected environmental images.</font>");
    environment_rate_spin = new QSpinBox;
    environment_rate_spin->setToolTip("<font>This is the rate of change for the selected environmental images.</font>");
    environment_rate_spin->setMinimum(0);
    environment_rate_spin->setMaximum(100000);
    environment_rate_spin->setValue(envchangerate);
    environmentSettingsGrid->addWidget(environment_rate_label,2,1);
    environmentSettingsGrid->addWidget(environment_rate_spin,2,2);
    //RJG - Note in order to use a lamda not only do you need to use C++11, but there are two valueChanged signals for spinbox - and int and a string. Need to cast it to an int
    connect(environment_rate_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i ) { envchangerate=i; });

    QLabel *environment_mode_label = new QLabel("Environment mode:");
    environmentSettingsGrid->addWidget(environment_mode_label,3,1,1,2);

    QGridLayout *environmentModeGrid = new QGridLayout;
    environmentModeStaticButton = new QRadioButton("Static");
    environmentModeStaticButton->setToolTip("<font>'Static' uses a single environment image.</font>");
    environmentModeOnceButton = new QRadioButton("Once");
    environmentModeOnceButton->setToolTip("<font>'Once' uses each image only once, simulation stops after last image.</font>");
    environmentModeLoopButton = new QRadioButton("Loop");
    environmentModeLoopButton->setToolTip("<font>'Loop' uses each image in order in a loop</font>");
    environmentModeBounceButton = new QRadioButton("Bounce");
    environmentModeBounceButton->setToolTip("<font>'Bounce' rebounds between the first and last image in a loop.</font>");
    QButtonGroup* environmentModeButtonGroup = new QButtonGroup;
    environmentModeButtonGroup->addButton(environmentModeStaticButton,ENV_MODE_STATIC);
    environmentModeButtonGroup->addButton(environmentModeOnceButton,ENV_MODE_ONCE);
    environmentModeButtonGroup->addButton(environmentModeLoopButton,ENV_MODE_LOOP);
    environmentModeButtonGroup->addButton(environmentModeBounceButton,ENV_MODE_BOUNCE);
    environmentModeLoopButton->setChecked(true);
    environmentModeGrid->addWidget(environmentModeStaticButton,1,1,1,2);
    environmentModeGrid->addWidget(environmentModeOnceButton,1,2,1,2);
    environmentModeGrid->addWidget(environmentModeLoopButton,2,1,1,2);
    environmentModeGrid->addWidget(environmentModeBounceButton,2,2,1,2);
    connect(environmentModeButtonGroup, (void(QButtonGroup::*)(int))&QButtonGroup::buttonClicked,[=](const int &i) { environment_mode_changed(i,false); });
    environmentSettingsGrid->addLayout(environmentModeGrid,4,1,1,2);

    interpolateCheckbox = new QCheckBox("Interpolate between images");
    interpolateCheckbox->setChecked(environment_interpolate);
    interpolateCheckbox->setToolTip("<font>Turning this ON will interpolate the environment between individual images.</font>");
    environmentSettingsGrid->addWidget(interpolateCheckbox,5,1,1,2);
    connect(interpolateCheckbox,&QCheckBox::stateChanged,[=](const bool &i) { environment_interpolate=i; });

    toroidal_checkbox = new QCheckBox("Toroidal environment");
    toroidal_checkbox->setChecked(toroidal);
    toroidal_checkbox->setToolTip("<font>Turning this ON will allow dispersal of progeny in an unbounded warparound environment. Progeny leaving one side of the population window will immediately reappear on the opposite side.</font>");
    environmentSettingsGrid->addWidget(toroidal_checkbox,6,1,1,2);
    connect(toroidal_checkbox,&QCheckBox::stateChanged,[=](const bool &i) { toroidal=i; });

    // Simulation Size Settings
    QGridLayout *simulationSizeSettingsGrid = new QGridLayout;

    QLabel *simulation_size_label= new QLabel("Simulation size");
    simulation_size_label->setStyleSheet("font-weight: bold");
    simulationSizeSettingsGrid->addWidget(simulation_size_label,0,1,1,2);

    QLabel *gridX_label = new QLabel("Grid X:");
    gridX_label->setToolTip("<font>Number of grid cells on the <i>x</i> axis.</font>");
    gridX_spin = new QSpinBox;
    gridX_spin->setMinimum(1);
    gridX_spin->setMaximum(256);
    gridX_spin->setValue(gridX);
    gridX_spin->setToolTip("<font>Number of grid cells on the <i>x</i> axis.</font>");
    simulationSizeSettingsGrid->addWidget(gridX_label,2,1);
    simulationSizeSettingsGrid->addWidget(gridX_spin,2,2);
    connect(gridX_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) { int oldrows=gridX; gridX=i;redoImages(oldrows,gridY);});

    QLabel *gridY_label = new QLabel("Grid Y:");
    gridY_label->setToolTip("<font>Number of grid cells on the <i>y</i> axis.</font>");
    gridY_spin = new QSpinBox;
    gridY_spin->setMinimum(1);
    gridY_spin->setMaximum(256);
    gridY_spin->setValue(gridY);
    gridY_spin->setToolTip("<font>Number of grid cells on the <i>y</i> axis.</font>");
    simulationSizeSettingsGrid->addWidget(gridY_label,3,1);
    simulationSizeSettingsGrid->addWidget(gridY_spin,3,2);
    connect(gridY_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) {int oldcols=gridY; gridY=i;redoImages(gridX,oldcols);});

    QLabel *slots_label = new QLabel("Slots:");
    slots_label->setToolTip("<font>Number of slots per grid cell.</font>");
    slots_spin = new QSpinBox;
    slots_spin->setMinimum(1);
    slots_spin->setMaximum(256);
    slots_spin->setValue(slotsPerSq);
    slots_spin->setToolTip("<font>Number of slots per grid cell.</font>");
    simulationSizeSettingsGrid->addWidget(slots_label,4,1);
    simulationSizeSettingsGrid->addWidget(slots_spin,4,2);
    connect(slots_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) { slotsPerSq=i;redoImages(gridX,gridY); });

    // Simulation Settings
    QGridLayout *simulationSettingsGrid = new QGridLayout;

    QLabel *simulation_settings_label= new QLabel("Simulation settings");
    simulation_settings_label->setStyleSheet("font-weight: bold");
    simulationSettingsGrid->addWidget(simulation_settings_label,0,1,1,2);

    QLabel *target_label = new QLabel("Fitness target:");
    target_label->setToolTip("<font>Target value effects the fitness landscape. See manual for more details.</font>");
    target_spin = new QSpinBox;
    target_spin->setMinimum(1);
    target_spin->setMaximum(96);
    target_spin->setValue(target);
    target_spin->setToolTip("<font>Target value effects the fitness landscape. See manual for more details.</font>");
    simulationSettingsGrid->addWidget(target_label,1,1);
    simulationSettingsGrid->addWidget(target_spin,1,2);
    connect(target_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) { target=i; });

    QLabel *energy_label = new QLabel("Energy input:");
    energy_label->setToolTip("<font>Energy level given to new organisms.</font>");
    energy_spin = new QSpinBox;
    energy_spin->setMinimum(1);
    energy_spin->setMaximum(20000);
    energy_spin->setValue(food);
    energy_spin->setToolTip("<font>Energy level given to new organisms.</font>");
    simulationSettingsGrid->addWidget(energy_label,2,1);
    simulationSettingsGrid->addWidget(energy_spin,2,2);
    connect(energy_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) { food=i; });

    QLabel *settleTolerance_label = new QLabel("Settle tolerance:");
    settleTolerance_label->setToolTip("<font>Defines the range of environments an organism can settle into.</font>");
    settleTolerance_spin = new QSpinBox;
    settleTolerance_spin->setMinimum(1);
    settleTolerance_spin->setMaximum(30);
    settleTolerance_spin->setValue(settleTolerance);
    settleTolerance_spin->setToolTip("<font>Defines the range of environments an organism can settle into.</font>");
    simulationSettingsGrid->addWidget(settleTolerance_label,3,1);
    simulationSettingsGrid->addWidget(settleTolerance_spin,3,2);
    connect(settleTolerance_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) { settleTolerance=i; });

    recalcFitness_checkbox = new QCheckBox("Recalculate fitness");
    recalcFitness_checkbox->setChecked(recalcFitness);
    recalcFitness_checkbox->setToolTip("<font>Turning on recalculates the fitness of each organism every iteration.</font>");
    simulationSettingsGrid->addWidget(recalcFitness_checkbox,4,1,1,2);
    connect(recalcFitness_checkbox,&QCheckBox::stateChanged,[=](const bool &i) { recalcFitness=i; });

    //Phylogeny Settings
    QGridLayout *phylogenySettingsGrid = new QGridLayout;

    QLabel *phylogeny_settings_label= new QLabel("Phylogeny settings");
    phylogeny_settings_label->setStyleSheet("font-weight: bold");
    phylogenySettingsGrid->addWidget(phylogeny_settings_label,0,1,1,1);

    QGridLayout *phylogeny_grid = new QGridLayout;
    phylogeny_off_button = new QRadioButton("Off");
    phylogeny_off_button->setToolTip("<font>Turns phylogeny logging off.</font>");
    basic_phylogeny_button = new QRadioButton("Basic");
    basic_phylogeny_button->setToolTip("<font>Turns on basic phylogeny logging.</font>");
    phylogeny_button = new QRadioButton("Phylogeny");
    phylogeny_button->setToolTip("<font>Turns on phylogeny logging.</font>");
    phylogeny_and_metrics_button = new QRadioButton("Phylogeny and metrics");
    phylogeny_and_metrics_button->setToolTip("<font>Turns on phylogeny and metrics logging.</font>");
    QButtonGroup* phylogeny_button_group = new QButtonGroup;
    phylogeny_button_group->addButton(phylogeny_off_button,SPECIES_MODE_NONE);
    phylogeny_button_group->addButton(basic_phylogeny_button,SPECIES_MODE_BASIC);
    phylogeny_button_group->addButton(phylogeny_button,SPECIES_MODE_PHYLOGENY);
    phylogeny_button_group->addButton(phylogeny_and_metrics_button,SPECIES_MODE_PHYLOGENY_AND_METRICS);
    basic_phylogeny_button->setChecked(true);
    phylogeny_grid->addWidget(phylogeny_off_button,1,1,1,2);
    phylogeny_grid->addWidget(basic_phylogeny_button,1,2,1,2);
    phylogeny_grid->addWidget(phylogeny_button,2,1,1,2);
    phylogeny_grid->addWidget(phylogeny_and_metrics_button,2,2,1,2);
    connect(phylogeny_button_group, (void(QButtonGroup::*)(int))&QButtonGroup::buttonClicked,[=](const int &i) { species_mode_changed(i, false); });
    phylogenySettingsGrid->addLayout(phylogeny_grid,1,1,1,2);

    //ARTS - Dock Grid Layout
    settings_grid->addLayout(environmentSettingsGrid,0,1);
    settings_grid->addLayout(simulationSizeSettingsGrid,1,1);
    settings_grid->addLayout(simulationSettingsGrid,2,1);
    settings_grid->addLayout(phylogenySettingsGrid,3,1);

    QWidget *settings_layout_widget = new QWidget;
    settings_layout_widget->setLayout(settings_grid);
    settings_layout_widget->setMinimumWidth(300);
    simulationSettingsDock->setWidget(settings_layout_widget);
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

    QGridLayout *output_settings_grid = new QGridLayout;
    output_settings_grid->setAlignment(Qt::AlignTop);

    //ARTS - Output Save Path
    QGridLayout *savePathGrid = new QGridLayout;
    QLabel *savePathLabel = new QLabel("Output save path");
    savePathLabel->setObjectName("savePathLabel");
    savePathGrid->addWidget(savePathLabel,1,1,1,2);
    QString program_path(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    program_path.append("/");
    path = new QLineEdit(program_path);
    path->setToolTip("<font>System path where all logs, images, and output data is saved.</font>");
    savePathGrid->addWidget(path,2,1,1,2);
    QPushButton *changePathButton = new QPushButton("&Change");
    changePathButton->setToolTip("<font>System path where all logs, images, and output data is saved.</font>");
    changePathButton->setObjectName("changePathButton");
    savePathGrid->addWidget(changePathButton,3,1,1,2);
    connect(changePathButton, SIGNAL (clicked()), this, SLOT(changepath_triggered()));

    //ARTS - Refresh/Polling Rate
    QGridLayout *pollingRateGrid = new QGridLayout;
    QLabel *pollingRateLabel = new QLabel("Refresh/Polling Rate");
    pollingRateLabel->setObjectName("pollingRateLabel");
    pollingRateGrid->addWidget(pollingRateLabel,1,1,1,2);

    RefreshRate=50;
    QLabel *refreshRateLabel = new QLabel("Refresh/polling rate:");
    refreshRateLabel->setObjectName("refreshRateLabel");
    refreshRateLabel->setToolTip("<font>Number of iteration between each logging and screen data refresh event.</font>");
    refreshRateSpin = new QSpinBox;
    refreshRateSpin->setMinimum(1);
    refreshRateSpin->setMaximum(10000);
    refreshRateSpin->setValue(RefreshRate);
    refreshRateSpin->setToolTip("<font>Number of iteration between each logging and screen data refresh event.</font>");
    pollingRateGrid->addWidget(refreshRateLabel,2,1);
    pollingRateGrid->addWidget(refreshRateSpin,2,2);
    connect(refreshRateSpin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) { RefreshRate=i; });

    //ARTS - Logging: Population & Environment
    QGridLayout *images_grid = new QGridLayout;

    QLabel *imagesLabel= new QLabel("Logging: Population/Enivronment");
    imagesLabel->setObjectName("imagesLabel");
    images_grid->addWidget(imagesLabel,1,1,1,2);

    save_population_count = new QCheckBox("Population count");
    save_population_count->setToolTip("<font>Turn on/off to log the 'Population Count' as an image.</font>");
    images_grid->addWidget(save_population_count,2,1,1,1);
    save_mean_fitness = new QCheckBox("Mean fitness");
    save_mean_fitness->setToolTip("<font>Turn on/off to log the 'Mean Fitness' as an image.</font>");
    images_grid->addWidget(save_mean_fitness,2,2,1,1);
    save_coding_genome_as_colour = new QCheckBox("Coding genome");
    save_coding_genome_as_colour->setToolTip("<font>Turn on/off to log the 'Coding Genome' as an image.</font>");
    images_grid->addWidget(save_coding_genome_as_colour,3,1,1,1);
    save_non_coding_genome_as_colour = new QCheckBox("Noncoding genome");
    save_non_coding_genome_as_colour->setToolTip("<font>Turn on/off to log the 'Noncoding Genome' as an image.</font>");
    images_grid->addWidget(save_non_coding_genome_as_colour,3,2,1,1);
    save_species = new QCheckBox("Species");
    save_species->setToolTip("<font>Turn on/off to log the 'Species' as an image.</font>");
    images_grid->addWidget(save_species,4,1,1,1);
    save_gene_frequencies = new QCheckBox("Gene frequencies");
    save_gene_frequencies->setToolTip("<font>Turn on/off to log the 'Gene Frequencies' as an image.</font>");
    images_grid->addWidget(save_gene_frequencies,4,2,1,1);
    save_settles = new QCheckBox("Settles");
    save_settles->setToolTip("<font>Turn on/off to log the 'Settles' as an image.</font>");
    images_grid->addWidget(save_settles,5,1,1,1);
    save_fails_settles = new QCheckBox("Fails + Settles");
    save_fails_settles->setToolTip("<font>Turn on/off to log the 'Fails + Settles' as an image.</font>");
    images_grid->addWidget(save_fails_settles,5,2,1,1);
    save_environment = new QCheckBox("Environment");
    save_environment->setToolTip("<font>Turn on/off to log the 'Environment' as an image.</font>");
    images_grid->addWidget(save_environment,6,1,1,1);

    QCheckBox *saveAllImagesCheckbox = new QCheckBox("All");
    saveAllImagesCheckbox->setObjectName("saveAllImagesCheckbox");
    saveAllImagesCheckbox->setToolTip("<font>Toggle all/none of the above checkboxes.</font>");
    images_grid->addWidget(saveAllImagesCheckbox,6,2,1,1);
    QObject::connect(saveAllImagesCheckbox, SIGNAL (toggled(bool)), this, SLOT(save_all_checkbox_state_changed(bool)));

    //ARTS - Logging to text file
    QGridLayout *fileLoggingGrid = new QGridLayout;

    QLabel *outputSettingsLabel= new QLabel("Logging: To Text File(s)");
    outputSettingsLabel->setObjectName("outputSettingsLabel");
    fileLoggingGrid->addWidget(outputSettingsLabel,1,1,1,2);

    logging_checkbox = new QCheckBox("Write Log Files");
    logging_checkbox->setChecked(logging);
    logging_checkbox->setToolTip("<font>Turn on/off this option to write to a text log file every refresh/poll.</font>");
    fileLoggingGrid->addWidget(logging_checkbox,2,1,1,2);
    connect(logging_checkbox,&QCheckBox::stateChanged,[=](const bool &i) { logging=i; });

    autodump_checkbox= new QCheckBox("Automatically create detailed log on batch runs");
    autodump_checkbox->setChecked(true);
    autodump_checkbox->setToolTip("<font>Turn on/off this option to automatically write detailed log after batch runs</font>");
    fileLoggingGrid->addWidget(autodump_checkbox,3,1,1,2);

    QPushButton *dump_nwk = new QPushButton("Write data (including tree) for current run");
    dump_nwk->setToolTip("<font>Use this option to manually trigger the writing of a detailed log after a run.</font>");
    fileLoggingGrid->addWidget(dump_nwk,4,1,1,2);
    connect(dump_nwk , SIGNAL (clicked()), this, SLOT(dump_run_data()));

    exclude_without_descendants_checkbox = new QCheckBox("Exclude species without descendents");
    exclude_without_descendants_checkbox->setChecked(allowExcludeWithDescendants);
    exclude_without_descendants_checkbox->setToolTip("<font>Use this option to exclude species without descendents.</font>");
    fileLoggingGrid->addWidget(exclude_without_descendants_checkbox,5,1,1,1);
    connect(exclude_without_descendants_checkbox,&QCheckBox::stateChanged,[=](const bool &i) { allowExcludeWithDescendants=i; });

    QLabel *Min_species_size_label = new QLabel("Minimum species size:");
    Min_species_size_label->setToolTip("<font>Selects the minimum number of organisms needed to define a species. Min = 0; Max = 1000000.</font>");
    QSpinBox *Min_species_size_spin = new QSpinBox;
    Min_species_size_spin->setToolTip("<font>Selects the minimum number of organisms needed to define a species. Min = 0; Max = 1000000.</font>");
    Min_species_size_spin->setMinimum(0);
    Min_species_size_spin->setMaximum(1000000);
    Min_species_size_spin->setValue(minspeciessize);
    fileLoggingGrid->addWidget(Min_species_size_label,6,1);
    fileLoggingGrid->addWidget(Min_species_size_spin,6,2);
    connect(Min_species_size_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) { minspeciessize=i; });

    //ARTS - Advanced
    QGridLayout *advancedLoggingGrid = new QGridLayout;

    QLabel *advancedSettingsLabel= new QLabel("Advanced");
    advancedSettingsLabel->setObjectName("advancedSettingsLabel");
    advancedLoggingGrid->addWidget(advancedSettingsLabel,1,1,1,2);

    gui_checkbox = new QCheckBox("Don't update GUI on refresh/poll");
    gui_checkbox->setChecked(gui);
    gui_checkbox->setToolTip("<font>Note: If you turn off GUI update you cannot log the population/environment windows using saved images.</font>");
    advancedLoggingGrid->addWidget(gui_checkbox,2,1,1,2);
    QObject::connect(gui_checkbox ,SIGNAL (toggled(bool)), this, SLOT(gui_checkbox_state_changed(bool)));

    //ARTS - Dock Grid Layout
    output_settings_grid->addLayout(savePathGrid,1,1,1,2);
    output_settings_grid->addLayout(pollingRateGrid,2,1,1,2);
    output_settings_grid->addLayout(images_grid,3,1,1,2);
    output_settings_grid->addLayout(fileLoggingGrid,4,1,1,2);
    output_settings_grid->addLayout(advancedLoggingGrid,5,1,1,2);

    QWidget *output_settings_layout_widget = new QWidget;
    output_settings_layout_widget->setLayout(output_settings_grid);
    outputSettingsDock->setWidget(output_settings_layout_widget);

    return outputSettingsDock;
}

/*!
 * \brief MainWindow::createOrganismSettingsDock
 * \return QDockWidget organismSettingsDock
 */
QDockWidget *MainWindow::createOrganismSettingsDock() {
    organismSettingsDock = new QDockWidget("Organism", this);
    organismSettingsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    organismSettingsDock->setFeatures(QDockWidget::DockWidgetMovable);
    organismSettingsDock->setFeatures(QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::RightDockWidgetArea, organismSettingsDock);

    QGridLayout *org_settings_grid = new QGridLayout;
    org_settings_grid->setAlignment(Qt::AlignTop);

    QLabel *org_settings_label= new QLabel("Organism settings");
    org_settings_label->setStyleSheet("font-weight: bold");
    org_settings_grid->addWidget(org_settings_label,1,1,1,2);

    QLabel *mutate_label = new QLabel("Chance of mutation:");
    mutate_label->setToolTip("<font>Selects the chance of mutation. Min = 0; Max = 255.</font>");
    mutate_spin = new QSpinBox;
    mutate_spin->setMinimum(0);
    mutate_spin->setMaximum(255);
    mutate_spin->setValue(mutate);
    mutate_spin->setToolTip("<font>Selects the chance of mutation. Min = 0; Max = 255.</font>");
    org_settings_grid->addWidget(mutate_label,2,1);
    org_settings_grid->addWidget(mutate_spin,2,2);
    connect(mutate_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) {mutate=i;});

    variable_mutation_checkbox = new QCheckBox("Variable mutation");
    variable_mutation_checkbox->setToolTip("<font>Turn on/off to toggle variable mutation. This overrides the selected mutation rate (above).</font>");
    org_settings_grid->addWidget(variable_mutation_checkbox,3,1,1,1);
    variable_mutation_checkbox->setChecked(variableMutate);
    connect(variable_mutation_checkbox,&QCheckBox::stateChanged,[=](const bool &i) { variableMutate=i; mutate_spin->setEnabled(!i); });

    QLabel *startAge_label = new QLabel("Start age:");
    startAge_label->setToolTip("<font>Sets the starting age for organisms. Min = 1; Max = 1000.</font>");
    startAge_spin = new QSpinBox;
    startAge_spin->setMinimum(1);
    startAge_spin->setMaximum(1000);
    startAge_spin->setValue(startAge);
    startAge_spin->setToolTip("<font>Sets the starting age for organisms. Min = 1; Max = 1000.</font>");
    org_settings_grid->addWidget(startAge_label,4,1);
    org_settings_grid->addWidget(startAge_spin,4,2);
    connect(startAge_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) {startAge=i;});

    QLabel *breed_settings_label= new QLabel("Breed settings");
    breed_settings_label->setStyleSheet("font-weight: bold");
    org_settings_grid->addWidget(breed_settings_label,5,1,1,2);

    QLabel *breedThreshold_label = new QLabel("Breed threshold:");
    breedThreshold_label->setToolTip("<font>Sets the 'Breed Threshold'. Min = 0; Max = 5000.</font>");
    breedThreshold_spin = new QSpinBox;
    breedThreshold_spin->setMinimum(1);
    breedThreshold_spin->setMaximum(5000);
    breedThreshold_spin->setValue(breedThreshold);
    breedThreshold_spin->setToolTip("<font>Sets the 'Breed Threshold'. Min = 0; Max = 5000.</font>");
    org_settings_grid->addWidget(breedThreshold_label,6,1);
    org_settings_grid->addWidget(breedThreshold_spin,6,2);
    connect(breedThreshold_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) {breedThreshold=i;});

    QLabel *breedCost_label = new QLabel("Breed cost:");
    breedCost_label->setToolTip("<font>Sets the 'Breed Cost'. Min = 0; Max = 10000.</font>");
    breedCost_spin = new QSpinBox;
    breedCost_spin->setMinimum(1);
    breedCost_spin->setMaximum(10000);
    breedCost_spin->setValue(breedCost);
    breedCost_spin->setToolTip("<font>Sets the 'Breed Cost'. Min = 0; Max = 10000.</font>");
    org_settings_grid->addWidget(breedCost_label,7,1);
    org_settings_grid->addWidget(breedCost_spin,7,2);
    connect(breedCost_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) {breedCost=i;});

    QLabel *maxDiff_label = new QLabel("Max difference to breed:");
    maxDiff_label->setToolTip("<font>Sets the maximum difference between organisms to allow breeding. Min = 1; Max = 31.</font>");
    maxDiff_spin = new QSpinBox;
    maxDiff_spin->setMinimum(1);
    maxDiff_spin->setMaximum(31);
    maxDiff_spin->setValue(maxDiff);
    maxDiff_spin->setToolTip("<font>Sets the maximum difference between organisms to allow breeding. Min = 1; Max = 31.</font>");
    org_settings_grid->addWidget(maxDiff_label,8,1);
    org_settings_grid->addWidget(maxDiff_spin,8,2);
    connect(maxDiff_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) {maxDiff=i;});

    breeddiff_checkbox = new QCheckBox("Use max diff to breed");
    breeddiff_checkbox->setToolTip("<font>Turn on/off the maximum difference between organisms test.</font>");
    org_settings_grid->addWidget(breeddiff_checkbox,9,1,1,1);
    breeddiff_checkbox->setChecked(breeddiff);
    connect(breeddiff_checkbox,&QCheckBox::stateChanged,[=](const bool &i) { breeddiff=i;});

    breedspecies_checkbox = new QCheckBox("Breed only within species");
    breedspecies_checkbox->setToolTip("<font>Turn on/off breeding only within the same species.</font>");
    org_settings_grid->addWidget(breedspecies_checkbox,10,1,1,1);
    breedspecies_checkbox->setChecked(breedspecies);
    connect(breedspecies_checkbox,&QCheckBox::stateChanged,[=](const bool &i) { breedspecies=i;});

    QLabel *breed_mode_label= new QLabel("Breed mode:");
    org_settings_grid->addWidget(breed_mode_label,11,1,1,2);
    sexual_radio = new QRadioButton("Sexual");
    sexual_radio->setToolTip("<font>Select to use 'Sexual' breeding.</font>");
    asexual_radio = new QRadioButton("Asexual");
    asexual_radio->setToolTip("<font>Select to use 'Asexual' breeding.</font>");
    variableBreed_radio = new QRadioButton("Variable");
    variableBreed_radio->setToolTip("<font>Select to use 'Variable' breeding.</font>");
    QButtonGroup *breeding_button_group = new QButtonGroup;
    breeding_button_group->addButton(sexual_radio,0);
    breeding_button_group->addButton(asexual_radio,1);
    breeding_button_group->addButton(variableBreed_radio,2);
    sexual_radio->setChecked(sexual);
    asexual_radio->setChecked(asexual);
    variableBreed_radio->setChecked(variableBreed);
    org_settings_grid->addWidget(sexual_radio,12,1,1,2);
    org_settings_grid->addWidget(asexual_radio,13,1,1,2);
    org_settings_grid->addWidget(variableBreed_radio,14,1,1,2);
    connect(breeding_button_group, (void(QButtonGroup::*)(int))&QButtonGroup::buttonClicked,[=](const int &i)
        {
        if(i==0){sexual=true;asexual=false;variableBreed=false;}
        if(i==1){sexual=false;asexual=true;variableBreed=false;}
        if(i==2){sexual=false;asexual=false;variableBreed=true;}
        });

    QLabel *settle_settings_label= new QLabel("Settle settings");
    settle_settings_label->setStyleSheet("font-weight: bold");
    org_settings_grid->addWidget(settle_settings_label,15,1,1,2);

    QLabel *dispersal_label = new QLabel("Dispersal:");
    dispersal_label->setToolTip("<font>Set the maximum dispersal of offspring from parent. Min = 1; Max = 200.</font>");
    dispersal_spin = new QSpinBox;
    dispersal_spin->setMinimum(1);
    dispersal_spin->setMaximum(200);
    dispersal_spin->setValue(dispersal);
    dispersal_spin->setToolTip("<font>Set the maximum dispersal of offspring from parent. Min = 1; Max = 200.</font>");
    org_settings_grid->addWidget(dispersal_label,16,1);
    org_settings_grid->addWidget(dispersal_spin,16,2);
    connect(dispersal_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) {dispersal=i;});

    nonspatial_checkbox = new QCheckBox("Nonspatial settling");
    nonspatial_checkbox->setToolTip("<font>Turn on/off nonspatial settling of offspring.</font>");
    org_settings_grid->addWidget(nonspatial_checkbox,17,1,1,2);
    nonspatial_checkbox->setChecked(nonspatial);
    connect(nonspatial_checkbox,&QCheckBox::stateChanged,[=](const bool &i) {nonspatial=i;});

    QLabel *pathogen_settings_label= new QLabel("Pathogen settings");
    pathogen_settings_label->setStyleSheet("font-weight: bold");
    org_settings_grid->addWidget(pathogen_settings_label,18,1,1,2);

    pathogens_checkbox = new QCheckBox("Pathogens layer");     
    pathogens_checkbox->setChecked(path_on);
    pathogens_checkbox->setToolTip("<font>Turn on/off the 'Pathogens' layer.</font>");
    org_settings_grid->addWidget(pathogens_checkbox,19,1,1,2);
    connect(pathogens_checkbox,&QCheckBox::stateChanged,[=](const bool &i) {path_on=i;});

    QLabel *pathogen_mutate_label = new QLabel("Pathogen mutation:");
    pathogen_mutate_label->setToolTip("<font>Select the 'Pathogen Mutation' rate. Min = 1; Max = 255.</font>");
    pathogen_mutate_spin = new QSpinBox;
    pathogen_mutate_spin->setMinimum(1);
    pathogen_mutate_spin->setMaximum(255);
    pathogen_mutate_spin->setValue(pathogen_mutate);
    pathogen_mutate_spin->setToolTip("<font>Select the 'Pathogen Mutation' rate. Min = 1; Max = 255.</font>");
    org_settings_grid->addWidget(pathogen_mutate_label,20,1);
    org_settings_grid->addWidget(pathogen_mutate_spin,20,2);
    connect(pathogen_mutate_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) {pathogen_mutate=i;});

    QLabel *pathogen_frequency_label = new QLabel("Pathogen frequency:");
    pathogen_frequency_label->setToolTip("<font>Select the 'Pathogen Fequency' rate. Min = 1; Max = 1000.</font>");
    pathogen_frequency_spin = new QSpinBox;
    pathogen_frequency_spin->setMinimum(1);
    pathogen_frequency_spin->setMaximum(1000);
    pathogen_frequency_spin->setValue(pathogen_frequency);
    pathogen_frequency_spin->setToolTip("<font>Select the 'Pathogen Fequency' rate. Min = 1; Max = 1000.</font>");
    org_settings_grid->addWidget(pathogen_frequency_label,21,1);
    org_settings_grid->addWidget(pathogen_frequency_spin,21,2);
    connect(pathogen_frequency_spin,(void(QSpinBox::*)(int))&QSpinBox::valueChanged,[=](const int &i) {pathogen_frequency=i;});

    QWidget *org_settings_layout_widget = new QWidget;
    org_settings_layout_widget->setLayout(org_settings_grid);
    organismSettingsDock->setWidget(org_settings_layout_widget);

    return organismSettingsDock;
}

/*!
 * \brief MainWindow::changepath_triggered
 *
 * Change the save path for various stuff.
 */
void MainWindow::changepath_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(this,"Select directory in which files should be saved.");
    if (dirname.length()!=0)
    {
        dirname.append("/");
        path->setText(dirname);
    }

}

/*!
 * \brief MainWindow::waitUntilPauseSignalIsEmitted
 * \return QEventLoop
 *
 * Adds a null loop to the simulation iteration run when pause button/command is descendantsd
 * this loop is removed on the next tiggered() signal send from either one.
 */
int MainWindow::waitUntilPauseSignalIsEmitted() {
    QEventLoop loop;
    QObject::connect(pauseButton, SIGNAL(triggered()),&loop, SLOT(quit()));
    QObject::connect(ui->actionPause_Sim, SIGNAL(triggered()),&loop, SLOT(quit()));
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
 * \brief MainWindow::on_actionReset_triggered
 *
 * Resets the simulation (i.e. fill the centre pixel with a genome, then sets up a run).
 */
void MainWindow::on_actionReset_triggered()
{
    // Reset the information bar
    resetInformationBar();

    //RJG - This resets all the species logging stuff as well as setting up the run
    TheSimManager->SetupRun();
    NextRefresh=0;

    //ARTS - Update views based on the new reset simulation
    RefreshPopulations();
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

    QString environment_scene_value;
    environment_scene_value.sprintf("%d/%d",CurrentEnvFile+1,EnvFiles.count());
    ui->LabelEnvironment->setText(environment_scene_value);
}

/*!
 * \brief MainWindow::on_actionReseed_triggered
 *
 * Reseed provides options to either reset using a random genome, or a user defined one - drawn
 * from the genome comparison docker.
 */
void MainWindow::on_actionReseed_triggered()
{
    reseed reseed_dialogue;
    reseed_dialogue.exec();
    on_actionReset_triggered();
}

/*!
 * \brief MainWindow::changeEvent
 * \param e
 */
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/*!
 * \brief MainWindow::on_actionStart_Sim_triggered
 *
 * Triggered simulation "Run" action.
 */
void MainWindow::on_actionStart_Sim_triggered()
{

    if (CurrentEnvFile==-1)
    {
        QMessageBox::critical(0,"","Cannot start simulation without environment");
        if (actionEnvironment_Files_triggered() == false) {
            return;
        }
    }

    RunSetUp();

    ui->LabelBatch->setText(tr("1/1"));

    while (stopflag==false)
    {
        while(pauseflag == true){
            waitUntilPauseSignalIsEmitted();
            pauseflag = false;
        }

        Report();
        qApp->processEvents();
        if (ui->actionGo_Slow->isChecked()) Sleeper::msleep(30);

        //ARTS - set Stop flag to returns true if reached end... but why? It will fire the FinishRun() function at the end.
        if (TheSimManager->iterate(environment_mode,environment_interpolate)) stopflag=true;
    }

    FinishRun();
}

/*!
 * \brief MainWindow::on_actionRun_for_triggered
 *
 * Triggered simulation "Run For..." action.
 */
void MainWindow::on_actionRun_for_triggered()
{
    if (CurrentEnvFile==-1)
    {
        QMessageBox::critical(0,"","Cannot start simulation without environment");
        if (actionEnvironment_Files_triggered() == false) {
            return;
        }
    }

    bool ok = false;
    int i, num_iterations;
    num_iterations = QInputDialog::getInt(this, "",tr("Iterations: "), 1000, 1, 10000000, 1, &ok);
    i = num_iterations;
    if (!ok) return;

    ui->LabelBatch->setText(tr("1/1"));

    RunSetUp();

    while (stopflag==false && i>0)
    {
        while(pauseflag == true){
            waitUntilPauseSignalIsEmitted();
            pauseflag = false;
        }

        Report();
        qApp->processEvents();

        if (TheSimManager->iterate(environment_mode,environment_interpolate)) stopflag=true;
        i--;
    }

    if(autodump_checkbox->isChecked())dump_run_data();

    //ARTS Show finish message and run FinshRun()
    if (stopflag==false) {
        QMessageBox::information(0,tr("Run For... Finished"),tr("The run for %1 iterations has finished.").arg(num_iterations));
        FinishRun();
    } else {
        QMessageBox::information(0,tr("Run For... Stopped"),tr("The run for %1 iterations has been stopped at iteration %2.").arg(num_iterations).arg(i));
        FinishRun();
    }
}

/*!
 * \brief MainWindow::on_actionBatch_triggered
 *
 * Batch - primarily intended to allow repeats of runs with the same settings, rather than allowing these to be changed between runs.
 */
void MainWindow::on_actionBatch_triggered()
{
    //ARTS - set default vaules
    batch_running=true;
    runs=0;
    int environment_start = CurrentEnvFile;

    bool repeat_environment;
    QString save_path(path->text());

    //ARTS - batch setup default and maxium values
    int maxIterations = 10000000;
    int defaultIterations = 1000;
    int maxRuns = 10000000;
    int defaultRuns = 1000;

    //ARTS - start of batch setup dialog form
    QDialog dialog(this);
    dialog.setMinimumSize(480,150);
    dialog.setWindowTitle(tr("Batch Run Setup"));

    QFormLayout form(&dialog);
    // Add some text above the fields
    form.addRow(new QLabel("You may: 1) set the number of runs you require; 2) set the number of iterations per run; and 3) choose to repeat or not to repeat the environment each run."));

    QSpinBox *iterationsSpinBox = new QSpinBox(&dialog);
    iterationsSpinBox->setRange(2, maxIterations);
    iterationsSpinBox->setSingleStep(1);
    iterationsSpinBox->setValue(defaultIterations);
    QString iterationsLabel = QString(tr("How many iterations would you like each run to go for (max = %1)?")).arg(maxIterations);
    form.addRow(iterationsLabel, iterationsSpinBox);

    QSpinBox *runsSpinBox = new QSpinBox(&dialog);
    runsSpinBox->setRange(1, maxRuns);
    runsSpinBox->setSingleStep(2);
    runsSpinBox->setValue(defaultRuns);
    QString runsLabel = QString(tr("And how many runs (max = %1)?")).arg(maxRuns);
    form.addRow(runsLabel, runsSpinBox);

    QComboBox *environmentComboBox = new QComboBox(&dialog);
    environmentComboBox->addItem("Yes", 1);
    environmentComboBox->addItem("No", 0);
    int index = environmentComboBox->findData(1);
    if ( index != -1 ) { // -1 for not found
       environmentComboBox->setCurrentIndex(index);
    }
    QString environmentLabel = QString(tr("Would you like the environment to repeat with each batch?"));
    form.addRow(environmentLabel, environmentComboBox);

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
    //ARTS - end of batch setup dialog form

    //ARTS - if OK has been pressed take values and update the defaults, else return false without running batch
    if (dialog.exec() == QDialog::Accepted) {
        batch_iterations=iterationsSpinBox->value();
        batch_target_runs=runsSpinBox->value();
        if(environmentComboBox->itemData(environmentComboBox->currentIndex()) == 1) repeat_environment = true;
            else repeat_environment = false;

        // Reset before starting batch run
        on_actionReset_triggered();

        ui->LabelBatch->setText(tr("%1/%2").arg(1).arg(batch_target_runs));
    } else {
        return;
    }

    //ARTS - run the batch
    do {

        //RJG - Sort environment so it repeats
        if(repeat_environment)
        {
            CurrentEnvFile=environment_start;
            TheSimManager->loadEnvironmentFromFile(environment_mode);
        }

        //And run...
        ui->LabelBatch->setText(tr("%1/%2").arg((runs+1)).arg(batch_target_runs));

        if (CurrentEnvFile==-1)
        {
            QMessageBox::critical(0,"","Cannot start simulation without environment");
            if (actionEnvironment_Files_triggered() == false) {
                return;
            }
        }

        RunSetUp();
        int i = batch_iterations;
        while (stopflag==false && i>0)
        {
            while(pauseflag == true){
                waitUntilPauseSignalIsEmitted();
                pauseflag = false;
            }

            Report();
            qApp->processEvents();

            TheSimManager->iterate(environment_mode,environment_interpolate);
            i--;
        }

       if(autodump_checkbox->isChecked())dump_run_data();

        runs++;

        if(stopflag==false && runs<batch_target_runs) {
            on_actionReset_triggered();
        }

    } while(runs<batch_target_runs && stopflag==false);

    //ARTS Show finish message and reset batch counters
    if ((runs)==batch_target_runs) {
        QMessageBox::information(0,tr("Batch Finished"),tr("The batch of %1 runs with %2 iterations has finished.").arg(batch_target_runs).arg(batch_iterations));
    } else {
        QMessageBox::information(0,tr("Batch Stopped"),tr("The batch of %1 runs with %2 iterations has been stopped at batch run number %3.").arg(batch_target_runs).arg(batch_iterations).arg(runs));
    }

    path->setText(save_path);
    runs=0;
    batch_running=false;
    FinishRun();
}

/*!
 * \brief MainWindow::on_actionPause_Sim_triggered
 *
 * Pause function to halt the simulation mid run and allow restart at same point.
 * Note this disables the Stop button as the Stop function runs outside the iteration loop,
 * so can not be triggered while paused.
 */
void MainWindow::on_actionPause_Sim_triggered()
{
    if (pauseflag == true)
    {
        pauseflag = false;
        ui->actionStop_Sim->setEnabled(true);
        ui->actionPause_Sim->setText(tr("Pause"));
        ui->actionPause_Sim->setToolTip(tr("Pause"));
        stopButton->setEnabled(true);
        pauseButton->setText(tr("Pause"));
    }
    else {
        pauseflag = true;
        ui->actionStop_Sim->setEnabled(false);
        ui->actionPause_Sim->setText(tr("Resume"));
        ui->actionPause_Sim->setToolTip(tr("Resume"));
        stopButton->setEnabled(false);
        pauseButton->setText(tr("Resume"));
    }
}

/*!
 * \brief MainWindow::on_actionStop_Sim_triggered
 *
 * Sets the stopflag to be true on Stop button/command trigger.
 */
void MainWindow::on_actionStop_Sim_triggered()
{
    stopflag=true;
}

/*!
 * \brief MainWindow::RunSetUp
 *
 * Sets up the defaults for a simulation run.
 */
void MainWindow::RunSetUp()
{
    stopflag=false;

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

    if(logging && species_mode==SPECIES_MODE_NONE)QMessageBox::warning(this,"Be aware","Species tracking is off, so the log files won't show species information");

    //Sort out path
    QString save_path_string(path->text());
    if(!save_path_string.endsWith(QDir::separator()))path->setText(save_path_string+QDir::separator());
    QDir save_path(path->text());
    if (!save_path.exists())
            {
                 QMessageBox::warning(0, "Error!", "The program doesn't think the save directory exists, so is going to default back to the direcctory in which the executable is.");
                 QString program_path(QCoreApplication::applicationDirPath());
                 program_path.append(QDir::separator());
                 path->setText(program_path);
                 save_path.setPath(program_path);
            }
    //RJG - Set up save directory
    if(!path->text().endsWith(QString(PRODUCTNAME)+"_output"+QDir::separator()))
        {
        if(!save_path.mkpath(QString(PRODUCTNAME)+"_output"+QDir::separator())){QMessageBox::warning(this,"Error","Cant save images. Permissions issue?");return;}
        else path->setText(save_path.absolutePath()+QDir::separator()+QString(PRODUCTNAME)+"_output"+QDir::separator());
        }

    timer.restart();
    NextRefresh=RefreshRate;

    if (logging_checkbox->isChecked())WriteLog();
}

/*!
 * \brief MainWindow::FinishRun
 *
 * Resets the buttons/commands back to a pre-run state.
 */
void MainWindow::FinishRun()
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
void MainWindow::closeEvent(QCloseEvent * e)
{
    Q_UNUSED(e);

    exit(0);
}

/*!
 * \brief MainWindow::Report
 *
 * Updates reports, and does logging.
 */
void MainWindow::Report()
{

    if (--NextRefresh>0) return;

    NextRefresh=RefreshRate;

    QString s;
    QTextStream sout(&s);

    int time=timer.elapsed();
    float atime= (float)time / (float) RefreshRate;
    timer.restart();
    double t=0;
    for (int n2=0; n2<gridX; n2++)
    for (int m2=0; m2<gridY; m2++)
        t+=totalfit[n2][m2];
    t/=(double)AliveCount;
    t/=(double)settleTolerance;
    t*=100; //now %age

    QString out;
    QTextStream o(&out);

    o<<generation; //need to use to avoid int64 descendantss
    ui->LabelIteration->setText(out);

    out.sprintf("%.3f",(3600000/atime)/1000000);
    ui->LabelIterationsPerHour->setText(out);

    //now back to sprintf for convenience
    if (CurrentEnvFile>=EnvFiles.count())
    out.sprintf("Finished (%d)",EnvFiles.count());
    else
    out.sprintf("%d/%d",CurrentEnvFile+1,EnvFiles.count());
    ui->LabelEnvironment->setText(out);

    out.sprintf("%.2f%%",t);
    ui->LabelFitness->setText(out);

    out.sprintf("%.2f",atime);
    ui->LabelSpeed->setText(out);

    out.sprintf("%d",AliveCount);
    ui->LabelCritters->setText(out);

    CalcSpecies();
    out="-";
    if (species_mode!=SPECIES_MODE_NONE)
    {
        int g5=0, g50=0;
        for (int i=0; i<oldspecieslist.count(); i++)
        {
            if (oldspecieslist[i].size>5) g5++;
            if (oldspecieslist[i].size>50) g50++;
        }
        out.sprintf("%d (>5:%d >50:%d)",oldspecieslist.count(), g5, g50);
    }
    ui->LabelSpecies->setText(out);

    //do species stuff
    if(!gui)RefreshPopulations();
    if(!gui)RefreshEnvironment();

    WriteLog();

    //reset the breedattempts and breedfails arrays
    for (int n2=0; n2<gridX; n2++)
    for (int m2=0; m2<gridY; m2++)
    {
        //breedattempts[n2][m2]=0;
        breedfails[n2][m2]=0;
        settles[n2][m2]=0;
        settlefails[n2][m2]=0;
    }
}

/*!
 * \brief MainWindow::ScaleFails
 * \param fails
 * \param gens
 * \return int
 *
 * Scales colour of fail count, correcting for generations, and scaling high values to something saner.
 */
int MainWindow::ScaleFails(int fails, float gens)
{
    float ffails=((float)fails)/gens;

    ffails*=100.0; // a fudge factor no less! Well it's only visualization...
    ffails=pow(ffails,0.8);

    if (ffails>255.0) ffails=255.0;
    return (int)ffails;
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
    int currentSelectedMode = ui->populationWindowComboBox->itemData(index).toInt();
    Q_UNUSED(currentSelectedMode);

    //view_mode_changed();
    RefreshPopulations();
}

/*!
 * \brief MainWindow::RefreshPopulations
 *
 * Rereshes the population window. Also runs the species identification code.
 */
void MainWindow::RefreshPopulations()
{
    //RJG - make path if required - this way as if user adds file name to path, this will create a
    //subfolder with the same file name as logs.
    QString save_path(path->text());
    if(!save_path.endsWith(QDir::separator()))save_path.append(QDir::separator());
    if(batch_running)
    {
        save_path.append(QString("Images_run_%1").arg(runs, 4, 10, QChar('0')));
        save_path.append(QDir::separator());
    }
    QDir save_dir(save_path);

    // 0 = Population count
    // 1 = Mean fitness
    // 2 = Coding genome as colour
    // 3 = NonCoding genome as colour
    // 4 = Gene Frequencies
    // 5 = Breed Attempts
    // 6 = Breed Fails
    // 7 = Settles
    // 8 = Settle Fails === Fails (R-Breed, G=Settle)
    // 9 = Breed Fails 2
    // 10 = Species

    //ARTS - Check to see what the mode is from the Population Window QComboBox; return the data as int.
    int currentSelectedMode = ui->populationWindowComboBox->itemData(ui->populationWindowComboBox->currentIndex()).toInt();

    // (0) Population Count
    //if (ui->actionPopulation_Count->isChecked()||save_population_count->isChecked())
    if (currentSelectedMode==0||save_population_count->isChecked())
    {
        //Popcount
        int bigcount=0;
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            int count=0;
            for (int c=0; c<slotsPerSq; c++)
            {
                if (critters[n][m][c].age) count++;
            }
            bigcount+=count;
            count*=2;
            if (count>255) count=255;
            pop_image->setPixel(n,m,count);
        }
        //if (ui->actionPopulation_Count->isChecked())pop_item->setPixmap(QPixmap::fromImage(*pop_image));
        if (currentSelectedMode==0)pop_item->setPixmap(QPixmap::fromImage(*pop_image));
        if (save_population_count->isChecked())
            if(save_dir.mkpath("population/"))
                pop_image_colour->save(QString(save_dir.path()+"/population/EvoSim_population_it_%1.png").arg(generation, 7, 10, QChar('0')));
    }

    // (1) Fitness
    if (currentSelectedMode==1||save_mean_fitness->isChecked())
    {
        //Popcount
        int multiplier=255/settleTolerance;
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            int count=0;
            for (int c=0; c<slotsPerSq; c++)
            {
                if (critters[n][m][c].age) count++;
            }
            if (count==0)
            pop_image->setPixel(n,m,0);
                else
            pop_image->setPixel(n,m,(totalfit[n][m] * multiplier) / count);

        }
        if (currentSelectedMode==1)pop_item->setPixmap(QPixmap::fromImage(*pop_image));
        if (save_mean_fitness->isChecked())
            if(save_dir.mkpath("fitness/"))
                pop_image_colour->save(QString(save_dir.path()+"/fitness/EvoSim_mean_fitness_it_%1.png").arg(generation, 7, 10, QChar('0')));
    }

    // (2) Genome as colour
    if (currentSelectedMode==2||save_coding_genome_as_colour->isChecked())
    {
        //find modal genome in each square, convert to colour
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            //data structure for mode
            quint64 genomes[SLOTS_PER_GRID_SQUARE];
            int counts[SLOTS_PER_GRID_SQUARE];
            int arraypos=0; //pointer

            if (totalfit[n][m]==0) pop_image_colour->setPixel(n,m,0); //black if square is empty
            else
            {
                //for each used slot
                for (int c=0; c<maxused[n][m]; c++)
                {
                    if (critters[n][m][c].age>0)
                    {
                        //If critter is alive

                        quint64 g = critters[n][m][c].genome;

                        //find genome frequencies
                        for (int i=0; i<arraypos; i++)
                        {
                            if (genomes[i]==g) //found it
                            {
                                counts[i]++;
                                goto gotcounts;
                            }
                        }
                        //didn't find it
                        genomes[arraypos]=g;
                        counts[arraypos++]=1;
                    }
                gotcounts:;
                }

                //find max frequency
                int max=-1;
                quint64 maxg=0;

                for (int i=0; i<arraypos; i++)
                    if (counts[i]>max)
                    {
                        max=counts[i];
                        maxg=genomes[i];
                    }

                //now convert first 32 bits to a colour
                // r,g,b each counts of 11,11,10 bits
                quint32 genome= (quint32)(maxg & ((quint64)65536*(quint64)65536-(quint64)1));
                quint32 b = bitcounts[genome & 2047] * 23;
                genome /=2048;
                quint32 g = bitcounts[genome & 2047] * 23;
                genome /=2048;
                quint32 r = bitcounts[genome] * 25;
                pop_image_colour->setPixel(n,m,qRgb(r, g, b));
            }

       }

        if (currentSelectedMode==2) pop_item->setPixmap(QPixmap::fromImage(*pop_image_colour));
        if (save_coding_genome_as_colour->isChecked())
            if(save_dir.mkpath("coding/"))
            pop_image_colour->save(QString(save_dir.path()+"/coding/EvoSim_coding_genome_it_%1.png").arg(generation, 7, 10, QChar('0')));
    }

    // (3) Non-coding Genome
    if (currentSelectedMode==3||save_non_coding_genome_as_colour->isChecked())
    {
        //find modal genome in each square, convert non-coding to colour
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            //data structure for mode
            quint64 genomes[SLOTS_PER_GRID_SQUARE];
            int counts[SLOTS_PER_GRID_SQUARE];
            int arraypos=0; //pointer

            if (totalfit[n][m]==0) pop_image_colour->setPixel(n,m,0); //black if square is empty
            else
            {
                //for each used slot
                for (int c=0; c<maxused[n][m]; c++)
                {
                    if (critters[n][m][c].age>0)
                    {
                        //If critter is alive

                        quint64 g = critters[n][m][c].genome;

                        //find genome frequencies
                        for (int i=0; i<arraypos; i++)
                        {
                            if (genomes[i]==g) //found it
                            {
                                counts[i]++;
                                goto gotcounts2;
                            }
                        }
                        //didn't find it
                        genomes[arraypos]=g;
                        counts[arraypos++]=1;
                    }
                gotcounts2:;
                }


                //find max frequency
                int max=-1;
                quint64 maxg=0;

                for (int i=0; i<arraypos; i++)
                    if (counts[i]>max)
                    {
                        max=counts[i];
                        maxg=genomes[i];
                    }

                //now convert second 32 bits to a colour
                // r,g,b each counts of 11,11,10 bits
                quint32 genome= (quint32)(maxg / ((quint64)65536*(quint64)65536));
                quint32 b = bitcounts[genome & 2047] * 23;
                genome /=2048;
                quint32 g = bitcounts[genome & 2047] * 23;
                genome /=2048;
                quint32 r = bitcounts[genome] * 25;
                pop_image_colour->setPixel(n,m,qRgb(r, g, b));
            }
       }

        if(currentSelectedMode==3)pop_item->setPixmap(QPixmap::fromImage(*pop_image_colour));
        if(save_non_coding_genome_as_colour->isChecked())
            if(save_dir.mkpath("non_coding/"))
                 pop_image_colour->save(QString(save_dir.path()+"/non_coding/EvoSim_non_coding_it_%1.png").arg(generation, 7, 10, QChar('0')));
    }

    // (4) Gene Frequencies
    if (currentSelectedMode==4||save_gene_frequencies->isChecked())
    {
        //Popcount
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            int count=0;
            int gen0tot=0;
            int gen1tot=0;
            int gen2tot=0;
            for (int c=0; c<slotsPerSq; c++)
            {
                if (critters[n][m][c].age)
                {
                    count++;
                    if (critters[n][m][c].genome & 1) gen0tot++;
                    if (critters[n][m][c].genome & 2) gen1tot++;
                    if (critters[n][m][c].genome & 4) gen2tot++;
                }
            }
            if (count==0) pop_image_colour->setPixel(n,m,qRgb(0, 0, 0));
            else
            {
                quint32 r = gen0tot *256 / count;
                quint32 g = gen1tot *256 / count;
                quint32 b = gen2tot *256 / count;
                pop_image_colour->setPixel(n,m,qRgb(r, g, b));
            }
          }

        if (currentSelectedMode==4)pop_item->setPixmap(QPixmap::fromImage(*pop_image_colour));
        if(save_gene_frequencies->isChecked())
            if(save_dir.mkpath("gene_freq/"))
                pop_image_colour->save(QString(save_dir.path()+"/gene_freq/EvoSim_gene_freq_it_%1.png").arg(generation, 7, 10, QChar('0')));
    }

    // (5) Breed Attempts
    //RJG - No save option as no longer in the menu as an option.
    if (currentSelectedMode==5)
    {
        //Popcount
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            int value=(breedattempts[n][m]*10)/RefreshRate;
            if (value>255) value=255;
            pop_image->setPixel(n,m,value);
        }
        pop_item->setPixmap(QPixmap::fromImage(*pop_image));
    }

    // (6) Breed Fails
    //RJG - No save option as no longer in the menu as an option.
    if (currentSelectedMode==6)
    {
        //Popcount
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            if (breedattempts[n][m]==0) pop_image->setPixel(n,m,0);
            else
            {
                int value=(breedfails[n][m]*255)/breedattempts[n][m];
                pop_image->setPixel(n,m,value);
            }
        }
        pop_item->setPixmap(QPixmap::fromImage(*pop_image));
    }

    // (7) Settles
    if (currentSelectedMode==7||save_settles->isChecked())
    {
        //Popcount
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            int value=(settles[n][m]*10)/RefreshRate;
            if (value>255) value=255;
            pop_image->setPixel(n,m,value);
        }

        if(currentSelectedMode==7)pop_item->setPixmap(QPixmap::fromImage(*pop_image));
        if(save_settles->isChecked())
            if(save_dir.mkpath("settles/"))
                pop_image->save(QString(save_dir.path()+"/settles/EvoSim_settles_it_%1.png").arg(generation, 7, 10, QChar('0')));
    }

    // (8) Settle Fails === Fails
    //RJG - this now combines breed fails (red) and settle fails (green)
    if (currentSelectedMode==8||save_fails_settles->isChecked())
    {
        //work out max and ratios
        /*int maxbf=1;
        int maxsf=1;
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            if (settlefails[n][m]>maxsf) maxsf=settlefails[n][m];
            if (breedfails[n][m]>maxsf) maxbf=breedfails[n][m];
        }
        float bf_mult=255.0 / (float)maxbf;
        float sf_mult=255.0 / (float)maxsf;
        qDebug()<<bf_mult<<sf_mult;
        bf_mult=1.0;
        sf_mult=1.0;
        */

        //work out average per generation
        float gens=generation-lastReport;


        //Make image
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            int r=ScaleFails(breedfails[n][m],gens);
            int g=ScaleFails(settlefails[n][m],gens);
            pop_image_colour->setPixel(n,m,qRgb(r, g, 0));
        }

        if(currentSelectedMode==8)pop_item->setPixmap(QPixmap::fromImage(*pop_image_colour));
        if(save_fails_settles->isChecked())
            if(save_dir.mkpath("settles_fails/"))
                pop_image_colour->save(QString(save_dir.path()+"/settles_fails/EvoSim_settles_fails_it_%1.png").arg(generation, 7, 10, QChar('0')));

    }

    // (9) Breed Fails 2
    if (currentSelectedMode==9)
    {
        //Popcount
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {
            if (breedfails[n][m]==0) pop_image->setPixel(n,m,0);
            else
            {
                int value=(breedfails[n][m]);
                if (value>255) value=255;
                pop_image->setPixel(n,m,value);
            }
        }
        pop_item->setPixmap(QPixmap::fromImage(*pop_image));
    }

    // (10) Species
    if (currentSelectedMode==10||save_species->isChecked()) //do visualisation if necessary
    {
        for (int n=0; n<gridX; n++)
        for (int m=0; m<gridY; m++)
        {

            if (totalfit[n][m]==0) pop_image_colour->setPixel(n,m,0); //black if square is empty
            else
            {
                quint64 thisspecies=0;
                for (int c=0; c<slotsPerSq; c++)
                {
                    if (critters[n][m][c].age>0)
                    {
                        thisspecies=critters[n][m][c].speciesid;
                        break;
                    }
                }

                pop_image_colour->setPixel(n,m,species_colours[thisspecies % 65536]);
            }
        }

        if (currentSelectedMode==10)pop_item->setPixmap(QPixmap::fromImage(*pop_image_colour));
        if (save_species->isChecked())
            if(save_dir.mkpath("species/"))
                pop_image_colour->save(QString(save_dir.path()+"/species/EvoSim_species_it_%1.png").arg(generation, 7, 10, QChar('0')));

    }

    lastReport=generation;
}

/*!
 * \brief MainWindow::RefreshEnvironment
 *
 * Refreshes the environment window.
 */
void MainWindow::RefreshEnvironment()
{
    QString save_path(path->text());
    if(!save_path.endsWith(QDir::separator()))save_path.append(QDir::separator());
    if(batch_running)
        {
        save_path.append(QString("Images_run_%1").arg(runs, 4, 10, QChar('0')));
        save_path.append(QDir::separator());
        }
    QDir save_dir(save_path);

    for (int n=0; n<gridX; n++)
    for (int m=0; m<gridY; m++)
        env_image->setPixel(n,m,qRgb(environment[n][m][0], environment[n][m][1], environment[n][m][2]));

    env_item->setPixmap(QPixmap::fromImage(*env_image));
    if(save_environment->isChecked())
        if(save_dir.mkpath("environment/"))
            env_image->save(QString(save_dir.path()+"/environment/EvoSim_environment_it_%1.png").arg(generation, 7, 10, QChar('0')));
}

/*!
 * \brief MainWindow::resizeEvent
 *
 * Resize windows on window size change event.
 */
void MainWindow::resizeEvent(QResizeEvent * e)
{
    Q_UNUSED(e);

    Resize();
}

/*!
 * \brief MainWindow::Resize
 *
 * Force and window resize and rescale of the graphic view
 */
void MainWindow::Resize()
{
    ui->GV_Population->fitInView(pop_item,Qt::KeepAspectRatio);
    ui->GV_Environment->fitInView(env_item,Qt::KeepAspectRatio);
}

/*!
 * \brief MainWindow::gui_checkbox_state_changed
 * \param dont_update
 *
 * Sets the gui don't update flag on gui checkbox change.
 */
void MainWindow::gui_checkbox_state_changed(bool dont_update)
{
    if(dont_update && QMessageBox::question(0, "Heads up", "If you don't update the GUI, images will also not be saved. OK?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes)==QMessageBox::No) {gui_checkbox->setChecked(false);return;}

    gui=dont_update;
}

/*!
 * \brief MainWindow::save_all_checkbox_state_changed
 * \param all
 *
 * Checks/unchecks all state checkboxes on check/uncheck of the All checkbox.
 */
void MainWindow::save_all_checkbox_state_changed(bool all)
{
    save_population_count->setChecked(all);
    save_mean_fitness->setChecked(all);
    save_coding_genome_as_colour->setChecked(all);
    save_non_coding_genome_as_colour->setChecked(all);
    save_species->setChecked(all);
    save_gene_frequencies->setChecked(all);
    save_settles->setChecked(all);
    save_fails_settles->setChecked(all);
    save_environment->setChecked(all);
}

/*!
 * \brief MainWindow::dump_run_data
 *
 * At end of run in run for/batch mode, or on click when a run is going, this allows user to
 * output the final log, along with the tree for the run.
 */
void MainWindow::dump_run_data()
{

    QString FinalLoggingFile(path->text());
    if(!FinalLoggingFile.endsWith(QDir::separator()))FinalLoggingFile.append(QDir::separator());
    FinalLoggingFile.append(QString(PRODUCTNAME)+"_end_run_log");
    if(batch_running)FinalLoggingFile.append(QString("_run_%1").arg(runs, 4, 10, QChar('0')));
    FinalLoggingFile.append(".txt");
    QFile outputfile(FinalLoggingFile);
    outputfile.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&outputfile);

    out<<"End run log ";
    QDateTime t(QDateTime::currentDateTime());
    out<<t.toString(Qt::ISODate)<< "\n\n===================\n\n"<<print_settings()<<"\n\n===================\n";
    out<<"\nThis log features the tree from a finished run, in Newick format, and then data for all the species that have existed with more individuals than minimum species size. The exact data provided depends on the phylogeny tracking mode selected in the GUI.\n";
    out<<"\n\n===================\n\n";
    out<<"Tree:\n\n"<<HandleAnalysisTool(ANALYSIS_TOOL_CODE_MAKE_NEWICK);
    out<<"\n\nSpecies data:\n\n";
    out<<HandleAnalysisTool(ANALYSIS_TOOL_CODE_DUMP_DATA);
    outputfile.close();
}

/*!
 * \brief MainWindow::environment_mode_changed
 * \param change_environment_mode
 * \param update_gui
 *
 * Sets the environment mode on change.
 */
void MainWindow::environment_mode_changed(int change_environment_mode, bool update_gui)
{
    int new_environment_mode=ENV_MODE_STATIC;
    if (change_environment_mode==ENV_MODE_ONCE) new_environment_mode=ENV_MODE_ONCE;
    if (change_environment_mode==ENV_MODE_LOOP) new_environment_mode=ENV_MODE_LOOP;
    if (change_environment_mode==ENV_MODE_BOUNCE) new_environment_mode=ENV_MODE_BOUNCE;

    if(update_gui)
    {
        if (change_environment_mode==ENV_MODE_STATIC)environmentModeStaticButton->setChecked(true);
        if (change_environment_mode==ENV_MODE_ONCE)environmentModeOnceButton->setChecked(true);
        if (change_environment_mode==ENV_MODE_LOOP)environmentModeLoopButton->setChecked(true);
        if (change_environment_mode==ENV_MODE_BOUNCE)environmentModeBounceButton->setChecked(true);
    }

    environment_mode=new_environment_mode;
}

/*!
 * \brief MainWindow::species_mode_changed
 * \param change_species_mode
 *
 * Sets the species mode on change.
 */
void MainWindow::species_mode_changed(int change_species_mode, bool update_gui)
{
    int new_species_mode=SPECIES_MODE_NONE;
    if (change_species_mode==SPECIES_MODE_PHYLOGENY) new_species_mode=SPECIES_MODE_PHYLOGENY;
    if (change_species_mode==SPECIES_MODE_PHYLOGENY_AND_METRICS) new_species_mode=SPECIES_MODE_PHYLOGENY_AND_METRICS;
    if (change_species_mode==SPECIES_MODE_BASIC) new_species_mode=SPECIES_MODE_BASIC;

    //some changes not allowed
    if (generation!=0)
    {
        //already running. Can switch tracking off - but not on
        //detailed tracking can be switched on/off at any point
        if (species_mode==SPECIES_MODE_NONE)
        {
            if (new_species_mode!=SPECIES_MODE_NONE)
            {
                QMessageBox::warning(this,"Error","Turning on species logging is not allowed mid-simulation");
                phylogeny_off_button->setChecked(true);
                return;
            }
        }

        if (species_mode==SPECIES_MODE_BASIC)
        {
            if (new_species_mode==SPECIES_MODE_PHYLOGENY || new_species_mode==SPECIES_MODE_PHYLOGENY_AND_METRICS)
            {
                QMessageBox::warning(this,"Error","Turning on phylogeny tracking is not allowed mid-simulation");
                basic_phylogeny_button->setChecked(true);
                return;
            }
        }
        //all other combinations are OK - hopefully
    }

    if(update_gui)
    {
        if (change_species_mode==SPECIES_MODE_BASIC)basic_phylogeny_button->setChecked(true);
        if (change_species_mode==SPECIES_MODE_PHYLOGENY_AND_METRICS)phylogeny_and_metrics_button->setChecked(true);
        if (change_species_mode==SPECIES_MODE_PHYLOGENY)phylogeny_button->setChecked(true);
        if (change_species_mode==SPECIES_MODE_NONE)phylogeny_off_button->setChecked(true);
    }

    species_mode=new_species_mode;
}

/*!
 * \brief MainWindow::ResetSquare
 * \param n
 * \param m
 *
 * Resets the grid square.
 */
void MainWindow::ResetSquare(int n, int m)
{
    //grid expanded - make sure everything is zeroed in new slots
    for (int c=0; c<slotsPerSq; c++) critters[n][m][c].age=0;

    totalfit[n][m]=0;

    breedattempts[n][m]=0;
    breedfails[n][m]=0;
    settles[n][m]=0;
    settlefails[n][m]=0;
}

/*!
 * \brief MainWindow::ResizeImageObjects
 */
void MainWindow::ResizeImageObjects()
{
    delete pop_image;
    delete env_image;
    delete pop_image_colour;
    pop_image =new QImage(gridX, gridY, QImage::Format_Indexed8);
    QVector <QRgb> clut(256);
    for (int ic=0; ic<256; ic++) clut[ic]=qRgb(ic,ic,ic);
    pop_image->setColorTable(clut);

    env_image=new QImage(gridX, gridY, QImage::Format_RGB32);

    pop_image_colour=new QImage(gridX, gridY, QImage::Format_RGB32);
}

/*!
 * \brief MainWindow::redoImages
 * \param oldrows
 * \param oldcols
 */
void MainWindow::redoImages(int oldrows, int oldcols)
{
    //check that the maxused's are in the new range
     for (int n=0; n<gridX; n++)
     for (int m=0; m<gridY; m++)
         if (maxused[n][m]>=slotsPerSq) maxused[n][m]=slotsPerSq-1;

     //If either rows or cols are bigger - make sure age is set to 0 in all critters in new bit!
    if (gridX>oldrows)
    {
        for (int n=oldrows; n<gridX; n++) for (int m=0; m<gridY; m++)
            ResetSquare(n,m);
    }
    if (gridY>oldcols)
    {
        for (int n=0; n<gridX; n++) for (int m=oldcols; m<gridY; m++)
            ResetSquare(n,m);
    }

    ResizeImageObjects();

    RefreshPopulations();
    RefreshEnvironment();
    Resize();
}

/*!
 * \brief MainWindow::actionSettings_triggered
 *
 * Triggered action to open/close the Settings Dock.
 */
void MainWindow::actionSettings_triggered()
{
    if(simulationSettingsDock->isVisible())
    {
        simulationSettingsDock->hide();
        organismSettingsDock->hide();
        outputSettingsDock->hide();
        settingsButton->setChecked(false);
        ui->actionSettings_Dock->setChecked(false);
    } else
    {
        simulationSettingsDock->show();
        organismSettingsDock->show();
        outputSettingsDock->show();
        settingsButton->setChecked(true);
        ui->actionSettings_Dock->setChecked(true);
    }
}

/*!
 * \brief MainWindow::on_actionCount_Peaks_triggered
 *
 * Trigers the Count Peaks action.
 */
void MainWindow::on_actionCount_Peaks_triggered()
{

    QString peaks(HandleAnalysisTool(ANALYSIS_TOOL_CODE_COUNT_PEAKS));
    //Count peaks returns empty string if error.
    if (peaks.length()<5)return;

    QString count_peaks_file(path->text());
    if(!count_peaks_file.endsWith(QDir::separator()))count_peaks_file.append(QDir::separator());
    count_peaks_file.append(QString(PRODUCTNAME)+"_count_peaks.txt");
    QFile outputfile(count_peaks_file);
    outputfile.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream out(&outputfile);

    out<<"REvoSim Peak Counting ";
    QDateTime t(QDateTime::currentDateTime());
    out<<t.toString(Qt::ISODate)<< "\n\n===================\n\n";
    out<<"\nBelow is a histogram showing the different fitnesses for all potential 32-bit organisms in REvoSim under the user-defined RGB levels.\n";
    out<<"\n\n===================\n\n";
    out<<peaks;

    outputfile.close();
}

/*!
 * \brief MainWindow::actionEnvironment_Files_triggered
 * \return bool
 *
 * Action to allow new environmental files to be loaded from the local filesystem.
 */
bool MainWindow::actionEnvironment_Files_triggered()
{
    //Select files
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more image files to load in simulation environment...",
                            "",
                            "Images (*.png *.bmp)");

    if (files.length()==0) return false;

    bool notsquare=false, different_size=false;
    for(int i=0;i<files.length();i++)
    {
        QImage LoadImage(files[i]);
        int x=LoadImage.width();
        int y=LoadImage.height();
        if(x!=y)notsquare=true;
        if(x!=100||y!=100)different_size=true;
    }
    if(notsquare||different_size)QMessageBox::warning(this,"FYI","For speed REvoSim currently has static arrays for the environment, which limits out of the box functionality to 100 x 100 square environments. "
    "It looks like some of your Environment images don't meet this requirement. Anything smaller than 100 x 100 will be stretched (irrespective of aspect ratio) to 100x100. Anything bigger, and we'll use the top left corner. Should you wish to use a different size environment, please email RJG or MDS.");

    EnvFiles = files;
    CurrentEnvFile=0;
    TheSimManager->loadEnvironmentFromFile(environment_mode);
    RefreshEnvironment();

    //RJG - Reset for this new environment
    TheSimManager->SetupRun();

    //ARTS - Update the information bar
    QString environment_scene_value;
    environment_scene_value.sprintf("%d/%d",CurrentEnvFile+1,EnvFiles.count());
    ui->LabelEnvironment->setText(environment_scene_value);

    return true;
}

/*!
 * \brief MainWindow::on_actionSave_triggered
 *
 * Action to save the current settings and simulation to an .evosim file.
 *
 */
void MainWindow::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(
                            this,
                            "Save file",
                            "",
                            "REvoSim files (*.revosim)");

    if (filename.length()==0) return;

    //Otherwise - serialise all my crap
    QFile outfile(filename);
    outfile.open(QIODevice::WriteOnly|QIODevice::Text);

    QDataStream out(&outfile);

    out<<QString("REvoSim Format File");
    out<<(int)FILEVERSION;

    //Ints
    out<<gridX;
    out<<gridY;
    out<<settleTolerance;
    out<<slotsPerSq;
    out<<startAge;
    out<<dispersal;
    out<<food;
    out<<breedCost;
    out<<mutate;
    out<<pathogen_mutate;
    out<<pathogen_frequency;
    out<<maxDiff;
    out<<breedThreshold;
    out<<target;
    out<<envchangerate;
    out<<environment_mode;
    out<<RefreshRate;
    out<<speciesSamples;
    out<<speciesSensitivity;
    out<<timeSliceConnect;
    out<<minspeciessize;
    out<<species_mode;

    //Bools
    out<<recalcFitness;
    out<<toroidal;
    out<<nonspatial;
    out<<breeddiff;
    out<<breedspecies;
    out<<path_on;
    out<<variableMutate;
    out<<allowExcludeWithDescendants;
    out<<sexual;
    out<<asexual;
    out<<variableBreed;
    out<<logging;
    out<<gui;
    out<<environment_interpolate;
    out<<fitnessLoggingToFile;
    out<<autodump_checkbox->isChecked();
    out<<save_population_count->isChecked();
    out<<save_mean_fitness->isChecked();
    out<<save_coding_genome_as_colour->isChecked();
    out<<save_species->isChecked();
    out<<save_non_coding_genome_as_colour->isChecked();
    out<<save_gene_frequencies->isChecked();
    out<<save_settles->isChecked();
    out<<save_fails_settles->isChecked();
    out<<save_environment->isChecked();

    //Strings
    out<<path->text();

    int vmode=0;
    vmode = ui->populationWindowComboBox->itemData(ui->populationWindowComboBox->currentIndex()).toInt();
    out<<vmode;

    //file list
    out<<EnvFiles.count();
    for (int i=0; i<EnvFiles.count(); i++)
        out<<EnvFiles[i];

    //now the big arrays
    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    for (int k=0; k<slotsPerSq; k++)
    {
        if ((critters[i][j][k]).age==0) //its dead
            out<<(int)0;
        else
        {
            out<<critters[i][j][k].age;
            out<<critters[i][j][k].genome;
            out<<critters[i][j][k].ugenecombo;
            out<<critters[i][j][k].fitness;
            out<<critters[i][j][k].energy;
            out<<critters[i][j][k].xpos;
            out<<critters[i][j][k].ypos;
            out<<critters[i][j][k].zpos;
        }
    }

    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        out<<environment[i][j][0];
        out<<environment[i][j][1];
        out<<environment[i][j][2];
    }

    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        out<<totalfit[i][j];
    }

    for (int i=0; i<256; i++)
    for (int j=0; j<3; j++)
    {
        out<<xormasks[i][j];
    }

    //ARTS - Genome Comparison Saving
    out<<genoneComparison->saveComparison();

    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        out<<breedattempts[i][j];
        out<<breedfails[i][j];
        out<<settles[i][j];
        out<<settlefails[i][j];
        out<<maxused[i][j];
    }

    //And some window state stuff
    out<<saveState(); //window state
    out<<ui->actionGenomeComparison->isChecked();

    //interpolate environment stuff
    out<<environment_interpolate;
    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        out<<environmentlast[i][j][0];
        out<<environmentlast[i][j][1];
        out<<environmentlast[i][j][2];
    }
    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        out<<environmentnext[i][j][0];
        out<<environmentnext[i][j][1];
        out<<environmentnext[i][j][2];
    }

    out<<speciesSamples;
    out<<speciesSensitivity;
    out<<timeSliceConnect;

    //now the species archive
    out<<oldspecieslist.count();
    for (int j=0; j<oldspecieslist.count(); j++)
    {
         out<<oldspecieslist[j].ID;
         out<<oldspecieslist[j].type;
         out<<oldspecieslist[j].origintime;
         out<<oldspecieslist[j].parent;
         out<<oldspecieslist[j].size;
         out<<oldspecieslist[j].internalID;
    }

    out<<archivedspecieslists.count();
    for (int i=0; i<archivedspecieslists.count(); i++)
    {
        out<<archivedspecieslists[i].count();
        for (int j=0; j<archivedspecieslists[i].count(); j++)
        {
             out<<archivedspecieslists[i][j].ID;
             out<<archivedspecieslists[i][j].type;
             out<<archivedspecieslists[i][j].origintime;
             out<<archivedspecieslists[i][j].parent;
             out<<archivedspecieslists[i][j].size;
             out<<archivedspecieslists[i][j].internalID;
        }
    }
    out<<nextspeciesid;
    out<<lastSpeciesCalc;

    //now random number array
    for (int i=0; i<65536; i++)
        out<<randoms[i];

    outfile.close();
}

/*!
 * \brief MainWindow::on_actionLoad_triggered
 *
 * Load a REvoSim file of saved settings and simulations.
 *
 * \todo RJG - Fitness logging to file not sorted on load as yet.
 */
void MainWindow::on_actionLoad_triggered()
{
    QString filename = QFileDialog::getOpenFileName(
                            this,
                            "Save file",
                            "",
                            "REvoSim files (*.revosim)");

    if (filename.length()==0) return;

    if (stopflag==false) stopflag=true;

    //Otherwise - serialise all my crap
    QFile infile(filename);
    infile.open(QIODevice::ReadOnly);

    QDataStream in(&infile);

    QString strtemp;
    in>>strtemp;
    if (strtemp!="REvoSim Format File")
    {QMessageBox::warning(this,"","Not an REvoSim Format File."); return;}

    int version;
    in>>version;
    if (version>FILEVERSION)
    {QMessageBox::warning(this,"","Version too high - will try to read, but may go horribly wrong.");}

    //Ints
    in>>gridX;
    in>>gridY;
    in>>settleTolerance;
    in>>slotsPerSq;
    in>>startAge;
    in>>dispersal;
    in>>food;
    in>>breedCost;
    in>>mutate;
    in>>pathogen_mutate;
    in>>pathogen_frequency;
    in>>maxDiff;
    in>>breedThreshold;
    in>>target;
    in>>envchangerate;
    in>>environment_mode;
    in>>RefreshRate;
    in>>speciesSamples;
    in>>speciesSensitivity;
    in>>timeSliceConnect;
    in>>minspeciessize;
    in>>species_mode;

    //Bools
    in>>recalcFitness;
    in>>toroidal;
    in>>nonspatial;
    in>>breeddiff;
    in>>breedspecies;
    in>>path_on;
    in>>variableMutate;
    in>>allowExcludeWithDescendants;
    in>>sexual;
    in>>asexual;
    in>>variableBreed;
    in>>logging;
    in>>gui;
    in>>environment_interpolate;
    in>>fitnessLoggingToFile;
    bool in_bool;
    in>>in_bool;
    autodump_checkbox->setChecked(in_bool);
    in>>in_bool;
    save_population_count->setChecked(in_bool);
    in>>in_bool;
    save_mean_fitness->setChecked(in_bool);
    in>>in_bool;
    save_coding_genome_as_colour->setChecked(in_bool);
    in>>in_bool;
    save_species->setChecked(in_bool);
    in>>in_bool;
    save_non_coding_genome_as_colour->setChecked(in_bool);
    in>>in_bool;
    save_gene_frequencies->setChecked(in_bool);
    in>>in_bool;
    save_settles->setChecked(in_bool);
    in>>in_bool;
    save_fails_settles->setChecked(in_bool);
    in>>in_bool;
    save_environment->setChecked(in_bool);

    //Strings
    QString load_path;
    in>>load_path;
    path->setText(load_path);

    // 0 = Static
    // 1 = Once
    // 2 = Looped (default)
    // 3 = Bounce
    environment_mode=ENV_MODE_LOOP;
    in>>environment_mode;
    if (environment_mode==0) environmentModeStaticButton->setChecked(true);
    if (environment_mode==1) environmentModeOnceButton->setChecked(true);
    if (environment_mode==2) environmentModeLoopButton->setChecked(true);
    if (environment_mode==3) environmentModeBounceButton->setChecked(true);

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
    in>>vmode;
    int index = ui->populationWindowComboBox->findData(vmode);
    if ( index != -1 ) { // -1 for not found
       ui->populationWindowComboBox->setCurrentIndex(index);
    }

    int count;

    //file list
    in>>count;
    EnvFiles.clear();

    for (int i=0; i<count; i++)
    {
        QString t;
        in>>t;
        EnvFiles.append(t);
    }

    //now the big arrays
    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    for (int k=0; k<slotsPerSq; k++)
    {
        in>>critters[i][j][k].age;
        if (critters[i][j][k].age>0)
        {
            in>>critters[i][j][k].genome;
            in>>critters[i][j][k].ugenecombo;
            in>>critters[i][j][k].fitness;
            in>>critters[i][j][k].energy;
            in>>critters[i][j][k].xpos;
            in>>critters[i][j][k].ypos;
            in>>critters[i][j][k].zpos;
        }
    }

    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        in>>environment[i][j][0];
        in>>environment[i][j][1];
        in>>environment[i][j][2];
    }

    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        in>>totalfit[i][j];
    }

    for (int i=0; i<256; i++)
    for (int j=0; j<3; j++)
    {
        in>>xormasks[i][j];
    }

    //Genome Comparison Loading
    QByteArray Temp;
    in>>Temp;
    genoneComparison->loadComparison(Temp);


    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        in>>breedattempts[i][j];
        in>>breedfails[i][j];
        in>>settles[i][j];
        in>>settlefails[i][j];
        in>>maxused[i][j];
    }

    in>>Temp;
    restoreState(Temp); //window state

    bool btemp;
    in>>btemp; ui->actionGenomeComparison->setChecked(btemp);

    //interpolate environment stuff
    environment_interpolate = true;
    in>>environment_interpolate;

    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        in>>environmentlast[i][j][0];
        in>>environmentlast[i][j][1];
        in>>environmentlast[i][j][2];
    }

    for (int i=0; i<gridX; i++)
    for (int j=0; j<gridY; j++)
    {
        in>>environmentnext[i][j][0];
        in>>environmentnext[i][j][1];
        in>>environmentnext[i][j][2];
    }

    in>>speciesSamples;
    in>>speciesSensitivity;
    in>>timeSliceConnect;

    //now the species archive
    archivedspecieslists.clear();
    oldspecieslist.clear();

    int temp;
    in>>temp; //oldspecieslist.count();
    for (int j=0; j<temp; j++)
    {
         species s;
         in>>s.ID;
         in>>s.type;
         in>>s.origintime;
         in>>s.parent;
         in>>s.size;
         in>>s.internalID;
         oldspecieslist.append(s);
    }

    in>>temp; //archivedspecieslists.count();

    for (int i=0; i<temp; i++)
    {
        int temp2;
        in>>temp2; //archivedspecieslists.count();
        QList<species> ql;
        for (int j=0; j<temp2; j++)
        {
            species s;
            in>>s.ID;
            in>>s.type;
            in>>s.origintime;
            in>>s.parent;
            in>>s.size;
            in>>s.internalID;
            ql.append(s);
        }
        archivedspecieslists.append(ql);
    }
    in>>nextspeciesid;
    in>>lastSpeciesCalc; //actually no - if we import this it will assume an 'a' object exists.
    //bodge
    lastSpeciesCalc--;

    //now random array
    for (int i=0; i<65536; i++)
        in>>randoms[i];

    infile.close();
    NextRefresh=0;
    ResizeImageObjects();
    Report();
    Resize();

    update_gui_from_variables();
}

/*!
 * \brief MainWindow::genomeComparisonAdd
 * \return bool
 *
 * Adds a selected cell's genome to the Genome Comparison Dock
 */
bool MainWindow::genomeComparisonAdd()
{
    int x=popscene->selectedx;
    int y=popscene->selectedy;

    //---- Get genome colour
    if (totalfit[x][y]!=0) {
        for (int c=0; c<slotsPerSq; c++)
        {
            if (critters[x][y][c].age>0){
                genoneComparison->addGenomeCritter(critters[x][y][c],environment[x][y]);
                return true;
            }
        }
    }
    return false;
}

/*!
 * \brief MainWindow::on_actionShow_positions_triggered
 */
void MainWindow::on_actionShow_positions_triggered()
{
    RefreshEnvironment();
}

/*!
 * \brief MainWindow::on_actionFitness_logging_to_File_triggered
 */
void MainWindow::on_actionFitness_logging_to_File_triggered()
{
    fitnessLoggingToFile=ui->actionFitness_logging_to_File->isChecked();
}

/*!
 * \brief MainWindow::on_actionGenerate_Tree_from_Log_File_triggered
 */
void MainWindow::on_actionGenerate_Tree_from_Log_File_triggered()
{
    HandleAnalysisTool(ANALYSIS_TOOL_CODE_GENERATE_TREE);
}

/*!
 * \brief MainWindow::on_actionRates_of_Change_triggered
 */
void MainWindow::on_actionRates_of_Change_triggered()
{
    HandleAnalysisTool(ANALYSIS_TOOL_CODE_RATES_OF_CHANGE);
}

/*!
 * \brief MainWindow::on_actionStasis_triggered
 */
void MainWindow::on_actionStasis_triggered()
{
    HandleAnalysisTool(ANALYSIS_TOOL_CODE_STASIS);
}

/*!
 * \brief MainWindow::on_actionExtinction_and_Origination_Data_triggered
 */
void MainWindow::on_actionExtinction_and_Origination_Data_triggered()
{
    HandleAnalysisTool(ANALYSIS_TOOL_CODE_EXTINCT_ORIGIN);
}

/*!
 * \brief MainWindow::CalcSpecies
 */
void MainWindow::CalcSpecies()
{
    if (species_mode==SPECIES_MODE_NONE) return; //do nothing!
    if (generation!=lastSpeciesCalc)
    {
        delete a;  //replace old analyser object with new
        a=new Analyser;

        //New species analyser
        a->Groups_2017();

        //Makre sure this is updated
        lastSpeciesCalc=generation;

        //OLDER CODE
        /*
        if (ui->actionSpecies->isChecked() || speciesLogging) //do species calcs here even if not showing species - unless turned off in settings
        {
            //set up species ID

            for (int n=0; n<gridX; n++)
            for (int m=0; m<gridY; m++)
            {
                if (totalfit[n][m]==0) continue;
                int found=0;
                for (int c=0; c<slotsPerSq; c++)
                {
                    if (critters[n][m][c].age>0)
                    {
                        a->AddGenome_Fast(critters[n][m][c].genome);
                        if ((++found)>=speciesSamples) break; //limit number sampled
                    }
                }
            }

            a->Groups_With_History_Modal();
            lastSpeciesCalc=generation;
        }
        */
    }
}

/*!
 * \brief MainWindow::WriteLog
 *
 * Writes main ongoing log
 */
void MainWindow::WriteLog()
{
    //Need to sort out file name in batch mode, check breed list entries is actually working, etc. then deal with logging after run
    //RJG - write main ongoing log
    if(logging)
    {
        SpeciesLoggingFile=path->text();
        if(!SpeciesLoggingFile.endsWith(QDir::separator()))SpeciesLoggingFile.append(QDir::separator());
        SpeciesLoggingFile.append(QString(PRODUCTNAME)+"_log");
        if(batch_running)SpeciesLoggingFile.append(QString("_run_%1").arg(runs, 4, 10, QChar('0')));
        SpeciesLoggingFile.append(".txt");
        QFile outputfile(SpeciesLoggingFile);

       if (generation==0)
            {
                outputfile.open(QIODevice::WriteOnly|QIODevice::Text);
                QTextStream out(&outputfile);
                out<<"New run ";
                QDateTime t(QDateTime::currentDateTime());
                out<<t.toString(Qt::ISODate)<< "\n\n";
                out<<"===================\n\n";
                out<<print_settings();
                out<<"\n\n";
                out<<"===================\n";
                out<<"\nFor each iteration, this log features:\n\n";
                out<<"- [I] Iteration Number\n";
                out<<"- [P] Population Grid Data:\n";
                out<<"-- Number of living digital organisms\n";
                out<<"-- Mean fitness of living digital organisms\n";
                out<<"-- Number of entries on the breed list\n";
                out<<"-- Number of failed breed attempts\n";
                out<<"-- Number of species\n";
                out<<"- [S] Species Data:\n";
                out<<"-- Species ID\n";
                out<<"-- Species origin (iterations)\n";
                out<<"-- Species parent\n";
                out<<"-- Species current size (number of individuals)\n";
                out<<"-- Species current genome (for speed this is the genome of a randomly sampled individual, not the modal organism)\n\n";
                out<<"**Note that this excludes species with less individuals than Minimum species size, but is not able to exlude species without descendants, which can only be achieved with the end-run log.**\n\n";
                out<<"===================\n\n";
                outputfile.close();
            }

       outputfile.open(QIODevice::Append|QIODevice::Text);
       QTextStream out(&outputfile);

        out<<"[I] "<<generation<<"\n";

        int gridNumberAlive=0, gridTotalFitness=0, gridBreedEntries=0, gridBreedFails=0;
        for (int i=0; i<gridX; i++)
            for (int j=0; j<gridY; j++)
                    {
                    gridTotalFitness+=totalfit[i][j];
                    //----RJG: Manually count breed stufffor grid
                    gridBreedEntries+=breedattempts[i][j];
                    gridBreedFails+=breedfails[i][j];
                    //----RJG: Manually count number alive thanks to maxused descendants
                    for  (int k=0; k<slotsPerSq; k++)if(critters[i][j][k].fitness)gridNumberAlive++;
                    }
        double mean_fitness=(double)gridTotalFitness/(double)gridNumberAlive;

        out<<"[P] "<<gridNumberAlive<<","<<mean_fitness<<","<<gridBreedEntries<<","<<gridBreedFails<<","<<oldspecieslist.count()<<"\n";

        //----RJG: And species details for each iteration
        for (int i=0; i<oldspecieslist.count(); i++)
        {
            //----RJG: Unable to exclude species without descendants, for obvious reasons.
            if(oldspecieslist[i].size>minspeciessize)
            {
                out<<"[S] ";
                out<<(oldspecieslist[i].ID)<<",";
                out<<oldspecieslist[i].origintime<<",";
                out<<oldspecieslist[i].parent<<",";
                out<<oldspecieslist[i].size<<",";
                //---- RJG - output binary genome if needed
                for (int j=0; j<63; j++)if (tweakers64[63-j] & oldspecieslist[i].type) out<<"1"; else out<<"0";
                if (tweakers64[0] & oldspecieslist[i].type) out<<"1"; else out<<"0";
                out<<"\n";
            }
        }
        out<<"\n";
        outputfile.close();
      }


    // ----RJG recombination logging to separate file
    //Need to add this to GUI
    if (ui->actionRecombination_logging->isChecked())
    {
        QString rFile(path->text()+QString(PRODUCTNAME)+"_recombination");
        if(batch_running)
            rFile.append(QString("_run_%1").arg(runs, 4, 10, QChar('0')));
        rFile.append(".txt");
        QFile routputfile(rFile);

        if (!(routputfile.exists()))
        {
            routputfile.open(QIODevice::WriteOnly|QIODevice::Text);
            QTextStream rout(&routputfile);

            // Info on simulation setup
            rout<<print_settings()<<"\n";
            rout<<"\nNote on log: this only calculates proportions when variable breeding is selected for speed, and also currently will only count total breed attempts when the fitness log is also running.";
            rout<<"For now, this is merely a list of:\nIteration\tAsexual breeds\tSexual breeds\tTotal breed attempts\tTotal breed fails\tTotal Alive\tPercent sexual.\n";

            routputfile.close();
        }

        routputfile.open(QIODevice::Append|QIODevice::Text);
        QTextStream rout(&routputfile);

        rout<<generation<<"\t";

        //RJG count breeding. There is probably a better way to do this, but keeping as is for now as not too slow
        int cntAsex=0, cntSex=0;
        int totalBreedAttempts=0, totalBreedFails=0;

        for (int i=0; i<gridX; i++)
                for (int j=0; j<gridY; j++)
                        {
                        for (int c=0; c<slotsPerSq; c++)
                            if (critters[i][j][c].fitness)
                                {
                                    if(critters[i][j][c].return_recomb()<0)cntAsex++;
                                    else if (critters[i][j][c].return_recomb()>0)cntSex++;
                                }
                        totalBreedAttempts+=breedattempts[i][j];
                        totalBreedFails+=breedfails[i][j];

                        }
        double percent=((float)cntSex/(float)(cntAsex+cntSex))*100.;
        rout<<cntAsex<<"\t"<<cntSex<<"\t"<<totalBreedAttempts<<"\t"<<totalBreedFails<<"\t"<<AliveCount<<"\t"<<percent<<"\n";

        routputfile.close();
    }

    //Add this to GUI too
    // ----RJG log fitness to separate file
    if (ui->actionFitness_logging_to_File->isChecked())
    {

        QString File(path->text()+"EvoSim_fitness");
        if(batch_running)
            File.append(QString("_run_%1").arg(runs, 4, 10, QChar('0')));
        File.append(".txt");
        QFile outputfile(File);

        if (!(outputfile.exists()))
        {
            outputfile.open(QIODevice::WriteOnly|QIODevice::Text);
            QTextStream out(&outputfile);

            // Info on simulation setup
            out<<print_settings()<<"\n";

            //Different versions of output, for reuse as needed
            //out<<"Each generation lists, for each pixel: mean fitness, entries on breed list";
            //out<<"Each line lists generation, then the grid's: total critter number, total fitness, total entries on breed list";
            //out<<"Each generation lists, for each pixel (top left to bottom right): total fitness, number of critters,entries on breed list\n\n";
            out<<"Each generation lists, for the grid: the number of living crittes, then the total fitness of all those critters, and then the number of entries on the breed list";
            out<<"\n";
           outputfile.close();
        }

        outputfile.open(QIODevice::Append|QIODevice::Text);
        QTextStream out(&outputfile);

        // ----RJG: Breedattempts was no longer in use - but seems accurate, so can be co-opted for this.
        //out<<"Iteration: "<<generation<<"\n";
        out<<generation<<"\t";
        int gridNumberAlive=0, gridTotalFitness=0, gridBreedEntries=0;

        for (int i=0; i<gridX; i++)
            {
                for (int j=0; j<gridY; j++)
                    {
                     //----RJG: Total fitness per grid square.
                     //out<<totalfit[i][j];

                    //----RJG: Number alive per square - output with +1 due to c numbering, zero is one critter, etc.
                    //out<<maxused[i][j]+1;
                    // ---- RJG: Note, however, there is an descendants that when critters die they remain in cell list for iteration
                    // ---- RJG: Easiest to account for this by removing those which are dead from alive count, or recounting - rather than dealing with death system
                    // int numberalive=0;

                    //----RJG: In case mean is ever required:
                    //float mean=0;
                    // mean = (float)totalfit[i][j]/(float)maxused[i][j]+1;

                    //----RJG: Manually calculate total fitness for grid
                    gridTotalFitness+=totalfit[i][j];

                    int critters_alive=0;

                     //----RJG: Manually count number alive thanks to maxused descendants
                    for  (int k=0; k<slotsPerSq; k++)if(critters[i][j][k].fitness)
                                    {
                                    //numberalive++;
                                    gridNumberAlive++;
                                    critters_alive++;
                                    }

                    //total fitness, number of critters,entries on breed list";
                    //out<<totalfit[i][j]<<" "<<critters_alive<<" "<<breedattempts[i][j];

                    //----RJG: Manually count breed attempts for grid
                    gridBreedEntries+=breedattempts[i][j];

                    //out<<"\n";

                    }
            }

        //---- RJG: If outputting averages to log.
        //float avFit=(float)gridTotalFitness/(float)gridNumberAlive;
        //float avBreed=(float)gridBreedEntries/(float)gridNumberAlive;
        //out<<avFit<<","<<avBreed;

        //---- RJG: If outputting totals
        //critter - fitness - breeds

        double mean_fitness= (double)gridTotalFitness / (double)gridNumberAlive;

        out<<mean_fitness<<"\t"<<gridBreedEntries<<"\t"<<gridNumberAlive<<"\n";
        outputfile.close();
      }
}

/*!
 * \brief MainWindow::setStatusBarText
 * \param text
 *
 * Sets the status bar text.
 */
void MainWindow::setStatusBarText(QString text)
{
    ui->statusBar->showMessage(text);
}

void MainWindow::statusProgressBar(QProgressBar *prBar,bool add)
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
                            "*.*");

    if (file.length()==0) return;

    int seedoffset;
    seedoffset=QInputDialog::getInt(this,"Seed value","Byte offset to start reading from (will read 65536 bytes)");

    //now read in values to array
    QFile rfile(file);
    rfile.open(QIODevice::ReadOnly);

    rfile.seek(seedoffset);

    int i=rfile.read((char *)randoms,65536);
    if (i!=65536) QMessageBox::warning(this,"Oops","Failed to read 65536 bytes from file - random numbers may be compromised - try again or restart program");
    else QMessageBox::information(this,"Success","New random numbers read successfully");
}

/*!
 * \brief MainWindow::HandleAnalysisTool
 * \param code
 * \return text
 *
 * Handle analyses at end of run for data.
 */
QString MainWindow::HandleAnalysisTool(int code)
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
            int red = QInputDialog::getInt(this, "Count peaks...","Red level?", 128, 0, 255, 1, &ok);
            if(!ok)return QString("");
            int green= QInputDialog::getInt(this, "Count peaks...","Green level?", 128, 0, 255, 1, &ok);
            if(!ok)return QString("");;
            int blue= QInputDialog::getInt(this, "Count peaks...","Green level?", 128, 0, 255, 1, &ok);
            if(!ok)return QString("");
            OutputString = a.CountPeaks(red,green,blue);
            break;
            }

        case ANALYSIS_TOOL_CODE_MAKE_NEWICK:
            if (phylogeny_button->isChecked()||phylogeny_and_metrics_button->isChecked())OutputString = a.MakeNewick(rootspecies, minspeciessize, allowExcludeWithDescendants);
            else OutputString = "Species tracking is not enabled.";
            break;

        case ANALYSIS_TOOL_CODE_DUMP_DATA:
            if (phylogeny_and_metrics_button->isChecked())OutputString = a.DumpData(rootspecies, minspeciessize, allowExcludeWithDescendants);
            else OutputString = "Species tracking is not enabled, or is set to phylogeny only.";
            break;

        default:
            QMessageBox::warning(this,"Error","No handler for analysis tool");
            return QString("Error, No handler for analysis tool");

    }

    //write result to screen
    //ui->plainTextEdit->clear();
    //ui->plainTextEdit->appendPlainText(OutputString);

    return OutputString;
 }

/*!
 * \brief MainWindow::on_actionGenerate_NWK_tree_file_triggered
 */
void MainWindow::on_actionGenerate_NWK_tree_file_triggered()
{
    dump_run_data();
}

/*!
 * \brief MainWindow::on_actionSpecies_sizes_triggered
 */
void MainWindow::on_actionSpecies_sizes_triggered()
{
    dump_run_data();
}

/*!
 * \brief MainWindow::print_settings
 * \return text
 */
QString MainWindow::print_settings()
{
    QString settings;
    QTextStream settings_out(&settings);

    settings_out<<"EvoSim settings:\n\n";

    settings_out<<"- Integers:\n";
    settings_out<<"-- Grid X: "<<gridX<<"\n";
    settings_out<<"-- Grid Y: "<<gridY<<"\n";
    settings_out<<"-- Settle tolerance: "<<settleTolerance<<"\n";
    settings_out<<"-- Start age: "<<startAge<<"\n";
    settings_out<<"-- Disperal: "<<dispersal<<"\n";
    settings_out<<"-- Food: "<<food<<"\n";
    settings_out<<"-- Breed cost: "<<breedCost<<"\n";
    settings_out<<"-- Mutate: "<<mutate<<"\n";
    settings_out<<"-- Pathogen mutate: "<<pathogen_mutate<<"\n";
    settings_out<<"-- Pathogen frequency: "<<pathogen_frequency<<"\n";
    settings_out<<"-- Max diff to breed: "<<maxDiff<<"\n";
    settings_out<<"-- Breed threshold: "<<breedThreshold<<"\n";
    settings_out<<"-- Slots per square: "<<slotsPerSq<<"\n";
    settings_out<<"-- Fitness target: "<<target<<"\n";
    settings_out<<"-- Environmental change rate: "<<envchangerate<<"\n";
    settings_out<<"-- Minimum species size:"<<minspeciessize<<"\n";
    settings_out<<"-- Environment mode:"<<environment_mode<<"\n";
    settings_out<<"-- Speices mode:"<<species_mode<<"\n";

    settings_out<<"\n- Bools:\n";
    settings_out<<"-- Recalculate fitness: "<<recalcFitness<<"\n";
    settings_out<<"-- Toroidal environment: "<<toroidal<<"\n";
    settings_out<<"-- Interpolate environment: "<<environment_interpolate<<"\n";
    settings_out<<"-- Nonspatial setling: "<<nonspatial<<"\n";
    settings_out<<"-- Enforce max diff to breed:"<<breeddiff<<"\n";
    settings_out<<"-- Only breed within species:"<<breedspecies<<"\n";
    settings_out<<"-- Pathogens enabled:"<<path_on<<"\n";
    settings_out<<"-- Variable mutate:"<<variableMutate<<"\n";
    settings_out<<"-- Exclude species without descendants:"<<allowExcludeWithDescendants<<"\n";
    settings_out<<"-- Breeding: ";
    if(sexual)settings_out<<"sexual"<<"\n";
    else if (asexual)settings_out<<"asexual"<<"\n";
    else settings_out<<"variable"<<"\n";

    return settings;
}

/*!
 * \brief MainWindow::load_settings
 *
 * Save and load settings (but not critter info, masks etc.).
 */
void MainWindow::load_settings()
{
    QString settings_filename=QFileDialog::getOpenFileName(this, tr("Open File"),path->text(),"XML files (*.xml)");
    if (settings_filename.length()<3) return;
    QFile settings_file(settings_filename);
    if(!settings_file.open(QIODevice::ReadOnly))
            {
            setStatusBarText("Error opening file.");
            return;
            }

           QXmlStreamReader settings_file_in(&settings_file);

           while (!settings_file_in.atEnd()&& !settings_file_in.hasError())
               {

                /* Read next element.*/
                QXmlStreamReader::TokenType token = settings_file_in.readNext();
                /* If token is just StartDocument, we'll go to next.*/

                if(token == QXmlStreamReader::StartDocument)continue;
                if(token == QXmlStreamReader::StartElement)
                    {
                         //Ints
                         if(settings_file_in.name() == "revosim")continue;
                         if(settings_file_in.name() == "gridX")gridX=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "gridY")gridY=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "settleTolerance")settleTolerance=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "slotsPerSq")slotsPerSq=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "startAge")startAge=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "dispersal")dispersal=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "food")food=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "breedCost")breedCost=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "mutate")mutate=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "pathogen_mutate")pathogen_mutate=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "pathogen_frequency")pathogen_frequency=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "maxDiff")maxDiff=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "breedThreshold")breedThreshold=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "target")target=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "envchangerate")envchangerate=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "RefreshRate")RefreshRate=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "environment_mode")environment_mode_changed(settings_file_in.readElementText().toInt(),true);
                         //No Gui options for the remaining settings as yet.
                         if(settings_file_in.name() == "speciesSamples")speciesSamples=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "speciesSensitivity")speciesSensitivity=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "timeSliceConnect")timeSliceConnect=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "minspeciessize")minspeciessize=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "species_mode")minspeciessize=settings_file_in.readElementText().toInt();

                         //Bools
                         if(settings_file_in.name() == "recalcFitness")recalcFitness=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "toroidal")toroidal=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "nonspatial")nonspatial=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "breeddiff")breeddiff=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "breedspecies")breedspecies=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "path_on")path_on=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "variableMutate")variableMutate=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "allowExcludeWithDescendants")allowExcludeWithDescendants=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "sexual")sexual=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "asexual")asexual=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "variableBreed")variableBreed=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "logging")logging=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "gui")gui=settings_file_in.readElementText().toInt();
                         if(settings_file_in.name() == "environment_interpolate")environment_interpolate=settings_file_in.readElementText().toInt();
                         //No gui options for below
                         if(settings_file_in.name() == "fitnessLoggingToFile")fitnessLoggingToFile=settings_file_in.readElementText().toInt();
                         //Only GUI options
                         if(settings_file_in.name() == "autodump")autodump_checkbox->setChecked(settings_file_in.readElementText().toInt());
                         if(settings_file_in.name() == "save_population_count")save_population_count->setChecked(settings_file_in.readElementText().toInt());
                         if(settings_file_in.name() == "save_mean_fitness")save_mean_fitness->setChecked(settings_file_in.readElementText().toInt());
                         if(settings_file_in.name() == "save_coding_genome_as_colour")save_coding_genome_as_colour->setChecked(settings_file_in.readElementText().toInt());
                         if(settings_file_in.name() == "save_species")save_species->setChecked(settings_file_in.readElementText().toInt());
                         if(settings_file_in.name() == "save_non_coding_genome_as_colour")save_non_coding_genome_as_colour->setChecked(settings_file_in.readElementText().toInt());
                         if(settings_file_in.name() == "save_gene_frequencies")save_gene_frequencies->setChecked(settings_file_in.readElementText().toInt());
                         if(settings_file_in.name() == "save_settles")save_settles->setChecked(settings_file_in.readElementText().toInt());
                         if(settings_file_in.name() == "save_fails_settles")save_fails_settles->setChecked(settings_file_in.readElementText().toInt());
                         if(settings_file_in.name() == "save_environment")save_environment->setChecked(settings_file_in.readElementText().toInt());

                         //Strings
                         if(settings_file_in.name() == "path")path->setText(settings_file_in.readElementText());

                       }
               }
           // Error
           if(settings_file_in.hasError()) setStatusBarText("There seems to have been an error reading in the XML file. Not all settings will have been loaded.");
           else setStatusBarText("Loaded settings file");

           settings_file.close();

           update_gui_from_variables();
}

/*!
 * \brief MainWindow::update_gui_from_variables
 *
 * Call this from either load settings, or plain load. Updates gui from those variables held as simulation globals
 */
void MainWindow::update_gui_from_variables()
{
    //Ints
    gridX_spin->setValue(gridX);
    gridY_spin->setValue(gridY);
    settleTolerance_spin->setValue(settleTolerance);
    slots_spin->setValue(slotsPerSq);
    startAge_spin->setValue(startAge);
    dispersal_spin->setValue(dispersal);
    energy_spin->setValue(food);
    breedCost_spin->setValue(breedCost);
    mutate_spin->setValue(mutate);
    pathogen_mutate_spin->setValue(pathogen_mutate);
    pathogen_frequency_spin->setValue(pathogen_frequency);
    maxDiff_spin->setValue(maxDiff);
    breedThreshold_spin->setValue(breedThreshold);
    target_spin->setValue(target);
    environment_rate_spin->setValue(envchangerate);
    refreshRateSpin->setValue(RefreshRate);
    // Add species_mode
    species_mode_changed(species_mode, true);
    // Add environment_mode
    environment_mode_changed(environment_mode, true);

    //Bools
    recalcFitness_checkbox->setChecked(recalcFitness);
    toroidal_checkbox->setChecked(toroidal);
    nonspatial_checkbox->setChecked(nonspatial);
    breeddiff_checkbox->setChecked(breeddiff);
    breedspecies_checkbox->setChecked(breedspecies);
    pathogens_checkbox->setChecked(path_on);
    variable_mutation_checkbox->setChecked(variableMutate);
    exclude_without_descendants_checkbox->setChecked(allowExcludeWithDescendants);
    sexual_radio->setChecked(sexual);
    asexual_radio->setChecked(asexual);
    variableBreed_radio->setChecked(variableBreed);
    logging_checkbox->setChecked(logging);
    gui_checkbox->setChecked(gui);
    interpolateCheckbox->setChecked(environment_interpolate);
}

/*!
 * \brief MainWindow::save_settings
 */
void MainWindow::save_settings()
{
    QString settings_filename=QFileDialog::getSaveFileName(this, tr("Save file as..."),QString(path->text()+QString(PRODUCTNAME)+"_settings.xml"));
    if(!settings_filename.endsWith(".xml"))settings_filename.append(".xml");
    QFile settings_file(settings_filename);
    if(!settings_file.open(QIODevice::WriteOnly|QIODevice::Text))
        {
            setStatusBarText("Error opening settings file to write to.");
            return;
        }

        QXmlStreamWriter settings_file_out(&settings_file);
        settings_file_out.setAutoFormatting(true);
        settings_file_out.setAutoFormattingIndent(-2);

        settings_file_out.writeStartDocument();

        settings_file_out.writeStartElement("revosim");

        //Ints
        settings_file_out.writeStartElement("gridX");
        settings_file_out.writeCharacters(QString("%1").arg(gridX));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("gridY");
        settings_file_out.writeCharacters(QString("%1").arg(gridY));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("settleTolerance");
        settings_file_out.writeCharacters(QString("%1").arg(settleTolerance));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("slotsPerSq");
        settings_file_out.writeCharacters(QString("%1").arg(slotsPerSq));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("startAge");
        settings_file_out.writeCharacters(QString("%1").arg(startAge));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("dispersal");
        settings_file_out.writeCharacters(QString("%1").arg(dispersal));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("food");
        settings_file_out.writeCharacters(QString("%1").arg(food));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("breedCost");
        settings_file_out.writeCharacters(QString("%1").arg(breedCost));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("mutate");
        settings_file_out.writeCharacters(QString("%1").arg(mutate));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("pathogen_mutate");
        settings_file_out.writeCharacters(QString("%1").arg(pathogen_mutate));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("pathogen_frequency");
        settings_file_out.writeCharacters(QString("%1").arg(pathogen_frequency));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("maxDiff");
        settings_file_out.writeCharacters(QString("%1").arg(maxDiff));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("breedThreshold");
        settings_file_out.writeCharacters(QString("%1").arg(breedThreshold));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("target");
        settings_file_out.writeCharacters(QString("%1").arg(target));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("envchangerate");
        settings_file_out.writeCharacters(QString("%1").arg(envchangerate));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("environment_mode");
        settings_file_out.writeCharacters(QString("%1").arg(environment_mode));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("RefreshRate");
        settings_file_out.writeCharacters(QString("%1").arg(RefreshRate));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("speciesSamples");
        settings_file_out.writeCharacters(QString("%1").arg(speciesSamples));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("speciesSensitivity");
        settings_file_out.writeCharacters(QString("%1").arg(speciesSensitivity));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("timeSliceConnect");
        settings_file_out.writeCharacters(QString("%1").arg(timeSliceConnect));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("minspeciessize");
        settings_file_out.writeCharacters(QString("%1").arg(minspeciessize));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("species_mode");
        settings_file_out.writeCharacters(QString("%1").arg(species_mode));
        settings_file_out.writeEndElement();

        //Bools
        settings_file_out.writeStartElement("recalcFitness");
        settings_file_out.writeCharacters(QString("%1").arg(recalcFitness));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("toroidal");
        settings_file_out.writeCharacters(QString("%1").arg(toroidal));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("nonspatial");
        settings_file_out.writeCharacters(QString("%1").arg(nonspatial));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("breeddiff");
        settings_file_out.writeCharacters(QString("%1").arg(breeddiff));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("breedspecies");
        settings_file_out.writeCharacters(QString("%1").arg(breedspecies));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("path_on");
        settings_file_out.writeCharacters(QString("%1").arg(path_on));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("variableMutate");
        settings_file_out.writeCharacters(QString("%1").arg(variableMutate));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("allowExcludeWithDescendants");
        settings_file_out.writeCharacters(QString("%1").arg(allowExcludeWithDescendants));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("sexual");
        settings_file_out.writeCharacters(QString("%1").arg(sexual));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("asexual");
        settings_file_out.writeCharacters(QString("%1").arg(asexual));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("variableBreed");
        settings_file_out.writeCharacters(QString("%1").arg(variableBreed));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("logging");
        settings_file_out.writeCharacters(QString("%1").arg(logging));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("gui");
        settings_file_out.writeCharacters(QString("%1").arg(gui));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("environment_interpolate");
        settings_file_out.writeCharacters(QString("%1").arg(environment_interpolate));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("fitnessLoggingToFile");
        settings_file_out.writeCharacters(QString("%1").arg(fitnessLoggingToFile));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("autodump");
        settings_file_out.writeCharacters(QString("%1").arg(autodump_checkbox->isChecked()));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("save_population_count");
        settings_file_out.writeCharacters(QString("%1").arg(save_population_count->isChecked()));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("save_mean_fitness");
        settings_file_out.writeCharacters(QString("%1").arg(save_mean_fitness->isChecked()));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("save_coding_genome_as_colour");
        settings_file_out.writeCharacters(QString("%1").arg(save_coding_genome_as_colour->isChecked()));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("save_species");
        settings_file_out.writeCharacters(QString("%1").arg(save_species->isChecked()));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("save_non_coding_genome_as_colour");
        settings_file_out.writeCharacters(QString("%1").arg(save_non_coding_genome_as_colour->isChecked()));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("save_gene_frequencies");
        settings_file_out.writeCharacters(QString("%1").arg(save_gene_frequencies->isChecked()));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("save_settles");
        settings_file_out.writeCharacters(QString("%1").arg(save_settles->isChecked()));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("save_fails_settles");
        settings_file_out.writeCharacters(QString("%1").arg(save_fails_settles->isChecked()));
        settings_file_out.writeEndElement();

        settings_file_out.writeStartElement("save_environment");
        settings_file_out.writeCharacters(QString("%1").arg(save_environment->isChecked()));
        settings_file_out.writeEndElement();

        //Strings
        settings_file_out.writeStartElement("path");
        settings_file_out.writeCharacters(path->text());
        settings_file_out.writeEndElement();

        settings_file_out.writeEndElement();

        settings_file_out.writeEndDocument();

        settings_file.close();

       setStatusBarText("File saved");
}

/*!
 * \brief MainWindow::on_actionExit_triggered
 *
 * Action to exit the program.
 */
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

/*!
 * \brief MainWindow::on_actionCode_on_GitHub_triggered
 *
 * Trigger to open url to gihub repository.
 */
void MainWindow::on_actionCode_on_GitHub_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(GITHUB_URL) + QString(GITREPOSITORY)));
}

/*!
 * \brief MainWindow::on_actionOnline_User_Manual_triggered
 *
 * Trigger to open url to online documentation
 */
void MainWindow::on_actionOnline_User_Manual_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(READTHEDOCS_URL)));
}

/*!
 * \brief MainWindow::on_actionSettings_Dock_triggered
 */
void MainWindow::on_actionSettings_Dock_triggered()
{
    actionSettings_triggered();
}

/*!
 * \brief MainWindow::on_actionGenomeComparison_triggered
 * \param checked
 *
 * Action to open the Genome Comparison Dock.
 */
void MainWindow::on_actionGenomeComparison_triggered(bool checked)
{
    ui->genomeComparisonDock->setVisible(checked);
    ui->actionGenomeComparison->setChecked(checked);
    genomeComparisonButton->setChecked(checked);
}

void MainWindow::process_app_events()
{
        qApp->processEvents();
}
