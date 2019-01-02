#ifndef COLOUR_H
#define COLOUR_H

#include <QColor>

class colour : public environmentclass
{
public:
    colour();
    void regenerate();

private:
    void readSettings();
    int colourRed, colourGreen, colourBlue;
};

#endif // COLOUR_H
