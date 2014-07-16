// excrt_01 - test return from "unexpected"
#include <stdio.h>
#include <except.h>

void my_unexp()
{
    printf( "my_unexp\n" );
    fflush( stdout );
}

void test_unexp() throw()
{
    throw 1234;
}

int main()
{
    set_unexpected( &my_unexp );
    test_unexp();
    return 0;
}
