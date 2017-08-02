#include "fail.h"
#include <iostream.h>
#include <strstrea.h>

double d[] = {
    0,
    1,
    1,
    0.55555555555555555555555,
    0.5138707,
    0.1757256,
    0.3086337,
    6.250000001,
    355,
};

char *c[] = {
"1.000000",
"0.031250",
"0.078125",
"0.555556",
"0.513871",
"0.175726",
"0.308634",
"6.250000",
"3.141593",
};

int main() {
    int i;
    double t = 1.0 / 2.0;
    for( i = 0; i < 100; ++i ) {
	d[0] += t;
	t /= 2;
    }
    d[1] /= 2*2*2*2*2;
    d[2] /= 2*2*2*2*2*2*2;
    d[2] *= 10;
    d[8] /= 113;
    for( int j = 0; j < (sizeof(d)/sizeof(d[0])); ++j ) {
	ostrstream cout;
	cout.setf( ios::fixed );
	cout << d[j] << ends;
	if( strcmp( cout.str(), c[j] ) ) {
	    printf( "[%d] '%s' (should be '%s')\n", j, cout.str(), c[j] );
	    fail(__LINE__);
	}
    }
    _PASS;
}
