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

STATIC MTOKEN_T lexLongFilePathName( STRM_T s, MTOKEN_T tok )
/************************************************************
 * This will enable taking in of special filenames
 * it takes long file names or long path names
 */
{
    char    file[_MAX_PATH];
    int     pos;

    assert( s == '\"' );

    pos = 0;

    s = PreGetCHR();

    /* \" is considered a double quote character                         */
    /* and if a double quote is found again then we break out as the end */
    /* of the filename                                                   */
    while( pos < _MAX_PATH && s != '\"' && s != '\n' && s != STRM_END ) {
        file[pos++] = s;
        s = PreGetCHR();
        if( s == '\\' ) {
            if( pos >= _MAX_PATH ) {
                break;
            }
            s = PreGetCHR();
            if( s == '\"' ) {
                file[pos++] = s;
                s = PreGetCHR();
            } else {
                file[pos++] = '\\';
            }
        }
    }

    if( pos >= _MAX_PATH ) {
        PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 ); // NOTREACHED
        ExitFatal();
        // never return
    }
    file[pos] = NULLCHAR;

    if( s != '\"' ) {
        UnGetCHR( s );
    }

    CurAttr.u.ptr = StrDupSafe( file );
    return( tok );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
MTOKEN_T LexPath( STRM_T s )
/*********************************
 * returns: ({filec}*";")+          TOK_PATH
 *          EOL                     TOK_EOL
 *          STRM_END                TOK_END
 */
{
    char        path[_MAX_PATH];
    bool        dquote;
    unsigned    pos;
    VECSTR      vec;                /* we'll store file/path here */

    for( ;; ) {                     /* get first valid character */
        if( s == '\n' || s == STRM_END ) {
            /* now we pass this to LexParser() so that it can reset */
            return( LexParser( s ) );
        }

        if( s == STRM_MAGIC ) {
            InsString( DeMacro( TOK_EOL ), true );
        } else if( !sisfilec( s ) && !IS_PATH_SPLIT( s ) && s != '\"' && !sisws( s ) ) {
            PrtMsg( ERR | LOC | EXPECTING_M, M_PATH );
        } else if( !sisws( s ) ) {
            break;
        }

        s = PreGetCHR(); /* keep fetching characters */
    }
    /* just so you know what we've got now */
    assert( sisfilec( s ) || IS_PATH_SPLIT( s ) || s == '\"' );

    vec = StartVec();

    pos = 0;
    for( ;; ) {
        /*
         * Extract path from stream. If double quote is found, start string mode
         * and ignore all filename character specifiers and just copy all
         * characters. String mode ends with another double quote. Backslash can
         * be used to escape only double quotes, otherwise they are recognized
         * as path seperators.  If we are not in string mode character validity
         * is checked against sisfilec().
         */

        dquote = false;

        while( pos < _MAX_PATH && s != '\n' && s != STRM_END ) {
            if( s == '\\' ) {
                s = PreGetCHR();

                if( s == '\"' ) {
                    path[pos++] = '\"';
                } else if( s == '\n' || s == STRM_END ) {
                    // Handle special case when backslash is placed at end of
                    // line or file
                    path[pos++] = '\\';
                } else {
                    path[pos++] = '\\';

                    // make sure we don't cross boundaries
                    if( pos < _MAX_PATH ) {
                        path[pos++] = s;
                    }
                }
            } else {
                if( s == '\"' ) {
                    dquote = !dquote;
                } else {
                    if( dquote ) {
                        path[pos++] = s;
                    } else if( sisfilec( s ) ) {
                        path[pos++] = s;
                    } else {
                        break; // not valid path character, break out.
                    }
                }
            }

            s = PreGetCHR();
        }

        if( dquote ) {
            FreeSafe( FinishVec( vec ) );
            PrtMsg( FTL | LOC | ERROR_STRING_OPEN );
            ExitFatal();
            // never return
        }

        if( pos == _MAX_PATH ) {
            FreeSafe( FinishVec( vec ) );
            PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 ); // NOTREACHED
            ExitFatal();
            // never return
        }

        path[pos] = NULLCHAR;
        WriteVec( vec, path );

        if( !IS_PATH_SPLIT( s ) ) {
            break;
        }

        pos = 0;
        path[pos++] = PATH_SPLIT;
        s = PreGetCHR();        /* use Pre here to allow path;&(nl)path */
    }
    UnGetCHR( s );

    CurAttr.u.ptr = FinishVec( vec );

    return( TOK_PATH );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC MTOKEN_T lexFileName( STRM_T s )
