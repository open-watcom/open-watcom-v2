#include "fail.h"
#include <stdio.h>
// #pragma on ( dump_ptree )
// #pragma on ( dump_emit_ic )

// test member pointers

#define VIRT


#ifdef VIRT

struct V
{   int a;
    int s;
    int memb( void );
    static int v_stat;
    static int v_stat_fun();
};

int V::v_stat = 13;


int V::v_stat_fun()
{
    return 19;
}


struct S : virtual V
{   int x;
};

int V::memb( void )
#else

struct S
{   int x;
    int a;
    int s;
    int memb( void );
};

int S::memb( void )
#endif
{
    static ctr = 0;

    ++ctr;
    return ctr;
}

static int foo_count = 0;

void foo()
{
    ++ foo_count;
}

int S::* sp1 = &S::s;
int S::* sp2 = sp1;
int S::* sp3;
int (S::*mp1)(void) = &S::memb;
int (S::*mp2)(void) = mp1;
int (S::*mp3)(void);

int S::* funimp( int S::*sp )
{
    sp3 = sp;
    return sp;
}


void funfmp( int (S::*mp)( void ) )
{
    mp3 = mp;
}



void error( const char* msg )
{
    printf( msg );
    fail(__LINE__);
}


int main( void )
{
    S sv1;
    int S::* sp4;
//  int (S::*mp4)(void);

    sp4 = funimp( sp1 );
    funfmp( mp1 );
    S* psv1 = &sv1;

    // de-reference using .*

    sv1.a = 1;
    sv1.s = 2;
    if( sv1.*(foo(),sp1) != 2 ) error( "sv1.*sp1 failed\n" );
    if( sv1.*(foo(),sp2) != 2 ) error( "sv1.*sp2 failed\n" );
    if( sv1.*(foo(),sp3) != 2 ) error( "sv1.*sp3 failed\n" );
    if( sv1.s != 2 ) error( "sv1.s failed\n" );
    if( sv1.*(foo(),(&S::s)) != 2 ) error( "sv1.*(&S::s) failed\n" );
    if( 1 != (sv1.*(foo(),mp1))() ) error( "sv1.*mp1 failed\n" );
    if( 2 != (sv1.*(foo(),mp2))() ) error( "sv1.*mp2 failed\n" );
    if( 3 != (sv1.*(foo(),mp3))() ) error( "sv1.*mp3 failed\n" );

    // de-reference using ->*

    if( psv1->*(foo(),foo(),sp1) != 2 ) error( "psv1->*sp1 failed\n" );
    if( psv1->*(foo(),foo(),sp2) != 2 ) error( "psv2->*sp2 failed\n" );
    if( psv1->*(foo(),foo(),sp3) != 2 ) error( "psv3->*sp3 failed\n" );
    if( 4 != (psv1->*(foo(),foo(),mp1))() ) error( "psv1->*mp1 failed\n" );
    if( 5 != (psv1->*((foo(),foo(),mp2)))() ) error( "psv1->*mp2 failed\n" );
    if( 6 != (psv1->*((foo(),foo(),mp3)))() ) error( "psv1->*mp3 failed\n" );
    if( 7 != (psv1->*(foo(),foo(),(&S::memb)))() ) error( "psv1->*(&S::memb) failed\n" );

    // comparisons

    if( sp1 != sp1 ) error( "sp1 != sp1 failed\n" );
    if( sp1 != sp2 ) error( "sp1 != sp2 failed\n" );
    if( sp1 != sp3 ) error( "sp1 != sp3 failed\n" );
    if( sp1 == 0   ) error( "sp1 == 0 failed\n" );
    if( 0 == sp1   ) error( "0 == sp1 failed\n" );
    if( sp1 != &S::s ) error( "sp1 != &S::s failed\n" );
    if( &S::s != sp1 ) error( "&S::s == sp1 failed\n" );
    if( mp1 != mp1 ) error( "mp1 != mp1 failed\n" );
    if( mp1 != mp2 ) error( "mp1 != mp2 failed\n" );
    if( mp1 != mp3 ) error( "mp1 != mp3 failed\n" );
    if( mp1 == 0   ) error( "mp1 == 0 failed\n" );
    if( 0 == mp1   ) error( "0 == mp1 failed\n" );
    if( mp1 != &S::memb ) error( "mp1 == &S::memb failed\n" );
    if( &S::memb != mp1 ) error( "&S::memb == mp1 failed\n" );

    // test address of statics

    int *i = &V::v_stat;
    if( *i != 13 ) {
        printf( "int *i = &V::v_stat failed\n" );
	fail(__LINE__);
    }
    auto int (*fun)() = &V::v_stat_fun;
    if( fun() != 19 ) {
        printf( "int (*fun)() = &V::v_stat_fun failed\n" );
	fail(__LINE__);
    }

    // final message

    if( foo_count != 21 ) {
        printf( "foo called %d times\n", foo_count );
	fail(__LINE__);
    }

    _PASS;
}

// #pragma on ( dump_exec_ic )
// #pragma on ( dump_scopes )
