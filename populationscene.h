/**
 * @file
 * Header: Population Scene
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

#ifndef POPULATIONSCENE_H
#define POPULATIONSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QString>

class MainWindow;

class PopulationScene : public QGraphicsScene
{
public:
        PopulationScene();
        int selectedx;
        int selectedy;
        MainWindow *mw;

protected:
     void mousePressEvent(QGraphicsSceneMouseEvent *event);
     void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
     void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
     void DoMouse(int x, int y, int button);
 private slots:
     void ScreenUpdate();
};


#endif // POPULATIONSCENE_H
