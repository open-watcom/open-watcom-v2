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


#include "make.h"
#include "mlex.h"
#include "macros.h"
#include "mmemory.h"
#include "mmisc.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mvecstr.h"

#include "clibext.h"


char    *targ_path;
char    *dep_path;

STATIC TOKEN_T lexLongFilePathName( STRM_T s, TOKEN_T tok )
/**********************************************************
 * This will enable taking in of special filenames
 * it takes long file names or long path names
 */
{
    char    file[_MAX_PATH];
    int     pos;

    assert( s == DOUBLEQUOTE );

    pos = 0;

    s = PreGetCH();

    /* \" is considered a double quote character                         */
    /* and if a double quote is found again then we break out as the end */
    /* of the filename                                                   */
    while( pos < _MAX_PATH && s != DOUBLEQUOTE && s != EOL && s != STRM_END ) {
        file[pos++] = s;
        s = PreGetCH();
        if( s == BACKSLASH ) {
            if( pos >= _MAX_PATH ) {
                break;
            }
            s = PreGetCH();
            if( s == DOUBLEQUOTE ) {
                file[pos++] = s;
                s = PreGetCH();
            } else {
                file[pos++] = BACKSLASH;
            }
        }
    }

    if( pos >= _MAX_PATH ) {
        PrtMsgExit(( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 )); // NOTREACHED
    }
    file[pos] = NULLCHAR;

    if( s != DOUBLEQUOTE ) {
        UnGetCH( s );
    }

    CurAttr.u.ptr = StrDupSafe( file );
    return( tok );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
TOKEN_T LexPath( STRM_T s )
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
        if( s == EOL || s == STRM_END ) {
            /* now we pass this to LexParser() so that it can reset */
            return( LexParser( s ) );
        }

        if( s == STRM_MAGIC ) {
            InsString( DeMacro( TOK_EOL ), TRUE );
        } else if( !isfilec( s ) && s != PATH_SPLIT && s != ';' && s != '\"' && !isws( s ) ) {
            PrtMsg( ERR | LOC | EXPECTING_M, M_PATH );
        } else if( !isws( s ) ) {
            break;
        }

        s = PreGetCH(); /* keep fetching characters */
    }
    /* just so you know what we've got now */
    assert( isfilec( s ) || s == PATH_SPLIT || s == ';' || s == '\"' );

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

        while( pos < _MAX_PATH && s != EOL && s != STRM_END ) {
            if( s == BACKSLASH ) {
                s = PreGetCH();

                if( s == DOUBLEQUOTE ) {
                    path[pos++] = DOUBLEQUOTE;
                } else if( s == EOL || s == STRM_END ) {
                    // Handle special case when backslash is placed at end of
                    // line or file
                    path[pos++] = BACKSLASH;
                } else {
                    path[pos++] = BACKSLASH;

                    // make sure we don't cross boundaries
                    if( pos < _MAX_PATH ) {
                        path[pos++] = s;
                    }
                }
            } else {
                if( s == DOUBLEQUOTE ) {
                    string_open = !string_open;
                } else {
                    if( string_open ) {
                        path[pos++] = s;
                    } else if( isfilec( s ) ) {
                        path[pos++] = s;
                    } else {
                        break; // not valid path character, break out.
                    }
                }
            }

            s = PreGetCH();
        }

        if( string_open ) {
            FreeSafe( FinishVec( vec ) );
            PrtMsgExit(( FTL | LOC | ERROR_STRING_OPEN ));
        }

        if( pos == _MAX_PATH ) {
            FreeSafe( FinishVec( vec ) );
            PrtMsgExit(( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 )); // NOTREACHED
        }

        path[pos] = NULLCHAR;
        WriteVec( vec, path );

        if( s != PATH_SPLIT && s != ';' ) {
            break;
        }

        pos = 0;
        path[pos++] = PATH_SPLIT;
        s = PreGetCH();        /* use Pre here to allow path;&(nl)path */
    }
    UnGetCH( s );

    CurAttr.u.ptr = FinishVec( vec );

    return( TOK_PATH );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC TOKEN_T lexFileName( STRM_T s )
