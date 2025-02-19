/**
 * @file
 * Reseed Widget
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

#include "mainwindow.h"
#include "reseed.h"
#include "simmanager.h"
#include "ui_reseed.h"

/*!
 * \brief Reseed::Reseed
 * \param parent
 */
Reseed::Reseed(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Reseed)
{
    ui->setupUi(this);

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    ui->genomeTextEdit->setFont(font);

    //RJG - set sensible defaults and connect slots
    connect(ui->checkBoxDualReseed, &QCheckBox::stateChanged, this, [ = ](const bool & checked)
    {
        if (checked && !ui->CheckBoxReseedSession->isChecked())
        {
            ui->randomRadio->setEnabled(true);
            ui->identicalRadio->setEnabled(true);
        }
        else
        {
            ui->randomRadio->setEnabled(false);
            ui->identicalRadio->setEnabled(false);
        }
    });

    connect(ui->CheckBoxReseedSession, &QCheckBox::stateChanged, this, [ = ](const bool & checked)
    {
        if (!checked && ui->checkBoxDualReseed->isChecked())
        {
            ui->randomRadio->setEnabled(true);
            ui->identicalRadio->setEnabled(true);
        }
        else
        {
            ui->randomRadio->setEnabled(false);
            ui->identicalRadio->setEnabled(false);
        }
    });
    ui->randomRadio->setEnabled(false);
    ui->identicalRadio->setEnabled(false);
    ui->randomRadio->setChecked(true);

    ui->checkBoxGenetic->setChecked(simulationManager->simulationSettings->randomReseedBeforeGenetic);

    //RJG - set up display to reflect correct mode
    if (simulationManager->simulationSettings->reseedMode == RESEED_MODE_DUAL_IDENTICAL)
    {
        ui->checkBoxDualReseed->setChecked(true);
        ui->identicalRadio->setChecked(true) ;
    }
    else if (simulationManager->simulationSettings->reseedMode == RESEED_MODE_DUAL_RANDOM)
    {
        ui->checkBoxDualReseed->setChecked(true);
        ui->randomRadio->setChecked(true);
    }
    else if (simulationManager->simulationSettings->reseedMode == RESEED_MODE_DUAL_KNOWN)
    {
        ui->checkBoxDualReseed->setChecked(true);
        ui->CheckBoxReseedSession->setChecked(true);
    }
    else if (simulationManager->simulationSettings->reseedMode == RESEED_MODE_SINGLE_KNOWN) ui->CheckBoxReseedSession->setChecked(true);

    //RJG - add radios if needed to a group so they are separate exclusive group to the above radios
    QButtonGroup *formatGroup = new QButtonGroup(this);
    formatGroup->addButton(ui->binaryRadio);
    formatGroup->addButton(ui->hexRadio);
    ui->binaryRadio->setChecked(true);
    connect(ui->hexRadio, SIGNAL(toggled(bool)), this, SLOT(hexToggled()));

    QButtonGroup *tierGroup = new QButtonGroup(this);
    ui->radio1Tier->setToolTip("<font>Select to use the default genome seeding system where a single genome is found that fits the central environment pixel.</font>");
    ui->radio3Tier->setToolTip("<font>Select to seed 3 trophic levels of organisms with fixed genomes.</font>");
    ui->radio5Tier->setToolTip("<font>Select to seed 5 trophic levels of organisms with fixed genomes.</font>");

    tierGroup->addButton(ui->radio1Tier, 0);
    tierGroup->addButton(ui->radio3Tier, 1);
    tierGroup->addButton(ui->radio5Tier, 2);

    connect(tierGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::idClicked), this, [ = ](const int &i)
    {
        //RJG - Deal here with the GUI - set options in accept so as to allow cancellation
        if (i > 0)
        {
            ui->checkBoxDualReseed->setChecked(false);
            ui->checkBoxDualReseed->setEnabled(false);
            ui->CheckBoxReseedSession->setChecked(false);
            ui->CheckBoxReseedSession->setEnabled(false);
        }
        else
        {
            ui->checkBoxDualReseed->setEnabled(true);
            ui->CheckBoxReseedSession->setEnabled(true);
        }
    });

    if (simulationManager->simulationSettings->trophicSeedMode == TROPHIC_SEED_3TIER)
    {
        ui->radio3Tier->setChecked(true);
        ui->checkBoxDualReseed->setEnabled(false);
    }
    else if (simulationManager->simulationSettings->trophicSeedMode == TROPHIC_SEED_5TIER)
    {
        ui->radio5Tier->setChecked(true);
        ui->checkBoxDualReseed->setEnabled(false);
    }
    else ui->radio1Tier->setChecked(true);

    //RJG - write genome string
    QString newGenome;
    for (int i = 0; i < simulationManager->simulationSettings->genomeSize; i++) newGenome.append(simulationManager->printGenome(simulationManager->simulationSettings->reseedGenome[i]));
    ui->genomeTextEdit->appendPlainText(newGenome);
    genomeGroup = new QButtonGroup(this);

    //RJG - get genomes from docker
    int length = mainWindow->genoneComparison->accessGenomeListLength();
    if (length > 10)length = 10;

    if (!length)
    {
        QLabel *label = new QLabel("There are currently no genomes recorded in the Genome Docker.", this);
        ui->genomesLayout->addWidget(label);
    }
    else
    {
        for (int i = 0; i < length; i++)
        {
            auto *radio = new QRadioButton(this);
            genomeGroup->addButton(radio);
            if (i == 0)radio->setChecked(true);
            radio->setText(mainWindow->genoneComparison->accessGenome(i));
            ui->genomesLayout->addWidget(radio);
            connect(radio, &QRadioButton::toggled, this, [ = ]()
            {
                ui->genomeTextEdit->clear();
                for (auto &radio : radios) if (radio->isChecked())ui->genomeTextEdit->appendPlainText(radio->text());
            });
            radios.append(radio);
        }
    }

    //RJG - Use hex if we have a long genome
    if (simulationManager->simulationSettings->genomeSize > 3)ui->hexRadio->setChecked(true);
}

