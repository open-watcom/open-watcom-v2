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


#include "global.h"
#include "errors.h"
#include "semantic.h"
#include "semantcw.h"
#include "wresdefn.h"
#include "layer0.h"
#include "rcrtns.h"
#include "rccore.h"


/*** Forward References ***/
static uint_16 CalcNestSize( FullVerBlockNest * nest );
static bool SemWriteVerBlockNest( FullVerBlockNest *nest, WResFileID handle, int *err_code );
static void FreeVerBlockNest( FullVerBlockNest * nest );

FullVerValueList * SemWINNewVerValueList( VerValueItem item )
/***********************************************************/
{
    FullVerValueList *  list;

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        item.strlen = VER_CALC_SIZE; // terminate at the first NULLCHAR
    }                                // instead of using the full string.
                                     // This is what Microsoft does.
    list = RCALLOC( sizeof(FullVerValueList) );
    list->NumItems = 1;
    list->Item = RCALLOC( sizeof(VerValueItem) );
    list->Item[0] = item;

    return( list );
}

FullVerValueList * SemWINAddVerValueList( FullVerValueList * list,
                                        VerValueItem item )
/****************************************************************/
{
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        item.strlen = VER_CALC_SIZE; // terminate at the first NULLCHAR
    }                                // instead of using the full string.
                                     // This is what MS does.
    list->NumItems++;
    list->Item = RCREALLOC( list->Item,
                            list->NumItems * sizeof(VerValueItem) );
    list->Item[list->NumItems - 1] = item;

    return( list );
}

static uint_16 CalcValListSize( FullVerValueList *list, bool use_unicode )
/************************************************************************/
{
    uint_16     size;
    int         curr_val;

    size = 0;
    for( curr_val = 0; curr_val < list->NumItems; curr_val++ ) {
        size += ResSizeVerValueItem( list->Item + curr_val, use_unicode );
    }

    return( size );
}

static void FreeValItem( VerValueItem * item )
/********************************************/
{
    if( !item->IsNum ) {
        RCFREE( item->Value.String );
    }
}

static void FreeValList( FullVerValueList * list )
/************************************************/
{
    int     curr_val;

    for( curr_val = 0; curr_val < list->NumItems; curr_val++ ) {
        FreeValItem( list->Item + curr_val );
    }
    RCFREE( list->Item );
    RCFREE( list );
}

static bool semWriteVerValueList( FullVerValueList * list, bool use_unicode,
                                     WResFileID handle, int *err_code )
/***************************************************************************/
{
    bool    error;
    int     curr_val;

    error = false;
    for( curr_val = 0; !error && curr_val < list->NumItems; curr_val++ ) {
        error = ResWriteVerValueItem( list->Item + curr_val, use_unicode,
                                                handle );
    }
    *err_code = LastWresErr();
    return( error );
}


FullVerBlock * SemWINNewBlockVal( char * name, FullVerValueList * list )
/**********************************************************************/
{
    FullVerBlock *  block;

    block = RCALLOC( sizeof(FullVerBlock) );
    block->Next = NULL;
    block->Prev = NULL;
    block->Head.Key = name;
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        block->UseUnicode = true;
    } else {
        block->UseUnicode = false;
    }
    block->Value = list;
    block->Nest = NULL;

    return( block );
}

FullVerBlock * SemWINNameVerBlock( char * name, FullVerBlockNest * nest )
/***********************************************************************/
{
    FullVerBlock *  block;

    block = RCALLOC( sizeof(FullVerBlock) );
    block->Next = NULL;
    block->Prev = NULL;
    block->Head.Key = name;
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        block->UseUnicode = true;
    } else {
        block->UseUnicode = false;
    }
    block->Value = NULL;
    block->Nest = nest;

    return( block );
}

