/**
 * @file
 * Fossil Record Widget
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

#include "fossrecwidget.h"
#include "ui_fossrecwidget.h"
#include "mainwindow.h"
#include "simmanager.h"
#include <math.h>
#include <QInputDialog>
#include <QMessageBox>

FossRecWidget::FossRecWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FossRecWidget)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnWidth(0,150);
    ui->treeWidget->setColumnWidth(1,40);
    ui->treeWidget->setColumnWidth(2,40);
    ui->treeWidget->setColumnWidth(3,40);
    ui->treeWidget->setColumnWidth(4,50);
    ui->treeWidget->setColumnWidth(5,50);
    ui->treeWidget->setColumnWidth(6,30);

    ui->treeWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
    FossilRecords.clear();

    RefreshMe();
    NextItem=0;

    LogDirectoryChosen=false;
}

void FossRecWidget::HideWarnLabel()
{
    ui->WarnLabel->setVisible(false);
}

int FossRecWidget::findclosest(int x, int y, int min)
{
    //find closest selected item to the given position. Return -1 if its further than min away

    int dist2=99999999;
    int closest=-1;
    QList<bool> selections = GetSelections();
    for (int i=0; i<FossilRecords.count(); i++)
        if (selections[i])
        {
            int dist=(FossilRecords[i]->xpos-x)*(FossilRecords[i]->xpos-x) + (FossilRecords[i]->ypos-y)*(FossilRecords[i]->ypos-y);
            if (dist<dist2)
            {
                dist2=dist;
                closest=i;
            }
        }
    if (sqrt((double)dist2)<min) return closest; else return -1;
}

FossRecWidget::~FossRecWidget()
{
    delete ui;

}

void FossRecWidget::RefreshMe()
{
    QList<bool> selections=GetSelections();
    //write in all the current records
    ui->treeWidget->clear();

    if (FossilRecords.count()==0) return;
    ui->treeWidget->setUniformRowHeights(true);
    ui->treeWidget->setUpdatesEnabled(false);

    for (int i=0; i<FossilRecords.count(); i++)
    {
        QStringList strings;
        QString temp;

        strings.append(FossilRecords[i]->name);
        temp.sprintf("%d",FossilRecords[i]->xpos); strings.append(temp);
        temp.sprintf("%d",FossilRecords[i]->ypos); strings.append(temp);
        temp.sprintf("%d",FossilRecords[i]->startage); strings.append(temp);
        temp.sprintf("%d",FossilRecords[i]->sparsity); strings.append(temp);
        temp.sprintf("%d",FossilRecords[i]->recorded); strings.append(temp);
        if (FossilRecords[i]->recording) strings.append("Yes"); else strings.append("No");

        QTreeWidgetItem *item=new QTreeWidgetItem(strings);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->treeWidget->addTopLevelItem(item);
    }

    //redo selections
    for (int i=0; i<selections.count(); i++)
        ui->treeWidget->topLevelItem(i)->setSelected(selections[i]);

   ui->treeWidget->setUpdatesEnabled(true);

   return;
}

void FossRecWidget::MakeRecords()
{
    if (!LogDirectoryChosen) return;
    for (int i=0; i<FossilRecords.count(); i++)
        FossilRecords[i]->MakeFossil();
}

void FossRecWidget::on_DeleteButton_pressed()
{
    QList<QTreeWidgetItem *> allitems;

    for (int i=0; i<ui->treeWidget->topLevelItemCount(); i++)
        allitems.append(ui->treeWidget->topLevelItem(i));

    for (int i=0; i<allitems.count(); i++)
    if (allitems[i]->isSelected())
    {
        //item i to be deleted
        FossilRecords[i]->xpos=-1; //mark for death
    }

    QMutableListIterator<FossilRecord *> i(FossilRecords);
    while (i.hasNext())
    {
        FossilRecord *item = i.next();
        if (item->xpos==-1)
       {
           delete item;
           i.remove();
       }

    }
    RefreshMe();
    MainWin->RefreshEnvironment();
}

void FossRecWidget::on_NewButton_pressed()
{
    NewItem(50,50,10);
    MainWin->RefreshEnvironment();
}

void FossRecWidget::on_SelectAllButton_pressed()
{
    ui->treeWidget->selectAll();
}

void FossRecWidget::NewItem(int x, int y, int s)
{
    QString String;
    String.sprintf("Record %d",NextItem++);
    FossilRecords.append(new FossilRecord(x,y,s,String));
    RefreshMe();
}

void FossRecWidget::on_treeWidget_itemSelectionChanged()
{
    MainWin->RefreshEnvironment();
}

void FossRecWidget::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    //find index
    int index=ui->treeWidget->indexOfTopLevelItem(item);

    if (column==0)
    //Name
    {
        QString newname=QInputDialog::getText(this,"","New unique name for record item",QLineEdit::Normal,FossilRecords[index]->name);
        if (newname=="") return;

        bool ok=true;
        //Check name not in use
        for (int i=0; i<FossilRecords.count(); i++)
        {
            if (i!=index) if (FossilRecords[i]->name==newname) ok=false;
        }
        if (ok) FossilRecords[index]->name=newname;
        else
        {
            QMessageBox::warning(this,"Warning","A record with that name already exists");
        }
    }

    if (column==1)
    //Xpos
    {
        bool ok;
        int newint=QInputDialog::getInt(this,"","New X coordinate",FossilRecords[index]->xpos,0,gridX-1,1,&ok);
        if (!ok) return;
        else FossilRecords[index]->xpos=newint;
    }

    if (column==2)
    //Ypos
    {
        bool ok;
        int newint=QInputDialog::getInt(this,"","New Y coordinate",FossilRecords[index]->ypos,0,gridY-1,1,&ok);
        if (!ok) return;
        else FossilRecords[index]->ypos=newint;
    }

    //3 is start -can't edit
    //4 is count of records - can't edit

    if (column==4)
    //Sparsity
    {
        bool ok;
        int newint=QInputDialog::getInt(this,"","New Sparsity",FossilRecords[index]->sparsity,1,100000000,1,&ok);
        if (!ok) return;
        else FossilRecords[index]->sparsity=newint;
    }

    if (column==6)
    //Active
        FossilRecords[index]->recording = !FossilRecords[index]->recording;

    RefreshMe();
    MainWin->RefreshEnvironment();
}

void FossRecWidget::WriteFiles()
{
    if (!LogDirectoryChosen) return;
    for (int i=0; i<FossilRecords.count(); i++)
    {
        FossilRecords[i]->WriteRecord(LogDirectory + "/" + FossilRecords[i]->name + ".csv");
    }
}

QList<bool> FossRecWidget::GetSelections()
{
    QList<bool> sl;

    QList<QTreeWidgetItem *> allitems;

    for (int i=0; i<ui->treeWidget->topLevelItemCount(); i++)
        allitems.append(ui->treeWidget->topLevelItem(i));

    for (int i=0; i<allitems.count(); i++)
    if (allitems[i]->isSelected())
        sl.append(true);
    else
        sl.append(false);

    while (sl.count()<FossilRecords.count()) sl.append(false);  //this is a safety feature - ensure list is right length
    return sl;
}

QByteArray FossRecWidget::SaveState()
{
    QByteArray OutArray;
    QDataStream out(&OutArray,QIODevice::WriteOnly);


    out<<LogDirectory;
    out<<LogDirectoryChosen;

    out<<NextItem;
    out<<FossilRecords.count();
    for (int i=0; i<FossilRecords.count(); i++)
    {
        out<<FossilRecords[i]->name;
        out<<FossilRecords[i]->xpos;
        out<<FossilRecords[i]->ypos;
        out<<FossilRecords[i]->sparsity;
        out<<FossilRecords[i]->startage;
        out<<FossilRecords[i]->recording;
        out<<FossilRecords[i]->writtenonce;
        out<<FossilRecords[i]->recorded;
    }
    return OutArray;
}

void FossRecWidget::RestoreState(QByteArray InArray)
{
    QDataStream in(&InArray,QIODevice::ReadOnly);

    in>>LogDirectory;
    in>>LogDirectoryChosen;
    in>>NextItem;

    int itemcount;
    in>>itemcount;

    //remove any old data
    qDeleteAll(FossilRecords);
    FossilRecords.clear();

    for (int i=0; i<itemcount; i++)
    {
        FossilRecord *newfr =  new FossilRecord(0,0,0,"");
        in>>newfr->name;
        in>>newfr->xpos;
        in>>newfr->ypos;
        in>>newfr->sparsity;
        in>>newfr->startage;
        in>>newfr->recording;
        in>>newfr->writtenonce;
        in>>newfr->recorded;
        FossilRecords.append(newfr);
    }
    RefreshMe();
}

void FossRecWidget::SelectedActive(bool s)
{
    QList<bool> selections = GetSelections();
    for (int i=0; i<FossilRecords.count(); i++)
        if (selections[i]) FossilRecords[i]->recording=s;
    RefreshMe();
}

void FossRecWidget::SelectedSparse(int s)
{
    QList<bool> selections = GetSelections();
    for (int i=0; i<FossilRecords.count(); i++)
        if (selections[i]) FossilRecords[i]->sparsity=s;
    RefreshMe();
}
