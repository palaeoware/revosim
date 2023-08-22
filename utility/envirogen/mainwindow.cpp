/**
 * @file
 * Source: MainWindow
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark Sutton, Russell Garwood,
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

#include "markenvironment.h"
#include "noiseenvironment.h"
#include "combine.h"
#include "colour.h"
#include "makestack.h"
#include "russellenvironment.h"
#include "about.h"
#include "globals.h"

#include <QDockWidget>
#include <QProgressDialog>
#include <QProgressBar>
#include <QColorDialog>
#include <QStandardPaths>
#include <QShortcut>
#include <QDebug>
#include <QInputDialog>
#include <QtConcurrent>

MainWindow *MainWin;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) + " - compiled - " + __DATE__);
    setWindowIcon(QIcon (":/icon.png"));
    showMaximized();
    MainWin = this;
    simulationRandoms = new randoms;

    //RJG - Globals for simulation
    generations = 500;
    iterations = 0;
    runs = 0;
    stackOneSize = 0;
    stackTwoSize = 0;
    stopFlag = false;
    pauseFlag = false;
    ui->path->setText(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));

    //RJG - set up graphics
    env_image = new QImage(100, 100, QImage::Format_RGB32);
    env_image->fill(QColor(42, 42, 42));
    env_item = new QGraphicsPixmapItem(QPixmap::fromImage(*env_image));

    environmentScene = new EnvironmentScene;
    environmentScene->mw = this;
    environmentScene->addItem(env_item);
    ui->enviroView->setScene(environmentScene);

    //RJG - Populate labels in GUI as required
    ui->envSettings->setWindowTitle("Environmental Settings");
    ui->display_label->setStyleSheet("font-weight: bold");
    ui->output_label->setStyleSheet("font-weight: bold");

    ui->russell_label->setAlignment(Qt::AlignJustify);
    ui->russell_label->setWordWrap(true);
    ui->russell_label->setText("Please enter the settings for the dynamic environment below. Most are obvious. Convergence sets the amount of smoothing between spots and background. 0.01 is very smooth, but will initially take about a minute to create environment.");

    ui->m_environment_label->setAlignment(Qt::AlignJustify);
    ui->m_environment_label->setWordWrap(true);
    ui->m_environment_label->setText("Please enter the settings for the dynamic environment below. Most are obvious.");

    ui->noise_label->setAlignment(Qt::AlignJustify);
    ui->noise_label->setWordWrap(true);
    ui->noise_label->setText("This will provide a stack of noise images, which could be of utility for combining with other kinds of image stacks.");

    ui->combine_label->setAlignment(Qt::AlignJustify);
    ui->combine_label->setWordWrap(true);
    ui->combine_label->setText("Combines two stacks between the start and end of either stack. If start slice is greater than zero, "
                               "it will copy stack one to start slice, and then combine stack two from that point on. "
                               "Percentage start and end dictate influence of stack one for slices which are combined, and "
                               "the programme interpolates between these. If stack one ends, stack two will be copied to "
                               "the end. This can also be used to concatenate stacks by setting start to the size of stack one.");

    ui->instructions_label->setAlignment(Qt::AlignJustify);
    ui->instructions_label->setWordWrap(true);
    ui->instructions_label->setText("Select output options here. You can define the type of environment you would like to generate either using the selection box below, or by choosing one of the tabs to the right, in which you can find the settings for each environment type. Press play to start generating.");

    //RJG - And combo box
    ui->environment_comboBox->addItem("Dynamic One");
    ui->environment_comboBox->addItem("Dynamic Two");
    ui->environment_comboBox->addItem("Noise");
    ui->environment_comboBox->addItem("Combine stacks");
    ui->environment_comboBox->addItem("Colour");
    ui->environment_comboBox->addItem("Stack from image");

    //RJG - Sort GUI
    ui->toolBar->setIconSize(QSize(25, 25));
    ui->statusBar->setMinimumHeight(25);
    ui->settings_tab_widget->setCurrentIndex(0);
    ui->pushButtonStackTwo->setEnabled(false);

    startButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_button_green.png")), QString("Run"), this);
    startButton->setToolTip(tr("<font>Use this button to generate an environment.</font>"));
    ui->toolBar->addAction(startButton);
    QObject::connect(startButton, SIGNAL(triggered()), this, SLOT(runPressed()));

    runForBatchButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_play_batch_button_green.png")), QString("Run batch"), this);
    runForBatchButton->setToolTip(tr("<font>Use this button to generate multiple replicates of an environment.</font>"));
    ui->toolBar->addAction(runForBatchButton);
    QObject::connect(runForBatchButton, SIGNAL (triggered()), this, SLOT (runBatchPressed()));

    stopButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_stop_button_red.png")), QString("Stop"), this);
    stopButton->setToolTip(tr("<font>Use this button stop the environmental generation.</font>"));
    ui->toolBar->addAction(stopButton);
    QObject::connect(stopButton, SIGNAL (triggered()), this, SLOT (stop()));
    stopButton->setEnabled(false);

    pauseButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_pause_button_orange.png")), QString("Pause"), this);
    pauseButton->setToolTip(tr("<font>Use this button to pause the environmental generation. Press again to unpause.</font>"));
    ui->toolBar->addAction(pauseButton);
    QObject::connect(pauseButton, SIGNAL (triggered()), this, SLOT (pause()));
    pauseButton->setEnabled(false);

    ui->toolBar->addSeparator();
    settingsButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_settings_1_button.png")), QString("Settings"), this);
    settingsButton->setToolTip(tr("<font>Use this button to open the Settings Dock.</font>"));
    ui->toolBar->addAction(settingsButton);
    QObject::connect(settingsButton, SIGNAL (triggered()), this, SLOT (settings()));
    settingsButton->setCheckable(true);
    settingsButton->setChecked(true);

    ui->toolBar->addSeparator();
    aboutButton = new QAction(QIcon(QPixmap(":/darkstyle/icon_about_button.png")), QString("About"), this);
    aboutButton->setToolTip(tr("<font>Use this button to view information about this program.</font>"));
    ui->toolBar->addAction(aboutButton);
    QObject::connect(aboutButton, SIGNAL (triggered()), this, SLOT (about()));

    //RJG - Other signal/slot connections
    QObject::connect(ui->change_path, SIGNAL (clicked()), this, SLOT (changePath()));
    QObject::connect(ui->settings_tab_widget, SIGNAL (currentChanged(int)), this, SLOT (tab_changed(int)));
    //RJG - these ones are pretty simple. Use lamdas.
    connect(ui->noiseMin, (void(QSpinBox::*)(int))&QSpinBox::valueChanged, this, [ = ](const int &i)
    {
        ui->noiseMax->setMinimum(i + 1);
    });
    connect(ui->noiseMax, (void(QSpinBox::*)(int))&QSpinBox::valueChanged, this, [ = ](const int &i)
    {
        ui->noiseMin->setMaximum(i - 1);
    });
    ui->noiseMin->setMaximum(ui->noiseMax->value() - 1);
    ui->noiseMax->setMinimum(ui->noiseMin->value() + 1);

    saveImages = true;
    ui->save_images_checkbox->setChecked(saveImages);
    connect(ui->save_images_checkbox, &QAbstractButton::toggled, this, [ = ](const bool & i)
    {
        saveImages = i;
    });

    //RJG - shortcuts
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this, SLOT(runPressed()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), this, SLOT(pause()));
    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(stop()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


//Add slot to update combo box to be the last selected tab

void MainWindow::tab_changed(int index)
{
    if (index)ui->environment_comboBox->setCurrentIndex(index - 1);
}

void MainWindow::runPressed()
{
    QString path = setupSaveDirectory(runs);
    if (path.length() < 2) return;
    currentEnvironmentSettings = new EnvironmentSettings(this);
    generateEnvironment(ui->environment_comboBox->currentIndex(), path, ui->spinSize->value(), ui->spinSize->value(), *currentEnvironmentSettings);
    delete currentEnvironmentSettings;
    runs++;
}

void MainWindow::runBatchPressed()
{

    if (ui->environment_comboBox->currentIndex() > 2)
    {
        QMessageBox::warning(this, "Sorry!", "Batch runs are not currently available for your chosen environment type. Please request the developers add it.");
        return;
    }

    bool ok;
    int runBatchFor = QInputDialog::getInt(this, "Batch...", "How many runs?", 25, 1, 999, 1, &ok);
    if (!ok) return;

    QVector<int> runsList(runBatchFor);
    std::iota(runsList.begin(), runsList.end(), 0);

    auto count = 0;
    bool batchRunning = true;

    currentEnvironmentSettings = new EnvironmentSettings(this);
    do
    {
        //In a previous version RJG had used the progress bar in the status bar, but connecting the future watcher signals and slots provide very challenging
        //Hence now do this with a dialogue - first create a dialogue, then QFutureWatcher and connect signals and slots.
        QProgressDialog dialog;
        dialog.setLabelText(QString("Starting a batch run of %1 simulations on %2 cores.").arg(runsList.length()).arg(QThread::idealThreadCount()));
        QFutureWatcher<void> futureWatcher;
        QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dialog, &QProgressDialog::reset);
        QObject::connect(&dialog, &QProgressDialog::canceled, &futureWatcher, &QFutureWatcher<void>::cancel);
        QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressRangeChanged, &dialog, &QProgressDialog::setRange);
        QObject::connect(&futureWatcher, &QFutureWatcher<void>::progressValueChanged,  &dialog, &QProgressDialog::setValue);

        //Do the runs using QtConcurrent::filter which modified the sequence in place
        futureWatcher.setFuture(QtConcurrent::filter(runsList, [this](const int &run)
        {
            QString path = setupSaveDirectory(run);
            if (path.length() < 2) return false;
            EnvironmentSettings localEnvironmentSettings = *currentEnvironmentSettings;
            return generateEnvironment(ui->environment_comboBox->currentIndex(), path, ui->spinSize->value(), ui->spinSize->value(), localEnvironmentSettings, true);
        }));

        // Display the dialog and start the event loop.
        dialog.exec();

        if (futureWatcher.isCanceled()) batchRunning = false;
        count ++;
    }
    //Run up to 1 times so this cannot get caught in an infinite loop
    while (runsList.count() > 0 && count < 1 && batchRunning);
    delete currentEnvironmentSettings;

    /*  for (runs = 0; runs < runBatchFor; runs++)
      {
          QString path = setupSaveDirectory(runs);
          if (path.length() < 2) return;
          generateEnvironment(ui->environment_comboBox->currentIndex(), path, ui->spinSize->value(), ui->spinSize->value(), true);
      }
      */
}

