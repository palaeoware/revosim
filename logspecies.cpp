/**
 * @file
 * Log Species
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

#include "logspecies.h"
#include "simmanager.h"
#include "analysistools.h"
#include <QTextStream>
extern quint64 generation;


#include "mainwindow.h"

LogSpecies::LogSpecies()
{

}

LogSpecies::~LogSpecies()
{
    qDeleteAll(data_items);
    qDeleteAll(children);
}



//Reporting recursive functions below
QString LogSpecies::my_data_line(quint64 start, quint64 end,quint64 myid, quint64 pid)
//dump actually multiple lines of data for this species
{
    QString outstring;
    QTextStream out(&outstring);

    foreach (LogSpeciesDataItem *di, data_items)
    {
        if (di->generation>=start && di->generation<end)
        {
            out<<myid<<","<<pid<<",";
            out<<di->generation<<","<<di->size<<","<<di->sample_genome<<","
              <<AnalysisTools::ReturnBinary(di->sample_genome)<<","
              <<di->genomic_diversity<<","<<di->cells_occupied<<","
             <<di->geographical_range<<","<<di->centroid_range_x<<","<<di->centroid_range_y<<",";
            out<<di->mean_fitness<<",";
            out<<di->min_env[0]<<","<<di->min_env[1]<<","<<di->min_env[2]<<",";
            out<<di->max_env[0]<<","<<di->max_env[1]<<","<<di->max_env[2]<<",";
            out<<di->mean_env[0]<<","<<di->mean_env[1]<<","<<di->mean_env[2]<<"\n";
        }
    }
    return outstring;
}

QString LogSpecies::dump_data(int childindex, quint64 last_time_base, bool killfluff, quint64 parentid)
{
    //modelled on newickstring
    int cc=children.count();
    quint64 myid=ids++;
    if (last_time_base==0) last_time_base=time_of_first_appearance;
    if (cc<=childindex)
        return my_data_line(last_time_base,time_of_last_appearance,myid,parentid);
    else
    {
        int nextchildindex=cc; //for if it runs off the end
        quint64 thisgeneration=0;
        bool genvalid=false;
        for (int i=childindex; i<cc; i++)
        {
            if (!genvalid || children[i]->time_of_first_appearance==thisgeneration)
            {
                if (!(children[i]->isfluff()))
                {
                    genvalid=true;
                    thisgeneration=children[i]->time_of_first_appearance;
                }
            }
            else
            {
                nextchildindex=i;
                break;
            }
        }

       if (!genvalid) return my_data_line(last_time_base,time_of_last_appearance,myid,parentid);

        //now recurse onto (a) this with new settings, and (b) the children
        QString s;
        QTextStream out(&s);
        out<<dump_data(nextchildindex,thisgeneration,killfluff,myid); //my 'offspring'
        for (int i=childindex; i<nextchildindex; i++)
        {

            if (!(children[i]->isfluff()))
            out<<children.at(i)->dump_data(0,thisgeneration,killfluff,myid);
        }
        out<<my_data_line(last_time_base,thisgeneration,myid,parentid);

        return s;
    }
    return "ERROR"; //shouldn't get here!
}


int LogSpecies::maxsize_inc_children()
{
    //find maxsize of this AND all child lineages - for fluff removal
    int recursemaxsize=maxsize;
    for (int i=0; i<children.count(); i++)
    {
        int ms=children[i]->maxsize_inc_children();
        if (ms>recursemaxsize) recursemaxsize=ms;
    }
    return recursemaxsize;
}

bool LogSpecies::isfluff()
{
    if (time_of_first_appearance==time_of_last_appearance) return true; //always fluff if only in one iteration

    //is this a 'fluff' species - i.e. one has no issue and is smaller than minspeciessize?
    //used by filter of newickstring and other recursives
    if (children.count()!=0 && allowexcludewithissue==false)
        return false;

    int recursemaxsize=maxsize;
    if (allowexcludewithissue) recursemaxsize=maxsize_inc_children();

    if (recursemaxsize<=minspeciessize) return true;

    return false;

}

QString LogSpecies::newickstring(int childindex, quint64 last_time_base, bool killfluff)
//childindex is a bodge to allow recursive calls on the same item
//moving up through list of children, grouping by creation time
//send it a 0 initially
//and last_time_base needs to be time_of_first_appearance initially too.
//no, it can be 0 - picked up in functoin
//killfluff flag - ignore any children that have a single generation life and no children of their own
{
    //recursively generate Newick-format text description of tree

    //bl is branch length. For simple nodes - just last appearance time - first
    int cc=children.count();
    int bl;
    quint64 myid=ids++;
    if (last_time_base==0) last_time_base=time_of_first_appearance;
    if (cc<=childindex)
    {
        bl=time_of_last_appearance-last_time_base;
        QString s;
        s.sprintf("ID%ld-%d:%d",myid,maxsize,bl);
        return s;
    }
    else
    {
        int nextchildindex=cc; //for if it runs off the end
        quint64 thisgeneration=0;
        bool genvalid=false;
        for (int i=childindex; i<cc; i++)
        {
            if (!genvalid || children[i]->time_of_first_appearance==thisgeneration)
            {
                if (!(children[i]->isfluff()))
                {
                    genvalid=true;
                    thisgeneration=children[i]->time_of_first_appearance;
                }
            }
            else
            {
                //OK, run too far - i is now next childindex
                nextchildindex=i;
                break;
            }
        }

        if (!genvalid)
        {
            //actually no children
            bl=time_of_last_appearance-last_time_base;
            QString s;
            s.sprintf("ID%ld-%d:%d",myid,maxsize,bl);
            return s;
        }
        bl=thisgeneration-last_time_base;

        //now recurse onto (a) this with new settings, and (b) the children
        QString s;
        QTextStream out(&s);
        out<<"("<<newickstring(nextchildindex,thisgeneration,killfluff);
        for (int i=childindex; i<nextchildindex; i++)
        {
            if (!(children[i]->isfluff()))
            out<<","<<children.at(i)->newickstring(0,thisgeneration,killfluff);
        }
        out<<")ID"<<myid<<"-"<<maxsize<<":"<<bl;
        return s;
    }
    return "ERROR"; //shouldn't get here!
}
