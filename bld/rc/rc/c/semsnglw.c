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


#include "wio.h"
#include "global.h"
#include "errors.h"
#include "semantic.h"
#include "semantcw.h"
#include "reserr.h"
#include "depend.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "rccore.h"

#include "clibext.h"


/**** forward references ****/
static void AddIconResource( WResID * name, ResMemFlags flags,
                        ResMemFlags group_flags, const char * filename );
static void AddCursorResource( WResID * name, ResMemFlags flags,
                        ResMemFlags group_flags, const char * filename );
static void AddBitmapResource( WResID * name, ResMemFlags, const char * filename );
static void AddFontResources( WResID * name, ResMemFlags, const char * filename );

/* MS changed the default purity for ICON and CURSOR resources from rc */
/* version 30 to 31. Note: the ICON_GROUP and CURSOR_GROUP resources */
/* still have the same purity */
#define CUR_ICON_PURITY_30      MEMFLAG_PURE
#define CUR_ICON_PURITY_31      0           /* impure */

void SemWINAddMessageTable( WResID *name, ScanString *filename ) {
/****************************************************************/

    ResLocation         start;

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        start = SemCopyRawFile( filename->string );
        RCFREE( filename->string );
        RCFREE( filename );
        SemAddResourceFree( name, WResIDFromNum( RESOURCE2INT( RT_MESSAGETABLE ) ),
                            MEMFLAG_MOVEABLE | MEMFLAG_PURE, start );
    } else {
        RcError( ERR_NT_KEYWORD, SemWINTokenToString( Y_MESSAGETABLE ) );
        ErrorHasOccured = true;
        RCFREE( name );
        RCFREE( filename->string );
        RCFREE( filename );
    }
}

void SemWINAddSingleLineResource( WResID * name, YYTOKENTYPE type,
                      FullMemFlags * fullflags, char * filename )
/***************************************************************/
{
    ResMemFlags flags;
    ResMemFlags group_flags;
    ResMemFlags purity_option;      /* used for icon and cursor resoures */
    char        full_filename[_MAX_PATH];

    if( ErrorHasOccured ) {
        RCFREE( name );
        RCFREE( filename );
        return;
    }
    if( CmdLineParms.VersionStamp30 ) {
        purity_option = CUR_ICON_PURITY_30;
    } else {
        purity_option = CUR_ICON_PURITY_31;
    }

    if( RcFindResource( filename, full_filename ) == -1 ) {
        RcError( ERR_CANT_FIND_FILE, filename );
        goto HANDLE_ERROR;
    }

    if( AddDependency( full_filename ) )
        goto HANDLE_ERROR;

    switch( type ) {
    case Y_ICON:
        if( fullflags != NULL ) {
            SemWINCheckMemFlags( fullflags, 0,
                                MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
                                purity_option );
            flags = fullflags->flags;
            SemWINCheckMemFlags( fullflags, 0,
                                MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
                                MEMFLAG_PURE );
            group_flags = fullflags->flags;
        } else {
            flags = MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE| purity_option;
            group_flags = MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE|MEMFLAG_PURE;
        }
        AddIconResource( name, flags, group_flags, full_filename );
        break;
    case Y_CURSOR:
        if( fullflags != NULL ) {
            SemWINCheckMemFlags( fullflags, 0,
                                MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
                                purity_option );
            flags = fullflags->flags;
            SemWINCheckMemFlags( fullflags, 0,
                                MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
                                MEMFLAG_PURE );
            group_flags = fullflags->flags;
        } else {
            flags = MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE| purity_option;
            group_flags = MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE|MEMFLAG_PURE;
        }
        AddCursorResource( name, flags, group_flags, full_filename );
        break;
    case Y_BITMAP:
        if( fullflags != NULL ) {
            SemWINCheckMemFlags( fullflags, 0, MEMFLAG_MOVEABLE, MEMFLAG_PURE );
            flags = fullflags->flags;
        } else {
            flags = MEMFLAG_MOVEABLE|MEMFLAG_PURE;
        }
        AddBitmapResource( name, flags, full_filename );
        break;
    case Y_FONT:
        if( fullflags != NULL ) {
            SemWINCheckMemFlags( fullflags, 0,
                                MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE,
                                MEMFLAG_PURE );
            flags = fullflags->flags;
        } else {
            flags = MEMFLAG_MOVEABLE|MEMFLAG_DISCARDABLE|MEMFLAG_PURE;
        }
        AddFontResources( name, flags, full_filename );
        break;
    default:
        RCFREE( name );
        break;
    }

    RCFREE( filename );

    return;

HANDLE_ERROR:
    ErrorHasOccured = true;
    RCFREE( name );
    RCFREE( filename );
} /* SemWINAddSingleLineResource */

