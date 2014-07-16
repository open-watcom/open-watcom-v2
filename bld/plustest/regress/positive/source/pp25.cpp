#include "fail.h"
#include <string.h>

#if __WATCOM_REVISION__ >= 7

char *p = __FUNCTION__;

struct S {
    char *foo() {
	return __FUNCTION__;
    }
};
char *foo() {
    return __FUNCTION__;
}

#define __FUNCTION__ "!"
char *q = __FUNCTION__;

int main() {
    S x;
    if( strstr( x.foo(), "S::foo" ) == NULL ) fail(__LINE__);
    if( strstr( foo(), "foo" ) == NULL ) fail(__LINE__);
    if( strcmp( p, "?" ) != 0 ) fail(__LINE__);
    if( strcmp( q, "!" ) != 0 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
