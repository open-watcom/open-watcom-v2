#include "fail.h"

double d1[] = {
    /*
    fractional-constant:
    	digit-seq .
    	digit-seq . digit-seq
    	          . digit-seq
    */
    12.,
    12.12,
    .12,
    12e12,
    12.f,
    12.12f,
    .12f,
    12e12f,
    12.F,
    12.12F,
    .12F,
    12e12F,
    12.L,
    12.12L,
    .12L,
    12e12L,
    12.l,
    12.12l,
    .12l,
    12e12l,

    12.e+12,
    12.12e+12,
    .12e+12,
    12e+12,
    12.e-12f,
    12.12e-12f,
    .12e-12f,
    12e+12f,
    12.e-12F,
    12.12e-12F,
    .12e-12F,
    12e-12F,
    12.e-12L,
    12.12e-12L,
    .12e-12L,
    12e-12L,
    12.e-12l,
    12.12e-12l,
    .12e-12l,
    12e-12l,
};
#define VALS \
    12., \
    12.12, \
    .12, \
    12e12, \
    12.f, \
    12.12f, \
    .12f, \
    12e12f, \
    12.F, \
    12.12F, \
    .12F, \
    12e12F, \
    12.L, \
    12.12L, \
    .12L, \
    12e12L, \
    12.l, \
    12.12l, \
    .12l, \
    12e12l, \
 \
    12.e+12, \
    12.12e+12, \
    .12e+12, \
    12e+12, \
    12.e-12f, \
    12.12e-12f, \
    .12e-12f, \
    12e+12f, \
    12.e-12F, \
    12.12e-12F, \
    .12e-12F, \
    12e-12F, \
    12.e-12L, \
    12.12e-12L, \
    .12e-12L, \
    12e-12L, \
    12.e-12l, \
    12.12e-12l, \
    .12e-12l, \
    12e-12l, \

double d2[] = {
    VALS
};

double d3[] = {
12,
12.12,
0.12,
1.2e+013,
12,
12.12,
0.12,
1.2e+013,
12,
12.12,
0.12,
1.2e+013,
12,
12.12,
0.12,
1.2e+013,
12,
12.12,
0.12,
1.2e+013,
1.2e+013,
1.212e+013,
1.2e+011,
1.2e+013,
1.2e-011,
1.212e-011,
1.2e-013,
1.2e+013,
1.2e-011,
1.212e-011,
1.2e-013,
1.2e-011,
1.2e-011,
1.212e-011,
1.2e-013,
1.2e-011,
1.2e-011,
1.212e-011,
1.2e-013,
1.2e-011,
};

#define ARRAY_SIZE( n )	(sizeof(n)/sizeof(n[0]))

void closeEnough( double v1, double v2, double v3 ) {
    double v = v1+v2+v3;
    double r = v-v1*3 + v-v2*3 + v-v3*3;
    if( r < 0 ) r = -r;
    if( r > .01 ) _fail;
}

int main() {
    int i;

    if( ARRAY_SIZE(d1) != ARRAY_SIZE(d2) || ARRAY_SIZE(d2)!=ARRAY_SIZE(d3) ) _fail;
    for( i = 0; i < ARRAY_SIZE(d1); ++i ) {
	closeEnough( d1[i], d2[i], d3[i] );
    }
    _PASS;
}
