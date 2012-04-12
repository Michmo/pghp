#ifndef PGMREADER_H
#define PGMREADER_H

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

class PgmReader
{
public:
    PgmReader(char *filename);
    ~PgmReader();

    bool readPgmFile();

    /* Getters & setters */
    unsigned short getPixValue(int i);
    unsigned short getPixValue(int i, int j);
    vector<unsigned short> getPixValues();
    int getHeight();
    int getWidth();

protected:
    char *filename;
    int width;
    int height;
    vector<unsigned short> pixValues;
};

#endif // PGMREADER_H
