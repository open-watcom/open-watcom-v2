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
#include "macros.h"
#include "mmemory.h"
#include "mlex.h"
#include "mpreproc.h"
#include "mupdate.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mvecstr.h"


STATIC TOKEN_T lexFormQualifier( TOKEN_T tok )
/*********************************************
 * pre:     $<file_specifier> read already; passed in tok
 * post:    1 character of input may be read
 * returns: tok; CurAttr.u.form contains enum FormQualifiers
 * errors:  If next character of input is not a form-qual. it is pushed back
 *          and CurAttr.u.form == FORM_FULL
 */
{
    STRM_T  s;

    s = PreGetCH();

    switch( s ) {
        case '@':   CurAttr.u.form = FORM_FULL;            break;
        case '*':   CurAttr.u.form = FORM_NOEXT;           break;
        case '&':   CurAttr.u.form = FORM_NOEXT_NOPATH;    break;
        case '.':   CurAttr.u.form = FORM_NOPATH;          break;
        case ':':   CurAttr.u.form = FORM_PATH;            break;
        case '!':   CurAttr.u.form = FORM_EXT;             break;
        default:
            PrtMsg( ERR | LOC | EXPECTING_M, M_FORM_QUALIFIER );
            UnGetCH( s );               /* put character back */
            CurAttr.u.form = FORM_FULL;   /* assume full name */
    }

    return( tok );
}


void GetModifier( void )
/*****************************/
{
    STRM_T  s;

    // Modifier already eaten by CatModifier
    if( !IsPartDeMacro ) {
        s = PreGetCH();
        switch( s ) {
        case 'D':
        case 'd':
             CurAttr.u.form = FORM_PATH;
             break;
        case 'B':
        case 'b':
             CurAttr.u.form = FORM_NOEXT_NOPATH;
             break;
        case 'F':
        case 'f':
             CurAttr.u.form = FORM_NOPATH;
             break;
        case 'R':
        case 'r':
             CurAttr.u.form = FORM_NOEXT;
             break;
        default:
             UnGetCH( s );
        }
    }
}

STATIC char *CatModifier( char *inString, bool destroy )
/*******************************************************
 *  Get the modifier
 *  if it is modify the value inInString to the specs of the modifier
 *  it then returns the modified string with the right format
 */
{
    STRM_T  s;
    VECSTR  output;
    char    buffer[2];
    char    *ret;

    assert( inString != NULL );

    s = PreGetCH();

    if( sismsmodifier( s ) ) {
        buffer[0] = s;
        buffer[1] = NULLCHAR;
        output = StartVec();
        WriteVec( output, "" );
        CatStrToVec( output, inString );
        CatStrToVec( output, buffer );
        if( destroy ) {
            FreeSafe( inString );
        }
        return( FinishVec( output ) );
    } else {
        UnGetCH( s );
        ret = StrDupSafe( inString );
        if( destroy ) {
            FreeSafe( inString );
        }
        return( ret );
    }
}

TOKEN_T LexMSDollar( STRM_T s )
/*************************************
 *  If it is PartDeMacro then we have to put back the tokens as if it were a
 *  MAC_NAME so that it can be DeMacroed fully later
 *  This part only works since microsoft does not handle recursions
 */
{
    char    temp[8];

    assert( sismsspecial( s ) );

    if( IsPartDeMacro || !DoingUpdate ) {
        /* we need to use SPECIAL_TMP_DOL_C to prevent recursion
           from kicking in because recursion occurs when there are
           still dollars remaining */
        temp[0] = SPECIAL_TMP_DOL_C;
        temp[1] = s;
        if( s == '*' ) {
            s = PreGetCH();
            if( s == '*' ) {
                temp[2] = s;
                temp[3] = NULLCHAR;
            } else {
                UnGetCH( s );
                temp[2] = NULLCHAR;
            }
        } else {
            temp[2] = NULLCHAR;
        }

        CurAttr.u.ptr = CatModifier( temp, false );
        return( MAC_NAME );

    } else {
        switch( s ) {
        case '<':
            CurAttr.u.form = FORM_FULL;
            return( MAC_INF_DEP );
        case '*':
            s = PreGetCH();
            if( s == '*' ) {
                CurAttr.u.form = FORM_FULL;
                return( MAC_ALL_DEP );
            } else {
                CurAttr.u.form = FORM_NOEXT;
                UnGetCH( s );
                return( MAC_CUR );
            }
        case '?':
            CurAttr.u.form = FORM_FULL;
            return( MAC_YOUNG_DEP );
        case '@':
            CurAttr.u.form = FORM_FULL;
            return( MAC_CUR );

        default:
            UnGetCH( s );
            return( MAC_START );
        }
    }
}


