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


#include <io.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

#include "macros.h"
#include "massert.h"
#include "make.h"
#include "memory.h"
#include "misc.h"
#include "mlex.h"
#include "mparse.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mstream.h"
#include "mtypes.h"

/*
 * This module presents a stream of characters to mlex.c.  Stripped from
 * the stream are all comments, and preprocessing directives.  Preprocessing
 * directives are handled invisibly to mlex.c.  This module also handles
 * line continuation.
 *
 */

#define MAX_PRE_TOK     8       /* chars needed for maximum keyword */
#define INCLUDE      "INCLUDE"  /* include directory for include file
                                    search*/


enum directiveTok {             /* must be kept in sync with directives */
    D_BLANK = -1,               /* a blank line */
    D_DEFINE,
    D_ELSE,
    D_ENDIF,
    D_ERROR,
    D_IF,   /* New Directive for MS Compatability adds binary and
             string operations present in NMAKE */
    D_IFDEF,
    D_IFEQ,
    D_IFEQI,
    D_IFNDEF,
    D_IFNEQ,
    D_IFNEQI,
    D_INCLUDE,
    D_INJECT,
    D_LOADDLL,
    D_MESSAGE,
    D_UNDEF,
    D_MAX
};

STATIC const char * const directives[] = {   /* table must be sorted */
    "define",
    "else",
    "endif",
    "error",
    "if",
    "ifdef",
    "ifeq",
    "ifeqi",
    "ifndef",
    "ifneq",
    "ifneqi",
    "include",
    "inject",
    "loaddll",
    "message",
    "undef"
};
#define NUM_DIRECT    ( sizeof( directives ) / sizeof( char * ) )

#define MAX_DIR_LEN     8           /* num chars incl null-terminator */

/*
    atStartOfLine == EOL if we are at the start of a line... otherwise
    it can have any other value.  This is a slight optimization for the
    critical code in PreGetCH().  DJG
*/
STATIC char     atStartOfLine;
STATIC STRM_T   lastChar;
STATIC BOOLEAN  doingPreProc;   /* are we doing some preprocessing? */


/*
 * MS Compatability extension to add the if (expression) functionality
 */
 // directory separators
#define isdir(_c) ((_c) == '/' || (_c) == BACKSLASH || (_c) == COLON)



STATIC int_32 makeHexNumber (char* inString,
                             int*  stringLength);
STATIC void doElIf( BOOLEAN (*logical)(void), enum directiveTok tok );

STATIC char* currentPtr;        // Pointer to current start in string
STATIC TOKEN_TYPE currentToken; // Contains the information for the current token


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
    BIT skip2endif : 1;
    BIT skip : 1;
    BIT elseFound : 1;
};

#define MAX_NEST    32                  /* maximum depth of if nesting */

STATIC struct nestIf nest[ MAX_NEST ];  /* stack for nesting */
STATIC size_t nestLevel;                /* items on stack */
STATIC struct nestIf curNest;           /* current skip info */


extern void PreProcInit( void )
/*****************************/
{
    StreamInit();

    atStartOfLine = EOL;
    doingPreProc = FALSE;


    curNest.skip2endif = FALSE;
    curNest.skip = FALSE;
    curNest.elseFound = FALSE;
    nestLevel = 0;
}


extern void PreProcFini( void )
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
    STRM_T  t;

    t = PreGetCH();
    while( isws( t ) ) t = PreGetCH();

    return( t );
}


STATIC STRM_T eatToEOL( void )
/*****************************
 * pre:
 * post:    atStartOfLine == EOL, 0 or more chars removed from input
 * returns: first ( EOL || STRM_END )
 */
{
    STRM_T  t;

    t = PreGetCH();
    while( t != EOL && t != STRM_END ) t = PreGetCH();

    return( t );
}


#pragma on (check_stack);
STATIC int getPreTok( void )
/***************************
 * pre:     the '!' has been eaten by caller
 * post:    first character following token is next char of input
 * returns: D_BLANK if no tokens on line, or token unrecognized,
 *          otherwise the D_ number of the token
 */
{
    TOKEN_T t;
    char    tok[ MAX_PRE_TOK ];
    int     pos;
    char    **key;
    char    *tmp;               /* to pass tok buf to bsearch */

    t = eatWhite();

    if( t == EOL ) {
        UnGetCH( t );
        return( D_BLANK );
    }

    pos = 0;
    while( isalpha( t ) && ( pos < MAX_PRE_TOK - 1 ) ) {
        tok[ pos++ ] = t;
        t = PreGetCH();
        // MS Compatability ELSE IFEQ can also be defined as ELSEIFEQ
        // similar for other types of if preprocessor directives
        if (pos == 4) {
            tok[pos] = NULLCHAR;
            if (strcmpi(directives[D_ELSE],tok) == 0)
            {
                break;
            }
        }
    }
    tok[ pos ] = NULLCHAR;

    UnGetCH( t );
    UnGetCH( eatWhite() );

    tmp = tok;
    key = bsearch( &tmp, directives, NUM_DIRECT, sizeof( char * ),
           (int (*) (const void*,const void*)) KWCompare );

    if( key == NULL ) {
        if( !curNest.skip ) {
            PrtMsg( ERR|LOC| UNK_PREPROC_DIRECTIVE, tok );
        }
        return( D_BLANK );
    }

    assert( ( key - (char **)directives >= 0 ) &&
            ( key - (char **)directives <= D_MAX ) );

    return( key - (char **)directives );
}
#pragma off(check_stack);


STATIC BOOLEAN ifDef( void )
/***************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: TRUE if macro is defined, FALSE otherwise
 */
{
    char        *name;
    char        *value;
    BOOLEAN     ret;

    assert( !curNest.skip2endif );

    name = DeMacro( MAC_PUNC );
    (void) eatToEOL();

    if( !IsMacroName( name ) ) {
        FreeSafe( name );
        return( FALSE );
    }

    value = GetMacroValue( name );
    ret = value != NULL;
    if (value != NULL) {
        FreeSafe( value );
    }
    FreeSafe( name );

    return( ret );
}


