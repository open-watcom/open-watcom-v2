#ifndef _token_h
#define	_token_h


//Revision 1.1  1994/04/08  15:27:59  peter
//Initial revision
//

#include "substr.h"

class Token {
  public:
    Str			text;
    uint		line;
  public:
    Token(SubStr, uint);
};

inline Token::Token(SubStr t, uint l) : text(t), line(l) {
    ;
}

#endif
