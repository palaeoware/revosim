/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionReset;
    QAction *actionStart_Sim;
    QAction *actionStop_Sim;
    QAction *actionRun_for;
    QAction *actionSave;
    QAction *actionLoad;
    QAction *actionGenomeComparison;
    QAction *actionGo_Slow;
    QAction *actionLoad_Random_Numbers;
    QAction *actionFitness_logging_to_File;
    QAction *actionReseed;
    QAction *actionBatch;
    QAction *actionRecombination_logging;
    QAction *actionCount_peaks;
    QAction *actionSave_settings;
    QAction *actionLoad_settings;
    QAction *actionPause_Sim;
    QAction *actionAbout;
    QAction *actionExit;
    QAction *actionCode_on_GitHub;
    QAction *actionOnline_User_Manual;
    QAction *actionSettings_Dock;
    QAction *actionReportBugIssueFeatureRequest;
    QAction *actionVariable_mutation_logging;
    QAction *actionSpeciation_logging;
    QAction *actionDisparity_logging;
    QAction *actionOutput_Dock;
    QAction *actionUseNewSpeciesID;
    QAction *actionGatherBySpecies;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_4;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QLabel *label_7;
    QComboBox *populationWindowComboBox;
    QSpacerItem *horizontalSpacer_2;
    QGraphicsView *GV_Population;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame_4;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_2;
    QComboBox *environmentWindowComboBox;
    QSpacerItem *horizontalSpacer_4;
    QGraphicsView *GV_Environment;
    QTextEdit *testLog;
    QFrame *frame;
    QVBoxLayout *verticalLayout_5;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    QLabel *batchLabel;
    QLabel *LabelBatch;
    QSpacerItem *horizontalSpacer_5;
    QLabel *iterationLabel;
    QLabel *LabelIteration;
    QSpacerItem *horizontalSpacer_6;
    QLabel *iterationsperhourLabel;
    QLabel *LabelIterationsPerHour;
    QSpacerItem *horizontalSpacer_7;
    QLabel *crittersLabel;
    QLabel *LabelCritters;
    QSpacerItem *horizontalSpacer_8;
    QLabel *msPerIterationLabel;
    QLabel *LabelSpeed;
    QSpacerItem *horizontalSpacer_9;
    QLabel *meanFitLabel;
    QLabel *LabelFitness;
    QSpacerItem *horizontalSpacer_10;
    QLabel *speciesLabel;
    QLabel *LabelSpecies;
    QSpacerItem *horizontalSpacer_11;
    QLabel *environmentLabel;
    QLabel *LabelEnvironment;
    QStatusBar *statusBar;
    QDockWidget *genomeComparisonDock;
    QWidget *genomeComparisonContent;
    QToolBar *toolBar;
    QMenuBar *menuBar;
    QMenu *menuCommands;
    QMenu *menuTools;
    QMenu *menuCustom_Logging;
    QMenu *menuHelp;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1292, 843);
        MainWindow->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        MainWindow->setDocumentMode(false);
        actionReset = new QAction(MainWindow);
        actionReset->setObjectName("actionReset");
        actionStart_Sim = new QAction(MainWindow);
        actionStart_Sim->setObjectName("actionStart_Sim");
        actionStop_Sim = new QAction(MainWindow);
        actionStop_Sim->setObjectName("actionStop_Sim");
        actionRun_for = new QAction(MainWindow);
        actionRun_for->setObjectName("actionRun_for");
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName("actionSave");
        actionSave->setEnabled(false);
        actionLoad = new QAction(MainWindow);
        actionLoad->setObjectName("actionLoad");
        actionLoad->setEnabled(false);
        actionGenomeComparison = new QAction(MainWindow);
        actionGenomeComparison->setObjectName("actionGenomeComparison");
        actionGenomeComparison->setCheckable(true);
        actionGenomeComparison->setChecked(false);
        actionGo_Slow = new QAction(MainWindow);
        actionGo_Slow->setObjectName("actionGo_Slow");
        actionGo_Slow->setCheckable(true);
        actionLoad_Random_Numbers = new QAction(MainWindow);
        actionLoad_Random_Numbers->setObjectName("actionLoad_Random_Numbers");
        actionFitness_logging_to_File = new QAction(MainWindow);
        actionFitness_logging_to_File->setObjectName("actionFitness_logging_to_File");
        actionFitness_logging_to_File->setCheckable(true);
        actionFitness_logging_to_File->setEnabled(true);
        actionReseed = new QAction(MainWindow);
        actionReseed->setObjectName("actionReseed");
        actionReseed->setCheckable(false);
        actionBatch = new QAction(MainWindow);
        actionBatch->setObjectName("actionBatch");
        actionRecombination_logging = new QAction(MainWindow);
        actionRecombination_logging->setObjectName("actionRecombination_logging");
        actionRecombination_logging->setCheckable(true);
        actionCount_peaks = new QAction(MainWindow);
        actionCount_peaks->setObjectName("actionCount_peaks");
        actionSave_settings = new QAction(MainWindow);
        actionSave_settings->setObjectName("actionSave_settings");
        actionLoad_settings = new QAction(MainWindow);
        actionLoad_settings->setObjectName("actionLoad_settings");
        actionPause_Sim = new QAction(MainWindow);
        actionPause_Sim->setObjectName("actionPause_Sim");
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName("actionExit");
        actionCode_on_GitHub = new QAction(MainWindow);
        actionCode_on_GitHub->setObjectName("actionCode_on_GitHub");
        actionOnline_User_Manual = new QAction(MainWindow);
        actionOnline_User_Manual->setObjectName("actionOnline_User_Manual");
        actionSettings_Dock = new QAction(MainWindow);
        actionSettings_Dock->setObjectName("actionSettings_Dock");
        actionSettings_Dock->setCheckable(true);
        actionReportBugIssueFeatureRequest = new QAction(MainWindow);
        actionReportBugIssueFeatureRequest->setObjectName("actionReportBugIssueFeatureRequest");
        actionVariable_mutation_logging = new QAction(MainWindow);
        actionVariable_mutation_logging->setObjectName("actionVariable_mutation_logging");
        actionVariable_mutation_logging->setCheckable(true);
        actionSpeciation_logging = new QAction(MainWindow);
        actionSpeciation_logging->setObjectName("actionSpeciation_logging");
        actionSpeciation_logging->setCheckable(true);
        actionDisparity_logging = new QAction(MainWindow);
        actionDisparity_logging->setObjectName("actionDisparity_logging");
        actionDisparity_logging->setCheckable(true);
        actionOutput_Dock = new QAction(MainWindow);
        actionOutput_Dock->setObjectName("actionOutput_Dock");
        actionOutput_Dock->setCheckable(true);
        actionUseNewSpeciesID = new QAction(MainWindow);
        actionUseNewSpeciesID->setObjectName("actionUseNewSpeciesID");
        actionUseNewSpeciesID->setCheckable(true);
        actionUseNewSpeciesID->setChecked(true);
        actionGatherBySpecies = new QAction(MainWindow);
        actionGatherBySpecies->setObjectName("actionGatherBySpecies");
        actionGatherBySpecies->setCheckable(true);
        actionGatherBySpecies->setChecked(true);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        verticalLayout_4 = new QVBoxLayout(centralWidget);
        verticalLayout_4->setSpacing(3);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        frame_2 = new QFrame(centralWidget);
        frame_2->setObjectName("frame_2");
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setSpacing(4);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName("verticalLayout");
        frame_3 = new QFrame(frame_2);
        frame_3->setObjectName("frame_3");
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(25);
        sizePolicy1.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy1);
        frame_3->setMinimumSize(QSize(0, 25));
        frame_3->setMaximumSize(QSize(16777215, 25));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame_3);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        label_7 = new QLabel(frame_3);
        label_7->setObjectName("label_7");
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy2);

        horizontalLayout_4->addWidget(label_7);

        populationWindowComboBox = new QComboBox(frame_3);
        populationWindowComboBox->setObjectName("populationWindowComboBox");
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(populationWindowComboBox->sizePolicy().hasHeightForWidth());
        populationWindowComboBox->setSizePolicy(sizePolicy3);

        horizontalLayout_4->addWidget(populationWindowComboBox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(frame_3);

        GV_Population = new QGraphicsView(frame_2);
        GV_Population->setObjectName("GV_Population");
        sizePolicy3.setHeightForWidth(GV_Population->sizePolicy().hasHeightForWidth());
        GV_Population->setSizePolicy(sizePolicy3);
        GV_Population->setMinimumSize(QSize(500, 350));
        GV_Population->setAutoFillBackground(false);
        GV_Population->setFrameShape(QFrame::Box);
        GV_Population->setFrameShadow(QFrame::Sunken);
        GV_Population->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        QBrush brush(QColor(13, 13, 13, 255));
        brush.setStyle(Qt::NoBrush);
        GV_Population->setBackgroundBrush(brush);

        verticalLayout->addWidget(GV_Population);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(3);
        verticalLayout_2->setObjectName("verticalLayout_2");
        frame_4 = new QFrame(frame_2);
        frame_4->setObjectName("frame_4");
        sizePolicy1.setHeightForWidth(frame_4->sizePolicy().hasHeightForWidth());
        frame_4->setSizePolicy(sizePolicy1);
        frame_4->setMinimumSize(QSize(0, 25));
        frame_4->setMaximumSize(QSize(16777215, 25));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame_4);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        label_2 = new QLabel(frame_4);
        label_2->setObjectName("label_2");
        label_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_2);

        environmentWindowComboBox = new QComboBox(frame_4);
        environmentWindowComboBox->setObjectName("environmentWindowComboBox");
        sizePolicy3.setHeightForWidth(environmentWindowComboBox->sizePolicy().hasHeightForWidth());
        environmentWindowComboBox->setSizePolicy(sizePolicy3);

        horizontalLayout_2->addWidget(environmentWindowComboBox);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout_2->addWidget(frame_4);

        GV_Environment = new QGraphicsView(frame_2);
        GV_Environment->setObjectName("GV_Environment");
        sizePolicy3.setHeightForWidth(GV_Environment->sizePolicy().hasHeightForWidth());
        GV_Environment->setSizePolicy(sizePolicy3);
        GV_Environment->setMinimumSize(QSize(500, 350));
        GV_Environment->setFrameShape(QFrame::Box);
        GV_Environment->setFrameShadow(QFrame::Sunken);

        verticalLayout_2->addWidget(GV_Environment);


        horizontalLayout->addLayout(verticalLayout_2);


        verticalLayout_4->addWidget(frame_2);

        testLog = new QTextEdit(centralWidget);
        testLog->setObjectName("testLog");
        sizePolicy3.setHeightForWidth(testLog->sizePolicy().hasHeightForWidth());
        testLog->setSizePolicy(sizePolicy3);
        testLog->setTextInteractionFlags(Qt::TextSelectableByMouse);

        verticalLayout_4->addWidget(testLog);

        frame = new QFrame(centralWidget);
        frame->setObjectName("frame");
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy4);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_5 = new QVBoxLayout(frame);
        verticalLayout_5->setSpacing(3);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(frame);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1288, 49));
        horizontalLayout_5 = new QHBoxLayout(scrollAreaWidgetContents);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setSizeConstraint(QLayout::SetMinAndMaxSize);
        batchLabel = new QLabel(scrollAreaWidgetContents);
        batchLabel->setObjectName("batchLabel");
        QSizePolicy sizePolicy5(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(batchLabel->sizePolicy().hasHeightForWidth());
        batchLabel->setSizePolicy(sizePolicy5);

        horizontalLayout_3->addWidget(batchLabel);

        LabelBatch = new QLabel(scrollAreaWidgetContents);
        LabelBatch->setObjectName("LabelBatch");
        sizePolicy5.setHeightForWidth(LabelBatch->sizePolicy().hasHeightForWidth());
        LabelBatch->setSizePolicy(sizePolicy5);

        horizontalLayout_3->addWidget(LabelBatch);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_5);

        iterationLabel = new QLabel(scrollAreaWidgetContents);
        iterationLabel->setObjectName("iterationLabel");
        sizePolicy5.setHeightForWidth(iterationLabel->sizePolicy().hasHeightForWidth());
        iterationLabel->setSizePolicy(sizePolicy5);
        iterationLabel->setTextFormat(Qt::AutoText);
        iterationLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(iterationLabel);

        LabelIteration = new QLabel(scrollAreaWidgetContents);
        LabelIteration->setObjectName("LabelIteration");
        sizePolicy5.setHeightForWidth(LabelIteration->sizePolicy().hasHeightForWidth());
        LabelIteration->setSizePolicy(sizePolicy5);

        horizontalLayout_3->addWidget(LabelIteration);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_6);

        iterationsperhourLabel = new QLabel(scrollAreaWidgetContents);
        iterationsperhourLabel->setObjectName("iterationsperhourLabel");
        sizePolicy5.setHeightForWidth(iterationsperhourLabel->sizePolicy().hasHeightForWidth());
        iterationsperhourLabel->setSizePolicy(sizePolicy5);
        iterationsperhourLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(iterationsperhourLabel);

        LabelIterationsPerHour = new QLabel(scrollAreaWidgetContents);
        LabelIterationsPerHour->setObjectName("LabelIterationsPerHour");
        sizePolicy5.setHeightForWidth(LabelIterationsPerHour->sizePolicy().hasHeightForWidth());
        LabelIterationsPerHour->setSizePolicy(sizePolicy5);

        horizontalLayout_3->addWidget(LabelIterationsPerHour);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_7);

        crittersLabel = new QLabel(scrollAreaWidgetContents);
        crittersLabel->setObjectName("crittersLabel");
        sizePolicy5.setHeightForWidth(crittersLabel->sizePolicy().hasHeightForWidth());
        crittersLabel->setSizePolicy(sizePolicy5);
        crittersLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(crittersLabel);

        LabelCritters = new QLabel(scrollAreaWidgetContents);
        LabelCritters->setObjectName("LabelCritters");
        sizePolicy5.setHeightForWidth(LabelCritters->sizePolicy().hasHeightForWidth());
        LabelCritters->setSizePolicy(sizePolicy5);

        horizontalLayout_3->addWidget(LabelCritters);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_8);

        msPerIterationLabel = new QLabel(scrollAreaWidgetContents);
        msPerIterationLabel->setObjectName("msPerIterationLabel");
        sizePolicy5.setHeightForWidth(msPerIterationLabel->sizePolicy().hasHeightForWidth());
        msPerIterationLabel->setSizePolicy(sizePolicy5);
        msPerIterationLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(msPerIterationLabel);

        LabelSpeed = new QLabel(scrollAreaWidgetContents);
        LabelSpeed->setObjectName("LabelSpeed");
        sizePolicy5.setHeightForWidth(LabelSpeed->sizePolicy().hasHeightForWidth());
        LabelSpeed->setSizePolicy(sizePolicy5);

        horizontalLayout_3->addWidget(LabelSpeed);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_9);

        meanFitLabel = new QLabel(scrollAreaWidgetContents);
        meanFitLabel->setObjectName("meanFitLabel");
        sizePolicy5.setHeightForWidth(meanFitLabel->sizePolicy().hasHeightForWidth());
        meanFitLabel->setSizePolicy(sizePolicy5);
        meanFitLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(meanFitLabel);

        LabelFitness = new QLabel(scrollAreaWidgetContents);
        LabelFitness->setObjectName("LabelFitness");
        sizePolicy5.setHeightForWidth(LabelFitness->sizePolicy().hasHeightForWidth());
        LabelFitness->setSizePolicy(sizePolicy5);

        horizontalLayout_3->addWidget(LabelFitness);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_10);

        speciesLabel = new QLabel(scrollAreaWidgetContents);
        speciesLabel->setObjectName("speciesLabel");
        sizePolicy5.setHeightForWidth(speciesLabel->sizePolicy().hasHeightForWidth());
        speciesLabel->setSizePolicy(sizePolicy5);
        speciesLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(speciesLabel);

        LabelSpecies = new QLabel(scrollAreaWidgetContents);
        LabelSpecies->setObjectName("LabelSpecies");
        sizePolicy5.setHeightForWidth(LabelSpecies->sizePolicy().hasHeightForWidth());
        LabelSpecies->setSizePolicy(sizePolicy5);

        horizontalLayout_3->addWidget(LabelSpecies);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_11);

        environmentLabel = new QLabel(scrollAreaWidgetContents);
        environmentLabel->setObjectName("environmentLabel");
        sizePolicy5.setHeightForWidth(environmentLabel->sizePolicy().hasHeightForWidth());
        environmentLabel->setSizePolicy(sizePolicy5);
        environmentLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(environmentLabel);

        LabelEnvironment = new QLabel(scrollAreaWidgetContents);
        LabelEnvironment->setObjectName("LabelEnvironment");
        sizePolicy5.setHeightForWidth(LabelEnvironment->sizePolicy().hasHeightForWidth());
        LabelEnvironment->setSizePolicy(sizePolicy5);
        LabelEnvironment->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(LabelEnvironment);


        horizontalLayout_5->addLayout(horizontalLayout_3);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_5->addWidget(scrollArea);


        verticalLayout_4->addWidget(frame);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);
        genomeComparisonDock = new QDockWidget(MainWindow);
        genomeComparisonDock->setObjectName("genomeComparisonDock");
        genomeComparisonDock->setMinimumSize(QSize(250, 150));
        genomeComparisonDock->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
        genomeComparisonContent = new QWidget();
        genomeComparisonContent->setObjectName("genomeComparisonContent");
        genomeComparisonDock->setWidget(genomeComparisonContent);
        MainWindow->addDockWidget(Qt::BottomDockWidgetArea, genomeComparisonDock);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName("toolBar");
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1292, 17));
        menuCommands = new QMenu(menuBar);
        menuCommands->setObjectName("menuCommands");
        menuCommands->setMouseTracking(true);
        menuCommands->setFocusPolicy(Qt::NoFocus);
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName("menuTools");
        menuCustom_Logging = new QMenu(menuTools);
        menuCustom_Logging->setObjectName("menuCustom_Logging");
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuCommands->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuCommands->addAction(actionLoad_Random_Numbers);
        menuCommands->addAction(actionStart_Sim);
        menuCommands->addAction(actionRun_for);
        menuCommands->addAction(actionBatch);
        menuCommands->addAction(actionPause_Sim);
        menuCommands->addAction(actionStop_Sim);
        menuCommands->addAction(actionReset);
        menuCommands->addAction(actionReseed);
        menuCommands->addAction(actionGo_Slow);
        menuCommands->addSeparator();
        menuCommands->addAction(actionSave);
        menuCommands->addAction(actionLoad);
        menuCommands->addAction(actionSave_settings);
        menuCommands->addAction(actionLoad_settings);
        menuCommands->addSeparator();
        menuCommands->addAction(actionCount_peaks);
        menuCommands->addSeparator();
        menuCommands->addAction(actionExit);
        menuTools->addAction(actionGenomeComparison);
        menuTools->addAction(actionSettings_Dock);
        menuTools->addAction(actionOutput_Dock);
        menuTools->addAction(menuCustom_Logging->menuAction());
        menuCustom_Logging->addAction(actionFitness_logging_to_File);
        menuCustom_Logging->addAction(actionRecombination_logging);
        menuCustom_Logging->addAction(actionVariable_mutation_logging);
        menuCustom_Logging->addAction(actionSpeciation_logging);
        menuCustom_Logging->addAction(actionDisparity_logging);
        menuHelp->addAction(actionReportBugIssueFeatureRequest);
        menuHelp->addAction(actionCode_on_GitHub);
        menuHelp->addAction(actionOnline_User_Manual);
        menuHelp->addSeparator();
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);
        QObject::connect(actionGenomeComparison, &QAction::triggered, genomeComparisonDock, &QDockWidget::setVisible);

        populationWindowComboBox->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "REvoSim", nullptr));
        actionReset->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
