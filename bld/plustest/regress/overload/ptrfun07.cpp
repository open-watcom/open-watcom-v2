#include "dump.h"

int f(double) {GOOD; return 0;};
int f(int){GOOD; return 0;};

//(int (*)(int)) &f;		// cast expression as selector
int (*pfd)(double) = &f;	// select f(double)
int (*pfi)(int) = &f;		// selects f(int)
int (&rfi)(int) = f;		// selects f(int)
int (&rfd)(double) = f;		// selects f(double)

int main()
{
    pfd(2.0);
    CHECK_GOOD(3);
    pfi(2);
    CHECK_GOOD(3+4);
    rfi(2);
    CHECK_GOOD(3+4+4);
    rfd(2.0);
    CHECK_GOOD(3+4+4+3);
    return errors != 0;

}


// from [over.over]  (13.4)