QString MainWindow::setupSaveDirectory(int runsLocal)
{

    QDir saveDirectory(ui->path->toPlainText());
    if (!saveDirectory.exists() && ui->save_images_checkbox->isChecked())
    {
        QMessageBox::warning(this, "Error!", "The program doesn't think the save directory exists, so is going to default back to the direcctory in which the executable is.");
        QString programPath(QCoreApplication::applicationDirPath());
        programPath.append(QDir::separator());
        ui->path->setText(programPath);
        saveDirectory.setPath(programPath);
    }

    //RJG - Set up save directory

    QString folderString(QString(PRODUCTNAME) + "_output_" + QString("%1").arg(runsLocal, 3, 10, QChar('0')) + QDir::separator());
    if (!saveDirectory.mkpath(folderString))
    {
        QMessageBox::warning(this, "Error", "Cant save images. Permissions issue?");
        return "";
    }
    else saveDirectory.cd(folderString);

    return saveDirectory.path();
}

//RJG - Generates environment based on which tab is selected in the tab dock widget.
bool MainWindow::generateEnvironment(int environmentType, QString path, int x, int y, EnvironmentSettings localEnvironmentSettings, bool batch)
{
    //RJG - new environment object
    EnvironmentClass *environmentObject = nullptr;

    //RJG - Select which kind of environment object to create - all inheret environmentclass, which has the random number and save functions in it
    switch (environmentType)
    {
    case 0:
        environmentObject = new russellenvironment(localEnvironmentSettings); // Russell environment
        break;
    case 1:
        environmentObject = new markenvironment(localEnvironmentSettings); //Mark environment
        break;
    case 2: //Noise stack
        environmentObject = new noiseenvironment(localEnvironmentSettings);
        if (MainWin->ui->noiseMin->value() >= MainWin->ui->noiseMax->value())
        {
            QMessageBox::warning(this, "Error", "Min is greater than Max - please change this before proceeding.", QMessageBox::Ok);
            reset(environmentObject);
            return false;
        }
        break;
    case 3: //Combine stacks
        environmentObject = new combine(localEnvironmentSettings);
        if (environmentObject->error)
        {
            reset(environmentObject);
            return false;
        }
        generations = MainWin->ui->combineStart->value() + stackTwoSize;
        break;
    case 4:
        environmentObject = new colour(localEnvironmentSettings); //Colour stacks
        break;
    case 5:   //Create stack from image
        environmentObject = new makestack (localEnvironmentSettings);
        if (environmentObject->error)
        {
            reset(environmentObject);
            return false;
        }
        break;
    default:
        QMessageBox::warning(this, "Error", "Environment number not recognised.", QMessageBox::Ok);
        return false;
    }

    //RJG - Sort generations (required for combine)
    generations = MainWin->ui->numGenerations->value();
    int store_generations = generations;

    //RJG - Add a progress bar
    QProgressBar prBar;

    //RJG - Need to sort GUI if we are not doing a batch
    if (!batch)
    {
        //RJG - Set up new environment image
        if (!batch)newEnvironmentImage();

        //RJG - set up GUI
        if (!batch) setGUIButtons();

        //RJG - sort progress bar
        prBar.setRange(0, generations);
        prBar.setAlignment(Qt::AlignCenter);
        ui->statusBar->addPermanentWidget(&prBar);
    }

    //RJG - Generate the environment
    for (int i = 0; i < generations; i++)
    {
        iterations = i;
        environmentObject->regenerate();

        //RJG - Update GUI
        if (!batch)
        {
            prBar.setValue(i);
            refreshEnvironment(environmentObject);
            qApp->processEvents();
        }

        if (saveImages)
        {
            QImage saveImage(x, y, QImage::Format_RGB32);
            for (int n = 0; n < x; n++)
                for (int m = 0; m < y; m++)
                    saveImage.setPixel(n, m, qRgb(environmentObject->environment[n][m][0], environmentObject->environment[n][m][1], environmentObject->environment[n][m][2]));
            QString savePath = QString(path + QDir::separator() + "%1.png").arg(i, 4, 10, QChar('0'));
            saveImage.save(savePath);
        }

        //RJG - Deal with pause
        if (pauseFlag)
            while (pauseFlag && !stopFlag)
                qApp->processEvents();
        //RJG - and stop.
        if (stopFlag)
        {
            stopFlag = false;
            generations = -1;
            break;
        }

    }

    //RJG - Reset GUI and inform user.
    if (!batch)
    {
        ui->statusBar->removeWidget(&prBar);
        if (generations < 0)ui->statusBar->showMessage("Generation cancelled.");
        else if (ui->save_images_checkbox->isChecked())ui->statusBar->showMessage(QString("Generation complete: %1 images were saved.").arg(generations));
        else ui->statusBar->showMessage("Generation complete; no images saved as save is not selected in the out tab.");
        reset(environmentObject);
    }

    generations = store_generations;

    return true;
}

