#undef AFS_CM
#include "pch06.h"
char *p = TEST;

int main() {
    if( strcmp( p, "undef'd" ) != 0 ) _fail;
    _PASS;
}
