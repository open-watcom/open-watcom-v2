#include "fail.h"
#include <string.h>

typedef char ch_type[];
unsigned curr_len;
void use( char *p )
{
    unsigned len = strlen( p );
    if( len != curr_len ) _fail;
    ++curr_len;
}
void foo1() {
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
void foo2() {
    ch_type a1 = "";
    ch_type a2 = "x";
    ch_type a3 = "xx";
    ch_type a4 = "xxx";
    ch_type a5 = "xxxx";
    ch_type a6 = "xxxxx";
    ch_type a7 = "xxxxxx";
    ch_type a8 = "xxxxxxx";
    ch_type a9 = "xxxxxxxx";

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
    foo1();
    curr_len = 0;
    foo2();
    _PASS;
}
