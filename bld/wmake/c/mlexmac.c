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
#include <string.h>

#include "make.h"
#include "massert.h"
#include "macros.h"
#include "mmemory.h"
#include "mlex.h"
#include "mpreproc.h"
#include "mupdate.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mtypes.h"
#include "mvecstr.h"


STATIC TOKEN_T lexFormQualifier( TOKEN_T tok )
/*********************************************
 * pre:     $<file_specifier> read already; passed in tok
 * post:    1 character of input may be read
 * returns: tok; CurAttr.num contains enum FormQualifiers
 * errors:  If next character of input is not a form-qual. it is pushed back
 *          and CurAttr.num == FORM_FULL
 */
{
    STRM_T  t;

    t = PreGetCH();

    switch( t ) {
        case '@':   CurAttr.num = FORM_FULL;            break;
        case '*':   CurAttr.num = FORM_NOEXT;           break;
        case '&':   CurAttr.num = FORM_NOEXT_NOPATH;    break;
        case '.':   CurAttr.num = FORM_NOPATH;          break;
        case ':':   CurAttr.num = FORM_PATH;            break;
        case '!':   CurAttr.num = FORM_EXT;             break;
        default:
            PrtMsg( ERR | LOC | EXPECTING_M, M_FORM_QUALIFIER );
            UnGetCH( t );               /* put character back */
            CurAttr.num = FORM_FULL;    /* assume full name */
    }

    return( tok );
}


void GetModifier( void )
/*****************************/
{
    STRM_T  t;

    // Modifier already eaten by CatModifier
    if( !IsPartDeMacro ) {
        t = PreGetCH();
        switch( t ) {
        case 'D':
        case 'd':
             CurAttr.num = FORM_PATH;
             break;
        case 'B':
        case 'b':
             CurAttr.num = FORM_NOEXT_NOPATH;
             break;
        case 'F':
        case 'f':
             CurAttr.num = FORM_NOPATH;
             break;
        case 'R':
        case 'r':
             CurAttr.num = FORM_NOEXT;
             break;
        default:
             UnGetCH( t );
        }
    }
}

STATIC char *CatModifier( char *inString, BOOLEAN destroy )
/**********************************************************
 *  Get the modifier
 *  if it is modify the value inInString to the specs of the modifier
 *  it then returns the modified string with the right format
 */
{
    STRM_T  t;
    VECSTR  output;
    char    buffer[2];
    char    *ret;

    assert( inString != NULL );

    t = PreGetCH();

    if( ismsmodifier( t ) ) {
        buffer[0] = t;
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
        UnGetCH( t );
        ret = StrDupSafe( inString );
        if( destroy ) {
            FreeSafe( inString );
        }
        return( ret );
    }
}

