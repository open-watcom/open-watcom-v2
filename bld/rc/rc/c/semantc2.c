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
* Description:  Semantic actions called by the YACC generated driver.
*               This is the OS/2 specific portion; semantic.c is needed too.
*
****************************************************************************/


#include "global.h"
#include "errors.h"
#include "rcmem.h"
#include "semantic.h"
#include "semantc2.h"


static uint_32  curCodepage = 850;  // default resource codepage

uint_32 SemOS2DefaultCodepage( void )
/***********************************/
{
    return( curCodepage );
}

void SemOS2SetCodepage( uint_32 codepage )
/****************************************/
{
    curCodepage = codepage;
}

FullOptFlagsOS2 SemOS2AddFirstResOption( YTOKEN token, uint_32 value )
/********************************************************************/
{
    FullOptFlagsOS2     newflags;

    newflags.flags = 0;
    newflags.codePage = curCodepage;
    newflags.loadOptGiven   = FALSE;
    newflags.memOptGiven    = FALSE;
    newflags.purityOptGiven = FALSE;
    newflags.cpOptGiven     = FALSE;

    return( SemOS2AddResOption( newflags, token, value ) );
}

/* IBM's RC has a tendency to add PURE flag when other memory flags
 * are specified. The flag will be ignored by OS but we do the same
 * for compatibility.
 */
FullOptFlagsOS2 SemOS2AddResOption( FullOptFlagsOS2 currflags, YTOKEN token, uint_32 value )
/******************************************************************************************/
{
    switch( token ) {
    case Y_PRELOAD:
        currflags.flags |= MEMFLAG_PRELOAD | MEMFLAG_PURE;
        currflags.loadOptGiven = TRUE;
        break;
    case Y_LOADONCALL:
        currflags.flags &= ~MEMFLAG_PRELOAD;
        currflags.flags |= MEMFLAG_PURE;
        currflags.loadOptGiven = TRUE;
        break;
    case Y_FIXED:
        currflags.flags &= ~MEMFLAG_MOVEABLE;
        currflags.flags |= MEMFLAG_PURE;
        currflags.memOptGiven = TRUE;
        break;
    case Y_MOVEABLE:
        currflags.flags |= MEMFLAG_MOVEABLE | MEMFLAG_PURE;
        currflags.memOptGiven = TRUE;
        break;
    case Y_DISCARDABLE:
        currflags.flags |= MEMFLAG_DISCARDABLE | MEMFLAG_PURE | MEMFLAG_MOVEABLE;
        currflags.memOptGiven = TRUE;
        break;
    case Y_PURE:
        currflags.flags |= MEMFLAG_PURE;
        currflags.purityOptGiven = TRUE;
        break;
    case Y_IMPURE:
        currflags.flags &= ~MEMFLAG_PURE;
        currflags.purityOptGiven = TRUE;
        break;
    case Y_SEGALIGN:    // This one is OS/2 2.x specific
        currflags.flags |= MEMFLAG_SEGALIGN;
        break;
    case Y_INTEGER:    // Is this OS/2 2.x specific too?
        currflags.codePage = value;
        currflags.cpOptGiven = TRUE;
        break;
    }

    return( currflags );
}

void SemOS2CheckResFlags( FullOptFlagsOS2 *currflags, ResMemFlags loadopts,
            ResMemFlags memopts, ResMemFlags pureopts )
/*************************************************************************/
{
    if( !currflags->loadOptGiven ) {
        currflags->flags |= loadopts;
    }
    if( !currflags->memOptGiven ) {
        currflags->flags |= memopts;
    }
    if( !currflags->purityOptGiven ) {
        currflags->flags |= pureopts;
    }

    /* If the user set the resource to be IMPURE but doesn't give a mem option */
    /* set the resource to be non-discardable. */
    if( currflags->purityOptGiven && !currflags->memOptGiven ) {
        if( !(currflags->flags & MEMFLAG_PURE) ) {
            currflags->flags &= ~MEMFLAG_DISCARDABLE;
        }
    }
}