/**************************************
 * Now we need two ways of taking file names if the filename needs special
 * characters then use "filename"  this will ignore all the different
 * characters except for the quote which can be specified as \t
 */
{
    char        file[_MAX_PATH];
    unsigned    pos;

    assert( sisfilec( s ) || s == '\"' ||
       ( (Glob.compat_nmake || Glob.compat_posix) && s == SPECIAL_TMP_DOLLAR ) );

    if( s == '\"' ) {
        return( lexLongFilePathName( s, TOK_FILENAME ) );
    }

    pos = 0;
    while( pos < _MAX_PATH && (sisfilec( s ) ||
            ( s == SPECIAL_TMP_DOLLAR && (Glob.compat_nmake || Glob.compat_posix) ) ) ) {
        file[pos++] = s;
        s = PreGetCHR();
    }
    if( pos == _MAX_PATH ) {
        PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 ); // NOTREACHED
        ExitFatal();
        // never return
    }
    file[pos] = NULLCHAR;
    UnGetCHR( s );

    /* if it is a file, we have to check last position for a ':', and
     * trim it off if it's there */
    if( pos > 1 && file[pos - 1] == ':' ) {
        file[pos - 1] = NULLCHAR;   /* trim a trailing colon */
        UnGetCHR( ':' );            /* push back the colon */
        --pos;
    }
    /*
     * try to do the trim twice because if file ends with a double colon
     * it means its a double colon explicit rule
     */
    if( pos > 1 && file[pos - 1] == ':' ) {
        file[pos - 1] = NULLCHAR;   /* trim a trailing colon */
        UnGetCHR( ':' );            /* push back the colon */
    }

    CurAttr.u.ptr = StrDupSafe( file );
    return( TOK_FILENAME );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC bool checkDotName( const char *str )
/******************************************
 * check if str is a special dotname. If it is, set CurAttr.u.dotname to the
 * appropriate value, and return true.  If not a special dotname
 * return false.
 */
{
    char        **key;
    char const  *ptr;

    assert( str[0] == '.' );

    ptr = str + 1;
    key = bsearch( &ptr, DotNames, DOT_MAX, sizeof( char * ), KWCompare );

    if( key == NULL ) {         /* not a special dot-name */
        return( false );
    }

    CurAttr.u.dotname = (DotName)( (const char **)key - DotNames );

    assert( DOT_MIN < CurAttr.u.dotname && CurAttr.u.dotname < DOT_MAX );

    return( true );
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

    s = PreGetCHR();

    if( s == '{' ) {
        for( s = PreGetCHR(); s != '}' && s != '\n' && pos < _MAX_PATH; s = PreGetCHR() ) {
            path[pos++] = s;
        }
        path[pos] = NULLCHAR;
        if( s == '\n' ) {
            UnGetCHR( '\n' );
            PrtMsg( ERR | LOC | NON_MATCHING_CURL_PAREN);
        } else if( pos == _MAX_PATH ) {
            PrtMsg( WRN | LOC | PATH_TOO_LONG );
        }
        return( StrDupSafe( path ) );

    } else {
        UnGetCHR( s );
        return( "" );
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC MTOKEN_T lexDotName( void )
/*********************************
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
    MTOKEN_T    ret;

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
    s = PreGetCHR();
    if( s != '.' ) {
        PrtMsg( ERR | LOC | INVALID_SUFSUF );
        return( TOK_NULL );
    } else {
        ext[pos++] = '.';
        s = PreGetCHR();
    }

    if( sisdirc( s ) || IS_PATH_SPLIT( s ) ) {          // check for "."{dirc}
        UnGetCHR( s );
        if( *dep_path != NULLCHAR ) {
            PrtMsg( ERR | LOC | INVALID_SUFSUF );
        }
        return( lexFileName( '.' ) );
    }

    if( s == '.' ) {        /* check if ".."{extc} or ".."{dirc} */
        s2 = PreGetCHR();    /* probe one character */
        UnGetCHR( s2 );
        if( sisdirc( s2 ) || IS_PATH_SPLIT( s2 ) ) {    // is ".."{dirc}
            UnGetCHR( s );
            if( *dep_path != NULLCHAR ) {
                PrtMsg( ERR | LOC | INVALID_SUFSUF );
            }
            return( lexFileName( '.' ) );
        }
    } else {    /* get string {extc}+ */
        while( pos < MAX_SUFFIX && sisextc( s ) && s != '{' ) {
            ext[pos++] = s;
            s = PreGetCHR();
        }
        if( pos == MAX_SUFFIX ) {
            PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, MAX_SUFFIX - 1 );
            ExitFatal();
            // never return
        }
        ext[pos] = NULLCHAR;
    }

    UnGetCHR( s );

    targ_path = getCurlPath();

    s = PreGetCHR();        /* next char */

    if( s == '.' ) {        /* maybe of form "."{extc}*"."{extc}* */
        ext[pos++] = s;
        for( s = PreGetCHR(); pos < MAX_SUFFIX && sisextc( s ); s = PreGetCHR() ) {
            ext[pos++] = s;
        }
        if( pos == MAX_SUFFIX ) {
            PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, MAX_SUFFIX - 1 ); //NOTREACHED
            ExitFatal();
            // never return
        }
        ext[pos] = NULLCHAR;

        ret = TOK_SUFSUF;
    } else {
        if( *targ_path != NULLCHAR && *dep_path != NULLCHAR ) {
            PrtMsg( ERR | LOC | INVALID_SUFSUF );
        }
        ret = TOK_SUF;
    }
    UnGetCHR( s );           /* put back what we don't need */

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