STATIC TOKEN_T lexDollar( void )
/*******************************
 * pre:     $ read off input
 * post:    0 or more characters of token read
 * returns: MAC token type
 */
{
    STRM_T  s;
    TOKEN_T t;

    s = PreGetCH();

    if( (Glob.compat_nmake || Glob.compat_posix) && sismsspecial( s ) ) {
        t = LexMSDollar( s );
        GetModifier();
        return( t );
    }
    switch( s ) {
    case DOLLAR:                        return( MAC_DOLLAR );
    case COMMENT:                       return( MAC_COMMENT );
    case '(':                           return( MAC_OPEN );
    case '+':                           return( MAC_EXPAND_ON );
    case '-':                           return( MAC_EXPAND_OFF );
    case '^':                           return( lexFormQualifier( MAC_CUR ) );
    case '[':                           return( lexFormQualifier( MAC_FIRST ) );
    case ']':                           return( lexFormQualifier( MAC_LAST ) );
    case '@': CurAttr.u.form = FORM_FULL;  return( MAC_CUR );       /* UNIX */
    case '*': CurAttr.u.form = FORM_NOEXT; return( MAC_CUR );       /* UNIX */
    case '<': CurAttr.u.form = FORM_FULL;  return( MAC_ALL_DEP );   /* UNIX */
    case '?': CurAttr.u.form = FORM_FULL;  return( MAC_YOUNG_DEP ); /* UNIX */
    default:
        UnGetCH( s );
        return( MAC_START );
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC TOKEN_T lexSubString( STRM_T s )
/**************************************/
{
    char        text[MAX_TOK_SIZE];     /* temporary storage                */
    unsigned    pos;                    /* position in text                 */
    TOKEN_T     state;                  /* what are we collecting           */
    bool        done;                   /* are we done collecting ?         */
    VECSTR      vec;                    /* build string here                */

    assert( sisascii( s ) );

    vec = StartVec();

    if( sismacc( s ) ) {
        state = MAC_NAME;
    } else if( sisws( s ) ) {
        state = MAC_WS;
    } else {
        state = MAC_PUNC;
    }

    pos = 0;
    done = false;
    while( !done ) {
        text[pos++] = s;
        if( pos == MAX_TOK_SIZE - 1 ) {
            text[pos] = NULLCHAR;
            WriteVec( vec, text );
            pos = 0;
        }

        s = PreGetCH();
        switch( s ) {
        case EOL:               /* always stop on these characters */
        case STRM_END:
        case STRM_MAGIC:
        case ')':
        case DOLLAR:
            done = true;
            break;
        default:
            switch( state ) {
            case MAC_NAME:  done = !sismacc( s );               break;
            case MAC_WS:    done = !sisws( s );                 break;
            case MAC_PUNC:  done = sismacc( s ) || sisws( s );  break;
            }
        }
    }
    UnGetCH( s );
    text[pos] = NULLCHAR;
    WriteVec( vec, text );

    CurAttr.u.ptr = FinishVec( vec );
    return( state );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


TOKEN_T LexMacSubst( STRM_T s )
/**************************************
 * returns: next macro substitution type token of input
 */
{
    switch( s ) {
    case SPECIAL_TMP_DOL_C:
    case DOLLAR:
        return( lexDollar() );
    case ')':
        return( MAC_CLOSE );
    case EOL:
        return( TOK_EOL );
    case STRM_END:
        return( TOK_END );
    case STRM_MAGIC:
        return( TOK_MAGIC );
    default:
        return( lexSubString( s ) );
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
TOKEN_T LexMacDef( STRM_T s )
/************************************
 * returns: MAC_TEXT, or MAC_WS up to EOL or $+
 */
{
    char    *cur;
    bool    onlyws;                 /* are we collecting ws?    */
    char    text[MAX_TOK_SIZE];     /* store stuff here temp.   */

    if( s == STRM_END ) {
        return( TOK_END );
    }
    if( s == STRM_MAGIC ) {
        return( TOK_MAGIC );
    }
    if( s == EOL ) {
        return( TOK_EOL );
    }

    assert( sisascii( s ) );

    cur = text;

    if( s == DOLLAR ) {
        *cur++ = DOLLAR;
        s = PreGetCH();
        if( s == '+' ) {
            return( MAC_EXPAND_ON );
        }
    }

    onlyws = sisws( s );

    while( cur - text < MAX_TOK_SIZE - 1 ) {
        *cur++ = s;

        s = PreGetCH();

        if(    s == STRM_END
            || s == STRM_MAGIC
            || s == EOL
            || s == DOLLAR
            || (onlyws && !sisws( s ))
            || (!onlyws && sisws( s )) ) {
            break;
        }
    }

    UnGetCH( s );

    *cur = NULLCHAR;
    CurAttr.u.ptr = StrDupSafe( text );

    if( onlyws ) {
        return( MAC_WS );
    }
    return( MAC_TEXT );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif
