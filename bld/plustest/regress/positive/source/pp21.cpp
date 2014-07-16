#include "fail.h"
#include <string.h>

void ack( char *m1, char *m2, char *c1, char *c2, unsigned line )
{
    if( strcmp( m1, c1 ) ) fail( line );
    if( strcmp( m2, c2 ) ) fail( line );
}

#define foo( a, b, c, d )	ack( #a, #b, c, d, __LINE__ )

#define bar()	ack( "!", "@", "!", "@", __LINE__ )

#define sam( a, b )	ack( #a, #b, #a, #b, __LINE__ )

int main() {
    foo(a,b,"a","b");
    foo(,c,"","c");
    foo(d,,"d","");
    foo(,,"","");
    sam(w,x);
    sam(,y);
    sam(z,);
    sam(,);
    bar();
    _PASS;
}