STATIC MTOKEN_T lexCmd( void )
/*****************************
 * returns: {ws}+?*"\n"         TOK_CMD
 */
{
    CurAttr.u.ptr = PartDeMacro( ForceDeMacro() );
    return( TOK_CMD );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC bool checkMacro( STRM_T s )
/*********************************
 * returns: true    if the line WAS a macro defn
 *          false   if it wasn't
 * recognizes:      {macc}+{ws}*"="{ws}*{defn}*"\n"
 *                  {macc}+{ws}*"+="{ws}*{defn}*"\n"
 * second gets translated from "macro += defn" to "macro=$+$(macro)$- defn"
 */
{
    char        mac[MAX_MAC_NAME];
    unsigned    pos;
    bool        ws;

    pos = 0;
    while( pos < MAX_MAC_NAME && sismacc( s ) ) {
        mac[pos++] = s;
        s = PreGetCHR();
    }
    if( pos == MAX_MAC_NAME ) {
        PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, MAX_MAC_NAME - 1 );
        ExitFatal();
        // never return
    }
    mac[pos] = NULLCHAR;
    ws = sisws( s );
    while( sisws( s ) ) {
        s = PreGetCHR();
    }
    if( s == '=' ) {
        DefMacro( mac );
        return( true );          /* go around again */
    } else if( s == '+' ) {
        s = PreGetCHR();
        if( s == '=' ) {
            InsString( ")$-", false );
            InsString( mac, false );
            InsString( "$+$(", false );
            DefMacro( mac );
            return( true );     /* go around again */
        }
        UnGetCHR( s );
        s = '+';
    }

    UnGetCHR( s );           /* not a macro line, put everything back*/
    if( ws ) {
        UnGetCHR( ' ' );
    }
    InsString( StrDupSafe( mac + 1 ), true );
    return( false );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


STATIC char *deMacroDoubleQuote( bool start_dquote )
/***************************************************
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
    bool    dquote;


    s = PreGetCHR();
    UnGetCHR( s );
    if( s == '\n' || s == STRM_END || s == STRM_MAGIC ) {
        return( CharToStrSafe( NULLCHAR ) );
    }
    if( s == STRM_TMP_LEX_START ) {
        PreGetCHR();  /* Eat STRM_TMP_LEX_START */
        pos = 0;
        for( s = PreGetCHR(); s != STRM_MAGIC && pos < _MAX_PATH; s = PreGetCHR() ) {
            assert( s != '\n' || s != STRM_END );
            buffer[pos++] = s;
        }

        if( pos >= _MAX_PATH ) {
            PrtMsg( FTL | LOC | MAXIMUM_TOKEN_IS, _MAX_PATH - 1 ); // NOTREACHED
            ExitFatal();
            // never return
        }

        buffer[pos] = NULLCHAR;
        p = StrDupSafe( buffer );
    } else {
        p = DeMacro( MAC_WS );
    }

    dquote = start_dquote;

    for( current = p; *current != NULLCHAR; ++current ) {
        if( *current == '\"' ) {
            if( !dquote ) {
                /* Found the start of a Double Quoted String */
                if( current != p ) {
                    UnGetCHR( STRM_MAGIC );
                    InsString( StrDupSafe( current ), true );
                    UnGetCHR( STRM_TMP_LEX_START );
                    *current = NULLCHAR;
                    return( p );
                }
                dquote = true;
            } else {
                /* Found the end of the Double Quoted String */
                if( *(current + 1) != NULLCHAR ) {
                    UnGetCHR( STRM_MAGIC );
                    InsString( StrDupSafe( current + 1 ), true );
                    UnGetCHR( STRM_TMP_LEX_START );
                    *(current + 1) = NULLCHAR;
                }
                return( p );
            }
        }
    }

    if( !start_dquote && !dquote ) {
        /* there are no double quotes in the text */
        /* so return text as is */
        return( p );

    }
    pos = 0;
    s = PreGetCHR();
    while( sisws( s ) ) {
        buffer[pos++] = s;
        s = PreGetCHR();
    }
    buffer[pos] = NULLCHAR;
    UnGetCHR( s );
    OutString = StartVec();
    WriteVec( OutString, p );
    FreeSafe( p );
    WriteVec( OutString, buffer );
    p = deMacroDoubleQuote( true );
    WriteVec( OutString, p );
    FreeSafe( p );
    return( FinishVec( OutString ) );
}


