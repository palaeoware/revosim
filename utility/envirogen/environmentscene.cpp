
/*********************************************

Subclassed QGraphicsScene object -
basically just has mouse handlers for main window


**********************************************/

#include "environmentscene.h"
#include <QGraphicsView>
#include <QPointF>
#include "mainwindow.h"

EnvironmentScene::EnvironmentScene() : QGraphicsScene()
{
    ;
}

/*
void EnvironmentScene::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
//this gets string of calls if mouse button is held
{

    QPointF position=event->scenePos();

    int x,y;

    x=(int)position.x();
    y=(int)position.y();

    //TODO - do something!
    return;
}


void EnvironmentScene::mousePressEvent(QGraphicsSceneMouseEvent * event )
{
    QPointF position=event->scenePos();
    int x,y;

    x=(int)position.x();
    y=(int)position.y();

    int but=0;
    if (event->button()==Qt::LeftButton) but=1;
    if (event->button()==Qt::RightButton) but=2;

    //TODO - do something!
    return;
}

void EnvironmentScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    QPointF position=event->scenePos();
    int x,y;

    x=(int)position.x();
    y=(int)position.y();

    //TODO - do something!
    return;
}*/


