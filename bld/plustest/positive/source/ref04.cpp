#include "fail.h"

void foo( int *p, int *q, int *r )
{
    int sum = 0;
    for( int i = 0; i < 5; ++i ) {
	sum += p[i] + q[i] + r[i];
    }
    if( sum != 60 ) {
	fail(__LINE__);
    }
}

int main()
{
   int x[5] ;
   int (&r)[5] = x ;
   int (&r2)[5] = *&x ;
   for( int i = 0; i < 5; ++i ) {
       x[i] = i;
       r[i] = i + 1;
       r2[i] = i + 2;
   }
   foo( x, r, r2 );
   _PASS;
}
