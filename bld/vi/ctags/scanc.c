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


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "ctags.h"

static int      acceptOnlyEndif;
static int      structStack[MAX_STRUCT_DEPTH];
static int      structStackDepth;

/*
 * eatWhiteSpace - eat white space in the file
 */
static int eatWhiteSpace( void )
{
    int ch;

    for( ;; ) {
        ch = GetChar();
        if( ch == EOF ) {
            break;
        }
        if( !isspace( ch ) ) {
            break;
        }
        if( ch == '\n' ) {
            NewFileLine();
        }
    }
    return( ch );

} /* eatWhiteSpace */

/*
 * eatCommment - skip over a comment
 */
static void eatComment( void )
{
    int         ch;
    int         nesting_level;
    bool        done = FALSE;

    nesting_level = 1;
    while( !done ) {
        ch = GetChar();
        switch( ch ) {
        case EOF:
            done = TRUE;
            break;
        case '*':
            ch = GetChar();
            if( ch == '/' ) {
                nesting_level--;
                if( nesting_level == 0 ) {
                    return;
                }
            } else {
                UnGetChar( ch );
            }
            break;
        case '/':
            ch = GetChar();
            if( ch == '*' ) {
                nesting_level++;
            } else {
                UnGetChar( ch );
            }
            break;
        case '\n':
            NewFileLine();
            break;
        }
    }

} /* eatComment */

/*
 * eatUntilChar - skip through file until a matching char is found
 *                this allows for lines ending in '\' (continuation)
 */
static void eatUntilChar( int match )
{
    char        escape;
    int         ch;

    escape = FALSE;

    for( ;; ) {
        ch = GetChar();
        if( ch == EOF ) {
            break;
        }
        if( escape ) {
            escape = FALSE;
            continue;
        }
        if( ch == '\n' ) {
            NewFileLine();
            if( match == '\n' ) {
                return;
            }
        } else if( ch == '\\' ) {
            escape = TRUE;
        } else if( ch == match ) {
            return;
        }
    }
} /* eatUntilChar */

/*
 * doPreProcessorDirective - handle a pre-processor directive
 */
static void doPreProcessorDirective( void )
{
    char        *buffptr;
    char        buff[MAX_STR];
    int         ch;

    /*
     * eat leading whitespace
     */
    for( ;; ) {
        ch = GetChar();
        if( ch == EOF ) {
            return;
        }
        if( !isspace( ch ) ) {
            UnGetChar( ch );
            break;
        }
    }

    /*
     * get token
     */
    buffptr = buff;
    for( ;; ) {
        ch = GetChar();
        if( ch == EOF ) {
            return;
        }
        if( isspace( ch ) ) {
            break;
        }
        *buffptr++ = ch;
    }
    *buffptr = 0;

    /*
     * swallow #else and #elif shit
     */
    if( !acceptOnlyEndif ) {
        if( !stricmp( buff, "else" ) || !stricmp( buff, "elif" ) ) {
            acceptOnlyEndif = 1;
        }
    } else {
        if( !stricmp( buff, "endif" ) ) {
            acceptOnlyEndif--;
        } else if( !strnicmp( buff, "if", 2 ) ) {
            acceptOnlyEndif++;
        }
    }

    /*
     * if it is a #define, and we want macros, then get macro
     */
    if( !stricmp( buff, "define" ) && (WantMacros || WantAllDefines) &&
        !acceptOnlyEndif ) {
        ch = eatWhiteSpace();
        buffptr = buff;
        for( ;; ) {
            *buffptr++ = ch;
            ch = GetChar();
            if( ch == EOF ) {
                return;
            }
            if( !IsTokenChar( ch ) ) {
                *buffptr = 0;
                break;
            }
        }
        if( WantAllDefines || ch == '(' ) {
            RecordCurrentLineData();
            AddTag( buff );
        }
    }

    /*
     * if the last thing we were on was a '\n', then go to
     * the next line.  if the previous line ended in a '\',
     * then eat following line(s)
     */
    if( ch == '\n' ) {
        NewFileLine();
        if( buffptr > buff ) {
            if( *(buffptr - 1) != '\\' ) {
                return;
            }
        }
    }
    eatUntilChar( '\n' );

} /* doPreProcessorDirective */

/*
 * eatUntilClosingBracket - eat all chars until final ')' is found
 */
static void eatUntilClosingBracket( void )
{
    char        escape;
    int         ch;
    int         brace_level = 1;

    escape = FALSE;

    for( ;; ) {
        ch = GetChar();
        if( ch == EOF ) {
            break;
        }
        if( escape ) {
            escape = FALSE;
            continue;
        }
        switch( ch ) {
        case '\n':
            NewFileLine();
            break;
        case '\\':
            escape = TRUE;
            break;
        case '(':
            brace_level++;
            break;
        case ')':
            brace_level--;
            if( brace_level == 0 ) {
                return;
            }
            break;
        }
    }

} /* eatUntilClosingBracket */

