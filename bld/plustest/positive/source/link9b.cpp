#include "fail.h"
#include "link9.h"

S<int> ww;

int main()
{
    if( C::count != 1 ) fail(__LINE__);
    _PASS;
}