STATIC BOOLEAN ifNDef( void )
/***************************/
{
    return( !ifDef() );
}


STATIC void chopTrailWS( char *str )
/***********************************
 * chop trailing whitespace from str
 */
{
    char    *p;
    char    *last;  /* last non-whitespace char */

    last = NULL;
    p = str;
    while( *p ) {
        if( !isws( *p ) ) {
            last = p;
        }
        ++p;
    }
    if( last != NULL ) {
        last[1] = NULLCHAR;
    }
}

// process the operands found in %f
STATIC BOOLEAN ifOpProcess( void )
{

    char* test;
    DATAVALUE temp;

    assert( !curNest.skip2endif );

    test = DeMacro( EOL );
    (void) eatToEOL();

    parseExpr(&temp,test);

    FreeSafe( test );
    return ((BOOLEAN) temp.data.number);
}


/* MS Compatability  -
 * Allows for NMAKE compatability in binary and string operators
 */
STATIC BOOLEAN ifOp( void )
{
    return( ifOpProcess() );

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
    test = DeMacro( EOL );
    (void) eatToEOL();

    if( !IsMacroName( name ) ) {
        FreeSafe( name );
        FreeSafe( test );
        return;
    }

    value = WrnGetMacroValue( name );
    if( value == NULL ) {
        FreeSafe( name );
        FreeSafe( test );
        return;
    }

    FreeSafe( name );               /* don't need name any more */

    UnGetCH( EOL );
    InsString( value, TRUE );
    value = DeMacro( EOL );
    (void) eatToEOL();

    beg = SkipWS( test );           /* find first non-ws */

    chopTrailWS( beg );             /* chop trailing ws */

    *v1 = value;
    *v2 = StrDupSafe( beg );        /* 18-nov-91 */
    FreeSafe( test );
}



STATIC BOOLEAN ifEq( void )
/**************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: TRUE if macro equals text, FALSE otherwise
 */
{
    BOOLEAN ret;
    char const *v1;
    char *v2;

    ifEqProcess( &v1, &v2 );
    if( v1 == NULL ) {
        return( 0 );
    }
    ret = strcmp( v1, v2 ) == 0;

    FreeSafe( (void *) v1 );
    FreeSafe( v2 );

    return( ret );
}



STATIC BOOLEAN ifEqi( void )
/***************************
 * pre:
 * post:    atStartOfLine == EOL
 * returns: TRUE if macro equals text (case independence), FALSE otherwise
 */
{
    BOOLEAN ret;
    char const *v1;
    char *v2;

    ifEqProcess( &v1, &v2 );
    if( v1 == NULL ) {
        return( 0 );
    }
    ret = stricmp( v1, v2 ) == 0;

    FreeSafe( (void *) v1 );
    FreeSafe( v2 );

    return( ret );
}


STATIC BOOLEAN ifNEq( void )
/**************************/
{
    return( !ifEq() );
}


STATIC BOOLEAN ifNEqi( void )
/***************************/
{
    return( !ifEqi() );
}


STATIC void bangIf( BOOLEAN (*logical)(void), enum directiveTok tok )
/********************************************************************
 * pre:
 * post:    nestLevel > old(nestLevel); skip if false logical, or currently
 *          skipping; !elseFound
 * aborts:  if nestLevel == MAX_NEST
 */
{
    if( nestLevel == MAX_NEST ) {
        PrtMsg( FTL|LOC| IF_NESTED_TOO_DEEP );
    }

    /* save old nesting on the stack */
    nest[ nestLevel++ ] = curNest;

        /* remember that curNest still contains info from previous level */
    curNest.skip2endif = curNest.skip || curNest.skip2endif;
    curNest.skip = FALSE;
    curNest.elseFound = FALSE;

    if( !curNest.skip2endif ) {
        /* it is ok to interpret arguments to if */
        curNest.skip = !logical();
    } else {
        /* this entire block is to be skipped, don't interpret args to if */
        curNest.skip = TRUE;
        (void) eatToEOL();
    }

    if( curNest.skip ) {
        PrtMsg( DBG|INF|LOC| SKIPPING_BLOCK, directives[ tok ] );
    } else {
        PrtMsg( DBG|INF|LOC| ENTERING_BLOCK, directives[ tok ] );
    }
}


STATIC void bangEndIf( void )
/****************************
 * pre:
 * post:    atStartOfLine == EOL; curNest < old(curNest)
 * aborts:  if not nested
 */
{
    PrtMsg( DBG|INF|LOC| AT_ENDIF );

    if( nestLevel == 0 ) {
        PrtMsg( FTL|LOC| UNMATCHED_WITH_IF, directives[ D_ENDIF ] );
    }
    curNest = nest[ --nestLevel ];

    (void) eatToEOL();
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
        PrtMsg( FTL|LOC| UNMATCHED_WITH_IF, directives[ D_ELSE ] );
    }

    if( curNest.elseFound ) {
        PrtMsg( WRN|LOC| SKIPPING_AFTER_ELSE, directives[ D_ELSE ],
            directives[ D_ELSE ] );
        /* we have to set these because we may not have been skipping the
         * previous block
         */
        curNest.skip2endif = TRUE;
        curNest.skip = TRUE;
        return;
    }
    curNest.elseFound = TRUE;

    if( !curNest.skip2endif ) {
        /* check if we're not skipping. if !skip then we should skip the
         * else part
         */
        if( !curNest.skip ) {
            /* skip to the end - we've done a block in this nesting */
            curNest.skip = TRUE;
            curNest.skip2endif = TRUE;
        } else {
            /* we still haven't done a block in this nesting, so do the
             * else portion
             */
            curNest.skip = FALSE;
        }
    }

    if( curNest.skip ) {
        PrtMsg( DBG|INF|LOC| SKIPPING_BLOCK, directives[ D_ELSE ] );
    } else {
        PrtMsg( DBG|INF|LOC| ENTERING_BLOCK, directives[ D_ELSE ] );
    }
}


