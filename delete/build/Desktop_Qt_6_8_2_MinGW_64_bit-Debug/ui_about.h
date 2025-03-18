/********************************************************************************
** Form generated from reading UI file 'about.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_About
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *header;
    QLabel *textLabel_1;
    QLabel *textLabel_2;
    QLabel *textLabel_3;
    QTextBrowser *textBrowser;
    QHBoxLayout *horizontalLayout;
    QLabel *footer1;
    QLabel *footer2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *About)
    {
        if (About->objectName().isEmpty())
            About->setObjectName("About");
        About->resize(600, 680);
        layoutWidget = new QWidget(About);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 10, 581, 661));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        header = new QLabel(layoutWidget);
        header->setObjectName("header");

        verticalLayout->addWidget(header);

        textLabel_1 = new QLabel(layoutWidget);
        textLabel_1->setObjectName("textLabel_1");

        verticalLayout->addWidget(textLabel_1);

        textLabel_2 = new QLabel(layoutWidget);
        textLabel_2->setObjectName("textLabel_2");

        verticalLayout->addWidget(textLabel_2);

        textLabel_3 = new QLabel(layoutWidget);
        textLabel_3->setObjectName("textLabel_3");

        verticalLayout->addWidget(textLabel_3);

        textBrowser = new QTextBrowser(layoutWidget);
        textBrowser->setObjectName("textBrowser");
        textBrowser->viewport()->setProperty("cursor", QVariant(QCursor(Qt::CursorShape::ArrowCursor)));
        textBrowser->setAcceptDrops(false);
        textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textBrowser->setOverwriteMode(false);

        verticalLayout->addWidget(textBrowser);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 10, -1, -1);
        footer1 = new QLabel(layoutWidget);
        footer1->setObjectName("footer1");

        horizontalLayout->addWidget(footer1);

        footer2 = new QLabel(layoutWidget);
        footer2->setObjectName("footer2");

        horizontalLayout->addWidget(footer2);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(layoutWidget);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(About);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, About, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, About, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(About);
    } // setupUi

    void retranslateUi(QDialog *About)
    {
        About->setWindowTitle(QCoreApplication::translate("About", "Dialog", nullptr));
        header->setText(QCoreApplication::translate("About", "TextLabel", nullptr));
        textLabel_1->setText(QCoreApplication::translate("About", "TextLabel", nullptr));
        textLabel_2->setText(QCoreApplication::translate("About", "TextLabel", nullptr));
        textLabel_3->setText(QCoreApplication::translate("About", "TextLabel", nullptr));
        footer1->setText(QCoreApplication::translate("About", "Footer 1", nullptr));
        footer2->setText(QCoreApplication::translate("About", "Footer 2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class About: public Ui_About {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_H
