/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "rcerrors.h"
#include "semantic.h"
#include "semantcw.h"
#include "wresdefn.h"
#include "layer0.h"
#include "rcrtns.h"
#include "rccore.h"

#include "clibext.h"


/*** Forward References ***/
static uint_16 CalcNestSize( FullVerBlockNest *nest );
static bool SemWriteVerBlockNest( FullVerBlockNest *nest, FILE *fp, int *err_code );
static void FreeVerBlockNest( FullVerBlockNest *nest );

FullVerValueList *SemWINAddVerValueList( FullVerValueList *list, VerValueItem item )
/**********************************************************************************/
{
    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        item.strlen = VER_CALC_SIZE;    // terminate at the first NULLCHAR
    }                                   // instead of using the full string.
                                        // This is what Microsoft does.
    list->NumItems++;
    list->Item = RCREALLOC( list->Item, list->NumItems * sizeof( VerValueItem ) );
    list->Item[list->NumItems - 1] = item;

    return( list );
}

FullVerValueList *SemWINNewVerValueList( VerValueItem item )
/**********************************************************/
{
    FullVerValueList    *list;

    list = RESALLOC( sizeof( FullVerValueList ) );
    list->NumItems = 0;
    list->Item = NULL;

    return( SemWINAddVerValueList( list, item ) );
}

static void FreeValItem( VerValueItem *item )
/*******************************************/
{
    if( !item->IsNum ) {
        RESFREE( item->Value.String );
    }
}

static void FreeValList( FullVerValueList *list )
/***********************************************/
{
    unsigned    i;

    for( i = 0; i < list->NumItems; i++ ) {
        FreeValItem( list->Item + i );
    }
    RESFREE( list->Item );
    RESFREE( list );
}

static bool semWriteVerValueList( FullVerValueList *list, bool use_unicode, FILE *fp, int *err_code )
/***************************************************************************************************/
{
    bool        error;
    unsigned    i;

    error = false;
    for( i = 0; !error && i < list->NumItems; i++ ) {
        error = ResWriteVerValueItem( list->Item + i, use_unicode, fp );
    }
    *err_code = LastWresErr();
    return( error );
}


FullVerBlock *SemWINNewBlockVal( char *name, FullVerValueList *list )
/*******************************************************************/
{
    FullVerBlock    *block;

    block = RESALLOC( sizeof( FullVerBlock ) );
    block->Next = NULL;
    block->Prev = NULL;
    block->Head.Key = name;
    block->UseUnicode = ( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 );
    block->Value = list;
    block->Nest = NULL;

    return( block );
}

FullVerBlock *SemWINNameVerBlock( char *name, FullVerBlockNest *nest )
/********************************************************************/
{
    FullVerBlock    *block;

    block = RESALLOC( sizeof( FullVerBlock ) );
    block->Next = NULL;
    block->Prev = NULL;
    block->Head.Key = name;
    block->UseUnicode = ( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 );
    block->Value = NULL;
    block->Nest = nest;

    return( block );
}

static uint_16 CalcBlockSize( FullVerBlock *block )
/*************************************************/
{
    uint_16         val_size;
    uint_16         padding;
    uint_16         head_size;
    uint_16         nest_size;
    WResTargetOS    res_os;

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        res_os = WRES_OS_WIN32;
    } else {
        res_os = WRES_OS_WIN16;
    }
    head_size = ResSizeVerBlockHeader( &block->Head, block->UseUnicode, res_os );
    val_size = 0;
    if( block->Value == NULL ) {
        padding = 0;
    } else {
        unsigned    i;

        for( i = 0; i < block->Value->NumItems; i++ ) {
            val_size += ResSizeVerValueItem( block->Value->Item + i, block->UseUnicode );
        }
        padding = RES_PADDING_DWORD( val_size );
    }
    if( block->Nest == NULL ) {
        nest_size = 0;
    } else {
        nest_size = CalcNestSize( block->Nest );
    }

    block->Head.ValSize = val_size;
    if( stricmp( block->Head.Key, "Translation" ) == 0 ) {
        block->Head.Type = 0;
    } else {
        if( block->UseUnicode )
            block->Head.ValSize /= 2;
        block->Head.Type = 1;
    }
    block->Head.Size = head_size + val_size + padding + nest_size;

    return( block->Head.Size );
}

static bool SemWriteVerBlock( FullVerBlock *block, FILE *fp, int *err_code )
/**************************************************************************/
{
    bool            error;
    WResTargetOS    res_os;

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        res_os = WRES_OS_WIN32;
    } else {
        res_os = WRES_OS_WIN16;
    }
    error = ResWriteVerBlockHeader( &block->Head, block->UseUnicode, res_os, fp );
    *err_code = LastWresErr();
    if( !error && block->Value != NULL ) {
        error = semWriteVerValueList( block->Value, block->UseUnicode, fp, err_code );
        if( !error ) {
            error = ResWritePadDWord( fp );
            *err_code = LastWresErr();
        }
    }
    if( !error && block->Nest != NULL ) {
        error = SemWriteVerBlockNest( block->Nest, fp, err_code );
    }

    return( error );
}

static void FreeVerBlock( FullVerBlock *block )
/*********************************************/
{
    RESFREE( block->Head.Key );
    if( block->Value != NULL ) {
        FreeValList( block->Value );
    }
    if( block->Nest != NULL ) {
        FreeVerBlockNest( block->Nest );
    }

    RESFREE( block );
}