#pragma on (check_stack);
STATIC void doElIf( BOOLEAN (*logical)(void), enum directiveTok tok )
/********************************************************************
 * post:    skip if !logical || skip2endif
 * aborts:  if not nested
 * errors:  if elseFound
 */
{
    char buf[ MAX_DIR_LEN * 2 ];

    FmtStr( buf, "%s %s", directives[ D_ELSE ], directives[ tok ] );

    if( nestLevel == 0 ) {
        PrtMsg( FTL|LOC| UNMATCHED_WITH_IF, buf );
    }

    if( curNest.elseFound ) {
        PrtMsg( WRN|LOC| SKIPPING_AFTER_ELSE, buf, directives[ D_ELSE ] );
        /* we have to set these because we may not have been skipping the
         * previous block
         */
        curNest.skip2endif = TRUE;
        curNest.skip = TRUE;
        (void) eatToEOL();
        return;
    }

    if( !curNest.skip2endif ) {
        /* check if we're not skipping. if !skip then we should skip the
         * else if part
         */
        if( !curNest.skip ) {
            /* skip to the end - we've done a block in this nesting */
            curNest.skip = TRUE;
            curNest.skip2endif = TRUE;
            (void) eatToEOL();
        } else {
            /* we still haven't done a block in this nesting, so try this
             * logical
             */
            curNest.skip = !logical();
        }
    } else {
        (void) eatToEOL();
    }

    if( curNest.skip ) {
        PrtMsg( DBG|INF|LOC| SKIPPING_BLOCK, buf );
    } else {
        PrtMsg( DBG|INF|LOC| ENTERING_BLOCK, buf );
    }
}
#pragma off(check_stack);


STATIC void bangElse( void )
/***************************
 * pre:     next character of input is first after 'else' token
 * post:    atStartOfLine == EOL; skip may be modified
 * aborts:  if illegal directive
 */
{
    enum directiveTok tok;

    tok = getPreTok();
    switch( tok ) {
    case D_BLANK:
        (void) eatToEOL();
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
        (void) eatToEOL();
        PrtMsg( FTL|LOC| NOT_ALLOWED_AFTER_ELSE, directives[ tok ],
            directives[ D_ELSE ] );
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
        (void) eatToEOL();
    } else {
        DefMacro( name );
    }
    FreeSafe( name );
}

static char *skipWhileWS( char *p )
{
    for( ; *p != '\0'; ++p ) {
        if( ! isws( *p ) ) break;
    }
    return( p );
}

static char *skipUntilWS( char *p )
{
    for( ; *p != '\0'; ++p ) {
        if( isws( *p ) ) break;
    }
    return( p );
}


// !inject <text> <mac-name1> <mac-name2> ... <mac-nameN>
STATIC void bangInject( void )
/*****************************
 * post:    atStartOfLine == EOL
 * errors:  none
 */
{
    char *text;
    char *contents;
    char *end_contents;
    char *curr;
    char *mac_name;
    char *value;

    assert( !curNest.skip );
    text = DeMacro( EOL );
    (void) eatToEOL();
    contents = skipWhileWS( text );
    if( *contents == '\0' ) {
        FreeSafe( text );
        return;
    }
    end_contents = skipUntilWS( contents );
    if( *end_contents == '\0' ) {
        FreeSafe( text );
        return;
    }
    *end_contents = '\0';
    curr = end_contents + 1;
    for(;;) {
        curr = skipWhileWS( curr );
        if( *curr == '\0' ) break;
        mac_name = curr;
        curr = skipUntilWS( curr );
        if( *curr != '\0' ) {
            *curr = '\0';
            ++curr;
        }
        if( !IsMacroName( mac_name ) ) {
            break;
        }
        UnGetCH( EOL );
        InsString( contents, FALSE );
        value = GetMacroValue( mac_name );
        if( value != NULL ) {
            InsString( " ", FALSE );
            InsString( value, TRUE );
        }
        DefMacro( mac_name );
    }
    FreeSafe( text );
}


// !loaddll <cmd-name> <dll-name> [<entry-pt>]
STATIC void bangLoadDLL( void )
/******************************
 * post:    atStartOfLine == EOL
 * errors:  none
 */
{
    char *text;
    char *cmd_name;
    char *end_cmd_name;
    char *dll_name;
    char *end_dll_name;
    char *ent_name;
    char *end_ent_name;

    assert( !curNest.skip );
    text = DeMacro( EOL );
    (void) eatToEOL();
    cmd_name = skipWhileWS( text );
    if( *cmd_name == '\0' ) {
        FreeSafe( text );
        return;
    }
    end_cmd_name = skipUntilWS( cmd_name );
    if( *end_cmd_name == '\0' ) {
        FreeSafe( text );
        return;
    }
    *end_cmd_name = '\0';
    dll_name = skipWhileWS( end_cmd_name + 1 );
    if( *dll_name == '\0' ) {
        FreeSafe( text );
        return;
    }
    end_dll_name = skipUntilWS( dll_name );
    if( *end_dll_name == '\0' ) {
        OSLoadDLL( cmd_name, dll_name, NULL );
        FreeSafe( text );
        return;
    }
    *end_dll_name = '\0';
    ent_name = skipWhileWS( end_dll_name + 1 );
    if( *ent_name == '\0' ) {
        OSLoadDLL( cmd_name, dll_name, NULL );
        FreeSafe( text );
        return;
    }
    end_ent_name = skipUntilWS( ent_name );
    if( *end_ent_name == '\0' ) {
        OSLoadDLL( cmd_name, dll_name, ent_name );
        FreeSafe( text );
        return;
    }
    *end_ent_name = '\0';
    OSLoadDLL( cmd_name, dll_name, ent_name );
    FreeSafe( text );
}


