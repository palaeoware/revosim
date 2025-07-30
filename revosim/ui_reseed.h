/********************************************************************************
** Form generated from reading UI file 'reseed.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESEED_H
#define UI_RESEED_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Reseed
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QLabel *label_4;
    QHBoxLayout *dualReseedLayout;
    QCheckBox *checkBoxDualReseed;
    QRadioButton *identicalRadio;
    QRadioButton *randomRadio;
    QSpacerItem *verticalSpacer;
    QLabel *label_8;
    QCheckBox *checkBoxGenetic;
    QLabel *label_9;
    QHBoxLayout *tieringRadioLayout;
    QRadioButton *radio1Tier;
    QRadioButton *radio3Tier;
    QRadioButton *radio5Tier;
    QLabel *label_5;
    QLabel *label_6;
    QCheckBox *CheckBoxReseedSession;
    QLabel *label_7;
    QHBoxLayout *hexRadioLayout;
    QRadioButton *hexRadio;
    QRadioButton *binaryRadio;
    QLabel *label;
    QPlainTextEdit *genomeTextEdit;
    QLabel *label_2;
    QVBoxLayout *genomesLayout;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Reseed)
    {
        if (Reseed->objectName().isEmpty())
            Reseed->setObjectName("Reseed");
        Reseed->resize(717, 788);
        verticalLayout = new QVBoxLayout(Reseed);
        verticalLayout->setObjectName("verticalLayout");
        label_3 = new QLabel(Reseed);
        label_3->setObjectName("label_3");
        QFont font;
        font.setBold(true);
        label_3->setFont(font);

        verticalLayout->addWidget(label_3);

        label_4 = new QLabel(Reseed);
        label_4->setObjectName("label_4");
        label_4->setWordWrap(true);

        verticalLayout->addWidget(label_4);

        dualReseedLayout = new QHBoxLayout();
        dualReseedLayout->setObjectName("dualReseedLayout");
        checkBoxDualReseed = new QCheckBox(Reseed);
        checkBoxDualReseed->setObjectName("checkBoxDualReseed");

        dualReseedLayout->addWidget(checkBoxDualReseed);

        identicalRadio = new QRadioButton(Reseed);
        identicalRadio->setObjectName("identicalRadio");

        dualReseedLayout->addWidget(identicalRadio);

        randomRadio = new QRadioButton(Reseed);
        randomRadio->setObjectName("randomRadio");

        dualReseedLayout->addWidget(randomRadio);


        verticalLayout->addLayout(dualReseedLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        label_8 = new QLabel(Reseed);
        label_8->setObjectName("label_8");

        verticalLayout->addWidget(label_8);

        checkBoxGenetic = new QCheckBox(Reseed);
        checkBoxGenetic->setObjectName("checkBoxGenetic");

        verticalLayout->addWidget(checkBoxGenetic);

        label_9 = new QLabel(Reseed);
        label_9->setObjectName("label_9");
        label_9->setFont(font);

        verticalLayout->addWidget(label_9);

        tieringRadioLayout = new QHBoxLayout();
        tieringRadioLayout->setObjectName("tieringRadioLayout");
        radio1Tier = new QRadioButton(Reseed);
        radio1Tier->setObjectName("radio1Tier");

        tieringRadioLayout->addWidget(radio1Tier);

        radio3Tier = new QRadioButton(Reseed);
        radio3Tier->setObjectName("radio3Tier");

        tieringRadioLayout->addWidget(radio3Tier);

        radio5Tier = new QRadioButton(Reseed);
        radio5Tier->setObjectName("radio5Tier");

        tieringRadioLayout->addWidget(radio5Tier);


        verticalLayout->addLayout(tieringRadioLayout);

        label_5 = new QLabel(Reseed);
        label_5->setObjectName("label_5");
        label_5->setFont(font);

        verticalLayout->addWidget(label_5);

        label_6 = new QLabel(Reseed);
        label_6->setObjectName("label_6");
        label_6->setWordWrap(true);

        verticalLayout->addWidget(label_6);

        CheckBoxReseedSession = new QCheckBox(Reseed);
        CheckBoxReseedSession->setObjectName("CheckBoxReseedSession");

        verticalLayout->addWidget(CheckBoxReseedSession);

        label_7 = new QLabel(Reseed);
        label_7->setObjectName("label_7");

        verticalLayout->addWidget(label_7);

        hexRadioLayout = new QHBoxLayout();
        hexRadioLayout->setObjectName("hexRadioLayout");
        hexRadio = new QRadioButton(Reseed);
        hexRadio->setObjectName("hexRadio");

        hexRadioLayout->addWidget(hexRadio);

        binaryRadio = new QRadioButton(Reseed);
        binaryRadio->setObjectName("binaryRadio");

        hexRadioLayout->addWidget(binaryRadio);


        verticalLayout->addLayout(hexRadioLayout);

        label = new QLabel(Reseed);
        label->setObjectName("label");

        verticalLayout->addWidget(label);

        genomeTextEdit = new QPlainTextEdit(Reseed);
        genomeTextEdit->setObjectName("genomeTextEdit");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(genomeTextEdit->sizePolicy().hasHeightForWidth());
        genomeTextEdit->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(genomeTextEdit);

        label_2 = new QLabel(Reseed);
        label_2->setObjectName("label_2");

        verticalLayout->addWidget(label_2);

        genomesLayout = new QVBoxLayout();
        genomesLayout->setObjectName("genomesLayout");

        verticalLayout->addLayout(genomesLayout);

        buttonBox = new QDialogButtonBox(Reseed);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(Reseed);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, Reseed, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, Reseed, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(Reseed);
    } // setupUi

    void retranslateUi(QDialog *Reseed)
    {
        Reseed->setWindowTitle(QCoreApplication::translate("Reseed", "REvoSim - reseed simulation", nullptr));
        label_3->setText(QCoreApplication::translate("Reseed", "Dual reseed", nullptr));
        label_4->setText(QCoreApplication::translate("Reseed", "REvoSim can be reseeded with two individuals at either side of the grid to, for example, compete different genomes. These can be known genomes, indetical random genomes, or different random genomes. Note that the last option will prevent the species system working correctly.", nullptr));
        checkBoxDualReseed->setText(QCoreApplication::translate("Reseed", "Enable dual reseed", nullptr));
        identicalRadio->setText(QCoreApplication::translate("Reseed", "Identical genomes", nullptr));
        randomRadio->setText(QCoreApplication::translate("Reseed", "Random genomes", nullptr));
        label_8->setText(QCoreApplication::translate("Reseed", "<html><head/><body><p><span style=\" font-weight:600;\">Random Reseed</span></p></body></html>", nullptr));
        checkBoxGenetic->setText(QCoreApplication::translate("Reseed", "Try random genomes before genetic algorithm", nullptr));
        label_9->setText(QCoreApplication::translate("Reseed", "Trophic Reseed", nullptr));
        radio1Tier->setText(QCoreApplication::translate("Reseed", "Default (1 tier)", nullptr));
        radio3Tier->setText(QCoreApplication::translate("Reseed", "3 Tier", nullptr));
        radio5Tier->setText(QCoreApplication::translate("Reseed", "5 Tier", nullptr));
        label_5->setText(QCoreApplication::translate("Reseed", "Reseed with known genome", nullptr));
        label_6->setText(QCoreApplication::translate("Reseed", "Use this dialogue to reseed REvoSim with a known genome (single, or dual, depending on above settings).", nullptr));
        CheckBoxReseedSession->setText(QCoreApplication::translate("Reseed", "Enable reseed with known genome", nullptr));
        label_7->setText(QCoreApplication::translate("Reseed", "Display format:", nullptr));
        hexRadio->setText(QCoreApplication::translate("Reseed", "Hexadecimal", nullptr));
        binaryRadio->setText(QCoreApplication::translate("Reseed", "Binary", nullptr));
        label->setText(QCoreApplication::translate("Reseed", "Please enter the reseed genome:", nullptr));
        label_2->setText(QCoreApplication::translate("Reseed", "Or choose one of those from the genome comparison dock (a max  of 10 are displayed):", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Reseed: public Ui_Reseed {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESEED_H
