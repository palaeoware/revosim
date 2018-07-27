/**
 * @file
 * Header: Reseed Widget
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

#ifndef RESEED_H
#define RESEED_H

#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include <QRadioButton>

namespace Ui {
class reseed;
}

class reseed : public QDialog
{
    Q_OBJECT

public:
    explicit reseed(QWidget *parent = 0);
    ~reseed();

    QList<QRadioButton *> radios;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void radio_toggled();

private:
    Ui::reseed *ui;
};

#endif // RESEED_H
