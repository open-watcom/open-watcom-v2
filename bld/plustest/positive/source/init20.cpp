#include "fail.h"

wchar_t a1[] = L"test";
char a2[] = "test";
wchar_t a3[9] = { 'a' };
wchar_t a4[] = L"";
char a5[] = "";
char a6[] = "x";

int main()
{
    if( sizeof(a2) != 5 ) _fail;
    if( sizeof(a3) != 18 ) _fail;
#if __WATCOM_REVISION__ >= 8
    if( sizeof(a1) != 10 ) _fail;
    if( sizeof(a4) != 2 ) _fail;
#endif
    if( sizeof(a5) != 1 ) _fail;
    if( sizeof(a6) != 2 ) _fail;
    _PASS;
}
