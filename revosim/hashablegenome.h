#ifndef HASHABLEGENOME_H
#define HASHABLEGENOME_H

#include <QtGlobal>

class HashableGenome
{
public:
    HashableGenome(quint32 *g, int length);
    HashableGenome(int size);
    HashableGenome(const HashableGenome &from);

    ~HashableGenome();
    void copyFrom(HashableGenome *from);
    HashableGenome& operator=(HashableGenome other);

    quint32 *genome;
    int length;
private:
    bool ownsData;
    quint32 *mallocData();
    void copyDataFrom(HashableGenome *from);
};

inline bool operator==(const HashableGenome &e1, const HashableGenome &e2)
{
    for (int i=0; i<e1.length; i++)
    {
        if (e1.genome[i]!=e2.genome[i]) return false;
    }
    return true;
}

inline uint qHash(const HashableGenome& key)
{
    return key.genome[0]; //hash is just first gene
}


#endif // HASHABLEGENOME_H
