/**
 * @file
 * Genome Comparison Dock.
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

#include "genomecomparison.h"
#include "ui_genomecomparison.h"

#include <QDebug>
#include <QMessageBox>
#include <QSizePolicy>

/*!
 * \brief QLabel class to allow changes in colour attached to timer (used in genome words GUI)
 */
class SysLabel: public QLabel
{
public:
    using QLabel::QLabel;
    void changeColour(const QColor &colour)
    {
        QPalette palette = this->palette();
        palette.setColor(QPalette::WindowText, colour);
        setPalette(palette);
    }
    void changeColourTimer(const QColor &colour, int timeout = 1000)
    {
        QColor defaultColour = Qt::white;
        changeColour(colour);
        QTimer::singleShot(timeout, this, [this, defaultColour]()
        {
            changeColour(defaultColour);
        });
    }
};

/*!
 * \brief GenomeComparison::GenomeComparison
 * \param parent
 *
 * Constructor
 */
GenomeComparison::GenomeComparison(QWidget *parent) : QWidget(parent),
    ui(new Ui::GenomeComparison)
{
    ui->setupUi(this);

    //Set Auto Compare
    autoComparison = true;
    ui->autoCheckbox->setChecked(true);

    //RJG2021 - hide these for now, they are not currently in use
    ui->compareTableWidget->hide();
    ui->compareButton->hide();
    ui->allButton->hide();
    ui->autoButton->hide();

    //Set Column Colours
    first32 = QColor(180, 200, 180);
    last32 = QColor(140, 160, 140);
    spacerColumn = QColor(QPalette().color(QPalette::Base).rgb());
    highlight = QColor(QPalette().color(QPalette::Highlight).rgb());

    columnWidth = 30;

    //Render Genome Tables
    QFont fnt;
    fnt.setPointSize(8);
    fnt.setFamily("Arial");
    fnt.setBold(true);

    ui->genomeTableWidget->setFont(fnt);
    renderGenomesTable();
    //ui->compareTableWidget->setFont(fnt);
    //ui->compareTableWidget->setColumnWidth(1, 100);


    //RJG - this needs to be a SysLabel to allow colour change. Easiest way to achieve correct layout given current setup is to just remove and then add line edit in right place
    SysLabel *wordsLabel = new SysLabel("Words to display: ");
    ui->horizontalLayout_2->addWidget(wordsLabel, 0);
    ui->horizontalLayout_2->removeWidget(ui->genomeWordsLineEdit);
    ui->horizontalLayout_2->addWidget(ui->genomeWordsLineEdit, 1);
    ui->genomeWordsLineEdit->setText(simulationManager->genomeComparisonSystem->returnGenomeWordInUseString());
    connect(ui->genomeWordsLineEdit, &QLineEdit::textChanged, this, [ = ](const QString & s)
    {
        //RJG - if we change number of words, need to start a new genomeList
        genomeList.clear();
        int max = simulationManager->breedSystem->returnMaxFromString(s);
        if (max < 0 || max > simulationManager->simulationSettings->genomeSize)
        {
            wordsLabel->changeColour(Qt::red);
            return;
        }

        if (!simulationManager->genomeComparisonSystem->setGenomeWordsFromString(s, simulationManager->simulationSettings->genomeSize)) wordsLabel->changeColour(Qt::red);
        else
        {
            wordsLabel->changeColourTimer(Qt::darkGreen, 1000);
            renderGenomesTable();
        }
    });

    //Signal to capture name change
    connect(ui->genomeTableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(updateGenomeName(int, int)));

    //Add Button Actions
    buttonActions();
    buttonUpdate();
}

/*!
 * \brief GenomeComparison::~GenomeComparison
 *
 * Destructor
 */
GenomeComparison::~GenomeComparison()
{
    delete ui;
}

/*!
 * \brief GenomeComparison::buttonActions
 *
 * Destructor // lies
 */
void GenomeComparison::buttonActions()
{
    connect(ui->compareButton, SIGNAL(pressed()), this, SLOT(compareGenomes()));
    connect(ui->autoCheckbox, SIGNAL(toggled(bool)), this, SLOT(setAuto(bool)));
    connect(ui->resetButton, SIGNAL(pressed()), this, SLOT(resetTables()));
    connect(ui->deleteButton, SIGNAL(pressed()), this, SLOT(deleteGenome()));
}

