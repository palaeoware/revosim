#include "randoms.h"

randoms::randoms()
{
    load_randoms();
    nextrandom = 0;
}

quint16 randoms::gen_Rand16()
{
    if (nextrandom == 65535) {
        nextrandom = 0;
        load_randoms();
    }
    return random_array[nextrandom++];

}

quint8 randoms::gen_Rand8()
{
    if (nextrandom == 65535) {
        nextrandom = 0;
        load_randoms();
    }
    return (quint8)(random_array[nextrandom++] & 255);

}

double randoms::gen_float()
{
    return (double)gen_Rand16() / (double)65535;
}

randoms::~randoms()
{


}

//Load random numbers
void randoms::load_randoms()
{
    //Seed pseudorandom numbers
    qsrand(QTime::currentTime().msec());

    //Load randoms into program - get random
    int seed_random = qrand();

    //Make sure 16 bit random so not too large for the randoms file
    float seedoffset = ((float)seed_random / (float)RAND_MAX) * (65536.);
    int intseedoffset = (int)seedoffset;

    QFile rfile(":/randoms.dat");
    if (!rfile.exists())QMessageBox::warning(0, "Damn", "Error loading randoms. Contact RJG in the hope he can sort this out.");
    rfile.open(QIODevice::ReadOnly);
    rfile.seek(intseedoffset);

    // multiply by two here as char * is 8bit, and randoms are 16 - no error if array is unfilled, randoms are just zero...
    int i = rfile.read((char *)random_array, (65536 * 2));
    if (i != (65536 * 2)) QMessageBox::warning(0, "Oops", "Failed to read 131072 bytes from randoms file - random numbers may be compromised - Contact RJG");
    rfile.close();
}

