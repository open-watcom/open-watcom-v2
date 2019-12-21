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
* Description:  Macro preprocessor for wmake (handles !ifdef and the like).
*
****************************************************************************/


#include "make.h"
#include "wio.h"
#include "mstream.h"
#include "mlex.h"
#include "macros.h"
#include "mexec.h"
#include "mmemory.h"
#include "mmisc.h"
#include "mparse.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"

#include "clibext.h"


/*
 * This module presents a stream of characters to mlex.c.  Stripped from the
 * stream are comments, and preprocessing directives.  Preprocessing directives
 * are handled invisibly to mlex.c.  Also handles line continuation.
 *
 */

#define MAX_PRE_TOK     8       // chars needed for maximum keyword
#define INCLUDE      "INCLUDE"  // include directory for include file search


typedef enum {                  // must be kept in sync with directives
    D_BLANK = -1,               // a blank line
    #define pick(text,enum) enum,
    #include "mdirectv.h"
    #undef pick
    D_MAX
} directiveTok;

STATIC const char * const directives[] = {   // table must be lexically sorted.
    #define pick(text,enum) text,
    #include "mdirectv.h"
    #undef pick
};
#define NUM_DIRECT      D_MAX

#define MAX_DIR_LEN     8       // num chars incl null-terminator

STATIC char     atStartOfLine;  /* EOL at the start of a line...
 * This is a slight optimization for the critical code in PreGetCHR().
 */
STATIC STRM_T   lastChar;
STATIC bool     doingPreProc;   // are we doing some preprocessing?


/*
 * MS Compatability extension to add the if (expression) functionality
 */

STATIC void doElIf( bool (*logical)(void), directiveTok tok );

// local functions
STATIC void parseExpr ( DATAVALUE *leftVal, char *inString );
STATIC void logorExpr ( DATAVALUE *leftVal );
STATIC void logandExpr( DATAVALUE *leftVal );
STATIC void bitorExpr ( DATAVALUE *leftVal );
STATIC void bitxorExpr( DATAVALUE *leftVal );
STATIC void bitandExpr( DATAVALUE *leftVal );
STATIC void equalExpr ( DATAVALUE *leftVal );
STATIC void relateExpr( DATAVALUE *leftVal );
STATIC void shiftExpr ( DATAVALUE *leftVal );
STATIC void addExpr   ( DATAVALUE *leftVal );
STATIC void multExpr  ( DATAVALUE *leftVal );
STATIC void unaryExpr ( DATAVALUE *leftVal );

STATIC char         *currentPtr;    // Pointer to current start in string
STATIC MTOKEN_TYPE  currentToken;   // Contains information for current token


/*
 * struct nestIf is used to keep track of the if-endif constructs
 *
 * These are what the various fields mean:
 *
 * if skip then         we are skipping until an elif, else or endif
 * elif skip2endif then we are skipping everything until matching endif
 * else                 we are not skipping
 * endif
 *
 * if elseFound then    we have already come across the else that matches
 *                      the current if (for error checking)
 *
 * invariant( if( curNest.skip2endif ) then curNest.skip )
 */
struct nestIf {
    boolbit     skip2endif  : 1;
    boolbit     skip        : 1;
    boolbit     elseFound   : 1;
};

#define MAX_NEST    32                  // maximum depth of if nesting

STATIC struct nestIf    nest[MAX_NEST]; // stack for nesting
STATIC nest_level       nestLevel;      // items on stack
STATIC struct nestIf    curNest;        // current skip info


void PreProcInit( void )
/*****************************/
{
    StreamInit();

    atStartOfLine = '\n';
    doingPreProc = false;

    curNest.skip2endif = false;
    curNest.skip = false;
    curNest.elseFound = false;
    nestLevel = 0;
    lastChar = NULLCHAR;
}


void PreProcFini( void )
/*****************************/
{
    StreamFini();
}


STATIC STRM_T eatWhite( void )
/*****************************
 * pre:
 * post:    0 or more ws characters removed from input
 * returns: the first non-whitespace character is returned
 */
{
    STRM_T  s;

    s = PreGetCHR();
    while( sisws( s ) ) {
        s = PreGetCHR();
    }

    return( s );
}


