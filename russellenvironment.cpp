#include "environmentclass.h"
#include <QDebug>
#include <QTextStream>
#include <QString>
#include <QFile>
#include <time.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
  To do:
      -- Implement background colour - this will initialse the image to the background colour selected.
         It should only have an effect if convergence is set to low, so I'm not entirely sure if it's worth implementing.
         I guess it will be if we want abrupt environment boundaries to investigate what these do...
*/


russellenvironment::russellenvironment()
{

    //Setup
   qsrand(time(NULL));
   saveMe=false;
   nseed=4;
   maxsize=14;
   initialiseSeeds(nseed);



   sizevel=5;
   maxvel=5;
   maxcvel=5;
   converge=.1;
   periodic=true;

   //Interpolate
   laplace();
}

void russellenvironment::initialiseSeeds(int number)
{
    for (int i=0;i<number;i++)
          {
          for (int j=0;j<3;j++)seeds[i].colour[j]=Rand8();
          seeds[i].n=(int)(Rand8()*(GRID_X/256.));
          seeds[i].m=(int)(Rand8()*(GRID_Y/256.));
          int r=Rand8();
          seeds[i].size=(r * maxsize)/256;
          }

}

void russellenvironment::regenerate()
{
    //This code iterates the environment - for each it adds the required factors for an iteration
    for (int i=0;i<nseed;i++)
          {
          for (int j=0;j<3;j++)seeds[i].colour[j]+=((double)((Rand8()-128)*((float)maxcvel/128.)));
          for (int j=0;j<3;j++)if((int)seeds[i].colour[j]>255)seeds[i].colour[j]=255.;
          for (int j=0;j<3;j++)if((int)seeds[i].colour[j]<=0)seeds[i].colour[j]=0.;
          seeds[i].n+=(int)((Rand8()-128)*((float)maxvel/128.));
          if(periodic){if(seeds[i].n>(GRID_X-1))seeds[i].n=0;
                       if(seeds[i].n<1)seeds[i].n=(GRID_X-1);}
          else        {if(seeds[i].n>(GRID_X-1))seeds[i].n=(GRID_X-1);
                       if(seeds[i].n<1)seeds[i].n=0;}
          seeds[i].m+=(int)((Rand8()-128)*((float)maxvel/128.));
          if(periodic){if(seeds[i].m>(GRID_Y-1))seeds[i].m=0;
                       if(seeds[i].m<1)seeds[i].m=(GRID_Y-1);}
          else        {if(seeds[i].m>(GRID_Y-1))seeds[i].m=(GRID_Y-1);
                       if(seeds[i].m<1)seeds[i].m=0;}
          seeds[i].size+=(int)((Rand8()-128)*((float)sizevel/128.));
          if(seeds[i].size>maxsize)seeds[i].size=maxsize;
          if(seeds[i].size<1)seeds[i].size=1;
          }

    //Then interpolation occurs in the laplace function
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
void russellenvironment::maxsize_change(int value)
{
maxsize= value;
}
void russellenvironment::sizevel_change(int value)
{
sizevel= value;
}
void russellenvironment::maxvel_change(int value)
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
void russellenvironment::converge_change(double value)
{
converge=value;
}
void russellenvironment::numbGenerations_change(int value)
{
numbGenerations=value;
}


void russellenvironment::laplace()
{


    //Interpolate
    //First fill colours
    double colourMap[GRID_X][GRID_Y][3];
    //Do it all in double colourMap so don't get errors from using environment (integers)
    bool laplace[GRID_X][GRID_Y];
    double eTotal, e[3];
    //Laplacian = residual, total and then residual for R,G and B

    //Initialise colourmap from environment to make laplacian faster
    for (int n=0; n<GRID_X; n++)
       for (int m=0; m<GRID_Y; m++)
            {
            laplace[n][m]=true;
            for (int i=0;i<3;i++)colourMap[n][m][i]=environment[n][m][i];
            }

   double x,y;
   for (int l=0;l<nseed;l++)
   {
    for(double z=-PI;z<PI;z+=.01)
    {
            //Draw circles...
            x=seeds[l].n+(seeds[l].size*cos(z));
            y=seeds[l].m+(seeds[l].size*sin(z));

            if(periodic)
                {
                if(y>(GRID_Y-1))y-=(GRID_Y-1);
                if(y<0)y+=(GRID_Y-1);
                }

                int radius=seeds[l].n-x;

                for(double newX=x;newX<seeds[l].n+radius;newX++)
                    {
                    if(newX>(GRID_X-1))
                        {
                        if(periodic)
                            {
                            laplace[(int)newX-(GRID_X-1)][(int)y]=false;
                            for (int i=0;i<3;i++)colourMap[(int)newX-(GRID_X-1)][(int)y][i]=seeds[l].colour[i];
                            }
                   }
                    else if(newX<0)
                        {
                        if(periodic)
                            {
                            laplace[(int)newX+(GRID_X-1)][(int)y]=false;
                            for (int i=0;i<3;i++)colourMap[(int)newX+(GRID_X-1)][(int)y][i]=seeds[l].colour[i];
                            }
                        }
                    else{
                        if(y>0 && y<GRID_Y)
                            {
                            laplace[(int)newX][(int)y]=false;
                            for (int i=0;i<3;i++)colourMap[(int)newX][(int)y][i]=seeds[l].colour[i];
                            }
                        }
                    }

             }

    }

//Now smooth/interpolate
    int count=0;
    do
    {
            eTotal=0.0;
            for (int n=0; n<GRID_X; n++)
               for (int m=0; m<GRID_Y; m++)
                            if ((n+m)%2==count%2)
                            //Calculate it chess-board style
                            {
                                 if (laplace[n][m])//If needs to be laplaced
                                    {
                                            for (int i=0;i<3;i++)
                                            { //Average difference four pixels above and to sides. Modulus to make periodic == laplacian residual
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
                                                  colourMap[n][m][i]+=1.2*e[i]/4.0;//Colour = average
                                                   //1.2 = factor to speed up - can't find correct value.
                                                   eTotal+=fabs(e[i]);
                                            }
                                    }
                            }

            count++;
            eTotal=eTotal/(3.0*((double) GRID_X)*((double) GRID_Y));
    }
  while (eTotal>converge);

  for (int n=0; n<GRID_X; n++)
       for (int m=0; m<GRID_Y; m++)
           for(int i=0; i<3; i++)
             {
                environment[n][m][i]=(quint8)colourMap[n][m][i];
             }

}

/* Old code for square seeds
for (int n=0; n<GRID_X; n++)
   for (int m=0; m<GRID_Y; m++)
            {
                    test=false;
                    int seedmatch;
                    for (int l=0;l<NSEED;l++)//test if seed
                            if ((n>=seeds[l].n && n<=(seeds[l].n+seeds[l].size)) && (m>=seeds[l].m && m<=(seeds[l].m+seeds[l].size)))
                            {
                                    test=true;
                                    seedmatch=l;
                                    break;
                            }
                    if (test)//if true make colour=seed
                            for (int i=0;i<3;i++)colourMap[n][m][i]=seeds[seedmatch].colour[i];
                    else//else set colour to average R/G/B level for the thing
                            for (int i=0;i<3;i++)colourMap[n][m][i]=environment[n][m][i];//0;//e[i];
                    //e is average, 0 is black
            }

    int x[50],y[50],size[50];//size=base multiplier
    int r[50],g[50],b[50];//influence factors

    for (int i=0;i<50;i++)
    {
    r[i]=(TheSimManager->Rand8()-128)*.4;
    g[i]=(TheSimManager->Rand8()-128)*.4;
    b[i]=(TheSimManager->Rand8()-128)*.4;

    size[i]=(TheSimManager->Rand8()*0.234375)+70;
qDebug()<<size[i];
    x[i]=(int)(TheSimManager->Rand8()*0.390625);
    y[i]=(int)(TheSimManager->Rand8()*0.390625);
    //qDebug()<<r[i]<<g[i]<<b[i]<<x[i]<<y[i];
    }

    for (int n=0; n<GRID_X; n++)
       for (int m=0; m<GRID_Y; m++)
       {

       int tr=128,tg=128,tb=128, flag=0;

       for (int i=0;i<50;i++)
            {
            double distance=sqrt((double)(((n-x[i])*(n-x[i]))+((m-y[i])*(m-y[i]))));
            tr+=(int)(((size[i]/(distance/500))*r[i])/1000);//random number for now
            tg+=(int)(((size[i]/(distance/500))*g[i])/1000);
            tb+=(int)(((size[i]/(distance/500))*b[i])/1000);
            if(x[i]==n&&y[i]==m)
                {
                flag=i;
                }
            }
//qDebug()<<n<<m<<tr<<tg<<tb<<"*";
       if(tr>256)tr=256;if(tg>256)tg=256;if(tb>256)tb=256;
       if(tr<0)tr=0;if(tg<0)tg=0;if(tb<0)tb=0;
//qDebug()<<n<<m<<tr<<tg<<tb;
       //if(flag==0)
           {
           environment[n][m][0]=(quint8)tr;
           environment[n][m][1]=(quint8)tg;
           environment[n][m][2]=(quint8)tb;
           qDebug()<<n<<m<<tr<<tg<<tb;
            }
       else {
            for (int i=0;i<50;i++)
                    {
                    if(flag==i)
                            {
                            environment[n][m][0]=(quint8)r[i];
                            environment[n][m][1]=(quint8)g[i];
                            environment[n][m][2]=(quint8)b[i];
                            }
                        }
            }

       }


    int r;

           for (int n=0; n<GRID_X; n++)
           for (int m=0; m<GRID_Y; m++)
           {
               if (m>70) r=0; else {if (m<30) r=200; else r=100;}
               environment[n][m][0]=(quint8)r;
               environment[n][m][1]=(quint8)r;
               environment[n][m][2]=(quint8)r;
           }
*/


