// excrt_05 - run out of space
//
// 95/09/07 -- J.W.Welch    -- use 20k for sizeof(S)
#include <stdio.h>
#include <except.h>
#include <limits.h>

struct S {
    char a[20*1024];
};
S s;

void fun()
{
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
