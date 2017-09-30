#include "fail.h"

#ifndef __WATCOM_RTTI__
ALWAYS_PASS
#else
#include <ctype.h>
#include <typeinfo.h>

namespace x {
    enum E1 {
	A1,B1,C1
    } v1;
    enum E2 {
	A2,B2,C2
    } v2;
    struct X {
	int x;
    } v3;
    struct Y {
	int y;
    } v4;
};
using namespace x;

void check( type_info const &q1, type_info const &q2 ) {
    if( q1 == q2 ) _fail;
    if( q1.before( q2 ) && q2.before( q1 ) ) _fail;
    if( !q1.before( q2 ) && !q2.before( q1 ) ) _fail;
    const char *name = q1.name();
    if( name == NULL || strlen( name ) < strlen( "x::?" ) ) _fail;
    if( name[0] != 'x' || name[1] != ':' || name[2] != ':' ) _fail;
    if( ! isalpha( name[3] ) ) _fail;
    name = q2.name();
    if( name == NULL || strlen( name ) < strlen( "x::?" ) ) _fail;
    if( name[0] != 'x' || name[1] != ':' || name[2] != ':' ) _fail;
    if( ! isalpha( name[3] ) ) _fail;
}

int main() {
    if( typeid( v1 ) != typeid( v1 ) ) _fail;
    if( typeid( v2 ) != typeid( v2 ) ) _fail;
    if( typeid( v3 ) != typeid( v3 ) ) _fail;
    if( typeid( v4 ) != typeid( v4 ) ) _fail;
    if( typeid( v1 ) == typeid( v2 ) ) _fail;
    if( typeid( v1 ) == typeid( v3 ) ) _fail;
    if( typeid( v1 ) == typeid( v4 ) ) _fail;
    if( typeid( v2 ) == typeid( v1 ) ) _fail;
    if( typeid( v2 ) == typeid( v3 ) ) _fail;
    if( typeid( v2 ) == typeid( v4 ) ) _fail;
    if( typeid( v3 ) == typeid( v1 ) ) _fail;
    if( typeid( v3 ) == typeid( v2 ) ) _fail;
    if( typeid( v3 ) == typeid( v4 ) ) _fail;
    if( typeid( v4 ) == typeid( v1 ) ) _fail;
    if( typeid( v4 ) == typeid( v2 ) ) _fail;
    if( typeid( v4 ) == typeid( v3 ) ) _fail;
    check( typeid( v1 ), typeid( v2 ) );
    check( typeid( v1 ), typeid( v3 ) );
    check( typeid( v1 ), typeid( v4 ) );
    check( typeid( v2 ), typeid( v1 ) );
    check( typeid( v2 ), typeid( v3 ) );
    check( typeid( v2 ), typeid( v4 ) );
    check( typeid( v3 ), typeid( v2 ) );
    check( typeid( v3 ), typeid( v1 ) );
    check( typeid( v3 ), typeid( v4 ) );
    check( typeid( v4 ), typeid( v2 ) );
    check( typeid( v4 ), typeid( v3 ) );
    check( typeid( v4 ), typeid( v1 ) );
    check( typeid( v1 ), typeid( E2 ) );
    check( typeid( v1 ), typeid( X ) );
    check( typeid( v1 ), typeid( Y ) );
    check( typeid( v2 ), typeid( E1 ) );
    check( typeid( v2 ), typeid( X ) );
    check( typeid( v2 ), typeid( Y ) );
    check( typeid( v3 ), typeid( E2 ) );
    check( typeid( v3 ), typeid( E1 ) );
    check( typeid( v3 ), typeid( Y ) );
    check( typeid( v4 ), typeid( E2 ) );
    check( typeid( v4 ), typeid( X ) );
    check( typeid( v4 ), typeid( E1 ) );
    _PASS;
}
#endif
