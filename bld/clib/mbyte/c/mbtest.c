/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*
 *  MBTEST.C
 *  Non-exhaustive test of the C library multibyte character functions.
 *
 */

#include <mbstring.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#ifdef __SW_BW
    #include <wdefwin.h>
#endif


/* DUMMY_DBCS won't be needed now that Windows and DOS are fully supported */
//#if !defined(__NT__) && !defined(__OS2__) && !defined(__WINDOWS__)
//    #define DUMMY_DBCS
//#endif


#define VERIFY( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    _mbslwr(__FILE__) );                    \
                            NumErrors++;                                    \
                        }


void TestCompare( void );
void TestCompareM( void );
void TestCase( void );
void TestCaseM( void );
void TestSearch( void );
void TestSearchM( void );
void TestSubstring( void );
void TestSubstringM( void );
void TestCopy( void );
void TestCopyM( void );
void TestToWide( void );
void TestFromWide( void );
void TestSet( void );
void TestSetM( void );
void TestToken( void );
void TestAddendum( void );
void TestMisc( void );
void TestInvalidDbcs( void );


char ProgramName[128];                          /* executable filename */
int NumErrors = 0;                              /* number of errors */



/****
***** Program entry point.
****/

void main( int argc, char *argv[] )
{
    int                 exitcode;

    /*** Initialize ***/
    #ifdef __SW_BW
        FILE *          my_stdout;
        my_stdout = freopen( "tmp.log", "a", stdout );
        if( my_stdout == NULL ) {
            fprintf( stderr, "Unable to redirect stdout\n" );
            exit( -1 );
        }
    #endif
    strcpy( ProgramName, strlwr(argv[0]) );     /* store executable filename */
    if( _setmbcp( 932 ) != 0 ) {
        printf( "Cannot initialize code page.\n\n" );
        exit( EXIT_FAILURE );
    }

    /*** Test various functions ***/
    TestCompare();                              /* comparison stuff */
    TestCase();                                 /* upper/lower case stuff */
    TestSearch();                               /* string searching */
    TestSubstring();                            /* substring stuff */
    TestCopy();                                 /* copying stuff */
    TestToWide();                               /* conversion to wide chars */
    TestFromWide();                             /* conversion from wide chars */
    TestSet();                                  /* test _mbsset() family */
    TestToken();                                /* test _mbstok() */
    TestAddendum();                             /* Normative Addendum stuff */
    TestMisc();                                 /* assorted string stuff */

    TestCompareM();
    TestCaseM();
    TestSearchM();
    TestSubstringM();
    TestCopyM();
    TestSetM();

    #ifndef DUMMY_DBCS
        TestInvalidDbcs();
    #endif

    /*** Print a pass/fail message and quit ***/
    if( NumErrors == 0 ) {
        printf( "%s: SUCCESS.\n", ProgramName );
        #ifdef __SW_BW
            fprintf( stderr, "%s: SUCCESS.\n", ProgramName );
        #endif
        exitcode = EXIT_SUCCESS;
    } else {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
        #ifdef __SW_BW
            fprintf( stderr, "%s: FAILURE (%d errors).\n",
                     ProgramName, NumErrors );
        #endif
        exitcode = EXIT_FAILURE;
    }

    #ifdef __SW_BW
        fclose( my_stdout );
        _dwShutDown();
    #endif
    exit( exitcode );
}



/****
***** Test _mbscmp(), _mbsicmp(), _mbsncmp(), _mbsnicmp(), _mbsnbcmp(),
***** and _mbsnbicmp().
****/

