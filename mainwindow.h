/**
 * @file
 * Header: Main Window
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QAction>
#include <QStandardPaths>
#include <QCheckBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QShortcut>

#include "simmanager.h"
#include "populationscene.h"
#include "environmentscene.h"
#include "genomecomparison.h"
#include "about.h"

extern MainWindow *MainWin;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    GenomeComparison *genoneComparison;
    bool genomeComparisonAdd();
    void RefreshReport();
    void Resize();
    void RefreshEnvironment();
    void setStatusBarText(QString text);
    Ui::MainWindow *ui;
    QDockWidget* createSimulationSettingsDock();
    QDockWidget* createOutputSettingsDock();
    QDockWidget* createOrganismSettingsDock();
    void createMainToolbar();

    int RefreshRate;

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    int ScaleFails(int fails, float gens);
    void closeEvent(QCloseEvent *e);
    void Report();
    void RunSetUp();
    void FinishRun();
    void RefreshPopulations();
    void resetInformationBar();

    //RJG - some imporant variables
    bool stopflag;
    bool pauseflag;
    int waitUntilPauseSignalIsEmitted();
    int NextRefresh;

    //RJG - GUI stuff
    EnvironmentScene *envscene;
    PopulationScene *popscene;
    QActionGroup *viewgroup2;
    QDockWidget *simulationSettingsDock, *organismSettingsDock, *outputSettingsDock;

    //RJG - GUI buttons and settings docker options which need to be accessible via slots, especially for load settings function
    QAction *startButton, *stopButton, *pauseButton, *runForButton, *resetButton, *reseedButton, *runForBatchButton, *genomeComparisonButton, *settingsButton, *aboutButton;
    //RJG - Save images checkboxes
    QCheckBox *gui_checkbox, *save_population_count, *save_mean_fitness, *save_coding_genome_as_colour, *save_species, *save_non_coding_genome_as_colour, *save_gene_frequencies, *save_settles, *save_fails_settles, *save_environment, *interpolateCheckbox;
    //RJG - other checkboxes
    QCheckBox *recalcFitness_checkbox, *toroidal_checkbox, *nonspatial_checkbox, *breeddiff_checkbox, *breedspecies_checkbox, *pathogens_checkbox, *variable_mutation_checkbox, *exclude_without_descendants_checkbox, *logging_checkbox, *autodump_checkbox;
    //RJG - radios and spins
    QRadioButton *phylogeny_off_button, *basic_phylogeny_button, *phylogeny_button, *phylogeny_and_metrics_button, *sexual_radio, *asexual_radio, *variableBreed_radio, *environmentModeBounceButton, *environmentModeLoopButton, *environmentModeOnceButton, *environmentModeStaticButton;
    QSpinBox *mutate_spin, *refreshRateSpin, *pathogen_mutate_spin, *pathogen_frequency_spin , *maxDiff_spin, *breedThreshold_spin, *target_spin , *environment_rate_spin, *gridX_spin , *gridY_spin, *settleTolerance_spin, *slots_spin, *startAge_spin, *dispersal_spin, *energy_spin, *breedCost_spin;
    //RJG - global save path for all outputs
    QLineEdit *path;

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
    void dump_run_data();
    void actionSettings_triggered();
    void redoImages(int oldrows, int oldcols);
    bool actionEnvironment_Files_triggered();
    void on_actionSave_triggered();
    void on_actionLoad_triggered();
    void load_settings();
    void update_gui_from_variables();
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
