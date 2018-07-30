/**
 * @file
 * Genome Comparison Dock.
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

#include "genomecomparison.h"
#include "ui_genomecomparison.h"

#include <QDebug>
#include <QMessageBox>

/**
 * @brief GenomeComparison::GenomeComparison
 * @param parent
 */
GenomeComparison::GenomeComparison(QWidget *parent) : QWidget(parent), ui(new Ui::GenomeComparison)
{
    ui->setupUi(this);

    //Set Auto Compare
    autoComparison = true;

    //Set Column Colours
    first32 = QColor(0, 100, 0); // Green
    last32 = QColor(200, 0, 0); // Red
    spacerColumn = QColor(255, 255, 255); //White
    highlight = QColor(127, 127, 127); //Gray

    columnWidth = 30;

    //Render Genome Tables
    QFont fnt;
    fnt.setPointSize(8);
    fnt.setFamily("Arial");

    ui->genomeTableWidget->setFont(fnt);
    ui->genomeTableWidget->setColumnWidth(1, 100);

    ui->compareTableWidget->setFont(fnt);
    ui->compareTableWidget->setColumnWidth(1, 100);

    int i = 2;
    while (i < 34) {
        ui->genomeTableWidget->setColumnWidth(i, columnWidth);
        ui->compareTableWidget->setColumnWidth(i, columnWidth);
        i++;
    }

    ui->genomeTableWidget->setColumnWidth(34, 2); // Spacer
    ui->compareTableWidget->setColumnWidth(34, 2);

    i = 35;
    while (i < 67) {
        ui->genomeTableWidget->setColumnWidth(i, columnWidth);
        ui->compareTableWidget->setColumnWidth(i, columnWidth);
        i++;
    }
    renderGenomesTable();

    //Signal to capture name change
    connect(ui->genomeTableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(updateGenomeName(int,
                                                                                              int)));

    //Add Button Actions
    buttonActions();
    buttonUpdate();
}

/**
 * @brief GenomeComparison::~GenomeComparison
 */
GenomeComparison::~GenomeComparison()
{
    delete ui;
}

/**
* @brief GenomeComparison::buttonActions
*/
void GenomeComparison::buttonActions()
{
    connect(ui->compareButton, SIGNAL(pressed()), this, SLOT(compareGenomes()));
    connect(ui->autoButton, SIGNAL(toggled(bool)), this, SLOT(setAuto(bool)));
    connect(ui->resetButton, SIGNAL(pressed()), this, SLOT(resetTables()));
    connect(ui->deleteButton, SIGNAL(pressed()), this, SLOT(deleteGenome()));
}

/**
 * @brief GenomeComparison::buttonUpdate
 */
void GenomeComparison::buttonUpdate()
{
    if (autoComparison) {
        ui->autoButton->setChecked(true);
    } else {
        ui->autoButton->setChecked(false);
    }

    if (genomeList.count() != 0) {
        //---- Activate Buttons
        ui->compareButton->setEnabled(true);
        ui->resetButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
    } else {
        ui->compareButton->setEnabled(false);
        ui->resetButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    }
}

/**
 * @brief GenomeComparison::renderGenomesTable
 * @return
 */
