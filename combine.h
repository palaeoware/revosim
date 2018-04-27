#ifndef COMBINE_H
#define COMBINE_H

class combine : public environmentclass
{
public:
    combine();
    void regenerate();

private:
   void ReadSettings();
   void combineImages(QImage sOne, double pOne, QImage sTwo);
   QDir stackOne, stackTwo;
   int start, end, currentGen, percentStart, percentEnd;
};

#endif // COMBINE_H
