#include "fail.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

#if __WATCOM_REVISION__>7
int foo( char *s, size_t slen, char *patt, size_t pattlen, int esc )
{
    static unsigned count;
    if( ++count > 2 ) fail(__LINE__);
    if( slen > INT_MAX ) fail(__LINE__);
    while( pattlen > 1 && *patt == '%' && patt[1] == '%' ) {
	patt++;
	pattlen--;
    }
    while( slen > 0 ) {
	if(( esc != (-1)) && (char)esc == *patt ) {
	    if (*s == patt[1]) {
		s++;
		slen--;
		patt += 2;
		pattlen -= 2;
	    } else {
		return(0);
	    }
	} else if (*patt == '%') {
	    unsigned int i;

	    patt++;
	    pattlen--;
	    if (pattlen == 0) {
		return(1);
	    }
	    if (!(esc != (-1) && (char)esc == *patt) && *patt != '%' && *patt != '_') {
		while (slen > 0 && *s != *patt) {
		    s++;
		    slen--;
		}

	    }
	    for (i = 0; i <= slen; i++) {
		if (foo(s + i, slen - i, patt, pattlen, esc)) {
		    return(1);
		}
	    }
	    return(0);
	} else if (*patt == '_' || *patt == *s) {
	    s++;
	    slen--;
	    patt++;
	    pattlen--;
	} else {
	    return(0);
	}
    }
    while (pattlen > 0) {
	if (*patt != '%') {
	    return(0);
	}
	patt++;
	pattlen--;
    }
    return(1);
}
                  

int main()
{
    int r;
    r = foo("aacc", 4, "a%b%c", 5, (-1));
    if( r != 0 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
