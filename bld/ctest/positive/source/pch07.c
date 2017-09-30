#include "pch06.h"
char *p = TEST;

int main() {
    if( strcmp( p, "def'd" ) != 0 ) _fail;
    _PASS;
}
