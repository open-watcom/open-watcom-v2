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


#include <stdlib.h>

#include "massert.h"
#include "mtypes.h"
#include "mlex.h"
#include "macros.h"
#include "make.h"
#include "mmemory.h"
#include "mmisc.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mvecstr.h"


char    *targ_path;
char    *dep_path;


STATIC TOKEN_T lexLongFilePathName( STRM_T t, TOKEN_T tok )
/**********************************************************
 * This will enable taking in of special filenames
 * it takes long file names or long path names
 */
{
    char    file[_MAX_PATH];
    int     pos;

    assert( t == DOUBLEQUOTE );

    pos = 0;

    t = PreGetCH();

    /* \" is considered a double quote character                         */
    /* and if a double quote is found again then we break out as the end */
    /* of the filename                                                   */
    while( pos < _MAX_PATH && t != DOUBLEQUOTE && t != EOL && t != STRM_END ) {
        file[pos++] = t;
        t = PreGetCH();
        if( t == BACKSLASH ) {
            if( pos >= _MAX_PATH ) {
                break;
            }
            t = PreGetCH();
            if( t == DOUBLEQUOTE ) {
                file[pos++] = t;
                t = PreGetCH();
            } else {
                file[pos++] = BACKSLASH;
            }
        }
    }

    if( pos >= _MAX_PATH ) {
        PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 ); // NOTREACHED
    }
    file[pos] = NULLCHAR;

    if( t != DOUBLEQUOTE ) {
        UnGetCH( t );
    }

    CurAttr.ptr = StrDupSafe( file );
    return( tok );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
TOKEN_T LexPath( STRM_T t )
/*********************************
 * returns: ({filec}*";")+          TOK_PATH
 *          EOL                     EOL
 *          STRM_END                STRM_END
 */
{
    char        path[_MAX_PATH];
    char        string_open;
    unsigned    pos;
    VECSTR      vec;                /* we'll store file/path here */

    for( ;; ) {                     /* get first valid character */
        if( t == EOL || t == STRM_END ) {
                /* now we pass this to LexParser() so that it can reset */
            return( LexParser( t ) );
        }

        if( t == STRM_MAGIC ) {
            InsString( DeMacro( EOL ), TRUE );
        } else if( !isfilec( t ) && t != PATH_SPLIT && t != ';' && t != '\"' &&
                !isws( t ) ) {
            PrtMsg( ERR | LOC | EXPECTING_M, M_PATH );
        } else if( !isws( t ) ) {
            break;
        }

        t = PreGetCH(); /* keep fetching characters */
    }
    /* just so you know what we've got now */
    assert( isfilec( t ) || t == PATH_SPLIT || t == ';' || t == '\"' );

    vec = StartVec();

    pos = 0;
    for( ;; ) {
        /*
         * Extract path from stream. If double quote is found, start string mode
         * and ignore all filename character specifiers and just copy all
         * characters. String mode ends with another double quote. Backslash can
         * be used to escape only double quotes, otherwise they are recognized
         * as path seperators.  If we are not in string mode character validity
         * is checked against isfilec().
         */

        string_open = 0;

        while( pos < _MAX_PATH && t != EOL && t != STRM_END ) {
            if( t == BACKSLASH ) {
                t = PreGetCH();

                if( t == DOUBLEQUOTE ) {
                    path[pos++] = DOUBLEQUOTE;
                } else if( t == EOL || t == STRM_END ) {
                    // Handle special case when backslash is placed at end of
                    // line or file
                    path[pos++] = BACKSLASH;
                } else {
                    path[pos++] = BACKSLASH;

                    // make sure we don't cross boundaries
                    if( pos < _MAX_PATH ) {
                        path[pos++] = t;
                    }
                }
            } else {
                if( t == DOUBLEQUOTE ) {
                    string_open = !string_open;
                } else {
                    if( string_open ) {
                        path[pos++] = t;
                    } else if( isfilec( t ) ) {
                        path[pos++] = t;
                    } else {
                        break; // not valid path character, break out.
                    }
                }
            }

            t = PreGetCH();
        }

        if( string_open ) {
            FreeSafe( FinishVec( vec ) );
            PrtMsg( FTL | LOC | ERROR_STRING_OPEN );
        }

        if( pos == _MAX_PATH ) {
            FreeSafe( FinishVec( vec ) );
            PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 ); // NOTREACHED
        }

        path[pos] = NULLCHAR;
        WriteVec( vec, path );

        if( t != PATH_SPLIT && t != ';' ) {
            break;
        }

        pos = 0;
        path[pos++] = PATH_SPLIT;
        t = PreGetCH();        /* use Pre here to allow path;&(nl)path */
    }
    UnGetCH( t );

    CurAttr.ptr = FinishVec( vec );

    return( TOK_PATH );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC TOKEN_T lexFileName( STRM_T t )
