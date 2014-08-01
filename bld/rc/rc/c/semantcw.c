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
*
****************************************************************************/


#include <stdio.h>      /* remove call for MS format stuff */
#include "global.h"
#include "errors.h"
#include "semantic.h"
#include "semantcw.h"
#include "tmpctl.h"
#include "rcrtns.h"
#include "clibext.h"
#include "rccore.h"

static WResLangType curLang;
static int          resourceHasLang;
static WResLangType resourceLang;

void SemSetDefLang( void )
/************************/
{
    curLang.lang = DEF_LANG;
    curLang.sublang = DEF_SUBLANG;
}

void SemWINSetGlobalLanguage( const WResLangType *newlang )
/*********************************************************/
{
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        curLang = *newlang;
    } else {
        RcWarning( ERR_NT_KEYWORD, SemWINTokenToString( Y_LANGUAGE ) );
    }
}

void SemWINSetResourceLanguage( const WResLangType *newlang, int from_parser )
/****************************************************************************/
{
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        resourceHasLang = TRUE;
        resourceLang = *newlang;
    } else if( from_parser ) {
        RcWarning( ERR_NT_KEYWORD,  SemWINTokenToString( Y_LANGUAGE ) );
    }
}

const WResLangType *SemGetResourceLanguage( void )
/************************************************/
{
    if( resourceHasLang ) {
        resourceHasLang = FALSE;
        return( &resourceLang );
    } else {
        return( &curLang );
    }
}

void SemWINUnsupported( YYTOKENTYPE token )
/*****************************************/
{
    RcWarning( ERR_UNSUPPORTED, SemWINTokenToString( token ) );
}

FullMemFlags SemWINAddFirstMemOption( YYTOKENTYPE token )
/*******************************************************/
{
    FullMemFlags    newflags;

    newflags.flags = 0;
    newflags.loadOptGiven = FALSE;
    newflags.memOptGiven = FALSE;
    newflags.purityOptGiven = FALSE;

    return( SemWINAddMemOption( newflags, token ) );
}

FullMemFlags SemWINAddMemOption( FullMemFlags currflags, YYTOKENTYPE token )
/**************************************************************************/
{
    switch (token) {
    case Y_PRELOAD:
        currflags.flags |= MEMFLAG_PRELOAD;
        currflags.loadOptGiven = TRUE;
        break;
    case Y_LOADONCALL:
        currflags.flags &= ~MEMFLAG_PRELOAD;
        currflags.loadOptGiven = TRUE;
        break;
    case Y_FIXED:
        currflags.flags &= ~MEMFLAG_MOVEABLE;
        currflags.memOptGiven = TRUE;
        break;
    case Y_MOVEABLE:
        currflags.flags |= MEMFLAG_MOVEABLE;
        currflags.memOptGiven = TRUE;
        break;
    case Y_DISCARDABLE:
        currflags.flags |= MEMFLAG_DISCARDABLE;
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
    }

    return( currflags );
}

void SemWINCheckMemFlags( FullMemFlags * currflags, ResMemFlags loadopts,
            ResMemFlags memopts, ResMemFlags pureopts )
/********************************************************************/
{
    if (!currflags->loadOptGiven) {
        currflags->flags |= loadopts;
    }
    if (!currflags->memOptGiven) {
        currflags->flags |= memopts;
    }
    if (!currflags->purityOptGiven) {
        currflags->flags |= pureopts;
    }

    /* If the user set the resource to be IMPURE but doesn't give a mem option*/
    /* set the resource to be non-discardable. */
    /* This seems to be what Microsoft is doing (test this with the sample */
    /* program clock). */
    if (currflags->purityOptGiven && !currflags->memOptGiven) {
        if (!(currflags->flags & MEMFLAG_PURE)) {
            currflags->flags &= ~MEMFLAG_DISCARDABLE;
        }
    }
}

