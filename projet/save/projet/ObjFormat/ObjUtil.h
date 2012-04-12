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

#ifndef _ObjUtil_h_
#define _ObjUtil_h_

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

/** Represent a 2 components floating point vector
*/
class ObjVector2
{
public:

    /** x, y components and their aliases
    */
    union {
        struct {float x, y;};
        struct {float s, t;};
    };
    
    ObjVector2() {}
    ObjVector2(float aX, float aY) : x(aX), y(aY) {}
    operator float* () {return &x;}
    operator const float* () const {return &x;}
};

/** Represent a 3 components floating point vector
*/
class ObjVector3
{
public:
    
    /** x, y, z components and their aliases
    */
    union {
        struct {float x, y, z;};
        struct {float r, g, b;};
        struct {float s, t, q;};
    };
    
    ObjVector3() {}
    ObjVector3(float aX, float aY, float aZ) : x(aX), y(aY), z(aZ) {}
    explicit ObjVector3(float s) : x(s), y(s), z(s) {}
    operator float* () {return &x;}
    operator const float* () const {return &x;}
};

typedef std::string _ObjStringBase;

/** Enhanced string
*/
class ObjString : public _ObjStringBase
{
public:
    ObjString() : _ObjStringBase() {}
    ObjString(const ObjString& str) : _ObjStringBase(static_cast<const _ObjStringBase&>(str)) {}
    ObjString(const _ObjStringBase& str) : _ObjStringBase(str) {}
    ObjString(const char* str) : _ObjStringBase(str) {}

    operator const char* () const { return c_str(); }

    void trim( bool left = true, bool right = true );
    
    std::vector<ObjString> split( const ObjString& delims = "\t\n ") const;
    
    bool endsWith(const ObjString& str) const
    {
        if(str.size()>this->size())
            return false;
        return this->substr(this->size()-str.size(),str.size()) == str;
    }
    bool contains(const ObjString& str) const
    {
        return this->find(str)<this->size();
    }
    bool beginsWith(const ObjString& str) const
    {
        if(str.size()>this->size())
            return false;
        return this->substr(0,str.size()) == str;
    }

    ObjString toLowerCase( void )
    {
        std::transform(begin(), end(), begin(), static_cast<int(*)(int)>(::tolower) );
        return *this;
    }
    ObjString toUpperCase( void )
    {
        std::transform(begin(), end(), begin(), static_cast<int(*)(int)>(::toupper) );
        return *this;
    }

    /** Case insensitive comparison.
    */
    bool isEquiv(const ObjString& str) const
    {
        ObjString str0 = *this;
        str0.toLowerCase();
        ObjString str1 = str;
        str1.toLowerCase();
        return str0 == str1;
    }
    
    /** Decompose the current string as a path and a file.
        For instance: "dir1/dir2/file.ext" leads to path="dir1/dir2/" and filename="file.ext"
    */
    void decomposePathAndFile(ObjString& path, ObjString& filename) const;
    
    /** Append any value to the string (assumming the << operator exist for the template type T)
    */
    template< typename T > ObjString& operator << (T value)
    {
        std::stringstream sstr;
        sstr.str(*this);
        sstr.seekp(0, std::ios_base::end);
        sstr << value;
        *this = _ObjStringBase( sstr.str() );
        return *this;
    }
    
    /// \name Conversion
    //@{
    int toInt(void) const;
    float toFloat(void) const;
    ObjVector3 toVector3(void) const;
    ObjVector2 toVector2(void) const;
    //@}
};

/** Performs a case-insensitive search of completefilename and modify it if a match exists.
    \note Only the name of the file is tested in a case insitive fashion. The path is not changed.
*/
extern bool findFile(ObjString& completefilename);

#endif
