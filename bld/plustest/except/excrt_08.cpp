// excrt_08 - violation of function exception spec.
#include <stdio.h>
#include <except.h>

void foo() {
    throw 123;
}

int main() throw()
{
    foo();
    return( 0 );
}
