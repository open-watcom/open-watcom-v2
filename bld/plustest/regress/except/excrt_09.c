// excrt_09 - throw from terminate
#include <stdio.h>
#include <except.h>

void my_term()
{
    throw "crap";
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