bool GenomeComparison::renderGenomesTable()
{
    ui->genomeTableWidget->hide();
    ui->genomeTableWidget->clear();
    ui->genomeTableWidget->setRowCount(0);

    int i = 0;
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("")));
    ui->genomeTableWidget->setColumnWidth(i, 20);
    i++;
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("Name")));
    i++;
    for (int x = 0; x < 64; x++) {
        if (x == 32) {
            ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("")));
            i++;
        }
        if (x < 33) {
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(x + 1));
            ui->genomeTableWidget->setHorizontalHeaderItem(i, newItem);
            i++;
        } else {
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(x + 1));
            ui->genomeTableWidget->setHorizontalHeaderItem(i, newItem);
            i++;
        }
    }
    ui->genomeTableWidget->setColumnWidth(i, columnWidth);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("E")));
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("G")));
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("N")));
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("F")));

    //Add rows if genomeList.count() != 0
    if (!genomeList.empty()) {
        for (int row = 0; row < genomeList.count(); row++) {
            //Add to table
            if (autoComparison && row != 0) {
                //Do comparison with last genome...
                QMap<QString, QString> genomeListMapA = genomeList[row - 1];
                QMap<QString, QString> genomeListMapB = genomeList[row];
                QString compareMask;

                //Create Masks
                for (int i = 0; i < 64; i++) {
                    if (genomeListMapA["genome"].at(i) == genomeListMapB["genome"].at(i)) {
                        //---- Same bit
                        compareMask.append("0");
                    } else {
                        compareMask.append("1");
                    }
                }

                insertRow(
                    row,
                    genomeList[row]["name"],
                    genomeList[row]["genome"],
                    genomeList[row]["envColorR"].toInt(),
                    genomeList[row]["envColorG"].toInt(),
                    genomeList[row]["envColorB"].toInt(),
                    genomeList[row]["genomeColorR"].toInt(),
                    genomeList[row]["genomeColorG"].toInt(),
                    genomeList[row]["genomeColorB"].toInt(),
                    genomeList[row]["nonCodeColorR"].toInt(),
                    genomeList[row]["nonCodeColorG"].toInt(),
                    genomeList[row]["nonCodeColorB"].toInt(),
                    genomeList[row]["fitness"].toInt(),
                    ui->genomeTableWidget,
                    compareMask);
            } else {
                //No compare, just add...
                insertRow(
                    row,
                    genomeList[row]["name"],
                    genomeList[row]["genome"],
                    genomeList[row]["envColorR"].toInt(),
                    genomeList[row]["envColorG"].toInt(),
                    genomeList[row]["envColorB"].toInt(),
                    genomeList[row]["genomeColorR"].toInt(),
                    genomeList[row]["genomeColorG"].toInt(),
                    genomeList[row]["genomeColorB"].toInt(),
                    genomeList[row]["nonCodeColorR"].toInt(),
                    genomeList[row]["nonCodeColorG"].toInt(),
                    genomeList[row]["nonCodeColorB"].toInt(),
                    genomeList[row]["fitness"].toInt(),
                    ui->genomeTableWidget);
            }
        }

    }

    ui->genomeTableWidget->show();
    return true;
}

/**
 * @brief GenomeComparison::insertRow
 * @param row
 * @param genomeName
 * @param genomeStr
 * @param environmentR
 * @param environmentG
 * @param environmentB
 * @param genomeR
 * @param genomeG
 * @param genomeB
 * @param nonCodeR
 * @param nonCodeG
 * @param nonCodeB
 * @param fitness
 * @param table
 * @param comparisonMask
 */
void GenomeComparison::insertRow(
    int row,
    const QString &genomeName,
    const QString &genomeStr,
    int environmentR,
    int environmentG,
    int environmentB,
    int genomeR,
    int genomeG,
    int genomeB,
    int nonCodeR,
    int nonCodeG,
    int nonCodeB,
    int fitness,
    QTableWidget *table,
    const QString &comparisonMask)
{
    table->blockSignals(true);
    table->insertRow(row);

    QColor envColour = QColor(environmentR, environmentG, environmentB);
    QColor genomeColour = QColor(genomeR, genomeG, genomeB);
    QColor nonCodeColour = QColor(nonCodeR, nonCodeG, nonCodeB);

    if (table == ui->genomeTableWidget) {
        auto *newItem = new QTableWidgetItem();
        newItem->setCheckState(Qt::Unchecked);
        newItem->setTextAlignment(Qt:: AlignCenter);
        table->setItem(row, 0, newItem);

        newItem = new QTableWidgetItem(genomeName);
        newItem->setTextAlignment(Qt:: AlignCenter);
        table->setItem(row, 1, newItem);

    } else {
        QString val;
        if (row == 0) {
            val = "A";
        } else {
            val = "B";
        }
        QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(val));
        newItem->setTextAlignment(Qt:: AlignCenter);
        newItem->setFlags(Qt::ItemIsEnabled);
        table->setItem(row, 0, newItem);

        newItem = new QTableWidgetItem(genomeName);
        newItem->setTextAlignment(Qt:: AlignCenter);
        newItem->setFlags(Qt::ItemIsEnabled);
        table->setItem(row, 1, newItem);
    }

    int col = 2;
    for (int i = 0; i < 64; i++) {
        if (col == 34) {
            QTableWidgetItem *newItem = new QTableWidgetItem(tr(""));
            newItem->setFlags(Qt::ItemIsEnabled);
            table->setItem(row, col, newItem);
            table->item(row, col)->setBackground(QBrush(spacerColumn));
            col++;
        }

        QString bit = genomeStr.at(i);
        auto *newItem = new QTableWidgetItem(bit);
        newItem->setTextAlignment(Qt:: AlignCenter);
        newItem->setFlags(Qt::ItemIsEnabled);

        table->setItem(row, col, newItem);
        if (i < 32) {
            table->item(row, col)->setForeground(QBrush(first32));
        } else {
            table->item(row, col)->setForeground(QBrush(last32));
        }

        if (comparisonMask.length() != 0 && comparisonMask.at(i) == QChar(49)) {
            //There is a mask, formate...
            table->item(row, col)->setBackground(QBrush(highlight));
        }

        col++;
    }

    //Add environment as cell background
    QTableWidgetItem *newItem = new QTableWidgetItem(tr(""));
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setToolTip(QString(tr("%1,%2,%3").arg(environmentR).arg(environmentG).arg(environmentB)));
    table->setItem(row, col, newItem);
    table->item(row, col)->setBackgroundColor(envColour);
    col++;

    //Add genome colour as cell background
    newItem = new QTableWidgetItem(tr(""));
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setToolTip(QString(tr("%1,%2,%3").arg(genomeR).arg(genomeG).arg(genomeB)));
    table->setItem(row, col, newItem);
    table->item(row, col)->setBackgroundColor(genomeColour);
    col++;

    //Add non coded colour as cell background
    newItem = new QTableWidgetItem(tr(""));
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setToolTip(QString(tr("%1,%2,%3").arg(nonCodeR).arg(nonCodeG).arg(nonCodeB)));
    table->setItem(row, col, newItem);
    table->item(row, col)->setBackgroundColor(nonCodeColour);
    col++;

    //Add Fitness
    newItem = new QTableWidgetItem();
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setData(Qt::DisplayRole, fitness);
    table->setItem(row, col, newItem);

    table->blockSignals(false);
}

