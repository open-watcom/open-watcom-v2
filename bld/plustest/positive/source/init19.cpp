#include "fail.h"
#include <string.h>

unsigned curr_len;
void use( char *p )
{
    unsigned len = strlen( p );
    if( len != curr_len ) _fail;
    ++curr_len;
}
void foo() {
    char a1[] = "";
    use( a1 );
    char a2[] = "x";
    use( a2 );
    char a3[] = "xx";
    use( a3 );
    char a4[] = "xxx";
    use( a4 );
    char a5[] = "xxxx";
    use( a5 );
    char a6[] = "xxxxx";
    use( a6 );
    char a7[] = "xxxxxx";
    use( a7 );
    char a8[] = "xxxxxxx";
    use( a8 );
    char a9[] = "xxxxxxxx";
    use( a9 );
}
int main() {
    foo();
    _PASS;
}
