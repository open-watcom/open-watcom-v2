#include "fail.h"
// Revisions:
//
// 93/11/01 -- J.W.Welch        -- removed test for throw of zero (standard
//                                 says only exact member-ptr conversions are
//                                 allowed)


#include <stdio.h>
#include <stdlib.h>

struct S {
    int a;
    int b;
    int c;
};

S x = { 6, 28, 1 };
int S::* vmp = &S::c;

int main()
{
    for( int i = 1; i < 3; ++i ) {
        try {
            switch( i ) {
            case 0:
                throw 0;
                break;
            case 1:
                throw &S::b;
                break;
            case 2:
                throw vmp;
                break;
            default:
		fail(__LINE__);
            }
        } catch( int S::*mp ) {
            switch( i ) {
            case 0:
                if( mp ) {
		    fail(__LINE__);
                }
                break;
            case 1:
                if( x.*mp != 28 ) {
		    fail(__LINE__);
                }
                break;
            case 2:
                if( x.*mp != 1 ) {
		    fail(__LINE__);
                }
                break;
            }
        } catch( ... ) {
	    fail(__LINE__);
        }
    }
    _PASS;
}
