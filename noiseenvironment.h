#ifndef NOISEENVIRONMENT_H
#define NOISEENVIRONMENT_H

class noiseenvironment :  public environmentclass
{
public:
    noiseenvironment();
    void regenerate();

private:
    void ReadSettings();
    int nMin,nMax;
};

#endif // NOISEENVIRONMENT_H