void MainWindow::newEnvironmentImage()
{
    //RJG - Add images to the scenes
    if (!env_image->isNull())delete env_image;
    env_image = new QImage(MainWin->ui->spinSize->value(), MainWin->ui->spinSize->value(), QImage::Format_RGB32);
    env_image->fill(0);
    env_item->setPixmap(QPixmap::fromImage(*env_image));
    //RJG - flag secene for deletion later, and then create a new one
    //If you don't do this the scene is too large, and doesn't center correctly.
    environmentScene->deleteLater();
    environmentScene = new EnvironmentScene;
    environmentScene->addItem(env_item);
    ui->enviroView->setScene(environmentScene);
}


void MainWindow::refreshEnvironment(EnvironmentClass *environmentObject)
{
    //RJG - Update the environment display (which is then used to save image)
    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
            env_image->setPixel(n, m, qRgb(environmentObject->environment[n][m][0], environmentObject->environment[n][m][1], environmentObject->environment[n][m][2]));

    env_item->setPixmap(QPixmap::fromImage(*env_image));

    ui->enviroView->resetTransform();
    if (ui->resize->isChecked()) ui->enviroView->fitInView(env_item, Qt::KeepAspectRatio);
    ui->enviroView->centerOn(0., 0.);
}

void MainWindow::setGUIButtons()
{
    //RJG - Sort out GUI and pause/stop
    stopFlag = false;
    startButton->setEnabled(false);
    pauseButton->setEnabled(true);
    stopButton->setEnabled(true);
    ui->output_tab->setEnabled(false);
}

