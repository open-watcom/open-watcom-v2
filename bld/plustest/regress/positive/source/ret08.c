// Test that PODS return is stuffed into memory before being pointed
// at by this on a subsequent call.

#include "fail.h"

const int V1 = 637;
const int V2 = 963;

struct S {
    int v1;
    int v2;
    void foo();
};


void S::foo()
{
    if( v1 != V1 || v2 != V2 ) {
        printf( __FILE__ " failed on line %s\n", __LINE__ );
        ++errors;
    }
}

S sv;


S goop()
{
    return sv;
}


int main()
{
    sv.v1 = V1;
    sv.v2 = V2;
    (goop()).foo();
    if( 0 == errors ) {
//        printf( "PASS " __FILE__ "\n" );
    } else {
        printf( "FAILED " __FILE__ "\n" );
    }
    return errors;
}
