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
    char a2[] = "x";
    char a3[] = "xx";
    char a4[] = "xxx";
    char a5[] = "xxxx";
    char a6[] = "xxxxx";
    char a7[] = "xxxxxx";
    char a8[] = "xxxxxxx";
    char a9[] = "xxxxxxxx";

    use( a1 );
    use( a2 );
    use( a3 );
    use( a4 );
    use( a5 );
    use( a6 );
    use( a7 );
    use( a8 );
    use( a9 );
}
int main() {
    foo();
    _PASS;
}