void TestCompare( void )
{
    unsigned char       bufA[] = "FoO bAr";
    int                 status;

    status = _mbscmp( bufA, "FoO bAr" );
    VERIFY( status == 0 );

    status = _mbscmp( bufA, "FoO bA" );
    VERIFY( status != 0 );


    status = _mbsicmp( bufA, "fOo BaR" );
    VERIFY( status == 0 );

    status = _mbsicmp( bufA, "fOo Ba" );
    VERIFY( status != 0 );


    status = _mbsncmp( bufA, "FoO bA", 6 );
    VERIFY( status == 0 );

    status = _mbsncmp( bufA, "", 0 );
    VERIFY( status == 0 );

    status = _mbsncmp( bufA, "FoO#$*(%%^*()", 3 );
    VERIFY( status == 0 );

    status = _mbsncmp( bufA, "FOo bAr", 7 );
    VERIFY( status != 0 );


    status = _mbsnicmp( bufA, "foo BA", 6 );
    VERIFY( status == 0 );

    status = _mbsnicmp( bufA, "", 0 );
    VERIFY( status == 0 );

    status = _mbsnicmp( bufA, "foO#$*(%%^*()", 3 );
    VERIFY( status == 0 );


    status = _mbsnbcmp( bufA, "FoO bA", 6 );
    VERIFY( status == 0 );

    status = _mbsnbcmp( bufA, "", 0 );
    VERIFY( status == 0 );

    status = _mbsnbcmp( bufA, "FoO#$*(%%^*()", 3 );
    VERIFY( status == 0 );

    status = _mbsnbcmp( bufA, "FOo bAr", 7 );
    VERIFY( status != 0 );


    status = _mbsnbicmp( bufA, "Foo BA", 6 );
    VERIFY( status == 0 );

    status = _mbsnbicmp( bufA, "", 0 );
    VERIFY( status == 0 );

    status = _mbsnbicmp( bufA, "foO#$*(%%^*()", 3 );
    VERIFY( status == 0 );
}


void TestCompareM( void )
{
    unsigned char       bufA[] = "FoO\x81\xFC"" bAr";
    int                 status;

    status = _mbscmp( bufA, "FoO\x81\xFC"" bAr" );
    VERIFY( status == 0 );

    status = _mbscmp( bufA, "FoO\x81\xFC"" bA" );
    VERIFY( status != 0 );


    status = _mbsicmp( bufA, "fOo\x81\xFC"" BaR" );
    VERIFY( status == 0 );

    status = _mbsicmp( bufA, "fOo\x81\xFC"" Ba" );
    VERIFY( status != 0 );


    status = _mbsncmp( bufA, "FoO\x81\xFC b!", 6 );
    VERIFY( status == 0 );

    status = _mbsncmp( bufA, "", 0 );
    VERIFY( status == 0 );

    status = _mbsncmp( bufA, "FoO\x81\xFC""#$*(%%^*()", 4 );
    VERIFY( status == 0 );

    status = _mbsncmp( bufA, "FOo\x81\xFC"" bAr", 8 );
    VERIFY( status != 0 );


    status = _mbsnicmp( bufA, "foo\x81\xFC"" BA", 7 );
    VERIFY( status == 0 );

    status = _mbsnicmp( bufA, "", 0 );
    VERIFY( status == 0 );

    status = _mbsnicmp( bufA, "foO\x81\xFC""#$*(%%^*()", 4 );
    VERIFY( status == 0 );


    status = _mbsnbcmp( bufA, "FoO\x81\xFC"" !!", 5 );
    VERIFY( status == 0 );

    status = _mbsnbcmp( bufA, "", 0 );
    VERIFY( status == 0 );

    status = _mbsnbcmp( bufA, "FoO\x81\xFC""#$*(%%^*()", 5 );
    VERIFY( status == 0 );

    status = _mbsnbcmp( bufA, "FOo\x81\xFC"" bAr", 30 );
    VERIFY( status != 0 );


    status = _mbsnbicmp( bufA, "Foo\x81\xFC""!BA", 5 );
    VERIFY( status == 0 );

    status = _mbsnbicmp( bufA, "", 0 );
    VERIFY( status == 0 );

    status = _mbsnbicmp( bufA, "foO#$*(%%^*()", 3 );
    VERIFY( status == 0 );
}



/****
***** Test _mbslwr() and _mbsupr().
****/

void TestCase( void )
{
    unsigned char       buf[] = "FOO BAR!";

    _mbslwr( buf );
    VERIFY( _mbscmp(buf,"foo bar!") == 0 );

    _mbsupr( buf );
    VERIFY( _mbscmp(buf,"FOO BAR!") == 0 );
}