/**
 * @brief GenomeComparison::renderCompareTable
 * @return
 */
bool GenomeComparison::renderCompareTable()
{
    //---- Reset Table
    ui->compareTableWidget->hide();
    ui->compareTableWidget->clear();
    ui->compareTableWidget->setRowCount(0);
    ui->compareTableWidget->setColumnWidth(0, columnWidth);
    ui->compareTableWidget->setColumnWidth(67, columnWidth);
    ui->compareTableWidget->setColumnWidth(68, columnWidth);
    ui->compareTableWidget->setColumnWidth(69, columnWidth);
    ui->compareTableWidget->setColumnWidth(70, columnWidth);

    //Setup
    if (!compareList.empty()) {
        QMap<QString, QString> genomeListMapA = compareList[0];
        QMap<QString, QString> genomeListMapB = compareList[1];
        QString compareMask;

        //Create Masks
        for (int i = 0; i < 64; i++) {
            if (genomeListMapA["genome"].at(i) == genomeListMapB["genome"].at(i)) {
                //Same bit
                compareMask.append("0");
            } else {
                compareMask.append("1");
            }
        }

        //Insert Rows
        insertRow(
            0,
            genomeListMapA["name"],
            genomeListMapA["genome"],
            genomeListMapA["envColorR"].toInt(),
            genomeListMapA["envColorG"].toInt(),
            genomeListMapA["envColorB"].toInt(),
            genomeListMapA["genomeColorR"].toInt(),
            genomeListMapA["genomeColorG"].toInt(),
            genomeListMapA["genomeColorB"].toInt(),
            genomeListMapA["nonCodeColorR"].toInt(),
            genomeListMapA["nonCodeColorG"].toInt(),
            genomeListMapA["nonCodeColorB"].toInt(),
            genomeListMapA["fitness"].toInt(),
            ui->compareTableWidget);

        insertRow(
            1,
            genomeListMapB["name"],
            genomeListMapB["genome"],
            genomeListMapB["envColorR"].toInt(),
            genomeListMapB["envColorG"].toInt(),
            genomeListMapB["envColorB"].toInt(),
            genomeListMapB["genomeColorR"].toInt(),
            genomeListMapB["genomeColorG"].toInt(),
            genomeListMapB["genomeColorB"].toInt(),
            genomeListMapB["nonCodeColorR"].toInt(),
            genomeListMapB["nonCodeColorG"].toInt(),
            genomeListMapB["nonCodeColorB"].toInt(),
            genomeListMapB["fitness"].toInt(),
            ui->compareTableWidget,
            compareMask);
    }
    ui->compareTableWidget->show();

    return true;
}

/**
 * @brief GenomeComparison::updateGenomeName
 * @param row
 * @param col
 */
void GenomeComparison::updateGenomeName(int row, int col)
{
    if (col == 1) {
        QString newName = ui->genomeTableWidget->item(row, col)->text();
        genomeList[row].insert("name", newName);
    }
}