/*!
 * \brief GenomeComparison::buttonUpdate
 */
void GenomeComparison::buttonUpdate()
{
    if (autoComparison)
    {
        ui->autoButton->setChecked(true);
    }
    else
    {
        ui->autoButton->setChecked(false);
    }

    if (genomeList.count() != 0)
    {
        //---- Activate Buttons
        ui->compareButton->setEnabled(true);
        ui->resetButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
    }
    else
    {
        ui->compareButton->setEnabled(false);
        ui->resetButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    }
}

/*!
 * \brief GenomeComparison::calculateColumnCount
 */
int  GenomeComparison::calculateColumnCount()
{
    int genomeWordCount = simulationManager->genomeComparisonSystem->returnUseGenomeWordsCount();
    return  (32 * genomeWordCount) + 11 + (genomeWordCount - 2);
}

/*!
 * \brief GenomeComparison::renderGenomesTable
 * \return
 *
 * Tabels
 */
bool GenomeComparison::renderGenomesTable()
{
    ui->genomeTableWidget->hide();
    ui->genomeTableWidget->clear();
    ui->genomeTableWidget->setRowCount(0);

    ui->genomeTableWidget->setColumnCount(0);

    for (int i = 0; i < calculateColumnCount(); i++) ui->genomeTableWidget->insertColumn(i);

    int i = 0;
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("")));
    ui->genomeTableWidget->setColumnWidth(i, 20);
    i++;
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("Name")));
    ui->genomeTableWidget->setColumnWidth(i, 100);
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("E")));
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("S")));
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("SC")));
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("EF")));
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("F")));
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth * 2);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("Le")));
    i++;
    ui->genomeTableWidget->setColumnWidth(i, columnWidth * 2);
    ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("Se")));
    i++;

    int cnt = 1;
    while (i < calculateColumnCount())
    {

        if ((i - 9) % 33 == 0)
        {
            ui->genomeTableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tr("W%1C").arg((i - 9) / 33)));
            ui->genomeTableWidget->setColumnWidth(i, columnWidth + 20);
            i++;
        }
        else
        {
            QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(cnt));
            ui->genomeTableWidget->setHorizontalHeaderItem(i, newItem);
            ui->genomeTableWidget->setColumnWidth(i, columnWidth);
            if (cnt > 99) ui->genomeTableWidget->setColumnWidth(i, columnWidth + 9);
            i++;
            cnt++;
        }
    }

    //Add rows if genomeList.count() != 0
    if (!genomeList.empty())
    {
        for (int row = 0; row < genomeList.count(); row++)
        {
            //Add to table
            if (autoComparison && row != 0)
            {
                //Do comparison with last genome...
                QMap<QString, QString> genomeListMapA = genomeList[row - 1];
                QMap<QString, QString> genomeListMapB = genomeList[row];
                QString compareMask;

                //Create Masks
                for (int i = 0; i < 64; i++)
                {
                    if (genomeListMapA["genome"].at(i) == genomeListMapB["genome"].at(i))
                    {
                        //---- Same bit
                        compareMask.append("0");
                    }
                    else
                    {
                        compareMask.append("1");
                    }
                }

                insertRow(
                    row,
                    genomeList[row]["name"],
                    genomeList[row]["envColorR"].toInt(),
                    genomeList[row]["envColorG"].toInt(),
                    genomeList[row]["envColorB"].toInt(),
                    genomeList[row]["speciesID"].toInt(),
                    genomeList[row]["speciesR"].toInt(),
                    genomeList[row]["speciesG"].toInt(),
                    genomeList[row]["speciesB"].toInt(),
                    genomeList[row]["environmentalFitness"].toInt(),
                    genomeList[row]["fitness"].toInt(),
                    genomeList[row]["lifetimeEnergy"].toInt(),
                    genomeList[row]["stolenEnergy"].toInt(),
                    genomeList[row]["genome"],
                    ui->genomeTableWidget,
                    compareMask);
            }
            else
            {
                //No compare, just add...
                insertRow(
                    row,
                    genomeList[row]["name"],
                    genomeList[row]["envColorR"].toInt(),
                    genomeList[row]["envColorG"].toInt(),
                    genomeList[row]["envColorB"].toInt(),
                    genomeList[row]["speciesID"].toInt(),
                    genomeList[row]["speciesR"].toInt(),
                    genomeList[row]["speciesG"].toInt(),
                    genomeList[row]["speciesB"].toInt(),
                    genomeList[row]["environmentalFitness"].toInt(),
                    genomeList[row]["fitness"].toInt(),
                    genomeList[row]["lifetimeEnergy"].toInt(),
                    genomeList[row]["stolenEnergy"].toInt(),
                    genomeList[row]["genome"],
                    ui->genomeTableWidget);
            }
        }

    }

    ui->genomeTableWidget->show();
    return true;
}


