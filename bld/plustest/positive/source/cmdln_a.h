// model program for testing command line processing
//
// command line defines macro XXX

#ifndef XXX
    #error FAILURE: XXX macro not defined
#endif

#include "fail.h"

int main()
{
    if( XXX ) {
    } else {
	fail(__LINE__);
    }
    _PASS;
}
