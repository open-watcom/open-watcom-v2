#define __FAILEX_H
#include "fail.h"
#include "er02.h"

void test( B2I * );

void test( D *p ) {
    p->f3( 9 );
    B2 *q = p;
    test( (B2I *)q );
}