void TestCaseM( void )
{
    unsigned char       buf[] = "FOO\x90\x90"" BAR!";

    _mbslwr( buf );
    VERIFY( _mbscmp(buf,"foo\x90\x90"" bar!") == 0 );

    _mbsupr( buf );
    VERIFY( _mbscmp(buf,"FOO\x90\x90"" BAR!") == 0 );
}


/****
***** Test _mbschr() and _mbsrchr().
****/

void TestSearch( void )
{
    unsigned char       buf[] = "foo!";
    unsigned char *     ptr;

    ptr = _mbschr( buf, _mbsnextc("!") );
    VERIFY( ptr == buf+3 );

    ptr = _mbschr( buf, _mbsnextc("#") );
    VERIFY( ptr == NULL );

    ptr = _mbschr( buf, (unsigned int) 0 );
    VERIFY( ptr == buf+4 );

    ptr = _mbsrchr( buf, _mbsnextc("f") );
    VERIFY( ptr == buf );

    ptr = _mbsrchr( buf, _mbsnextc("#") );
    VERIFY( ptr == NULL );

    ptr = _mbsrchr( buf, (unsigned int) 0 );
    VERIFY( ptr == buf+4 );
}


void TestSearchM( void )
{
    unsigned char       buf[] = "foo\x81\xFC""!";
    unsigned char *     ptr;

    ptr = _mbschr( buf, _mbsnextc("\x81\xFC") );
    VERIFY( ptr == buf+3 );

    ptr = _mbschr( buf, _mbsnextc("#") );
    VERIFY( ptr == NULL );

    ptr = _mbschr( buf, (unsigned int) 0 );
    VERIFY( ptr == buf+6 );

    ptr = _mbsrchr( buf, _mbsnextc("\x81\xFC") );
    VERIFY( ptr == buf+3 );

    ptr = _mbsrchr( buf, _mbsnextc("#") );
    VERIFY( ptr == NULL );

    ptr = _mbsrchr( buf, (unsigned int) 0 );
    VERIFY( ptr == buf+6 );
}



/****
***** Test _mbscspn(), _mbspbrk(), _mbsspn(), _mbsspnp(), and _mbsstr().
****/

void TestSubstring( void )
{
    unsigned char       buf[] = "Foo Bar";
    int                 len;
    unsigned char *     ptr;

    len = _mbscspn( buf, "$oF" );
    VERIFY( len == 0 );

    len = _mbscspn( buf, "raB" );
    VERIFY( len == 4 );

    len = _mbscspn( buf, "" );
    VERIFY( len == 7 );

    len = _mbscspn( buf, "!" );
    VERIFY( len == 7 );

    ptr = _mbspbrk( buf, "%23498-\tF34t" );
    VERIFY( ptr == buf );

    ptr = _mbspbrk( buf, "0789063478\rnzcb" );
    VERIFY( ptr == NULL );

    ptr = _mbspbrk( buf, "" );
    VERIFY( ptr == NULL );

    len = _mbsspn( buf, " rFaBo" );
    VERIFY( len == 7 );

    len = _mbsspn( buf, "025686089oah;F=~" );
    VERIFY( len == 3 );

    len = _mbsspn( buf, "37225252 r" );
    VERIFY( len == 0 );

    ptr = _mbsspnp( buf, " rFaBo" );
    VERIFY( ptr == NULL );

    ptr = _mbsspnp( buf, "025686089oah;F=~" );
    VERIFY( ptr == buf+3 );

    ptr = _mbsspnp( buf, "37225252 r" );
    VERIFY( ptr == buf );

    ptr = _mbsstr( buf, " Bar" );
    VERIFY( ptr == buf+3 );

    ptr = _mbsstr( buf, "Foo Ba!" );
    VERIFY( ptr == NULL );

    ptr = _mbsstr( buf, "" );
    VERIFY( ptr == buf );
}


