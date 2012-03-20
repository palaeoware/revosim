/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created: Thu 8. Mar 11:14:21 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Settings
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *SpinBoxGridX;
    QLabel *label_2;
    QSpinBox *SpinBoxGridY;
    QLabel *label_3;
    QSpinBox *SpinBoxSlots;
    QLabel *label_4;
    QSpinBox *SpinBoxFitnessTarget;
    QLabel *label_5;
    QSpinBox *SpinBoxSettleTolerance;
    QLabel *label_8;
    QSpinBox *SpinBoxBreedThreshold;
    QLabel *label_12;
    QSpinBox *SpinBoxStartAge;
    QLabel *label_6;
    QSpinBox *SpinBoxDispersal;
    QLabel *label_7;
    QSpinBox *SpinBoxFood;
    QLabel *label_9;
    QSpinBox *SpinBoxBreedCost;
    QLabel *label_11;
    QSpinBox *SpinBoxMutationChance;
    QLabel *label_10;
    QSpinBox *SpinBoxDifferenceBreed;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Settings)
    {
        if (Settings->objectName().isEmpty())
            Settings->setObjectName(QString::fromUtf8("Settings"));
        Settings->resize(306, 218);
        verticalLayout = new QVBoxLayout(Settings);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(Settings);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        SpinBoxGridX = new QSpinBox(Settings);
        SpinBoxGridX->setObjectName(QString::fromUtf8("SpinBoxGridX"));
        SpinBoxGridX->setEnabled(true);
        SpinBoxGridX->setMinimum(1);
        SpinBoxGridX->setMaximum(256);

        gridLayout->addWidget(SpinBoxGridX, 0, 1, 1, 1);

        label_2 = new QLabel(Settings);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 2, 1, 1);

        SpinBoxGridY = new QSpinBox(Settings);
        SpinBoxGridY->setObjectName(QString::fromUtf8("SpinBoxGridY"));
        SpinBoxGridY->setEnabled(true);
        SpinBoxGridY->setMinimum(1);
        SpinBoxGridY->setMaximum(256);

        gridLayout->addWidget(SpinBoxGridY, 0, 3, 1, 1);

        label_3 = new QLabel(Settings);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        SpinBoxSlots = new QSpinBox(Settings);
        SpinBoxSlots->setObjectName(QString::fromUtf8("SpinBoxSlots"));
        SpinBoxSlots->setEnabled(true);
        SpinBoxSlots->setMinimum(1);
        SpinBoxSlots->setMaximum(256);

        gridLayout->addWidget(SpinBoxSlots, 1, 1, 1, 1);

        label_4 = new QLabel(Settings);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 1, 2, 1, 1);

        SpinBoxFitnessTarget = new QSpinBox(Settings);
        SpinBoxFitnessTarget->setObjectName(QString::fromUtf8("SpinBoxFitnessTarget"));
        SpinBoxFitnessTarget->setEnabled(true);
        SpinBoxFitnessTarget->setMinimum(1);
        SpinBoxFitnessTarget->setMaximum(96);

        gridLayout->addWidget(SpinBoxFitnessTarget, 1, 3, 1, 1);

        label_5 = new QLabel(Settings);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        SpinBoxSettleTolerance = new QSpinBox(Settings);
        SpinBoxSettleTolerance->setObjectName(QString::fromUtf8("SpinBoxSettleTolerance"));
        SpinBoxSettleTolerance->setMinimum(1);
        SpinBoxSettleTolerance->setMaximum(30);

        gridLayout->addWidget(SpinBoxSettleTolerance, 2, 1, 1, 1);

        label_8 = new QLabel(Settings);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 2, 2, 1, 1);

        SpinBoxBreedThreshold = new QSpinBox(Settings);
        SpinBoxBreedThreshold->setObjectName(QString::fromUtf8("SpinBoxBreedThreshold"));
        SpinBoxBreedThreshold->setEnabled(true);
        SpinBoxBreedThreshold->setMinimum(1);
        SpinBoxBreedThreshold->setMaximum(5000);
        SpinBoxBreedThreshold->setValue(250);

        gridLayout->addWidget(SpinBoxBreedThreshold, 2, 3, 1, 1);

        label_12 = new QLabel(Settings);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout->addWidget(label_12, 3, 0, 1, 1);

        SpinBoxStartAge = new QSpinBox(Settings);
        SpinBoxStartAge->setObjectName(QString::fromUtf8("SpinBoxStartAge"));
        SpinBoxStartAge->setMinimum(1);
        SpinBoxStartAge->setMaximum(1000);

        gridLayout->addWidget(SpinBoxStartAge, 3, 1, 1, 1);

        label_6 = new QLabel(Settings);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 3, 2, 1, 1);

        SpinBoxDispersal = new QSpinBox(Settings);
        SpinBoxDispersal->setObjectName(QString::fromUtf8("SpinBoxDispersal"));
        SpinBoxDispersal->setMinimum(1);
        SpinBoxDispersal->setMaximum(200);

        gridLayout->addWidget(SpinBoxDispersal, 3, 3, 1, 1);

        label_7 = new QLabel(Settings);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 4, 0, 1, 1);

        SpinBoxFood = new QSpinBox(Settings);
        SpinBoxFood->setObjectName(QString::fromUtf8("SpinBoxFood"));
        SpinBoxFood->setMinimum(1);
        SpinBoxFood->setMaximum(20000);

        gridLayout->addWidget(SpinBoxFood, 4, 1, 1, 1);

        label_9 = new QLabel(Settings);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 4, 2, 1, 1);

        SpinBoxBreedCost = new QSpinBox(Settings);
        SpinBoxBreedCost->setObjectName(QString::fromUtf8("SpinBoxBreedCost"));
        SpinBoxBreedCost->setMinimum(1);
        SpinBoxBreedCost->setMaximum(10000);

        gridLayout->addWidget(SpinBoxBreedCost, 4, 3, 1, 1);

        label_11 = new QLabel(Settings);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout->addWidget(label_11, 5, 0, 1, 1);

        SpinBoxMutationChance = new QSpinBox(Settings);
        SpinBoxMutationChance->setObjectName(QString::fromUtf8("SpinBoxMutationChance"));
        SpinBoxMutationChance->setMaximum(255);

        gridLayout->addWidget(SpinBoxMutationChance, 5, 1, 1, 1);

        label_10 = new QLabel(Settings);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 5, 2, 1, 1);

        SpinBoxDifferenceBreed = new QSpinBox(Settings);
        SpinBoxDifferenceBreed->setObjectName(QString::fromUtf8("SpinBoxDifferenceBreed"));
        SpinBoxDifferenceBreed->setMinimum(1);
        SpinBoxDifferenceBreed->setMaximum(31);

        gridLayout->addWidget(SpinBoxDifferenceBreed, 5, 3, 1, 1);


        verticalLayout->addLayout(gridLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(Settings);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(Settings);
        QObject::connect(buttonBox, SIGNAL(accepted()), Settings, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Settings, SLOT(reject()));

        QMetaObject::connectSlotsByName(Settings);
    } // setupUi

    void retranslateUi(QDialog *Settings)
    {
        Settings->setWindowTitle(QApplication::translate("Settings", "Settings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Settings", "Grid X", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Settings", "Grid Y", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Settings", "Slots", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Settings", "Fitness target", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Settings", "Settle tolerance", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("Settings", "Breed Threshold", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("Settings", "Start Age", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Settings", "Dispersal", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Settings", "Food", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("Settings", "Breed Cost", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Settings", "Chance of Mutation", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("Settings", "Max Diff to Breed", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