MTOKEN_T LexParser( STRM_T s )
/************************************
 * returns: next token for parser
 * remarks: possibly defines a macro
 */
{
    static bool     atstart = true;
    char            *p;

    for( ;; ) {

        if( atstart ) {
                /* atstart == true if either of these succeed */
            if( sisws( s ) ) {           /* cmd line */
                return( lexCmd() );
            }
            if( sismacc( s ) && checkMacro( s ) ) {  /* check if macro = body */
                s = PreGetCHR();
                continue;
            }

            atstart = false;
            UnGetCHR( s );           /* put back our probe */
            s = STRM_MAGIC;         /* force macro expansion */
        }

        switch( s ) {
        case STRM_END:
            atstart = true;
            return( TOK_END );
        case '\n':
            atstart = true;
            return( TOK_EOL );
        case STRM_TMP_LEX_START:
        case STRM_MAGIC:
            p = deMacroDoubleQuote( false );  /* expand to next white space */
            if( *p == NULLCHAR ) {  /* already at ws */
                FreeSafe( p );
                s = PreGetCHR();    /* eat the ws */
                while( sisws( s ) ) {
                    s = PreGetCHR();
                }
                if( s == '\n' ) {
                    atstart = true;
                    return( TOK_EOL );
                }
                if( s == STRM_END ) {
                    atstart = true;
                    return( TOK_END );
                }
                UnGetCHR( s );
                p = deMacroDoubleQuote( false );
            }
            UnGetCHR( STRM_MAGIC ); /* mark spot we have to expand from nxt */
            InsString( p, true );   /* put expansion in stream */
            break;
        case ' ':
        case '\t':
            break;
        case '{':                   /* could only be a sufsuf */
        case '.':
            UnGetCHR( s );
            return( lexDotName() ); /* could be a file... */
        case ';':                   /* treat semi-colon as {nl}{ws} */
            InsString( "\n ", false );
            break;                  /* try again */
        case ':':
            s = PreGetCHR();
            if( s == ':' ) {
                return( TOK_DCOLON );
            }
            UnGetCHR( s );
            return( TOK_SCOLON );
        default:
            if( sisfilec( s ) || s == '\"' || ( (Glob.compat_nmake || Glob.compat_posix) && s == SPECIAL_TMP_DOLLAR ) ) {
                return( lexFileName( s ) );
            }
            PrtMsg( WRN | LOC | UNKNOWN_TOKEN, s );
            break;
        }
        s = PreGetCHR();             /* fetch a character */
    }
}