TOKEN_T LexMSDollar( STRM_T t )
/*************************************
 *  If it is PartDeMacro then we have to put back the tokens as if it were a
 *  MAC_NAME so that it can be DeMacroed fully later
 *  This part only works since microsoft does not handle recursions
 */
{
    char    temp[8];

    assert( ismsspecial( t ) );

    if( IsPartDeMacro || !DoingUpdate ) {
        /* we need to use SPECIAL_TMP_DOL_C to prevent recursion
           from kicking in because recursion occurs when there are
           still dollars remaining */
        temp[0] = SPECIAL_TMP_DOL_C;
        temp[1] = t;
        if( t == '*' ) {
            t  = PreGetCH();
            if( t == '*' ) {
                temp[2] = t;
                temp[3] = NULLCHAR;
            } else {
                UnGetCH( t );
                temp[2] = NULLCHAR;
            }
        } else {
            temp[2] = NULLCHAR;
        }

        CurAttr.ptr = CatModifier( temp, FALSE );
        return( MAC_NAME );

    } else {
        switch( t ) {
        case '<':
            CurAttr.num = FORM_FULL;
            return( MAC_INF_DEP );
        case '*':
            t = PreGetCH();
            if( t == '*' ) {
                CurAttr.num = FORM_FULL;
                return( MAC_ALL_DEP );
            } else {
                CurAttr.num = FORM_NOEXT;
                UnGetCH( t );
                return( MAC_CUR );
            }
        case '?':
            CurAttr.num = FORM_FULL;
            return( MAC_YOUNG_DEP );
        case '@':
            CurAttr.num = FORM_FULL;
            return( MAC_CUR );

        default:
            UnGetCH( t );
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
    STRM_T  t;

    t = PreGetCH();

    if( (Glob.microsoft || Glob.posix) && ismsspecial( t ) ) {
        t = LexMSDollar( t );
        GetModifier();
        return( t );
    }
    switch( t ) {
    case DOLLAR:                        return( MAC_DOLLAR );
    case COMMENT:                       return( MAC_COMMENT );
    case '(':                           return( MAC_OPEN );
    case '+':                           return( MAC_EXPAND_ON );
    case '-':                           return( MAC_EXPAND_OFF );
    case '^':                           return( lexFormQualifier( MAC_CUR ) );
    case '[':                           return( lexFormQualifier( MAC_FIRST ) );
    case ']':                           return( lexFormQualifier( MAC_LAST ) );
    case '@': CurAttr.num = FORM_FULL;  return( MAC_CUR );       /* UNIX */
    case '*': CurAttr.num = FORM_NOEXT; return( MAC_CUR );       /* UNIX */
    case '<': CurAttr.num = FORM_FULL;  return( MAC_ALL_DEP );   /* UNIX */
    case '?': CurAttr.num = FORM_FULL;  return( MAC_YOUNG_DEP ); /* UNIX */
    default:
        UnGetCH( t );
        return( MAC_START );
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
STATIC TOKEN_T lexSubString( TOKEN_T t )
/**************************************/
{
    char        text[MAX_TOK_SIZE];     /* temporary storage                */
    unsigned    pos;                    /* position in text                 */
    TOKEN_T     state;                  /* what are we collecting           */
    BOOLEAN     done;                   /* are we done collecting ?         */
    VECSTR      vec;                    /* build string here                */

    assert( isascii( t ) );

    vec = StartVec();

    if( ismacc( t ) ) {
        state = MAC_NAME;
    } else if( isws( t ) ) {
        state = MAC_WS;
    } else {
        state = MAC_PUNC;
    }

    pos = 0;
    done = FALSE;
    while( !done ) {
        text[pos++] = t;
        if( pos == MAX_TOK_SIZE - 1 ) {
            text[pos] = NULLCHAR;
            WriteVec( vec, text );
            pos = 0;
        }

        t = PreGetCH();
        switch( t ) {
        case EOL:               /* always stop on these characters */
        case STRM_END:
        case STRM_MAGIC:
        case ')':
        case DOLLAR:
            done = TRUE;
            break;
        default:
            switch( state ) {
            case MAC_NAME:  done = !ismacc( t );                break;
            case MAC_WS:    done = !isws( t );                  break;
            case MAC_PUNC:  done = ismacc( t ) || isws( t );    break;
            }
        }
    }
    UnGetCH( t );
    text[pos] = NULLCHAR;
    WriteVec( vec, text );

    CurAttr.ptr = FinishVec( vec );
    return( state );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


TOKEN_T LexMacSubst( TOKEN_T t )
/**************************************
 * returns: next macro substitution type token of input
 */
{
    switch( t ) {
    case SPECIAL_TMP_DOL_C:
    case DOLLAR:
        return( lexDollar() );
    case ')':
        return( MAC_CLOSE );
    case EOL:
    case STRM_END:
    case STRM_MAGIC:
        return( t );
    default:
        return( lexSubString( t ) );
    }
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
TOKEN_T LexMacDef( TOKEN_T t )
/************************************
 * returns: MAC_TEXT, or MAC_WS up to EOL or $+
 */
{
    char    *cur;
    BOOLEAN onlyws;                 /* are we collecting ws?    */
    char    text[MAX_TOK_SIZE];     /* store stuff here temp.   */

    if( t == STRM_END || t == STRM_MAGIC ) {
        return( t );
    }

    assert( isascii( t ) );

    cur = text;

    if( t == DOLLAR ) {
        *cur++ = DOLLAR;
        t = PreGetCH();
        if( t == '+' ) {
            return( MAC_EXPAND_ON );
        }
    } else if( t == EOL ) {
        return( EOL );
    }

    onlyws = isws( t );

    while( cur - text < MAX_TOK_SIZE - 1 ) {
        *cur++ = t;

        t = PreGetCH();

        if(    t == STRM_END
            || t == STRM_MAGIC
            || t == EOL
            || t == DOLLAR
            || (onlyws && !isws( t ))
            || (!onlyws && isws( t )) ) {
            break;
        }
    }

    UnGetCH( t );

    *cur = NULLCHAR;
    CurAttr.ptr = StrDupSafe( text );

    if( onlyws ) {
        return( MAC_WS );
    }
    return( MAC_TEXT );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif
