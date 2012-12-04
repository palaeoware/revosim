#include "environmentclass.h"
#include <QDebug>
#include <QTextStream>
#include <QString>
#include <QFile>
#include <QApplication>
#include <time.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
  To do:
 -- At some point move GUI code to here to get rid of crappy access functions in main
 -- Similarly - add blur option to gui? Pretty easy...
*/



russellenvironment::russellenvironment()
{

    //Setup
   qsrand(time(NULL));
   saveMe=false;
   nseed=4;
   maxsize=14;

   sizevel=5;

   maxvel=.1;
   maxacc=1;

   maxcvel=5;
   converge=.1;
   periodic=true;
   blur = false;
   buffer=1;
   factor=1.;

   initialiseSeeds(nseed);
   //Interpolate
   laplace();
}

void russellenvironment::initialiseSeeds(int number)
{
    for (int i=0;i<number;i++)
          {
          for (int j=0;j<3;j++)seeds[i].colour[j]=Rand8();
          seeds[i].n=(Rand8()*(GRID_X/256.));
          seeds[i].m=(Rand8()*(GRID_Y/256.));
          seeds[i].nv=0.;
          seeds[i].mv=0.;
          int r=Rand8();
          seeds[i].size=(r * maxsize)/256;
          }

}

void russellenvironment::regenerate()
{
    //This code iterates the environment - for each it adds the required factors for an iteration
    for (int i=0;i<nseed;i++)
          {
          //na to be added to velocity n - first come up with this iteration's value
          //+/-RAND    //limit it to max acceleration //apply factor
          na = (Rand8()-128.)*((float)maxacc/128.)*factor;
          //Apply soft limit if velocity is above/below max and acc is in wrong direction//
          if (fabs(seeds[i].nv)>maxvel && (seeds[i].nv*na)>0)na*=(1/((fabs(seeds[i].nv)-maxvel+1)*5));
          // 5 == 'strength' of soft limit

          ma = (Rand8()-128.)*((float)maxacc/128.)*factor;
          if (fabs(seeds[i].mv)>maxvel && (seeds[i].mv*ma)>0)ma*=(1/((fabs(seeds[i].mv)-maxvel+1)*5));

          //Accelerations to apply to nv/mv are now sorted.... Apply next
          seeds[i].nv+=na;
          seeds[i].mv+=ma;

          seeds[i].n+=(seeds[i].nv*factor);
          seeds[i].m+=(seeds[i].mv*factor);
          for (int j=0;j<3;j++)seeds[i].colour[j]+=factor*((double)((Rand8()-128)*((float)maxcvel/128.)));
          for (int j=0;j<3;j++)if((int)seeds[i].colour[j]>255)seeds[i].colour[j]=255.;
          for (int j=0;j<3;j++)if((int)seeds[i].colour[j]<=0)seeds[i].colour[j]=0.;
          seeds[i].size+=factor*((Rand8()-128)*((float)sizevel/128.));


          if(periodic){if(seeds[i].n>(GRID_X-.1))seeds[i].n=0.;
                       if(seeds[i].n<0)seeds[i].n=(GRID_X-.1);}
          else        {if(seeds[i].n>(GRID_X-.1))seeds[i].n=(GRID_X-.1);
                       if(seeds[i].n<0)seeds[i].n=0.;}

          if(periodic){if(seeds[i].m>(GRID_Y-.1))seeds[i].m=0.;
                       if(seeds[i].m<0)seeds[i].m=(GRID_Y-.1);}
          else        {if(seeds[i].m>(GRID_Y-.1))seeds[i].m=(GRID_Y-.1);
                       if(seeds[i].m<0)seeds[i].m=0.;}

          if(seeds[i].size>maxsize)seeds[i].size=maxsize;
          if(seeds[i].size<1)seeds[i].size=1;


          }

    //Interpolation then occurs in the laplace function
     laplace();
}

int russellenvironment::Rand8()
{
    //Randon 8bit number
    return (quint8)(qrand() & 255);
}

//Access functions because I couldn't find a better way to get the UI to communicate with the Russell Environment
//version of the class. There will be one, but I have forgotten polymorphism and was a little too lazy to look it up..

