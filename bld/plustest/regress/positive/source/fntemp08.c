#include "fail.h"
#include <stdio.h>


template <class T>
    void swap( T&l, T&r )
    {
	T t; t=l;l=r;r=t;
    }

int __near i, __near j;
#if defined(__LARGE__) || defined(__COMPACT__) || defined(__HUGE__)
int __far ii, __far jj;
#endif

int main()
{
    i = 1;
    j = 2;
    swap( i, j );
    if( i != 2 || j != 1 ) fail(__LINE__);
#if defined(__LARGE__) || defined(__COMPACT__) || defined(__HUGE__)
    ii = 3;
    jj = 4;
    swap( ii, jj );
    if( ii != 4 || jj != 3 ) fail(__LINE__);
#endif
    _PASS;
}
