////NOEXPEHEADER////
//-------------------------------------------------------------------------------
// The ObjFormat library
// Copyright (C) 2007  Gael Guennebaud
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// http://www.gnu.org/licenses/lgpl.html
//-------------------------------------------------------------------------------

#include "ObjUtil.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#ifndef _WIN32
#include <dirent.h>
#endif
#include <cstdlib>
//#include <QFile>

std::vector<ObjString> ObjString::split( const ObjString& delims /*= "\t\n "*/) const
{
    std::vector<ObjString> ret;
    unsigned int numSplits = 0;
    size_t start, pos;
    start = 0;
    do
    {
        pos = find_first_of(delims, start);
        if (pos == start)
        {
            ret.push_back("");
            start = pos + 1;
        }
        else if (pos == npos)
            ret.push_back( substr(start) );
        else
        {
            ret.push_back( substr(start, pos - start) );
            start = pos + 1;
        }
        //start = find_first_not_of(delims, start);
        ++numSplits;
    } while (pos != npos);
    return ret;
}

void ObjString::trim( bool left /*= true*/, bool right /*= true*/ )
{
    int lspaces, rspaces, len = length(), i;
    lspaces = rspaces = 0;

    if ( left )
        for (i=0; i<len && (at(i)==' '||at(i)=='\t'||at(i)=='\r'||at(i)=='\n'); ++lspaces,++i);

    if ( right && lspaces < len )
        for(i=len-1; i>=0 && (at(i)==' '||at(i)=='\t'||at(i)=='\r'||at(i)=='\n'); rspaces++,i--);

    *this = substr(lspaces, len-lspaces-rspaces);
}

int ObjString::toInt(void) const
{
    return atoi(c_str());
}
float ObjString::toFloat(void) const
{
    return atof(c_str());
}
ObjVector3 ObjString::toVector3(void) const
{
    std::vector<ObjString> elements = split();
    if (elements.size() != 3)
        return ObjVector3(0,0,0);
    else
        return ObjVector3(elements[0].toFloat(),elements[1].toFloat(),elements[2].toFloat());
}
ObjVector2 ObjString::toVector2(void) const
{
    std::vector<ObjString> elements = split();
    if (elements.size() != 2)
        return ObjVector2(0,0);
    else
        return ObjVector2(elements[0].toFloat(),elements[1].toFloat());
}

void ObjString::decomposePathAndFile(ObjString& path, ObjString& filename) const
{
    std::vector<ObjString> elements = this->split("/\\");
    path = "";
    filename = elements.back();
    elements.pop_back();
    if (this->at(0)=='/')
        path = "/";
    for (unsigned int i=0 ; i<elements.size() ; ++i)
        path += elements[i] + "/";
}

bool findFile(ObjString& completefilename)
{
    std::ifstream ifs;
    ifs.open(completefilename);
    if (!ifs)
    {
        // perform a case insensitive search
        ObjString path;
        ObjString filename;
        completefilename.decomposePathAndFile(path,filename);
        
#ifndef _WIN32
        DIR* dir = opendir(path.c_str());
        for (struct dirent* dp = readdir(dir); dp!=NULL; dp = readdir(dir))
        {
            if (filename.isEquiv(dp->d_name))
            {
                completefilename = path + ObjString(dp->d_name);
                closedir(dir);
                return true;
            }
        }
        closedir(dir);
#endif
        return false;
    }
    ifs.close();
    return true;
}
