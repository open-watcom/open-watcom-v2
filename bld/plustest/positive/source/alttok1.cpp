#include "fail.h"
#include <string.h>

#define s1(x) %:x
#define s2(x) s1(x)

#if defined( and ) || defined( and_eq ) || defined( bitor ) \
 || defined( or_eq ) || defined( or ) || defined( xor_eq ) \
 || defined( xor ) || defined( not ) || defined( compl ) \
 || defined( not_eq ) || defined( bitand )
#error alternative tokens shouldn't be defined
#endif

#if ( 0 and 1 ) or ( not 0 )
#else
#error alternative tokens don't appear to work for the preprocessor
#endif

struct A {
    compl A();
};

A::compl A()
{ }

int main() {
    A a;

    // check stringization of alternative tokens and digraphs
    char c1[] = s2(and);
    if( strcmp( c1, "and" ) ) fail(__LINE__);

    char c2[] = s2(t <% %>);
    if( strcmp( c2, "t <% %>" ) ) fail(__LINE__);

    int i = ( 2 bitor 5 ) bitand 11;
    if( i != 3 ) fail(__LINE__);
    if( ( i bitand compl 1 ) != 2 ) fail(__LINE__);

    bool b = ( ( not ( false xor true ) ) or true ) and true;
    if( ! b ) fail(__LINE__);

    b xor_eq true;
    if( b ) fail(__LINE__);

    b or_eq true;
    if( ! b ) fail(__LINE__);

    b and_eq false;
    if( b ) fail(__LINE__);
    if( b not_eq false ) fail(__LINE__);

    _PASS;
}