/*!
* \brief Reseed::on_buttonBox_accepted
*/
void Reseed::on_buttonBox_accepted()
{
    QString newGenome(ui->genomeTextEdit->toPlainText());
    bool OK = true;
    OK = validateString(newGenome, ui->hexRadio->isChecked());
    if (OK && ui->hexRadio->isChecked())newGenome = convertStringToBinary(newGenome, OK);
    if (newGenome.length() != (32 * simulationManager->simulationSettings->genomeSize)) OK = false;
    if (OK)
        //Split in 32 bit chunks and write to genome
        for (int i = 0; i < newGenome.length(); i += 32)
        {
            QString word = newGenome.mid(i, 32);
            simulationManager->simulationSettings->reseedGenome[i / 32] = convertWordStringToNumber(word, OK, 2);
        }

    if (!OK)
    {
        QMessageBox::warning(this, "Oops", "This doesn't look like a valid genome, and so this is not going to be set. Sorry. Please try again by relaunching Reseed.");
        simulationManager->simulationSettings->reseedMode = RESEED_MODE_SINGLE_RANDOM;
        return;
    }


    if (ui->radio1Tier->isChecked()) simulationManager->simulationSettings->trophicSeedMode = TROPHIC_SEED_DEFAULT;
    else if (ui->radio3Tier->isChecked()) simulationManager->simulationSettings->trophicSeedMode = TROPHIC_SEED_3TIER;
    else simulationManager->simulationSettings->trophicSeedMode = TROPHIC_SEED_5TIER;

    if (ui->checkBoxDualReseed->isChecked() && ui->identicalRadio->isChecked()) simulationManager->simulationSettings->reseedMode = RESEED_MODE_DUAL_IDENTICAL;
    else if (ui->checkBoxDualReseed->isChecked() && ui->randomRadio->isChecked()) simulationManager->simulationSettings->reseedMode = RESEED_MODE_DUAL_RANDOM;
    else if (ui->checkBoxDualReseed->isChecked() && ui->CheckBoxReseedSession->isChecked()) simulationManager->simulationSettings->reseedMode = RESEED_MODE_DUAL_KNOWN;
    else if (ui->CheckBoxReseedSession->isChecked()) simulationManager->simulationSettings->reseedMode = RESEED_MODE_SINGLE_KNOWN;
    else simulationManager->simulationSettings->reseedMode = RESEED_MODE_SINGLE_RANDOM;

    simulationManager->simulationSettings->randomReseedBeforeGenetic = ui->checkBoxGenetic->isChecked();
}

