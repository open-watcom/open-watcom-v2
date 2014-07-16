#include "fail.h"

#define catch(n) catch(n &exception)
#define xall (...)
#define catch_all catch xall

void foo() {
    throw 1;
}

int main()
{
    try{
	foo();
    }
    catch_all{
    }
    _PASS;
}

