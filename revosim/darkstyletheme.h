/**
 * @file
 * Header: Dark Style Theme
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

#ifndef DARKSTYLETHEME_H
#define DARKSTYLETHEME_H

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QProxyStyle>
#include <QStyleFactory>

class DarkStyleTheme : public QProxyStyle
{
    Q_OBJECT

public:
    DarkStyleTheme();

    explicit DarkStyleTheme(QStyle *style);

    void polish(QPalette &palette) override;
    void polish(QApplication *app) override;
    QStyle *baseStyle() const;

private:
    QStyle *styleBase(QStyle *style = Q_NULLPTR) const;
};

#endif // DARKSTYLETHEME_H
