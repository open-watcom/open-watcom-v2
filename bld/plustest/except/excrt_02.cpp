// excrt_02 - test return from "terminate"
#include <stdio.h>
#include <except.h>

void my_term()
{
    printf( "my_term\n" );
    fflush( stdout );
}

void test_term() throw()
{
    throw 1234;
}

int main()
{
    set_terminate( &my_term );
    test_term();
    return 0;
}
