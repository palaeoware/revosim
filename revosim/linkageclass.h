#ifndef LINKAGECLASS_H
#define LINKAGECLASS_H

#include <QString>
#include <QStringList>
#include <QMessageBox>

#include <imagesequence.h>

class linkageClass
{
public:
    linkageClass(QString linkedVariable);
    linkageClass();
    //The images used to control variable
    ImageSequence *imageSequence;
    //Their mode - using same lookup as environment
    int mode;
    //Whether images interpolate
    bool interpolate;
    //Whether this particular linkage has been set - there is a list of one for each variable implemented
    bool set;
    //Which variable - used to write the options combo box
    QString variable;
};

#endif // LINKAGECLASS_H