STATIC void bangUnDef( void )
/****************************
 * post:    atStartOfLine == EOL; macro may be undefined
 * errors:  if env_variable || !IsMacroName || not-defined
 */
{
    char        *name;
    char        *value;

    assert( !curNest.skip );

    name = DeMacro( MAC_PUNC );
    (void) eatToEOL();

    if( !IsMacroName( name ) ) {
        FreeSafe( name );
        return;
    }

    value = GetMacroValue( name );
    if( value == NULL ) {
        PrtMsg( DBG|WRN|LOC| TRYING_UNDEF_UNDEF, directives[ D_UNDEF ] );
        FreeSafe( name );
        return;
    }

    UnDefMacro( name );
    FreeSafe( name );
    FreeSafe( value );
}

STATIC char* formatLongFileName(char* text) {

    char* ret;
    char* currentRet;
    char* currentTxt;

    assert(text != NULL);
    ret = StrDupSafe(text);
    currentRet = ret;
    currentTxt = text;

    if (currentTxt[0] == DOUBLEQUOTE) {
        ++currentTxt;
    }
    while (*currentTxt != NULLCHAR && *currentTxt != DOUBLEQUOTE) {
        if (*currentTxt == '\\') {
            if (*(currentTxt +1) == DOUBLEQUOTE) {
                ++currentTxt;
            }
        }
        *(currentRet++) = *(currentTxt++);
    }
    *(currentRet) = NULLCHAR;
    if (*currentTxt == DOUBLEQUOTE) {
        if (*(currentTxt + 1) != NULLCHAR) {
            PrtMsg( ERR|LOC| UNABLE_TO_INCLUDE, text );
            FreeSafe(ret);
            return(NULL);
        }
    }
    return(ret);
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
    char    *current;
    char    full_path[ _MAX_PATH ];
    RET_T   ret;

    assert( !curNest.skip );

    text = DeMacro( EOL );
    (void) eatToEOL();

    chopTrailWS( text );    /* get rid of trailing ws */


    if (*text == LESSTHAN) {
        current = text;
        while (*current != GREATERTHAN && *current != NULLCHAR) {
            ++current;
        }
        if (*current == GREATERTHAN) {
            *current = NULLCHAR;
            temp = text;
            text = formatLongFileName(temp+1);
            if (text == NULL) {
                FreeSafe( temp );
                return;
            }
            for(;;) {
                if (*current == NULLCHAR) {
                    _searchenv(text,INCLUDE,full_path);
                    ret = RET_ERROR;
                    if (*full_path != NULLCHAR) {
                        ret = InsFile( full_path, FALSE );
                    }
                    if (ret == RET_ERROR) {
                        PrtMsg( ERR|LOC| UNABLE_TO_INCLUDE, text );
                    }
                    break;
                }
                // check if there are any trailing characters if there are
                // then error
                if (!isws(*current)) {
                    PrtMsg( ERR|LOC| UNABLE_TO_INCLUDE, text );
                    break;
                }
                ++current;
            }
        } else {
              PrtMsg( ERR|LOC| UNABLE_TO_INCLUDE, text );
        }
    } else {
        temp = text;
        text = formatLongFileName(text);
        if (text == NULL) {
            FreeSafe( temp );
            return;
        }
        if( InsFile( text, FALSE ) != RET_SUCCESS ) {
            PrtMsg( ERR|LOC| UNABLE_TO_INCLUDE, text );
        }
    }
    FreeSafe( temp );
    FreeSafe( text );
}

STATIC void bangMessage( void )
/****************************
 * post:    atStartOfLine == EOL
 */
{
    char *text;

    assert( !curNest.skip );

    text = DeMacro( EOL );
    (void) eatToEOL();

    chopTrailWS( text );

    PrtMsg(PRNTSTR, text );
    FreeSafe( text );
}


STATIC void bangError( void )
/****************************
 * post:    atStartOfLine == EOL
 */
{
    char *text;

    assert( !curNest.skip );

    text = DeMacro( EOL );
    (void) eatToEOL();

    chopTrailWS( text );

    PrtMsg( ERR|LOC|NEOL| USER_ERROR );
    PrtMsg( ERR|PRNTSTR, text );
    FreeSafe( text );
}


STATIC void handleBang( void )
/*****************************
 * post:    atStartOfLine == EOL
 */
{
    enum directiveTok tok;

    tok = getPreTok();
    switch( tok ) {
                    /* these are executed regardless of skip */
    case D_BLANK:   (void) eatToEOL();          break;
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
            }
        } else {
                    /* otherwise, we just eat it up */
            (void) eatToEOL();
        }
    }
}