/*************************************
 * Now we need two ways of taking file names if the filename needs special
 * characters then use "filename"  this will ignore all the different
 * characters except for the quote which can be specified as \t
 */
{
    char        file[_MAX_PATH];
    unsigned    pos;

    assert( isfilec( s ) || s == DOUBLEQUOTE ||
       ( (Glob.compat_nmake || Glob.compat_posix) && s == SPECIAL_TMP_DOL_C ) );

    if( s == DOUBLEQUOTE ) {
        return( lexLongFilePathName( s, TOK_FILENAME ) );
    }

    pos = 0;
    while( pos < _MAX_PATH && (isfilec( s ) ||
            ( s == SPECIAL_TMP_DOL_C && (Glob.compat_nmake || Glob.compat_posix) ) ) ) {
        file[pos++] = s;
        s = PreGetCH();
    }
    if( pos == _MAX_PATH ) {
        PrtMsgExit(( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 )); // NOTREACHED
    }
    file[pos] = NULLCHAR;
    UnGetCH( s );

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

    CurAttr.u.ptr = StrDupSafe( file );
    return( TOK_FILENAME );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC BOOLEAN checkDotName( const char *str )
/*********************************************
 * check if str is a special dotname. If it is, set CurAttr.u.dotname to the
 * appropriate value, and return TRUE.  If not a special dotname
 * return FALSE.
 */
{
    char        **key;
    char const  *ptr;

    assert( str[0] == DOT );

    ptr = str + 1;
    key = bsearch( &ptr, DotNames, DOT_MAX, sizeof( char * ), KWCompare );

    if( key == NULL ) {         /* not a special dot-name */
        return( FALSE );
    }

    CurAttr.u.dotname = (DotName)( (const char **)key - DotNames );

    assert( DOT_MIN < CurAttr.u.dotname && CurAttr.u.dotname < DOT_MAX );

    return( TRUE );
}

STATIC char *getCurlPath( void )
/*******************************
 * get the path between  { and }
 */
{
    STRM_T  s;
    char    path[_MAX_PATH + 1];
    int     pos;

    pos = 0;

    s = PreGetCH();

    if( s == L_CURL_PAREN ) {
        for( s = PreGetCH(); s != R_CURL_PAREN && s != EOL && pos < _MAX_PATH; s = PreGetCH() ) {
            path[pos++] = s;
        }
        path[pos] = NULLCHAR;
        if( s == EOL ) {
            UnGetCH( EOL );
            PrtMsg( ERR | LOC | NON_MATCHING_CURL_PAREN);
        } else if( pos == _MAX_PATH ) {
            PrtMsg( WRN | LOC | PATH_TOO_LONG );
        }
        return( StrDupSafe( path ) );

    } else {
        UnGetCH( s );
        return( "" );
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
 * recognizes:
 *   "."{extc}*                                 TOK_SUF
 *   "."{extc}*"."{extc}*                       TOK_SUFSUF
 *   "{"path"}""."{extc}*"{"path"}""."{extc}    TOK_SUFSUF
 *   "."{dot-name}                              TOK_DOTNAME
 *   "."{dirc}                                  passes to lexFileName()
 *   ".."{dirc}                                 passes to lexFileName()
 */
{
    char        ext[MAX_SUFFIX];
    unsigned    pos;
    STRM_T      s;
    STRM_T      s2;
    TOKEN_T     ret;

    pos  = 0;

    if( *targ_path != NULLCHAR ) {
        FreeSafe( targ_path );
        targ_path = "";
    }
    if( *dep_path != NULLCHAR ) {
        FreeSafe( dep_path );
        dep_path = "";
    }
    dep_path = getCurlPath();
    s = PreGetCH();
    if( s != DOT ) {
        PrtMsg( ERR | LOC | INVALID_SUFSUF );
        return( TOK_NULL );
    } else {
        ext[pos++] = DOT;
        s = PreGetCH();
    }

    if( isdirc( s ) || s == PATH_SPLIT || s == ';' ) {  // check for "."{dirc}
        UnGetCH( s );
        if( *dep_path != NULLCHAR ) {
            PrtMsg( ERR | LOC | INVALID_SUFSUF );
        }
        return( lexFileName( DOT ) );
    }

    if( s == DOT ) {        /* check if ".."{extc} or ".."{dirc} */
        s2 = PreGetCH();    /* probe one character */
        UnGetCH( s2 );
        if( isdirc( s2 ) || s2 == PATH_SPLIT || s2 == ';' ) {   // is ".."{dirc}
            UnGetCH( s );
            if( *dep_path != NULLCHAR ) {
                PrtMsg( ERR | LOC | INVALID_SUFSUF );
            }
            return( lexFileName( DOT ) );
        }
    } else {    /* get string {extc}+ */
        while( pos < MAX_SUFFIX && isextc( s ) && s != L_CURL_PAREN ) {
            ext[pos++] = s;
            s = PreGetCH();
        }
        if( pos == MAX_SUFFIX ) {
            PrtMsgExit(( FTL | LOC | MAXIMUM_TOKEN_IS, MAX_SUFFIX - 1 ));
        }
        ext[pos] = NULLCHAR;
    }

    UnGetCH( s );

    targ_path = getCurlPath();

    s = PreGetCH();         /* next char */

    if( s == DOT ) {        /* maybe of form "."{extc}*"."{extc}* */
        ext[pos++] = s;
        for( s = PreGetCH(); pos < MAX_SUFFIX && isextc( s ); s = PreGetCH() ) {
            ext[pos++] = s;
        }
        if( pos == MAX_SUFFIX ) {
            PrtMsgExit(( FTL | LOC | MAXIMUM_TOKEN_IS, MAX_SUFFIX - 1 )); //NOTREACHED
        }
        ext[pos] = NULLCHAR;

        ret = TOK_SUFSUF;
    } else {
        if( *targ_path != NULLCHAR && *dep_path != NULLCHAR ) {
            PrtMsg( ERR | LOC | INVALID_SUFSUF );
        }
        ret = TOK_SUF;
    }
    UnGetCH( s );           /* put back what we don't need */

    if( *targ_path != NULLCHAR && *dep_path != NULLCHAR && ret == TOK_SUF ) {
        PrtMsg( ERR | LOC | INVALID_SUFSUF );
    } else if( ret == TOK_SUF && checkDotName( ext ) ) {
        return( TOK_DOTNAME );
    }
    CurAttr.u.ptr = StrDupSafe( ext );
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
    CurAttr.u.ptr = PartDeMacro( ForceDeMacro() );
    return( TOK_CMD );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC BOOLEAN checkMacro( STRM_T s )
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
    while( pos < MAX_MAC_NAME && ismacc( s ) ) {
        mac[pos++] = s;
        s = PreGetCH();
    }
    if( pos == MAX_MAC_NAME ) {
        PrtMsgExit(( FTL | LOC | MAXIMUM_TOKEN_IS, MAX_MAC_NAME - 1 ));
    }
    mac[pos] = NULLCHAR;
    ws = isws( s );
    while( isws( s ) ) {
        s = PreGetCH();
    }
    if( s == '=' ) {
        DefMacro( mac );
        return( TRUE );          /* go around again */
    } else if( s == '+' ) {
        s = PreGetCH();
        if( s == '=' ) {
            InsString( ")$-", FALSE );
            InsString( mac, FALSE );
            InsString( "$+$(", FALSE );
            DefMacro( mac );
            return( TRUE );     /* go around again */
        }
        UnGetCH( s );
        s = '+';
    }


    UnGetCH( s );           /* not a macro line, put everything back*/
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
    STRM_T  s;
    BOOLEAN StartDoubleQuote;


    s = PreGetCH();
    UnGetCH( s );
    if( s == EOL || s == STRM_END || s == STRM_MAGIC ) {
        return( StrDupSafe( "" ) );
    }
    if( s == STRM_TMP_LEX_START ) {
        PreGetCH();  /* Eat STRM_TMP_LEX_START */
        pos = 0;
        for( s = PreGetCH(); s != STRM_MAGIC && pos < _MAX_PATH; s = PreGetCH() ) {
            assert( s != EOL || s != STRM_END );
            buffer[pos++] = s;
        }

        if( pos >= _MAX_PATH ) {
            PrtMsgExit(( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 )); // NOTREACHED
        }

        buffer[pos] = NULLCHAR;
        p = StrDupSafe( buffer );
    } else {
        p = DeMacro( MAC_WS );
    }

    StartDoubleQuote = IsDoubleQuote;

    for( current = p; *current != NULLCHAR; ++current ) {
        if( *current == DOUBLEQUOTE ) {
            if( !IsDoubleQuote ) {
                /* Found the start of a Double Quoted String */
                if( current != p ) {
                    UnGetCH( STRM_MAGIC );
                    InsString( StrDupSafe( current ), TRUE );
                    UnGetCH( STRM_TMP_LEX_START );
                    *current = NULLCHAR;
                    return( p );
                }
                IsDoubleQuote = TRUE;
            } else {
                /* Found the end of the Double Quoted String */
                if( *(current + 1) != NULLCHAR) {
                    UnGetCH( STRM_MAGIC );
                    InsString( StrDupSafe( current + 1 ), TRUE );
                    UnGetCH( STRM_TMP_LEX_START );
                    *(current + 1) = NULLCHAR;
                }
                return( p );
            }
        }
    }

    if( !StartDoubleQuote && !IsDoubleQuote ) {
        /* there are no double quotes in the text */
        /* so return text as is */
        return( p );

    }
    pos = 0;
    s = PreGetCH();
    while( isws( s ) ) {
        buffer[pos++] = s;
        s = PreGetCH();
    }
    buffer[pos] = NULLCHAR;
    UnGetCH( s );
    OutString = StartVec();
    CatStrToVec( OutString, p );
    FreeSafe( p );
    CatStrToVec( OutString, buffer );
    p = DeMacroDoubleQuote( TRUE );
    CatStrToVec( OutString, p );
    FreeSafe( p );
    return( FinishVec( OutString ) );
}


TOKEN_T LexParser( STRM_T s )
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
            if( isws( s ) ) {           /* cmd line */
                return( lexCmd() );
            }
            if( ismacc( s ) && checkMacro( s ) ) {  /* check if macro = body */
                s = PreGetCH();
                continue;
            }

            atstart = FALSE;
            UnGetCH( s );           /* put back our probe */
            s = STRM_MAGIC;         /* force macro expansion */
        }

        switch( s ) {
        case STRM_END:
            atstart = TRUE;
            return( TOK_END );
        case EOL:
            atstart = TRUE;
            return( TOK_EOL );
        case STRM_TMP_LEX_START:
        case STRM_MAGIC:
            p = DeMacroDoubleQuote( FALSE );  /* expand to next white space */
            if( *p == NULLCHAR ) {  /* already at ws */
                FreeSafe( p );
                s = PreGetCH();     /* eat the ws */
                while( isws( s ) ) {
                    s = PreGetCH();
                }
                if( s == EOL ) {
                    atstart = TRUE;
                    return( TOK_EOL );
                }
                if( s == STRM_END ) {
                    atstart = TRUE;
                    return( TOK_END );
                }
                UnGetCH( s );
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
            UnGetCH( s );
            return( lexDotName() ); /* could be a file... */
        case SEMI:                  /* treat semi-colon as {nl}{ws} */
            InsString( "\n ", FALSE );
            break;                  /* try again */
        case COLON:
            s = PreGetCH();
            if( s == COLON ) {
                return( TOK_DCOLON );
            }
            UnGetCH( s );
            return( TOK_SCOLON );
        default:
            if( isfilec( s ) || s == DOUBLEQUOTE ||
                ( (Glob.compat_nmake || Glob.compat_posix) &&  s == SPECIAL_TMP_DOL_C ) ) {
                return( lexFileName( s ) );
            }
            PrtMsg( WRN | LOC | UNKNOWN_TOKEN, s );
            break;
        }
        s = PreGetCH();             /* fetch a character */
    }
}
