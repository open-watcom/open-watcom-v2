#include <stdio.h>

char *p1 = "abc";
char *p2 = "abc" + 1;
char *p3 = 1 + "abc";

char *r[] = {
    "abc",
    "def" + 1,
    2 + "ghi"
};
int main( void ) {
    puts( p1 );
    puts( p2 );
    puts( p3 );

    puts( r[0] );
    puts( r[1] );
    puts( r[2] );
    return 0;
}
