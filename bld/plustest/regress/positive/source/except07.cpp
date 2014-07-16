#include "fail.h"
#include <stdio.h>

void foo()
{
    throw 0;
}

int main()
{
    int x=0;
    try {
	x++;
	foo();
    } catch(...) {
	if (x!=1) fail(__LINE__);
    }
    _PASS;
}
