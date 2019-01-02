#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QProgressDialog>
#include <QProgressBar>
#include <QColorDialog>
#include <QStandardPaths>
#include <QShortcut>

MainWindow *MainWin;
randoms *simulation_randoms;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) + " - compiled - " + __DATE__);
    setWindowIcon(QIcon (":/icon.png"));
    showMaximized();

    MainWin = this;

    //RJG - Globals for simulation
    generations = 500;
    currentGeneration = 0;
    stackOneSize = 0;
    stackTwoSize = 0;
    stop_flag = false;
    pause_flag = false;
    Directory.setPath(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    ui->path->setText(Directory.path());

    //RJG - set up graphics
    env_image = new QImage(100, 100, QImage::Format_RGB32);
    env_image->fill(QColor(42, 42, 42));
    env_item = new QGraphicsPixmapItem(QPixmap::fromImage(*env_image));

    envscene = new EnvironmentScene;
    envscene->mw = this;
    envscene->addItem(env_item);

    ui->enviroView->setScene(envscene);

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
    ui->combine_label->setText("Combines two stacks between the start and end slice. If start slice is greater than zero, "
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
    QObject::connect(startButton, SIGNAL(triggered()), this, SLOT(generateEnvironment()));

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
    QObject::connect(ui->change_path, SIGNAL (clicked()), this, SLOT (change_path()));
    QObject::connect(ui->settings_tab_widget, SIGNAL (currentChanged(int)), this, SLOT (tab_changed(int)));
    //RJG - these ones are pretty simple. Use lamdas.
    connect(ui->noiseMin, (void(QSpinBox::*)(int))&QSpinBox::valueChanged, [ = ](const int &i) {
        ui->noiseMax->setMinimum(i + 1);
    });
    connect(ui->noiseMax, (void(QSpinBox::*)(int))&QSpinBox::valueChanged, [ = ](const int &i) {
        ui->noiseMin->setMaximum(i - 1);
    });
    ui->noiseMin->setMaximum(ui->noiseMax->value() - 1);
    ui->noiseMax->setMinimum(ui->noiseMin->value() + 1);

    //RJG - Load random numbers
    simulation_randoms = new randoms();

    //RJG - shortcuts
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_R), this, SLOT(generateEnvironment()));
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

//RJG - Generates environment based on which tab is selected in the tab dock widget.
void MainWindow::generateEnvironment()
{

    if (!Directory.exists() && ui->save_images_checkbox->isChecked()) {
        QMessageBox::warning(0, "Error", "No such directory.", QMessageBox::Ok);
        return;
    }

    //RJG - Select which kind of environment object to create - all inheret environmentclass, which has the random number and save functions in it
    if (ui->environment_comboBox->currentIndex() == 0) // Russell environment
        environmentobject = new russellenvironment;

    if (ui->environment_comboBox->currentIndex() == 1) //Mark environment
        environmentobject = new markenvironment;

    if (ui->environment_comboBox->currentIndex() == 2) { //Noise stack
        environmentobject = new noiseenvironment;
        if (MainWin->ui->noiseMin->value() >= MainWin->ui->noiseMax->value()) {
            QMessageBox::warning(0, "Error", "Min is greater than Max - please change this before proceeding.", QMessageBox::Ok);
            reset_gui();
            return;
        }
    }

    if (ui->environment_comboBox->currentIndex() == 3) { //Combine stacks
        environmentobject = new combine;
        if (environmentobject->error) {
            reset_gui();
            return;
        }
        generations = MainWin->ui->combineStart->value() + stackTwoSize;
    }

    if (ui->environment_comboBox->currentIndex() == 4) //Colour stacks
        environmentobject = new colour;

    if (ui->environment_comboBox->currentIndex() == 5) { //Create stack from image
        environmentobject = new makestack;
        if (environmentobject->error) {
            reset_gui();
            return;
        }
    }



    //RJG - Set up new environment image
    newEnvironmentImage();

    //RJG - Sort out GUI and pause/stop
    stop_flag = false;
    startButton->setEnabled(false);
    pauseButton->setEnabled(true);
    stopButton->setEnabled(true);
    ui->output_tab->setEnabled(false);

    //RJG - Sort generations (required for combine)
    generations = MainWin->ui->numGenerations->value();
    int store_generations = generations;

    //RJG - Add a progress bar
    QProgressBar prBar;
    prBar.setRange(0, generations);
    prBar.setAlignment(Qt::AlignCenter);
    ui->statusBar->addPermanentWidget(&prBar);

    //RJG - Set up save directory
    Directory.setPath(ui->path->toPlainText());
    if (!Directory.exists()) {
        QMessageBox::warning(0, "Error!", "The program doesn't think the save directory exists, so is going to default back to the direcctory in which the executable is.");
        QString program_path(QCoreApplication::applicationDirPath());
        program_path.append(QDir::separator());
        ui->path->setText(program_path);
        Directory.setPath(program_path);
    }
    if (!Directory.mkpath(QString(PRODUCTNAME) + "_output")) {
        QMessageBox::warning(this, "Error", "Cant save images. Permissions issue?");
        return;
    } else Directory.cd(QString(PRODUCTNAME) + "_output");

    //RJG - Generate the environment
    for (int i = 0; i < generations; i++) {

        currentGeneration = i;
        environmentobject->regenerate();

        prBar.setValue(i);
        RefreshEnvironment();

        qApp->processEvents();

        if (ui->save_images_checkbox->isChecked()) {
            QImage saveImage(MainWin->ui->spinSize->value(), MainWin->ui->spinSize->value(), QImage::Format_RGB32);
            for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
                for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
                    saveImage.setPixel(n, m, qRgb(environmentobject->environment[n][m][0], environmentobject->environment[n][m][1], environmentobject->environment[n][m][2]));
            QString save_directory = QString(Directory.path() + QDir::separator() + "%1.png").arg(i, 4, 10, QChar('0'));
            saveImage.save(save_directory);
        }

        //RJG - Deal with pause
        if (pause_flag)
            while (pause_flag && !stop_flag)
                qApp->processEvents();
        //RJG - and stop.
        if (stop_flag) {
            stop_flag = false;
            generations = -1;
            break;
        }

    }

    //RJG - Reet GUI and inform user.
    ui->statusBar->removeWidget(&prBar);
    if (generations < 0)ui->statusBar->showMessage("Generation cancelled.");
    else if (ui->save_images_checkbox->isChecked())ui->statusBar->showMessage(QString("Generation complete: %1 images were saved.").arg(generations));
    else ui->statusBar->showMessage("Generation complete; no images saved as save is not selected in the out tab.");

    reset_gui();
    generations = store_generations;

    //RJG - Free memory
    delete environmentobject;
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
    envscene->deleteLater();
    envscene = new EnvironmentScene;
    envscene->addItem(env_item);
    ui->enviroView->setScene(envscene);
}


