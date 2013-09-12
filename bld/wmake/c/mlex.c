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
* Description:  lexical token support functions
*
****************************************************************************/


#include "make.h"
#include "mmemory.h"
#include "mlex.h"
#include "mpreproc.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mstream.h"


union CurAttrUnion  CurAttr;    /* Attribute for last LexToken call */

const char * const DotNames[] = {
    #define pick(text,enum) text,
    #include "mdotname.h"
    #undef pick
};


void LexFini( void )
/*************************/
{
    if( *targ_path != NULLCHAR ) {
        FreeSafe( targ_path );
        targ_path = "";
    }
    if( *dep_path != NULLCHAR ) {
        FreeSafe( dep_path );
        dep_path = "";
    }
    PreProcFini();
}


void LexInit( void )
/*************************/
{
    PreProcInit();
    LexParser( EOL );    /* sync parser to start of line */
    targ_path = "";
    dep_path  = "";
}


TOKEN_T LexToken( enum LexMode mode )
/*******************************************
 * returns: next token of input
 */
{
    STRM_T s;
    TOKEN_T t = TOK_NULL;

    s = PreGetCH();

    switch( mode ) {
    case LEX_MAC_DEF:   t = LexMacDef( s );     break;
    case LEX_MAC_SUBST: t = LexMacSubst( s );   break;
    case LEX_PARSER:    t = LexParser( s );     break;
    case LEX_PATH:      t = LexPath( s );       break;
    case LEX_MS_MAC:    t = LexMSDollar ( s );  break;
    }

    return( t );
}


void LexMaybeFree( TOKEN_T tok )
/**************************************
 * remarks: Some tokens set CurAttr.u.ptr.p1 to a memory region.  This routine
 *          FreeSafes the region if tok is one of these token types.
 */
{
    switch( tok ) {         /* fall through intentional */
    case TOK_SUFSUF:
        FreeSafe( CurAttr.u.ptr.p2 );
        // fall through
    case TOK_FILENAME:
    case TOK_CMD:
    case TOK_SUF:
    case TOK_PATH:
    case MAC_WS:
    case MAC_NAME:
    case MAC_PUNC:
    case MAC_TEXT:
        FreeSafe( CurAttr.u.ptr.p1 );
        break;
    }
}
