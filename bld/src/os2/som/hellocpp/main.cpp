#include <hello.xh>

int main( int argc, char **argv )
{
    /* Declare a variable to point to an instance of Hello */
    Hello *obj;

    /* Create an instance of the Hello class */
    obj = new Hello;

    /* Execute the "sayHello" method */
    obj->sayHello( somGetGlobalEnvironment() );

    obj->somFree();
    return( 0 );
}
