#include "fail.h"

int ctors;

struct CD {
    CD(int x = 0) : cond(x) {++ctors;};
    CD(CD const &s ) : cond(s.cond) {++ctors;};
    ~CD(){--ctors;};
    int cond;
};

CD ack()
{
    static int ctr;
    ++ctr;
    return CD(ctr);
}

#define LIMIT 4

int i;

int main() {
    {
	while( ( ack().cond / (LIMIT+2) ) < LIMIT ) {
	    while( ( ack().cond % (LIMIT+2) )  < LIMIT ) {
		++i;
	    }
	    ++i;
	}
    }
    if( ctors != 0 ) fail(__LINE__);
    if( ack().cond != 30 ) fail(__LINE__);
    _PASS;
}