extern STRM_T PreGetCH( void )
/*****************************
 * returns: next character of input that is not a preprocessor directive
 * errors:  if an EOF occurs while nested
 */
{
    STRM_T  t;
    STRM_T  temp;
    BOOLEAN skip;

    t = GetCHR();
    if( !Glob.preproc ) {
        return( t );
    }

    for(;;) {
        if(( atStartOfLine == EOL  && !doingPreProc )  ||
             t == TMP_EOL && !doingPreProc) {
            if (t == TMP_EOL) {
                // Throw away the unwanted TMP character
                t = GetCHR();
                if (t != BANG) {
                    UnGetCH( t );
                    t = TMP_EOL;
                }
            }
            doingPreProc = TRUE;
            while( t == BANG ) {
                handleBang();

                assert( atStartOfLine == EOL );

                t = GetCHR();
            }
            doingPreProc = FALSE;
        }

        /* now we have a character of input */

        atStartOfLine = t;
        temp          = t;

        skip = curNest.skip && !doingPreProc;

        if( t == TMP_EOL) {
            t = GetCHR();
        }
        if( t == COMMENT && lastChar != DOLLAR ) {
            t = GetCHR();
            while( t != EOL && t != STRM_END ) t = GetCHR();
            if (temp == TMP_EOL) {
                t = TMP_EOL;
            }
            /* we already have next char in t */
            continue;
        }


        if( t == STRM_END ) {
            if( nestLevel > 0 ) {
                PrtMsg( WRN| EOF_BEFORE_ENDIF, directives[ D_ENDIF ] );
                nestLevel = 0;          /* reset nesting level */
            }
            curNest.skip = FALSE;       /* so we don't skip a later file */
            curNest.skip2endif = FALSE;

            atStartOfLine = EOL;        /* reset preprocessor */
            lastChar = STRM_END;
            return( t );
        }

        if( inlineLevel > 0 ) {   // We are currently defining an inline file
            lastChar = t;         // ignore all special characters ie {nl}
            if ( skip ) {
                t = GetCHR();
                continue;
            }
            return (t);
        } else {
            if (Glob.microsoft && t == MS_LINECONT) {
                t = GetCHR();
                if (t == EOL) {
                    lastChar = SPACE;
                    if ( skip ) {
                        t = TMP_EOL;
                        continue;
                    }
                    // place holder for temporary EOL
                    // this is to be able to implement the
                    // bang statements after line continues
                    UnGetCH(TMP_EOL);
                    return (SPACE);
                } else {
                    lastChar = MS_LINECONT;
                    if ( skip ) {
                        t = GetCHR();
                        continue;
                    }
                    UnGetCH(t);
                    return (MS_LINECONT);
                }
            }


            if( t != LINECONT ) {
                if( t != UNIX_LINECONT || !Glob.unix ) {
                    lastChar = t;
                    if( skip ) {
                        t = GetCHR();   /* must get next char */
                        continue;
                    }
                    return( t );
                }
                t = GetCHR();
                if( t != EOL ) {
                    lastChar = UNIX_LINECONT;
                    if( skip ) continue;        /* already have next char */
                    UnGetCH( t );
                    return( UNIX_LINECONT );
                } else {
                    UnGetCH( TMP_EOL );
                }
            } else {
                t = GetCHR();           /* check if '&' followed by {nl} */
                if( t != EOL || lastChar == '^' ||
                    lastChar == '[' || lastChar == ']' ) {
                           /* nope... restore state */
                    lastChar = LINECONT;
                    if( skip ) continue;        /* already have next char */
                    UnGetCH( t );
                    return( LINECONT );
                } else {
                    UnGetCH( TMP_EOL );
                }
            }
        }
        t = GetCHR();
    }
}


// Finds the next character that has no white space
STATIC char* removeWhiteSP(char* inString)
{
    int index = 0;

    while (inString[index] == SPACE ||
           inString[index] == TAB)
    {
        ++index;
    }
    return (inString + index);
}