void TestSubstringM( void )
{
    unsigned char       buf[] = "Foo\x81\xFC"" Bar";
    int                 len;
    unsigned char *     ptr;

    len = _mbscspn( buf, "\x81\xFC""oF" );
    VERIFY( len == 0 );

    len = _mbscspn( buf, "r\x81\xFC""aB" );
    VERIFY( len == 3 );

    len = _mbscspn( buf, "" );
    VERIFY( len == 9 );

    len = _mbscspn( buf, "\x81\x90\x81\xFC" );
    VERIFY( len == 3 );

    ptr = _mbspbrk( buf, "%234\x81\xFC""98-\t34t" );
    VERIFY( ptr == buf+3 );

    ptr = _mbspbrk( buf, "0789063478\rnzcb" );
    VERIFY( ptr == NULL );

    ptr = _mbspbrk( buf, "" );
    VERIFY( ptr == NULL );

    len = _mbsspn( buf, " Fr\x81\xFC""aBo" );
    VERIFY( len == 9 );

    len = _mbsspn( buf, "025686089oah;F=~" );
    VERIFY( len == 3 );

    len = _mbsspn( buf, "37\x80\x90""225252 r" );
    VERIFY( len == 0 );

    ptr = _mbsstr( buf, "\x81\xFC"" Bar" );
    VERIFY( ptr == buf+3 );

    ptr = _mbsstr( buf, "Foo\x81\xFC"" Ba!" );
    VERIFY( ptr == NULL );

    ptr = _mbsstr( buf, "" );
    VERIFY( ptr == buf );
}



/****
***** Test _mbsncpy(), _mbsncat(), _mbsnbcpy(), and _mbsnbcat().
****/

void TestCopy( void )
{
    unsigned char       bufA[80] = "";
    unsigned char       bufB[] = "Foo Bar";
    unsigned char *     ptr;

    _mbsncpy( bufA, "!!!!!!!!!!!!!!!", 20 );
    VERIFY( _mbscmp(bufA,"!!!!!!!!!!!!!!!") == 0 );

    ptr = _mbsncpy( bufA, bufB, 30 );
    VERIFY( ptr == bufA );
    VERIFY( _mbscmp(bufA,bufB) == 0 );

    ptr = _mbsncpy( bufA, "Bob", 30 );
    VERIFY( ptr == bufA );
    ptr = _mbsncat( bufA, "Fred!", 0 );
    VERIFY( ptr == bufA );
    VERIFY( _mbscmp(bufA,"Bob") == 0 );

    ptr = _mbsncat( bufA, "Fred!", 4 );
    VERIFY( ptr == bufA );
    VERIFY( _mbscmp(bufA,"BobFred") == 0 );

    ptr = _mbsnbcpy( bufA, "Bob", 30 );
    VERIFY( ptr == bufA );
    ptr = _mbsnbcat( bufA, "Fred!", 0 );
    VERIFY( ptr == bufA );
    VERIFY( _mbscmp(bufA,"Bob") == 0 );

    ptr = _mbsnbcat( bufA, "Fred!", 4 );
    VERIFY( ptr == bufA );
    VERIFY( _mbscmp(bufA,"BobFred") == 0 );

    strcat( bufA, "\x90" );     /* strcat, not _mbscat -- just one byte */
    ptr = _mbsnbcat( bufA, "Grok", 5 );
    VERIFY( ptr == bufA );
    #ifndef DUMMY_DBCS
        VERIFY( _mbscmp(bufA,"BobFredGrok") == 0 );
    #else
        VERIFY( _mbscmp(bufA,"BobFred""\x90""Grok") == 0 );
    #endif
}


