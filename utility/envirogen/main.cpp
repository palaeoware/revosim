/**
 * @file
 * Source: Main
 *
 * All REvoSim code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All REvoSim code is Copyright 2008-2019 by Mark Sutton, Russell Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include <QApplication>

#include <QSplashScreen>
#include <QString>
#include <QStyle>
#include <QTimer>

#include "mainwindow.h"
#include "globals.h"
#include "darkstyletheme.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Style program with our dark style
    a.setStyle(new DarkStyleTheme);

    //Splash image
    QPixmap splashPixmap(":/palaeoware_logo_square_Envirogen.png");
    QSplashScreen *splash = new QSplashScreen(splashPixmap, Qt::WindowStaysOnTopHint);
    splash->show();
    splash->showMessage("<font><b>" + QString(PRODUCTNAME) + " - " + QString(PRODUCTTAG) + "</b></font>", Qt::AlignHCenter, Qt::white);
    QApplication::processEvents();
    QTimer::singleShot(5000, splash, SLOT(close()));

    MainWindow w;
    w.show();

    return a.exec();
}