char *SemOS2TokenToString( YTOKEN token )
/***************************************/
{
    switch( token ) {
    case Y_LPAREN:
        return( "(" );
        break;
    case Y_RPAREN:
        return( ")" );
        break;
    case Y_LSQ_BRACKET:
        return( "[" );
        break;
    case Y_RSQ_BRACKET:
        return( "]" );
        break;
    case Y_PLUS:
        return( "+" );
        break;
    case Y_MINUS:
        return( "-" );
        break;
    case Y_BITNOT:
        return( "~" );
        break;
    case Y_NOT:
        return( "!" );
        break;
    case Y_TIMES:
        return( "*" );
        break;
    case Y_DIVIDE:
        return( "/" );
        break;
    case Y_MOD:
        return( "%" );
        break;
    case Y_SHIFTL:
        return( "<<" );
        break;
    case Y_SHIFTR:
        return( ">>" );
        break;
    case Y_GT:
        return( ">" );
        break;
    case Y_LT:
        return( "<" );
        break;
    case Y_GE:
        return( ">=" );
        break;
    case Y_LE:
        return( "<=" );
        break;
    case Y_EQ:
        return( "==" );
        break;
    case Y_NE:
        return( "!=" );
        break;
    case Y_BITAND:
        return( "&" );
        break;
    case Y_BITXOR:
        return( "^" );
        break;
    case Y_BITOR:
        return( "|" );
        break;
    case Y_AND:
        return( "&&" );
        break;
    case Y_OR:
        return( "||" );
        break;
    case Y_QUESTION:
        return( "?" );
        break;
    case Y_COLON:
        return( ":" );
        break;
    case Y_COMMA:
        return( "," );
        break;
    case Y_ACCELTABLE:
        return( "ACCELTABLE" );
        break;
    case Y_ALT:
        return( "ALT" );
        break;
    case Y_AUTOCHECKBOX:
        return( "AUTOCHECKBOX" );
        break;
    case Y_AUTORADIOBUTTON:
        return( "AUTORADIOBUTTON" );
        break;
    case Y_BEGIN:
        return( "BEGIN" );
        break;
    case Y_BITMAP:
        return( "BITMAP" );
        break;
    case Y_CHECKBOX:
        return( "CHECKBOX" );
        break;
    case Y_CODEPAGE:
        return( "CODEPAGE" );
        break;
    case Y_COMBOBOX:
        return( "COMBOBOX" );
        break;
    case Y_CONTAINER:
        return( "CONTAINER" );
        break;
    case Y_CONTROL:
        return( "CONTROL" );
        break;
    case Y_CTEXT:
        return( "CTEXT" );
        break;
    case Y_CTLDATA:
        return( "CTLDATA" );
        break;
    case Y_CURSOR:
        return( "CURSOR" );
        break;
    case Y_DEFPUSHBUTTON:
        return( "DEFPUSHBUTTON" );
        break;
    case Y_DIALOG:
        return( "DIALOG" );
        break;
    case Y_DISCARDABLE:
        return( "DISCARDABLE" );
        break;
    case Y_EDITTEXT:
        return( "EDITTEXT" );
        break;
    case Y_END:
       return( "END" );
        break;
    case Y_ENTRYFIELD:
       return( "ENTRYFIELD" );
        break;
    case Y_FIXED:
        return( "FIXED" );
        break;
    case Y_FONT:
        return( "FONT" );
        break;
    case Y_GROUPBOX:
        return( "GROUPBOX" );
        break;
    case Y_HELP:
        return( "HELP" );
        break;
    case Y_HELPITEM:
        return( "HELPITEM" );
        break;
    case Y_HELPTABLE:
        return( "HELPTABLE" );
        break;
    case Y_HELPSUBITEM:
        return( "HELPSUBITEM" );
        break;
    case Y_HELPSUBTABLE:
        return( "HELPSUBTABLE" );
        break;
    case Y_ICON:
        return( "ICON" );
        break;
    case Y_IMPURE:
        return( "IMPURE" );
        break;
    case Y_LISTBOX:
        return( "LISTBOX" );
        break;
    case Y_LOADONCALL:
        return( "LOADONCALL" );
        break;
    case Y_LONEKEY:
        return( "LONEKEY" );
        break;
    case Y_LTEXT:
        return( "LTEXT" );
        break;
    case Y_MENU:
        return( "MENU" );
        break;
    case Y_MENUITEM:
        return( "MENUITEM" );
        break;
    case Y_MESSAGETABLE:
        return( "MESSAGETABLE" );
        break;
    case Y_MLE:
        return( "MLE" );
        break;
    case Y_MOVEABLE:
        return( "MOVEABLE" );
        break;
    case Y_NOT_KEYWORD:
        return( "NOT" );
        break;
    case Y_NOTEBOOK:
        return( "NOTEBOOK" );
        break;
    case Y_POUND_PRAGMA:
        return( "#pragma" );
        break;
    case Y_PRELOAD:
        return( "PRELOAD" );
        break;
    case Y_PRESPARAMS:
        return( "PRESPARAMS" );
        break;
    case Y_PURE:
        return( "PURE" );
        break;
    case Y_PUSHBUTTON:
        return( "PUSHBUTTON" );
        break;
    case Y_RADIOBUTTON:
        return( "RADIOBUTTON" );
        break;
    case Y_RCDATA:
        return( "RCDATA" );
        break;
    case Y_RCINCLUDE:
        return( "RCINCLUDE" );
        break;
    case Y_RTEXT:
        return( "RTEXT" );
        break;
    case Y_SEPARATOR:
        return( "SEPARATOR" );
        break;
    case Y_SHIFT:
        return( "SHIFT" );
        break;
    case Y_SLIDER:
        return( "SLIDER" );
        break;
    case Y_SPINBUTTON:
        return( "SPINBUTTON" );
        break;
    case Y_STRINGTABLE:
        return( "STRINGTABLE" );
        break;
    case Y_VALUESET:
        return( "VALUESET" );
        break;
    case Y_VIRTUALKEY:
        return( "VIRTUALKEY" );
        break;
    case 0:
        return( "end of file" );
        break;
    default:
        return( "" );
        break;
    }
}

void SemanticInitStaticsOS2( void )
/*********************************/
{
    curCodepage = 850;
}
