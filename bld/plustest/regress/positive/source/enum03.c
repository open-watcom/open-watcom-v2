#include "fail.h"
#include <ctype.h>

enum Boolean {
    FALSE=0, TRUE=(1==1),
};

class test{
    char exch;
public:
    test(char ch)       { exch = ch; }
    Boolean islow()     { return Boolean(islower(exch)); }
    Boolean iscap()     { return Boolean(!islow()); }
};

int main() {
    test u('A');
    test l('a');

    if( ! u.iscap() ) fail(__LINE__);
    if( u.islow() ) fail(__LINE__);
    if( l.iscap() ) fail(__LINE__);
    if( ! l.islow() ) fail(__LINE__);
    _PASS;
}
