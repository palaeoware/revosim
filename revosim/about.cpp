/**
 * @file
 * About Dialog
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "about.h"
#include "ui_about.h"
#include "globals.h"
#include "mainwindow.h"

#include <QString>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    setWindowTitle("About");
    setWindowIcon(QIcon (":/icon.png"));

    QPixmap picture_1(":/img.png");
    ui->header->setPixmap(picture_1);
    ui->header->setAlignment(Qt::AlignCenter);

    ui->textLabel_1->setText(mainWindow->windowTitle());
    ui->textLabel_1->setObjectName("aboutTextLabel1");
    ui->textLabel_1->setAlignment(Qt::AlignCenter);

    ui->textLabel_2->setWordWrap(true);
    ui->textLabel_2->setText("This version of " + QString(PRODUCTNAME) +
                             " was compiled on the date shown above. It was coded by:"
                             "<br><br><b>Mark Sutton</b> (m.sutton@imperial.ac.uk)<br><b>Russell Garwood</b> (russell.garwood@gmail.com)<br><b>Alan R.T. Spencer</b> (alan.spencer@imperial.ac.uk)<br><b>Euan N. Furness</b> (e.furness19@imperial.ac.uk)"
                             "<br><br>Reports are appreciated, and comments, suggestions, and feature requests are welcome.");
    ui->textLabel_2->setAlignment(Qt::AlignCenter);


    ui->textLabel_3->setWordWrap(true);
    ui->textLabel_3->setText("<b>Copyright and License:</b>"
                             "<br><br>" + QString(COPYRIGHT) +
                             "<br><br>" + QString(LICENCE) + " (see below).");
    ui->textLabel_3->setAlignment(Qt::AlignCenter);

    ui->textBrowser->setHtml(returnLicense());
    ui->textBrowser->setOpenLinks(true);
    ui->textBrowser->setOpenExternalLinks(true);

    QPixmap picture_2(":/gplV3Logo");
    ui->footer1->setPixmap(picture_2);
    ui->footer1->setAlignment(Qt::AlignCenter);

    QPixmap picture_3(":/builtWithQTLogo");
    ui->footer2->setPixmap(picture_3);
    ui->footer2->setAlignment(Qt::AlignCenter);

    ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
}

About::~About()
{
    delete ui;
}

QString About::returnLicense()
{
    QFile file(":/license_markup.txt");

    if (file.open(QIODevice::ReadOnly))
    {
        QString license = file.readAll();
        file.close();
        return license;
    }
    else return "License Missing!";
}
