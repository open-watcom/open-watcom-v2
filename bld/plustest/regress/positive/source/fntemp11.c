#include "fail.h"
#include <string.h>

struct Foo {
    Foo &operator <<(const char *string)
    {
	if( strcmp( string, "123456789" ) ) fail(__LINE__);
	return *this;
    }
};

struct Bar {
    char *temp;
};

template <class Type>
    Type &operator <<(Type &type,const Bar &bar)
    { type << bar.temp; return type; }

int main( void )
{
    Foo foo;
    Bar bar;
    bar.temp = "123456789";
    foo << bar;
    _PASS;
}
