#include "fail.h"

int main()
{
    char dud = 5;
    dud %= 256;
    if( dud != 5 ) fail(__LINE__);
    _PASS;
}
