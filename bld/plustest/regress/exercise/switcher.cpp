#include <stdio.h>

struct S
{
    char a[4];
    S();
    S( const S& );
    ~S();
};

S::S() {}
S::S( const S& ) {}
S::~S() { printf( "S DTOR'd\n" ); fflush( stdout ); }

S sv;

static int fun( S )
{
    return sizeof( S );
}

// Test a switch in which the default can be generated internally.
//
int foo1( int a )
{
    int b = 9;
    switch( a ) {
      case 1:
        b = fun( sv );      // temp for arg.
        break;
      case 2:
        b = 12;
        break;
    }
    printf( "foo1 return -- a=%d b=%d\n", a, b );
    return 1;
}


// Test a switch in which the default is present
//
int foo2( int a )
{
    int b = 9;
    switch( a ) {
      case 1:
        b = fun( sv );      // temp for arg.
        break;
      case 2:
        b = 12;
        break;
      default:
        break;
    }
    printf( "foo2 return -- a=%d b=%d\n", a, b );
    return 1;
}

struct block_exit
{
    ~block_exit();
};
block_exit::~block_exit() { printf( " DTOR naked" ); }

// This tests "miller" in combination with naked DTOR.
//
static void millerTime( unsigned param )
{
    printf( "Miller Time: %d", param );
    switch( param % 4 )
        for( ; ; ) {
      default :
            if( param == 0 ) break;
            printf( " 4: %d", param );
            param -= 4;
            continue;
      case 1 :
            printf( " 1" );
            param -= 1;
            continue;
      case 2 :
            block_exit marker;
            printf( " 2" );
            param -= 2;
            continue;
      case 3 :
            printf( " 3" );
            param -= 3;
            continue;
        }
}


int main( void )
{
    foo1( 1 );
    foo1( 2 );
    foo1( 3 );
    foo2( 1 );
    foo2( 2 );
    foo2( 3 );
    for( int i = 0; i < 6; ++ i ) {
        millerTime( i );
        printf( "\n" );
    }
    return 0;
}
