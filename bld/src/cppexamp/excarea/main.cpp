// EXCAREA main
//
// Throw and catch an 8K object to demonstrate that exceptions area
// has been extended.

#include <stdio.h>

struct object {
    char array[ 8 * 1024 ];
};


int main()
{
    int retn;
    try {
        object obj;
        obj.array[0] = 'o';
        obj.array[ sizeof( obj.array ) - 1 ] = 'k';
        throw obj;
    } catch( object const & ob ) {
        if( ob.array[0] == 'o'
         && ob.array[ sizeof( ob.array ) - 1 ] == 'k' ) {
            puts( "Passed\n" );
            retn = 0;
        } else {
            puts( "FAILED: bad object\n" );
            retn = 1;
        }
    } catch( ... ) {
        puts( "FAILED to catch object\n" );
        retn = 1;
    }
    return retn;
}