void MainWindow::RefreshEnvironment()
{
    //RJG - Update the environment display (which is then used to save image)
    for (int n = 0; n < MainWin->ui->spinSize->value(); n++)
        for (int m = 0; m < MainWin->ui->spinSize->value(); m++)
            env_image->setPixel(n, m, qRgb(environmentobject->environment[n][m][0], environmentobject->environment[n][m][1], environmentobject->environment[n][m][2]));

    env_item->setPixmap(QPixmap::fromImage(*env_image));

    ui->enviroView->resetMatrix();
    if (ui->resize->isChecked()) ui->enviroView->fitInView(env_item, Qt::KeepAspectRatio);
    ui->enviroView->centerOn(0., 0.);
}


void MainWindow::reset_gui()
{

    //RJG - Add logo to the scene
    if (!env_image->isNull())delete env_image;
    env_image = new QImage(":/palaeoware_logo_square.png");
    env_item->setPixmap(QPixmap::fromImage(*env_image));

    envscene->deleteLater();

    envscene = new EnvironmentScene;
    envscene->mw = this;
    envscene->addItem(env_item);
    ui->enviroView->resetMatrix();
    ui->enviroView->setScene(envscene);

    //RJG - Sort flags
    pause_flag = false;
    stop_flag = false;

    //RJG - Sort buttons and GUI
    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);
    ui->output_tab->setEnabled(true);
}

//RJG -Change save path
void MainWindow::change_path()
{
    QString files_directory = QFileDialog::getExistingDirectory(this, tr("Select folder in which you would like to save image files"),
                                                                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), QFileDialog::ShowDirsOnly);
    if (files_directory == "") return;
    else Directory = files_directory;
    ui->path->setText(Directory.path());
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
    if (dirList.count() == 0) {
        QMessageBox::warning(0, "Error", "No image files in this folder.", QMessageBox::Ok);
        return;
    } else ui->stackOneText->setText(files_directory);
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
    if (dirList.count() == 0) {
        QMessageBox::warning(0, "Error", "No image files in this folder.", QMessageBox::Ok);
        return;
    } else ui->stackTwoText->setText(files_directory);

    stackTwoSize = dirList.count();
}

//RJG - select colour
void MainWindow::on_selectColour_clicked()
{
    QColor mainWinColour(ui->spinRed->value(), ui->spinGreen->value(), ui->spinBlue->value());
    QColorDialog chooseColour;
    mainWinColour = chooseColour.getColor(mainWinColour);
    if (mainWinColour.isValid()) {
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
    stop_flag = true;
}

void MainWindow::pause()
{
    pause_flag = !pause_flag;
}

//RJG - Show and hide settings docker
void MainWindow::settings()
{
    if (ui->settings_tab_widget->isVisible()) {
        ui->envSettings->hide();
        settingsButton->setChecked(false);
    } else {
        ui->envSettings->show();
        settingsButton->setChecked(true);
    }
}