static uint_16 CalcBlockSize( FullVerBlock * block )
/**************************************************/
{
    uint_16     val_size;
    uint_16     padding;
    uint_16     head_size;
    uint_16     nest_size;
    uint_8      os;

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        os = WRES_OS_WIN32;
    } else {
        os = WRES_OS_WIN16;
    }
    head_size = ResSizeVerBlockHeader( &block->Head, block->UseUnicode, os );
    if( block->Value == NULL ) {
        val_size = 0;
        padding = 0;
    } else {
        val_size = CalcValListSize( block->Value, block->UseUnicode );
        padding = RES_PADDING( val_size, sizeof(uint_32) );
    }
    if( block->Nest == NULL ) {
        nest_size = 0;
    } else {
        nest_size = CalcNestSize( block->Nest );
    }

    block->Head.ValSize = val_size;
    if( !stricmp( block->Head.Key, "Translation" ) ) {
        block->Head.Type = 0;
    } else {
        if( block->UseUnicode )
            block->Head.ValSize /= 2;
        block->Head.Type = 1;
    }
    block->Head.Size = head_size + val_size + padding + nest_size;

    return( block->Head.Size );
}

static bool SemWriteVerBlock( FullVerBlock * block, WResFileID handle, int *err_code )
/************************************************************************************/
{
    bool        error;
    uint_8      os;

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        os = WRES_OS_WIN32;
    } else {
        os = WRES_OS_WIN16;
    }
    error = ResWriteVerBlockHeader( &block->Head, block->UseUnicode, os, handle );
    *err_code = LastWresErr();
    if( !error && block->Value != NULL ) {
        error = semWriteVerValueList( block->Value, block->UseUnicode, handle, err_code );
        if( !error ) {
            error = ResPadDWord( handle );
            *err_code = LastWresErr();
        }
    }
    if( !error && block->Nest != NULL ) {
        error = SemWriteVerBlockNest( block->Nest, handle, err_code );
    }

    return( error );
}

static void FreeVerBlock( FullVerBlock * block )
/**********************************************/
{
    RCFREE( block->Head.Key );
    if( block->Value != NULL ) {
        FreeValList( block->Value );
    }
    if( block->Nest != NULL ) {
        FreeVerBlockNest( block->Nest );
    }

    RCFREE( block );
}

FullVerBlockNest * SemWINNewBlockNest( FullVerBlock * child )
/***********************************************************/
{
    FullVerBlockNest *  parent;

    parent = RCALLOC( sizeof(FullVerBlockNest) );
    parent->Head = NULL;
    parent->Tail = NULL;

    return( SemWINAddBlockNest( parent, child ) );
}

FullVerBlockNest * SemWINAddBlockNest( FullVerBlockNest * parent,
                                FullVerBlock * child )
/***************************************************************/
{
    ResAddLLItemAtEnd( (void **) &(parent->Head), (void **) &(parent->Tail), child );
    return( parent );
}

FullVerBlockNest * SemWINMergeBlockNest( FullVerBlockNest * nest1,
                            FullVerBlockNest * nest2 )
/****************************************************************/
{
    FullVerBlock *  block;

    for( block = nest2->Head; block != NULL; block = block->Next ) {
        ResAddLLItemAtEnd( (void **) &nest1->Head, (void **) &nest1->Tail, block );
    }

    RCFREE( nest2 );

    return( nest1 );
}

static uint_16 CalcNestSize( FullVerBlockNest * nest )
/****************************************************/
{
    FullVerBlock *  block;
    uint_16         size;

    size = 0;
    for( block = nest->Head; block != NULL; block = block->Next ) {
        size += CalcBlockSize( block );
    }

    return( size );
}

static void FreeVerBlockNest( FullVerBlockNest * nest )
/*****************************************************/
{
    FullVerBlock *  block;
    FullVerBlock *  old_block;

    block = nest->Head;
    while( block != NULL ) {
        old_block = block;
        block = block->Next;

        FreeVerBlock( old_block );
    }

    RCFREE( nest );
}

static bool SemWriteVerBlockNest( FullVerBlockNest *nest, WResFileID handle, int *err_code )
/******************************************************************************************/
{
    bool            error;
    FullVerBlock    *block;

    error = false;
    for( block = nest->Head; block != NULL && !error; block = block->Next ) {
        error = SemWriteVerBlock( block, handle, err_code );
    }

    return( error );
}


