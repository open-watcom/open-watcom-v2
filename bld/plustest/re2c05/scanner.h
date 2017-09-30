#ifndef _scanner_h
#define	_scanner_h


//Revision 1.1  1994/04/08  15:27:59  peter
//Initial revision
//

#include "token.h"

class Scanner {
  private:
    int			in;
    uchar		*bot, *tok, *ptr, *cur, *pos, *lim, *top, *eof;
    uint		tchar, tline, cline;
  private:
    uchar *fill(uchar*);
  public:
    Scanner(int);
    int echo(ostream&);
    int scan();
    void fatal(char*);
    SubStr token();
    uint line();
};

inline SubStr Scanner::token(){
    return SubStr(tok, cur - tok);
}

inline uint Scanner::line(){
    return cline;
}

#endif
