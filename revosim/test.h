#ifndef TEST_H
#define TEST_H

#include "simmanager.h"
#include "mainwindow.h"
#include "reseed.h"
#include<QBitArray>
#include <bitset>
#include <QRegularExpression>

class test
{
public:
    test();
    bool callTest(int i, QString &outString);
private:
    bool testZero(QString &outString);
    bool testOne(QString &outString);
    bool testTwo(QString &outString);
    bool testThree(QString &outString);
    bool testFour(QString &outString);
    bool testFive(QString &outString);
    bool testSix(QString &outString);
    bool testSeven(QString &outString);
    bool testEight(QString &outString);
    bool testNine(QString &outString);
    bool testTen(QString &outString);
    bool testEleven(QString &outString);
    bool testTwelve(QString &outString);
    bool testThirteen(QString &outString);
    bool testFourteen(QString &outString);
    bool testFifteen(QString &outString);
    bool testSixteen(QString &outString);
    bool testSeventeen(QString &outString);
    bool testEighteen(QString &outString);
    bool testNineteen(QString &outString);
    bool testTwenty(QString &outString);
};

#endif // TEST_H
