#include "dump.h"
union U {
    char a;
    int b;
};

U u = { 'a' };	// good
U v = u;	// good
int main( void )
{
    DUMP( u );
    DUMP( v );
    return 0;
}
