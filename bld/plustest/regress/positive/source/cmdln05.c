#include "fail.h"

#if __WATCOM_REVISION__ < 8
#define XS1 "a-/ b"
#define XS2 "a-/ b"
#define XN1 2
#define XN2 2
#endif

char *a[] = {
    XS1,
    XS2,
};
int b[] = {
    XN1,
    XN2,
};

int main() {
    if( strcmp( a[0], "a-/ b" ) ) _fail;
    if( strcmp( a[1], "a-/ b" ) ) _fail;
    if( b[0] != 2 ) _fail;
    if( b[1] != 2 ) _fail;
    _PASS;
}
