#undef AFS_CM
#include "pch08.h"
char *p = TEST;

int main() {
    if( strcmp( p, "undef'd" ) != 0 ) _fail;
    _PASS;
}
