/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_3;
    QGraphicsView *enviroView;
    QMenuBar *menuBar;
    QToolBar *toolBar;
    QStatusBar *statusBar;
    QDockWidget *envSettings;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QFrame *dockWidgetFrame;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *settings_tab_widget;
    QWidget *output_tab;
    QVBoxLayout *verticalLayout_4;
    QLabel *instructions_label;
    QLabel *output_label;
    QCheckBox *save_images_checkbox;
    QLabel *label_7;
    QTextBrowser *path;
    QPushButton *change_path;
    QLabel *label_5;
    QSpinBox *numGenerations;
    QLabel *label_26;
    QSpinBox *spinSize;
    QLabel *label_32;
    QComboBox *environment_comboBox;
    QLabel *display_label;
    QCheckBox *resize;
    QSpacerItem *verticalSpacer;
    QWidget *russell_tab;
    QGridLayout *gridLayout_4;
    QSpinBox *buffSpin;
    QLabel *russell_label;
    QLabel *label;
    QLabel *label_6;
    QSpinBox *sMin;
    QDoubleSpinBox *velMax2;
    QLabel *label_13;
    QSpinBox *cvelMax;
    QDoubleSpinBox *convergeAt;
    QLabel *label_4;
    QCheckBox *periodic;
    QLabel *label_12;
    QSpinBox *numSeed;
    QSpacerItem *verticalSpacer_6;
    QLabel *label_3;
    QLabel *label_11;
    QDoubleSpinBox *factorSpin;
    QSpinBox *sMax;
    QCheckBox *blurChk;
    QLabel *label_8;
    QLabel *label_2;
    QLabel *label_9;
    QWidget *mark_tab;
    QGridLayout *gridLayout;
    QDoubleSpinBox *ME_MaxAccel;
    QLabel *label_44;
    QLabel *label_45;
    QLabel *label_46;
    QDoubleSpinBox *ME_maxSizeAccel;
    QLabel *label_47;
    QDoubleSpinBox *ME_MaxTightAccel;
    QLabel *label_48;
    QDoubleSpinBox *ME_MaxColAccel;
    QLabel *label_49;
    QSpinBox *ME_Iterations;
    QSpacerItem *verticalSpacer_5;
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
    QLabel *label_15;
    QSpinBox *ME_Objects;
    QLabel *label_16;
    QSpinBox *ME_maxSize;
    QDoubleSpinBox *ME_MaxVel;
    QLabel *label_18;
    QSpinBox *ME_MinSize;
    QLabel *label_19;
    QDoubleSpinBox *ME_maxSizeVel;
    QLabel *m_environment_label;
    QWidget *noise_tab;
    QGridLayout *gridLayout_5;
    QLabel *label_24;
    QSpinBox *noiseMin;
    QLabel *label_25;
    QSpinBox *noiseMax;
    QSpacerItem *verticalSpacer_2;
    QLabel *noise_label;
    QWidget *combine_tab;
    QVBoxLayout *verticalLayout_3;
    QLabel *combine_label;
    QLabel *label_27;
    QTextBrowser *stackOneText;
    QPushButton *pushButtonStackOne;
    QLabel *label_28;
    QTextBrowser *stackTwoText;
    QPushButton *pushButtonStackTwo;
    QLabel *label_31;
    QSpinBox *combineStart;
    QLabel *label_29;
    QSpinBox *percentStartSpin;
    QLabel *label_30;
    QSpinBox *percentEndSpin;
    QSpacerItem *verticalSpacer_4;
    QWidget *colour_tab;
    QGridLayout *gridLayout_7;
    QPushButton *selectColour;
    QSpinBox *spinGreen;
    QLabel *label_10;
    QSpinBox *spinRed;
    QSpinBox *spinBlue;
    QLabel *label_34;
    QLabel *label_36;
    QLabel *label_35;
    QLabel *label_14;
    QSpacerItem *verticalSpacer_3;
    QWidget *stack_tab;
    QGridLayout *gridLayout_8;
    QTextBrowser *sFromImTxt;
    QPushButton *sFromImPushbutton;
    QLabel *label_37;
    QLabel *stack_label;
    QCheckBox *ME_Toroidal;
    QLabel *labelToroidal;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(723, 861);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_3 = new QGridLayout(centralWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        enviroView = new QGraphicsView(centralWidget);
        enviroView->setObjectName(QString::fromUtf8("enviroView"));
        enviroView->setMinimumSize(QSize(400, 400));
        enviroView->setFrameShape(QFrame::Box);

        gridLayout_3->addWidget(enviroView, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 723, 26));
        MainWindow->setMenuBar(menuBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setIconSize(QSize(30, 30));
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        envSettings = new QDockWidget(MainWindow);
        envSettings->setObjectName(QString::fromUtf8("envSettings"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(envSettings->sizePolicy().hasHeightForWidth());
        envSettings->setSizePolicy(sizePolicy);
        envSettings->setMinimumSize(QSize(300, 400));
        envSettings->setMaximumSize(QSize(500, 524287));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        dockWidgetContents->setMinimumSize(QSize(50, 50));
        dockWidgetContents->setMaximumSize(QSize(16777215, 16777215));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        dockWidgetFrame = new QFrame(dockWidgetContents);
        dockWidgetFrame->setObjectName(QString::fromUtf8("dockWidgetFrame"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(dockWidgetFrame->sizePolicy().hasHeightForWidth());
        dockWidgetFrame->setSizePolicy(sizePolicy1);
        dockWidgetFrame->setMinimumSize(QSize(0, 114));
        dockWidgetFrame->setFrameShape(QFrame::StyledPanel);
        dockWidgetFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(dockWidgetFrame);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        settings_tab_widget = new QTabWidget(dockWidgetFrame);
        settings_tab_widget->setObjectName(QString::fromUtf8("settings_tab_widget"));
        settings_tab_widget->setTabPosition(QTabWidget::West);
        output_tab = new QWidget();
        output_tab->setObjectName(QString::fromUtf8("output_tab"));
        verticalLayout_4 = new QVBoxLayout(output_tab);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        instructions_label = new QLabel(output_tab);
        instructions_label->setObjectName(QString::fromUtf8("instructions_label"));

        verticalLayout_4->addWidget(instructions_label);

        output_label = new QLabel(output_tab);
        output_label->setObjectName(QString::fromUtf8("output_label"));

        verticalLayout_4->addWidget(output_label);

        save_images_checkbox = new QCheckBox(output_tab);
        save_images_checkbox->setObjectName(QString::fromUtf8("save_images_checkbox"));
        save_images_checkbox->setChecked(true);

        verticalLayout_4->addWidget(save_images_checkbox);

        label_7 = new QLabel(output_tab);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        verticalLayout_4->addWidget(label_7);

        path = new QTextBrowser(output_tab);
        path->setObjectName(QString::fromUtf8("path"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(path->sizePolicy().hasHeightForWidth());
        path->setSizePolicy(sizePolicy2);
        path->setMaximumSize(QSize(1000, 30));

        verticalLayout_4->addWidget(path);

        change_path = new QPushButton(output_tab);
        change_path->setObjectName(QString::fromUtf8("change_path"));

        verticalLayout_4->addWidget(change_path);

        label_5 = new QLabel(output_tab);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout_4->addWidget(label_5);

        numGenerations = new QSpinBox(output_tab);
        numGenerations->setObjectName(QString::fromUtf8("numGenerations"));
        numGenerations->setMinimum(1);
        numGenerations->setMaximum(100000);
        numGenerations->setValue(500);

        verticalLayout_4->addWidget(numGenerations);

        label_26 = new QLabel(output_tab);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        verticalLayout_4->addWidget(label_26);

        spinSize = new QSpinBox(output_tab);
        spinSize->setObjectName(QString::fromUtf8("spinSize"));
        spinSize->setMinimum(50);
        spinSize->setMaximum(1000);
        spinSize->setValue(100);

        verticalLayout_4->addWidget(spinSize);

        label_32 = new QLabel(output_tab);
        label_32->setObjectName(QString::fromUtf8("label_32"));

        verticalLayout_4->addWidget(label_32);

        environment_comboBox = new QComboBox(output_tab);
        environment_comboBox->setObjectName(QString::fromUtf8("environment_comboBox"));

        verticalLayout_4->addWidget(environment_comboBox);

        display_label = new QLabel(output_tab);
        display_label->setObjectName(QString::fromUtf8("display_label"));

        verticalLayout_4->addWidget(display_label);

        resize = new QCheckBox(output_tab);
        resize->setObjectName(QString::fromUtf8("resize"));
        resize->setChecked(true);

        verticalLayout_4->addWidget(resize);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        settings_tab_widget->addTab(output_tab, QString());
        russell_tab = new QWidget();
        russell_tab->setObjectName(QString::fromUtf8("russell_tab"));
        gridLayout_4 = new QGridLayout(russell_tab);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        buffSpin = new QSpinBox(russell_tab);
        buffSpin->setObjectName(QString::fromUtf8("buffSpin"));
        buffSpin->setMaximum(50);
        buffSpin->setValue(1);

        gridLayout_4->addWidget(buffSpin, 8, 1, 1, 1);

        russell_label = new QLabel(russell_tab);
        russell_label->setObjectName(QString::fromUtf8("russell_label"));

        gridLayout_4->addWidget(russell_label, 0, 0, 1, 2);

        label = new QLabel(russell_tab);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_4->addWidget(label, 1, 0, 1, 2);

        label_6 = new QLabel(russell_tab);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_4->addWidget(label_6, 7, 0, 1, 1);

        sMin = new QSpinBox(russell_tab);
        sMin->setObjectName(QString::fromUtf8("sMin"));
        sMin->setMinimum(0);
        sMin->setValue(5);

        gridLayout_4->addWidget(sMin, 5, 1, 1, 1);

        velMax2 = new QDoubleSpinBox(russell_tab);
        velMax2->setObjectName(QString::fromUtf8("velMax2"));
        velMax2->setValue(0.100000000000000);

        gridLayout_4->addWidget(velMax2, 6, 1, 1, 1);

        label_13 = new QLabel(russell_tab);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout_4->addWidget(label_13, 11, 0, 1, 1);

        cvelMax = new QSpinBox(russell_tab);
        cvelMax->setObjectName(QString::fromUtf8("cvelMax"));
        cvelMax->setValue(5);

        gridLayout_4->addWidget(cvelMax, 7, 1, 1, 1);

        convergeAt = new QDoubleSpinBox(russell_tab);
        convergeAt->setObjectName(QString::fromUtf8("convergeAt"));
        convergeAt->setDecimals(4);
        convergeAt->setMinimum(0.000000000000000);
        convergeAt->setMaximum(1.000000000000000);
        convergeAt->setSingleStep(0.001000000000000);
        convergeAt->setValue(0.100000000000000);

        gridLayout_4->addWidget(convergeAt, 10, 1, 1, 1);

        label_4 = new QLabel(russell_tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_4->addWidget(label_4, 6, 0, 1, 1);

        periodic = new QCheckBox(russell_tab);
        periodic->setObjectName(QString::fromUtf8("periodic"));
        periodic->setChecked(true);

        gridLayout_4->addWidget(periodic, 9, 1, 1, 1);

        label_12 = new QLabel(russell_tab);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout_4->addWidget(label_12, 8, 0, 1, 1);

        numSeed = new QSpinBox(russell_tab);
        numSeed->setObjectName(QString::fromUtf8("numSeed"));
        numSeed->setMinimum(1);
        numSeed->setMaximum(1000);
        numSeed->setValue(4);

        gridLayout_4->addWidget(numSeed, 2, 1, 1, 1);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer_6, 13, 0, 1, 2);

        label_3 = new QLabel(russell_tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_4->addWidget(label_3, 5, 0, 1, 1);

        label_11 = new QLabel(russell_tab);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout_4->addWidget(label_11, 2, 0, 1, 1);

        factorSpin = new QDoubleSpinBox(russell_tab);
        factorSpin->setObjectName(QString::fromUtf8("factorSpin"));
        factorSpin->setDecimals(3);
        factorSpin->setMinimum(0.001000000000000);
        factorSpin->setSingleStep(0.100000000000000);
        factorSpin->setValue(1.000000000000000);

        gridLayout_4->addWidget(factorSpin, 11, 1, 1, 1);

        sMax = new QSpinBox(russell_tab);
        sMax->setObjectName(QString::fromUtf8("sMax"));
        sMax->setMinimum(2);
        sMax->setValue(14);

        gridLayout_4->addWidget(sMax, 4, 1, 1, 1);

        blurChk = new QCheckBox(russell_tab);
        blurChk->setObjectName(QString::fromUtf8("blurChk"));
        blurChk->setChecked(false);

        gridLayout_4->addWidget(blurChk, 12, 0, 1, 2);

        label_8 = new QLabel(russell_tab);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_4->addWidget(label_8, 9, 0, 1, 1);

        label_2 = new QLabel(russell_tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_4->addWidget(label_2, 4, 0, 1, 1);

        label_9 = new QLabel(russell_tab);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_4->addWidget(label_9, 10, 0, 1, 1);

        settings_tab_widget->addTab(russell_tab, QString());
        mark_tab = new QWidget();
        mark_tab->setObjectName(QString::fromUtf8("mark_tab"));
        gridLayout = new QGridLayout(mark_tab);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        ME_MaxAccel = new QDoubleSpinBox(mark_tab);
        ME_MaxAccel->setObjectName(QString::fromUtf8("ME_MaxAccel"));
        ME_MaxAccel->setMaximum(10.000000000000000);
        ME_MaxAccel->setSingleStep(0.100000000000000);
        ME_MaxAccel->setValue(1.000000000000000);

        gridLayout->addWidget(ME_MaxAccel, 13, 1, 1, 1);

        label_44 = new QLabel(mark_tab);
        label_44->setObjectName(QString::fromUtf8("label_44"));

        gridLayout->addWidget(label_44, 17, 0, 1, 1);

        label_45 = new QLabel(mark_tab);
        label_45->setObjectName(QString::fromUtf8("label_45"));

        gridLayout->addWidget(label_45, 13, 0, 1, 1);

        label_46 = new QLabel(mark_tab);
        label_46->setObjectName(QString::fromUtf8("label_46"));

        gridLayout->addWidget(label_46, 14, 0, 1, 1);

        ME_maxSizeAccel = new QDoubleSpinBox(mark_tab);
        ME_maxSizeAccel->setObjectName(QString::fromUtf8("ME_maxSizeAccel"));
        ME_maxSizeAccel->setMaximum(50.000000000000000);
        ME_maxSizeAccel->setSingleStep(0.100000000000000);
        ME_maxSizeAccel->setValue(2.000000000000000);

        gridLayout->addWidget(ME_maxSizeAccel, 14, 1, 1, 1);

        label_47 = new QLabel(mark_tab);
        label_47->setObjectName(QString::fromUtf8("label_47"));

        gridLayout->addWidget(label_47, 15, 0, 1, 1);

        ME_MaxTightAccel = new QDoubleSpinBox(mark_tab);
        ME_MaxTightAccel->setObjectName(QString::fromUtf8("ME_MaxTightAccel"));
        ME_MaxTightAccel->setMaximum(4.000000000000000);
        ME_MaxTightAccel->setSingleStep(0.100000000000000);
        ME_MaxTightAccel->setValue(0.100000000000000);

        gridLayout->addWidget(ME_MaxTightAccel, 15, 1, 1, 1);

        label_48 = new QLabel(mark_tab);
        label_48->setObjectName(QString::fromUtf8("label_48"));

        gridLayout->addWidget(label_48, 16, 0, 1, 1);

        ME_MaxColAccel = new QDoubleSpinBox(mark_tab);
        ME_MaxColAccel->setObjectName(QString::fromUtf8("ME_MaxColAccel"));
        ME_MaxColAccel->setMaximum(50.000000000000000);
        ME_MaxColAccel->setSingleStep(0.100000000000000);
        ME_MaxColAccel->setValue(2.000000000000000);

        gridLayout->addWidget(ME_MaxColAccel, 16, 1, 1, 1);

        label_49 = new QLabel(mark_tab);
        label_49->setObjectName(QString::fromUtf8("label_49"));

        gridLayout->addWidget(label_49, 18, 0, 1, 1);

        ME_Iterations = new QSpinBox(mark_tab);
        ME_Iterations->setObjectName(QString::fromUtf8("ME_Iterations"));
        ME_Iterations->setMinimum(1);
        ME_Iterations->setMaximum(1000);

        gridLayout->addWidget(ME_Iterations, 18, 1, 1, 1);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_5, 20, 0, 1, 1);

        label_20 = new QLabel(mark_tab);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        gridLayout->addWidget(label_20, 10, 0, 1, 1);

        ME_MaxColVel = new QDoubleSpinBox(mark_tab);
        ME_MaxColVel->setObjectName(QString::fromUtf8("ME_MaxColVel"));
        ME_MaxColVel->setMaximum(50.000000000000000);
        ME_MaxColVel->setSingleStep(0.100000000000000);
        ME_MaxColVel->setValue(8.000000000000000);

        gridLayout->addWidget(ME_MaxColVel, 10, 1, 1, 1);

        ME_MaxTightVel = new QDoubleSpinBox(mark_tab);
        ME_MaxTightVel->setObjectName(QString::fromUtf8("ME_MaxTightVel"));
        ME_MaxTightVel->setMaximum(5.000000000000000);
        ME_MaxTightVel->setSingleStep(0.100000000000000);
        ME_MaxTightVel->setValue(0.400000000000000);

        gridLayout->addWidget(ME_MaxTightVel, 12, 1, 1, 1);

        label_23 = new QLabel(mark_tab);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        gridLayout->addWidget(label_23, 4, 0, 1, 1);

        label_22 = new QLabel(mark_tab);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        gridLayout->addWidget(label_22, 5, 0, 1, 1);

        ME_MaxTight = new QDoubleSpinBox(mark_tab);
        ME_MaxTight->setObjectName(QString::fromUtf8("ME_MaxTight"));
        ME_MaxTight->setMinimum(0.010000000000000);
        ME_MaxTight->setMaximum(50.000000000000000);
        ME_MaxTight->setSingleStep(0.100000000000000);
        ME_MaxTight->setValue(8.000000000000000);

        gridLayout->addWidget(ME_MaxTight, 4, 1, 1, 1);

        ME_MinTight = new QDoubleSpinBox(mark_tab);
        ME_MinTight->setObjectName(QString::fromUtf8("ME_MinTight"));
        ME_MinTight->setMinimum(0.010000000000000);
        ME_MinTight->setMaximum(50.000000000000000);
        ME_MinTight->setSingleStep(0.100000000000000);
        ME_MinTight->setValue(0.100000000000000);

        gridLayout->addWidget(ME_MinTight, 5, 1, 1, 1);

        label_17 = new QLabel(mark_tab);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout->addWidget(label_17, 8, 0, 1, 1);

        label_21 = new QLabel(mark_tab);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        gridLayout->addWidget(label_21, 12, 0, 1, 1);

        ME_SpeedFactor = new QDoubleSpinBox(mark_tab);
        ME_SpeedFactor->setObjectName(QString::fromUtf8("ME_SpeedFactor"));
        ME_SpeedFactor->setMinimum(0.010000000000000);
        ME_SpeedFactor->setMaximum(50.000000000000000);
        ME_SpeedFactor->setSingleStep(0.100000000000000);
        ME_SpeedFactor->setValue(1.000000000000000);

        gridLayout->addWidget(ME_SpeedFactor, 17, 1, 1, 1);

        label_15 = new QLabel(mark_tab);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout->addWidget(label_15, 1, 0, 1, 1);

        ME_Objects = new QSpinBox(mark_tab);
        ME_Objects->setObjectName(QString::fromUtf8("ME_Objects"));
        ME_Objects->setMinimum(1);
        ME_Objects->setMaximum(500);
        ME_Objects->setValue(15);

        gridLayout->addWidget(ME_Objects, 1, 1, 1, 1);

        label_16 = new QLabel(mark_tab);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout->addWidget(label_16, 2, 0, 1, 1);

        ME_maxSize = new QSpinBox(mark_tab);
        ME_maxSize->setObjectName(QString::fromUtf8("ME_maxSize"));
        ME_maxSize->setMinimum(1);
        ME_maxSize->setMaximum(500);
        ME_maxSize->setValue(100);

        gridLayout->addWidget(ME_maxSize, 2, 1, 1, 1);

        ME_MaxVel = new QDoubleSpinBox(mark_tab);
        ME_MaxVel->setObjectName(QString::fromUtf8("ME_MaxVel"));
        ME_MaxVel->setMaximum(50.000000000000000);
        ME_MaxVel->setSingleStep(0.100000000000000);
        ME_MaxVel->setValue(3.000000000000000);

        gridLayout->addWidget(ME_MaxVel, 8, 1, 1, 1);

        label_18 = new QLabel(mark_tab);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        gridLayout->addWidget(label_18, 3, 0, 1, 1);

        ME_MinSize = new QSpinBox(mark_tab);
        ME_MinSize->setObjectName(QString::fromUtf8("ME_MinSize"));
        ME_MinSize->setMinimum(1);
        ME_MinSize->setMaximum(500);
        ME_MinSize->setValue(10);

        gridLayout->addWidget(ME_MinSize, 3, 1, 1, 1);

        label_19 = new QLabel(mark_tab);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        gridLayout->addWidget(label_19, 9, 0, 1, 1);

        ME_maxSizeVel = new QDoubleSpinBox(mark_tab);
        ME_maxSizeVel->setObjectName(QString::fromUtf8("ME_maxSizeVel"));
        ME_maxSizeVel->setMaximum(50.000000000000000);
        ME_maxSizeVel->setSingleStep(0.100000000000000);
        ME_maxSizeVel->setValue(3.000000000000000);

        gridLayout->addWidget(ME_maxSizeVel, 9, 1, 1, 1);


        ME_Toroidal = new QCheckBox(mark_tab);
        ME_Toroidal->setObjectName(QString::fromUtf8("ME_Toroidal"));
        ME_Toroidal->setText("Toroidal");
        ME_Toroidal->setChecked(false);
        labelToroidal = new QLabel(mark_tab);
        labelToroidal->setObjectName(QString::fromUtf8("ME_Toroidal_Label"));
        labelToroidal->setText("Background:");
        gridLayout->addWidget(ME_Toroidal, 19, 1, 1, 1);
        gridLayout->addWidget(labelToroidal, 19, 0, 1, 1);


        m_environment_label = new QLabel(mark_tab);
        m_environment_label->setObjectName(QString::fromUtf8("m_environment_label"));

        gridLayout->addWidget(m_environment_label, 0, 0, 1, 2);

        settings_tab_widget->addTab(mark_tab, QString());
        noise_tab = new QWidget();
        noise_tab->setObjectName(QString::fromUtf8("noise_tab"));
        gridLayout_5 = new QGridLayout(noise_tab);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        label_24 = new QLabel(noise_tab);
        label_24->setObjectName(QString::fromUtf8("label_24"));

        gridLayout_5->addWidget(label_24, 1, 0, 1, 1);

        noiseMin = new QSpinBox(noise_tab);
        noiseMin->setObjectName(QString::fromUtf8("noiseMin"));
        noiseMin->setMaximum(254);
        noiseMin->setValue(100);

        gridLayout_5->addWidget(noiseMin, 1, 1, 1, 1);

        label_25 = new QLabel(noise_tab);
        label_25->setObjectName(QString::fromUtf8("label_25"));

        gridLayout_5->addWidget(label_25, 2, 0, 1, 1);

        noiseMax = new QSpinBox(noise_tab);
        noiseMax->setObjectName(QString::fromUtf8("noiseMax"));
        noiseMax->setMinimum(1);
        noiseMax->setMaximum(255);
        noiseMax->setSingleStep(1);
        noiseMax->setValue(156);

        gridLayout_5->addWidget(noiseMax, 2, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_2, 4, 0, 1, 2);

        noise_label = new QLabel(noise_tab);
        noise_label->setObjectName(QString::fromUtf8("noise_label"));

        gridLayout_5->addWidget(noise_label, 0, 0, 1, 2);

        settings_tab_widget->addTab(noise_tab, QString());
        combine_tab = new QWidget();
        combine_tab->setObjectName(QString::fromUtf8("combine_tab"));
        verticalLayout_3 = new QVBoxLayout(combine_tab);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        combine_label = new QLabel(combine_tab);
        combine_label->setObjectName(QString::fromUtf8("combine_label"));

        verticalLayout_3->addWidget(combine_label);

        label_27 = new QLabel(combine_tab);
        label_27->setObjectName(QString::fromUtf8("label_27"));

        verticalLayout_3->addWidget(label_27);

        stackOneText = new QTextBrowser(combine_tab);
        stackOneText->setObjectName(QString::fromUtf8("stackOneText"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(stackOneText->sizePolicy().hasHeightForWidth());
        stackOneText->setSizePolicy(sizePolicy3);
        stackOneText->setMaximumSize(QSize(16777215, 50));
        stackOneText->setTextInteractionFlags(Qt::NoTextInteraction);

        verticalLayout_3->addWidget(stackOneText);

        pushButtonStackOne = new QPushButton(combine_tab);
        pushButtonStackOne->setObjectName(QString::fromUtf8("pushButtonStackOne"));

        verticalLayout_3->addWidget(pushButtonStackOne);

        label_28 = new QLabel(combine_tab);
        label_28->setObjectName(QString::fromUtf8("label_28"));

        verticalLayout_3->addWidget(label_28);

        stackTwoText = new QTextBrowser(combine_tab);
        stackTwoText->setObjectName(QString::fromUtf8("stackTwoText"));
        stackTwoText->setMaximumSize(QSize(16777215, 50));
        stackTwoText->setTextInteractionFlags(Qt::NoTextInteraction);

        verticalLayout_3->addWidget(stackTwoText);

        pushButtonStackTwo = new QPushButton(combine_tab);
        pushButtonStackTwo->setObjectName(QString::fromUtf8("pushButtonStackTwo"));

        verticalLayout_3->addWidget(pushButtonStackTwo);

        label_31 = new QLabel(combine_tab);
        label_31->setObjectName(QString::fromUtf8("label_31"));

        verticalLayout_3->addWidget(label_31);

        combineStart = new QSpinBox(combine_tab);
        combineStart->setObjectName(QString::fromUtf8("combineStart"));
        combineStart->setMaximum(10000);

        verticalLayout_3->addWidget(combineStart);

        label_29 = new QLabel(combine_tab);
        label_29->setObjectName(QString::fromUtf8("label_29"));

        verticalLayout_3->addWidget(label_29);

        percentStartSpin = new QSpinBox(combine_tab);
        percentStartSpin->setObjectName(QString::fromUtf8("percentStartSpin"));
        percentStartSpin->setMaximum(100);
        percentStartSpin->setValue(50);

        verticalLayout_3->addWidget(percentStartSpin);

        label_30 = new QLabel(combine_tab);
        label_30->setObjectName(QString::fromUtf8("label_30"));

        verticalLayout_3->addWidget(label_30);

        percentEndSpin = new QSpinBox(combine_tab);
        percentEndSpin->setObjectName(QString::fromUtf8("percentEndSpin"));
        percentEndSpin->setMaximum(100);
        percentEndSpin->setValue(50);

        verticalLayout_3->addWidget(percentEndSpin);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_4);

        settings_tab_widget->addTab(combine_tab, QString());
        colour_tab = new QWidget();
        colour_tab->setObjectName(QString::fromUtf8("colour_tab"));
        gridLayout_7 = new QGridLayout(colour_tab);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        selectColour = new QPushButton(colour_tab);
        selectColour->setObjectName(QString::fromUtf8("selectColour"));

        gridLayout_7->addWidget(selectColour, 6, 0, 1, 2);

        spinGreen = new QSpinBox(colour_tab);
        spinGreen->setObjectName(QString::fromUtf8("spinGreen"));
        spinGreen->setMaximum(255);
        spinGreen->setValue(255);

        gridLayout_7->addWidget(spinGreen, 3, 1, 1, 1);

        label_10 = new QLabel(colour_tab);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setMaximumSize(QSize(16777215, 35));
        label_10->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        label_10->setWordWrap(true);

        gridLayout_7->addWidget(label_10, 1, 0, 1, 2);

        spinRed = new QSpinBox(colour_tab);
        spinRed->setObjectName(QString::fromUtf8("spinRed"));
        spinRed->setMaximum(255);
        spinRed->setValue(255);

        gridLayout_7->addWidget(spinRed, 2, 1, 1, 1);

        spinBlue = new QSpinBox(colour_tab);
        spinBlue->setObjectName(QString::fromUtf8("spinBlue"));
        spinBlue->setMaximum(255);
        spinBlue->setValue(255);

        gridLayout_7->addWidget(spinBlue, 4, 1, 1, 1);

        label_34 = new QLabel(colour_tab);
        label_34->setObjectName(QString::fromUtf8("label_34"));

        gridLayout_7->addWidget(label_34, 2, 0, 1, 1);

        label_36 = new QLabel(colour_tab);
        label_36->setObjectName(QString::fromUtf8("label_36"));

        gridLayout_7->addWidget(label_36, 4, 0, 1, 1);

        label_35 = new QLabel(colour_tab);
        label_35->setObjectName(QString::fromUtf8("label_35"));

        gridLayout_7->addWidget(label_35, 3, 0, 1, 1);

        label_14 = new QLabel(colour_tab);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setMaximumSize(QSize(16777215, 35));

        gridLayout_7->addWidget(label_14, 5, 0, 1, 2);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_7->addItem(verticalSpacer_3, 7, 0, 1, 2);

        settings_tab_widget->addTab(colour_tab, QString());
        stack_tab = new QWidget();
        stack_tab->setObjectName(QString::fromUtf8("stack_tab"));
        gridLayout_8 = new QGridLayout(stack_tab);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        sFromImTxt = new QTextBrowser(stack_tab);
        sFromImTxt->setObjectName(QString::fromUtf8("sFromImTxt"));

        gridLayout_8->addWidget(sFromImTxt, 3, 0, 1, 1);

        sFromImPushbutton = new QPushButton(stack_tab);
        sFromImPushbutton->setObjectName(QString::fromUtf8("sFromImPushbutton"));

        gridLayout_8->addWidget(sFromImPushbutton, 1, 0, 1, 1);

        label_37 = new QLabel(stack_tab);
        label_37->setObjectName(QString::fromUtf8("label_37"));

        gridLayout_8->addWidget(label_37, 2, 0, 1, 1);

        stack_label = new QLabel(stack_tab);
        stack_label->setObjectName(QString::fromUtf8("stack_label"));
        stack_label->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);
        stack_label->setWordWrap(true);

        gridLayout_8->addWidget(stack_label, 0, 0, 1, 1);

        settings_tab_widget->addTab(stack_tab, QString());

        verticalLayout_2->addWidget(settings_tab_widget);


        verticalLayout->addWidget(dockWidgetFrame);

        envSettings->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(Qt::RightDockWidgetArea, envSettings);

        retranslateUi(MainWindow);

        settings_tab_widget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
#if QT_CONFIG(accessibility)
        envSettings->setAccessibleName(QCoreApplication::translate("MainWindow", "Enivronmental Settings", nullptr));
#endif // QT_CONFIG(accessibility)
        instructions_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        output_label->setText(QCoreApplication::translate("MainWindow", "Output:", nullptr));
        save_images_checkbox->setText(QCoreApplication::translate("MainWindow", "Save images", nullptr));
        label_7->setText(QCoreApplication::translate("MainWindow", "Save Location:", nullptr));
        path->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:7.8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p></body></html>", nullptr));
        change_path->setText(QCoreApplication::translate("MainWindow", "Change...", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "Generate for:", nullptr));
        label_26->setText(QCoreApplication::translate("MainWindow", "Image size:", nullptr));
        label_32->setText(QCoreApplication::translate("MainWindow", "Environment type:", nullptr));
        display_label->setText(QCoreApplication::translate("MainWindow", "Display:", nullptr));
        resize->setText(QCoreApplication::translate("MainWindow", "Resize image", nullptr));
        settings_tab_widget->setTabText(settings_tab_widget->indexOf(output_tab), QCoreApplication::translate("MainWindow", "Output", nullptr));
        russell_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Seeds:", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "Max colour velocity", nullptr));
        label_13->setText(QCoreApplication::translate("MainWindow", "Speed factor", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Max velocity", nullptr));
        periodic->setText(QCoreApplication::translate("MainWindow", "Toroidal", nullptr));
        label_12->setText(QCoreApplication::translate("MainWindow", "Overlap blur factor", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Max size velocity", nullptr));
        label_11->setText(QCoreApplication::translate("MainWindow", "Number", nullptr));
        blurChk->setText(QCoreApplication::translate("MainWindow", "Universal Blur", nullptr));
        label_8->setText(QCoreApplication::translate("MainWindow", "Background:", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Max size", nullptr));
        label_9->setText(QCoreApplication::translate("MainWindow", "Convergence", nullptr));
        settings_tab_widget->setTabText(settings_tab_widget->indexOf(russell_tab), QCoreApplication::translate("MainWindow", "Dynamic 1", nullptr));
        label_44->setText(QCoreApplication::translate("MainWindow", "Speed Factor", nullptr));
        label_45->setText(QCoreApplication::translate("MainWindow", "Maximum Acceleration", nullptr));
        label_46->setText(QCoreApplication::translate("MainWindow", "Max Size Acceleration", nullptr));
        label_47->setText(QCoreApplication::translate("MainWindow", "Max Tightness Acceleration", nullptr));
        label_48->setText(QCoreApplication::translate("MainWindow", "Max Colour Acceleration", nullptr));
        label_49->setText(QCoreApplication::translate("MainWindow", "Apply accelerations every ...", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "Maximum Colour Velocity", nullptr));
        label_23->setText(QCoreApplication::translate("MainWindow", "Maximum Tightness", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "Minimum Tightness", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "Maximum Velocity", nullptr));
        label_21->setText(QCoreApplication::translate("MainWindow", "Maximum Tightness Velocity", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "Object Count", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "Maximum Object Size", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "Minimum Object Size", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "Maximum Size Velocity", nullptr));
        m_environment_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        settings_tab_widget->setTabText(settings_tab_widget->indexOf(mark_tab), QCoreApplication::translate("MainWindow", "Dynamic 2", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "Minimum value", nullptr));
        label_25->setText(QCoreApplication::translate("MainWindow", "Maximum value", nullptr));
        noise_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        settings_tab_widget->setTabText(settings_tab_widget->indexOf(noise_tab), QCoreApplication::translate("MainWindow", "Noise", nullptr));
        combine_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        label_27->setText(QCoreApplication::translate("MainWindow", "Stack one:", nullptr));
        pushButtonStackOne->setText(QCoreApplication::translate("MainWindow", "Change...", nullptr));
        label_28->setText(QCoreApplication::translate("MainWindow", "Stack two:", nullptr));
        pushButtonStackTwo->setText(QCoreApplication::translate("MainWindow", "Change...", nullptr));
        label_31->setText(QCoreApplication::translate("MainWindow", "Start slice:", nullptr));
        label_29->setText(QCoreApplication::translate("MainWindow", "Percent influence stack one (start combine):", nullptr));
        label_30->setText(QCoreApplication::translate("MainWindow", "Percent influence stack one (end combine):", nullptr));
        settings_tab_widget->setTabText(settings_tab_widget->indexOf(combine_tab), QCoreApplication::translate("MainWindow", "Combine stacks", nullptr));
        selectColour->setText(QCoreApplication::translate("MainWindow", "Select...", nullptr));
        label_10->setText(QCoreApplication::translate("MainWindow", "This will create a stack of colour images. Enter RGB levels:", nullptr));
        label_34->setText(QCoreApplication::translate("MainWindow", "Red", nullptr));
        label_36->setText(QCoreApplication::translate("MainWindow", "Blue", nullptr));
        label_35->setText(QCoreApplication::translate("MainWindow", "Green", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "Or pick a colour:", nullptr));
        settings_tab_widget->setTabText(settings_tab_widget->indexOf(colour_tab), QCoreApplication::translate("MainWindow", "Colour", nullptr));
        sFromImPushbutton->setText(QCoreApplication::translate("MainWindow", "Choose image", nullptr));
        label_37->setText(QCoreApplication::translate("MainWindow", "Current:", nullptr));
        stack_label->setText(QCoreApplication::translate("MainWindow", "This will create a stack from a single image.", nullptr));
        settings_tab_widget->setTabText(settings_tab_widget->indexOf(stack_tab), QCoreApplication::translate("MainWindow", "Stack from image", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
