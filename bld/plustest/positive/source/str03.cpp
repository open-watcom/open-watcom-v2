#include "fail.h"
#include <string.h>

char check[2][11] = {
    { '0', '9', '8', '7', '6', '5', '4', '3', '2', '1', '\0' },
    { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\0' },
};

int main()
{
    char *p = "1234567890";
    if( p == "1234567890" ) fail(__LINE__);

    char *q = "0987654321";
    if( q == "0987654321" ) fail(__LINE__);

    auto char buff[32];
#ifdef __386__
    buff[0] = buff[0];
#else
    p = strcpy( buff, p );
#endif

    for( int i = 0; i < 2; i++ ) {
	if( strcmp( p, "1234567890" ) == 0 ) {
	    p = strcpy( p, "0987654321" );
	} else if( strcmp( p, "0987654321" ) == 0 ) {
	    p = strcpy( p, "1234567890" );
	}
	if( strcmp( p, check[i] ) != 0 ) fail(__LINE__);
    }
    _PASS;
}
