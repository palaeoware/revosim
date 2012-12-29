/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Sat 29. Dec 18:42:01 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDockWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSave_settings;
    QAction *actionResize_environment;
    QAction *actionRussell_Environment;
    QAction *actionMark_Environment;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QGraphicsView *enviroView;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *envSettings;
    QWidget *dockWidgetContents_3;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QLabel *label_10;
    QPushButton *generateEnvironment;
    QLabel *label_7;
    QCheckBox *checkBox_2;
    QTextBrowser *textBrowser_2;
    QPushButton *pushButton;
    QSpinBox *numGenerations;
    QLabel *label_5;
    QSpinBox *spinSize;
    QLabel *label_26;
    QTabWidget *tabWidget;
    QWidget *tab;
    QLabel *label_11;
    QLabel *label_9;
    QTextBrowser *textBrowser;
    QDoubleSpinBox *velMax2;
    QLabel *label_3;
    QDoubleSpinBox *factorSpin;
    QSpinBox *sMax;
    QCheckBox *periodic;
    QLabel *label_6;
    QLabel *label;
    QSpinBox *numSeed;
    QLabel *label_14;
    QLabel *label_12;
    QCheckBox *checkBox;
    QLabel *label_2;
    QLabel *label_13;
    QLabel *label_8;
    QDoubleSpinBox *convergeAt;
    QLabel *label_4;
    QSpinBox *sMin;
    QSpinBox *buffSpin;
    QPushButton *colourbutton;
    QSpinBox *cvelMax;
    QCheckBox *blurChk;
    QWidget *tab_2;
    QGridLayout *gridLayout;
    QLabel *label_15;
    QSpinBox *ME_Objects;
    QLabel *label_16;
    QSpinBox *ME_MaxSize;
    QDoubleSpinBox *ME_MaxVel;
    QLabel *label_18;
    QSpinBox *ME_MinSize;
    QLabel *label_19;
    QDoubleSpinBox *ME_MaxSizeVel;
    QLabel *label_20;
    QDoubleSpinBox *ME_MaxColVel;
    QDoubleSpinBox *ME_MaxTightVel;
    QLabel *label_23;
    QLabel *label_22;
    QDoubleSpinBox *ME_MaxTight;
    QDoubleSpinBox *ME_MinTight;
    QLabel *label_17;
    QLabel *label_21;
    QDoubleSpinBox *ME_SpeedFactor;
    QDoubleSpinBox *ME_MaxAccel;
    QLabel *label_44;
    QLabel *label_45;
    QLabel *label_46;
    QDoubleSpinBox *ME_MaxSizeAccel;
    QLabel *label_47;
    QDoubleSpinBox *ME_MaxTightAccel;
    QLabel *label_48;
    QDoubleSpinBox *ME_MaxColAccel;
    QLabel *label_49;
    QSpinBox *ME_Iterations;
    QWidget *tab_3;
    QLabel *label_24;
    QSpinBox *noiseMin;
    QLabel *label_25;
    QSpinBox *noiseMax;
    QWidget *tab_4;
    QLabel *label_27;
    QTextBrowser *stackOneText;
    QPushButton *pushButtonStackOne;
    QLabel *label_28;
    QTextBrowser *stackTwoText;
    QPushButton *pushButtonStackTwo;
    QLabel *label_29;
    QSpinBox *percentStartSpin;
    QLabel *label_30;
    QSpinBox *percentEndSpin;
    QLabel *label_31;
    QSpinBox *combineStart;
    QTextBrowser *textBrowser_3;
    QLabel *label_32;
    QSpinBox *combineEnd;
    QLabel *label_33;
    QWidget *tab_5;
    QPushButton *selectColour;
    QSpinBox *spinRed;
    QSpinBox *spinGreen;
    QSpinBox *spinBlue;
    QLabel *label_34;
    QLabel *label_35;
    QLabel *label_36;
    QWidget *tab_6;
    QPushButton *sFromImPushbutton;
    QTextBrowser *sFromImTxt;
    QLabel *label_37;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(664, 779);
        actionSave_settings = new QAction(MainWindow);
        actionSave_settings->setObjectName(QString::fromUtf8("actionSave_settings"));
        actionResize_environment = new QAction(MainWindow);
        actionResize_environment->setObjectName(QString::fromUtf8("actionResize_environment"));
        actionResize_environment->setCheckable(true);
        actionResize_environment->setChecked(true);
        actionRussell_Environment = new QAction(MainWindow);
        actionRussell_Environment->setObjectName(QString::fromUtf8("actionRussell_Environment"));
        actionMark_Environment = new QAction(MainWindow);
        actionMark_Environment->setObjectName(QString::fromUtf8("actionMark_Environment"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        enviroView = new QGraphicsView(centralWidget);
        enviroView->setObjectName(QString::fromUtf8("enviroView"));

        horizontalLayout->addWidget(enviroView);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 664, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        envSettings = new QDockWidget(MainWindow);
        envSettings->setObjectName(QString::fromUtf8("envSettings"));
        envSettings->setMinimumSize(QSize(350, 650));
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName(QString::fromUtf8("dockWidgetContents_3"));
        dockWidgetContents_3->setMaximumSize(QSize(16777215, 600));
        verticalLayout = new QVBoxLayout(dockWidgetContents_3);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame = new QFrame(dockWidgetContents_3);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setMinimumSize(QSize(0, 114));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label_10 = new QLabel(frame);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(280, 40, 81, 20));
        generateEnvironment = new QPushButton(frame);
        generateEnvironment->setObjectName(QString::fromUtf8("generateEnvironment"));
        generateEnvironment->setGeometry(QRect(10, 90, 311, 31));
        label_7 = new QLabel(frame);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 30, 101, 20));
        checkBox_2 = new QCheckBox(frame);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setGeometry(QRect(280, 60, 31, 16));
        checkBox_2->setChecked(false);
        textBrowser_2 = new QTextBrowser(frame);
        textBrowser_2->setObjectName(QString::fromUtf8("textBrowser_2"));
        textBrowser_2->setGeometry(QRect(120, 35, 150, 41));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textBrowser_2->sizePolicy().hasHeightForWidth());
        textBrowser_2->setSizePolicy(sizePolicy1);
        textBrowser_2->setMaximumSize(QSize(150, 50));
        pushButton = new QPushButton(frame);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(10, 50, 91, 23));
        numGenerations = new QSpinBox(frame);
        numGenerations->setObjectName(QString::fromUtf8("numGenerations"));
        numGenerations->setGeometry(QRect(110, 10, 61, 20));
        numGenerations->setMinimum(1);
        numGenerations->setMaximum(100000);
        numGenerations->setValue(500);
        label_5 = new QLabel(frame);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 10, 91, 20));
        spinSize = new QSpinBox(frame);
        spinSize->setObjectName(QString::fromUtf8("spinSize"));
        spinSize->setGeometry(QRect(240, 10, 60, 21));
        spinSize->setMinimum(50);
        spinSize->setMaximum(1000);
        spinSize->setValue(100);
        label_26 = new QLabel(frame);
        label_26->setObjectName(QString::fromUtf8("label_26"));
        label_26->setGeometry(QRect(200, 10, 66, 17));
        tabWidget = new QTabWidget(frame);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(0, 130, 331, 451));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        label_11 = new QLabel(tab);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(0, 109, 157, 20));
        label_9 = new QLabel(tab);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(0, 288, 157, 20));
        textBrowser = new QTextBrowser(tab);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(0, 9, 313, 75));
        textBrowser->setMaximumSize(QSize(16777215, 75));
        velMax2 = new QDoubleSpinBox(tab);
        velMax2->setObjectName(QString::fromUtf8("velMax2"));
        velMax2->setGeometry(QRect(163, 187, 140, 20));
        velMax2->setValue(0.1);
        label_3 = new QLabel(tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(0, 161, 157, 20));
        factorSpin = new QDoubleSpinBox(tab);
        factorSpin->setObjectName(QString::fromUtf8("factorSpin"));
        factorSpin->setGeometry(QRect(163, 314, 140, 20));
        factorSpin->setDecimals(3);
        factorSpin->setMinimum(0.001);
        factorSpin->setSingleStep(0.1);
        factorSpin->setValue(1);
        sMax = new QSpinBox(tab);
        sMax->setObjectName(QString::fromUtf8("sMax"));
        sMax->setGeometry(QRect(163, 135, 140, 20));
        sMax->setMinimum(2);
        sMax->setValue(14);
        periodic = new QCheckBox(tab);
        periodic->setObjectName(QString::fromUtf8("periodic"));
        periodic->setGeometry(QRect(163, 265, 140, 17));
        periodic->setChecked(true);
        label_6 = new QLabel(tab);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(0, 213, 157, 20));
        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 90, 157, 13));
        numSeed = new QSpinBox(tab);
        numSeed->setObjectName(QString::fromUtf8("numSeed"));
        numSeed->setGeometry(QRect(163, 109, 140, 20));
        numSeed->setMinimum(1);
        numSeed->setMaximum(1000);
        numSeed->setValue(4);
        label_14 = new QLabel(tab);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(0, 499, 157, 13));
        label_12 = new QLabel(tab);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(0, 239, 157, 20));
        checkBox = new QCheckBox(tab);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(0, 343, 157, 17));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(0, 135, 157, 20));
        label_13 = new QLabel(tab);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(0, 314, 157, 20));
        label_8 = new QLabel(tab);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(0, 265, 157, 17));
        convergeAt = new QDoubleSpinBox(tab);
        convergeAt->setObjectName(QString::fromUtf8("convergeAt"));
        convergeAt->setGeometry(QRect(163, 288, 140, 20));
        convergeAt->setDecimals(4);
        convergeAt->setMinimum(0);
        convergeAt->setMaximum(1);
        convergeAt->setSingleStep(0.001);
        convergeAt->setValue(0.1);
        label_4 = new QLabel(tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(0, 187, 157, 20));
        sMin = new QSpinBox(tab);
        sMin->setObjectName(QString::fromUtf8("sMin"));
        sMin->setGeometry(QRect(163, 161, 140, 20));
        sMin->setMinimum(1);
        sMin->setValue(5);
        buffSpin = new QSpinBox(tab);
        buffSpin->setObjectName(QString::fromUtf8("buffSpin"));
        buffSpin->setGeometry(QRect(163, 239, 140, 20));
        buffSpin->setMaximum(50);
        buffSpin->setValue(1);
        colourbutton = new QPushButton(tab);
        colourbutton->setObjectName(QString::fromUtf8("colourbutton"));
        colourbutton->setGeometry(QRect(163, 340, 140, 23));
        cvelMax = new QSpinBox(tab);
        cvelMax->setObjectName(QString::fromUtf8("cvelMax"));
        cvelMax->setGeometry(QRect(163, 213, 140, 20));
        cvelMax->setValue(5);
        blurChk = new QCheckBox(tab);
        blurChk->setObjectName(QString::fromUtf8("blurChk"));
        blurChk->setGeometry(QRect(0, 370, 150, 21));
        blurChk->setChecked(false);
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout = new QGridLayout(tab_2);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_15 = new QLabel(tab_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout->addWidget(label_15, 0, 0, 1, 1);

        ME_Objects = new QSpinBox(tab_2);
        ME_Objects->setObjectName(QString::fromUtf8("ME_Objects"));
        ME_Objects->setMinimum(1);
        ME_Objects->setMaximum(500);
        ME_Objects->setValue(15);

        gridLayout->addWidget(ME_Objects, 0, 1, 1, 1);

        label_16 = new QLabel(tab_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout->addWidget(label_16, 1, 0, 1, 1);

        ME_MaxSize = new QSpinBox(tab_2);
        ME_MaxSize->setObjectName(QString::fromUtf8("ME_MaxSize"));
        ME_MaxSize->setMinimum(1);
        ME_MaxSize->setMaximum(500);
        ME_MaxSize->setValue(100);

        gridLayout->addWidget(ME_MaxSize, 1, 1, 1, 1);

        ME_MaxVel = new QDoubleSpinBox(tab_2);
        ME_MaxVel->setObjectName(QString::fromUtf8("ME_MaxVel"));
        ME_MaxVel->setMaximum(50);
        ME_MaxVel->setSingleStep(0.1);
        ME_MaxVel->setValue(3);

        gridLayout->addWidget(ME_MaxVel, 7, 1, 1, 1);

        label_18 = new QLabel(tab_2);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout->addWidget(label_18, 2, 0, 1, 1);

        ME_MinSize = new QSpinBox(tab_2);
        ME_MinSize->setObjectName(QString::fromUtf8("ME_MinSize"));
        ME_MinSize->setMinimum(1);
        ME_MinSize->setMaximum(500);
        ME_MinSize->setValue(10);

        gridLayout->addWidget(ME_MinSize, 2, 1, 1, 1);

        label_19 = new QLabel(tab_2);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout->addWidget(label_19, 8, 0, 1, 1);

        ME_MaxSizeVel = new QDoubleSpinBox(tab_2);
        ME_MaxSizeVel->setObjectName(QString::fromUtf8("ME_MaxSizeVel"));
        ME_MaxSizeVel->setMaximum(50);
        ME_MaxSizeVel->setSingleStep(0.1);
        ME_MaxSizeVel->setValue(3);

        gridLayout->addWidget(ME_MaxSizeVel, 8, 1, 1, 1);

        label_20 = new QLabel(tab_2);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        gridLayout->addWidget(label_20, 9, 0, 1, 1);

        ME_MaxColVel = new QDoubleSpinBox(tab_2);
        ME_MaxColVel->setObjectName(QString::fromUtf8("ME_MaxColVel"));
        ME_MaxColVel->setMaximum(50);
        ME_MaxColVel->setSingleStep(0.1);
        ME_MaxColVel->setValue(8);

        gridLayout->addWidget(ME_MaxColVel, 9, 1, 1, 1);

        ME_MaxTightVel = new QDoubleSpinBox(tab_2);
        ME_MaxTightVel->setObjectName(QString::fromUtf8("ME_MaxTightVel"));
        ME_MaxTightVel->setMaximum(5);
        ME_MaxTightVel->setSingleStep(0.1);
        ME_MaxTightVel->setValue(0.4);

        gridLayout->addWidget(ME_MaxTightVel, 11, 1, 1, 1);

        label_23 = new QLabel(tab_2);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        gridLayout->addWidget(label_23, 3, 0, 1, 1);

        label_22 = new QLabel(tab_2);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        gridLayout->addWidget(label_22, 4, 0, 1, 1);

        ME_MaxTight = new QDoubleSpinBox(tab_2);
        ME_MaxTight->setObjectName(QString::fromUtf8("ME_MaxTight"));
        ME_MaxTight->setMinimum(0.01);
        ME_MaxTight->setMaximum(50);
        ME_MaxTight->setSingleStep(0.1);
        ME_MaxTight->setValue(8);

        gridLayout->addWidget(ME_MaxTight, 3, 1, 1, 1);

        ME_MinTight = new QDoubleSpinBox(tab_2);
        ME_MinTight->setObjectName(QString::fromUtf8("ME_MinTight"));
        ME_MinTight->setMinimum(0.01);
        ME_MinTight->setMaximum(50);
        ME_MinTight->setSingleStep(0.1);
        ME_MinTight->setValue(0.1);

        gridLayout->addWidget(ME_MinTight, 4, 1, 1, 1);

        label_17 = new QLabel(tab_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout->addWidget(label_17, 7, 0, 1, 1);

        label_21 = new QLabel(tab_2);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        gridLayout->addWidget(label_21, 11, 0, 1, 1);

        ME_SpeedFactor = new QDoubleSpinBox(tab_2);
        ME_SpeedFactor->setObjectName(QString::fromUtf8("ME_SpeedFactor"));
        ME_SpeedFactor->setMinimum(0.01);
        ME_SpeedFactor->setMaximum(50);
        ME_SpeedFactor->setSingleStep(0.1);
        ME_SpeedFactor->setValue(1);

        gridLayout->addWidget(ME_SpeedFactor, 16, 1, 1, 1);

        ME_MaxAccel = new QDoubleSpinBox(tab_2);
        ME_MaxAccel->setObjectName(QString::fromUtf8("ME_MaxAccel"));
        ME_MaxAccel->setMaximum(10);
        ME_MaxAccel->setSingleStep(0.1);
        ME_MaxAccel->setValue(1);

        gridLayout->addWidget(ME_MaxAccel, 12, 1, 1, 1);

        label_44 = new QLabel(tab_2);
        label_44->setObjectName(QString::fromUtf8("label_44"));

        gridLayout->addWidget(label_44, 16, 0, 1, 1);

        label_45 = new QLabel(tab_2);
        label_45->setObjectName(QString::fromUtf8("label_45"));

        gridLayout->addWidget(label_45, 12, 0, 1, 1);

        label_46 = new QLabel(tab_2);
        label_46->setObjectName(QString::fromUtf8("label_46"));

        gridLayout->addWidget(label_46, 13, 0, 1, 1);

        ME_MaxSizeAccel = new QDoubleSpinBox(tab_2);
        ME_MaxSizeAccel->setObjectName(QString::fromUtf8("ME_MaxSizeAccel"));
        ME_MaxSizeAccel->setMaximum(50);
        ME_MaxSizeAccel->setSingleStep(0.1);
        ME_MaxSizeAccel->setValue(2);

        gridLayout->addWidget(ME_MaxSizeAccel, 13, 1, 1, 1);

        label_47 = new QLabel(tab_2);
        label_47->setObjectName(QString::fromUtf8("label_47"));

        gridLayout->addWidget(label_47, 14, 0, 1, 1);

        ME_MaxTightAccel = new QDoubleSpinBox(tab_2);
        ME_MaxTightAccel->setObjectName(QString::fromUtf8("ME_MaxTightAccel"));
        ME_MaxTightAccel->setMaximum(4);
        ME_MaxTightAccel->setSingleStep(0.1);
        ME_MaxTightAccel->setValue(0.1);

        gridLayout->addWidget(ME_MaxTightAccel, 14, 1, 1, 1);

        label_48 = new QLabel(tab_2);
        label_48->setObjectName(QString::fromUtf8("label_48"));

        gridLayout->addWidget(label_48, 15, 0, 1, 1);

        ME_MaxColAccel = new QDoubleSpinBox(tab_2);
        ME_MaxColAccel->setObjectName(QString::fromUtf8("ME_MaxColAccel"));
        ME_MaxColAccel->setMaximum(50);
        ME_MaxColAccel->setSingleStep(0.1);
        ME_MaxColAccel->setValue(2);

        gridLayout->addWidget(ME_MaxColAccel, 15, 1, 1, 1);

        label_49 = new QLabel(tab_2);
        label_49->setObjectName(QString::fromUtf8("label_49"));

        gridLayout->addWidget(label_49, 17, 0, 1, 1);

        ME_Iterations = new QSpinBox(tab_2);
        ME_Iterations->setObjectName(QString::fromUtf8("ME_Iterations"));
        ME_Iterations->setMinimum(1);
        ME_Iterations->setMaximum(1000);

        gridLayout->addWidget(ME_Iterations, 17, 1, 1, 1);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        label_24 = new QLabel(tab_3);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        label_24->setGeometry(QRect(30, 20, 121, 17));
        noiseMin = new QSpinBox(tab_3);
        noiseMin->setObjectName(QString::fromUtf8("noiseMin"));
        noiseMin->setGeometry(QRect(170, 20, 60, 27));
        noiseMin->setMaximum(254);
        noiseMin->setValue(100);
        label_25 = new QLabel(tab_3);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setGeometry(QRect(30, 60, 121, 17));
        noiseMax = new QSpinBox(tab_3);
        noiseMax->setObjectName(QString::fromUtf8("noiseMax"));
        noiseMax->setGeometry(QRect(170, 50, 60, 27));
        noiseMax->setMaximum(255);
        noiseMax->setSingleStep(1);
        noiseMax->setValue(156);
        tabWidget->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        label_27 = new QLabel(tab_4);
        label_27->setObjectName(QString::fromUtf8("label_27"));
        label_27->setGeometry(QRect(10, 140, 81, 17));
        stackOneText = new QTextBrowser(tab_4);
        stackOneText->setObjectName(QString::fromUtf8("stackOneText"));
        stackOneText->setGeometry(QRect(100, 140, 221, 51));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(stackOneText->sizePolicy().hasHeightForWidth());
        stackOneText->setSizePolicy(sizePolicy2);
        pushButtonStackOne = new QPushButton(tab_4);
        pushButtonStackOne->setObjectName(QString::fromUtf8("pushButtonStackOne"));
        pushButtonStackOne->setGeometry(QRect(0, 160, 98, 27));
        label_28 = new QLabel(tab_4);
        label_28->setObjectName(QString::fromUtf8("label_28"));
        label_28->setGeometry(QRect(10, 200, 71, 17));
        stackTwoText = new QTextBrowser(tab_4);
        stackTwoText->setObjectName(QString::fromUtf8("stackTwoText"));
        stackTwoText->setGeometry(QRect(100, 200, 221, 51));
        pushButtonStackTwo = new QPushButton(tab_4);
        pushButtonStackTwo->setObjectName(QString::fromUtf8("pushButtonStackTwo"));
        pushButtonStackTwo->setGeometry(QRect(0, 220, 98, 27));
        label_29 = new QLabel(tab_4);
        label_29->setObjectName(QString::fromUtf8("label_29"));
        label_29->setGeometry(QRect(10, 320, 301, 17));
        percentStartSpin = new QSpinBox(tab_4);
        percentStartSpin->setObjectName(QString::fromUtf8("percentStartSpin"));
        percentStartSpin->setGeometry(QRect(240, 340, 60, 27));
        percentStartSpin->setMaximum(100);
        percentStartSpin->setValue(50);
        label_30 = new QLabel(tab_4);
        label_30->setObjectName(QString::fromUtf8("label_30"));
        label_30->setGeometry(QRect(10, 370, 291, 17));
        percentEndSpin = new QSpinBox(tab_4);
        percentEndSpin->setObjectName(QString::fromUtf8("percentEndSpin"));
        percentEndSpin->setGeometry(QRect(240, 390, 60, 27));
        percentEndSpin->setMaximum(100);
        percentEndSpin->setValue(50);
        label_31 = new QLabel(tab_4);
        label_31->setObjectName(QString::fromUtf8("label_31"));
        label_31->setGeometry(QRect(10, 280, 91, 17));
        combineStart = new QSpinBox(tab_4);
        combineStart->setObjectName(QString::fromUtf8("combineStart"));
        combineStart->setGeometry(QRect(90, 280, 60, 27));
        combineStart->setMaximum(10000);
        textBrowser_3 = new QTextBrowser(tab_4);
        textBrowser_3->setObjectName(QString::fromUtf8("textBrowser_3"));
        textBrowser_3->setGeometry(QRect(10, 10, 311, 121));
        label_32 = new QLabel(tab_4);
        label_32->setObjectName(QString::fromUtf8("label_32"));
        label_32->setGeometry(QRect(160, 280, 91, 20));
        combineEnd = new QSpinBox(tab_4);
        combineEnd->setObjectName(QString::fromUtf8("combineEnd"));
        combineEnd->setGeometry(QRect(260, 280, 60, 27));
        combineEnd->setMaximum(10000);
        label_33 = new QLabel(tab_4);
        label_33->setObjectName(QString::fromUtf8("label_33"));
        label_33->setGeometry(QRect(10, 260, 81, 17));
        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        selectColour = new QPushButton(tab_5);
        selectColour->setObjectName(QString::fromUtf8("selectColour"));
        selectColour->setGeometry(QRect(10, 20, 98, 27));
        spinRed = new QSpinBox(tab_5);
        spinRed->setObjectName(QString::fromUtf8("spinRed"));
        spinRed->setGeometry(QRect(150, 70, 60, 27));
        spinRed->setMaximum(255);
        spinRed->setValue(255);
        spinGreen = new QSpinBox(tab_5);
        spinGreen->setObjectName(QString::fromUtf8("spinGreen"));
        spinGreen->setGeometry(QRect(150, 110, 60, 27));
        spinGreen->setMaximum(255);
        spinGreen->setValue(255);
        spinBlue = new QSpinBox(tab_5);
        spinBlue->setObjectName(QString::fromUtf8("spinBlue"));
        spinBlue->setGeometry(QRect(150, 150, 60, 27));
        spinBlue->setMaximum(255);
        spinBlue->setValue(255);
        label_34 = new QLabel(tab_5);
        label_34->setObjectName(QString::fromUtf8("label_34"));
        label_34->setGeometry(QRect(50, 70, 66, 17));
        label_35 = new QLabel(tab_5);
        label_35->setObjectName(QString::fromUtf8("label_35"));
        label_35->setGeometry(QRect(50, 110, 66, 17));
        label_36 = new QLabel(tab_5);
        label_36->setObjectName(QString::fromUtf8("label_36"));
        label_36->setGeometry(QRect(50, 150, 66, 17));
        tabWidget->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        sFromImPushbutton = new QPushButton(tab_6);
        sFromImPushbutton->setObjectName(QString::fromUtf8("sFromImPushbutton"));
        sFromImPushbutton->setGeometry(QRect(10, 20, 121, 27));
        sFromImTxt = new QTextBrowser(tab_6);
        sFromImTxt->setObjectName(QString::fromUtf8("sFromImTxt"));
        sFromImTxt->setGeometry(QRect(10, 80, 311, 91));
        label_37 = new QLabel(tab_6);
        label_37->setObjectName(QString::fromUtf8("label_37"));
        label_37->setGeometry(QRect(10, 60, 66, 17));
        tabWidget->addTab(tab_6, QString());

        verticalLayout->addWidget(frame);

        envSettings->setWidget(dockWidgetContents_3);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), envSettings);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionResize_environment);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionSave_settings->setText(QApplication::translate("MainWindow", "Save settings", 0, QApplication::UnicodeUTF8));
        actionResize_environment->setText(QApplication::translate("MainWindow", "Resize environment", 0, QApplication::UnicodeUTF8));
        actionRussell_Environment->setText(QApplication::translate("MainWindow", "Russell Environment", 0, QApplication::UnicodeUTF8));
        actionMark_Environment->setText(QApplication::translate("MainWindow", "Mark Environment", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "Environment", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        envSettings->setAccessibleName(QApplication::translate("MainWindow", "Enivronmental Settings", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        label_10->setText(QApplication::translate("MainWindow", "Save?", 0, QApplication::UnicodeUTF8));
        generateEnvironment->setText(QApplication::translate("MainWindow", "Go!", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Save Location:", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QString());
        textBrowser_2->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">C:/tmp/</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "Change...", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Generate for...", 0, QApplication::UnicodeUTF8));
        label_26->setText(QApplication::translate("MainWindow", "Size:", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Number", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "Convergence", 0, QApplication::UnicodeUTF8));
        textBrowser->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Please enter the settings for the dynamic environment below. Most are obvious. Convergence sets the amount of smoothing between spots and background. 0.01 is very smooth, but will initially take about a minute to create environment.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Max size velocity", 0, QApplication::UnicodeUTF8));
        periodic->setText(QApplication::translate("MainWindow", "Periodic", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Max colour velocity", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Seeds:", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("MainWindow", "Universal blur", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MainWindow", "Overlap blur factor", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("MainWindow", "Background colour", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Max size", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("MainWindow", "Speed factor", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Background:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Max velocity", 0, QApplication::UnicodeUTF8));
        colourbutton->setText(QApplication::translate("MainWindow", "Pick", 0, QApplication::UnicodeUTF8));
        blurChk->setText(QApplication::translate("MainWindow", "Universal Blur", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Dyn 1", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MainWindow", "Object Count", 0, QApplication::UnicodeUTF8));
        label_16->setText(QApplication::translate("MainWindow", "Maximum Object Size", 0, QApplication::UnicodeUTF8));
        label_18->setText(QApplication::translate("MainWindow", "Minimum Object Size", 0, QApplication::UnicodeUTF8));
        label_19->setText(QApplication::translate("MainWindow", "Maximum Size Velocity", 0, QApplication::UnicodeUTF8));
        label_20->setText(QApplication::translate("MainWindow", "Maximum Colour Velocity", 0, QApplication::UnicodeUTF8));
        label_23->setText(QApplication::translate("MainWindow", "Maximum Tightness", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("MainWindow", "Minimum Tightness", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MainWindow", "Maximum Velocity", 0, QApplication::UnicodeUTF8));
        label_21->setText(QApplication::translate("MainWindow", "Maximum Tightness Velocity", 0, QApplication::UnicodeUTF8));
        label_44->setText(QApplication::translate("MainWindow", "Speed Factor", 0, QApplication::UnicodeUTF8));
        label_45->setText(QApplication::translate("MainWindow", "Maximum Acceleration", 0, QApplication::UnicodeUTF8));
        label_46->setText(QApplication::translate("MainWindow", "Max Size Acceleration", 0, QApplication::UnicodeUTF8));
        label_47->setText(QApplication::translate("MainWindow", "Max Tightness Acceleration", 0, QApplication::UnicodeUTF8));
        label_48->setText(QApplication::translate("MainWindow", "Max Colour Acceleration", 0, QApplication::UnicodeUTF8));
        label_49->setText(QApplication::translate("MainWindow", "Apply accelerations every ...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Dyn 2", 0, QApplication::UnicodeUTF8));
        label_24->setText(QApplication::translate("MainWindow", "Minimum value", 0, QApplication::UnicodeUTF8));
        label_25->setText(QApplication::translate("MainWindow", "Maximum value", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Noise", 0, QApplication::UnicodeUTF8));
        label_27->setText(QApplication::translate("MainWindow", "Stack one:", 0, QApplication::UnicodeUTF8));
        pushButtonStackOne->setText(QApplication::translate("MainWindow", "Change...", 0, QApplication::UnicodeUTF8));
        label_28->setText(QApplication::translate("MainWindow", "Stack two:", 0, QApplication::UnicodeUTF8));
        pushButtonStackTwo->setText(QApplication::translate("MainWindow", "Change...", 0, QApplication::UnicodeUTF8));
        label_29->setText(QApplication::translate("MainWindow", "Percent influence stack one (start combine):", 0, QApplication::UnicodeUTF8));
        label_30->setText(QApplication::translate("MainWindow", "Percent influence stack one (end combine):", 0, QApplication::UnicodeUTF8));
        label_31->setText(QApplication::translate("MainWindow", "Start slice:", 0, QApplication::UnicodeUTF8));
        textBrowser_3->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:9pt;\">Combines stack two with stack one from  Start Slice to End Slice. Percentage start and end for influence of stack one, interpolate between. Limit for End Slice is end of stack one. If number of generations is larger than this it will duplicate stack two. If start and end slice are maximum it will concatenate two stacks. </span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_32->setText(QApplication::translate("MainWindow", "End slice +1:", 0, QApplication::UnicodeUTF8));
        label_33->setText(QApplication::translate("MainWindow", "Stack two:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("MainWindow", "Comb", 0, QApplication::UnicodeUTF8));
        selectColour->setText(QApplication::translate("MainWindow", "Select...", 0, QApplication::UnicodeUTF8));
        label_34->setText(QApplication::translate("MainWindow", "Red", 0, QApplication::UnicodeUTF8));
        label_35->setText(QApplication::translate("MainWindow", "Green", 0, QApplication::UnicodeUTF8));
        label_36->setText(QApplication::translate("MainWindow", "Blue", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("MainWindow", "Col", 0, QApplication::UnicodeUTF8));
        sFromImPushbutton->setText(QApplication::translate("MainWindow", "Choose image", 0, QApplication::UnicodeUTF8));
        label_37->setText(QApplication::translate("MainWindow", "Current:", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("MainWindow", "Stack", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
