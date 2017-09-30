#ifndef _scanner_h
#define _scanner_h

/* $Log:        SCANNER.H $
 * Revision 1.1  92/08/20  15:53:14  Anthony_Scian
 * .
 *
Revision 1.1  92/08/20  15:53:12  Anthony_Scian
.

 * Revision 1.1  1992/08/20  17:14:07  peter
 * Initial revision
 *
 */

#include <iostream.h>
#include "useful.h"

class Scanner {
private:
    streambuf           &in;
    uint                curLine;
    char                *buffer, *tokBuf, *cursor, *limit;
    uint                bufLen;
public:
    int                 token;
    uint                tokLine;
private:
    bool fill();
    bool need(uint);
    int scan();
public:
    Scanner(streambuf&);
    void fatal(char*);
    int peek();
    void copy(ostream&);
    void bump();
    SubString tokStr();
};

const uint cChunkSize   = 256;

enum {
    tIdent,
    tString,
    tCClass,
    tCode,

    tLParen,
    tRParen,
    t0orMore,
    t1orMore,
    tOptional,
    tChoice,
    tEqual,
    tSemi,
    tColon,
    tBang,
    tContext,
    tDiff,

    tEnd,

    tEOF,
    tOther
};

inline bool Scanner::need(uint n){
    if((limit - cursor) < n)
        return fill();
    else
        return true;
}

inline int Scanner::peek()
    { return token; }
inline void Scanner::bump()
    { token = scan(); }

inline SubString Scanner::tokStr()
    { return SubString(tokBuf, cursor - tokBuf); }

#endif
