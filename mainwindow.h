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

extern MainWindow *MainWin;

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
    void setStatusBarText(const QString& text);
    void statusProgressBar(QProgressBar *prBar, bool add);
    void createMainToolbar();
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
    int scaleFails(int fails, float generations);
    void closeEvent(QCloseEvent *e);
    void report();
    void runSetUp();
    void finishRun();
    void refreshPopulations();
    void resetInformationBar();

    bool stopFlag{};
    bool pauseFlag;
    int waitUntilPauseSignalIsEmitted();
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
    QCheckBox *save_species{};
    QCheckBox *save_non_coding_genome_as_colour{};
    QCheckBox *save_gene_frequencies{};
    QCheckBox *save_settles{};
    QCheckBox *save_fails_settles{};
    QCheckBox *save_environment{};
    QCheckBox *interpolateCheckbox{};

    //RJG - other checkboxes
    QCheckBox *recalcFitness_checkbox{};
    QCheckBox *toroidal_checkbox{};
    QCheckBox *nonspatial_checkbox{};
    QCheckBox *breeddiff_checkbox{};
    QCheckBox *breedspecies_checkbox{};
    QCheckBox *pathogens_checkbox{};
    QCheckBox *variable_mutation_checkbox{};
    QCheckBox *exclude_without_descendants_checkbox{};
    QCheckBox *logging_checkbox{};
    QCheckBox *autowrite_checkbox{};

    //RJG - radios and spins
    QRadioButton *phylogeny_off_button{};
    QRadioButton *basic_phylogeny_button{};
    QRadioButton *phylogeny_button{};
    QRadioButton *phylogeny_and_metrics_button{};
    QRadioButton *sexual_radio{};
    QRadioButton *asexual_radio{};
    QRadioButton *variableBreed_radio{};
    QRadioButton *environmentModeBounceButton{};
    QRadioButton *environmentModeLoopButton{};
    QRadioButton *environmentModeOnceButton{};
    QRadioButton *environmentModeStaticButton{};

    QSpinBox *mutate_spin{};
    QSpinBox *refreshRateSpin{};
    QSpinBox *pathogen_mutate_spin{};
    QSpinBox *pathogen_frequency_spin{};
    QSpinBox *maxDiff_spin{};
    QSpinBox *breedThreshold_spin{};
    QSpinBox *target_spin{};
    QSpinBox *environment_rate_spin{};
    QSpinBox *gridX_spin{};
    QSpinBox *gridY_spin{};
    QSpinBox *settleTolerance_spin{};
    QSpinBox *slots_spin{};
    QSpinBox *startAge_spin{};
    QSpinBox *dispersal_spin{};
    QSpinBox *energy_spin{};
    QSpinBox *breedCost_spin{};

    //RJG - global save path for all outputs
    QLineEdit *path{};

    //RJG - options for batching
    bool batch_running;
    int runs, batch_iterations, batch_target_runs;

    //Now some things settable by the ui
    QTime timer;
    QImage *pop_image, *env_image, *pop_image_colour;
    QGraphicsPixmapItem *pop_item, *env_item;

    void ResetSquare(int n, int m);
    void ResizeImageObjects();
    void WriteLog();
    void CalcSpecies();
    QString HandleAnalysisTool(int code);
    Analyser *a;
    QString print_settings();

private slots:
    void on_actionReset_triggered();
    void on_actionReseed_triggered();
    void on_actionStart_Sim_triggered();
    void on_actionRun_for_triggered();
    void on_actionBatch_triggered();
    void on_actionPause_Sim_triggered();
    void on_actionStop_Sim_triggered();
    void on_actionCount_Peaks_triggered();
    void gui_checkbox_state_changed(bool);
    void save_all_checkbox_state_changed(bool);
    void write_run_data();
    void actionSettings_triggered();
    void redoImages(int oldrows, int oldcols);
    bool actionEnvironment_Files_triggered();
    void on_actionSave_triggered();
    void on_actionLoad_triggered();
    void load_settings();
    void save_settings();
    void on_actionShow_positions_triggered();
    void on_actionFitness_logging_to_File_triggered();
    void on_actionGenerate_Tree_from_Log_File_triggered();
    void on_actionExtinction_and_Origination_Data_triggered();
    void on_actionRates_of_Change_triggered();
    void on_actionStasis_triggered();
    void on_actionLoad_Random_Numbers_triggered();
    void species_mode_changed(int change_species_mode, bool update_gui);
    void environment_mode_changed(int change_environment_mode, bool update_gui);
    void on_actionGenerate_NWK_tree_file_triggered();
    void on_actionSpecies_sizes_triggered();
    void changepath_triggered();
    void on_actionAbout_triggered();
    void on_actionExit_triggered();
    void on_populationWindowComboBox_currentIndexChanged(int index);
    void on_actionCode_on_GitHub_triggered();
    void on_actionOnline_User_Manual_triggered();
    void on_actionSettings_Dock_triggered();
    void on_actionGenomeComparison_triggered(bool checked);
};

#endif // MAINWINDOW_H