#if QT_CONFIG(tooltip)
        actionReset->setToolTip(QCoreApplication::translate("MainWindow", "Reset", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionReset->setShortcut(QCoreApplication::translate("MainWindow", "Alt+X", nullptr));
#endif // QT_CONFIG(shortcut)
        actionStart_Sim->setText(QCoreApplication::translate("MainWindow", "Run", nullptr));
#if QT_CONFIG(tooltip)
        actionStart_Sim->setToolTip(QCoreApplication::translate("MainWindow", "Run", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionStart_Sim->setShortcut(QCoreApplication::translate("MainWindow", "Alt+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionStop_Sim->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
#if QT_CONFIG(tooltip)
        actionStop_Sim->setToolTip(QCoreApplication::translate("MainWindow", "Stop", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionStop_Sim->setShortcut(QCoreApplication::translate("MainWindow", "Alt+X", nullptr));
#endif // QT_CONFIG(shortcut)
        actionRun_for->setText(QCoreApplication::translate("MainWindow", "Run for...", nullptr));
#if QT_CONFIG(tooltip)
        actionRun_for->setToolTip(QCoreApplication::translate("MainWindow", "Run for", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionRun_for->setShortcut(QCoreApplication::translate("MainWindow", "Alt+R", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave->setText(QCoreApplication::translate("MainWindow", "Save...", nullptr));
#if QT_CONFIG(shortcut)
        actionSave->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionLoad->setText(QCoreApplication::translate("MainWindow", "Load...", nullptr));
#if QT_CONFIG(shortcut)
        actionLoad->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionGenomeComparison->setText(QCoreApplication::translate("MainWindow", "Genome Comparison Dock", nullptr));
        actionGo_Slow->setText(QCoreApplication::translate("MainWindow", "Go Slow", nullptr));
        actionLoad_Random_Numbers->setText(QCoreApplication::translate("MainWindow", "Load Random Numbers...", nullptr));
        actionFitness_logging_to_File->setText(QCoreApplication::translate("MainWindow", "Fitness logging", nullptr));
        actionReseed->setText(QCoreApplication::translate("MainWindow", "Reseed with known genome...", nullptr));
        actionBatch->setText(QCoreApplication::translate("MainWindow", "Batch", nullptr));
        actionRecombination_logging->setText(QCoreApplication::translate("MainWindow", "Recombination logging", nullptr));
        actionCount_peaks->setText(QCoreApplication::translate("MainWindow", "Count peaks...", nullptr));
#if QT_CONFIG(shortcut)
        actionCount_peaks->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSave_settings->setText(QCoreApplication::translate("MainWindow", "Save settings...", nullptr));
#if QT_CONFIG(shortcut)
        actionSave_settings->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionLoad_settings->setText(QCoreApplication::translate("MainWindow", "Load settings...", nullptr));
#if QT_CONFIG(shortcut)
        actionLoad_settings->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionPause_Sim->setText(QCoreApplication::translate("MainWindow", "Pause", nullptr));
#if QT_CONFIG(shortcut)
        actionPause_Sim->setShortcut(QCoreApplication::translate("MainWindow", "Alt+P", nullptr));
#endif // QT_CONFIG(shortcut)
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionCode_on_GitHub->setText(QCoreApplication::translate("MainWindow", "Code on GitHub", nullptr));
        actionOnline_User_Manual->setText(QCoreApplication::translate("MainWindow", "Online User Manual", nullptr));
        actionSettings_Dock->setText(QCoreApplication::translate("MainWindow", "Settings Dock", nullptr));
        actionReportBugIssueFeatureRequest->setText(QCoreApplication::translate("MainWindow", "Report Bug/Issue/Feature Request", nullptr));
        actionVariable_mutation_logging->setText(QCoreApplication::translate("MainWindow", "Variable mutation logging", nullptr));
        actionSpeciation_logging->setText(QCoreApplication::translate("MainWindow", "Speciation logging", nullptr));
        actionDisparity_logging->setText(QCoreApplication::translate("MainWindow", "Disparity logging", nullptr));
        actionOutput_Dock->setText(QCoreApplication::translate("MainWindow", "Output Dock", nullptr));
        actionUseNewSpeciesID->setText(QCoreApplication::translate("MainWindow", "UseNewSpeciesID", nullptr));
        actionGatherBySpecies->setText(QCoreApplication::translate("MainWindow", "GatherBySpecies", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "<b>Population:</b>", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-weight:600;\">Environment/Linkage:</span></p></body></html>", nullptr));
        batchLabel->setText(QCoreApplication::translate("MainWindow", "<b>Batch:</b>", nullptr));
        LabelBatch->setText(QCoreApplication::translate("MainWindow", "1/1", nullptr));
        iterationLabel->setText(QCoreApplication::translate("MainWindow", "<b>Iteration:</b>", nullptr));
        LabelIteration->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        iterationsperhourLabel->setText(QCoreApplication::translate("MainWindow", "<b>iterations/h:</b>", nullptr));
        LabelIterationsPerHour->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        crittersLabel->setText(QCoreApplication::translate("MainWindow", "<b>Organisms:</b>", nullptr));
        LabelCritters->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        msPerIterationLabel->setText(QCoreApplication::translate("MainWindow", "<b>ms/iteration:</b>", nullptr));
        LabelSpeed->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        meanFitLabel->setText(QCoreApplication::translate("MainWindow", "<b>Mean Fit:</b>", nullptr));
        LabelFitness->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        speciesLabel->setText(QCoreApplication::translate("MainWindow", "<b>Species:</b>", nullptr));
        LabelSpecies->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        environmentLabel->setText(QCoreApplication::translate("MainWindow", "<b>Environment:</b>", nullptr));
        LabelEnvironment->setText(QCoreApplication::translate("MainWindow", "[Not defined]", nullptr));
        genomeComparisonDock->setWindowTitle(QCoreApplication::translate("MainWindow", "Genome Comparison", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
        menuCommands->setTitle(QCoreApplication::translate("MainWindow", "Commands", nullptr));
        menuTools->setTitle(QCoreApplication::translate("MainWindow", "Tools", nullptr));
        menuCustom_Logging->setTitle(QCoreApplication::translate("MainWindow", "Custom Logging", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
