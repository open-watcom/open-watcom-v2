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
* Description:  Non-exhaustive test of ctype.h functions and macros.
*               Note: Tests assume the C locale.
*
****************************************************************************/


#include <ctype.h>
#include <wctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __SW_BW
    #include <wdefwin.h>
#endif


#define VERIFY( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    strlwr(__FILE__) );                     \
                            NumErrors++;                                    \
                        }

#define TEST_ARRAY_SIZE         256
#define TEST_ARRAY_SIZE_WIDE    512


struct CtypeBits {
    unsigned alnum  : 1;
    unsigned alpha  : 1;
    unsigned blank  : 1;
    unsigned cntrl  : 1;
    unsigned digit  : 1;
    unsigned graph  : 1;
    unsigned lower  : 1;
    unsigned print  : 1;
    unsigned punct  : 1;
    unsigned space  : 1;
    unsigned upper  : 1;
    unsigned xdigit : 1;
    unsigned ascii  : 1;
    unsigned csym   : 1;
    unsigned csymf  : 1;
};

struct CtypeBits MacroResults[TEST_ARRAY_SIZE];
struct CtypeBits FunctResults[TEST_ARRAY_SIZE];
struct CtypeBits WideMacroResults[TEST_ARRAY_SIZE_WIDE];
struct CtypeBits WideFunctResults[TEST_ARRAY_SIZE_WIDE];

char    ProgramName[_MAX_PATH];                 /* executable filename */
int     NumErrors = 0;                          /* number of errors */

int __far far_data = 0;

void TestClassifyMacro( void )
/****************************/
{
    int     i;

    far_data++; // set ds outside DGROUP

    MacroResults[0].alnum  = isalnum( EOF );
    MacroResults[0].alpha  = isalpha( EOF );
    MacroResults[0].blank  = isblank( EOF );
    MacroResults[0].cntrl  = iscntrl( EOF );
    MacroResults[0].digit  = isdigit( EOF );
    MacroResults[0].graph  = isgraph( EOF );
    MacroResults[0].lower  = islower( EOF );
    MacroResults[0].print  = isprint( EOF );
    MacroResults[0].punct  = ispunct( EOF );
    MacroResults[0].space  = isspace( EOF );
    MacroResults[0].upper  = isupper( EOF );
    MacroResults[0].xdigit = isxdigit( EOF );
    MacroResults[0].ascii  = isascii( EOF );
    MacroResults[0].csym   = __iscsym( EOF );
    MacroResults[0].csymf  = __iscsymf( EOF );

    for( i = 1; i < TEST_ARRAY_SIZE; i++ ) {
        MacroResults[i].alnum  = isalnum( i );
        MacroResults[i].alpha  = isalpha( i );
        MacroResults[i].blank  = isblank( i );
        MacroResults[i].cntrl  = iscntrl( i );
        MacroResults[i].digit  = isdigit( i );
        MacroResults[i].graph  = isgraph( i );
        MacroResults[i].lower  = islower( i );
        MacroResults[i].print  = isprint( i );
        MacroResults[i].punct  = ispunct( i );
        MacroResults[i].space  = isspace( i );
        MacroResults[i].upper  = isupper( i );
        MacroResults[i].xdigit = isxdigit( i );
        MacroResults[i].ascii  = isascii( i );
        MacroResults[i].csym   = __iscsym( i );
        MacroResults[i].csymf  = __iscsymf( i );
    }
}

void TestClassifyFunct( void )
/****************************/
{
    int     i;

    far_data++; // set ds outside DGROUP

    FunctResults[0].alnum  = (isalnum)( EOF );
    FunctResults[0].alpha  = (isalpha)( EOF );
    FunctResults[0].blank  = (isblank)( EOF );
    FunctResults[0].cntrl  = (iscntrl)( EOF );
    FunctResults[0].digit  = (isdigit)( EOF );
    FunctResults[0].graph  = (isgraph)( EOF );
    FunctResults[0].lower  = (islower)( EOF );
    FunctResults[0].print  = (isprint)( EOF );
    FunctResults[0].punct  = (ispunct)( EOF );
    FunctResults[0].space  = (isspace)( EOF );
    FunctResults[0].upper  = (isupper)( EOF );
    FunctResults[0].xdigit = (isxdigit)( EOF );
    FunctResults[0].ascii  = (isascii)( EOF );
    FunctResults[0].csym   = (__iscsym)( EOF );
    FunctResults[0].csymf  = (__iscsymf)( EOF );

    for( i = 1; i < TEST_ARRAY_SIZE; i++ ) {
        FunctResults[i].alnum  = (isalnum)( i );
        FunctResults[i].alpha  = (isalpha)( i );
        FunctResults[i].blank  = (isblank)( i );
        FunctResults[i].cntrl  = (iscntrl)( i );
        FunctResults[i].digit  = (isdigit)( i );
        FunctResults[i].graph  = (isgraph)( i );
        FunctResults[i].lower  = (islower)( i );
        FunctResults[i].print  = (isprint)( i );
        FunctResults[i].punct  = (ispunct)( i );
        FunctResults[i].space  = (isspace)( i );
        FunctResults[i].upper  = (isupper)( i );
        FunctResults[i].xdigit = (isxdigit)( i );
        FunctResults[i].ascii  = (isascii)( i );
        FunctResults[i].csym   = (__iscsym)( i );
        FunctResults[i].csymf  = (__iscsymf)( i );
    }
}