FullVerBlockNest *SemWINNewBlockNest( FullVerBlock *child )
/*********************************************************/
{
    FullVerBlockNest    *parent;

    parent = RESALLOC( sizeof( FullVerBlockNest ) );
    parent->Head = NULL;
    parent->Tail = NULL;

    return( SemWINAddBlockNest( parent, child ) );
}

FullVerBlockNest *SemWINAddBlockNest( FullVerBlockNest *parent, FullVerBlock *child )
/***********************************************************************************/
{
    ResAddLLItemAtEnd( (void **)&(parent->Head), (void **)&(parent->Tail), child );
    return( parent );
}

FullVerBlockNest *SemWINMergeBlockNest( FullVerBlockNest *nest1, FullVerBlockNest *nest2 )
/****************************************************************************************/
{
    FullVerBlock    *block;

    for( block = nest2->Head; block != NULL; block = block->Next ) {
        ResAddLLItemAtEnd( (void **)&nest1->Head, (void **)&nest1->Tail, block );
    }

    RESFREE( nest2 );

    return( nest1 );
}

static uint_16 CalcNestSize( FullVerBlockNest *nest )
/***************************************************/
{
    FullVerBlock    *block;
    uint_16         size;

    size = 0;
    for( block = nest->Head; block != NULL; block = block->Next ) {
        size += CalcBlockSize( block );
    }

    return( size );
}

static void FreeVerBlockNest( FullVerBlockNest *nest )
/****************************************************/
{
    FullVerBlock    *block;
    FullVerBlock    *nextblock;

    for( block = nest->Head; block != NULL; block = nextblock ) {
        nextblock = block->Next;
        FreeVerBlock( block );
    }

    RESFREE( nest );
}

static bool SemWriteVerBlockNest( FullVerBlockNest *nest, FILE *fp, int *err_code )
/*********************************************************************************/
{
    bool            error;
    FullVerBlock    *block;

    error = false;
    for( block = nest->Head; block != NULL && !error; block = block->Next ) {
        error = SemWriteVerBlock( block, fp, err_code );
    }

    return( error );
}


VerFixedInfo *SemWINNewVerFixedInfo( VerFixedOption option )
/**********************************************************/
{
    VerFixedInfo    *info;

    info = RESALLOC( sizeof( VerFixedInfo ) );
    memset( info, 0, sizeof( VerFixedInfo ) );

    return( SemWINAddVerFixedInfo( info, option ) );
}


#define MakeVersion( verp ) ((uint_32)(verp).LowWord | \
                            ((uint_32)(verp).HighWord << 16 ))

VerFixedInfo *SemWINAddVerFixedInfo( VerFixedInfo *info, VerFixedOption option )
/******************************************************************************/
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

void SemWINWriteVerInfo( WResID *name, ResMemFlags flags, VerFixedInfo *info, FullVerBlockNest *nest )
/****************************************************************************************************/
{
#if 0
    WResLangType    lang;
#endif
    VerBlockHeader  root;
    ResLocation     loc;
    int             padding;
    bool            error;
    bool            use_unicode;
    WResTargetOS    res_os;
    int             err_code;

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        use_unicode = true;
        res_os = WRES_OS_WIN32;
    } else {
        use_unicode = false;
        res_os = WRES_OS_WIN16;
    }
    root.Key = "VS_VERSION_INFO";
    root.ValSize = sizeof( VerFixedInfo );
    root.Type = 0;
    padding = RES_PADDING_DWORD( root.ValSize );
    root.Size = ResSizeVerBlockHeader( &root, use_unicode, res_os )
                    + root.ValSize + padding + CalcNestSize( nest );
    /* pad the start of the resource so that padding within the resource */
    /* is easier */
    error = ResWritePadDWord( CurrResFile.fp );
    if( error ) {
        err_code = LastWresErr();
        goto OutputWriteError;
    }

    if( !ErrorHasOccured ) {
        loc.start = SemStartResource();

        error = ResWriteVerBlockHeader( &root, use_unicode, res_os, CurrResFile.fp );
        if( error ) {
            err_code = LastWresErr();
            goto OutputWriteError;
        }

        error = ResWriteVerFixedInfo( info, CurrResFile.fp );
        if( error ) {
            err_code = LastWresErr();
            goto OutputWriteError;
        }

        if( ResSeek( CurrResFile.fp, padding, SEEK_CUR ) )  {
            err_code = LastWresErr();
            goto OutputWriteError;
        }

        error = SemWriteVerBlockNest( nest, CurrResFile.fp, &err_code );
        if( error)
            goto OutputWriteError;

        loc.len = SemEndResource( loc.start );

/*
 * MS resource compiler use global Language definition for Version Info resource
 * it is reason to comment out following code
 */
#if 0
        /* version info resources must be language neutral */

        lang.lang = DEF_LANG;
        lang.sublang = DEF_SUBLANG;
        SemWINSetResourceLanguage( &lang, false );
#endif
        SemAddResourceFree( name, WResIDFromNum( RESOURCE2INT( RT_VERSIONINFO ) ), flags, loc );
    } else {
        RESFREE( name );
    }

    RESFREE( info );
    FreeVerBlockNest( nest );

    return;

OutputWriteError:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code )  );
    ErrorHasOccured = true;
    RESFREE( info );
    FreeVerBlockNest( nest );
    return;
}
