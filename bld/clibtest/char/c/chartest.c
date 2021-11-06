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

#define TEST_ARRAY_SIZE         0x101
#define TEST_ARRAY_SIZE_WIDE    0x201


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
    int     c;

    far_data++; // set ds outside DGROUP

    MacroResults[0].alnum  = isalnum( EOF ) != 0;
    MacroResults[0].alpha  = isalpha( EOF ) != 0;
    MacroResults[0].blank  = isblank( EOF ) != 0;
    MacroResults[0].cntrl  = iscntrl( EOF ) != 0;
    MacroResults[0].digit  = isdigit( EOF ) != 0;
    MacroResults[0].graph  = isgraph( EOF ) != 0;
    MacroResults[0].lower  = islower( EOF ) != 0;
    MacroResults[0].print  = isprint( EOF ) != 0;
    MacroResults[0].punct  = ispunct( EOF ) != 0;
    MacroResults[0].space  = isspace( EOF ) != 0;
    MacroResults[0].upper  = isupper( EOF ) != 0;
    MacroResults[0].xdigit = isxdigit( EOF ) != 0;
    MacroResults[0].ascii  = isascii( EOF ) != 0;
    MacroResults[0].csym   = __iscsym( EOF ) != 0;
    MacroResults[0].csymf  = __iscsymf( EOF ) != 0;

    for( i = 1; i < TEST_ARRAY_SIZE; i++ ) {
        c = i - 1;
        MacroResults[i].alnum  = isalnum( c ) != 0;
        MacroResults[i].alpha  = isalpha( c ) != 0;
        MacroResults[i].blank  = isblank( c ) != 0;
        MacroResults[i].cntrl  = iscntrl( c ) != 0;
        MacroResults[i].digit  = isdigit( c ) != 0;
        MacroResults[i].graph  = isgraph( c ) != 0;
        MacroResults[i].lower  = islower( c ) != 0;
        MacroResults[i].print  = isprint( c ) != 0;
        MacroResults[i].punct  = ispunct( c ) != 0;
        MacroResults[i].space  = isspace( c ) != 0;
        MacroResults[i].upper  = isupper( c ) != 0;
        MacroResults[i].xdigit = isxdigit( c ) != 0;
        MacroResults[i].ascii  = isascii( c ) != 0;
        MacroResults[i].csym   = __iscsym( c ) != 0;
        MacroResults[i].csymf  = __iscsymf( c ) != 0;
    }
}

void TestClassifyFunct( void )
/****************************/
{
    int     i;
    int     c;

    far_data++; // set ds outside DGROUP

    FunctResults[0].alnum  = (isalnum)( EOF ) != 0;
    FunctResults[0].alpha  = (isalpha)( EOF ) != 0;
    FunctResults[0].blank  = (isblank)( EOF ) != 0;
    FunctResults[0].cntrl  = (iscntrl)( EOF ) != 0;
    FunctResults[0].digit  = (isdigit)( EOF ) != 0;
    FunctResults[0].graph  = (isgraph)( EOF ) != 0;
    FunctResults[0].lower  = (islower)( EOF ) != 0;
    FunctResults[0].print  = (isprint)( EOF ) != 0;
    FunctResults[0].punct  = (ispunct)( EOF ) != 0;
    FunctResults[0].space  = (isspace)( EOF ) != 0;
    FunctResults[0].upper  = (isupper)( EOF ) != 0;
    FunctResults[0].xdigit = (isxdigit)( EOF ) != 0;
    FunctResults[0].ascii  = (isascii)( EOF ) != 0;
    FunctResults[0].csym   = (__iscsym)( EOF ) != 0;
    FunctResults[0].csymf  = (__iscsymf)( EOF ) != 0;

    for( i = 1; i < TEST_ARRAY_SIZE; i++ ) {
        c = i - 1;
        FunctResults[i].alnum  = (isalnum)( c ) != 0;
        FunctResults[i].alpha  = (isalpha)( c ) != 0;
        FunctResults[i].blank  = (isblank)( c ) != 0;
        FunctResults[i].cntrl  = (iscntrl)( c ) != 0;
        FunctResults[i].digit  = (isdigit)( c ) != 0;
        FunctResults[i].graph  = (isgraph)( c ) != 0;
        FunctResults[i].lower  = (islower)( c ) != 0;
        FunctResults[i].print  = (isprint)( c ) != 0;
        FunctResults[i].punct  = (ispunct)( c ) != 0;
        FunctResults[i].space  = (isspace)( c ) != 0;
        FunctResults[i].upper  = (isupper)( c ) != 0;
        FunctResults[i].xdigit = (isxdigit)( c ) != 0;
        FunctResults[i].ascii  = (isascii)( c ) != 0;
        FunctResults[i].csym   = (__iscsym)( c ) != 0;
        FunctResults[i].csymf  = (__iscsymf)( c ) != 0;
    }
}