void russellenvironment::nseed_change(int value)
{   
nseed= value;
initialiseSeeds(nseed);
}
void  russellenvironment::buff_change (int value)
{
buffer= value;
}
void russellenvironment::maxsize_change(int value)
{
maxsize= value;
}
void russellenvironment::sizevel_change(int value)
{
sizevel= value;
}
void russellenvironment::maxvel_change(double value)
{
maxvel=value;
}
void russellenvironment::maxcvel_change(int value)
{
maxcvel=value;
}
void russellenvironment::periodic_change(bool value)
{
periodic=value;
}
void russellenvironment::blur_change(bool value)
{
blur=value;
}
void russellenvironment::converge_change(double value)
{
converge=value;
}
void russellenvironment::numbGenerations_change(int value)
{
numbGenerations=value;
}

void russellenvironment::fact_change(double value)
{
factor=value;
}

void russellenvironment::laplace()
{


    //Interpolate
    //First fill colours
    double colourMap[GRID_X][GRID_Y][3];
    //Do it all in double colourMap so don't get errors from using environment (integers)
    int laplace[GRID_X][GRID_Y];
    double eTotal, e[3];
    //Laplacian = residual, total and then residual for R,G and B

    //Initialise colourmap from environment to make laplacian faster
    for (int n=0; n<GRID_X; n++)
       for (int m=0; m<GRID_Y; m++)
            {
            laplace[n][m]=0;
            for (int i=0;i<3;i++)colourMap[n][m][i]=environment[n][m][i];
            }

   double x,y;
   for (int l=0;l<nseed;l++)
   {
   bool templaplace[GRID_X][GRID_Y];
   for (int n=0; n<GRID_X; n++)
      for (int m=0; m<GRID_Y; m++)
           templaplace[n][m]=false;

    for(double z=-PI;z<PI;z+=.01)
    {
            //Draw circles...
            x=seeds[l].n+(seeds[l].size*cos(z));
            y=seeds[l].m+(seeds[l].size*sin(z));

            if(periodic)
                {
                if(y>(GRID_Y-1)) y-=(GRID_Y-1);
                if(y<0)y+=(GRID_Y-1);
                }

            int radius=seeds[l].n-x;

            for(double newX=x;newX<seeds[l].n+radius;newX++)
                {
                if(newX>(GRID_X-1))
                    {
                    if(periodic)
                        {
                        templaplace[(int)newX-(GRID_X-1)][(int)y]=true;
                        for (int i=0;i<3;i++)colourMap[(int)newX-(GRID_X-1)][(int)y][i]=seeds[l].colour[i];
                        }
                    }

                else if(newX<0)
                    {
                    if(periodic)
                        {
                        templaplace[(int)newX+(GRID_X-1)][(int)y]=true;
                        for (int i=0;i<3;i++)colourMap[(int)newX+(GRID_X-1)][(int)y][i]=seeds[l].colour[i];
                        }
                    }

                else
                    {
                    if(y>0 && y<GRID_Y)
                        {
                        templaplace[(int)newX][(int)y]=true;
                        for (int i=0;i<3;i++)colourMap[(int)newX][(int)y][i]=seeds[l].colour[i];
                        }
                    }
                }

             }

    //Create laplace matrix which counts how many spots are overlapping in any given area
    for (int n=0; n<GRID_X; n++)
       for (int m=0; m<GRID_Y; m++)
           if(templaplace[n][m])laplace[n][m]++;

    if(buffer==0) //But not if buffer is set to zero - easy way of turning off system
        for (int n=0; n<GRID_X; n++)
           for (int m=0; m<GRID_Y; m++)
                if (laplace[n][m]>1)laplace[n][m]=1;

    if(blur) //Set all pixels to laplace
        for (int n=0; n<GRID_X; n++)
           for (int m=0; m<GRID_Y; m++)
                laplace[n][m]=0;
   }


//Dilate overlapped selection if needed
    for (int n=0; n<GRID_X; n++)
      for (int m=0; m<GRID_Y; m++)
          if(laplace[n][m]>1)
                //Current implementation is simple square which enlarges each overlapping pixel by amount buffer
                for (int i=(n-buffer);i<(n+buffer);i++)
                    for (int j=(m-buffer);j<(m+buffer);j++)
                        if(!periodic && i>0 && j>0 && i<(GRID_X-1) && j<(GRID_Y-1) && laplace[i][j]==1)laplace[i][j]=-1;
                        else laplace[(i+GRID_X)%GRID_X][(j+GRID_Y)%GRID_Y]=-1;

//Now smooth/interpolate
    int count=0;
    do
    {
            eTotal=0.0;//This is the residual
            for (int n=0; n<GRID_X; n++)
               for (int m=0; m<GRID_Y; m++)
                            if ((n+m)%2==count%2)//Calculate it chess-board style
                                if (laplace[n][m]!=1)//If needs to be laplaced
                                          for (int i=0;i<3;i++)
                                            { //Average difference surounding four pixels. Modulus to make periodic. Calculate laplacian residual.
                                                    if(periodic)e[i]=colourMap[(n+1)%(GRID_X-1)][m][i]+colourMap[(n-1+(GRID_X-1))%(GRID_X-1)][m][i]+
                                                            colourMap[n][(m+1)%(GRID_Y-1)][i]+colourMap[n][(m-1+(GRID_Y-1))%(GRID_Y-1)][i]
                                                                    -4.0*colourMap[n][m][i];
                                                    else{
                                                        if(n==0&&m==0)e[i]=(colourMap[n][m+1][i]+colourMap[n+1][m][i])-2*colourMap[n][m][i];
                                                        else if (n==(GRID_X-1)&&m==0)e[i]=(colourMap[n-1][m][i]+colourMap[n][m+1][i])-2*colourMap[n][m][i];
                                                        else if (n==0&&m==(GRID_Y-1))e[i]=(colourMap[n][m-1][i]+colourMap[n+1][m][i])-2*colourMap[n][m][i];
                                                        else if (n==(GRID_X-1)&&m==(GRID_Y-1))e[i]=(colourMap[n-1][m][i]+colourMap[n][m-1][i])-2*colourMap[n][m][i];
                                                        else if (n==0)e[i]=(colourMap[n][m+1][i]+colourMap[n][m-1][i]+colourMap[n+1][m][i])-3*colourMap[n][m][i];
                                                        else if (m==0)e[i]=(colourMap[n][m+1][i]+colourMap[n-1][m][i]+colourMap[n+1][m][i])-3*colourMap[n][m][i];
                                                        else if (n==(GRID_X-1))e[i]=(colourMap[n][m+1][i]+colourMap[n-1][m][i]+colourMap[n][m-1][i])-3*colourMap[n][m][i];
                                                        else if (m==(GRID_Y-1))e[i]=(colourMap[n-1][m][i]+colourMap[n+1][m][i]+colourMap[n][m-1][i])-3*colourMap[n][m][i];
                                                        else e[i]=colourMap[n+1][m][i]+colourMap[n-1][m][i]+
                                                             colourMap[n][m+1][i]+colourMap[n][m-1][i]
                                                                     -4.0*colourMap[n][m][i];
                                                        }
                                                  colourMap[n][m][i]+=1.2*e[i]/4.0;//Colour = average of surrounding pixels
                                                   //1.2 == factor to speed up the calculation. I can't find correct value.
                                                   eTotal+=fabs(e[i]);
                                            }


            count++;
            eTotal=eTotal/(3.0*((double) GRID_X)*((double) GRID_Y));

            //Ideally still need to implement some kind of status bar / update
            /*if (count%1000==0)
            {
            QString prog = QString("Residual is currently %1 ").arg(eTotal);
            ui->statusBar()->showMessage(prog);
            }*/
    }
  while (eTotal>converge);

  for (int n=0; n<GRID_X; n++)
       for (int m=0; m<GRID_Y; m++)
           for(int i=0; i<3; i++)
             {
                environment[n][m][i]=(quint8)colourMap[n][m][i];
             }

}
