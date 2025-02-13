/********************************************************************************
** Form generated from reading UI file 'genomecomparison.ui'
**
** Created by: Qt User Interface Compiler version 5.15.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENOMECOMPARISON_H
#define UI_GENOMECOMPARISON_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GenomeComparison
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *genomeWordsLineEdit;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QCheckBox *autoCheckbox;
    QCheckBox *modalOnlyCheckbox;
    QTableWidget *genomeTableWidget;
    QTableWidget *compareTableWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *compareButton;
    QPushButton *autoButton;
    QPushButton *allButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *deleteButton;
    QPushButton *resetButton;

    void setupUi(QWidget *GenomeComparison)
    {
        if (GenomeComparison->objectName().isEmpty())
            GenomeComparison->setObjectName(QString::fromUtf8("GenomeComparison"));
        GenomeComparison->resize(938, 400);
        verticalLayout_2 = new QVBoxLayout(GenomeComparison);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        genomeWordsLineEdit = new QLineEdit(GenomeComparison);
        genomeWordsLineEdit->setObjectName(QString::fromUtf8("genomeWordsLineEdit"));

        horizontalLayout_2->addWidget(genomeWordsLineEdit);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label = new QLabel(GenomeComparison);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        horizontalLayout_4->addWidget(label);

        autoCheckbox = new QCheckBox(GenomeComparison);
        autoCheckbox->setObjectName(QString::fromUtf8("autoCheckbox"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(autoCheckbox->sizePolicy().hasHeightForWidth());
        autoCheckbox->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(autoCheckbox);

        modalOnlyCheckbox = new QCheckBox(GenomeComparison);
        modalOnlyCheckbox->setObjectName(QString::fromUtf8("modalOnlyCheckbox"));
        sizePolicy.setHeightForWidth(modalOnlyCheckbox->sizePolicy().hasHeightForWidth());
        modalOnlyCheckbox->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(modalOnlyCheckbox);


        verticalLayout_2->addLayout(horizontalLayout_4);

        genomeTableWidget = new QTableWidget(GenomeComparison);
        if (genomeTableWidget->columnCount() < 74)
            genomeTableWidget->setColumnCount(74);
        genomeTableWidget->setObjectName(QString::fromUtf8("genomeTableWidget"));
        genomeTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        genomeTableWidget->setSortingEnabled(true);
        genomeTableWidget->setColumnCount(74);
        genomeTableWidget->horizontalHeader()->setMinimumSectionSize(20);
        genomeTableWidget->horizontalHeader()->setDefaultSectionSize(20);
        genomeTableWidget->verticalHeader()->setVisible(false);
        genomeTableWidget->verticalHeader()->setMinimumSectionSize(20);
        genomeTableWidget->verticalHeader()->setDefaultSectionSize(20);
        genomeTableWidget->verticalHeader()->setHighlightSections(false);
        genomeTableWidget->verticalHeader()->setStretchLastSection(false);

        verticalLayout_2->addWidget(genomeTableWidget);

        compareTableWidget = new QTableWidget(GenomeComparison);
        if (compareTableWidget->columnCount() < 71)
            compareTableWidget->setColumnCount(71);
        compareTableWidget->setObjectName(QString::fromUtf8("compareTableWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(compareTableWidget->sizePolicy().hasHeightForWidth());
        compareTableWidget->setSizePolicy(sizePolicy1);
        compareTableWidget->setMinimumSize(QSize(0, 63));
        compareTableWidget->setBaseSize(QSize(0, 63));
        compareTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        compareTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        compareTableWidget->setColumnCount(71);
        compareTableWidget->horizontalHeader()->setVisible(false);
        compareTableWidget->horizontalHeader()->setMinimumSectionSize(20);
        compareTableWidget->horizontalHeader()->setDefaultSectionSize(20);
        compareTableWidget->horizontalHeader()->setHighlightSections(false);
        compareTableWidget->verticalHeader()->setVisible(false);
        compareTableWidget->verticalHeader()->setMinimumSectionSize(20);
        compareTableWidget->verticalHeader()->setDefaultSectionSize(20);
        compareTableWidget->verticalHeader()->setHighlightSections(false);

        verticalLayout_2->addWidget(compareTableWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        compareButton = new QPushButton(GenomeComparison);
        compareButton->setObjectName(QString::fromUtf8("compareButton"));
        compareButton->setEnabled(false);

        horizontalLayout->addWidget(compareButton);

        autoButton = new QPushButton(GenomeComparison);
        autoButton->setObjectName(QString::fromUtf8("autoButton"));
        autoButton->setCheckable(true);
        autoButton->setChecked(true);

        horizontalLayout->addWidget(autoButton);

        allButton = new QPushButton(GenomeComparison);
        allButton->setObjectName(QString::fromUtf8("allButton"));
        allButton->setCheckable(true);
        allButton->setChecked(true);

        horizontalLayout->addWidget(allButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        deleteButton = new QPushButton(GenomeComparison);
        deleteButton->setObjectName(QString::fromUtf8("deleteButton"));
        deleteButton->setEnabled(false);

        horizontalLayout->addWidget(deleteButton);

        resetButton = new QPushButton(GenomeComparison);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        resetButton->setEnabled(false);

        horizontalLayout->addWidget(resetButton);


        verticalLayout_2->addLayout(horizontalLayout);


        retranslateUi(GenomeComparison);

        QMetaObject::connectSlotsByName(GenomeComparison);
    } // setupUi

    void retranslateUi(QWidget *GenomeComparison)
    {
        GenomeComparison->setWindowTitle(QCoreApplication::translate("GenomeComparison", "Form", nullptr));
        label->setText(QCoreApplication::translate("GenomeComparison", "Columns: E = environment colour;  S = species ID; SC = species colour; EF = environmental fitness; F = fitness;  WnC = colour for word n.", nullptr));
        autoCheckbox->setText(QCoreApplication::translate("GenomeComparison", "Auto compare", nullptr));
        modalOnlyCheckbox->setText(QCoreApplication::translate("GenomeComparison", "Modal genome only", nullptr));
        compareButton->setText(QCoreApplication::translate("GenomeComparison", "Compare", nullptr));
        autoButton->setText(QCoreApplication::translate("GenomeComparison", "Auto Compare ON", nullptr));
        allButton->setText(QCoreApplication::translate("GenomeComparison", "Add all ON", nullptr));
        deleteButton->setText(QCoreApplication::translate("GenomeComparison", "Delete", nullptr));
        resetButton->setText(QCoreApplication::translate("GenomeComparison", "Reset All", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GenomeComparison: public Ui_GenomeComparison {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENOMECOMPARISON_H
