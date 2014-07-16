// excrt_03 - test DTOR during stack unwinding
#include <stdio.h>
#include <except.h>

struct S
{   ~S();
};

S::~S()
{
    throw 123L;     // throw a long
}

void fun()
{
    S s;            // this should be DTOR'ed during unwind
    throw 123;      // throw an int
}

int main()
{
    try {
        fun();
    } catch( int ) {
        printf( "FAILURE -- should have aborted\n" );
    }
    return 0;
}
