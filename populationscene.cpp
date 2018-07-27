/**
 * @file
 * Population Scene
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

#include "populationscene.h"
#include "simmanager.h"
#include <QGraphicsView>
#include <QDebug>
#include <QPointF>
#include "mainwindow.h"

/*********************************************
Subclassed QGraphicsScene object -
basically just has mouse handlers for main window
**********************************************/
PopulationScene::PopulationScene() : QGraphicsScene()
{
    selectedx = 0;
    selectedy = 0;
}

void PopulationScene::DoMouse(int x, int y, int button)
{
    //ARTS - Genome Comparison Dock uses right mouse button for selection of grid cell
    if (button == 2 && x >= 0 && x < gridX && y >= 0 && y < gridY) {
        selectedx = x;
        selectedy = y;
        mainWindow->genomeComparisonAdd();
    }
}

void PopulationScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
//this gets string of calls if mouse button is held
{
    QPointF position = event->scenePos();
    int x, y;

    x = (int)position.x();
    y = (int)position.y();
    int but = 0;
    if (event->button() == Qt::LeftButton) but = 1;
    if (event->button() == Qt::RightButton) but = 2;
    if (but > 0) DoMouse(x, y, but);
}


void PopulationScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF position = event->scenePos();
    int x, y;

    x = (int)position.x();
    y = (int)position.y();

    int but = 0;
    if (event->button() == Qt::LeftButton) but = 1;
    if (event->button() == Qt::RightButton) but = 2;

    DoMouse(x, y, but);
}

void PopulationScene::mouseReleaseEvent (QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    //don't do anything
}