void TestClassifyWideMacro( void )
/********************************/
{
    int     i;
    int     c;

    far_data++; // set ds outside DGROUP

    WideMacroResults[0].alnum  = iswalnum( WEOF ) != 0;
    WideMacroResults[0].alpha  = iswalpha( WEOF ) != 0;
    WideMacroResults[0].blank  = iswblank( WEOF ) != 0;
    WideMacroResults[0].cntrl  = iswcntrl( WEOF ) != 0;
    WideMacroResults[0].digit  = iswdigit( WEOF ) != 0;
    WideMacroResults[0].graph  = iswgraph( WEOF ) != 0;
    WideMacroResults[0].lower  = iswlower( WEOF ) != 0;
    WideMacroResults[0].print  = iswprint( WEOF ) != 0;
    WideMacroResults[0].punct  = iswpunct( WEOF ) != 0;
    WideMacroResults[0].space  = iswspace( WEOF ) != 0;
    WideMacroResults[0].upper  = iswupper( WEOF ) != 0;
    WideMacroResults[0].xdigit = iswxdigit( WEOF ) != 0;
    WideMacroResults[0].ascii  = iswascii( WEOF ) != 0;
    WideMacroResults[0].csym   = __iswcsym( WEOF ) != 0;
    WideMacroResults[0].csymf  = __iswcsymf( WEOF ) != 0;

    for( i = 1; i < TEST_ARRAY_SIZE_WIDE; i++ ) {
        c = i - 1;
        WideMacroResults[i].alnum  = iswalnum( c ) != 0;
        WideMacroResults[i].alpha  = iswalpha( c ) != 0;
        WideMacroResults[i].blank  = iswblank( c ) != 0;
        WideMacroResults[i].cntrl  = iswcntrl( c ) != 0;
        WideMacroResults[i].digit  = iswdigit( c ) != 0;
        WideMacroResults[i].graph  = iswgraph( c ) != 0;
        WideMacroResults[i].lower  = iswlower( c ) != 0;
        WideMacroResults[i].print  = iswprint( c ) != 0;
        WideMacroResults[i].punct  = iswpunct( c ) != 0;
        WideMacroResults[i].space  = iswspace( c ) != 0;
        WideMacroResults[i].upper  = iswupper( c ) != 0;
        WideMacroResults[i].xdigit = iswxdigit( c ) != 0;
        WideMacroResults[i].ascii  = iswascii( c ) != 0;
        WideMacroResults[i].csym   = __iswcsym( c ) != 0;
        WideMacroResults[i].csymf  = __iswcsymf( c ) != 0;
    }
}

void TestClassifyWideFunct( void )
/********************************/
{
    int     i;
    int     c;

    far_data++; // set ds outside DGROUP

    WideFunctResults[0].alnum  = (iswalnum)( WEOF ) != 0;
    WideFunctResults[0].alpha  = (iswalpha)( WEOF ) != 0;
    WideFunctResults[0].blank  = (iswblank)( WEOF ) != 0;
    WideFunctResults[0].cntrl  = (iswcntrl)( WEOF ) != 0;
    WideFunctResults[0].digit  = (iswdigit)( WEOF ) != 0;
    WideFunctResults[0].graph  = (iswgraph)( WEOF ) != 0;
    WideFunctResults[0].lower  = (iswlower)( WEOF ) != 0;
    WideFunctResults[0].print  = (iswprint)( WEOF ) != 0;
    WideFunctResults[0].punct  = (iswpunct)( WEOF ) != 0;
    WideFunctResults[0].space  = (iswspace)( WEOF ) != 0;
    WideFunctResults[0].upper  = (iswupper)( WEOF ) != 0;
    WideFunctResults[0].xdigit = (iswxdigit)( WEOF ) != 0;
    WideFunctResults[0].ascii  = (iswascii)( WEOF ) != 0;
    WideFunctResults[0].csym   = (__iswcsym)( WEOF ) != 0;
    WideFunctResults[0].csymf  = (__iswcsymf)( WEOF ) != 0;

    for( i = 1; i < TEST_ARRAY_SIZE_WIDE; i++ ) {
        c = i - 1;
        WideFunctResults[i].alnum  = (iswalnum)( c ) != 0;
        WideFunctResults[i].alpha  = (iswalpha)( c ) != 0;
        WideFunctResults[i].blank  = (iswblank)( c ) != 0;
        WideFunctResults[i].cntrl  = (iswcntrl)( c ) != 0;
        WideFunctResults[i].digit  = (iswdigit)( c ) != 0;
        WideFunctResults[i].graph  = (iswgraph)( c ) != 0;
        WideFunctResults[i].lower  = (iswlower)( c ) != 0;
        WideFunctResults[i].print  = (iswprint)( c ) != 0;
        WideFunctResults[i].punct  = (iswpunct)( c ) != 0;
        WideFunctResults[i].space  = (iswspace)( c ) != 0;
        WideFunctResults[i].upper  = (iswupper)( c ) != 0;
        WideFunctResults[i].xdigit = (iswxdigit)( c ) != 0;
        WideFunctResults[i].ascii  = (iswascii)( c ) != 0;
        WideFunctResults[i].csym   = (__iswcsym)( c ) != 0;
        WideFunctResults[i].csymf  = (__iswcsymf)( c ) != 0;
    }
}

/* Helper function to print mismatches in human readable form */
void CheckResults( struct CtypeBits *s1, struct CtypeBits *s2, int count )
/************************************************************************/
{
    int i;

    /* unused parameters */ (void)count;

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

    for( c = 0; c < 0x100; c++ ) {
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

    for( c = 0; c < 0x400; c++ ) {
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

    /* unused parameters */ (void)argc;

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
    } else {
        printf( "Tests completed (%s).\n", ProgramName );
    }
#ifdef __SW_BW
    if( NumErrors != 0 ) {
        fprintf( stderr, "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
    } else {
        fprintf( stderr, "Tests completed (%s).\n", ProgramName );
    }
    fclose( my_stdout );
    _dwShutDown();
#endif

    if( NumErrors != 0 )
        return( EXIT_FAILURE );
    return( EXIT_SUCCESS );
}