void MainWindow::resetGUIButtons()
{
    //RJG - Sort buttons and GUI
    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);
    ui->output_tab->setEnabled(true);
}

void MainWindow::reset(EnvironmentClass *environmentObject)
{
    //RJG - Add logo to the scene
    if (!env_image->isNull())delete env_image;
    env_image = new QImage(":/palaeoware_logo_square.png");
    env_item->setPixmap(QPixmap::fromImage(*env_image));

    environmentScene->deleteLater();

    environmentScene = new EnvironmentScene;
    environmentScene->mw = this;
    environmentScene->addItem(env_item);
    ui->enviroView->resetTransform();
    ui->enviroView->setScene(environmentScene);

    //RJG - Sort flags
    pauseFlag = false;
    stopFlag = false;

    //RJG - sort memory and sort GUI
    if (environmentObject != nullptr)
    {
        delete environmentObject;
        resetGUIButtons();
    }
}

//RJG - Load stack one
void MainWindow::on_pushButtonStackOne_clicked()
{
    QString files_directory = QFileDialog::getExistingDirectory(this, tr("Select folder containing image files"),
                                                                "d:/", QFileDialog::ShowDirsOnly);

    QDir current = files_directory;
    QStringList filterList, dirList;
    filterList << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";
    dirList = current.entryList(filterList, QDir::Files, QDir::Name);
    if (dirList.count() == 0)
    {
        QMessageBox::warning(nullptr, "Error", "No image files in this folder.", QMessageBox::Ok);
        return;
    }
    else ui->stackOneText->setText(files_directory);
    stackOneSize = dirList.count();
    ui->combineStart->setMaximum(stackOneSize);

    ui->pushButtonStackTwo->setEnabled(true);
}