STATIC void eatToEOL( void )
/*****************************
 * pre:
 * post:    atStartOfLine == EOL, 0 or more chars removed from input
 * returns: void
 */
{
    STRM_T  s;

    s = PreGetCHR();
    while( s != '\n' && s != STRM_END ) {
        s = PreGetCHR();
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC directiveTok getPreTok( void )
/************************************
 * pre:     the '!' has been eaten by caller
 * post:    first character following token is next char of input
 * returns: D_BLANK if no tokens on line, or token unrecognized,
 *          otherwise the D_ number of the token
 */
{
    STRM_T  s;
    char    tok[MAX_PRE_TOK];
    int     pos;
    char    **key;
    char    *tmp;               /* to pass tok buf to bsearch */

    s = eatWhite();

    if( s == '\n' ) {
        UnGetCHR( s );
        return( D_BLANK );
    }

    pos = 0;
    while( sisalpha( s ) && ( pos < MAX_PRE_TOK - 1 ) ) {
        tok[pos++] = s;
        s = PreGetCHR();
        // MS Compatability ELSE IFEQ can also be defined as ELSEIFEQ
        // similar for other types of if preprocessor directives
        if( pos == 4 ) {
            tok[pos] = NULLCHAR;
            if( stricmp( directives[D_ELSE], tok ) == 0 ) {
                break;
            }
        }
    }
    tok[pos] = NULLCHAR;

    UnGetCHR( s );
    UnGetCHR( eatWhite() );

    tmp = tok;
    key = bsearch( &tmp, directives, NUM_DIRECT, sizeof( char * ), KWCompare );

    if( key == NULL ) {
        if( !curNest.skip ) {
            PrtMsg( ERR | LOC | UNK_PREPROC_DIRECTIVE, tok );
        }
        return( D_BLANK );
    }

    assert( ( key - (char **)directives >= 0 ) &&
            ( key - (char **)directives <= D_MAX ) );

    return( (int)( key - (char **)directives ) );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC bool ifDef( void )
/************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: true if macro is defined, false otherwise
 */
{
    char    *name;
    char    *value;
    bool    ret;

    assert( !curNest.skip2endif );

    name = DeMacro( MAC_PUNC );
    eatToEOL();

    if( !IsMacroName( name ) ) {
        FreeSafe( name );
        return( false );
    }

    value = GetMacroValue( name );
    ret = ( value != NULL );
    if( value != NULL ) {
        FreeSafe( value );
    }
    FreeSafe( name );

    return( ret );
}


STATIC bool ifNDef( void )
/************************/
{
    return( !ifDef() );
}


STATIC void chopTrailWS( char *str )
/***********************************
 * chop trailing whitespace from str
 */
{
    char    *p;

    for( p = str + strlen( str ) - 1; p >= str && cisws( *p ); --p ) {
        *p = NULLCHAR;
    }
}


STATIC bool ifOp( void )
/***********************
 * MS Compatability  -
 * Allows for NMAKE compatability in binary and string operators
 * process the operands found in !if
 */
{
    char        *test;
    DATAVALUE   temp;

    assert( !curNest.skip2endif );

    test = DeMacro( TOK_EOL );
    eatToEOL();

    parseExpr( &temp, test );

    FreeSafe( test );
    return( temp.data.number != 0 );
}


STATIC void ifEqProcess( char const **v1, char **v2 )
/****************************************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: v1 & v2 set to freeable strings
 */
{
    char        *name;
    char        *test;
    char        *value;
    char        *beg;

    assert( !curNest.skip2endif );

    *v1 = NULL;
    *v2 = NULL;

    name = DeMacro( MAC_PUNC );
    test = DeMacro( TOK_EOL );
    eatToEOL();

    if( !IsMacroName( name ) ) {
        FreeSafe( name );
        FreeSafe( test );
        return;
    }

    value = WrnGetMacroValue( name );

    FreeSafe( name );               /* don't need name any more */

    if( value == NULL ) {
        FreeSafe( test );
        return;
    }

    UnGetCHR( '\n' );
    InsString( value, true );
    value = DeMacro( TOK_EOL );
    eatToEOL();

    beg = SkipWS( test );           /* find first non-ws */
    chopTrailWS( beg );             /* chop trailing ws */

    *v1 = value;
    *v2 = StrDupSafe( beg );
    FreeSafe( test );
}


STATIC bool ifEq( void )
/***********************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: true if macro equals text, false otherwise
 */
{
    bool        ret;
    char const  *v1;
    char        *v2;

    ifEqProcess( &v1, &v2 );
    if( v1 == NULL ) {
        return( 0 );
    }
    ret = ( strcmp( v1, v2 ) == 0 );

    FreeSafe( (void *)v1 );
    FreeSafe( v2 );

    return( ret );
}


STATIC bool ifEqi( void )
/************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: true if macro equals text (case independence), false otherwise
 */
{
    bool        ret;
    char const  *v1;
    char        *v2;

    ifEqProcess( &v1, &v2 );
    if( v1 == NULL ) {
        return( 0 );
    }
    ret = ( stricmp( v1, v2 ) == 0 );

    FreeSafe( (void *)v1 );
    FreeSafe( v2 );

    return( ret );
}


STATIC bool ifNEq( void )
/***********************/
{
    return( !ifEq() );
}


STATIC bool ifNEqi( void )
/************************/
{
    return( !ifEqi() );
}


STATIC void bangIf( bool (*logical)(void), directiveTok tok )
/************************************************************
 * pre:
 * post:    nestLevel > old(nestLevel); skip if false logical, or currently
 *          skipping; !elseFound
 * aborts:  if nestLevel >= MAX_NEST
 */
{
    if( nestLevel >= MAX_NEST ) {
        PrtMsg( FTL | LOC | IF_NESTED_TOO_DEEP );
        ExitFatal();
        // never return
    }

    nest[nestLevel++] = curNest; // save old nesting on the stack

        // remember that curNest still contains info from previous level
    curNest.skip2endif = (curNest.skip || curNest.skip2endif);
    curNest.skip = false;
    curNest.elseFound = false;

    if( !curNest.skip2endif ) { // ok to interpret if arguments?
        curNest.skip = !logical();
    } else {
        // this block is to be skipped, don't interpret args to if
        curNest.skip = true;
        eatToEOL();
    }

    if( curNest.skip ) {
        PrtMsg( DBG | INF | LOC | SKIPPING_BLOCK, directives[tok] );
    } else {
        PrtMsg( DBG | INF | LOC | ENTERING_BLOCK, directives[tok] );
    }
}


STATIC void bangEndIf( void )
/****************************
 * pre:
 * post:    atStartOfLine == EOL; curNest < old(curNest)
 * aborts:  if not nested
 */
{
    PrtMsg( DBG | INF | LOC | AT_ENDIF );

    if( nestLevel == 0 ) {
        PrtMsg( FTL | LOC | UNMATCHED_WITH_IF, directives[D_ENDIF] );
        ExitFatal();
        // never return
    }
    curNest = nest[--nestLevel];

    eatToEOL();
}


STATIC void doElse( void )
/*************************
 * pre:
 * post:    elseFound; skip if !old( skip ) || skip2endif
 * aborts:  if not nested
 * errors:  if elseFound
 */
{
    if( nestLevel == 0 ) {
        PrtMsg( FTL | LOC | UNMATCHED_WITH_IF, directives[D_ELSE] );
        ExitFatal();
        // never return
    }

    if( curNest.elseFound ) {
        PrtMsg( WRN | LOC | SKIPPING_AFTER_ELSE, directives[D_ELSE],
            directives[D_ELSE] );
        // must set these because we may not have been skipping previous block
        curNest.skip2endif = true;
        curNest.skip = true;
        return;
    }
    curNest.elseFound = true;

    if( !curNest.skip2endif ) {
        // check we're not skipping. if !skip then we should skip the else part.
        if( !curNest.skip ) {
            // skip to the end - we've done a block in this nesting
            curNest.skip = true;
            curNest.skip2endif = true;
        } else {
            // we still haven't done block in this nesting, do the else portion
            curNest.skip = false;
        }
    }

    if( curNest.skip ) {
        PrtMsg( DBG | INF | LOC | SKIPPING_BLOCK, directives[D_ELSE] );
    } else {
        PrtMsg( DBG | INF | LOC | ENTERING_BLOCK, directives[D_ELSE] );
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC void doElIf( bool (*logical)(void), directiveTok tok )
/************************************************************
 * post:    skip if !logical || skip2endif
 * aborts:  if not nested
 * errors:  if elseFound
 */
{
    char    buf[MAX_DIR_LEN * 2];

    FmtStr( buf, "%s %s", directives[D_ELSE], directives[tok] );

    if( nestLevel == 0 ) {
        PrtMsg( FTL | LOC | UNMATCHED_WITH_IF, buf );
        ExitFatal();
        // never return
    }

    if( curNest.elseFound ) {
        PrtMsg( WRN | LOC | SKIPPING_AFTER_ELSE, buf, directives[D_ELSE] );
        // must set these because we may not have been skipping previous block
        curNest.skip2endif = true;
        curNest.skip = true;
        eatToEOL();
        return;
    }

    if( !curNest.skip2endif ) {
        // check we're not skipping. if !skip, we should skip the else if part
        if( !curNest.skip ) {
            // skip to the end - we've done a block in this nesting
            curNest.skip = true;
            curNest.skip2endif = true;
            eatToEOL();
        } else {
            // we still haven't done block in this nesting, try this logical.
            curNest.skip = !logical();
        }
    } else {
        eatToEOL();
    }

    if( curNest.skip ) {
        PrtMsg( DBG | INF | LOC | SKIPPING_BLOCK, buf );
    } else {
        PrtMsg( DBG | INF | LOC | ENTERING_BLOCK, buf );
    }
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC void bangElse( void )
/***************************
 * pre:     next character of input is first after 'else' token
 * post:    atStartOfLine == EOL; skip may be modified
 * aborts:  if illegal directive
 */
{
    directiveTok   tok;

    tok = getPreTok();
    switch( tok ) {
    case D_BLANK:
        eatToEOL();
        doElse();
        break;
    case D_IFDEF:   doElIf( ifDef,  D_IFDEF );  break;
    case D_IFEQ:    doElIf( ifEq,   D_IFEQ );   break;
    case D_IFEQI:   doElIf( ifEqi,  D_IFEQI );  break;
    case D_IFNDEF:  doElIf( ifNDef, D_IFNDEF ); break;
    case D_IF:      doElIf( ifOp,   D_IF );     break;
    case D_IFNEQ:   doElIf( ifNEq,  D_IFNEQ );  break;
    case D_IFNEQI:  doElIf( ifNEqi, D_IFNEQI ); break;
    default:
        eatToEOL();
        PrtMsg( FTL | LOC | NOT_ALLOWED_AFTER_ELSE, directives[tok], directives[D_ELSE] );
        ExitFatal();
        // never return
    }
}


STATIC void bangDefine( void )
/*****************************
 * post:    atStartOfLine == EOL; macro may be defined
 * errors:  if !IsMacroName
 */
{
    char    *name;

    assert( !curNest.skip );

    name = DeMacro( MAC_PUNC );    /* decode name */

    if( !IsMacroName( name ) ) {
        eatToEOL();
    } else {
        DefMacro( name );
    }
    FreeSafe( name );
}


static char *skipUntilWS( const char *p )
/***************************************/
{
    while( *p != NULLCHAR && !cisws( *p ) ) {
        ++p;
    }
    return( (char *)p );
}


STATIC void bangInject( void )
/*****************************
 * !inject <text> <mac-name1> <mac-name2> ... <mac-nameN>
 * post:    atStartOfLine == EOL
 * errors:  none
 */
{
    char    *text;
    char    *contents;
    char    *end_contents;
    char    *curr;
    char    *mac_name;
    char    *value;

    assert( !curNest.skip );
    text = DeMacro( TOK_EOL );
    eatToEOL();
    contents = SkipWS( text );
    if( *contents == NULLCHAR ) {
        FreeSafe( text );
        return;
    }
    end_contents = skipUntilWS( contents );
    if( *end_contents == NULLCHAR ) {
        FreeSafe( text );
        return;
    }
    *end_contents++ = NULLCHAR;
    curr = end_contents;
    for( ;; ) {
        curr = SkipWS( curr );
        if( *curr == NULLCHAR )
            break;
        mac_name = curr;
        curr = skipUntilWS( curr );
        if( *curr != NULLCHAR ) {
            *curr++ = NULLCHAR;
        }
        if( !IsMacroName( mac_name ) ) {
            break;
        }
        UnGetCHR( '\n' );
        InsString( contents, false );
        value = GetMacroValue( mac_name );
        if( value != NULL ) {
            InsString( " ", false );
            InsString( value, true );
        }
        DefMacro( mac_name );
    }
    FreeSafe( text );
}


STATIC void bangLoadDLL( void )
/******************************
 * !loaddll <cmd-name> <dll-name> [<entry-pt>]
 * post:    atStartOfLine == EOL
 * errors:  none
 */
{
    char    *text;
    char    *cmd_name;
    char    *end_cmd_name;
    char    *dll_name;
    char    *end_dll_name;
    char    *ent_name;
    char    *end_ent_name;

    assert( !curNest.skip );
    text = DeMacro( TOK_EOL );
    eatToEOL();
    cmd_name = SkipWS( text );
    if( *cmd_name == NULLCHAR ) {
        FreeSafe( text );
        return;
    }
    end_cmd_name = skipUntilWS( cmd_name );
    if( *end_cmd_name == NULLCHAR ) {
        FreeSafe( text );
        return;
    }
    *end_cmd_name++ = NULLCHAR;
    dll_name = SkipWS( end_cmd_name );
    if( *dll_name == NULLCHAR ) {
        FreeSafe( text );
        return;
    }
    end_dll_name = skipUntilWS( dll_name );
    if( *end_dll_name == NULLCHAR ) {
        OSLoadDLL( cmd_name, dll_name, NULL );
        FreeSafe( text );
        return;
    }
    *end_dll_name++ = NULLCHAR;
    ent_name = SkipWS( end_dll_name );
    if( *ent_name == NULLCHAR ) {
        OSLoadDLL( cmd_name, dll_name, NULL );
        FreeSafe( text );
        return;
    }
    end_ent_name = skipUntilWS( ent_name );
    if( *end_ent_name == NULLCHAR ) {
        OSLoadDLL( cmd_name, dll_name, ent_name );
        FreeSafe( text );
        return;
    }
    *end_ent_name = NULLCHAR;
    OSLoadDLL( cmd_name, dll_name, ent_name );
    FreeSafe( text );
}


STATIC void bangUnDef( void )
/****************************
 * post:    atStartOfLine == EOL; macro may be undefined
 * errors:  if env_variable || !IsMacroName || not-defined
 */
{
    char    *name;
    char    *value;

    assert( !curNest.skip );

    name = DeMacro( MAC_PUNC );
    eatToEOL();

    if( !IsMacroName( name ) ) {
        FreeSafe( name );
        return;
    }

    value = GetMacroValue( name );
    if( value == NULL ) {
        PrtMsg( DBG | WRN | LOC | TRYING_UNDEF_UNDEF, directives[D_UNDEF] );
        FreeSafe( name );
        return;
    }

    UnDefMacro( name );
    FreeSafe( name );
    FreeSafe( value );
}


STATIC char *formatLongFileName( char *text )
/*******************************************/
{
    char    *ret;
    char    *pRet;
    char    *pTxt;

    assert( text != NULL );
    ret = StrDupSafe( text );
    pRet = ret;
    pTxt = text;

    if( pTxt[0] == '\"' ) {
        ++pTxt;
    }
    while( *pTxt != NULLCHAR && *pTxt != '\"' ) {
        if( *pTxt == '\\' ) {
            if( *(pTxt + 1) == '\"' ) {
                ++pTxt;
            }
        }
        *(pRet++) = *(pTxt++);
    }
    *pRet = NULLCHAR;
    if( *pTxt == '\"' ) {
        if( *(pTxt + 1) != NULLCHAR ) {
            PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
            FreeSafe( ret );
            return( NULL );
        }
    }
    return( ret );
}


STATIC void bangInclude( void )
/******************************
 * post:    atStartOfLine == EOL; maybe( new file in stream )
 * errors:  if unable to InsertFile()
 * for MS-Compatability we add <> angular bracket support to look
 *    for the file in the INCLUDE directory
 */
{
    char    *text;
    char    *temp = NULL;
    char    *p;
    char    full_path[_MAX_PATH];
    bool    ok;

    assert( !curNest.skip );

    text = DeMacro( TOK_EOL );
    eatToEOL();

    chopTrailWS( text );    /* get rid of trailing ws */

    if( *text == '<' ) {
        p = text;
        while( *p != '>' && *p != NULLCHAR ) {
            ++p;
        }
        if( *p == '>' ) {
            *p = NULLCHAR;
            temp = text;
            text = formatLongFileName( temp + 1 );
            if( text == NULL ) {
                FreeSafe( temp );
                return;
            }
            for( ;; ) {
                if( *p == NULLCHAR ) {
                    _searchenv( text, INCLUDE, full_path );
                    ok = ( *full_path != NULLCHAR );
                    if( ok ) {
                        ok = InsFile( full_path, false );
                    }
                    if( !ok ) {
                        PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
                    }
                    break;
                }
                // check if there are any trailing characters if there are
                // then error
                if( !cisws( *p ) ) {
                    PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
                    break;
                }
                ++p;
            }
        } else {
              PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
        }
    } else {
        temp = text;
        text = formatLongFileName( text );
        if( text == NULL ) {
            FreeSafe( temp );
            return;
        }
        if( !InsFile( text, false ) ) {
            PrtMsg( ERR | LOC | UNABLE_TO_INCLUDE, text );
        }
    }
    FreeSafe( temp );
    FreeSafe( text );
}


STATIC void bangMessage( void )
/******************************
 * post:    atStartOfLine == EOL
 */
{
    char    *text;

    assert( !curNest.skip );

    text = DeMacro( TOK_EOL );
    eatToEOL();

    chopTrailWS( text );

    PrtMsg( PRNTSTR, text );
    FreeSafe( text );
}


STATIC void bangError( void )
/****************************
 * post:    atStartOfLine == EOL
 */
{
    char    *text;

    assert( !curNest.skip );

    text = DeMacro( TOK_EOL );
    eatToEOL();

    chopTrailWS( text );

    PrtMsg( ERR | LOC | NEOL | USER_ERROR );
    PrtMsg( ERR | PRNTSTR, text );
    FreeSafe( text );
}


STATIC void handleBang( void )
/*****************************
 * post:    atStartOfLine == EOL
 */
{
    directiveTok   tok;

    tok = getPreTok();
    /* these are executed regardless of skip */
    switch( tok ) {
    case D_BLANK:   eatToEOL();                 break;
    case D_ELSE:    bangElse();                 break;
    case D_ENDIF:   bangEndIf();                break;
    case D_IF:      bangIf( ifOp,   D_IF );     break;
    case D_IFDEF:   bangIf( ifDef,  D_IFDEF );  break;
    case D_IFEQ:    bangIf( ifEq,   D_IFEQ );   break;
    case D_IFEQI:   bangIf( ifEqi,  D_IFEQI );  break;
    case D_IFNDEF:  bangIf( ifNDef, D_IFNDEF ); break;
    case D_IFNEQ:   bangIf( ifNEq,  D_IFNEQ );  break;
    case D_IFNEQI:  bangIf( ifNEqi, D_IFNEQI ); break;
    default:
        if( !curNest.skip ) {
            /* these are only executed if !skip */
            switch( tok ) {
            case D_DEFINE:  bangDefine();       break;
            case D_ERROR:   bangError();        break;
            case D_MESSAGE: bangMessage();      break;
            case D_INCLUDE: bangInclude();      break;
            case D_INJECT:  bangInject();       break;
            case D_LOADDLL: bangLoadDLL();      break;
            case D_UNDEF:   bangUnDef();        break;
            default:
                break;
            }
        } else {
            eatToEOL(); /* otherwise, we just eat it up */
        }
    }
}


static bool PreTestString( const char *str )
/*******************************************
 * Test if 'str' is the next sequence of characters in stream.
 * If not, push back any characters read.
 */
{
    const char  *p = str;
    STRM_T      s;
    bool        rc = false;

    for( ;; ) {
        s = GetCHR();
        if( s != *p ) {
            UnGetCHR( s );
            while( p-- > str ) {
                UnGetCHR( *p );
            }
            break;
        }
        ++p;
        if( *p == NULLCHAR ) {
            rc = true;
            break;
        }
    }
    return( rc );
}


STRM_T PreGetCHR( void )
/*****************************
 * returns: next character of input that is not a preprocessor directive
 * errors:  if an EOF occurs while nested
 */
{
    STRM_T  s;
    STRM_T  temp;
    bool    skip;

    s = GetCHR();
    if( !Glob.preproc ) {
        return( s );
    }

    for( ;; ) {
        if( !doingPreProc && (atStartOfLine == '\n' || s == STRM_TMP_EOL) ) {
            if( s == STRM_TMP_EOL ) {
                // Throw away the unwanted TMP character
                s = GetCHR();
                if( s != BANG_C ) {
                    UnGetCHR( s );
                    s = STRM_TMP_EOL;
                }
            }
            doingPreProc = true;

            if( Glob.compat_nmake || Glob.compat_posix ) {
                /* Check for NMAKE and UNIX compatible 'include' directive */
                if( s == 'i' && PreTestString( "nclude " ) ) {
                    UnGetCHR( eatWhite() );
                    bangInclude();
                    s = GetCHR();
                }
            }
            while( s == BANG_C ) {
                handleBang();

                assert( atStartOfLine == '\n' );

                s = GetCHR();
            }
            doingPreProc = false;
        }

        /* now we have a character of input */

        atStartOfLine = s;
        temp          = s;

        skip = curNest.skip && !doingPreProc;

        if( s == STRM_TMP_EOL ) {
            s = GetCHR();
        }
        if( s == COMMENT_C && lastChar != '$' && inlineLevel == 0 ) {
            s = GetCHR();
            while( s != '\n' && s != STRM_END ) {
                s = GetCHR();
            }
            if( temp == STRM_TMP_EOL ) {
                s = STRM_TMP_EOL;
            }
            /* we already have next char in t */
            continue;
        }

        if( s == STRM_END ) {
            curNest.skip = false;   /* so we don't skip a later file */
            curNest.skip2endif = false;

            atStartOfLine = '\n';   /* reset preprocessor */
            lastChar = STRM_END;
            return( s );
        }

        if( inlineLevel > 0 ) {     // We are currently defining an inline file
            lastChar = s;           // ignore all special characters ie {nl}
            if( skip ) {
                s = GetCHR();
                continue;
            }
            return( s );
        } else {
            if( Glob.compat_nmake && s == MS_LINECONT_C ) {
                s = GetCHR();
                if( s == '\n' ) {
                    lastChar = ' ';
                    if( skip ) {
                        s = STRM_TMP_EOL;
                        continue;
                    }
                    // place holder for temporary EOL
                    // this is to be able to implement the
                    // bang statements after line continues
                    UnGetCHR( STRM_TMP_EOL );
                    return( ' ' );
                } else {
                    lastChar = MS_LINECONT_C;
                    if( skip ) {
                        s = GetCHR();
                        continue;
                    }
                    UnGetCHR( s );
                    return( MS_LINECONT_C );
                }
            }

            if( s != LINECONT_C ) {
                if( s != UNIX_LINECONT_C || !Glob.compat_unix ) {
                    lastChar = s;
                    if( skip ) {
                        s = GetCHR();   /* must get next char */
                        continue;
                    }
                    return( s );
                }
                s = GetCHR();
                if( s != '\n' ) {
                    lastChar = UNIX_LINECONT_C;
                    if( skip ) {
                        continue;       /* already have next char */
                    }
                    UnGetCHR( s );
                    return( UNIX_LINECONT_C );
                } else {
                    if( skip ) {
                        continue;       /* already have next char */
                    }
                    UnGetCHR( STRM_TMP_EOL );
                }
            } else {
                s = GetCHR();           /* check if '&' followed by {nl} */
                if( s != '\n' || lastChar == '^' || lastChar == '[' || lastChar == ']' ) {
                                        /* nope... restore state */
                    lastChar = LINECONT_C;
                    if( skip ) {
                        continue;       /* already have next char */
                    }
                    UnGetCHR( s );
                    return( LINECONT_C );
                } else {
                    if( skip ) {
                        continue;       /* already have next char */
                    }
                    UnGetCHR( STRM_TMP_EOL );
                }
            }
        }
        s = GetCHR();
    }
}


STATIC void makeToken( MTOKEN_O type, MTOKEN_TYPE *current, size_t *index )
/*******************************************66****************************/
{
    switch( type ) {
    case OP_COMPLEMENT:
    case OP_LOG_NEGATION:
    case OP_ADD:
    case OP_SUBTRACT:
    case OP_MULTIPLY:
    case OP_DIVIDE:
    case OP_MODULUS:
    case OP_BIT_AND:
    case OP_BIT_OR:
    case OP_BIT_XOR:
    case OP_LESSTHAN:
    case OP_GREATERTHAN:
    case OP_PAREN_LEFT:
    case OP_PAREN_RIGHT:
    case OP_ERROR:
        (*index)++;
        break;
    case OP_LOG_AND:
    case OP_LOG_OR:
    case OP_SHIFT_LEFT:
    case OP_SHIFT_RIGHT:
    case OP_INEQU:
    case OP_EQUAL:
    case OP_LESSEQU:
    case OP_GREATEREQU:
        (*index) += 2;
        break;
    default:
        current->type = OP_ERROR;
        (*index)++;
    }

    current->type = type;
}


STATIC INT32 makeHexNumber( const char *inString, size_t *stringLength )
/**********************************************************************/
{
    INT32       value;
    char        c;
    const char  *pChar;

    value = 0;
    pChar = inString;
    for( ;; ) {
        c = pChar[0];
        if( c >= '0' && c <= '9' ) {
            c = c - '0';
        } else if( c >= 'a' && c <= 'f' ) {
            c = c - 'a' + 10;
        } else if( c >= 'A' && c <= 'F' ) {
            c = c - 'A' + 10;
        } else {
            break;
        }
        value = value * 16 + c;
        ++pChar;
    }
    *stringLength = pChar - inString;
    return( value );
}


STATIC void makeNumberToken( const char *inString, MTOKEN_TYPE *current, size_t *index )
/**************************************************************************************/
{
    INT32       value;
    const char  *pChar;
    char        c;
    size_t      hexLength;

    pChar = inString;
    value       = 0;
    *index      = 0;
    c = pChar[0];
    if( c == '0' ) {                            // octal or hex number
        ++pChar;
        c = pChar[0];
        if( c == 'x'  ||  c == 'X' ) {          // hex number
            ++pChar;
            value = makeHexNumber( pChar, &hexLength );
            pChar += hexLength;
        } else {                                // octal number
            while( c >= '0'  &&  c <= '7' ) {
                value = value * 8 + c - '0';
                ++pChar;
                c = pChar[0];
            }
        }
    } else {                                    // decimal number
        while( c >= '0'  &&  c <= '9' ) {
            value = value * 10 + c - '0';
            ++pChar;
            c = pChar[0];
        }
    }
    current->data.number = value;
    current->type = OP_INTEGER;

    *index = pChar - inString;
}


STATIC void makeStringToken( const char *inString, MTOKEN_TYPE *current, size_t *index )
/**************************************************************************************/
{
    size_t  inIndex;
    size_t  currentIndex;

    inIndex       = 1;   // skip initial double quote
    currentIndex  = 0;
    current->type = OP_STRING;
    for( ;; ) {
        if( inString[inIndex] == '\"' ) {
            // skip the second double quote
            ++inIndex;
            break;
        }
        if( currentIndex >= MAX_STRING ) {
            current->type = OP_ENDOFSTRING;
            break;
        }
        switch( inString[inIndex] ) {
        case NULLCHAR:
        case '\n':
        case COMMENT_C:
            /* error did not find closing quotation */
            current->type = OP_ERROR;
            break;
        default:
            current->data.string[currentIndex] = inString[inIndex];
        }

        if( current->type == OP_ERROR ) {
            break;
        }
        ++inIndex;
        ++currentIndex;
    }

    current->data.string[currentIndex] = NULLCHAR;
    *index = inIndex;
}

STATIC void makeAlphaToken( const char *inString, MTOKEN_TYPE *current, size_t *index )
/*************************************************************************************/
{
    char const  *r;
    char        *pwrite;
    char const  *pwritelast;

    r = inString;
    pwrite = current->data.string;
    pwritelast = pwrite + sizeof( current->data.string ) - 1;
    current->type = OP_STRING;

    // Note that in this case we are looking at a string that has no quotations
    // nmake gives expected error with exists(a(b) but also with exists("a(b")
    while( *r != ')' && *r != '(' && !cisws( *r ) ) {
        if( pwrite >= pwritelast ) {
            // VC++ 6 nmake allows 512 or more bytes here. We limit to 255.
            current->type = OP_ENDOFSTRING; // This truncates.
            break;
        }
        *pwrite++ = *r++;
    }

    *pwrite = NULLCHAR;
    *index = r - inString;
}



STATIC bool IsMacro( char const *name )
/*************************************/
{
    char    *value;

    // Seemingly redundant but GetMacroValue() needs plausible name
    if( !IsMacroName( name ) ) {
       return( false );
    }
    value = GetMacroValue( name );

    if( value == NULL ) {
        return( false );
    }
    FreeSafe( value );
    return( true );
}


STATIC bool name2function( MTOKEN_TYPE const *current, char const *criterion,
    bool (*action)(const char *), bool (**pquestion)(const char *) )
/*****************************************************************************/
{
    if( stricmp( current->data.string, criterion ) != 0 ) {
        return( false );
    }
    *pquestion = action;
    return( true );
}

STATIC void makeFuncToken( const char *inString, MTOKEN_TYPE *current, size_t *index )
/*************************************************************************************
 * parses only to get alphanumeric characters for special functions
 * ie. EXIST, defined.  if special characters are needed enclose in quotes
 */
{
    char    *probe;

    makeAlphaToken( inString, current, index );
    // check that the next token is a '(', swallow it, and check we have more.
    probe = SkipWS( inString + *index );
    if( *probe != '(' || (probe = SkipWS( probe + 1), *probe == NULLCHAR) ) {
        current->type = OP_ERROR;
    } else {
        bool (*is)(const char *);

        if( name2function( current, DEFINED, IsMacro,   &is )
          || name2function( current, EXIST,  ExistFile, &is )
          || name2function( current, EXISTS, ExistFile, &is ) ) {
            if( *probe == '\"' ) {      // Get macro or file name
                makeStringToken( probe, current, index );
            } else {
                makeAlphaToken( probe, current, index );
            }
            probe += *index;
            if( current->type == OP_STRING ) {
                probe = SkipWS( probe );
                if( *probe != ')' ) {
                    current->type = OP_ERROR;
                } else {
                    if( is == ExistFile ) {
                        FixName( current->data.string );
                    }
                    current->type          = OP_INTEGER;
                    current->data.number   = is( current->data.string );
                    ++probe;    // Swallow OP_PAREN_RIGHT
                }
            }
            *index = probe - inString;
        } else {
            current->type = OP_ERROR;
        }
    }
}

STATIC void makeCmdToken( const char *inString, MTOKEN_TYPE *current, size_t *index )
/************************************************************************************
 * get a command token enclosed in square brackets; very basic - a right
 * square bracket terminates command regardless of quoting
 */
{
    size_t  inIndex;
    size_t  currentIndex;

    inIndex       = 1;   // skip opening bracket
    currentIndex  = 0;
    current->type = OP_SHELLCMD;
    for( ;; ) {
        if( inString[inIndex] == ']' ) {
            // skip the closing bracket
            ++inIndex;
            break;
        }
        if( currentIndex >= MAX_STRING ) {
            current->type = OP_ENDOFSTRING;
            break;
        }
        switch( inString[inIndex] ) {
        case NULLCHAR :
        case '\n':
        case COMMENT_C:
            // error did not find closing quotation
            current->type = OP_ERROR;
            break;
        default:
            current->data.string[currentIndex] = inString[inIndex];
        }

        if( current->type == OP_ERROR ) {
            break;
        }
        ++inIndex;
        ++currentIndex;
    }

    current->data.string[currentIndex] = NULLCHAR;
    *index = inIndex;
}

STATIC size_t ScanToken( const char *inString, MTOKEN_TYPE *current )
/*******************************************************************/
{
    const char  *pString;
    size_t      index = 0;

    pString = SkipWS( inString );
    switch( pString[index] ) {
    case NULLCHAR:
    case '\n':
    case COMMENT_C:
        makeToken( OP_ENDOFSTRING, current, &index );
        break;
    case '~':
        makeToken( OP_COMPLEMENT, current, &index );
        break;
    case '+':
        makeToken( OP_ADD, current, &index );
        break;
    case '-':
        makeToken( OP_SUBTRACT, current, &index );
        break;
    case '*':
        makeToken( OP_MULTIPLY, current, &index );
        break;
    case '/':
        makeToken( OP_DIVIDE, current, &index );
        break;
    case '%':
        makeToken( OP_MODULUS, current, &index );
        break;
    case '^':
        makeToken( OP_BIT_XOR, current, &index );
        break;
    case '(':
        makeToken( OP_PAREN_LEFT, current, &index );
        break;
    case ')':
        makeToken( OP_PAREN_RIGHT, current, &index );
        break;
    case '!':
        switch( pString[index + 1] ) {
        case '=':
            makeToken( OP_INEQU, current, &index );
            break;
        default:
            makeToken( OP_LOG_NEGATION, current, &index );
            break;
        }
        break;
    case '&':
        switch( pString[index + 1] ) {
        case '&':
            makeToken( OP_LOG_AND, current, &index );
            break;
        default:
            makeToken( OP_BIT_AND, current, &index );
            break;
        }
        break;
    case '|':
        switch( pString[index + 1] ) {
        case '|':
            makeToken( OP_LOG_OR, current, &index );
            break;
        default:
            makeToken( OP_BIT_OR, current, &index );
            break;
        }
        break;
    case '<':
        switch( pString[index + 1] ) {
        case '<':
            makeToken( OP_SHIFT_LEFT, current, &index );
            break;
        case '=':
            makeToken( OP_LESSEQU, current, &index );
            break;
        default:
            makeToken( OP_LESSTHAN, current, &index );
            break;
        }
        break;
    case '>':
        switch( pString[index + 1] ) {
        case '>':
            makeToken( OP_SHIFT_RIGHT, current, &index );
            break;
        case '=':
            makeToken( OP_GREATEREQU, current, &index );
            break;
        default:
            makeToken( OP_GREATERTHAN, current, &index );
            break;
        }
        break;
    case '=':
        switch( pString[index + 1] ) {
        case '=':
            makeToken( OP_EQUAL, current, &index );
            break;
        default:
            makeToken( OP_ERROR, current, &index );
            break;
        }
        break;
    case '\"':
        makeStringToken( pString, current, &index );
        break;
    case '[':
        makeCmdToken( pString, current, &index );
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        makeNumberToken( pString, current, &index );
        break;
    default:
        makeFuncToken( pString, current, &index );
        break;
    }

    return( index + ( pString - inString ) );
}


STATIC void nextToken( void )
/****************************
 * Get the next token
 */
{
    if( *currentPtr != NULLCHAR ) {
        currentPtr += ScanToken( currentPtr, &currentToken );
        while( cisws( *currentPtr ) ) {
            ++currentPtr;
        }
    } else {
        currentToken.type = OP_ERROR;  // no more tokens
    }
}


STATIC void parseExpr( DATAVALUE *leftVal, char *inString )
/**********************************************************
 * Calls functions that parses and evaluates the given expression
 * contained in global variables currentPtr (pointer to the string to be
 * parsed)
 */
{
    assert( inString != NULL && leftVal != NULL );
    currentPtr = inString;
    nextToken();
    logorExpr( leftVal );
    if( leftVal->type != OP_INTEGER ) {
        PrtMsg( ERR | LOC | PARSE_IFEXPR );
    }
}


STATIC void logorExpr( DATAVALUE *leftVal )
/*****************************************/
{
    DATAVALUE   rightVal;

    logandExpr( leftVal );
    while( currentToken.type == OP_LOG_OR && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        logandExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number || rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void logandExpr( DATAVALUE *leftVal )
/******************************************/
{
    DATAVALUE   rightVal;

    bitorExpr( leftVal );
    while( currentToken.type == OP_LOG_AND && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        bitorExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number && rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void bitorExpr( DATAVALUE *leftVal )
/*****************************************/
{
    DATAVALUE   rightVal;

    bitxorExpr( leftVal );
    while( currentToken.type == OP_BIT_OR && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        bitxorExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number | rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void bitxorExpr( DATAVALUE *leftVal )
/******************************************/
{
    DATAVALUE   rightVal;

    bitandExpr( leftVal );
    while( currentToken.type == OP_BIT_XOR && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        bitandExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number ^ rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void bitandExpr( DATAVALUE *leftVal )
/******************************************/
{
    DATAVALUE   rightVal;

    equalExpr( leftVal );
    while( currentToken.type == OP_BIT_AND && leftVal->type != OP_ERROR) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        equalExpr( &rightVal );
        if( rightVal.type == OP_INTEGER ) {
            leftVal->data.number = leftVal->data.number & rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}


STATIC void equalExpr( DATAVALUE *leftVal )
/*****************************************/
{
    DATAVALUE   rightVal;

    relateExpr( leftVal );
    while( leftVal->type == OP_INTEGER || leftVal->type == OP_STRING ) {
        if( currentToken.type == OP_EQUAL ) {
            nextToken();
            relateExpr( &rightVal );
            if( leftVal->type == rightVal.type ) {
                switch( leftVal->type ) {
                case OP_INTEGER:
                    leftVal->data.number =
                        ( leftVal->data.number == rightVal.data.number );
                    break;
                case OP_STRING:
                    leftVal->data.number =
                        ( strcmp( leftVal->data.string, rightVal.data.string ) == 0 );
                    leftVal->type = OP_INTEGER;
                    break;
                default:
                    // error
                    break;
                }
            } else {
                leftVal->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_INEQU ) {
            nextToken();
            relateExpr( &rightVal );
            if( leftVal->type == rightVal.type ) {
                switch( leftVal->type ) {
                case OP_INTEGER:
                    leftVal->data.number =
                        ( leftVal->data.number != rightVal.data.number );
                    break;
                case OP_STRING:
                    leftVal->data.number =
                        ( strcmp( leftVal->data.string, rightVal.data.string ) != 0 );
                    leftVal->type = OP_INTEGER;
                    break;
                default:
                    // error
                    break;
                }
            } else {
                leftVal->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


STATIC void relateExpr( DATAVALUE *leftVal )
/******************************************/
{
    DATAVALUE   rVal;

    shiftExpr( leftVal );
    while( leftVal->type != OP_ERROR && leftVal->type != OP_STRING ) {
        if( leftVal->type != OP_INTEGER ) {
            leftVal->type = OP_ERROR;
            break;
        }
        if( currentToken.type == OP_LESSTHAN ) {
            nextToken();
            shiftExpr( &rVal );
            if( rVal.type == OP_INTEGER ) {
                leftVal->data.number = ( leftVal->data.number < rVal.data.number );
            } else {
                leftVal->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_LESSEQU ) {
            nextToken();
            shiftExpr( &rVal );
            if( rVal.type == OP_INTEGER ) {
                leftVal->data.number = ( leftVal->data.number <= rVal.data.number );
            } else {
                leftVal->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_GREATERTHAN ) {
            nextToken();
            shiftExpr( &rVal );
            if( rVal.type == OP_INTEGER ) {
                leftVal->data.number = ( leftVal->data.number > rVal.data.number );
            } else {
                leftVal->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_GREATEREQU ) {
            nextToken();
            shiftExpr( &rVal );
            if( rVal.type == OP_INTEGER ) {
                leftVal->data.number = ( leftVal->data.number >= rVal.data.number );
            } else {
                leftVal->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


STATIC void shiftExpr( DATAVALUE *leftValue )
/*******************************************/
{
    DATAVALUE   rightValue;
    unsigned_32 *leftNumber;

    addExpr( leftValue );
    leftNumber = (void *)&leftValue->data.number;
    while( leftValue->type != OP_ERROR && leftValue->type != OP_STRING ) {
        if( leftValue->type != OP_INTEGER ) {
            leftValue->type = OP_ERROR;
            break;
        }
        if( currentToken.type == OP_SHIFT_LEFT ) {
            nextToken();
            addExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                *leftNumber <<= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_SHIFT_RIGHT ) {
            nextToken();
            addExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                *leftNumber >>= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


STATIC void addExpr( DATAVALUE *leftValue )
/*****************************************/
{
    DATAVALUE   rightValue;

    multExpr( leftValue );
    while( leftValue->type != OP_ERROR && leftValue->type != OP_STRING) {
        if( leftValue->type != OP_INTEGER) {
            leftValue->type = OP_ERROR;
            break;
        }
        if( currentToken.type == OP_ADD ) {
            nextToken();
            multExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number += rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }

        } else if( currentToken.type == OP_SUBTRACT ) {
            nextToken();
            multExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number -= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


STATIC void multExpr( DATAVALUE *leftValue )
/******************************************/
{
    DATAVALUE   rightValue;

    unaryExpr( leftValue );
    while( leftValue->type != OP_ERROR && leftValue->type != OP_STRING ) {
        if( leftValue->type != OP_INTEGER ) {
            leftValue->type = OP_ERROR;
            break;
        }
        if( currentToken.type == OP_MULTIPLY ) {
            nextToken();
            unaryExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number *= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }

        } else if( currentToken.type == OP_DIVIDE ) {
            nextToken();
            unaryExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number /= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else if( currentToken.type == OP_MODULUS ) {
            nextToken();
            unaryExpr( &rightValue );
            if( rightValue.type == OP_INTEGER ) {
                leftValue->data.number %= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}


bool ExistFile( char const *inPath )
/***********************************
 * This function is to determine whether or not a particular
 * filename / directory exists  (for use with EXIST())
 */
{
     return( access( inPath, F_OK ) == 0 );
}


STATIC void unaryExpr( DATAVALUE *leftValue )
/********************************************
 * handles the unary expressions, strings and numbers
 */
{
    switch( currentToken.type ) {
    case OP_ADD:
        nextToken();
        unaryExpr( leftValue );
        if( leftValue->type != OP_INTEGER ) {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_SUBTRACT:
        nextToken();
        unaryExpr( leftValue );
        if( leftValue->type == OP_INTEGER ) {
            leftValue->data.number = - leftValue->data.number;
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_LOG_NEGATION:
        nextToken();
        unaryExpr( leftValue );
        if( leftValue->type == OP_INTEGER ) {
            leftValue->data.number = ( leftValue->data.number == 0 );
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_COMPLEMENT:
        nextToken();
        unaryExpr( leftValue );
        if( leftValue->type == OP_INTEGER ) {
            leftValue->data.number = (INT32)( ~(UINT32)leftValue->data.number );
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_PAREN_LEFT:
        nextToken();
        logorExpr( leftValue );
        if( currentToken.type == OP_PAREN_RIGHT ) {
            nextToken();
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_STRING:
        leftValue->type = currentToken.type;
        strcpy( leftValue->data.string, currentToken.data.string );
        nextToken();
        break;
    case OP_INTEGER:
        leftValue->type        = currentToken.type;
        leftValue->data.number = currentToken.data.number;
        nextToken();
        break;
    case OP_SHELLCMD:
        leftValue->type        = OP_INTEGER;
        PrtMsg( DBG | INF | EXECING_CMD, currentToken.data.string );
        leftValue->data.number = ExecCommand( currentToken.data.string );
        PrtMsg( DBG | INF | CMD_RETCODE, leftValue->data.number );
        nextToken();
        break;
    default:
        leftValue->type        = OP_ERROR;
        // error
    }
}


nest_level GetNestLevel( void )
/*****************************/
{
    return( nestLevel );
}
