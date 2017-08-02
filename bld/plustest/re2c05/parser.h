#ifndef _parser_h
#define _parser_h


//Revision 1.1  1994/04/08  15:27:59  peter
//Initial revision
//

#include "scanner.h"
#include "re.h"

class Symbol {
public:
    static Symbol	*first;
    Symbol		*next;
    Str			name;
    RegExp		*re;
public:
    Symbol(const SubStr&);
    static Symbol *find(const SubStr&);
};

void parse(int, ostream&);

#endif