/*!
 * \brief Reseed::hexToggled
 */
void Reseed::hexToggled()
{
    bool OK = true;
    bool startingState = ui->hexRadio->isChecked();
    QString originalString = ui->genomeTextEdit->toPlainText();

    if (startingState)
    {
        //RJG - check the string to convert is valid binary
        OK = validateString(originalString, false);
        if (OK) ui->genomeTextEdit->setPlainText(convertStringToHex(originalString, OK));
        for (auto &r : genomeGroup->buttons()) r->setText(convertStringToHex(r->text(), OK));
    }
    else
    {
        //RJG - check string to convert is valid hex
        OK = validateString(originalString, true);
        if (OK) ui->genomeTextEdit->setPlainText(convertStringToBinary(originalString, OK));
        for (auto &r : genomeGroup->buttons()) r->setText(convertStringToBinary(r->text(), OK));
    }

    //RJG - Error message
    if (!OK)
    {
        //RJG - block signals on this switch to avoid an infinite loop
        ui->hexRadio->blockSignals(true);
        ui->binaryRadio->setChecked(startingState);
        ui->hexRadio->setChecked(!startingState);
        ui->hexRadio->blockSignals(false);
        ui->genomeTextEdit->setPlainText(originalString);
        QMessageBox::warning(this, "Oops", "Wasn't able to convert - is string correct length, and correctly formatted?");
    }
}

/*!
 * \brief Reseed::convertStringToHex
 * Converts a binary string to a hex one
 */
QString Reseed::convertStringToHex(QString genomeString, bool &OK)
{
    QString hexGenome;
    //Split in 32 bit chunks
    for (int i = 0; i < genomeString.length(); i += 32)
    {
        QString word = genomeString.mid(i, 32);
        quint32 wordInt = word.toUInt(&OK, 2);
        hexGenome.append(QString("%1").arg(wordInt, 8, 16, QChar('0')));
    }
    return hexGenome.toUpper();
}

/*!
 * \brief Reseed::convertStringToBinary
 * Converts a hex string to a binary one
 */
QString Reseed::convertStringToBinary(QString genomeString, bool &OK)
{
    QString binaryGenome;
    //Split in 8 character chunks
    for (int i = 0; i < genomeString.length(); i += 8)
    {
        QString word = genomeString.mid(i, 8);
        quint32 wordInt = word.toUInt(&OK, 16);
        binaryGenome.append(QString("%1").arg(wordInt, 32, 2, QChar('0')));
    }
    return binaryGenome;
}

/*!
 * \brief Reseed::convertWordStringToNumber
 * Converts a hex or binary word string (i.e. one quint32s worth) to an int in chosen base
 */
quint32 Reseed::convertWordStringToNumber(QString genomeString, bool &OK, int base)
{
    if (base == 16) OK = validateString(genomeString, true);
    else OK = validateString(genomeString, false);
    return genomeString.toUInt(&OK, base);
}

/*!
 * \brief Reseed::validateString(QString genomeString, bool hex)
 * Validates string to ensure it is the correct length for conversion
 */
bool Reseed::validateString(QString genomeString, bool hex)
{
    int length = genomeString.length();
    if (hex)
    {
        if (length % 8) return false;
        //RJG - check valid hex
        QRegularExpression hexMatcher("^[0-9A-F]*$", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = hexMatcher.match(genomeString);
        if (!match.hasMatch()) return false;
    }
    else
    {
        if (length % 32) return false;
        QRegularExpression binaryMatcher("^[0-1]*$", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = binaryMatcher.match(genomeString);
        if (!match.hasMatch()) return false;
    }
    return true;
}

/*!
 * \brief Reseed::on_buttonBox_rejected
 */
void Reseed::on_buttonBox_rejected()
{
    close();
}

/*!
 * \brief Reseed::~Reseed
 */
Reseed::~Reseed()
{
    delete ui;
}
