// pp01.c -- preprocessor test # 1
//
//
#include "fail.h"
#include <string.h>

// Support

#define compare(v1,v2) comp( v1, v2, __LINE__ )

void comp( int v1, int v2, unsigned line )
{
    if( v1 != v2 ) {
	fail(line);
    }
}

void comp( char v1, char v2, unsigned line )
{
    if( v1 != v2 ) {
	fail(line);
    }
}

void comp( float v1, float v2, unsigned line )
{
    if( v1 != v2 ) {
	fail(line);
    }
}

void comp( double v1, double v2, unsigned line )
{
    if( v1 != v2 ) {
	fail(line);
    }
}

void comp( char* v1, char* v2, unsigned line )
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

    compare( s01, 1 );
    compare( s02, 1. );
    compare( s03, 1.3 );
    compare( s04, .3 );
    compare( s05, IDENTIFIER );
    s06( k ) {
      case IDENTIFIER - 1 :
        compare( 0, 0 );
        break;
      default :
        compare( 0, 1 );
        break;
    }
    compare( s07, "LITERAL STRING" );
    compare( s08, "LITERAL STRING QUOTED \"\'\"" );
    compare( s09, "LITERAL STRING WITH // COMMENT" );
    compare( s10, "LITERAL STRING WITH /* COMMENT" );
    compare( s12, 'z' );
}

#define quoted(p) #p
#define sharp(p) quoted(p)

void test_sharp()
{
    compare( sharp( s01 ), "1" );
    compare( sharp( s02 ), "1." );
    compare( sharp( s03 ), "1.3" );
    compare( sharp( s04 ), ".3" );
    compare( sharp( s05 ), "5632" );
    compare( sharp( s06 ), "switch" );
    compare( sharp( s07 ), "\"LITERAL STRING\"" );
    compare( sharp( s08 ), "\"LITERAL STRING QUOTED \\\"\\\'\\\"\"" );
    compare( sharp( s09 ), "\"LITERAL STRING WITH // COMMENT\"" );
    compare( sharp( s10 ), "\"LITERAL STRING WITH /* COMMENT\"" );
    compare( sharp( s12 ), "'z'" );
}

#define paste1(P,Q) quoted( P ## Q )
#define paste(P,Q) paste1( P, Q )

void test_paste()
{
    compare( paste( s11, s01 ), "LIT1" );
    compare( paste( s11, s02 ), "LIT1." );
    compare( paste( s11, s03 ), "LIT1.3" );
    compare( paste( s11, s04 ), "LIT.3" );
    compare( paste( s11, s05 ), "LIT5632" );
    compare( paste( s11, s06 ), "LITswitch" );
    compare( paste( s01, s11 ), "1LIT" );
    compare( paste( s02, s11 ), "1.LIT" );
    compare( paste( s03, s11 ), "1.3LIT" );
    compare( paste( s04, s11 ), ".3LIT" );
    compare( paste( s05, s11 ), "5632LIT" );
    compare( paste( s06, s11 ), "switchLIT" );
}


main()
{
    test_basic_substitution();
    test_sharp();
    test_paste();

    _PASS;
}
