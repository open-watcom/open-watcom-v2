#include "fail.h"

wchar_t a1[] = L"test";
char a2[] = "test";
wchar_t a3[9] = { 'a' };
wchar_t a4[] = L"";
char a5[] = "";
char a6[] = "x";
wchar_t a7[] = L"t" "e" "st";
wchar_t a8[] = "t" L"e" "st";

int main()
{
    if( sizeof(a2) != 5 ) _fail;
    if( sizeof(a3) != 18 ) _fail;
    if( sizeof(a1) != 10 ) _fail;
    if( sizeof(a4) != 2 ) _fail;
    if( sizeof(a5) != 1 ) _fail;
    if( sizeof(a6) != 2 ) _fail;
    if( sizeof(a7) != 10 ) _fail;
    if( sizeof(a8) != 10 ) _fail;
    _PASS;
}