void TestCopyM( void )
{
    unsigned char       bufA[80] = "";
    unsigned char       bufB[] = "Foo\x81\xFC"" Bar";
    unsigned char *     ptr;

    _mbsncpy( bufA, "!!!!!!!!\x90\x90""!!!!!!!", 20 );
    VERIFY( _mbscmp(bufA,"!!!!!!!!\x90\x90""!!!!!!!") == 0 );

    ptr = _mbsncpy( bufA, bufB, 10 );
    VERIFY( ptr == bufA );
    VERIFY( _mbscmp(bufA,bufB) == 0 );

    ptr = _mbsncpy( bufA, "Bob", 30 );
    VERIFY( ptr == bufA );
    ptr = _mbsncat( bufA, "Fred!\x81\xFC", 0 );
    VERIFY( ptr == bufA );
    VERIFY( _mbscmp(bufA,"Bob") == 0 );

    ptr = _mbsncat( bufA, "Fred\x81\xFC!", 5 );
    VERIFY( ptr == bufA );
    #ifndef DUMMY_DBCS
        VERIFY( _mbscmp(bufA,"BobFred\x81\xFC") == 0 );
    #else
        VERIFY( _mbscmp(bufA,"BobFred\x81") == 0 );
    #endif

    ptr = _mbsnbcpy( bufA, "Bob\x81\xFC", 6 );
    VERIFY( ptr == bufA );
    ptr = _mbsnbcat( bufA, "Fred!", 0 );
    VERIFY( ptr == bufA );
    VERIFY( _mbscmp(bufA,"Bob\x81\xFC") == 0 );

    ptr = _mbsnbcat( bufA, "Fred\x81\xFC!", 6 );
    VERIFY( ptr == bufA );
    VERIFY( _mbscmp(bufA,"Bob\x81\xFC""Fred\x81\xFC") == 0 );
}



/****
***** Test mbtowc() and mbstowcs().
****/

void TestToWide( void )
{
    wchar_t             wc = L'\0';
    wchar_t             wcs[20];
    int                 rc;

    rc = mbtowc( &wc, NULL, 1 );
    VERIFY( rc == 0 );

    rc = mbtowc( NULL, "!", 1 );
    VERIFY( rc == 1 );
    VERIFY( wc == L'\0' );

    rc = mbtowc( &wc, "", 1 );
    VERIFY( rc == 0 );

    rc = mbtowc( &wc, "", 0 );
    VERIFY( rc == -1 );

    rc = mbtowc( &wc, "foo", 0 );
    VERIFY( rc == -1 );

    rc = mbtowc( &wc, "foo", 1 );
    VERIFY( rc == 1 );
    VERIFY( wc == 'f' );

    rc = mbtowc( &wc, "\x81\xFC""foo", 2 );
    #ifndef DUMMY_DBCS
        VERIFY( rc == 2 );
    #else
        VERIFY( rc == 1 );
    #endif
    VERIFY( wc != 0 );

    rc = mbstowcs( wcs, "", 20 );
    VERIFY( rc == 0 );
    VERIFY( wcs[0] == L'\0' );

    rc = mbstowcs( wcs, "foo", 0 );
    VERIFY( rc == 0 );

    rc = mbstowcs( wcs, "\x81\xFC""foo", 0 );
    VERIFY( rc == 0 );

    rc = mbstowcs( wcs, "foo", 20 );
    VERIFY( rc == 3 );

    rc = mbstowcs( wcs, "\x81\xFC""foo", 20 );
    #ifndef DUMMY_DBCS
        VERIFY( rc == 4 );
        VERIFY( wcs[1] == 'f' );
        VERIFY( wcs[2] == 'o' );
        VERIFY( wcs[3] == 'o' );
        VERIFY( wcs[4] == '\0' );
    #else
        VERIFY( rc == 5 );
        VERIFY( wcs[2] == 'f' );
        VERIFY( wcs[3] == 'o' );
        VERIFY( wcs[4] == 'o' );
        VERIFY( wcs[5] == '\0' );
    #endif
    VERIFY( mbstowcs( NULL, "\x81\xFC""foo", 20 )  ==  rc );
}



/****
***** Test wctomb() and wcstombs().
****/

void TestFromWide( void )
{
    unsigned char       mbs[20];
    wchar_t             wc = L'\0';
    wchar_t             wcs[] = { 'H', 'e', 'l', 'l', 'o', '\0' };
    int                 len;

    len = wctomb( NULL, 0 );
    VERIFY( len == 0 );

    len = wctomb( mbs, L'\0' );
    VERIFY( len == 1 );
    VERIFY( mbs[0] == '\0' );

    len = wctomb( mbs, 'H' );
    VERIFY( len == 1 );
    VERIFY( mbs[0] == 'H' );

    len = wcstombs( mbs, wcs, 20 );
    VERIFY( len == 5 );
    VERIFY( !_mbscmp(mbs,"Hello") );
    VERIFY( wcstombs( NULL, wcs, 20 )  ==  len );

    len = wcstombs( mbs, &wc, 20 );
    VERIFY( len == 0 );
    VERIFY( mbs[0] == '\0' );
}



