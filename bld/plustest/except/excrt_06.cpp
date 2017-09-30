// excrt_06 - no handler
#include <stdio.h>
#include <except.h>

void foo() {
    throw 8;
}

int main()
{
    foo();
    return 0;
}
