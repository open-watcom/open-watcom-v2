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
#include "rcmem.h"
#include "ytabw.gh"
#include "semantic.h"
#include "tmpctl.h"

static WResLangType     curLang;
static int              resourceHasLang;
static WResLangType     resourceLang;

/* used in the work around for MS format RES files */
static int  MSFormatHandle;     /* holding place for the RES file handle */
static char MSFormatTmpFile[ _MAX_PATH ] = "";

/* Modified from WINNT.H */
#ifndef MAKELANGID
#define MAKELANGID(p, s)       ((((uint_16)(s)) << 10) | (uint_16)(p))
#endif

void SetDefLang( void ) {
/*************************/
    curLang.lang = DEF_LANG;
    curLang.sublang = DEF_SUBLANG;
}

void SemSetGlobalLanguage( WResLangType *newlang ) {
/*******************************************************************/
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        curLang = *newlang;
    } else {
        RcWarning( ERR_NT_KEYWORD, SemTokenToString( Y_LANGUAGE ) );
    }
}

void SemSetResourceLanguage( WResLangType *newlang, int from_parser ) {
/**********************************************************************/
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
         resourceHasLang = TRUE;
         resourceLang = *newlang;
    } else if( from_parser ) {
        RcWarning( ERR_NT_KEYWORD,  SemTokenToString( Y_LANGUAGE ) );
    }
}

WResLangType GetResourceLanguage( void ) {
/******************************************/

    if( resourceHasLang ) {
        return( resourceLang );
    } else {
        return( curLang );
    }
}

void ClearResourceLanguage( void ) {
/***********************************/
    resourceHasLang = FALSE;
}

void SemUnsupported( uint_8 token ) {
/***********************************/

    RcWarning( ERR_UNSUPPORTED, SemTokenToString( token ) );
}

SemOffset SemStartResource( void )
/********************************/
{
    if( StopInvoked ) {
        RcFatalError( ERR_STOP_REQUESTED );
    }
    if (CurrResFile.IsWatcomRes) {
        return( ResTell( CurrResFile.handle ) );
    } else {
        /* open a temporary file and trade handles with the RES file */
        RcTmpFileName( MSFormatTmpFile );
        MSFormatHandle = CurrResFile.handle;
        CurrResFile.handle = MResOpenNewFile( MSFormatTmpFile );
        if (CurrResFile.handle == -1) {
            CurrResFile.handle = MSFormatHandle;
            ResCloseFile( CurrResFile.handle );
            remove( CurrResFile.filename );
            RcFatalError( ERR_OPENING_TMP, MSFormatTmpFile, LastWresErrStr() );
        } else {
            RegisterTmpFile( MSFormatTmpFile );
            CurrResFile.filename = MSFormatTmpFile;
        }
        /* The start position should be 0 but to be safe call ResTell */
        return( ResTell( CurrResFile.handle ) );
    }
}

SemLength SemEndResource( SemOffset start )
/*****************************************/
{
    SemLength   len;

    if (CurrResFile.IsWatcomRes) {
        return( ResTell( CurrResFile.handle ) - start );
    } else {
        /* Close the temperary file, reset the RES file handle and return */
        /* the length of the resource */
        len = ResTell( CurrResFile.handle ) - start;

        if( ResCloseFile( CurrResFile.handle ) == -1 ) {
            RcError( ERR_CLOSING_TMP, CurrResFile.filename, LastWresErrStr() );
            ErrorHasOccured = TRUE;
        }
        CurrResFile.handle = MSFormatHandle;
        CurrResFile.filename = CurrResFile.namebuf;

        return( len );
    }
}

void SemAddResourceFree( WResID * name, WResID * type, ResMemFlags flags,
                ResLocation loc )
/***********************************************************************/
{
    SemAddResource2( name, type, flags, loc, NULL );
    RcMemFree( name );
    RcMemFree( type );
}

static void copyMSFormatRes( WResID * name, WResID * type, ResMemFlags flags,
                ResLocation loc, WResLangType *lang )