//RJG - Load stack two
void MainWindow::on_pushButtonStackTwo_clicked()
{
    QString files_directory = QFileDialog::getExistingDirectory(this, tr("Select folder containing image files"),
                                                                "d:/", QFileDialog::ShowDirsOnly);

    QDir current = files_directory;
    QStringList filterList, dirList;
    filterList << "*.bmp" << "*.jpg" << "*.jpeg" << "*.png";
    dirList = current.entryList(filterList, QDir::Files, QDir::Name);
    if (dirList.count() == 0)
    {
        QMessageBox::warning(nullptr, "Error", "No image files in this folder.", QMessageBox::Ok);
        return;
    }
    else ui->stackTwoText->setText(files_directory);

    stackTwoSize = dirList.count();
}

//RJG - select colour
void MainWindow::on_selectColour_clicked()
{
    QColor mainWinColour(ui->spinRed->value(), ui->spinGreen->value(), ui->spinBlue->value());
    QColorDialog chooseColour;
    mainWinColour = chooseColour.getColor(mainWinColour);
    if (mainWinColour.isValid())
    {
        ui->spinRed->setValue(mainWinColour.red());
        ui->spinGreen->setValue(mainWinColour.green());
        ui->spinBlue->setValue(mainWinColour.blue());
    }
}

//RJG - Load image to create stack from
void MainWindow::on_sFromImPushbutton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Select image file"),
                                                     "d:/",
                                                     tr("Images (*.png *.bmp *.jpg)"));
    if (!file_name.isNull())
        ui->sFromImTxt->setText(file_name);

    if (file_name.length() < 5)return;
    QImage original;
    original.load(file_name);

    ui->spinSize->setValue(original.width());
}

//RJG - Launch about dialogue
void MainWindow::about()
{
    About adialogue;
    adialogue.exec();
}

//RJG - Stop and pause rely on flags, which are toggled here
void MainWindow::stop()
{
    stopFlag = true;
}

void MainWindow::pause()
{
    pauseFlag = !pauseFlag;
}

//RJG - Show and hide settings docker
void MainWindow::settings()
{
    if (ui->settings_tab_widget->isVisible())
    {
        ui->envSettings->hide();
        settingsButton->setChecked(false);
    }
    else
    {
        ui->envSettings->show();
        settingsButton->setChecked(true);
    }
}

//RJG -Change save path
void MainWindow::changePath()
{
    QString filesDirectory = QFileDialog::getExistingDirectory(this, tr("Select folder in which you would like to save image files"),
                                                               QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), QFileDialog::ShowDirsOnly);
    ui->path->setText(filesDirectory);
}