/****
***** Test _mbsset(), _mbsnset(), and _mbsnbset().
****/

void TestSet( void )
{
    unsigned char       buf[] = "abcdefghij";
    unsigned char *     ptr;
    unsigned int        mbc = '!';

    ptr = _mbsset( buf, mbc );
    VERIFY( ptr == buf );
    VERIFY( buf[0] == '!' );
    VERIFY( buf[9] == '!' );
    VERIFY( buf[10] == '\0' );

    _mbscpy( buf, "abcdefghij" );
    ptr = _mbsnset( buf, mbc, 5 );
    VERIFY( ptr == buf );
    VERIFY( buf[0] == '!' );
    VERIFY( buf[4] == '!' );
    VERIFY( buf[5] == 'f' );

    _mbscpy( buf, "abcdefghij" );
    ptr = _mbsnbset( buf, mbc, 5 );
    VERIFY( ptr == buf );
    VERIFY( buf[0] == '!' );
    VERIFY( buf[4] == '!' );
    VERIFY( buf[5] == 'f' );
}


void TestSetM( void )
{
    unsigned char       buf[] = "abcdefghij";
    unsigned char *     ptr;
    unsigned int        mbc;

    mbc = _mbsnextc( "\x81\xFC" );

    ptr = _mbsset( buf, mbc );
    VERIFY( ptr == buf );
    VERIFY( buf[0] == '\x81' );
    #ifndef DUMMY_DBCS
        VERIFY( buf[1] == '\xFC' );
    #else
        VERIFY( buf[1] == '\x81' );
    #endif

    _mbscpy( buf, "abcdefghij" );
    ptr = _mbsnset( buf, mbc, 4 );
    VERIFY( ptr == buf );
    VERIFY( buf[0] == '\x81' );
    #ifndef DUMMY_DBCS
        VERIFY( buf[1] == '\xFC' );
        VERIFY( buf[8] == 'i' );
    #else
        VERIFY( buf[1] == '\x81' );
        VERIFY( buf[4] == 'e' );
    #endif

    _mbscpy( buf, "abcdefghij" );
    ptr = _mbsnbset( buf, mbc, 5 );
    VERIFY( ptr == buf );
    VERIFY( buf[0] == '\x81' );
    #ifndef DUMMY_DBCS
        VERIFY( buf[4] == ' ' );
    #else
        VERIFY( buf[4] == '\x81' );
    #endif
    VERIFY( buf[5] == 'f' );
}



/****
***** Test _mbstok().
****/

void TestToken( void )
{
    unsigned char       buf[] = "F\x81\xFC""ind\x90\x90""all;the.tokens,";
    unsigned char *     ptr;

    ptr = _mbstok( buf, " ;.,!\x90\x90" );      /* find a token */
    VERIFY( !_mbscmp(ptr,"F\x81\xFC""ind") );

    ptr = _mbstok( NULL, " ;.,!" );             /* find a token */
    #ifndef DUMMY_DBCS
        VERIFY( !_mbscmp(ptr,"all") );
    #else
        VERIFY( !_mbscmp(ptr,"\x90""all") );
    #endif

    ptr = _mbstok( NULL, " ;.,!" );             /* find a token */
    VERIFY( !_mbscmp(ptr,"the") );

    ptr = _mbstok( NULL, " ;.,!" );             /* find a token */
    VERIFY( !_mbscmp(ptr,"tokens") );

    ptr = _mbstok( NULL, " '.,!" );             /* try to find another */
    VERIFY( ptr == NULL );
}



