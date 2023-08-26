/**
 * @file
 * Header: MakeStack header
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


#ifndef MAKESTACK_H
#define MAKESTACK_H

#include "environmentclass.h"

class makestack : public EnvironmentClass
{
public:
    makestack(EnvironmentSettings constructorSettings);
    void regenerate();

private:
    QString fileName;

};

#endif // MAKESTACK_H
