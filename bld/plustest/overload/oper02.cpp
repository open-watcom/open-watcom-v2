#include "dump.h"

static char str[] = "str";

struct MyString
{
    MyString() { }
    operator const char *() { BAD; return str; };
}; 

int operator==(const MyString&, const char *) { GOOD; return 1; };

struct foo{
    operator long();
    foo operator+(foo)  { GOOD;  return foo(); };
    foo operator>>(long){ BAD; return foo(); };
    foo operator>>(int) { GOOD; return foo(); };
};

int main()
{
    MyString m;
    foo a,b,c;

    b=(a+b)>>12; 
    CHECK_GOOD(15+17);

    if( m == "aaa"); // error -za
    CHECK_GOOD(15+17+11);
    return( errors != 0);
}