/*!
 * \brief GenomeComparison::addGenomeCritter
 * \param critter
 * \param environment
 * \return
 */
bool GenomeComparison::addGenomeCritter(Critter critter, quint8 *environment)
{
    int row = genomeList.count();

    //Start with whole genome
    QString genomeStr = simulationManager->genomeComparisonSystem->returnGenomeInUseString(critter.genomeWords);
    QString fullGenomeStr = simulationManager->genomeComparisonSystem->returnGenomeString(critter.genomeWords, simulationManager->simulationSettings->genomeSize);
    quint64 SpeciesColourAssignment = speciesColours[critter.speciesID % 65536];
    quint32 speciesB = SpeciesColourAssignment % 256;
    quint32 speciesG = (SpeciesColourAssignment / 256) % 256;
    quint32 speciesR = ((SpeciesColourAssignment / 256) / 256) % 256;

    //Fitness
    int fitness = critter.fitness;
    int environmentalFitness = critter.environmentalFitness;
    int speciesID = critter.speciesID;
    QMap<QString, QString> genomeListMap;
    genomeListMap.insert("name", QString(tr("Genome %1").arg(row + 1)));
    genomeListMap.insert("genome", genomeStr);
    genomeListMap.insert("fullGenome", fullGenomeStr);
    genomeListMap.insert("envColorR", QString("%1").arg(environment[0]));
    genomeListMap.insert("envColorG", QString("%1").arg(environment[1]));
    genomeListMap.insert("envColorB", QString("%1").arg(environment[2]));
    genomeListMap.insert("speciesID", QString("%1").arg(speciesID));
    genomeListMap.insert("speciesColorR", QString("%1").arg(speciesR));
    genomeListMap.insert("speciesColorG", QString("%1").arg(speciesG));
    genomeListMap.insert("speciesColorB", QString("%1").arg(speciesB));
    genomeListMap.insert("environmentalFitness", QString("%1").arg(environmentalFitness));
    genomeListMap.insert("fitness", QString("%1").arg(fitness));

    //Energy Records
    int lifetimeEnergy = critter.lifetimeEnergy;
    int stolenEnergy = critter.stolenEnergy;
    genomeListMap.insert("lifetimeEnergy", QString("%1").arg(lifetimeEnergy));
    genomeListMap.insert("stolenEnergy", QString("%1").arg(stolenEnergy));

    genomeList.append(genomeListMap);

    //Add to table
    if (autoComparison && row != 0)
    {
        //Do comparison with last genome...
        QMap<QString, QString> genomeListMapA = genomeList[row - 1];
        QMap<QString, QString> genomeListMapB = genomeList[row];
        QString compareMask;
        int compareMaskLength = genomeList[row]["genome"].length();

        //Create Masks
        for (int i = 0; i < compareMaskLength; i++)
        {
            if (genomeListMapA["genome"].at(i) == genomeListMapB["genome"].at(i))
            {
                //---- Same bit
                compareMask.append("0");
            }
            else
            {
                compareMask.append("1");
            }
        }

        insertRow(
            row,
            genomeListMapB["name"],
            genomeListMapB["envColorR"].toInt(),
            genomeListMapB["envColorG"].toInt(),
            genomeListMapB["envColorB"].toInt(),
            genomeListMapB["speciesID"].toInt(),
            genomeListMapB["speciesColorR"].toInt(),
            genomeListMapB["speciesColorG"].toInt(),
            genomeListMapB["speciesColorB"].toInt(),
            genomeListMapB["environmentalFitness"].toInt(),
            genomeListMapB["fitness"].toInt(),
            genomeListMapB["lifetimeEnergy"].toInt(),
            genomeListMapB["stolenEnergy"].toInt(),
            genomeListMapB["genome"],
            ui->genomeTableWidget,
            compareMask);
    }
    else
    {
        //No compare, just add...
        insertRow(
            row,
            genomeListMap["name"],
            genomeListMap["envColorR"].toInt(),
            genomeListMap["envColorG"].toInt(),
            genomeListMap["envColorB"].toInt(),
            genomeListMap["speciesID"].toInt(),
            genomeListMap["speciesColorR"].toInt(),
            genomeListMap["speciesColorG"].toInt(),
            genomeListMap["speciesColorB"].toInt(),
            genomeListMap["environmentalFitness"].toInt(),
            genomeListMap["fitness"].toInt(),
            genomeListMap["lifetimeEnergy"].toInt(),
            genomeListMap["stolenEnergy"].toInt(),
            genomeListMap["genome"],
            ui->genomeTableWidget);
    }

    //Update Button States
    buttonUpdate();

    return true;
}


