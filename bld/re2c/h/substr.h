/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _substr_h
#define _substr_h

#include <iostream>
#include "basics.h"

class SubStr {
public:
    char                *str;
    uint                len;
public:
    friend bool operator==(const SubStr, const SubStr);
    SubStr(uchar*, uint);
    SubStr(char*, uint);
    SubStr(const SubStr&);
    void out(std::ostream&) const;
};

class Str: public SubStr {
public:
    Str(const SubStr&);
    Str(Str&);
    Str();
    ~Str();
};

inline std::ostream& operator<<(std::ostream& o, const SubStr s){
    s.out(o);
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const SubStr* s){
    return o << *s;
}

inline SubStr::SubStr(uchar *s, uint l)
    : str((char*) s), len(l) { }

inline SubStr::SubStr(char *s, uint l)
    : str(s), len(l) { }

inline SubStr::SubStr(const SubStr &s)
    : str(s.str), len(s.len) { }

#endif
