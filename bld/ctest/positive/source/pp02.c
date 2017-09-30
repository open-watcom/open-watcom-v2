// 3.8.3.5 ANSI C standard
#include "fail.h"
#include <string.h>

#define x 3
#define f(a) f(x*(a))
#undef x
#define x 2
#define g f
#define z z[0]
#define h g(~
#define m(a) a(w)
#define w 0,1
#define t(a) a

#define str(s) # s
#define xstr(s) str(s)
#define xxstr(s) xstr(s)

char *p1 = xstr(f(y+1) + f(f(z)) % t(t(g)(0) + t)(1););
char *p2 = xxstr(g(x+(3,4)-w) | h 5) & m (f)^m(m););
char *chk1 = "f(2 * (y+1)) + f(2 * (f(2 * (z[0])))) % f(2 * (0)) + t(1);";
char *chk2 = "f(2 * (2+(3,4)-0,1)) | f(2 * (~ 5)) & f(2 * (0,1))^m(0,1);";

int bcomp( const char *p, const char *q ) {
    for(;;) {
        while( *p == ' ' ) {
            ++p;
        }
        while( *q == ' ' ) {
            ++q;
        }
        if( *p != *q ) {
            return( 1 );
        }
        if( *p == '\0' ) {
            return( 0 );
        }
        ++p;
        ++q;
    }
}

int main()
{
    unsigned errors = 0;
    if( bcomp( p1, chk1 ) != 0 ) {
        puts( p1 );
        puts( chk1 );
        fail(__LINE__);
    }
    if( bcomp( p2, chk2 ) != 0 ) {
        puts( p2 );
        puts( chk2 );
        fail(__LINE__);
    }
    _PASS;
}
