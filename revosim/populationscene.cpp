/**
 * @file
 * Population Scene
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
#include "populationscene.h"
#include "simmanager.h"

#include <QDebug>
#include <QGraphicsView>
#include <QPointF>


/*!
 * \brief PopulationScene::PopulationScene
 *
 * Subclassed QGraphicsScene object - basically just has mouse handlers for main window
 */

PopulationScene::PopulationScene()
{
    selectedX = 0;
    selectedY = 0;
}

/*!
 * \brief PopulationScene::doMouse
 * \param x
 * \param y
 * \param button
 */
void PopulationScene::doMouse(int x, int y, int button)
{
    //ARTS - Genome Comparison Dock uses right mouse button for selection of grid cell
    if (button == 2 && x >= 0 && x < simulationManager->simulationSettings->gridX && y >= 0 && y < simulationManager->simulationSettings->gridY)
    {
        selectedX = x;
        selectedY = y;
        mainWindow->genomeComparisonAdd();
    }
}

/*!
 * \brief PopulationScene::mouseMoveEvent
 * \param event
 *
 * This gets string of calls if mouse button is held
 */
void PopulationScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF position = event->scenePos();
    int x, y;

    x = (int)position.x();
    y = (int)position.y();
    int but = 0;
    if (event->button() == Qt::LeftButton) but = 1;
    if (event->button() == Qt::RightButton) but = 2;
    if (but > 0) doMouse(x, y, but);
}

/*!
 * \brief PopulationScene::mousePressEvent
 * \param event
 */
void PopulationScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF position = event->scenePos();
    int x, y;

    x = (int)position.x();
    y = (int)position.y();

    int but = 0;
    if (event->button() == Qt::LeftButton) but = 1;
    if (event->button() == Qt::RightButton) but = 2;

    doMouse(x, y, but);
}

/*!
 * \brief PopulationScene::mouseReleaseEvent
 * \param event
 */
void PopulationScene::mouseReleaseEvent (QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    //Don't do anything
}
