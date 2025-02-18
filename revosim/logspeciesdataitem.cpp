/**
 * @file
 * Log Spieces Data Item
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

#include "logspeciesdataitem.h"
#include <QTextStream>
#include <QString>

LogSpeciesDataItem::LogSpeciesDataItem()
    = default;

/**
 * @brief LogSpeciesDataItem::sharedCSVoutput
 *
 * Output most of the data as a csv string - this is shared with both types of log, so it's separated out. Does not provide an initial comma or a terminal newline.
 *
 * @return
 */
const QString LogSpeciesDataItem::sharedCSVoutput()
//output most of the data - this is shared with both logs, so it's separated out. Initial comma exists, terminal newline done elsewhere.
//Headers for this are diversity,cellsOccupied,geog_range,centroid_x,centroid_y,mean_fit,min_env_red,min_env_green,min_env_blue,max_env_red,max_env_green,max_env_blue,mean_env_red,mean_env_green,mean_env_blue
{
    //return string below
    QString s;
    QTextStream out(&s);

    out << genomicDiversity << "," << cellsOccupied << "," << geographicalRange << "," << centroidRangeX << "," << centroidRangeY << ",";
    out << meanFitness << ",";
    out << minEnvironment[0] << "," << minEnvironment[1] << "," << minEnvironment[2] << ",";
    out << maxEnvironment[0] << "," << maxEnvironment[1] << "," << maxEnvironment[2] << ",";
    out << meanEnvironment[0] << "," << meanEnvironment[1] << "," << meanEnvironment[2];
    return s;
}

/**
 * @brief LogSpeciesDataItem::sharedCSVoutput
 *
 * Static to provide headers for the data in sharedCSVoutput as a QString. Does not provide an initial comma or a terminal newline.
 *
 * @return
 */
QString LogSpeciesDataItem::headersForSharedOutput()
{
    return "diversity,cellsOccupied,geog_range,centroid_x,centroid_y,mean_fit,min_env_red,min_env_green,min_env_blue,max_env_red,max_env_green,max_env_blue,mean_env_red,mean_env_green,mean_env_blue";
}
