#include "fail.h"

#include <stdlib.h>
#include <excpt.h>

#if defined( __386__ ) && (defined( __OS2__ ) || defined( __NT__ ))

int unwind[5];
int marker[3];

int test_fn1( int a )
{
    int rc = -2;

    _try {
        _try {
            rc = a;
            if( a == 2 ) _leave;    /* Verify _leave keyword */
            rc = 1 / a;
        }
        _except( EXCEPTION_EXECUTE_HANDLER ) {
            return( 0 );
        }
    }
    _finally {
        return( rc );
    }
    return( -1 );                   /* Make sure we never get here */
}

void func_level4( void )
{
    char    *nullp = NULL;

    marker[0] = 1;
    _try {
        *nullp = '\1';
    }
    _finally {
        if( AbnormalTermination() )
            unwind[4] = 1;
    }
    fail( __LINE__ );
}

void func_level3( void )
{
    _try {
        func_level4();
    }
    _finally {
        if( AbnormalTermination() )
            unwind[3] = 1;
    }
    fail( __LINE__ );
}

void func_level2( void )
{
    _try {
        _try {
            func_level3();

        }
        _except( EXCEPTION_CONTINUE_SEARCH ) {
            fail( __LINE__ );
        }
    }
    _finally {
        if( AbnormalTermination() )
            unwind[2] = 1;
    }
    fail( __LINE__ );
}

void func_level1( void )
{
    _try {
        func_level2();
    }
    _finally {
        if( AbnormalTermination() )
            unwind[1] = 1;
    }
    fail( __LINE__ );
}

void func_level0( void )
{
    _try {
        _try {
            func_level1();
        }
        _except( EXCEPTION_EXECUTE_HANDLER ) {
            marker[1] = 1;
        }
    }
    _finally {
        if( AbnormalTermination() )
            fail( __LINE__ );
    }
    marker[2] = 1;
}

int main( int argc, char **argv )
{
    _try {
        _try {
            func_level0();
        }
        _except( EXCEPTION_EXECUTE_HANDLER ) {
            fail( __LINE__ );
        }
    }
    _finally {
        if( AbnormalTermination() )
            fail( __LINE__ );
    }

    /* Verify that we visited all the places we were supposed to visit */
    if( marker[0] != 1 ) fail( __LINE__ );
    if( marker[1] != 1 ) fail( __LINE__ );
    if( marker[2] != 1 ) fail( __LINE__ );

    if( unwind[0] != 0 ) fail( __LINE__ );
    if( unwind[1] != 1 ) fail( __LINE__ );
    if( unwind[2] != 1 ) fail( __LINE__ );
    if( unwind[3] != 1 ) fail( __LINE__ );
    if( unwind[4] != 1 ) fail( __LINE__ );

    /* Test a slightly different SEH setup */
    if( test_fn1( 0 ) != 0 ) fail( __LINE__ );
    if( test_fn1( 1 ) != 1 ) fail( __LINE__ );
    if( test_fn1( 2 ) != 2 ) fail( __LINE__ );
    _PASS;
}

#else

int main( void )
{
    _PASS;
}

#endif
