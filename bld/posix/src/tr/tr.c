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
* Description:  POSIX tr utility
*               Translates input according to specification
*
****************************************************************************/


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "bool.h"
#include "getopt.h"
#include "misc.h"

char *OptEnvVar = "tr";

static const char *usageTxt[] = {
    "Usage: tr [-?bcds] string1 [string2]",
    "\tstring1     : characters to translate",
    "\tstring2     : chars from string1 are translated into the corresponding",
    "\t\t      char from string2",
    "\tOptions: -? : print this list",
    "\t\t -c : complement string1 w.r.t the universe of ascii chars",
    "\t\t -d : delete all input characters in string1",
    "\t\t -s : squeeze all repeated output characters in string2 into a",
    "\t\t      single char",
    "",
    "\t<string> ::= <char> | <range> | <repeat>",
    "\t<char>\t ::= literal | '\\\\' | '\\a' | '\\b' | '\\f' | '\\n' | '\\r' | '\\t' ",
    "\t\t     | '\\v' | '\\'<octal-digits> | '\\x'<hex-digits>",
    "\t<range>\t ::= '['<char>'-'<char>']'",
    "\t<repeat> ::= '['<char>'*'<number>']' | '['<char>'*]'",
    "\t<number> ::= <digits> | '0x'<hex-digits> | '0'<octal-digits>",
    NULL
};

#define MAX_STR 256

unsigned char   translationMatrix[MAX_STR];
bool            deleteSet[MAX_STR];           /* formed from string1 */
bool            squeezeSet[MAX_STR];          /* formed from string2 */
int             flagDelete;
int             flagSqueeze;
int             flagComplement;

/*
    in str:

    c           is treated as a literal character, unless it is one of
                \, -, [, *, or ].
    \           as in C... supports \\, \a, \b, \f, \n, \r, \t, \v,
                \###, and \x#.  (last are decimal/octal, and hex)
    [x-y]       The ascii sequence starting at x and ending at y.
    [a*n]       Expands to n occurences of a.  n is decoded like an
                integer constant in C (i.e. 0x means hex, 0 means octal ).
                If n is not present, then it defaults to 256.
*/

unsigned char expandChar( char **str ) {

    unsigned char   ch;

    ch = **(unsigned char **)str;
    if( ch == 0 )
        return( ch );
    ++*str;
    if( ch != '\\' )
        return( ch );
    ch = **(unsigned char **)str;
    if( ch == 0 )
        return( ch );
    ++*str;
    switch( ch ) {
    case 'a':   return( '\a' );
    case 'b':   return( '\b' );
    case 'f':   return( '\f' );
    case 'n':   return( '\n' );
    case 'r':   return( '\r' );
    case 't':   return( '\t' );
    case 'v':   return( '\v' );
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':   return( (unsigned char)strtol( *str, str, 8 ) );
    case 'x':   return( (unsigned char)strtol( *str + 1, str, 16 ) );
    default:    return( ch );
    }
}


size_t expandString( char *str, unsigned char *output ) {

    static const char class_msg[] = {
        "[] class must be of the form [x-y] or [a*n]\n"
    };
    static const char length_msg[] = {
        "a string cannot expand to greater than 256 characters\n"
    };
    unsigned char   low;
    unsigned        hi;
    unsigned char   *outp;
    int             numb;

    outp = output;
    for( ;; ) {
        if( *str == 0 ) break;
        if( *str == '[' ) {
            ++str;
            low = expandChar( &str );
            if( *str == 0 )
                Die( class_msg );
            switch( *str ) {
            case '-':
                ++str;
                hi = expandChar( &str );
                if( *str == 0 )
                    Die( class_msg );
                while( low <= hi ) {
                    if( outp - output >= MAX_STR )
                        Die( length_msg );
                    *outp++ = low++;
                }
                break;
            case '*':
                ++str;
                if( *str != ']' ) {
                    numb = strtoul( str, &str, 0 );
                    if( *str == 0 ) {
                        Die( class_msg );
                    }
                } else {
                    numb = (int)( MAX_STR - ( outp - output ) );
                }
                while( numb ) {
                    if( outp - output >= MAX_STR )
                        Die( length_msg );
                    *outp++ = low;
                    --numb;
                }
                break;
            default:
                Die( class_msg );
                break;
            }
            if( *str != ']' )
                Die( class_msg );
            ++str;
        } else {
            if( outp - output >= MAX_STR )
                Die( length_msg );
            *outp++ = expandChar( &str );
        }
    }
    return( outp - output );
}


void doTranslate( size_t len1, unsigned char *str1, size_t len2, unsigned char *str2 ) {

    int         i;
    int         j;
    int         ch;
    int         last_ch;

    for( i = 0; i < MAX_STR; ++i ) {
        translationMatrix[i] = (unsigned char)i;
    }
    if( flagComplement ) {
        for( i = 0; i < MAX_STR; ++i ) {
            deleteSet[i] = !deleteSet[i];
        }
        j = 0;
        for( i = 0; i < MAX_STR && j < len2; ++i ) {
            if( deleteSet[i] ) {
                translationMatrix[i] = str2[j];
                ++j;
            }
        }
    } else {
        j = 0;
        for( i = 0; i < len1 && i < len2; ++i ) {
            translationMatrix[str1[i]] = str2[i];
        }
    }
    last_ch = -1;
    for( ;; ) {
        ch = getchar();
        if( ch == EOF ) break;
        if( flagDelete && deleteSet[ch] ) continue;
        ch = translationMatrix[ch];
        if( flagSqueeze && last_ch == ch && squeezeSet[ch] ) continue;
        putchar( ch );
        last_ch = ch;
    }
}


void makeSet( size_t len, unsigned char *str, bool set[], bool no_dups ) {

    size_t      i;

    memset( set, FALSE, MAX_STR );
    for( i = 0; i < len; ++i ) {
        if( no_dups && set[str[i]] ) {
            Die( "no duplicates allowed in string1" );
        }
        set[str[i]] = TRUE;
    }
}


void main( int argc, char **argv ) {

    int             ch;
    unsigned char   string1[MAX_STR];
    size_t          string1_len;
    unsigned char   string2[MAX_STR];
    size_t          string2_len;

    for( ;; ) {
        ch = GetOpt( &argc, argv, "cds", usageTxt );
        if( ch == -1 ) break;
        switch( ch ) {
        case 'c':       flagComplement = 1;     break;
        case 'd':       flagDelete = 1;         break;
        case 's':       flagSqueeze = 1;        break;
        }
    }
    if( argc > 3 ) {
        Quit( usageTxt, "too many arguments" );
    }
    if( argc > 1 ) {
        string1_len = expandString( argv[1], string1 );
        makeSet( string1_len, string1, deleteSet, TRUE );
    } else {
        string1_len = 0;
    }
    if( argc > 2 ) {
        string2_len = expandString( argv[2], string2 );
        makeSet( string2_len, string2, squeezeSet, FALSE );
    } else {
        string2_len = 0;
    }
    /* We ALWAYS open the input and output streams in binary mode
     * to don't do CRLF translations.
     */
    setmode( fileno( stdin ), O_BINARY );
    setmode( fileno( stdout ), O_BINARY );
    doTranslate( string1_len, string1, string2_len, string2 );
    exit( 0 );
}
