#include "dump.h"

void g1( char ) BAD;
void g1( short ) GOOD;
void g1( float ) BAD;

void g2( char ) BAD;
void g2( short ) GOOD;
void g2( float ) BAD;

void f( auto void (*g1)( float* ), auto void (*g2)( float* ) ) BAD;
void f( auto void (*g1)( float* ), auto void (*g2)( short ) ) BAD;
void f( auto void (*g1)( float* ), auto void (*g2)( char* ) ) BAD;
void f( auto void (*g1)( short ), auto void (*g2)( float* ) ) BAD;
void f( auto void (*g1)( short ), auto void (*g2)( short ) ) GOOD;
void f( auto void (*g1)( short ), auto void (*g2)( char* ) ) BAD;
void f( auto void (*g1)( char* ), auto void (*g2)( float* ) ) BAD;
void f( auto void (*g1)( char* ), auto void (*g2)( short ) ) BAD;
void f( auto void (*g1)( char* ), auto void (*g2)( char* ) ) BAD;

void foo( void )
{
    f( &g1, &g2 );
}
int main( void ) {
    foo();
    CHECK_GOOD( 15 );
    return errors != 0;
}
