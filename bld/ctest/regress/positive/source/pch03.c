#include "pch03.h"
#include "fail.h"

int main() {
    if( A != 1 ) _fail;
    if( B != 2 ) _fail;
    if( C != 3 ) _fail;
    _PASS;
}