/***************************************************************************/
{
    MResResourceHeader  ms_head;
    long                cur_byte_num;
    uint_8              cur_byte;
    long                seek_rc;
    int                 error;
    int                 tmp_handle;

    /* fill in and output a MS format resource header */
    ms_head.Type = WResIDToNameOrOrd( type );
    ms_head.Name = WResIDToNameOrOrd( name );
    ms_head.MemoryFlags = flags;
    ms_head.Size = loc.len;
    ms_head.LanguageId = MAKELANGID( lang->lang, lang->sublang );
    ms_head.Version = 0L; /* Currently Unsupported */
    ms_head.DataVersion = 0L;
    ms_head.Characteristics = 0L; /* Currently Unsupported */

    /* OS/2 resource header happens to be identical to Win16 */
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ||
        CmdLineParms.TargetOS == RC_TARGET_OS_OS2 ) {
        error = MResWriteResourceHeader( &ms_head, CurrResFile.handle, FALSE );
    } else {
        error = MResWriteResourceHeader( &ms_head, CurrResFile.handle, TRUE );
    }
    if (error) {
        RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename,
                 LastWresErrStr() );
        RcMemFree( ms_head.Type );
        RcMemFree( ms_head.Name );
        ErrorHasOccured = TRUE;
    } else {
        RcMemFree( ms_head.Type );
        RcMemFree( ms_head.Name );
        tmp_handle = ResOpenFileRO( MSFormatTmpFile );
        if (tmp_handle == -1) {
            RcError( ERR_OPENING_TMP, MSFormatTmpFile, LastWresErrStr() );
            ErrorHasOccured = TRUE;
            return;
        }

        /* copy the data from the temperary file to the RES file */
        seek_rc = ResSeek( tmp_handle, loc.start, SEEK_SET );
        if (seek_rc == -1) {
            RcError( ERR_READING_TMP, MSFormatTmpFile, LastWresErrStr() );
            ResCloseFile( tmp_handle );
            ErrorHasOccured = TRUE;
            return;
        }

        /* this is very inefficient but hopefully the buffering in layer0.c */
        /* will make it tolerable */
        for (cur_byte_num = 0; cur_byte_num < loc.len; cur_byte_num++) {
            error = ResReadUint8( &cur_byte, tmp_handle );
            if( error ) {
                RcError( ERR_READING_TMP, MSFormatTmpFile, LastWresErrStr() );
                ResCloseFile( tmp_handle );
                ErrorHasOccured = TRUE;
                return;
            } else {
                error = ResWriteUint8( &cur_byte, CurrResFile.handle );
                if( error ) {
                    RcError( ERR_WRITTING_RES_FILE,
                             CurrResFile.filename, LastWresErrStr() );
                    ResCloseFile( tmp_handle );
                    ErrorHasOccured = TRUE;
                    return;
                }
            }
        }
        if( ResCloseFile( tmp_handle ) == -1 ) {
            RcError( ERR_WRITTING_RES_FILE, MSFormatTmpFile,
                     LastWresErrStr() );
            ErrorHasOccured = TRUE;
        }
    }
}

void SemAddResource( WResID * name, WResID * type, ResMemFlags flags,
                ResLocation loc )
/**********************************************************************/
{
    SemAddResource2( name, type, flags, loc, NULL );
}

void SemAddResource2( WResID * name, WResID * type, ResMemFlags flags,
                ResLocation loc, char *filename )
/******************************************************************/
{
    int                 error;
    int                 duplicate;
    char *              namestr;
    WResLangType        *lang;

    if( resourceHasLang ) {
        lang = &resourceLang;
        resourceHasLang = FALSE;
    } else {
        lang = &curLang;
    }
    // Windows 95 is currently unable to load an exe that contains a resource
    // with numeric type or numeric identifier greater than 0x7FFF
    // so we warn the user
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        if( !type->IsName && type->ID.Num > 0x7FFF ) {
            namestr = WResIDToStr( name );
            RcWarning( ERR_TYPE_GT_7FFF, namestr );
            RcMemFree( namestr );
        }
        if( !name->IsName && name->ID.Num > 0x7FFF ) {
            namestr = WResIDToStr( name );
            RcWarning( ERR_NAME_GT_7FFF, namestr );
            RcMemFree( namestr );
        }
    }
    error = WResAddResource( type, name, flags, loc.start, loc.len,
                        CurrResFile.dir, lang, &duplicate );

    if (duplicate) {
        if( filename == NULL ) {
            ReportDupResource( name, type, NULL, NULL, TRUE );
        } else {
            ReportDupResource( name, type, filename, CmdLineParms.InFileName,
                               TRUE );
        }
        /* The resource has already been written but we can't add it to */
        /* directory. This will make the .RES file larger but will otherwise */
        /* not affect it since there will be no references to the resource in */
        /* the directory. */
    } else if (error) {
        RcError( ERR_OUT_OF_MEMORY );
        ErrorHasOccured = TRUE;
    }

    if (!CurrResFile.IsWatcomRes) {
        if (!duplicate) {
            copyMSFormatRes( name, type, flags, loc, lang );
        }
        /* erase the temporary file */
        remove( MSFormatTmpFile );
        UnregisterTmpFile( MSFormatTmpFile );
        MSFormatTmpFile[0] = '\0';
    }
}

FullMemFlags SemAddFirstMemOption( uint_8 token )
/***********************************************/
{
    FullMemFlags    newflags;

    newflags.flags = 0;
    newflags.loadOptGiven = FALSE;
    newflags.memOptGiven = FALSE;
    newflags.purityOptGiven = FALSE;

    return( SemAddMemOption( newflags, token ) );
}

FullMemFlags SemAddMemOption( FullMemFlags currflags, uint_8 token )
/******************************************************************/
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

void SemCheckMemFlags( FullMemFlags * currflags, ResMemFlags loadopts,
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

extern char *SemTokenToString( uint_8 token )
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

extern void SemanticInitStatics( void )
/*************************************/
{
    memset( &curLang, 0, sizeof( WResLangType ) );
    resourceHasLang = 0;
    memset( &resourceLang, 0, sizeof( WResLangType ) );
    MSFormatHandle = 0;
}