/*
 * eatStuffBeforeOpenBrace - eat allowable stuff before an opening brace
 *                           (#stuff, comments, '\')
 */
bool eatStuffBeforeOpenBrace( int ch )
{
    if( ch == '#' ) {
        doPreProcessorDirective();
        return( TRUE );
    }
    if( ch == '\\' ) {
        eatUntilChar( '\n' );
        return( TRUE );
    }
    if( ch == '/' ) {
        ch = GetChar();
        if( ch == '*' ) {
            eatComment();
            return( TRUE );
        } else if( ch == '/' ) {
            eatUntilChar( '\n' );
            return( TRUE );
        } else {
            UnGetChar( ch );
        }
    }
    return( FALSE );

} /* eatStuffBeforeOpenBrace */

typedef enum {
    TAG_FUNC,
    TAG_PROTO,
    TAG_NOTHING
} tag_type;

/*
 * doFunction - handle a function declaration or prototype
 */
static tag_type doFunction( int *brace_level )
{
    char        ch;

    eatUntilClosingBracket();
    for( ;; ) {
        ch = eatWhiteSpace();
        if( IsTokenChar( ch ) || ch == '{' ) {
            break;
        }
        if( WantProtos && ch == ';' ) {
            return( TAG_PROTO );
        }
        if( eatStuffBeforeOpenBrace( ch ) ) {
            continue;
        }
        return( TAG_NOTHING );
    }
    if( ch != '{' ) {
        eatUntilChar( '{' );
    }
    *brace_level += 1;
    return( TAG_FUNC );

} /* doFunction */

/*
 * doCUSE - handle structures, enums, unions and classes
 */
static bool doCUSE( int ch )
{
    char        buff[MAX_STR];
    char        *buffptr;

    /*
     * scan out white space
     */
    while( isspace( ch ) ) {
        ch = GetChar();
        if( ch == EOF ) {
            return( FALSE );
        }
    }
    /*
     * check for a '{'.  If we have one, this is a defn with no name.
     */
    if( ch == '{' ) {
        return( TRUE );
    }

    /*
     * get struct name
     */
    buffptr = buff;
    for( ;; ) {
        *buffptr++ = ch;
        ch = GetChar();
        if( ch == EOF ) {
            return( FALSE );
        }
        if( ch == '\n' ) {
            NewFileLine();
        }
        if( !IsTokenChar( ch ) ) {
            break;
        }
    }

    /*
     * We have a struct name.  Now, look for an opening '{'.
     * If we don't find one, then this is not a struct defn.
     */
    if( ch == '{' ) {
        buffptr--;
    } else {
        for( ;; ) {
            ch = GetChar();
            if( ch == EOF ) {
                break;
            }
            if( isspace( ch ) ) {
                continue;
            }
            if( ch != '{' ) {
                if( eatStuffBeforeOpenBrace( ch ) ) {
                    continue;
                }
                UnGetChar( ch );
                return( FALSE );
            } else {
                break;
            }
        }
    }
    *buffptr = 0;
    AddTag( buff );
    return( TRUE );

} /* doCUSE */

#define CHANGE_STATE( a ) goto a;
#define STATE( a ) a:

/*
 * ScanC - scan c files for tags
 */
