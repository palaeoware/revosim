/**
 * @file
 * Environment Scene
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

#include "environmentscene.h"
#include "simmanager.h"
#include <QGraphicsView>
#include <QDebug>
#include <QPointF>
#include "mainwindow.h"

/*********************************************
Subclassed QGraphicsScene object -
basically just has mouse handlers for main window
**********************************************/
EnvironmentScene::EnvironmentScene() : QGraphicsScene()
{
    button=0;
    grabbed=-1;
}

void EnvironmentScene::DoMouse(int x, int y)
{
    if (button==1 && x>=0 && x<gridX && y>=0 && y<gridY)
    {
        if (grabbed==-1)
        {
            int closestFR = MainWin->FRW->findclosest(x,y,6);
            if (closestFR>=0)
            {
                //grab one
                grabbed=closestFR;
            }
        }

        if (grabbed!=-1)
        {
            MainWin->FRW->FossilRecords[grabbed]->xpos=x;
            MainWin->FRW->FossilRecords[grabbed]->ypos=y;
            MainWin->FRW->RefreshMe();
        }
    }


}

void EnvironmentScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
//this gets string of calls if mouse button is held
{
    QPointF position=event->scenePos();
    int x,y;

    x=(int)position.x();
    y=(int)position.y();

    if (button!=0) DoMouse(x,y);

    return;
}


void EnvironmentScene::mousePressEvent(QGraphicsSceneMouseEvent * event )
{
    QPointF position=event->scenePos();
    int x,y;

    x=(int)position.x();
    y=(int)position.y();

    button=0;
    if (event->button()==Qt::LeftButton) button=1;
    if (event->button()==Qt::RightButton) button=2;

    DoMouse(x,y);
    return;
}

void EnvironmentScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    QPointF position=event->scenePos();
    int x,y;

    grabbed=-1;

    x=(int)position.x();
    y=(int)position.y();

    Q_UNUSED(x);
    Q_UNUSED(y);

    return;
}

void EnvironmentScene::DrawLocations(QList <FossilRecord *> frlist, bool show)
{
    if (show)
    {
        QList <bool> selecteds = MainWin->FRW->GetSelections();
        //add any new items needed
        while (frlist.count()>HorizontalLineList.count())
        {
            QGraphicsLineItem *temp1=new QGraphicsLineItem;
            HorizontalLineList.append(temp1);
            QGraphicsLineItem *temp2=new QGraphicsLineItem;
            VerticalLineList.append(temp2);
            QGraphicsSimpleTextItem *temp3=new QGraphicsSimpleTextItem;
            LabelsList.append(temp3);
            addItem(temp1);
            addItem(temp2);
            addItem(temp3);
        }
        //now remove any spurious ones
        while (frlist.count()<HorizontalLineList.count())
        {
            QGraphicsLineItem *r1=HorizontalLineList.takeLast();
            QGraphicsLineItem *r2=VerticalLineList.takeLast();
            QGraphicsSimpleTextItem *t=LabelsList.takeLast();
            removeItem(r1);
            removeItem(r2);
            removeItem(t);
        }

        //now set them up
        for (int i=0; i<frlist.count(); i++)
        {
            HorizontalLineList[i]->setLine(frlist[i]->xpos-.5,frlist[i]->ypos+.5, frlist[i]->xpos+1.5,frlist[i]->ypos+.5);
            VerticalLineList[i]->setLine(frlist[i]->xpos+.5,frlist[i]->ypos-.5, frlist[i]->xpos+.5,frlist[i]->ypos+1.5);
            HorizontalLineList[i]->setVisible(selecteds[i]);
            VerticalLineList[i]->setVisible(selecteds[i]);
            VerticalLineList[i]->setZValue(i+1);
            HorizontalLineList[i]->setZValue(i+1);

            LabelsList[i]->setText(frlist[i]->name);
            LabelsList[i]->setX(frlist[i]->xpos+1);
            LabelsList[i]->setY(frlist[i]->ypos-2);
            LabelsList[i]->setFont(QFont("Arial",12));
            LabelsList[i]->setScale(.15);
            LabelsList[i]->setVisible(selecteds[i]);
            LabelsList[i]->setZValue(i+1);

            //sort out colours
            int bright = (int)(environment[frlist[i]->xpos][frlist[i]->ypos][0])
                    + (int)(environment[frlist[i]->xpos][frlist[i]->ypos][1])
                    + (int)(environment[frlist[i]->xpos][frlist[i]->ypos][2]);


            if (bright>250)
            {
                VerticalLineList[i]->setPen(QPen(Qt::black));
                HorizontalLineList[i]->setPen(QPen(Qt::black));
                LabelsList[i]->setBrush(QBrush(Qt::black));
            }
            else
            {
                VerticalLineList[i]->setPen(QPen(Qt::white));
                HorizontalLineList[i]->setPen(QPen(Qt::white));
                LabelsList[i]->setBrush(QBrush(Qt::white));
            }
        }
    }
    else
    for (int i=0; i<VerticalLineList.count(); i++)
    {
            HorizontalLineList[i]->setVisible(false);
            VerticalLineList[i]->setVisible(false);
            LabelsList[i]->setVisible(false);
    }
}
