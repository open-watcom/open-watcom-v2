#include "dump.h"

struct Foo {
        Foo( const char * ) {GOOD};

        operator char *() const {GOOD; return "a"; };
};

void MyFunction( const char * )
{
}

void main()
{
    int                 i  = 0;
    Foo                 f3("a");

    CHECK_GOOD(4);
    MyFunction( ( ( i == 0 ) ? f3 : "No" ) );
    // We complain that we could either unify to char * or to Foo
    // so we call it ambiguous
    CHECK_GOOD(4+6);
}
