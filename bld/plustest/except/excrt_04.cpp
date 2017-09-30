// excrt_04 - test copy CTOR throw during construction
#include <stdio.h>
#include <except.h>

struct S
{   S();
    S( const S& );
};

S::S()
{
}

S::S( const S& )
{
    throw 123L;     // throw a long to cause error
}

void fun()
{
    S s;
    throw s;
}

int main()
{
    try {
        fun();
    } catch( S ) {
        printf( "FAILURE -- should have aborted\n" );
	fflush( stdout );
    }
    return 0;
}