/*!
 * \brief GenomeComparison::insertRow
 * \param row
 * \param genomeName
 * \param genomeStr
 * \param environmentR
 * \param environmentG
 * \param environmentB
 * \param genomeR
 * \param genomeG
 * \param genomeB
 * \param nonCodeR
 * \param nonCodeG
 * \param nonCodeB
 * \param speciesR
 * \param speciesG
 * \param speciesB
 * \param fitness
 * \param lifetimeEnergy
 * \param stolenEnergy
 * \param speciesID
 * \param environmentalFitness
 * \param table
 * \param comparisonMask
 */
void GenomeComparison::insertRow(
    int row,
    const QString &genomeName,
    int environmentR,
    int environmentG,
    int environmentB,
    int speciesID,
    int speciesR,
    int speciesG,
    int speciesB,
    int environmentalFitness,
    int fitness,
    int lifetimeEnergy,
    int stolenEnergy,
    const QString &genomeStr,
    QTableWidget *table,
    const QString &comparisonMask)
{
    table->blockSignals(true);
    table->insertRow(row);

    QColor envColour = QColor(environmentR, environmentG, environmentB);
    QColor speciesColour = QColor(speciesR, speciesG, speciesB);

    if (table == ui->genomeTableWidget)
    {
        auto *newItem = new QTableWidgetItem();
        newItem->setCheckState(Qt::Unchecked);
        newItem->setTextAlignment(Qt:: AlignCenter);
        table->setItem(row, 0, newItem);

        newItem = new QTableWidgetItem(genomeName);
        newItem->setTextAlignment(Qt:: AlignCenter);
        table->setItem(row, 1, newItem);
    }
    //RJG - this is compare table - currently hidden
    else
    {
        QString val;
        if (row == 0)
        {
            val = "A";
        }
        else
        {
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

    //Add environment as cell background
    QTableWidgetItem *newItem = new QTableWidgetItem(tr(""));
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setToolTip(QString(tr("%1,%2,%3").arg(environmentR).arg(environmentG).arg(environmentB)));
    table->setItem(row, col, newItem);
    table->item(row, col)->QTableWidgetItem::setBackground(envColour);
    col++;

    //Add SpeciesID
    newItem = new QTableWidgetItem();
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setData(Qt::DisplayRole, speciesID);
    table->setItem(row, col, newItem);
    col++;

    //Add species colour as cell background
    newItem = new QTableWidgetItem(tr(""));
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setToolTip(QString(tr("%1,%2,%3").arg(speciesR).arg(speciesG).arg(speciesB)));
    table->setItem(row, col, newItem);
    table->item(row, col)->QTableWidgetItem::setBackground(speciesColour);
    col++;

    //Add environmental fitness
    newItem = new QTableWidgetItem();
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setData(Qt::DisplayRole, environmentalFitness);
    table->setItem(row, col, newItem);
    col++;

    //Add Fitness
    newItem = new QTableWidgetItem();
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setData(Qt::DisplayRole, fitness);
    table->setItem(row, col, newItem);
    col++;

    //Add Lifetime Energy
    newItem = new QTableWidgetItem();
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setData(Qt::DisplayRole, lifetimeEnergy);
    table->setItem(row, col, newItem);
    col++;

    //Add Stolen Energy
    newItem = new QTableWidgetItem();
    newItem->setTextAlignment(Qt:: AlignCenter);
    newItem->setFlags(Qt::ItemIsEnabled);
    newItem->setData(Qt::DisplayRole, stolenEnergy);
    table->setItem(row, col, newItem);
    col++;

    int i = 0;

    while (col < calculateColumnCount())
    {

        if ((col - 9) % 33 == 0)
        {
            //RJG - add spacer/gnome colour column for each word
            QTableWidgetItem *newItem = new QTableWidgetItem(tr(""));
            newItem->setFlags(Qt::ItemIsEnabled);
            table->setItem(row, col, newItem);

            //Add genome colour as cell background
            bool OK;
            QString word = genomeStr.mid(i, 32);
            quint32 genome = word.toUInt(&OK, 2);
            if (!OK) continue;

            quint32 genomeB = bitCounts[genome & 2047] * 23;
            genome /= 2048;
            quint32 genomeG = bitCounts[genome & 2047] * 23;
            genome /= 2048;
            quint32 genomeR = bitCounts[genome] * 25;
            QColor genomeColour = QColor(genomeR, genomeG, genomeB);
            newItem->setToolTip(QString(tr("%1,%2,%3").arg(genomeR).arg(genomeG).arg(genomeB)));
            newItem->QTableWidgetItem::setBackground(genomeColour);

            col++;
        }
        else
        {
            QString bit = genomeStr.at(i);
            auto *newItem = new QTableWidgetItem(bit);
            newItem->setTextAlignment(Qt:: AlignCenter);
            newItem->setFlags(Qt::ItemIsEnabled);

            table->setItem(row, col, newItem);
            int word = (col - 9) / 33;
            if (word % 2 == 0)
            {
                table->item(row, col)->setForeground(QBrush(first32));
            }
            else
            {
                table->item(row, col)->setForeground(QBrush(last32));
            }

            if (comparisonMask.length() != 0 && comparisonMask.at(i) == QChar(49))
            {
                //There is a mask, format...
                table->item(row, col)->setBackground(QBrush(highlight));
            }
            i++;
            col++;
        }
    }

    table->blockSignals(false);
}

/*!
 * \brief GenomeComparison::renderCompareTable
 * \return
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
    if (!compareList.empty())
    {
        QMap<QString, QString> genomeListMapA = compareList[0];
        QMap<QString, QString> genomeListMapB = compareList[1];
        QString compareMask;

        //Create Masks
        for (int i = 0; i < 64; i++)
        {
            if (genomeListMapA["genome"].at(i) == genomeListMapB["genome"].at(i))
            {
                //Same bit
                compareMask.append("0");
            }
            else
            {
                compareMask.append("1");
            }
        }

        //Insert Rows
        insertRow(
            0,
            genomeListMapA["name"],
            genomeListMapA["envColorR"].toInt(),
            genomeListMapA["envColorG"].toInt(),
            genomeListMapA["envColorB"].toInt(),
            genomeListMapA["speciesID"].toInt(),
            genomeListMapA["speciesColorR"].toInt(),
            genomeListMapA["speciesColorG"].toInt(),
            genomeListMapA["speciesColorB"].toInt(),
            genomeListMapA["environmentalFitness"].toInt(),
            genomeListMapA["fitness"].toInt(),
            genomeListMapA["lifetimeEnergy"].toInt(),
            genomeListMapA["stolenEnergy"].toInt(),
            genomeListMapA["genome"],
            ui->compareTableWidget);

        insertRow(
            1,
            genomeListMapB["name"],
            genomeListMapB["envColorR"].toInt(),
            genomeListMapB["envColorG"].toInt(),
            genomeListMapB["envColorB"].toInt(),
            genomeListMapB["speciesID"].toInt(),
            genomeListMapB["speciesColorR"].toInt(),
            genomeListMapB["speciesColorG"].toInt(),
            genomeListMapB["speciesColorB"].toInt(),
            genomeListMapB["environmentalFitness"].toInt(),
            genomeListMapB["fitness"].toInt(),
            genomeListMapB["lifetimeEnergy"].toInt(),
            genomeListMapB["stolenEnergy"].toInt(),
            genomeListMapB["genome"],
            ui->compareTableWidget,
            compareMask);
    }
    ui->compareTableWidget->show();

    return true;
}

/*!
 * \brief GenomeComparison::updateGenomeName
 * \param row
 * \param col
 *
 * Table Actions follow...
 */
void GenomeComparison::updateGenomeName(int row, int col)
{
    if (col == 1)
    {
        QString newName = ui->genomeTableWidget->item(row, col)->text();
        genomeList[row].insert("name", newName);
    }
}

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

bool GenomeComparison::loadComparison(QByteArray inData)
{
    QDataStream in(&inData, QIODevice::ReadOnly);

    //Get setup variable states
    in >> autoComparison;

    //Reset All Tables
    resetTables();

    //Get Manual Comparison Table Genome List
    in >> compareList;
    if (!compareList.empty())
    {
        renderCompareTable();
    }
    //Get Main Comparison Table Genome List
    in >> genomeList;
    if (!genomeList.empty())
    {
        renderGenomesTable();
    }

    //Update Button States
    buttonUpdate();

    return true;
}

//Button Actions
bool GenomeComparison::compareGenomes()
{
    //Are there any checked genomes?
    QList<int> checkedList = isGenomeChecked();
    int numChecked = checkedList.count();

    //Check that there two genomes checked
    if (numChecked == 0)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Genome Comparison: Error"));
        msgBox.setText(tr("You need to select 2 genomes from the table in order to compare."));
        msgBox.exec();
        return false;
    }
    if (numChecked == 1)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Genome Comparison: Error"));
        msgBox.setText(tr("You need to select 1 more genome from the table to begin comparing."));
        msgBox.exec();
        return false;
    }
    else if (numChecked > 2)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Genome Comparison: Error"));
        msgBox.setText(tr("You can not select more than 2 genomes to compare."));
        msgBox.exec();
        return false;
    }
    else
    {
        //---- Compare...
        compareList.clear();
        compareList.append(genomeList[checkedList[0]]);
        compareList.append(genomeList[checkedList[1]]);
        renderCompareTable();
    }
    return true;
}