VerFixedInfo * SemWINNewVerFixedInfo( VerFixedOption option )
/***********************************************************/
{
    VerFixedInfo *  info;

    info = RCALLOC( sizeof(VerFixedInfo) );
    memset( info, 0, sizeof(VerFixedInfo) );

    return( SemWINAddVerFixedInfo( info, option ) );
}


#define MakeVersion( verp ) ((uint_32)(verp).LowWord | \
                            ((uint_32)(verp).HighWord << 16 ))

VerFixedInfo * SemWINAddVerFixedInfo( VerFixedInfo * info,
                                        VerFixedOption option )
/*************************************************************/
{
    switch( option.token ) {
    case Y_FILEFLAGS:
        info->FileFlags = option.u.Option;
        break;
    case Y_FILEFLAGSMASK:
        info->FileFlagsMask = option.u.Option;
        break;
    case Y_FILEOS:
        info->FileOS = option.u.Option;
        break;
    case Y_FILESUBTYPE:
        info->FileSubType = option.u.Option;
        break;
    case Y_FILETYPE:
        info->FileType = option.u.Option;
        break;
    case Y_FILEVERSION:
        info->FileVerHigh = MakeVersion( option.u.Version.High );
        info->FileVerLow = MakeVersion( option.u.Version.Low );
        break;
    case Y_PRODUCTVERSION:
        info->ProdVerHigh = MakeVersion( option.u.Version.High );
        info->ProdVerLow = MakeVersion( option.u.Version.Low );
        break;
    }

    return( info );
}

void SemWINWriteVerInfo( WResID * name, ResMemFlags flags,
                        VerFixedInfo * info, FullVerBlockNest * nest )
/********************************************************************/
{
    WResLangType    lang;
    VerBlockHeader  root;
    ResLocation     loc;
    int             padding;
    bool            error;
    bool            use_unicode;
    uint_8          os;
    int             err_code;

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        use_unicode = true;
        os = WRES_OS_WIN32;
    } else {
        use_unicode = false;
        os = WRES_OS_WIN16;
    }
    root.Key = "VS_VERSION_INFO";
    root.ValSize = sizeof(VerFixedInfo);
    root.Type = 0;
    padding = RES_PADDING( root.ValSize, sizeof(uint_32) );
    root.Size = ResSizeVerBlockHeader( &root, use_unicode, os )
                    + root.ValSize + padding + CalcNestSize( nest );
    /* pad the start of the resource so that padding within the resource */
    /* is easier */
    error = ResPadDWord( CurrResFile.handle );
    if( error ) {
        err_code = LastWresErr();
        goto OutputWriteError;
    }

    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();

        error = ResWriteVerBlockHeader( &root, use_unicode, os,
                                        CurrResFile.handle);
        if( error ) {
            err_code = LastWresErr();
            goto OutputWriteError;
        }

        error = ResWriteVerFixedInfo( info, CurrResFile.handle );
        if( error ) {
            err_code = LastWresErr();
            goto OutputWriteError;
        }

        if( ResSeek( CurrResFile.handle, padding, SEEK_CUR ) == -1 )  {
            err_code = LastWresErr();
            goto OutputWriteError;
        }

        error = SemWriteVerBlockNest( nest, CurrResFile.handle, &err_code );
        if( error)
            goto OutputWriteError;

        loc.len = SemEndResource( loc.start );

        /* version info resources must be language neutral */

        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
        SemWINSetResourceLanguage( &lang, false );
        SemAddResourceFree( name, WResIDFromNum( RT_VERSIONINFO ), flags, loc );
    } else {
        RCFREE( name );
    }

    RCFREE( info );
    FreeVerBlockNest( nest );

    return;

OutputWriteError:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code )  );
    ErrorHasOccured = true;
    RCFREE( info );
    FreeVerBlockNest( nest );
    return;
}