/****
***** Test mbtob(), sisinit(), mbrlen(), mbrtowc(), wcrtomb(), mbsrtowcs(),
***** wcsrtombs(), ie. all Normative Addendum functions.
****/

void TestAddendum( void )
{
    wchar_t             wc;
    wchar_t             wcs[10];
    wchar_t *           wcsPtr;
    int                 status;
    mbstate_t           state;
    unsigned char       mbc[MB_LEN_MAX];
    unsigned char       mbcs[10*MB_LEN_MAX+1];
    unsigned char *     mbcsPtr;

    status = wctob( L'!' );
    VERIFY( status == '!' );

    status = mbtowc( &wc, "\x90\x90", 2 );
    #ifndef DUMMY_DBCS
        VERIFY( status == 2 );
        status = wctob( wc );
        VERIFY( status == EOF );
    #else
        VERIFY( status == 1 );
        status = wctob( wc );
        VERIFY( status == 0x90 );
    #endif

    status = sisinit( NULL );
    VERIFY( status != 0 );

    state = 0;
    status = sisinit( &state );
    VERIFY( status == 0 );

    state = 123;
    status = sisinit( &state );
    VERIFY( status == 0 );

    status = mbrlen( "\0""!", 2, &state );
    VERIFY( status == 0 );

    status = mbrlen( "!", 2, &state );
    VERIFY( status == 1 );

    status = mbrlen( "\x90\x90""!", 1, &state );
    #ifndef DUMMY_DBCS
        VERIFY( status == -2 );
    #else
        VERIFY( status == 1 );
    #endif

    status = mbrtowc( NULL, NULL, 0, &state );
    VERIFY( status == 0 );

    status = mbrtowc( &wc, "!", 20, &state );
    VERIFY( status == 1 );
    VERIFY( wc == L'!' );

    status = wcrtomb( NULL, wc, &state );
    VERIFY( status == 0 );

    status = wcrtomb( mbc, wc, &state );
    VERIFY( status == 1 );
    VERIFY( *mbc == '!' );

    _mbscpy( mbcs, "Foo!\x90\x90" );
    mbcsPtr = mbcs;
    wcs[5] = wcs[6] = L'-';
    #ifndef DUMMY_DBCS
        status = mbsrtowcs( wcs, (const char**)(&mbcsPtr), 5, &state );
        VERIFY( status == 5 );
    #else
        status = mbsrtowcs( wcs, (const char**)(&mbcsPtr), 6, &state );
        VERIFY( status == 6 );
    #endif
    VERIFY( wcs[0] == L'F' );
    VERIFY( wcs[1] == L'o' );
    VERIFY( wcs[2] == L'o' );
    VERIFY( wcs[3] == L'!' );
    #ifndef DUMMY_DBCS
        VERIFY( wcs[4] & 0xFF00 );
        VERIFY( wcs[5] == L'-' );
        wcs[5] = L'\0';
    #else
        VERIFY( wcs[4] == 0x90 );
        VERIFY( wcs[5] == 0x90 );
        VERIFY( wcs[6] == L'-' );
        wcs[6] = L'\0';
    #endif

    mbcsPtr = mbcs;
    status = mbsrtowcs( NULL, (const char**)(&mbcsPtr), 1, &state );
    #ifdef DUMMY_DBCS
        VERIFY( status == 6 );  /* ignore len when dst==NULL */
    #else
        VERIFY( status == 5 );  /* ignore len when dst==NULL */
    #endif

    wcsPtr = wcs;
    _mbsset( mbcs, _mbsnextc("#") );
    status = wcsrtombs( mbcs, (const wchar_t**)(&wcsPtr), 6, &state );
    VERIFY( status == 6 );
    mbcs[status] = '\0';
    VERIFY( _mbscmp( mbcs, "Foo!\x90\x90" ) == 0 );

    wcsPtr = wcs;
    status = wcsrtombs( NULL, (const wchar_t**)(&wcsPtr), 20, &state );
    VERIFY( status == 6 );
}



/****
***** Test _mbsrev(), _mbslen(), _mbsninc(), _mbsnextc(), _mbsnbcnt(),
***** _mbsdec(), and _mbsnccnt().
****/

