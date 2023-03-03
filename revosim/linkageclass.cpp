#include "linkageclass.h"

linkageClass::linkageClass(QString linkedVariable)
{
    //Their mode - using same lookup as environment
    mode = ENV_MODE_STATIC;
    interpolate = true;
    set = false;
    variable = linkedVariable;
    imageSequence = nullptr;
}

linkageClass::linkageClass()
{
    //Their mode - using same lookup as environment
    mode = ENV_MODE_STATIC;
    interpolate = true;
    set = false;
    variable = "temp";
    imageSequence = nullptr;
}

