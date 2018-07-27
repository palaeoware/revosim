/**
 * @file
 * Header: Genome Comparison Dock.
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

#ifndef GENOMECOMPARISON_H
#define GENOMECOMPARISON_H

#include "simmanager.h"

#include <QString>
#include <QTableWidget>
#include <QWidget>

namespace Ui {
class GenomeComparison;
}

class GenomeComparison : public QWidget
{
    Q_OBJECT

public:
    explicit GenomeComparison(QWidget *parent = nullptr);
    ~GenomeComparison();

    Ui::GenomeComparison *ui;

    bool addGenomeCritter(Critter critter, quint8 *environment);
    bool loadComparison(QByteArray data);
    int accessGenomeListLength();
    QByteArray saveComparison();
    QString accessGenome(int row);

private slots:
    //---- Actions
    bool resetTables();
    bool deleteGenome();
    bool compareGenomes();
    void setAuto(bool toggle);
    void updateGenomeName(int row, int col);

private:
    //---- Styling
    void widgetStyling();

    //---- Tables
    bool renderGenomesTable();
    void insertRow(
        int row,
        QString genomeName,
        QString genomeStr,
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
        QString comparisonMask = QString(""));
    bool renderCompareTable();

    //---- Buttons
    void buttonUpdate();
    void buttonActions();

    //---- Table Functions
    QList<int> isGenomeChecked();

    //---- Vars
    QList< QMap<QString, QString> > genomeList;
    QList< QMap<QString, QString> > compareList;
    QColor first32;
    QColor last32;
    QColor spacerColumn;
    QColor highlight;
    bool autoComparison;
    int columnWidth;
};

#endif // GENOMECOMPARISON_H