void TestMisc( void )
{
    unsigned char       buf[] = "abcde\x81\xFC""fghij";
    unsigned char       foo[] = "foo""\x82\x81\x81\xFC\x90\x90\x81\x81\x81\x81""bar";
    unsigned char *     ptr;
    int                 len;
    unsigned int        ch;
    char                mbc[MB_LEN_MAX+1];

    VERIFY( _mbsdec( foo, foo ) == NULL );
    VERIFY( _mbsdec( foo, foo-1 ) == NULL );
    VERIFY( _mbsdec( foo, foo+2 ) == foo+1 );
    VERIFY( _mbsdec( foo, foo+3 ) == foo+2 );
    VERIFY( _mbsdec( foo, foo+5 ) == foo+3 );
    VERIFY( _mbsdec( foo, foo+7 ) == foo+5 );
    VERIFY( _mbsdec( foo, foo+9 ) == foo+7 );
    VERIFY( _mbsdec( foo, foo+11 ) == foo+9 );
    VERIFY( _mbsdec( foo, foo+13 ) == foo+11 );

    len = _mbslen( buf );
    #ifndef DUMMY_DBCS
        VERIFY( len == 11 );
    #else
        VERIFY( len == 12 );
    #endif

    ptr = _mbsninc( buf, 4 );
    VERIFY( ptr == buf+4 );

    ptr = _mbsninc( buf, 6 );
    #ifndef DUMMY_DBCS
        VERIFY( ptr == buf+7 );
    #else
        VERIFY( ptr == buf+6 );
    #endif

    ch = _mbsnextc( ptr );
    _mbvtop( ch, mbc );
    mbc[_mbclen(mbc)] = '\0';
    VERIFY( _mbccmp(mbc,ptr) == 0 );

    len = _mbsnbcnt( buf, 30 );
    VERIFY( len == 12 );

    len = _mbsnbcnt( "a\x81\xFC""bc\x90\0", 30 );
    #ifndef DUMMY_DBCS
        VERIFY( len == 5 );
    #else
        VERIFY( len == 6 );
    #endif

    len = _mbsnbcnt( buf, 5 );
    VERIFY( len == 5 );

    len = _mbsnccnt( buf, 5 );
    VERIFY( len == 5 );

    len = _mbsnccnt( buf, 6 );
    #ifndef DUMMY_DBCS
        VERIFY( len == 5 );
    #else
        VERIFY( len == 6 );
    #endif

    len = _mbsnccnt( buf, 7 );
    #ifndef DUMMY_DBCS
        VERIFY( len == 6 );
    #else
        VERIFY( len == 7 );
    #endif

    ptr = _mbsninc( NULL, 10 );
    VERIFY( ptr == NULL );

    ptr = _mbsninc( buf, 6 );
    #ifndef DUMMY_DBCS
        VERIFY( ptr == buf+7 );
    #else
        VERIFY( ptr == buf+6 );
    #endif

    ptr = _mbsrev( buf );
    VERIFY( ptr == buf );
    #ifndef DUMMY_DBCS
        VERIFY( !_mbscmp(buf,"jihgf\x81\xFC""edcba") );
    #else
        VERIFY( !_mbscmp(buf,"jihgf\xFC\x81""edcba") );
    #endif
}



/****
***** Test a few lead_byte:null_byte situations.
****/

void TestInvalidDbcs( void )
{
    unsigned char       bufA[80] = "foo\x81";
    unsigned char       bufB[] = "abcde\x81\xFC""fghij";
    unsigned char *     ptr = "\x81";

    VERIFY( _mbslen( bufA ) == 3 );
    VERIFY( _mbclen( "\x81" ) == 2 );           /* for MS compatability */
    VERIFY( _mbsinc( ptr ) == ptr+1 );

    _mbsnbcat( bufA, bufB, 6 );
    VERIFY( _mbscmp( bufA, "fooabcde" ) == 0 );

    strset( bufA, '\0' );
    _mbsnbcpy( bufA, bufB, 6 );
    VERIFY( strcmp( bufA, "abcde" ) == 0 );
}
