/**
 * @file
 * resize Catcher
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
#include "resizecatcher.h"

/*!
 * \brief ResizeCatcher::ResizeCatcher
 * \param parent
 *
 *Exists to act as an event filter and catch resize of mainwidget of main window - avoid needs to subclass the widget while still catching dock resizes
 */
ResizeCatcher::ResizeCatcher(QObject *parent) :
    QObject(parent)
{
}

bool ResizeCatcher::eventFilter(QObject *obj, QEvent *event)
{
    //if it's a resize event - do resize via Main Window
    if (event->type() == QEvent::Resize)
        mainWindow->resize();

    return QObject::eventFilter(obj, event);
}

