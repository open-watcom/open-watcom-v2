



/*
 *
 *          Copyright (C) 1994, M. A. Sridhar
 *  
 *
 *     This software is Copyright M. A. Sridhar, 1994. You are free
 *     to copy, modify or distribute this software  as you see fit,
 *     and to use  it  for  any  purpose, provided   this copyright
 *     notice and the following   disclaimer are included  with all
 *     copies.
 *
 *                        DISCLAIMER
 *
 *     The author makes no warranties, either expressed or implied,
 *     with respect  to  this  software, its  quality, performance,
 *     merchantability, or fitness for any particular purpose. This
 *     software is distributed  AS IS.  The  user of this  software
 *     assumes all risks  as to its quality  and performance. In no
 *     event shall the author be liable for any direct, indirect or
 *     consequential damages, even if the  author has been  advised
 *     as to the possibility of such damages.
 *
 */





#include "base/strsplit.h"


CL_StringSplitter::CL_StringSplitter (const CL_String& s, const char* fld)
: _string (s), _index (-1), _fieldSeps (fld)
{
}

void CL_StringSplitter::Reset ()
{
    _index = -1;
}



CL_String CL_StringSplitter::Next (const char* fld)
{
    if (_index >= _string.Length())
        return "";
    if (fld)
        _fieldSeps = fld;
    long spos = _string.NCharIndex (_fieldSeps.AsPtr(), _index+1);
    if (spos < 0) {
        _index = _string.Length();
        return "";
    }
    long epos = _string.CharIndex (_fieldSeps.AsPtr(), spos);
    if (epos < 0) {
        _index = _string.Length()-1;
        return CL_String (_string._string + spos);
    }
    _index = epos-1;
    return CL_String (_string._string + spos, epos-spos);
}

CL_String CL_StringSplitter::Remaining ()
{
    long l = _index+1;
    _index = _string.Length()-1;
    return CL_String (_string._string + l);
}

CL_String CL_StringSplitter::Scan (const char* char_set)
{
    if (!char_set || _index >= _string.Length())
        return  "";
    long l = _string.NCharIndex (char_set, _index+1);
    if (l > _index) {
        CL_String tmp = ((CL_String&)_string) (_index, l - _index);
        _index = l-1;
        return tmp;
    }
    return "";
}


