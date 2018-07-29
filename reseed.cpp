/**
 * @file
 * Reseed Widget
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2018 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "reseed.h"
#include "mainwindow.h"
#include "simmanager.h"
#include "ui_reseed.h"

/**
 * @brief Reseed::Reseed
 * @param parent
 */
Reseed::Reseed(QWidget *parent) : QDialog(parent), ui(new Ui::Reseed)
{
    ui->setupUi(this);
    QString newGenome;

    for (int i = 0; i < 64; i++)
        //---- RJG - if genome bit is 1, number is > 0; else it's zero.
        //---- RJG - Endian-ness to match genome comparison
        if (tweakers64[63 - i] & reseedGenome)
            newGenome.append("1");
        else
            newGenome.append("0");

    ui->genomeTextEdit->appendPlainText(newGenome);

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    ui->genomeTextEdit->setFont(font);

    ui->checkBoxReseedSession->setChecked(reseedKnown);

    int length = mainWindow->genoneComparison->accessGenomeListLength();
    if (length > 10)
        length = 10;

    if (!length) {
        QLabel *label = new QLabel("There are currently no genomes recorded in the Genome Docker.", this);
        ui->genomesLayout->addWidget(label);
    } else {
        for (int i = 0; i < length; i++) {
            auto *radio = new QRadioButton(this);
            radio->setText(mainWindow->genoneComparison->accessGenome(i));
            ui->genomesLayout->addWidget(radio);
            connect(radio, SIGNAL(toggled(bool)), this, SLOT(radio_toggled()));
            radios.append(radio);
        }
    }
}

/**
 * @brief reseed::~reseed
 */
Reseed::~Reseed()
{
    delete ui;
}

/**
 * @brief reseed::on_buttonBox_accepted
 */
void Reseed::on_buttonBox_accepted()
{
    QString newGenome(ui->genomeTextEdit->toPlainText());
    bool error = false;
    for (int i = 0; i < 64; i++)
        if (newGenome[i] != '1' && newGenome[i] != '0')
            error = true;
    if (newGenome.length() != 64 || error)
        QMessageBox::warning(
            this,
            "Oops",
            "This doesn't look like a valid genome, and so this is not going to be set. Sorry. Please try again by relaunching reseed."
        );
    else {
        for (int i = 0; i < 64; i++)
            if (newGenome[63 - i] == '1')
                reseedGenome |= tweakers64[i];
            else
                //RJG - ~ is a bitwise not operator.
                reseedGenome &= (~tweakers64[i]);

        reseedKnown = ui->checkBoxReseedSession->isChecked();
    }
}

/**
 * @brief Reseed::radio_toggled
 */
void Reseed::radio_toggled()
{
    ui->genomeTextEdit->clear();
    for (auto &radio : radios)
        if (radio->isChecked())
            ui->genomeTextEdit->appendPlainText(radio->text());
}

/**
 * @brief reseed::on_buttonBox_rejected
 */
void Reseed::on_buttonBox_rejected()
{
    close();
}