STATIC void makeToken(enum Tokens        type,
                      TOKEN_TYPE*        current,
                      int*               index) {

    switch (type) {
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

STATIC void makeNumberToken (char*       inString,
                             TOKEN_TYPE* current,
                             int*        index) {
    int_32      value;
    char*       currentChar;
    char        c;
    int         hexLength;

    currentChar = inString;
    value       = 0;
    *index      = 0;
    c = currentChar[0];
    if( c == '0' ) {                            // octal or hex number
        ++currentChar;
        c = currentChar[0];
        if( c == 'x'  ||  c == 'X' ) {          // hex number
            ++currentChar;
            value = makeHexNumber(currentChar,&hexLength);
            currentChar += hexLength;
        } else {                                // octal number
            while( c >= '0'  &&  c <= '7' ) {
                value = value * 8 + c - '0';
                ++currentChar;
                c = currentChar[0];
            }
        }
    } else {                                    // decimal number
        while( c >= '0'  &&  c <= '9' ) {
            value = value * 10 + c - '0';
            ++currentChar;
            c = currentChar[0];
        }
    }
    current->data.number= value;
    current->type= OP_INTEGER;

    *index = currentChar - inString;

}

STATIC int_32 makeHexNumber (char* inString,
                             int*  stringLength)
{
    int_32     value;
    char       c;
    char*      currentChar;

    value = 0;
    currentChar = inString;
    while (1) {
        c = currentChar[0];
        if ( c >= '0' && c <= '9') {
            c = c - '0';
        } else if (c >= 'a' && c <= 'f' ) {
            c = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F' ) {
            c = c - 'A' + 10;
        } else {
            break;
        }
        value = value * 16 + c;
        ++currentChar;
    }
    *stringLength = currentChar - inString;
    return (value);

}

STATIC void makeStringToken (char*       inString,
                             TOKEN_TYPE* current,
                             int*        index) {

    int      inIndex;
    int      currentIndex;

    inIndex      = 1;   // skip initial DOUBLEQUOTE
    currentIndex = 0;
    current->type= OP_STRING;
    while (1)
    {
        if (inString[inIndex] == DOUBLEQUOTE) {
            //skip the second double quote
            ++inIndex;
            break;
        }
        if (currentIndex >= MAX_STRING)
        {
            current->type = OP_ENDOFSTRING;
            break;
        }
        switch  (inString[inIndex]) {
            // error did not find closing quotation
            case NULLCHAR :
            case EOL:
            case COMMENT:
                current->type = OP_ERROR;
                break;
            default:
            current->data.string[currentIndex] = inString[inIndex];

        }

        if (current->type == OP_ERROR) {
            break;
        }
        ++inIndex;
        ++currentIndex;

    }

    current->data.string[currentIndex] = NULLCHAR;
    *index = inIndex;
}

STATIC void makeAlphaToken(char*       inString,
                           TOKEN_TYPE* current,
                           int*        index) {

    int      inIndex;
    int      currentIndex;

    inIndex      = 0;
    currentIndex = 0;
    current->type= OP_STRING;

    // Note that in this case we are looking at a string that has no
    // quotations
    while ( inString[inIndex]!= PAREN_RIGHT &&
            inString[inIndex]!= PAREN_LEFT  &&
            !isws(inString[inIndex]) ) {

        if (currentIndex >= MAX_STRING)
        {
            current->type = OP_ENDOFSTRING;
            break;
        }
        current->data.string[currentIndex] = inString[inIndex];

        ++inIndex;
        ++currentIndex;
    }


    current->data.string[currentIndex] = NULLCHAR;
    *index = inIndex;
}


STATIC void ScanToken (char *             inString,
                      TOKEN_TYPE*        current,
                      int *              tokenLength) {

    char   *currentString;
    int    index = 0;

    currentString = removeWhiteSP (inString);

    if (currentString[index] != EOL ||
        currentString[index] != NULLCHAR ||
        currentString[index] != COMMENT)
    {
        switch (currentString[index]) {
            case COMPLEMENT:     makeToken(OP_COMPLEMENT,
                                           current,
                                           &index);
                                 break;
            case ADD:            makeToken(OP_ADD,
                                           current,
                                           &index);
                                 break;
            case SUBTRACT:       makeToken(OP_SUBTRACT,
                                           current,
                                           &index);
                                 break;
            case MULTIPLY:       makeToken(OP_MULTIPLY,
                                           current,
                                           &index);
                                 break;
            case DIVIDE:         makeToken(OP_DIVIDE,
                                           current,
                                           &index);
                                 break;
            case MODULUS:        makeToken(OP_MODULUS,
                                           current,
                                           &index);
                                 break;
            case BIT_XOR:        makeToken(OP_BIT_XOR,
                                           current,
                                           &index);
                                 break;
            case PAREN_LEFT:     makeToken(OP_PAREN_LEFT,
                                           current,
                                           &index);
                                 break;
            case PAREN_RIGHT:    makeToken(OP_PAREN_RIGHT,
                                           current,
                                           &index);
                                 break;
            case LOG_NEGATION:
                switch (currentString[index+1]) {
                   case EQUAL:   makeToken(OP_INEQU,
                                           current,
                                           &index);
                                 break;
                   default:      makeToken(OP_LOG_NEGATION,
                                           current,
                                           &index);
                                 break;
                }
                break;
            case BIT_AND:
                switch (currentString[index+1]) {
                   case BIT_AND: makeToken(OP_LOG_AND,
                                           current,
                                           &index);
                                 break;
                   default:      makeToken(OP_BIT_AND,
                                           current,
                                           &index);
                                 break;
                }
                break;
            case BIT_OR:
                switch (currentString[index+1]) {
                   case BIT_OR:  makeToken(OP_LOG_OR,
                                           current,
                                           &index);
                                 break;
                   default:      makeToken(OP_BIT_OR,
                                           current,
                                           &index);
                                 break;
                }
                break;
            case LESSTHAN:
                switch (currentString[index+1]) {
                   case LESSTHAN: makeToken(OP_SHIFT_LEFT,
                                            current,
                                            &index);
                                  break;
                   case EQUAL:    makeToken(OP_LESSEQU,
                                            current,
                                            &index);
                                  break;
                   default:       makeToken(OP_LESSTHAN,
                                            current,
                                            &index);
                                  break;
                }
                break;
            case GREATERTHAN:
                switch (currentString[index+1]) {
                   case GREATERTHAN: makeToken(OP_SHIFT_RIGHT,
                                               current,
                                               &index);
                                     break;
                   case EQUAL:       makeToken(OP_GREATEREQU,
                                               current,
                                               &index);
                                     break;
                   default:          makeToken(OP_GREATERTHAN,
                                               current,
                                               &index);
                                     break;
                }
                break;
            case EQUAL:
                switch (currentString[index+1]) {
                    case EQUAL: makeToken(OP_EQUAL,
                                          current,
                                          &index);
                                break;
                    default:
                         makeToken(OP_ERROR,
                                   current,
                                   &index);
                         break;

                }
                break;
            case DOUBLEQUOTE: makeStringToken(currentString,
                                              current,
                                              &index);
                break;
            default:
                if (currentString[index] >= '0' &&
                    currentString[index] <= '9') {
                    makeNumberToken(currentString,
                                    current,
                                    &index);
                } else {
                    // parses only to get alphanumeric characters for
                    // purpose of getting special functions ie. EXIST,
                    // defined
                    // if special characters are needed
                    // enclose in quotes
                    makeAlphaToken(currentString,
                                   current,
                                   &index);
                }
                break;

        }
    }
    else
    {
        makeToken(OP_ENDOFSTRING,
                  current,
                  &index);
    }

    *tokenLength = index + (currentString - inString);

}


// Get the next token
STATIC void nextToken() {
    int tokenLength;

    if (*currentPtr != NULLCHAR) {
        ScanToken(currentPtr,&currentToken,&tokenLength);
        currentPtr += tokenLength;
        while (isws(*currentPtr)) {
            ++currentPtr;
        }
    } else {
        currentToken.type = OP_ERROR;  // no more tokens
    }
}


// Taking a peek at the next token
STATIC enum Tokens preToken() {
    int tokenLength;
    DATAVALUE nextToken;

    if (*currentPtr != NULLCHAR) {
        ScanToken(currentPtr,&nextToken,&tokenLength);
    } else {
        nextToken.type = OP_ERROR;  // no more tokens
    }
    return (nextToken.type);
}


/*
 * Calls functions that parses and evaluates the given expression
 * contained in global variables currentPtr (pointer to the string to be
 * parsed)
 */
STATIC void parseExpr (DATAVALUE* leftVal, char* inString) {

    assert(inString != NULL && leftVal != NULL);
    currentPtr = inString;
    nextToken();
    logorExpr(leftVal);
    if (leftVal->type != OP_INTEGER) {
        PrtMsg( ERR|LOC| PARSE_IFEXPR);
    }
}


STATIC void logorExpr (DATAVALUE* leftVal) {
    DATAVALUE rightVal;

    logandExpr(leftVal);
    while (currentToken.type == OP_LOG_OR &&
           leftVal->type      != OP_ERROR) {
        if (leftVal->type     != OP_INTEGER) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        logandExpr(&rightVal);
        if (rightVal.type == OP_INTEGER) {
            leftVal->data.number = (leftVal->data.number ||
                                    rightVal.data.number);
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}

STATIC void logandExpr (DATAVALUE* leftVal) {
    DATAVALUE rightVal;

    bitorExpr(leftVal);
    while (currentToken.type == OP_LOG_AND &&
           leftVal->type      != OP_ERROR) {
        if (leftVal->type     != OP_INTEGER) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        bitorExpr(&rightVal);
        if (rightVal.type == OP_INTEGER) {
            leftVal->data.number = leftVal->data.number &&
                                   rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}

STATIC void bitorExpr (DATAVALUE* leftVal) {
    DATAVALUE rightVal;

    bitxorExpr(leftVal);
    while (currentToken.type == OP_BIT_OR &&
           leftVal->type      != OP_ERROR) {
        if (leftVal->type     != OP_INTEGER) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        bitxorExpr(&rightVal);
        if (rightVal.type == OP_INTEGER) {
            leftVal->data.number = leftVal->data.number |
                                   rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }

    }
}

STATIC void bitxorExpr (DATAVALUE* leftVal) {
    DATAVALUE rightVal;

    bitandExpr(leftVal);
    while (currentToken.type == OP_BIT_XOR &&
           leftVal->type      != OP_ERROR) {
        if (leftVal->type     != OP_INTEGER) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        bitandExpr(&rightVal);
        if (rightVal.type == OP_INTEGER) {
            leftVal->data.number = leftVal->data.number ^
                                   rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}

STATIC void bitandExpr (DATAVALUE* leftVal) {
    DATAVALUE rightVal;

    equalExpr(leftVal);
    while (currentToken.type == OP_BIT_AND &&
           leftVal->type      != OP_ERROR) {
        if (leftVal->type     != OP_INTEGER) {
            leftVal->type = OP_ERROR;
            break;
        }
        nextToken();
        equalExpr(&rightVal);
        if (rightVal.type == OP_INTEGER) {
            leftVal->data.number = leftVal->data.number &
                                   rightVal.data.number;
        } else {
            leftVal->type = OP_ERROR;
        }
    }
}

STATIC void equalExpr(DATAVALUE* leftVal) {
    DATAVALUE rightVal;

    relateExpr(leftVal);
    while (leftVal->type == OP_INTEGER ||
           leftVal->type == OP_STRING) {
        if (currentToken.type == OP_EQUAL) {
            nextToken();
            relateExpr(&rightVal);
            if (leftVal->type == rightVal.type) {
                switch (leftVal->type) {
                case OP_INTEGER:
                    leftVal->data.number = (leftVal->data.number ==
                                           rightVal.data.number);
                    break;
                case OP_STRING:
                    leftVal->data.number = (strcmp(leftVal->data.string,
                                             rightVal.data.string) == 0);
                    leftVal->type = OP_INTEGER;
                    break;
                default:
                    // error
                    break;
                }
            } else {
                leftVal->type = OP_ERROR;
            }
        } else if (currentToken.type == OP_INEQU) {
            nextToken();
            relateExpr(&rightVal);
            if (leftVal->type == rightVal.type) {
                switch (leftVal->type) {
                case OP_INTEGER:
                    leftVal->data.number = (leftVal->data.number !=
                                           rightVal.data.number);
                    break;
                case OP_STRING:
                    leftVal->data.number = (strcmp(leftVal->data.string,
                                             rightVal.data.string) != 0);
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

STATIC void relateExpr(DATAVALUE* leftVal) {
    DATAVALUE rightVal;

    shiftExpr(leftVal);
    while (leftVal->type      != OP_ERROR &&
           leftVal->type      != OP_STRING) {
        if (leftVal->type     != OP_INTEGER) {
            leftVal->type = OP_ERROR;
            break;
        }
        if (currentToken.type == OP_LESSTHAN) {
            nextToken();
            shiftExpr(&rightVal);
            if (rightVal.type == OP_INTEGER) {
                leftVal->data.number = leftVal->data.number <
                                       rightVal.data.number;
            }
            else {
                leftVal->type = OP_ERROR;
            }
        } else if (currentToken.type == OP_LESSTHAN) {
            nextToken();
            shiftExpr(&rightVal);
            if (rightVal.type == OP_INTEGER) {
                leftVal->data.number = leftVal->data.number <
                                       rightVal.data.number;
            }
            else {
                leftVal->type = OP_ERROR;
            }
        } else if (currentToken.type == OP_LESSEQU) {
            nextToken();
            shiftExpr(&rightVal);
            if (rightVal.type == OP_INTEGER) {
                leftVal->data.number = leftVal->data.number <=
                                       rightVal.data.number;
            }
            else {
                leftVal->type = OP_ERROR;
            }
        } else if (currentToken.type == OP_GREATERTHAN) {
            nextToken();
            shiftExpr(&rightVal);
            if (rightVal.type == OP_INTEGER) {
                leftVal->data.number = leftVal->data.number >
                                       rightVal.data.number;
            }
            else {
                leftVal->type = OP_ERROR;
            }
        } else if (currentToken.type == OP_GREATEREQU) {
            nextToken();
            shiftExpr(&rightVal);
            if (rightVal.type == OP_INTEGER) {
                leftVal->data.number = leftVal->data.number >=
                                       rightVal.data.number;
            }
            else {
                leftVal->type = OP_ERROR;
            }
        } else {
            break;
        }

    }
}

STATIC void shiftExpr(DATAVALUE* leftValue) {
    DATAVALUE rightValue;
    addExpr (leftValue);
    while (leftValue->type != OP_ERROR &&
           leftValue->type != OP_STRING) {
        if (leftValue->type != OP_INTEGER) {
            leftValue->type = OP_ERROR;
            break;
        }
        if (currentToken.type == OP_SHIFT_LEFT) {
            nextToken();
            addExpr(&rightValue);
            if (rightValue.type == OP_INTEGER) {
                leftValue->data.number <<= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }

        } else if (currentToken.type == OP_SHIFT_RIGHT) {
            nextToken();
            addExpr(&rightValue);
            if (rightValue.type == OP_INTEGER) {
                leftValue->data.number >>= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else {
            break;
        }


    }
}

STATIC void addExpr(DATAVALUE* leftValue) {
    DATAVALUE rightValue;
    multExpr (leftValue);
    while (leftValue->type != OP_ERROR &&
           leftValue->type != OP_STRING) {
        if (leftValue->type != OP_INTEGER) {
            leftValue->type = OP_ERROR;
            break;
        }
        if (currentToken.type == OP_ADD) {
            nextToken();
            multExpr(&rightValue);
            if (rightValue.type == OP_INTEGER) {
                leftValue->data.number += rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }

        } else if (currentToken.type == OP_SUBTRACT) {
            nextToken();
            multExpr(&rightValue);
            if (rightValue.type == OP_INTEGER) {
                leftValue->data.number -= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else {
            break;
        }


    }
}
STATIC void multExpr(DATAVALUE* leftValue) {
    DATAVALUE rightValue;
    unaryExpr (leftValue);
    while (leftValue->type != OP_ERROR &&
           leftValue->type != OP_STRING) {
        if (leftValue->type != OP_INTEGER) {
            leftValue->type = OP_ERROR;
            break;
        }
        if (currentToken.type == OP_MULTIPLY) {
            nextToken();
            unaryExpr(&rightValue);
            if (rightValue.type == OP_INTEGER) {
                leftValue->data.number *= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }

        } else if (currentToken.type == OP_DIVIDE) {
            nextToken();
            unaryExpr(&rightValue);
            if (rightValue.type == OP_INTEGER) {
                leftValue->data.number /= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else if (currentToken.type == OP_MODULUS) {
            nextToken();
            unaryExpr(&rightValue);
            if (rightValue.type == OP_INTEGER) {
                leftValue->data.number %= rightValue.data.number;
            } else {
                leftValue->type = OP_ERROR;
            }
        } else {
            break;
        }
    }
}

// This function is to determine whether or not a particular
// filename / directory exists  (for use with EXIST())
extern BOOLEAN existFile(char *inPath) {
     if (access(inPath, F_OK) == 0 ) {
         return (TRUE);
     }
     return (FALSE);
}


// handles the unary expressions, strings and numbers
// identifies the logical functions EXIST and DEFINED
STATIC void unaryExpr (DATAVALUE* leftValue) {

    enum Tokens type;
    char        *value;

    switch (currentToken.type) {
    case OP_ADD:
        nextToken();
        unaryExpr(leftValue);
        if (leftValue->type != OP_INTEGER) {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_SUBTRACT:
        nextToken();
        unaryExpr(leftValue);
        if (leftValue->type == OP_INTEGER) {
            leftValue->data.number = - leftValue->data.number;
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_LOG_NEGATION:
        nextToken();
        unaryExpr(leftValue);
        if (leftValue->type == OP_INTEGER) {
            leftValue->data.number = !(leftValue->data.number);
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_COMPLEMENT:
        nextToken();
        unaryExpr(leftValue);
        if (leftValue->type == OP_INTEGER) {
            leftValue->data.number = ~(leftValue->data.number);
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_PAREN_LEFT:
        nextToken();
        logorExpr(leftValue);
        if (currentToken.type == OP_PAREN_RIGHT) {
            nextToken();
        } else {
            leftValue->type = OP_ERROR;
        }
        break;
    case OP_STRING:
        // check to see if the next token is a parenthesis
        // without changing the value of currentToken
        if (preToken() == OP_PAREN_LEFT) {
            if (strcmpi(currentToken.data.string,
                DEFINED) == 0) {
                type = OP_DEFINED;
            } else if (strcmpi(currentToken.data.string,
                EXIST) == 0) {
                type = OP_EXIST;
            } else {
                leftValue->type = OP_ERROR;
                break;
            }
            nextToken();
            nextToken();
            if (currentToken.type == OP_STRING) {
                if (preToken()== OP_PAREN_RIGHT) {
                    switch (type) {
                        // Check if macro is defined
                    case OP_DEFINED:
                        if( !IsMacroName( currentToken.data.string)) {
                            leftValue->type        = OP_INTEGER;
                            leftValue->data.number = FALSE;
                        } else {
                            leftValue->type        = OP_INTEGER;
                            value = GetMacroValue(currentToken.data.string);
                            if (value != NULL) {
                                leftValue->data.number = TRUE;
                                FreeSafe( value );
                            } else {
                                leftValue->data.number = FALSE;
                            }

                        }
                        break;
                    case OP_EXIST:
                        // check if the input file is defined
                        leftValue->type = OP_INTEGER;
                        leftValue->data.number =
                                        existFile(currentToken.data.string);
                        break;
                    default:
                        leftValue->type = OP_ERROR;
                        break;
                    }
                    nextToken();
                    nextToken();
                } else {
                    currentToken.type = OP_ERROR;
                }
            } else {
                leftValue->type = OP_ERROR;
            }
        // If the following token is not an open parenthesis then
        // the string is just a normal string
        //
        } else {
            leftValue->type        = currentToken.type;
            strcpy(leftValue->data.string,currentToken.data.string);
            nextToken();
        }
        break;
    case OP_INTEGER:
        leftValue->type        = currentToken.type;
        leftValue->data.number = currentToken.data.number;
        nextToken();
        break;
    default:
        leftValue->type        = OP_ERROR;
        // error
    }
}
