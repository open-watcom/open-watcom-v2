#include "fail.h"

void f(int i, int levels)
{
    if (i <= 1) {
        throw 1;
    }
    try {
        f(i-1, levels + 1);
    } catch (int c1) {
        try {
            f(i-2, levels + 1);
        } catch (int c2) {
            throw c1+c2;
        }
    }
}

int main()
{
    int fm2 = 0;
    int fm1 = 1;

    for( static int i; i < 10; ++i ) {
        try {
            f(i+1, 1);
        } catch (int r1) {
	    if( r1 != ( fm1+fm2 ) ) fail(__LINE__);
	    fm2 = fm1;
	    fm1 = r1;
        }
    }
    _PASS;
}