void ScanC( void )
{
    char        *buffptr;
    char        buff[MAX_STR];
    int         brace_level;
    int         typedef_level;
    int         paren_level;
    bool        have_token;
    bool        have_typedef;
    bool        have_cuse;
    bool        have_struct;
    bool        have_enum;
    bool        doit;
    tag_type    type;
    int         ch;

    buffptr = buff;
    brace_level = 0;
    paren_level = 0;
    typedef_level = -1;
    have_token = FALSE;
    have_typedef = FALSE;
    have_enum = FALSE;
    acceptOnlyEndif = FALSE;
    structStackDepth = 0;

    for( ;; ) {
        ch = GetChar();
        if( ch == EOF ) {
            break;
        }
        switch( ch ) {
        case '{':
            if( !acceptOnlyEndif ) {
                brace_level++;
            }
            CHANGE_STATE( end_token );

        case '}':
            if( !acceptOnlyEndif ) {
                if( structStackDepth > 1 ) {
                    if( structStack[structStackDepth - 1] == brace_level ) {
                        structStackDepth--;
                    }
                }
                brace_level--;
                if( brace_level < 0 ) {
                    brace_level = 0;
                }
                have_enum = FALSE;
            }
            CHANGE_STATE( end_token );

        case '\n':
            NewFileLine();
            CHANGE_STATE( end_token );

        STATE( end_token );
            have_token = FALSE;
            if( buffptr > buff ) {
                have_token = TRUE;
                *buffptr = 0;
                buffptr = buff;
            }
            continue;

        case '"':
        case '\'':
            eatUntilChar( ch );
            break;

        case '/':
            ch = GetChar();
            if( ch == '*' ) {
                eatComment();
                continue;
            } else if( ch == '/' ) {
                eatUntilChar( '\n' );
                continue;
            }
            UnGetChar( ch );
            ch = '/';
            CHANGE_STATE( save_char );

        case '(':
            if( !acceptOnlyEndif ) {
                if( have_typedef && brace_level == typedef_level ) {
                    paren_level++;
                } else if( have_token ) {
                    doit = FALSE;
                    if( brace_level == 0 ) {
                        doit = TRUE;
                    } else if( structStackDepth > 1 ) {
                        if( structStack[structStackDepth - 1] == brace_level ) {
                            doit = TRUE;
                        }
                    }
                    if( doit ) {
#ifdef __ENABLE_FNAME_PROCESSING__
                        if( !strnicmp( buff, "__F_NAME", 8 ) ) {
                            *buffptr++ = '(';

                            do {
                                ch = GetChar();
                                if( ch == EOF ) {
                                    break;
                                }
                                *buffptr++ = ch;
                            } while( ch != ')' );

                            if( ch == EOF ) {
                                break;
                            }

                            eatWhiteSpace();
                        }
#endif //__ENABLE_FNAME_PROCESSING__

                        if( buffptr != buff ) {
                            *buffptr = 0;
                        }
                        RecordCurrentLineData();
                        type = doFunction( &brace_level );
                        if( type != TAG_NOTHING ) {
                            AddTag( buff );
                            break;
                        }
                    }
                }
                if( structStackDepth > 1 ) {
                    if( structStack[structStackDepth - 1] == brace_level ) {
                        structStackDepth--;
                    }
                }
            }
            CHANGE_STATE( save_char );

        case ')':
            if( !acceptOnlyEndif ) {
                if( have_typedef && brace_level == typedef_level ) {
                    paren_level--;
                    if( paren_level == 0 ) {
                        typedef_level = -1;
                        have_typedef = FALSE;
                        if( buffptr != buff ) {
                            *buffptr = 0;
                        }
                        RecordCurrentLineData();
                        AddTag( buff );
                        eatUntilChar( ';' );
                        break;
                    }
                }
            }
            CHANGE_STATE( save_char );

        case '#':
            if( buffptr == buff ) {
                doPreProcessorDirective();
                break;
            }
            CHANGE_STATE( save_char );

        case ',':
            if( !acceptOnlyEndif ) {
                if( (have_typedef && brace_level == typedef_level) ||
                    (WantEnums && have_token && have_enum) ) {
                    RecordCurrentLineData();
                    if( buffptr != buff ) {
                        *buffptr = 0;
                    }
                    AddTag( buff );
                    break;
                }
            }
            CHANGE_STATE( save_char );

        case ';':
            if( !acceptOnlyEndif && have_typedef && brace_level == typedef_level ) {
                have_typedef = FALSE;
                typedef_level = -1;
                RecordCurrentLineData();
                if( buffptr != buff ) {
                    *buffptr = 0;
                }
                AddTag( buff );
                break;
            }
            CHANGE_STATE( save_char );

        default:
        STATE( save_char );
            if( !IsTokenChar( ch ) ) {
                if( buffptr == buff ) {
                    break;
                }
                *buffptr = 0;
                if( !acceptOnlyEndif ) {
                    if( WantTypedefs && !have_typedef && !stricmp( buff, "typedef" ) ) {
                        have_typedef = TRUE;
                        typedef_level = brace_level;
                        paren_level = 0;
                        break;
                    }
                    if( WantEnums && have_token && have_enum ) {
                        RecordCurrentLineData();
                        *buffptr = 0;
                        AddTag( buff );
                    }
                    if( WantUSE || WantClasses ) {
                        if( !have_typedef ) {
                            have_struct = FALSE;
                            have_cuse = FALSE;
                            if( WantClasses && !stricmp( buff, "class" ) ) {
                                have_struct = TRUE;
                                have_cuse = TRUE;
                            } else if( WantUSE ) {
                                if( !stricmp( buff, "struct" ) ) {
                                    have_struct = TRUE;
                                    have_cuse = TRUE;
                                } else if( !stricmp( buff, "union" ) ){
                                    have_cuse = TRUE;
                                } else if( !stricmp( buff, "enum" ) ) {
                                    have_cuse = TRUE;
                                    have_enum = TRUE;
                                }
                            }
                            if( have_cuse ) {
                                RecordCurrentLineData();
                                if( doCUSE( ch ) ) {
                                    brace_level++;
                                    if( structStackDepth < MAX_STRUCT_DEPTH &&
                                        have_struct ) {
                                        structStack[structStackDepth] = brace_level;
                                        structStackDepth++;
                                    }
                                }
                                break;
                            }
                        } else {
                            if( !strcmp( buff, "enum" ) ) {
                                have_enum = TRUE;
                            }
                        }
                    }
                }
                buffptr = buff;
            } else if( buffptr != buff || (isalpha( ch ) || ch == '_') ) {
                *buffptr++ = ch;
                have_token = TRUE;
            }
            continue;
        }
        buffptr = buff;
        have_token = FALSE;
    }

} /* ScanC */
