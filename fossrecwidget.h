/**
 * @file
 * Header: Fossil Record Widget
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2018 by Mark Sutton, Russell Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef FOSSRECWIDGET_H
#define FOSSRECWIDGET_H

#include <QWidget>
#include <QDockWidget>
#include <QTreeWidgetItem>
#include "fossilrecord.h"
#include <QList>
#include <QDir>

namespace Ui {
    class FossRecWidget;
}

class FossRecWidget : public QWidget
{
    Q_OBJECT

public:
    FossRecWidget(QWidget *parent = 0);
    ~FossRecWidget();
    QList <FossilRecord *> FossilRecords;
    void MakeRecords();
    void RefreshMe();
    void HideWarnLabel();
    QString LogDirectory;
    bool LogDirectoryChosen;
    void WriteFiles();
    QList<bool> GetSelections();
    int findclosest(int x, int y, int min);
    QByteArray SaveState();
    void RestoreState(QByteArray InArray);
    void NewItem(int x, int y, int s);
    void SelectedActive(bool s);
    void SelectedSparse(int s);
private:
    Ui::FossRecWidget *ui;

    int NextItem;


private slots:
    void on_DeleteButton_pressed();
    void on_NewButton_pressed();
    void on_SelectAllButton_pressed();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeWidget_itemSelectionChanged();
};

#endif // FOSSRECWIDGET_H