/**
 * @brief GenomeComparison::addGenomeCritter
 * @param critter
 * @param environment
 * @return
 */
bool GenomeComparison::addGenomeCritter(Critter critter, quint8 *environment)
{
    int row = genomeList.count();
    quint32 genome;

    //Start with whole genome
    QString genomeStr;
    for (int j = 0; j < 64; j++)
        //RJG - if genome bit is 1, number is > 0; else it's zero.
        if (tweakers64[63 - j] & critter.genome) genomeStr.append("1");
        else genomeStr.append("0");

    //Coding Genome Colour
    genome = static_cast<quint32>(critter.genome & (static_cast<quint64>(65536) * static_cast<quint64>(65536) - static_cast<quint64>(1))); //lower 32 bits
    quint32 genomeB = bitCounts[genome & 2047] * 23;
    genome /= 2048;
    quint32 genomeG = bitCounts[genome & 2047] * 23;
    genome /= 2048;
    quint32 genomeR = bitCounts[genome] * 25;

    //Non-coding Genome Colour
    genome = static_cast<quint32>(critter.genome / (static_cast<quint64>(65536) * static_cast<quint64>(65536))); //upper 32 bits
    quint32 nonCodeB = bitCounts[genome & 2047] * 23;
    genome /= 2048;
    quint32 nonCodeG = bitCounts[genome & 2047] * 23;
    genome /= 2048;
    quint32 nonCodeR = bitCounts[genome] * 25;

    //Fitness
    int fitness = critter.fitness;

    QMap<QString, QString> genomeListMap;
    genomeListMap.insert("name", QString(tr("Genome %1").arg(row + 1)));
    genomeListMap.insert("genome", genomeStr);
    genomeListMap.insert("envColorR", QString("%1").arg(environment[0]));
    genomeListMap.insert("envColorG", QString("%1").arg(environment[1]));
    genomeListMap.insert("envColorB", QString("%1").arg(environment[2]));
    genomeListMap.insert("genomeColorR", QString("%1").arg(genomeR));
    genomeListMap.insert("genomeColorG", QString("%1").arg(genomeG));
    genomeListMap.insert("genomeColorB", QString("%1").arg(genomeB));
    genomeListMap.insert("nonCodeColorR", QString("%1").arg(nonCodeR));
    genomeListMap.insert("nonCodeColorG", QString("%1").arg(nonCodeG));
    genomeListMap.insert("nonCodeColorB", QString("%1").arg(nonCodeB));
    genomeListMap.insert("fitness", QString("%1").arg(fitness));
    genomeList.append(genomeListMap);

    //Add to table
    if (autoComparison && row != 0) {
        //Do comparison with last genome...
        QMap<QString, QString> genomeListMapA = genomeList[row - 1];
        QMap<QString, QString> genomeListMapB = genomeList[row];
        QString compareMask;

        //Create Masks
        for (int i = 0; i < 64; i++) {
            if (genomeListMapA["genome"].at(i) == genomeListMapB["genome"].at(i)) {
                //---- Same bit
                compareMask.append("0");
            } else {
                compareMask.append("1");
            }
        }

        insertRow(
            row,
            genomeListMapB["name"],
            genomeListMapB["genome"],
            genomeListMapB["envColorR"].toInt(),
            genomeListMapB["envColorG"].toInt(),
            genomeListMapB["envColorB"].toInt(),
            genomeListMapB["genomeColorR"].toInt(),
            genomeListMapB["genomeColorG"].toInt(),
            genomeListMapB["genomeColorB"].toInt(),
            genomeListMapB["nonCodeColorR"].toInt(),
            genomeListMapB["nonCodeColorG"].toInt(),
            genomeListMapB["nonCodeColorB"].toInt(),
            genomeListMapB["fitness"].toInt(),
            ui->genomeTableWidget,
            compareMask);
    } else {
        //No compare, just add...
        insertRow(
            row,
            genomeListMap["name"],
            genomeListMap["genome"],
            genomeListMap["envColorR"].toInt(),
            genomeListMap["envColorG"].toInt(),
            genomeListMap["envColorB"].toInt(),
            genomeListMap["genomeColorR"].toInt(),
            genomeListMap["genomeColorG"].toInt(),
            genomeListMap["genomeColorB"].toInt(),
            genomeListMap["nonCodeColorR"].toInt(),
            genomeListMap["nonCodeColorG"].toInt(),
            genomeListMap["nonCodeColorB"].toInt(),
            genomeListMap["fitness"].toInt(),
            ui->genomeTableWidget);
    }

    //Update Button States
    buttonUpdate();

    return true;
}