/*
 * ReadBitmapInfoHeader-
 * NB when an error occurs this func must return without altering errno
 */
static RcStatus ReadBitmapInfoHeader( BitmapInfoHeader * head, WResFileID handle )
/********************************************************************************/
{
    WResFileSSize   numread;

    numread = RCREAD( handle, head, sizeof(BitmapInfoHeader) );
    if( numread == sizeof( BitmapInfoHeader ) )
        return( RS_OK );
    return( RCIOERR( handle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
}

static RcStatus readIcoCurFileDirHeader( IconCurDirHeader * head,
                                    WResFileID handle, int *err_code )
/********************************************************************/
{
    WResFileSSize   numread;

    numread = RCREAD( handle, head, sizeof(IconCurDirHeader) );
    if( numread != sizeof( IconCurDirHeader ) ) {
        *err_code = errno;
        return( RCIOERR( handle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    return( RS_OK );
} /* readIcoCurFileDirHeader */

static RcStatus readIcoFileDirEntry( IcoFileDirEntry * entry,
                                WResFileID handle, int *err_code )
/****************************************************************/
{
    WResFileSSize   numread;

    numread = RCREAD( handle, entry, sizeof(IcoFileDirEntry) );
    if( numread != sizeof(IcoFileDirEntry ) ) {
        *err_code = errno;
        return( RCIOERR( handle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    return( RS_OK );
} /* readIcoFileDirEntry */

static RcStatus readIcoFileDir( WResFileID handle, FullIconDir *dir, int *err_code )
/**********************************************************************************/
/* this funtion returns one of the above enum constants */
{
    RcStatus            ret;
    int                 currentry;
    FullIconDirEntry *  entry;

    ret = readIcoCurFileDirHeader( &(dir->Header), handle, err_code );
    /* type 1 is a icon file */
    if( ret == RS_OK && dir->Header.Type != 1 ) {
        return( RS_INVALID_RESOURCE );
    }

    for( currentry = 0; ret == RS_OK && currentry < dir->Header.ResCount;
                            currentry++ ) {
        entry = RCALLOC( sizeof(FullIconDirEntry) );
        entry->Next = NULL;
        entry->Prev = NULL;
        entry->IsIcoFileEntry = true;
        ret = readIcoFileDirEntry( &(entry->Entry.Ico), handle, err_code );
        if( ret != RS_OK ) {
            RCFREE( entry );
        } else {
            ResAddLLItemAtEnd( (void **) &(dir->Head), (void **)&(dir->Tail), entry );
        }
    }
    return( ret );
} /* readIcoFileDir */


static RcStatus copyOneIcon( const IcoFileDirEntry *entry, WResFileID handle,
                void *buffer, int buffer_size, BitmapInfoHeader *dibhead,
                int *err_code )
/**************************************************************************/
/* NOTE: this routine fills in dibhead as it copies the data */
{
    RcStatus            ret;
    WResFileOffset      curpos;

    ret = RS_OK;
    if( RCSEEK( handle, entry->Offset, SEEK_SET ) == -1 ) {
        ret = RS_READ_ERROR;
        *err_code = errno;
    }
    if( ret == RS_OK ) {
        ret = ReadBitmapInfoHeader( dibhead, handle );
        *err_code = errno;
    }
    if( ret == RS_OK ) {
        if( ResWriteBitmapInfoHeader( dibhead, CurrResFile.handle ) ) {
            ret = RS_WRITE_ERROR;
            *err_code = LastWresErr();
        }
    }
    if( ret == RS_OK ) {
        curpos = RCTELL( handle );
        if( curpos == -1 ) {
            ret = RS_READ_ERROR;
            *err_code = errno;
        } else {
            ret = CopyData( curpos,
                              entry->Info.Length - sizeof(BitmapInfoHeader),
                              handle, buffer, buffer_size, err_code );
        }
    }

    return( ret );
} /* copyOneIcon */

#define BUFFER_SIZE     1024

static RcStatus copyIcons( FullIconDir * dir, WResFileID handle,
                            ResMemFlags flags, int *err_code )
/**************************************************************/
{
    RcStatus            ret;
    char *              buffer;
    FullIconDirEntry *  entry;
    BitmapInfoHeader    dibhead;
    ResLocation         loc;

    ret = RS_OK;
    buffer = RCALLOC( BUFFER_SIZE );

    for( entry = dir->Head; entry != NULL; entry = entry->Next ) {
        /* copy the icon */
        loc.start = SemStartResource();

        /* NOTE: the dibhead structure is filled in as a result of this call */
        ret = copyOneIcon( &(entry->Entry.Ico), handle, buffer, BUFFER_SIZE, &(dibhead), err_code );
        if( ret != RS_OK )
            break;

        loc.len = SemEndResource( loc.start );
        /* add the icon to the RES file directory */
        SemAddResourceFree( WResIDFromNum( CurrResFile.NextCurOrIcon ),
                WResIDFromNum( RESOURCE2INT( RT_ICON ) ), flags, loc );
        /* change the reference in the ICON directory */
        entry->IsIcoFileEntry = false;
        entry->Entry.Res.IconID = CurrResFile.NextCurOrIcon;
        entry->Entry.Res.Info.Planes = dibhead.Planes;
        entry->Entry.Res.Info.BitCount = dibhead.BitCount;
        CurrResFile.NextCurOrIcon += 1;
    }

    RCFREE( buffer );

    return( ret );
} /* copyIcons */

static void FreeIconDir( FullIconDir * dir )
/******************************************/
{
    FullIconDirEntry * currentry;
    FullIconDirEntry * oldentry;

    currentry = dir->Head;
    while( currentry != NULL ) {
        oldentry = currentry;
        currentry = currentry->Next;

        RCFREE( oldentry );
    }
} /* FreeIconDir */

static bool writeIconDir( FullIconDir * dir, WResID * name, ResMemFlags flags,
                         int *err_code )
/****************************************************************************/
{
    bool                error;
    FullIconDirEntry *  entry;
    ResLocation         loc;

    loc.start = SemStartResource();
    error = ResWriteIconCurDirHeader( &(dir->Header), CurrResFile.handle );

    for( entry = dir->Head; !error && entry != NULL; entry = entry->Next ) {
        error = ResWriteIconDirEntry( &(entry->Entry.Res), CurrResFile.handle );
    }

    if( !error ) {
        loc.len = SemEndResource( loc.start );
        SemAddResourceFree( name, WResIDFromNum( RESOURCE2INT( RT_GROUP_ICON ) ), flags, loc );
    } else {
        *err_code = LastWresErr();
    }

    return( error );
} /* writeIconDir */

static void AddIconResource( WResID * name, ResMemFlags flags,
                  ResMemFlags group_flags, const char * filename )
/****************************************************************/
{
    WResFileID      handle;
    RcStatus        ret;
    bool            error;
    FullIconDir     dir;
    int             err_code;

    handle = RcIoOpenInput( filename, O_RDONLY | O_BINARY );
    if( handle == NIL_HANDLE)
        goto FILE_OPEN_ERROR;

    dir.Head = NULL;
    dir.Tail = NULL;

    ret = readIcoFileDir( handle, &dir, &err_code );
    if( ret != RS_OK )
        goto READ_DIR_ERROR;

    ret = copyIcons( &dir, handle, flags, &err_code );
    if( ret != RS_OK )
        goto COPY_ICONS_ERROR;

    error = writeIconDir( &dir, name, group_flags, &err_code );
    if( error)
        goto WRITE_DIR_ERROR;

    FreeIconDir( &dir );
    RCCLOSE( handle );

    return;


FILE_OPEN_ERROR:
    RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
    ErrorHasOccured = true;
    RCFREE( name );
    return;

READ_DIR_ERROR:
    if( ret == RS_INVALID_RESOURCE ) {
        RcError( ERR_NOT_ICON_FILE, filename );
    } else {
        ReportCopyError( ret, ERR_READING_ICON, filename, err_code );
    }
    ErrorHasOccured = true;
    RCFREE( name );
    FreeIconDir( &dir );
    RCCLOSE( handle );
    return;

WRITE_DIR_ERROR:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code ) );
    ErrorHasOccured = true;
    FreeIconDir( &dir );
    RCCLOSE( handle );
    return;

COPY_ICONS_ERROR:
    ReportCopyError( ret, ERR_READING_ICON, filename, err_code );
    ErrorHasOccured = true;
    RCFREE( name );
    FreeIconDir( &dir );
    RCCLOSE( handle );
    return;
} /* AddIconResource */

static bool writeCurDir( FullCurDir *dir, WResID *name, ResMemFlags flags,
                        int *err_code )
/****************************************************************************/
{
    bool                error;
    FullCurDirEntry *   entry;
    ResLocation         loc;

    loc.start = SemStartResource();
    error = ResWriteIconCurDirHeader( &(dir->Header), CurrResFile.handle );

    for( entry = dir->Head; !error && entry != NULL; entry = entry->Next ) {
        error = ResWriteCurDirEntry( &(entry->Entry.Res), CurrResFile.handle );
    }

    if( !error ) {
        loc.len = SemEndResource( loc.start );
        SemAddResourceFree( name, WResIDFromNum( RESOURCE2INT( RT_GROUP_CURSOR ) ), flags, loc );
    } else {
        *err_code = LastWresErr();
    }

    return( error );
}

static RcStatus copyOneCursor( const CurFileDirEntry *entry, WResFileID handle,
                void *buffer, int buffer_size, BitmapInfoHeader *dibhead,
                int *err_code )
/*****************************************************************************/
/* NOTE: this routine fills in dibhead as it copies the data */
{
    RcStatus        ret;
    WResFileOffset  curpos;

    ret = RS_OK;
    if( RCSEEK( handle, entry->Offset, SEEK_SET ) == -1 ) {
        ret = RS_READ_ERROR;
        *err_code = errno;
    }

    if( ret == RS_OK ) {
        ret = ReadBitmapInfoHeader( dibhead, handle );
        *err_code = errno;
    }
    if( ret == RS_OK ) {
        if( ResWriteBitmapInfoHeader( dibhead, CurrResFile.handle ) ) {
            ret = RS_WRITE_ERROR;
            *err_code = LastWresErr();
        }
    }
    if( ret == RS_OK ) {
        curpos = RCTELL( handle );
        if( curpos == -1 ) {
            ret = RS_READ_ERROR;
            *err_code = errno;
        } else {
            ret = CopyData( curpos,
                              entry->Length - sizeof(BitmapInfoHeader),
                              handle, buffer, buffer_size, err_code );
        }
    }

    return( ret );
}


static RcStatus copyCursors( FullCurDir * dir, WResFileID handle,
                             ResMemFlags flags, int *err_code )
/***********************************************************************/
/* This function uses the same size of buffers to copy info as for icons */
{
    RcStatus            ret = RS_OK; // should this be RS_PARAM_ERROR ??
    char *              buffer;
    FullCurDirEntry *   entry;
    CurFileDirEntry     fileentry;
    CurHotspot          hotspot;
    BitmapInfoHeader    dibhead;
    ResLocation         loc;

    buffer = RCALLOC( BUFFER_SIZE );

    for( entry = dir->Head; entry != NULL; entry = entry->Next ) {
        /* copy the cursor */
        loc.start = SemStartResource();

        hotspot.X = entry->Entry.Cur.XHotspot;
        hotspot.Y = entry->Entry.Cur.YHotspot;
        if( ResWriteCurHotspot( &hotspot, CurrResFile.handle ) ) {
            ret = RS_WRITE_ERROR;
            *err_code = LastWresErr();
            break;
        }

        /* NOTE: the dibhead structure is filled in as a result of this call */
        ret = copyOneCursor( &(entry->Entry.Cur), handle, buffer,
                        BUFFER_SIZE, &(dibhead), err_code );
        if( ret != RS_OK )
            break;

        loc.len = SemEndResource( loc.start );
        /* add the cursor to the RES file directory */
        SemAddResourceFree( WResIDFromNum( CurrResFile.NextCurOrIcon ),
                WResIDFromNum( RESOURCE2INT( RT_CURSOR ) ), flags, loc );
        /* change the reference in the cursor directory */
        fileentry = entry->Entry.Cur;
        entry->IsCurFileEntry = false;
        entry->Entry.Res.Width = dibhead.Width;
        entry->Entry.Res.Height = dibhead.Height;
        entry->Entry.Res.Planes = dibhead.Planes;
        entry->Entry.Res.BitCount = dibhead.BitCount;
        /* the hotspot data is now part of the components */
        entry->Entry.Res.Length = fileentry.Length + sizeof(CurHotspot);
        entry->Entry.Res.CurID = CurrResFile.NextCurOrIcon;
        CurrResFile.NextCurOrIcon += 1;
    }

    RCFREE( buffer );

    return( ret );
} /* copyCursors */

static RcStatus readCurFileDirEntry( CurFileDirEntry * entry, WResFileID handle,
                                        int *err_code )
/******************************************************************************/
{
    WResFileSSize   numread;

    numread = RCREAD( handle, entry, sizeof(CurFileDirEntry) );
    if( numread != sizeof(CurFileDirEntry ) ) {
        *err_code = errno;
        return( RCIOERR( handle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    return( RS_OK );
} /* readCurFileDirEntry */

static RcStatus readCurFileDir( WResFileID handle, FullCurDir *dir, int *err_code )
/*********************************************************************************/
/* this funtion returns one of the above enum constants */
{
    RcStatus            ret;
    int                 currentry;
    FullCurDirEntry *   entry;

    ret = readIcoCurFileDirHeader( &(dir->Header), handle, err_code );
    /* type 2 is a cursor file */
    if( ret == RS_OK && dir->Header.Type != 2 ) {
        return( RS_INVALID_RESOURCE );
    }

    for( currentry = 0; ret == RS_OK && currentry < dir->Header.ResCount;
                            currentry++ ) {
        entry = RCALLOC( sizeof(FullCurDirEntry) );
        entry->Next = NULL;
        entry->Prev = NULL;
        entry->IsCurFileEntry = true;
        ret = readCurFileDirEntry( &(entry->Entry.Cur), handle, err_code );
        if( ret != RS_OK ) {
            RCFREE( entry );
        } else {
            ResAddLLItemAtEnd( (void **) &(dir->Head), (void **) &(dir->Tail), entry );
        }
    }
    return( ret );

} /* readCurFileDir */

static void FreeCurDir( FullCurDir * dir )
/****************************************/
{
    FullCurDirEntry * currentry;
    FullCurDirEntry * oldentry;

    currentry = dir->Head;
    while( currentry != NULL ) {
        oldentry = currentry;
        currentry = currentry->Next;

        RCFREE( oldentry );
    }
} /* FreeCurDir */

static void AddCursorResource( WResID * name, ResMemFlags flags,
                      ResMemFlags group_flags, const char * filename )
/********************************************************************/
{
    WResFileID      handle;
    RcStatus        ret;
    bool            error;
    FullCurDir      dir;
    int             err_code;

    handle = RcIoOpenInput( filename, O_RDONLY | O_BINARY );
    if( handle == NIL_HANDLE)
        goto FILE_OPEN_ERROR;

    dir.Head = NULL;
    dir.Tail = NULL;

    ret = readCurFileDir( handle, &dir, &err_code );
    if( ret != RS_OK)
        goto READ_DIR_ERROR;

    ret = copyCursors( &dir, handle, flags, &err_code );
    if( ret != RS_OK )
        goto COPY_CURSORS_ERROR;

    error = writeCurDir( &dir, name, group_flags, &err_code );
    if( error)
        goto WRITE_DIR_ERROR;

    FreeCurDir( &dir );
    RCCLOSE( handle );

    return;


FILE_OPEN_ERROR:
    RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
    ErrorHasOccured = true;
    RCFREE( name );
    return;

READ_DIR_ERROR:
    if( ret == RS_INVALID_RESOURCE ) {
        RcError( ERR_NOT_CURSOR_FILE, filename );
    } else {
        ReportCopyError( ret, ERR_READING_CURSOR, filename, err_code );
    }
    ErrorHasOccured = true;
    RCFREE( name );
    FreeCurDir( &dir );
    RCCLOSE( handle );
    return;

WRITE_DIR_ERROR:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, strerror( err_code )  );
    ErrorHasOccured = true;
    FreeCurDir( &dir );
    RCCLOSE( handle );
    return;

COPY_CURSORS_ERROR:
    ReportCopyError( ret, ERR_READING_CURSOR, filename, err_code );
    ErrorHasOccured = true;
    RCFREE( name );
    FreeCurDir( &dir );
    RCCLOSE( handle );
    return;
} /* AddCursorResource */

static RcStatus readBitmapFileHeader( WResFileID handle, BitmapFileHeader *head,
                                        int *err_code )
/******************************************************************************/
{
    WResFileSSize   numread;

    numread = RCREAD( handle, head, sizeof(BitmapFileHeader) );
    if( numread != sizeof( BitmapFileHeader ) ) {
        *err_code = errno;
        return( RCIOERR( handle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    return( RS_OK );
}

#define BITMAP_BUFFER_SIZE  0x1000

static RcStatus copyBitmap( BitmapFileHeader *head, WResFileID handle,
                            WResID *name, ResMemFlags flags, int *err_code )
/**************************************************************************/
{
    RcStatus            ret;
    char *              buffer;
    ResLocation         loc;
    WResFileOffset      pos;

    buffer = RCALLOC( BITMAP_BUFFER_SIZE );

    loc.start = SemStartResource();

    pos = RCTELL( handle );
    if( pos == -1 ) {
        ret = RS_READ_ERROR;
        *err_code = errno;
    } else {
        ret = CopyData( pos, head->Size - sizeof(BitmapFileHeader),
                          handle, buffer, BITMAP_BUFFER_SIZE, err_code );
    }

    loc.len = SemEndResource( loc.start );
    /* add the bitmap to the RES file directory */
    SemAddResourceFree( name, WResIDFromNum( RESOURCE2INT( RT_BITMAP ) ), flags, loc );

    RCFREE( buffer );

    return( ret );
} /* copyBitmap */

static void AddBitmapResource( WResID * name, ResMemFlags flags,
                            const char * filename )
/**************************************************************/
{
    BitmapFileHeader    head;
    WResFileID          handle;
    RcStatus            ret;
    int                 err_code;

    handle = RcIoOpenInput( filename, O_RDONLY | O_BINARY );
    if( handle == NIL_HANDLE)
        goto FILE_OPEN_ERROR;

    ret = readBitmapFileHeader( handle, &head, &err_code );
    if( ret != RS_OK )
        goto READ_HEADER_ERROR;

    if( head.Type != BITMAP_MAGIC )
        goto NOT_BITMAP_ERROR;

    ret = copyBitmap( &head, handle, name, flags, &err_code );
    if( ret != RS_OK )
        goto COPY_BITMAP_ERROR;

    RCCLOSE( handle );

    return;


FILE_OPEN_ERROR:
    RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
    ErrorHasOccured = true;
    RCFREE( name );
    return;

READ_HEADER_ERROR:
    ReportCopyError( ret, ERR_READING_BITMAP, filename, err_code );
    ErrorHasOccured = true;
    RCFREE( name );
    RCCLOSE( handle );
    return;

NOT_BITMAP_ERROR:
    RcError( ERR_NOT_BITMAP_FILE, filename );
    ErrorHasOccured = true;
    RCFREE( name );
    RCCLOSE( handle );
    return;

COPY_BITMAP_ERROR:
    ReportCopyError( ret, ERR_READING_BITMAP, filename, err_code );
    ErrorHasOccured = true;
    RCCLOSE( handle );
    return;
}

static RcStatus readFontInfo( WResFileID handle, FontInfo *info, int *err_code )
/******************************************************************************/
{
    WResFileSSize   numread;

    numread = RCREAD( handle, info, sizeof(FontInfo) );
    if( numread != sizeof(FontInfo) ) {
        *err_code = errno;
        return( RCIOERR( handle, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    return( RS_OK );
}

#define FONT_BUFFER_SIZE  0x1000

static RcStatus copyFont( FontInfo *info, WResFileID handle, WResID *name,
                                ResMemFlags flags, int *err_code )
/**************************************************************************/
{
    RcStatus            ret;
    char *              buffer;
    ResLocation         loc;
    WResFileOffset      pos;

    buffer = RCALLOC( FONT_BUFFER_SIZE );

    loc.start = SemStartResource();

    if( ResWriteFontInfo( info, CurrResFile.handle ) ) {
        ret = RS_WRITE_ERROR;
        *err_code = LastWresErr();
    } else {
        pos = RCTELL( handle );
        if( pos == -1 ) {
            ret = RS_READ_ERROR;
            *err_code = errno;
        } else {
            ret = SemCopyDataUntilEOF( pos, handle, buffer,
                                         FONT_BUFFER_SIZE, err_code );
        }
    }

    loc.len = SemEndResource( loc.start );
    /* add the font to the RES file directory */
    SemAddResourceFree( name, WResIDFromNum( RESOURCE2INT( RT_FONT ) ), flags, loc );

    RCFREE( buffer );

    return( ret );
} /* copyFont */

typedef struct {
    RcStatus    status;
    int         err_code;
}ReadStrErrInfo;

static void * readString( WResFileID handle, long offset, ReadStrErrInfo *err )
/*****************************************************************************/
{
    char    *retstr;

    if( RCSEEK( handle, offset, SEEK_SET ) == -1 ) {
        err->status = RS_READ_ERROR;
        err->err_code = errno;
        return( NULL );
    } else {
        retstr = ResReadString( handle, NULL );
        if( retstr == NULL ) {
            if( LastWresStatus() == WRS_READ_INCOMPLETE ) {
                err->status = RS_READ_INCMPLT;
            } else {
                err->status = RS_READ_ERROR;
                err->err_code = LastWresErr();
            }
            return( NULL );
        } else {
            return( retstr );
        }
    }
}

static FullFontDir * NewFontDir( void )
/*************************************/
{
    FullFontDir *   newdir;

    newdir = RCALLOC( sizeof(FullFontDir) );
    newdir->Head = NULL;
    newdir->Tail = NULL;
    newdir->NumOfFonts = 0;

    return( newdir );
}

static FullFontDirEntry * NewFontDirEntry( FontInfo * info, char * devicename,
                        char * facename, WResID * fontid )
/****************************************************************************/
{
    FullFontDirEntry *      entry;
    size_t                  structextra;
    size_t                  devicelen;
    size_t                  facelen;

    devicelen = strlen( devicename ) + 1;
    facelen = strlen( facename ) + 1;
    structextra = devicelen + facelen;

    /* -1 for the 1 char in the struct already */
    entry = RCALLOC( sizeof(FullFontDirEntry) + structextra - 1 );
    entry->Next = NULL;
    entry->Prev = NULL;
    /* -1 for the 1 char in the struct already */
    entry->Entry.StructSize = sizeof(FontDirEntry) + structextra - 1;
    entry->Entry.FontID = fontid->ID.Num;
    entry->Entry.Info = *info;
    memcpy( &(entry->Entry.DevAndFaceName[0]), devicename, devicelen );
    memcpy( &(entry->Entry.DevAndFaceName[devicelen]), facename, facelen );
    /* set dfDevice and dfFace to be the offset of the strings from the start */
    /* of the FontInfo structure (entry->Entry.Info) */
    entry->Entry.Info.dfDevice = sizeof(FontInfo);
    entry->Entry.Info.dfFace = sizeof(FontInfo) + devicelen;

    return( entry );
}

static void AddFontToDir( FontInfo * info, char * devicename, char * facename,
                WResID * fontid )
/****************************************************************************/
{
    FullFontDirEntry *      entry;

    entry = NewFontDirEntry( info, devicename, facename, fontid );

    if( CurrResFile.FontDir == NULL ) {
        CurrResFile.FontDir = NewFontDir();
    }

    ResAddLLItemAtEnd( (void **) &(CurrResFile.FontDir->Head),
                        (void **) &(CurrResFile.FontDir->Tail), entry );
    CurrResFile.FontDir->NumOfFonts += 1;
}

static void AddFontResources( WResID * name, ResMemFlags flags,
                              const char * filename )
/**************************************************************/
{
    FontInfo            info;
    char *              devicename;
    char *              facename;
    WResFileID          handle;
    RcStatus            ret;
    int                 err_code;
    ReadStrErrInfo      readstr_err;

    if( name->IsName ) {
        RcError( ERR_FONT_NAME );
        return;
    }

    handle = RcIoOpenInput( filename, O_RDONLY | O_BINARY );
    if( handle == NIL_HANDLE)
        goto FILE_OPEN_ERROR;

    ret = readFontInfo( handle, &info, &err_code );
    if( ret != RS_OK)
        goto READ_HEADER_ERROR;

    ret = copyFont( &info, handle, name, flags, &err_code );
    if( ret != RS_OK )
        goto COPY_FONT_ERROR;

    devicename = readString( handle, info.dfDevice, &readstr_err );
    if( devicename == NULL ) {
        ret = readstr_err.status;
        err_code = readstr_err.err_code;
        goto READ_HEADER_ERROR;
    }

    facename = readString( handle, info.dfFace, &readstr_err );
    if( facename == NULL ) {
        ret = readstr_err.status;
        err_code = readstr_err.err_code;
        RCFREE( devicename );
        goto READ_HEADER_ERROR;
    }

    AddFontToDir( &info, devicename, facename, name );

    RCFREE( devicename );
    RCFREE( facename );

    RCCLOSE( handle );

    return;


FILE_OPEN_ERROR:
    RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
    ErrorHasOccured = true;
    RCFREE( name );
    return;

READ_HEADER_ERROR:
    ReportCopyError( ret, ERR_READING_FONT, filename, err_code );
    ErrorHasOccured = true;
    RCFREE( name );
    RCCLOSE( handle );
    return;

COPY_FONT_ERROR:
    ReportCopyError( ret, ERR_READING_FONT, filename, err_code );
    ErrorHasOccured = true;
    RCCLOSE( handle );
    return;
}

static void FreeFontDir( FullFontDir * olddir )
/*********************************************/
{
    FullFontDirEntry *  currentry;
    FullFontDirEntry *  oldentry;

    currentry = olddir->Head;
    while( currentry != NULL ) {
        oldentry = currentry;
        currentry = currentry->Next;

        RCFREE( oldentry );
    }

    RCFREE( olddir );
}

/* name and memory flags of the font directory resource */
#define FONT_DIR_NAME   "FONTDIR"
#define FONT_DIR_FLAGS  MEMFLAG_MOVEABLE|MEMFLAG_PRELOAD   /* not PURE */

void SemWINWriteFontDir( void )
/*****************************/
{
    FullFontDirEntry *  currentry;
    ResLocation         loc;
    bool                error;

    if( CurrResFile.FontDir == NULL ) {
        return;
    }

    loc.start = SemStartResource();

    error = ResWriteUint16( &(CurrResFile.FontDir->NumOfFonts),
                        CurrResFile.handle );
    if( error)
        goto OUTPUT_WRITE_ERROR;

    for( currentry = CurrResFile.FontDir->Head; currentry != NULL;
                currentry = currentry->Next ) {
        error = ResWriteFontDirEntry( &(currentry->Entry), CurrResFile.handle );
        if( error ) {
            goto OUTPUT_WRITE_ERROR;
        }
    }

    loc.len = SemEndResource( loc.start );

    SemAddResourceFree( WResIDFromStr( FONT_DIR_NAME ),
                WResIDFromNum( RESOURCE2INT( RT_FONTDIR ) ), FONT_DIR_FLAGS, loc );

    FreeFontDir( CurrResFile.FontDir );
    CurrResFile.FontDir = NULL;

    return;


OUTPUT_WRITE_ERROR:
    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, LastWresErrStr() );
    ErrorHasOccured = true;
    FreeFontDir( CurrResFile.FontDir );
    CurrResFile.FontDir = NULL;
    return;
}
