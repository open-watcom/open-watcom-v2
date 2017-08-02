#include <assert.h>
#include <stdlib.h>
#include "fail.h"

#ifndef __cplusplus

typedef char bool;

#define true   1
#define false  0

#endif

int side_effect;

bool test_flow_or( bool True, bool False ) {

    if( !( True  || False ) ) _fail;
    if( !( True  || false ) ) _fail;
    if( !( True  || true  ) ) _fail;
    if( !( True  || True  ) ) _fail;

    if( !( true  || False ) ) _fail;
    if( !( true  || false ) ) _fail;
    if( !( true  || true  ) ) _fail;
    if( !( true  || True  ) ) _fail;

    if(  ( False || False ) ) _fail;
    if(  ( False || false ) ) _fail;
    if( !( False || true  ) ) _fail;
    if( !( False || True  ) ) _fail;

    if(  ( false || False ) ) _fail;
    if(  ( false || false ) ) _fail;
    if( !( false || true  ) ) _fail;
    if( !( false || True  ) ) _fail;

    side_effect = 1;
    if( !( ( side_effect = 0 ) || true ) ) _fail;
    if( side_effect != 0 ) _fail;

    side_effect = 1;
    if( !( ( side_effect = 0 ) || True ) ) _fail;
    if( side_effect != 0 ) _fail;

    return( true );
}

bool test_flow_and( bool True, bool False ) {

    if(  ( True  && False ) ) _fail;
    if(  ( True  && false ) ) _fail;
    if( !( True  && true  ) ) _fail;
    if( !( True  && True  ) ) _fail;

    if(  ( true  && False ) ) _fail;
    if(  ( true  && false ) ) _fail;
    if( !( true  && true  ) ) _fail;
    if( !( true  && True  ) ) _fail;

    if(  ( False && False ) ) _fail;
    if(  ( False && false ) ) _fail;
    if(  ( False && true  ) ) _fail;
    if(  ( False && True  ) ) _fail;

    if(  ( false && False ) ) _fail;
    if(  ( false && false ) ) _fail;
    if(  ( false && true  ) ) _fail;
    if(  ( false && True  ) ) _fail;

    side_effect = 1;
    if( ( ( side_effect = 0 ) && false ) ) _fail;
    if( side_effect != 0 ) _fail;

    side_effect = 1;
    if( ( ( side_effect = 0 ) && False ) ) _fail;
    if( side_effect != 0 ) _fail;

    return( true );
}

int main() {

    test_flow_or( true, false );
    test_flow_and( true, false );
    _PASS;
}