/*************************************
 * Now we need two ways of taking file names if the filename needs special
 * characters then use "filename"  this will ignore all the different
 * characters except for the quote which can be specified as \t
 */
{
    char        file[_MAX_PATH];
    unsigned    pos;

    assert( isfilec( t ) || t == DOUBLEQUOTE ||
       ( (Glob.microsoft || Glob.posix) && t == SPECIAL_TMP_DOL_C ) );

    if( t == DOUBLEQUOTE ) {
        return( lexLongFilePathName( t, TOK_FILENAME ) );
    }

    pos = 0;
    while( pos < _MAX_PATH && (isfilec( t ) ||
            ( t == SPECIAL_TMP_DOL_C && (Glob.microsoft || Glob.posix) ) ) ) {
        file[pos++] = t;
        t = PreGetCH();
    }
    if( pos == _MAX_PATH ) {
        PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 ); // NOTREACHED
    }
    file[pos] = NULLCHAR;
    UnGetCH( t );

    /* if it is a file, we have to check last position for a ':', and
     * trim it off if it's there */
    if( pos > 1 && file[pos - 1] == COLON ) {
        file[pos - 1] = NULLCHAR; /* trim a trailing colon */
        UnGetCH( COLON );       /* push back the colon */
        --pos;
    }
    /*
     * try to do the trim twice because if file ends with a double colon
     * it means its a double colon explicit rule
     */
    if( pos > 1 && file[pos - 1] == COLON ) {
        file[pos - 1] = NULLCHAR;   /* trim a trailing colon */
        UnGetCH( COLON );           /* push back the colon */
    }

    CurAttr.ptr = StrDupSafe( file );
    return( TOK_FILENAME );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC BOOLEAN checkDotName( const char *str )
/*********************************************
 * check if str is a special dotname. If it is, set CurAttr.num to the
 * appropriate value, and return TRUE.  If not a special dotname
 * return FALSE.
 */
{
    char        **key;
    char const  *ptr;

    assert( str[0] == DOT );

    ptr = str + 1;
    key = bsearch( &ptr, DotNames, DOT_MAX,
            sizeof( char * ), (int (*)( const void*, const void* ))KWCompare );

    if( key == NULL ) {         /* not a special dot-name */
        return( FALSE );
    }

    CurAttr.num = (const char **)key - DotNames;

    assert( DOT_MIN < CurAttr.num && CurAttr.num < DOT_MAX );

    return( TRUE );
}

