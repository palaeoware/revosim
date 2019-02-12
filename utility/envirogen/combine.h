/**
 * @file
 * Header: Combine
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


#ifndef COMBINE_H
#define COMBINE_H

class combine : public environmentclass
{
public:
    combine();
    void regenerate();

private:
    void ReadSettings();
    void combineImages(QImage sOne, double pOne, QImage sTwo);
    QDir stackOne, stackTwo;
    int start, end, currentGen, percentStart, percentEnd;
    bool combine_error;
};

#endif // COMBINE_H
