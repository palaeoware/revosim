/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Tue 20. Mar 14:16:10 2012
**      by: Qt User Interface Compiler version 4.8.0
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
#include <QtGui/QTextBrowser>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSave_settings;
    QAction *actionResize_environment;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QGraphicsView *enviroView;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *envSettings;
    QWidget *dockWidgetContents_3;
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;
    QLabel *label;
    QLabel *label_11;
    QSpinBox *numSeed;
    QLabel *label_2;
    QSpinBox *sMax;
    QLabel *label_3;
    QSpinBox *sMin;
    QLabel *label_4;
    QSpinBox *velMax;
    QLabel *label_6;
    QSpinBox *cvelMax;
    QLabel *label_8;
    QCheckBox *periodic;
    QLabel *label_9;
    QDoubleSpinBox *convergeAt;
    QCheckBox *checkBox;
    QPushButton *colourbutton;
    QLabel *label_5;
    QSpinBox *numGenerations;
    QPushButton *generateEnvironment;
    QPushButton *pushButton;
    QLabel *label_7;
    QTextBrowser *textBrowser_2;
    QLabel *label_10;
    QCheckBox *checkBox_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(662, 702);
        actionSave_settings = new QAction(MainWindow);
        actionSave_settings->setObjectName(QString::fromUtf8("actionSave_settings"));
        actionResize_environment = new QAction(MainWindow);
        actionResize_environment->setObjectName(QString::fromUtf8("actionResize_environment"));
        actionResize_environment->setCheckable(true);
        actionResize_environment->setChecked(true);
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
        menuBar->setGeometry(QRect(0, 0, 662, 21));
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
        envSettings->setMinimumSize(QSize(285, 610));
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName(QString::fromUtf8("dockWidgetContents_3"));
        dockWidgetContents_3->setMaximumSize(QSize(16777215, 600));
        gridLayout = new QGridLayout(dockWidgetContents_3);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        textBrowser = new QTextBrowser(dockWidgetContents_3);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setMaximumSize(QSize(16777215, 75));

        gridLayout->addWidget(textBrowser, 0, 0, 1, 2);

        label = new QLabel(dockWidgetContents_3);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        label_11 = new QLabel(dockWidgetContents_3);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout->addWidget(label_11, 2, 0, 1, 1);

        numSeed = new QSpinBox(dockWidgetContents_3);
        numSeed->setObjectName(QString::fromUtf8("numSeed"));
        numSeed->setMinimum(1);
        numSeed->setMaximum(1000);
        numSeed->setValue(4);

        gridLayout->addWidget(numSeed, 2, 1, 1, 1);

        label_2 = new QLabel(dockWidgetContents_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        sMax = new QSpinBox(dockWidgetContents_3);
        sMax->setObjectName(QString::fromUtf8("sMax"));
        sMax->setMinimum(2);
        sMax->setValue(14);

        gridLayout->addWidget(sMax, 3, 1, 1, 1);

        label_3 = new QLabel(dockWidgetContents_3);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        sMin = new QSpinBox(dockWidgetContents_3);
        sMin->setObjectName(QString::fromUtf8("sMin"));
        sMin->setMinimum(1);
        sMin->setValue(5);

        gridLayout->addWidget(sMin, 4, 1, 1, 1);

        label_4 = new QLabel(dockWidgetContents_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 5, 0, 1, 1);

        velMax = new QSpinBox(dockWidgetContents_3);
        velMax->setObjectName(QString::fromUtf8("velMax"));
        velMax->setValue(5);

        gridLayout->addWidget(velMax, 5, 1, 1, 1);

        label_6 = new QLabel(dockWidgetContents_3);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 6, 0, 1, 1);

        cvelMax = new QSpinBox(dockWidgetContents_3);
        cvelMax->setObjectName(QString::fromUtf8("cvelMax"));
        cvelMax->setValue(5);

        gridLayout->addWidget(cvelMax, 6, 1, 1, 1);

        label_8 = new QLabel(dockWidgetContents_3);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 7, 0, 1, 1);

        periodic = new QCheckBox(dockWidgetContents_3);
        periodic->setObjectName(QString::fromUtf8("periodic"));
        periodic->setChecked(true);

        gridLayout->addWidget(periodic, 7, 1, 1, 1);

        label_9 = new QLabel(dockWidgetContents_3);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 8, 0, 1, 1);

        convergeAt = new QDoubleSpinBox(dockWidgetContents_3);
        convergeAt->setObjectName(QString::fromUtf8("convergeAt"));
        convergeAt->setDecimals(4);
        convergeAt->setMinimum(0);
        convergeAt->setMaximum(1);
        convergeAt->setSingleStep(0.001);

        gridLayout->addWidget(convergeAt, 8, 1, 1, 1);

        checkBox = new QCheckBox(dockWidgetContents_3);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout->addWidget(checkBox, 9, 0, 1, 1);

        colourbutton = new QPushButton(dockWidgetContents_3);
        colourbutton->setObjectName(QString::fromUtf8("colourbutton"));

        gridLayout->addWidget(colourbutton, 9, 1, 1, 1);

        label_5 = new QLabel(dockWidgetContents_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 10, 0, 1, 1);

        numGenerations = new QSpinBox(dockWidgetContents_3);
        numGenerations->setObjectName(QString::fromUtf8("numGenerations"));
        numGenerations->setMinimum(1);
        numGenerations->setMaximum(100000);
        numGenerations->setValue(500);

        gridLayout->addWidget(numGenerations, 10, 1, 1, 1);

        generateEnvironment = new QPushButton(dockWidgetContents_3);
        generateEnvironment->setObjectName(QString::fromUtf8("generateEnvironment"));

        gridLayout->addWidget(generateEnvironment, 12, 0, 1, 1);

        pushButton = new QPushButton(dockWidgetContents_3);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 12, 1, 1, 1);

        label_7 = new QLabel(dockWidgetContents_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 13, 0, 1, 1);

        textBrowser_2 = new QTextBrowser(dockWidgetContents_3);
        textBrowser_2->setObjectName(QString::fromUtf8("textBrowser_2"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textBrowser_2->sizePolicy().hasHeightForWidth());
        textBrowser_2->setSizePolicy(sizePolicy);
        textBrowser_2->setMaximumSize(QSize(150, 50));

        gridLayout->addWidget(textBrowser_2, 13, 1, 1, 1);

        label_10 = new QLabel(dockWidgetContents_3);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 14, 0, 1, 1);

        checkBox_2 = new QCheckBox(dockWidgetContents_3);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setChecked(false);

        gridLayout->addWidget(checkBox_2, 14, 1, 1, 1);

        envSettings->setWidget(dockWidgetContents_3);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), envSettings);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionSave_settings);
        menuFile->addAction(actionResize_environment);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        actionSave_settings->setText(QApplication::translate("MainWindow", "Save settings", 0, QApplication::UnicodeUTF8));
        actionResize_environment->setText(QApplication::translate("MainWindow", "Resize environment", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        envSettings->setAccessibleName(QApplication::translate("MainWindow", "Enivronmental Settings", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        textBrowser->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Please enter the settings for the dynamic environment below. Most are obvious. Convergence sets the amount of smoothing between spots and background.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Seeds:", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Number", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Max size", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Max size velocity", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Max velocity", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Max colour velocity", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Background:", 0, QApplication::UnicodeUTF8));
        periodic->setText(QApplication::translate("MainWindow", "Periodic", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "Convergence", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("MainWindow", "Background colour", 0, QApplication::UnicodeUTF8));
        colourbutton->setText(QApplication::translate("MainWindow", "Pick", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Generate for...", 0, QApplication::UnicodeUTF8));
        generateEnvironment->setText(QApplication::translate("MainWindow", "Go!", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MainWindow", "Save location...", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Current:", 0, QApplication::UnicodeUTF8));
        textBrowser_2->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">C:/tmp/</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "Save images?", 0, QApplication::UnicodeUTF8));
        checkBox_2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