/**
 * @brief GenomeComparison::saveComparison
 * @return
 */
QByteArray GenomeComparison::saveComparison()
{
    QByteArray outData;
    QDataStream out(&outData, QIODevice::WriteOnly);

    //Output setup variable states
    out << autoComparison;

    //Output Manual Comparison Table Genome List
    out << compareList;

    //Output Main Comparison Table Genome List
    out << genomeList;

    return outData;
}

/**
 * @brief GenomeComparison::loadComparison
 * @param inData
 * @return
 */
bool GenomeComparison::loadComparison(QByteArray inData)
{
    QDataStream in(&inData, QIODevice::ReadOnly);

    //Get setup variable states
    in >> autoComparison;

    //Reset All Tables
    resetTables();

    //Get Manual Comparison Table Genome List
    in >> compareList;
    if (!compareList.empty()) {
        renderCompareTable();
    }
    //Get Main Comparison Table Genome List
    in >> genomeList;
    if (!genomeList.empty()) {
        renderGenomesTable();
    }

    //Update Button States
    buttonUpdate();

    return true;
}

/**
 * @brief GenomeComparison::compareGenomes
 * @return
 */
bool GenomeComparison::compareGenomes()
{
    //Are there any checked genomes?
    QList<int> checkedList = isGenomeChecked();
    int numChecked = checkedList.count();

    //Check that there two genomes checked
    if (numChecked == 0) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Genome Comparison: Error"));
        msgBox.setText(tr("You need to select 2 genomes from the table in order to compare."));
        msgBox.exec();
        return false;
    }
    if (numChecked == 1) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Genome Comparison: Error"));
        msgBox.setText(tr("You need to select 1 more genome from the table to begin comparing."));
        msgBox.exec();
        return false;
    } else if (numChecked > 2) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Genome Comparison: Error"));
        msgBox.setText(tr("You can not select more than 2 genomes to compare."));
        msgBox.exec();
        return false;
    } else {
        //---- Compare...
        compareList.clear();
        compareList.append(genomeList[checkedList[0]]);
        compareList.append(genomeList[checkedList[1]]);
        renderCompareTable();
    }
    return true;
}

/**
 * @brief GenomeComparison::resetTables
 * @return
 */
bool GenomeComparison::resetTables()
{
    genomeList.clear();
    renderGenomesTable();

    compareList.clear();
    renderCompareTable();

    buttonUpdate();
    return true;
}

/**
 * @brief GenomeComparison::deleteGenome
 * @return
 */
bool GenomeComparison::deleteGenome()
{
    //Are there any checked genomes?
    QList<int> checkedList = isGenomeChecked();
    int numChecked = checkedList.count();

    if (numChecked == 0) {
        //Nothing Checked
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Genome Comparison: Error"));
        msgBox.setText(tr("You need to select at least 1 genome from the table to delete."));
        msgBox.exec();
        return false;
    }
    //Something Checked, Remove selected from genomeList
    for (int i = 0; i < numChecked; i++) {
        genomeList.removeAt(checkedList[i - i]);
    }
    renderGenomesTable();
    buttonUpdate();

    return true;
}

/**
 * @brief GenomeComparison::setAuto
 * @param toggle
 */
void GenomeComparison::setAuto(bool toggle)
{
    if (toggle) {
        ui->autoButton->setText(QString("Auto Compare ON"));
    } else {
        ui->autoButton->setText(QString("Auto Compare OFF"));
    }
    if (autoComparison != toggle) {
        autoComparison = toggle;

        //Reload Genome Table
        renderGenomesTable();
    }
}

/**
 * @brief GenomeComparison::isGenomeChecked
 * @return
 */
QList<int> GenomeComparison::isGenomeChecked()
{
    QList<int> checkedList;

    for (int i = 0; i < genomeList.count(); i++) {
        if (ui->genomeTableWidget->item(i, 0)->checkState() == Qt::Checked) {
            //Yes, add to list
            checkedList.append(i);
        }
    }

    return checkedList;
}


/**
 * @brief GenomeComparison::accessGenome
 * @param row
 * @return
 */
QString GenomeComparison::accessGenome(int row)
{
    return genomeList[row]["genome"];
}

/**
 * @brief GenomeComparison::accessGenomeListLength
 * @return
 */
int GenomeComparison::accessGenomeListLength()
{
    return genomeList.length();
}
