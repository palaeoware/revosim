#ifndef MAKESTACK_H
#define MAKESTACK_H

class makestack : public environmentclass
{
public:
    makestack();
    void regenerate();

    private:
        void readSettings();
        QString filename;

};

#endif // MAKESTACK_H
