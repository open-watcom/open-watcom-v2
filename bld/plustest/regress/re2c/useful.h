#ifndef _useful_h
#define _useful_h

// Copyright (C) 1992 Peter Bumbulis.  All rights reserved.
/* $Log:	USEFUL.H $
Revision 1.1  92/08/20  15:50:30  Anthony_Scian
.

 * Revision 1.1  1992/08/20  17:14:07  peter
 * Initial revision
 *
 */

#include <iostream.h>
#include "basics.h"

class SubString {
public:
    char		*str;
    uint		len;
public:
    friend bool operator==(const SubString&,const SubString&);
    SubString(uchar*, uint);
    SubString(char*, uint);
    SubString(const SubString&);
    void out(ostream&) const;
};

inline ostream& operator<<(ostream& o, const SubString& s){
    s.out(o);
    return o;
}

inline ostream& operator<<(ostream& o, const SubString* s){
    return o << *s;
}

inline SubString::SubString(uchar *s, uint l)
    : str((char*) s), len(l) { }

inline SubString::SubString(char *s, uint l)
    : str(s), len(l) { }

inline SubString::SubString(const SubString &s)
    : str(s.str), len(s.len) { }

class String: public SubString {
public:
    String(const SubString&);
    String(String&);
    String();
    ~String();
};

#endif
