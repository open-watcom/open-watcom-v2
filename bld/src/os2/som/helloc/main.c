#include <hello.h>

int main( int argc, char **argv )
{
    /* Declare a variable to point to an instance of Hello */
    Hello   *obj;

    /* Create an instance of the Hello class */
    obj = HelloNew();

    /* Execute the "sayHello" method */
    _sayHello( obj, somGetGlobalEnvironment() );

    /* Free the instance: */
    _somFree( obj );
    return( 0 );
}
