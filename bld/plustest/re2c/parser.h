#ifndef _parser_h
#define _parser_h

/* $Log:        PARSER.H $
Revision 1.1  92/08/20  15:50:30  Anthony_Scian
.

 * Revision 1.1  1992/08/20  17:14:07  peter
 * Initial revision
 *
 */

#include "scanner.h"
#include "re.h"

class Symbol {
public:
    static Symbol       *first;
    Symbol              *next;
    String              name;
    RegExp              *re;
public:
    Symbol(const SubString&);
    static Symbol *find(const SubString&);
};

void parse(istream&, ostream&);

#endif