STATIC char *getCurlPath( void )
/*******************************
 * get the path between  { and }
 */
{
    TOKEN_T t;
    char    path[_MAX_PATH + 1];
    int     pos;

    pos = 0;

    t = PreGetCH();

    if( t == L_CURL_PAREN ) {
        t = PreGetCH();
        while( t != R_CURL_PAREN && t != EOL && pos < _MAX_PATH ) {
            path[pos++] = t;
            t = PreGetCH();
        }
        path[pos] = NULLCHAR;
        if( t == EOL ) {
            UnGetCH( EOL );
            PrtMsg( ERR | LOC | NON_MATCHING_CURL_PAREN);
        } else if( pos == _MAX_PATH ) {
            PrtMsg( WRN | LOC | PATH_TOO_LONG );
        }
        return( StrDupSafe( path ) );

    } else {
        UnGetCH( t );
        return( NULL );
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC TOKEN_T lexDotName( void )
/********************************
 * Given that the last character was a DOT, input the maximum string
 * possible, and check if it is a TOK_DOTNAME, TOK_SUF, or TOK_SUFSUF
 * Special cases to look for: "."{dirc}; ".."{dirc}; ".."{extc}+;
 * and "."{extc}+"."
 * recognizes:  "."{extc}*              TOK_SUF
 *              "."{extc}*{extc}*       TOK_SUFSUF
 *              "."{dot-name}           TOK_DOTNAME
 *              "."{dirc}               passes to lexFileName()
 *              ".."{dirc}              passes to lexFileName()
 *              "."{extc}*"{"path"}""."{extc}
 */
{
    char        ext[MAX_SUFFIX];
    unsigned    pos;
    TOKEN_T     t;
    TOKEN_T     t2;
    TOKEN_T     ret;

    pos  = 0;

    dep_path  = NULL;
    targ_path = NULL;
    dep_path = getCurlPath();
    t = PreGetCH();
    if( t != DOT ) {
        PrtMsg( ERR | LOC | INVALID_SUFSUF );
        return( t );
    } else {
        ext[pos++] = DOT;
        t = PreGetCH();
    }

    if( isdirc( t ) || t == PATH_SPLIT || t == ';' ) {  // check for "."{dirc}
        UnGetCH( t );
        if( dep_path != NULL ) {
            PrtMsg( ERR | LOC | INVALID_SUFSUF );
        }
        return( lexFileName( DOT ) );
    }

    if( t == DOT ) {        /* check if ".."{extc} or ".."{dirc} */
        t2 = PreGetCH();    /* probe one character */
        UnGetCH( t2 );
        if( isdirc( t2 ) || t2 == PATH_SPLIT || t2 == ';' ) {   // is ".."{dirc}
            UnGetCH( t );
            if( dep_path != NULL ) {
                PrtMsg( ERR | LOC | INVALID_SUFSUF );
            }
            return( lexFileName( DOT ) );
        }
    } else {    /* get string {extc}+ */
        while( pos < MAX_SUFFIX && isextc( t ) && t != L_CURL_PAREN ) {
            ext[pos++] = t;
            t = PreGetCH();
        }
        if( pos == MAX_SUFFIX ) {
            PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, MAX_SUFFIX - 1 );
            return( 0 );
        }
        ext[pos] = NULLCHAR;
    }

    UnGetCH( t );

    targ_path = getCurlPath();

    t = PreGetCH();         /* next char */

    if( t == DOT ) {        /* maybe of form "."{extc}*"."{extc}* */
        ext[pos++] = DOT;
        t = PreGetCH();     /* next char */
        while( pos < MAX_SUFFIX && isextc( t ) ) {
            ext[pos++] = t;
            t = PreGetCH();
        }
        if( pos == MAX_SUFFIX ) {
            PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, MAX_SUFFIX - 1 ); //NOTREACHED
        }
        ext[pos] = NULLCHAR;

        ret = TOK_SUFSUF;
    } else {
        if( targ_path != NULL && dep_path != NULL ) {
            PrtMsg( ERR | LOC | INVALID_SUFSUF );
        }
        ret = TOK_SUF;
    }
    UnGetCH( t );           /* put back what we don't need */

    if( targ_path != NULL && dep_path != NULL && ret == TOK_SUF ) {
        PrtMsg( ERR | LOC | INVALID_SUFSUF );
    } else if( ret == TOK_SUF && checkDotName( ext ) ) {
        return( TOK_DOTNAME );
    }

    CurAttr.ptr = StrDupSafe( ext );
    return( ret );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC TOKEN_T lexCmd( void )
/****************************
 * returns: {ws}+?*"\n"         TOK_CMD
 */
{
    CurAttr.ptr = PartDeMacro( ForceDeMacro() );
    return( TOK_CMD );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC BOOLEAN checkMacro( TOKEN_T t )
/*************************************
 * returns: TRUE    if the line WAS a macro defn
 *          FALSE   if it wasn't
 * recognizes:      {macc}+{ws}*"="{ws}*{defn}*"\n"
 *                  {macc}+{ws}*"+="{ws}*{defn}*"\n"
 * second gets translated from "macro += defn" to "macro=$+$(macro)$- defn"
 */
{
    char        mac[MAX_MAC_NAME];
    unsigned    pos;
    BOOLEAN     ws;

    pos = 0;
    while( pos < MAX_MAC_NAME && ismacc( t ) ) {
        mac[pos++] = t;
        t = PreGetCH();
    }
    if( pos == MAX_MAC_NAME ) {
        PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, MAX_MAC_NAME - 1 );
        return( 0 );
    }
    mac[pos] = NULLCHAR;
    ws = isws( t );
    while( isws( t ) ) {
        t = PreGetCH();
    }
    if( t == '=' ) {
        DefMacro( mac );
        return( TRUE );          /* go around again */
    } else if( t == '+' ) {
        t = PreGetCH();
        if( t == '=' ) {
            InsString( ")$-", FALSE );
            InsString( mac, FALSE );
            InsString( "$+$(", FALSE );
            DefMacro( mac );
            return( TRUE );     /* go around again */
        }
        UnGetCH( t );
        t = '+';
    }


    UnGetCH( t );           /* not a macro line, put everything back*/
    if( ws ) {
        UnGetCH( SPACE );
    }
    InsString( StrDupSafe( mac + 1 ), TRUE );
    return( FALSE );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC char *DeMacroDoubleQuote( BOOLEAN IsDoubleQuote )
/*******************************************************
 * This procedure takes care of double quotes in the stream
 * Note: each double quote must be paired with a double quote in the
 * input stream or this will expand until the EOL a backlash double
 * quote will be considered as a non-double quote.
 */
{
    char    buffer[_MAX_PATH];
    char    *current;
    char    *p;
    VECSTR  OutString;
    int     pos;
    TOKEN_T t;
    BOOLEAN StartDoubleQuote;


    t = PreGetCH();
    UnGetCH( t );
    if( t == EOL || t == STRM_END || t == STRM_MAGIC ) {
        return( StrDupSafe( "" ) );
    }
    if( t == TMP_LEX_START ) {
        PreGetCH();  /* Eat TMP_LEX_START */
        pos = 0;
        t = PreGetCH();
        while( t != STRM_MAGIC && pos < _MAX_PATH ) {
            assert( t!= EOL || t != STRM_END );
            buffer[pos++] = t;
            t = PreGetCH();
        }

        if( pos >= _MAX_PATH ) {
            PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 ); // NOTREACHED
        }

        buffer[pos] = NULLCHAR;
        p = StrDupSafe( buffer );
    } else {
        p = DeMacro( MAC_WS );
    }

    StartDoubleQuote = IsDoubleQuote;

    current = p;
    while( *current != NULLCHAR ) {
        /* Found the start of a Double Quoted String */
        if( *current == DOUBLEQUOTE && !IsDoubleQuote ) {
            if( current != p ) {
                UnGetCH( STRM_MAGIC );
                InsString( StrDupSafe( current ), TRUE );
                UnGetCH( TMP_LEX_START );
                *current = NULLCHAR;
                return( p );
            }
            IsDoubleQuote = TRUE;
            ++current;
            continue;
        }
        /* Found the end of the Double Quoted String */
        if( *current == DOUBLEQUOTE && IsDoubleQuote ) {
            if( *(current + 1) != NULLCHAR) {
                UnGetCH( STRM_MAGIC );
                InsString( StrDupSafe( current + 1 ), TRUE );
                UnGetCH( TMP_LEX_START );
                *(current + 1) = NULLCHAR;
                return( p );
            } else {
                return( p );
            }
        }
        ++current;
    }

    if( !StartDoubleQuote && !IsDoubleQuote ) {
        /* there are no double quotes in the text */
        /* so return text as is */
        return( p );

    }
    pos = 0;
    t = PreGetCH();
    while( isws( t ) ) {
        buffer[pos++] = t;
        t = PreGetCH();
    }
    buffer[pos] = NULLCHAR;
    UnGetCH( t );
    OutString = StartVec();
    CatStrToVec( OutString, p );
    FreeSafe( p );
    CatStrToVec( OutString, buffer );
    p = DeMacroDoubleQuote( TRUE );
    CatStrToVec( OutString, p );
    FreeSafe( p );
    return( FinishVec( OutString ) );
}


TOKEN_T LexParser( TOKEN_T t )
/************************************
 * returns: next token for parser
 * remarks: possibly defines a macro
 */
{
    static BOOLEAN  atstart = TRUE;
    char            *p;

    for( ;; ) {

        if( atstart ) {
                /* atstart == TRUE if either of these succeed */
            if( isws( t ) ) {           /* cmd line */
                return( lexCmd() );
            }
            if( ismacc( t ) && checkMacro( t ) ) {  /* check if macro = body */
                t = PreGetCH();
                continue;
            }

            atstart = FALSE;
            UnGetCH( t );           /* put back our probe */
            t = STRM_MAGIC;         /* force macro expansion */
        }

        switch( t ) {
        case STRM_END:
        case EOL:
            atstart = TRUE;
            return( t );
        case TMP_LEX_START:
        case STRM_MAGIC:
            p = DeMacroDoubleQuote( FALSE );  /* expand to next white space */
            if( *p == NULLCHAR ) {  /* already at ws */
                FreeSafe( p );
                t = PreGetCH();     /* eat the ws */
                while( isws( t ) ) {
                    t = PreGetCH();
                }
                if( t == EOL || t == STRM_END ) {
                    atstart = TRUE;
                    return( t );
                }
                UnGetCH( t );
                p = DeMacroDoubleQuote( FALSE );
            }
            UnGetCH( STRM_MAGIC );  /* mark spot we have to expand from nxt */
            InsString( p, TRUE );   /* put expansion in stream */
            break;
        case SPACE: /* fall through */
        case TAB:
            break;
        case L_CURL_PAREN:          /* could only be a sufsuf */
        case DOT:
            UnGetCH( t );
            return( lexDotName() ); /* could be a file... */
        case SEMI:                  /* treat semi-colon as {nl}{ws} */
            InsString( "\n ", FALSE );
            break;                  /* try again */
        case COLON:
            t = PreGetCH();
            if( t == COLON ) {
                return( TOK_DCOLON );
            }
            UnGetCH( t );
            return( TOK_SCOLON );
        default:
            if( isfilec( t ) || t == DOUBLEQUOTE ||
                ( (Glob.microsoft || Glob.posix) &&  t == SPECIAL_TMP_DOL_C ) ) {
                return( lexFileName( t ) );
            }
            PrtMsg( WRN | LOC | UNKNOWN_TOKEN, t );
            break;
        }
        t = PreGetCH();             /* fetch a character */
    }
}