char *SemWINTokenToString( YYTOKENTYPE token )
/********************************************/
{
    switch (token) {
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
    case Y_ACCELERATORS:
        return( "accelerators" );
        break;
    case Y_ALT:
        return( "alt" );
        break;
    case Y_ASCII:
        return( "ascii" );
        break;
    case Y_AUTO3STATE:
        return( "auto3state" );
        break;
    case Y_AUTOCHECKBOX:
        return( "autocheckbox" );
        break;
    case Y_AUTORADIOBUTTON:
        return( "autoradiobutton" );
        break;
    case Y_BEGIN:
        return( "begin" );
        break;
    case Y_BITMAP:
        return( "bitmap" );
        break;
    case Y_BLOCK:
        return( "block" );
        break;
    case Y_BUTTON:
        return( "button" );
        break;
    case Y_CAPTION:
        return( "caption" );
        break;
    case Y_CHARACTERISTICS:
        return( "characteristics" );
        break;
    case Y_CHECKBOX:
        return( "checkbox" );
        break;
    case Y_CHECKED:
        return( "checked" );
        break;
    case Y_CLASS:
        return( "class" );
        break;
    case Y_CODEPAGE:
        return( "code_page" );
        break;
    case Y_COMBOBOX:
        return( "combobox" );
        break;
    case Y_CONTROL:
        return( "control" );
        break;
    case Y_CTEXT:
        return( "ctext" );
        break;
    case Y_CURSOR:
        return( "cursor" );
        break;
    case Y_DEFPUSHBUTTON:
        return( "defpushbutton" );
        break;
    case Y_DIALOG:
        return( "dialog" );
        break;
    case Y_DISCARDABLE:
        return( "discardable" );
        break;
    case Y_EDIT:
        return( "edit" );
    case Y_EDITTEXT:
        return( "edittext" );
        break;
    case Y_END:
        return( "end" );
        break;
    case Y_ERRTABLE:
        return( "errtable" );
        break;
    case Y_EXSTYLE:
        return( "exstyle" );
        break;
    case Y_FILEFLAGS:
        return( "fileflags" );
        break;
    case Y_FILEFLAGSMASK:
        return( "fileflagsmask" );
        break;
    case Y_FILEOS:
        return( "fileos" );
        break;
    case Y_FILESUBTYPE:
        return( "filesubtype" );
        break;
    case Y_FILETYPE:
        return( "filetype" );
        break;
    case Y_FILEVERSION:
        return( "fileversion" );
        break;
    case Y_FIXED:
        return( "fixed" );
        break;
    case Y_FONT:
        return( "font" );
        break;
    case Y_GRAYED:
        return( "grayed" );
        break;
    case Y_GROUPBOX:
        return( "groupbox" );
        break;
    case Y_HELP:
        return( "help" );
        break;
    case Y_ICON:
        return( "icon" );
        break;
    case Y_IMPURE:
        return( "impure" );
        break;
    case Y_INACTIVE:
        return( "inactive" );
        break;
    case Y_LANGUAGE:
        return( "language" );
        break;
    case Y_LISTBOX:
        return( "listbox" );
        break;
    case Y_LOADONCALL:
        return( "loadoncall" );
        break;
    case Y_LTEXT:
        return( "ltext" );
        break;
    case Y_MENU:
        return( "menu" );
        break;
    case Y_MENUBARBREAK:
        return( "menubarbreak" );
        break;
    case Y_MENUBREAK:
        return( "menubreak" );
        break;
    case Y_MENUITEM:
        return( "menuitem" );
        break;
    case Y_MESSAGETABLE:
        return( "messagetable" );
        break;
    case Y_MOVEABLE:
        return( "moveable" );
        break;
    case Y_NOINVERT:
        return( "noinvert" );
        break;
    case Y_OWNERDRAW:
        return( "ownerdraw" );
        break;
    case Y_POPUP:
        return( "popup" );
        break;
    case Y_POUND_PRAGMA:
        return( "#pragma" );
        break;
    case Y_PRELOAD:
        return( "preload" );
        break;
    case Y_PRODUCTVERSION:
        return( "productversion" );
        break;
    case Y_PURE:
        return( "pure" );
        break;
    case Y_PUSHBUTTON:
        return( "pushbutton" );
        break;
    case Y_RADIOBUTTON:
        return( "radiobutton" );
        break;
    case Y_RCDATA:
        return( "rcdata" );
        break;
    case Y_RCINCLUDE:
        return( "rcinclude" );
        break;
    case Y_RTEXT:
        return( "rtext" );
        break;
    case Y_SCROLLBAR:
        return( "scrollbar" );
        break;
    case Y_SEPARATOR:
        return( "separator" );
        break;
    case Y_SHIFT:
        return( "shift" );
        break;
    case Y_STATE3:
        return( "state3" );
        break;
    case Y_STATIC:
        return( "static" );
    case Y_STRINGTABLE:
        return( "stringtable" );
        break;
    case Y_STYLE:
        return( "style" );
        break;
    case Y_VALUE:
        return( "value" );
        break;
    case Y_VERSION:
        return( "version" );
        break;
    case Y_VERSIONINFO:
        return( "versioninfo" );
        break;
    case Y_VIRTKEY:
        return( "virtkey" );
        break;
    case 0:
        return( "end of file" );
        break;
    default:
        return( "" );
        break;
    }
}

void SemanticInitStaticsWIN( void )
/*********************************/
{
    memset( &curLang, 0, sizeof( WResLangType ) );
    resourceHasLang = FALSE;
    memset( &resourceLang, 0, sizeof( WResLangType ) );
}
