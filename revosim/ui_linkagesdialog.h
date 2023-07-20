/********************************************************************************
** Form generated from reading UI file 'linkagesdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LINKAGESDIALOG_H
#define UI_LINKAGESDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_linkagesDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *topLabel;
    QLabel *label;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *comboBox;
    QPushButton *loadImages;
    QTextBrowser *imageListDisplay;
    QLabel *label_3;
    QGridLayout *gridLayout;
    QCheckBox *interpolateImages;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QSpinBox *refreshRate;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *linkagesDialog)
    {
        if (linkagesDialog->objectName().isEmpty())
            linkagesDialog->setObjectName(QString::fromUtf8("linkagesDialog"));
        linkagesDialog->resize(486, 567);
        verticalLayout = new QVBoxLayout(linkagesDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        topLabel = new QLabel(linkagesDialog);
        topLabel->setObjectName(QString::fromUtf8("topLabel"));
        topLabel->setTextFormat(Qt::RichText);
        topLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(topLabel);

        label = new QLabel(linkagesDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        verticalLayout->addWidget(label);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));

        verticalLayout->addLayout(verticalLayout_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(linkagesDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        comboBox = new QComboBox(linkagesDialog);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        horizontalLayout_2->addWidget(comboBox);


        verticalLayout->addLayout(horizontalLayout_2);

        loadImages = new QPushButton(linkagesDialog);
        loadImages->setObjectName(QString::fromUtf8("loadImages"));
        loadImages->setAutoDefault(true);
        loadImages->setFlat(false);

        verticalLayout->addWidget(loadImages);

        imageListDisplay = new QTextBrowser(linkagesDialog);
        imageListDisplay->setObjectName(QString::fromUtf8("imageListDisplay"));

        verticalLayout->addWidget(imageListDisplay);

        label_3 = new QLabel(linkagesDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        verticalLayout->addLayout(gridLayout);

        interpolateImages = new QCheckBox(linkagesDialog);
        interpolateImages->setObjectName(QString::fromUtf8("interpolateImages"));

        verticalLayout->addWidget(interpolateImages);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_4 = new QLabel(linkagesDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_3->addWidget(label_4);

        refreshRate = new QSpinBox(linkagesDialog);
        refreshRate->setObjectName(QString::fromUtf8("refreshRate"));
        refreshRate->setMaximum(10000);

        horizontalLayout_3->addWidget(refreshRate);


        verticalLayout->addLayout(horizontalLayout_3);

        buttonBox = new QDialogButtonBox(linkagesDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(linkagesDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), linkagesDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), linkagesDialog, SLOT(reject()));

        loadImages->setDefault(false);


        QMetaObject::connectSlotsByName(linkagesDialog);
    } // setupUi

    void retranslateUi(QDialog *linkagesDialog)
    {
        linkagesDialog->setWindowTitle(QCoreApplication::translate("linkagesDialog", "REvoSim - Linkages", nullptr));
        topLabel->setText(QCoreApplication::translate("linkagesDialog", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("linkagesDialog", "Please use the options below to add a link between a variable and an image stack. The chosen variable, when a linkage is present, is the product of the GUI value and the R  value of the image for each pixel.", nullptr));
        label_2->setText(QCoreApplication::translate("linkagesDialog", "Variable", nullptr));
        loadImages->setText(QCoreApplication::translate("linkagesDialog", "Load images", nullptr));
        label_3->setText(QCoreApplication::translate("linkagesDialog", "Mode:", nullptr));
        interpolateImages->setText(QCoreApplication::translate("linkagesDialog", "Interpolate images?", nullptr));
        label_4->setText(QCoreApplication::translate("linkagesDialog", "Refresh rate:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class linkagesDialog: public Ui_linkagesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LINKAGESDIALOG_H
