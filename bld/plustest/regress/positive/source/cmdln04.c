#include "fail.h"

#define str(x) #x
#define xstr(x) str(x)

#ifndef CMDMAC
    // it must be defined
    char *c = "!";
#else
    // verify older compiler defines a null macro
    // and compensate for this in the next test
    #if __WATCOM_REVISION__ < 7
	char *c = xstr(CMDMAC);
	#undef CMDMAC
	#define CMDMAC 1
    #else
	char *c = "";
    #endif
#endif

#if CMDMAC
char *p = "ok";
#else
char *p = "fail!";
#endif

char *n1 = xstr(CMD1);
char *n2 = xstr(CMD2);

int main() {
    if( strcmp( c, "" ) != 0 ) fail(__LINE__);
    if( strcmp( p, "ok" ) != 0 ) fail(__LINE__);
    if( strcmp( n1, "" ) != 0 ) fail(__LINE__);
    if( strcmp( n2, "" ) != 0 ) fail(__LINE__);
    _PASS;
}
