#include "pch01.h"
#include "fail.h"

int main() {
    if( inc( 1, 2 ) != 3 ) fail(__LINE__);
    if( inc( __LINE__, 2 ) != __LINE__+2 ) fail(__LINE__);
    _PASS;
}
