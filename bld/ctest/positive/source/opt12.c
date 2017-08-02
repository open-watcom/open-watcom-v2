// PT160259
#include "fail.h"

int cc_iter(
	int n,
	int maxn,
	int a2_0,
	int a2_1,
        int a5_0,
	int a5_1,
	int a5_2,
	int a5_3,
	int a5_4,
        int a10_0,
	int a10_1,
	int a10_2,
	int a10_3,
	int a10_4,
        int a10_5,
	int a10_6,
	int a10_7,
	int a10_8,
	int a10_9)
{
    int new_a2_0, new_a5_0, new_a10_0;

    return
	    ( n <= maxn )
	?
	    new_a2_0 = a2_0+n,
	    new_a5_0=a5_0+new_a2_0,
	    new_a10_0=a10_0+new_a5_0,
	    cc_iter(n+1,
		maxn,
		a2_1,
		new_a2_0,
		a5_1,
		a5_2,
		a5_3,
		a5_4,
		new_a5_0,
		a10_1,
		a10_2,
		a10_3,
		a10_4,
		a10_5,
		a10_6,
		a10_7,
		a10_8,
		a10_9,
		new_a10_0)
	 :
	    a10_9
	 ;
}

int cc(int value)
{
    return cc_iter(0,value/5+1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
}

main()
{
    if( cc(100) != 292 ) fail(__LINE__);
    if( cc(200) != 2435 ) fail(__LINE__);
    _PASS;
}
