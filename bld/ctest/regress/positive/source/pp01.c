// pp01.c -- preprocessor test # 1
//
//
#include "fail.h"
#include <string.h>

// Support

#define comparei(v1,v2) compi( v1, v2, __LINE__ )
#define comparec(v1,v2) compc( v1, v2, __LINE__ )
#define comparef(v1,v2) compf( v1, v2, __LINE__ )
#define compared(v1,v2) compd( v1, v2, __LINE__ )
#define compares(v1,v2) comps( v1, v2, __LINE__ )

void compi( int v1, int v2, unsigned line )
{
    if( v1 != v2 ) {
        fail(line);
    }
}

void compc( char v1, char v2, unsigned line )
{
    if( v1 != v2 ) {
        fail(line);
    }
}

void compf( float v1, float v2, unsigned line )
{
    if( v1 != v2 ) {
        fail(line);
    }
}

void compd( double v1, double v2, unsigned line )
{
    if( v1 != v2 ) {
        fail(line);
    }
}

void comps( char* v1, char* v2, unsigned line )
{
    if( strcmp( v1, v2 ) ) {
        fail(line);
    }
}

// Basic substitution

#define s00
#define s01 1
#define s02 1.
#define s03 1.3
#define s04 .3
#define s05 IDENTIFIER
#define s06 switch
#define s07 "LITERAL STRING"
#define s08 "LITERAL STRING QUOTED \"\'\""
#define s09 "LITERAL STRING WITH // COMMENT"
#define s10 "LITERAL STRING WITH /* COMMENT"
#define s11 LIT
#define s12 'z'

#ifndef s00
    #error *** FAILED ***
#endif

#define IDENTIFIER 5632

void test_basic_substitution()
{
    int k = IDENTIFIER - 1;

    comparei( s01, 1 );
    comparef( s02, 1. );
    comparef( s03, 1.3 );
    comparef( s04, .3 );
    comparei( s05, IDENTIFIER );
    s06( k ) {
      case IDENTIFIER - 1 :
        comparei( 0, 0 );
        break;
      default :
        comparei( 0, 1 );
        break;
    }
    compares( s07, "LITERAL STRING" );
    compares( s08, "LITERAL STRING QUOTED \"\'\"" );
    compares( s09, "LITERAL STRING WITH // COMMENT" );
    compares( s10, "LITERAL STRING WITH /* COMMENT" );
    comparec( s12, 'z' );
}

#define quoted(p) #p
#define sharp(p) quoted(p)

void test_sharp()
{
    compares( sharp( s01 ), "1" );
    compares( sharp( s02 ), "1." );
    compares( sharp( s03 ), "1.3" );
    compares( sharp( s04 ), ".3" );
    compares( sharp( s05 ), "5632" );
    compares( sharp( s06 ), "switch" );
    compares( sharp( s07 ), "\"LITERAL STRING\"" );
    compares( sharp( s08 ), "\"LITERAL STRING QUOTED \\\"\\\'\\\"\"" );
    compares( sharp( s09 ), "\"LITERAL STRING WITH // COMMENT\"" );
    compares( sharp( s10 ), "\"LITERAL STRING WITH /* COMMENT\"" );
    compares( sharp( s12 ), "'z'" );
}

#define paste1(P,Q) quoted( P ## Q )
#define paste(P,Q) paste1( P, Q )

void test_paste()
{
    compares( paste( s11, s01 ), "LIT1" );
    compares( paste( s11, s02 ), "LIT1." );
    compares( paste( s11, s03 ), "LIT1.3" );
    compares( paste( s11, s04 ), "LIT.3" );
    compares( paste( s11, s05 ), "LIT5632" );
    compares( paste( s11, s06 ), "LITswitch" );
    compares( paste( s01, s11 ), "1LIT" );
    compares( paste( s02, s11 ), "1.LIT" );
    compares( paste( s03, s11 ), "1.3LIT" );
    compares( paste( s04, s11 ), ".3LIT" );
    compares( paste( s05, s11 ), "5632LIT" );
    compares( paste( s06, s11 ), "switchLIT" );
}


main()
{
    test_basic_substitution();
    test_sharp();
    test_paste();

    _PASS;
}