void TestClassifyWideMacro( void )
/********************************/
{
    int     i;

    far_data++; // set ds outside DGROUP

    WideMacroResults[0].alnum  = iswalnum( WEOF );
    WideMacroResults[0].alpha  = iswalpha( WEOF );
    WideMacroResults[0].blank  = iswblank( WEOF );
    WideMacroResults[0].cntrl  = iswcntrl( WEOF );
    WideMacroResults[0].digit  = iswdigit( WEOF );
    WideMacroResults[0].graph  = iswgraph( WEOF );
    WideMacroResults[0].lower  = iswlower( WEOF );
    WideMacroResults[0].print  = iswprint( WEOF );
    WideMacroResults[0].punct  = iswpunct( WEOF );
    WideMacroResults[0].space  = iswspace( WEOF );
    WideMacroResults[0].upper  = iswupper( WEOF );
    WideMacroResults[0].xdigit = iswxdigit( WEOF );
    WideMacroResults[0].ascii  = iswascii( WEOF );
    WideMacroResults[0].csym   = __iswcsym( WEOF );
    WideMacroResults[0].csymf  = __iswcsymf( WEOF );

    for( i = 1; i < TEST_ARRAY_SIZE_WIDE; i++ ) {
        WideMacroResults[i].alnum  = iswalnum( i );
        WideMacroResults[i].alpha  = iswalpha( i );
        WideMacroResults[i].blank  = iswblank( i );
        WideMacroResults[i].cntrl  = iswcntrl( i );
        WideMacroResults[i].digit  = iswdigit( i );
        WideMacroResults[i].graph  = iswgraph( i );
        WideMacroResults[i].lower  = iswlower( i );
        WideMacroResults[i].print  = iswprint( i );
        WideMacroResults[i].punct  = iswpunct( i );
        WideMacroResults[i].space  = iswspace( i );
        WideMacroResults[i].upper  = iswupper( i );
        WideMacroResults[i].xdigit = iswxdigit( i );
        WideMacroResults[i].ascii  = iswascii( i );
        WideMacroResults[i].csym   = __iswcsym( i );
        WideMacroResults[i].csymf  = __iswcsymf( i );
    }
}

void TestClassifyWideFunct( void )
/********************************/
{
    int i;

    far_data++; // set ds outside DGROUP

    WideFunctResults[0].alnum  = (iswalnum)( WEOF );
    WideFunctResults[0].alpha  = (iswalpha)( WEOF );
    WideFunctResults[0].blank  = (iswblank)( WEOF );
    WideFunctResults[0].cntrl  = (iswcntrl)( WEOF );
    WideFunctResults[0].digit  = (iswdigit)( WEOF );
    WideFunctResults[0].graph  = (iswgraph)( WEOF );
    WideFunctResults[0].lower  = (iswlower)( WEOF );
    WideFunctResults[0].print  = (iswprint)( WEOF );
    WideFunctResults[0].punct  = (iswpunct)( WEOF );
    WideFunctResults[0].space  = (iswspace)( WEOF );
    WideFunctResults[0].upper  = (iswupper)( WEOF );
    WideFunctResults[0].xdigit = (iswxdigit)( WEOF );
    WideFunctResults[0].ascii  = (iswascii)( WEOF );
    WideFunctResults[0].csym   = (__iswcsym)( WEOF );
    WideFunctResults[0].csymf  = (__iswcsymf)( WEOF );

    for( i = 1; i < TEST_ARRAY_SIZE_WIDE; i++ ) {
        WideFunctResults[i].alnum  = (iswalnum)( i );
        WideFunctResults[i].alpha  = (iswalpha)( i );
        WideFunctResults[i].blank  = (iswblank)( i );
        WideFunctResults[i].cntrl  = (iswcntrl)( i );
        WideFunctResults[i].digit  = (iswdigit)( i );
        WideFunctResults[i].graph  = (iswgraph)( i );
        WideFunctResults[i].lower  = (iswlower)( i );
        WideFunctResults[i].print  = (iswprint)( i );
        WideFunctResults[i].punct  = (iswpunct)( i );
        WideFunctResults[i].space  = (iswspace)( i );
        WideFunctResults[i].upper  = (iswupper)( i );
        WideFunctResults[i].xdigit = (iswxdigit)( i );
        WideFunctResults[i].ascii  = (iswascii)( i );
        WideFunctResults[i].csym   = (__iswcsym)( i );
        WideFunctResults[i].csymf  = (__iswcsymf)( i );
    }
}