bool GenomeComparison::resetTables()
{
    genomeList.clear();
    renderGenomesTable();

    //RJG2021 - removed these for time being - seem comments above
    //compareList.clear();
    //renderCompareTable();

    buttonUpdate();
    return true;
}

bool GenomeComparison::deleteGenome()
{
    //Are there any checked genomes?
    QList<int> checkedList = isGenomeChecked();
    int numChecked = checkedList.count();

    if (numChecked == 0)
    {
        //Nothing Checked
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Genome Comparison: Error"));
        msgBox.setText(tr("You need to select at least 1 genome from the table to delete."));
        msgBox.exec();
        return false;
    }
    //Something Checked, Remove selected from genomeList
    for (int i = 0; i < numChecked; i++)
    {
        genomeList.removeAt(checkedList[i - i]);
    }
    renderGenomesTable();
    buttonUpdate();

    return true;
}

bool GenomeComparison::returnAll()
{
    return ui->modalOnlyCheckbox->isChecked();
}


void GenomeComparison::setAuto(bool toggle)
{
    if (toggle)
    {
        ui->autoButton->setText(QString("Auto Compare ON"));
    }
    else
    {
        ui->autoButton->setText(QString("Auto Compare OFF"));
    }
    if (autoComparison != toggle)
    {
        autoComparison = toggle;

        //Reload Genome Table
        renderGenomesTable();
    }
}

/*!
 * \brief GenomeComparison::isGenomeChecked
 * \return
 *
 * Table Functions
 */
QList<int> GenomeComparison::isGenomeChecked()
{
    QList<int> checkedList;

    for (int i = 0; i < genomeList.count(); i++)
    {
        if (ui->genomeTableWidget->item(i, 0)->checkState() == Qt::Checked)
        {
            //Yes, add to list
            checkedList.append(i);
        }
    }

    return checkedList;
}

/*!
 * \brief GenomeComparison::accessGenome
 * \param row
 * \return
 *
 * RJG - acccess functions
 */
QString GenomeComparison::accessGenome(int row)
{
    return genomeList[row]["fullGenome"];
}

/*!
 * \brief GenomeComparison::accessGenomeListLength
 * \return
 */
int GenomeComparison::accessGenomeListLength()
{
    return genomeList.length();
}

