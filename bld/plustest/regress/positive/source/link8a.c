#include "fail.h"
#include <stdio.h>
#include "link8.h"


int main()
{
    try {
	C n;
    } catch ( int x ) {
	if( x != 5 ) fail(__LINE__);
    } catch( ...  ) {
	fail(__LINE__);
    }
    _PASS;
}