/* Helper function to print mismatches in human readable form */
void CheckResults( struct CtypeBits *s1, struct CtypeBits *s2, int count )
/************************************************************************/
{
    int i;

    far_data++; // set ds outside DGROUP

    for( i = 0; i < TEST_ARRAY_SIZE; i++ ) {
        if( s1[i].alnum != WideMacroResults[i].alnum )
            printf( "Mismatch at %d (alnum)\n", i );
        if( s1[i].alpha != s2[i].alpha )
            printf( "Mismatch at %d (alpha)\n", i );
        if( s1[i].blank != s2[i].blank )
            printf( "Mismatch at %d (blank)\n", i );
        if( s1[i].cntrl != s2[i].cntrl )
            printf( "Mismatch at %d (cntrl)\n", i );
        if( s1[i].digit != s2[i].digit )
            printf( "Mismatch at %d (digit)\n", i );
        if( s1[i].graph != s2[i].graph )
            printf( "Mismatch at %d (graph)\n", i );
        if( s1[i].lower != s2[i].lower )
            printf( "Mismatch at %d (lower)\n", i );
        if( s1[i].print != s2[i].print )
            printf( "Mismatch at %d (print)\n", i );
        if( s1[i].punct != s2[i].punct )
            printf( "Mismatch at %d (punct)\n", i );
        if( s1[i].space != s2[i].space )
            printf( "Mismatch at %d (space)\n", i );
        if( s1[i].upper != s2[i].upper )
            printf( "Mismatch at %d (upper)\n", i );
        if( s1[i].xdigit != s2[i].xdigit )
            printf( "Mismatch at %d (xdigit)\n", i );
        if( s1[i].ascii != s2[i].ascii )
            printf( "Mismatch at %d (ascii)\n", i );
        if( s1[i].csym != s2[i].csym )
            printf( "Mismatch at %d (csym)\n", i );
        if( s1[i].csymf != s2[i].csymf )
            printf( "Mismatch at %d (csymf)\n", i );
    }
}

void TestResults( void )
/**********************/
{
    size_t  len;
    size_t  wide_len;

    far_data++; // set ds outside DGROUP

    len = sizeof( MacroResults );
    wide_len = sizeof( WideMacroResults );

    CheckResults( MacroResults, FunctResults, TEST_ARRAY_SIZE );
    VERIFY( !memcmp( MacroResults, FunctResults, len ) );
    VERIFY( !memcmp( WideMacroResults, WideFunctResults, wide_len ) );
    VERIFY( !memcmp( MacroResults, WideMacroResults, len ) );
    VERIFY( !memcmp( MacroResults, WideFunctResults, len ) );
}

void TestConversion( void )
/*************************/
{
    int c, c1, c2;

    far_data++; // set ds outside DGROUP

    for( c = 0; c < 256; c++ ) {
        c1 = tolower( c );
        c2 = toupper( c );
        if( isalpha( c ) ) {
            if( islower( c ) )
                VERIFY( (c1 == c) && (c2 != c) );
            if( isupper( c ) )
                VERIFY( (c1 != c) && (c2 == c) );
        } else {
            VERIFY( !isalpha( c1 ) && !isalpha( c2 ) );
        }
    }
}

void TestWideConversion( void )
/*****************************/
{
    wchar_t c, c1, c2;

    far_data++; // set ds outside DGROUP

    for( c = 0; c < 1024; c++ ) {
        c1 = towlower( c );
        c2 = towupper( c );
        if( iswalpha( c ) ) {
            if( iswlower( c ) )
                VERIFY( (c1 == c) && (c2 != c) );
            if( iswupper( c ) )
                VERIFY( (c1 != c) && (c2 == c) );
        } else {
            VERIFY( !iswalpha( c1 ) && !iswalpha( c2 ) );
        }
    }
}

int main( int argc, char *argv[] )
/********************************/
{
#ifdef __SW_BW
    FILE    *my_stdout;

    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        return( EXIT_FAILURE );
    }
#endif
    far_data++; // set ds outside DGROUP

    /*** Initialize ***/
    strcpy( ProgramName, strlwr( argv[0] ) );

    /*** Test stuff ***/
    TestClassifyMacro();
    TestClassifyFunct();
    TestClassifyWideMacro();
    TestClassifyWideFunct();
    TestResults();
    TestConversion();
    TestWideConversion();

    /*** Print a pass/fail message and quit ***/
    if( NumErrors != 0 ) {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
        return( EXIT_FAILURE );
    }
    printf( "Tests completed (%s).\n", ProgramName );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", ProgramName );
    fclose( my_stdout );
    _dwShutDown();
#endif

    return( EXIT_SUCCESS );
}
