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
* Description:  OS/2 single line resource semantic actions.
*
****************************************************************************/


#include "global.h"
#include "rcerrors.h"
#include "semantic.h"
#include "semantc2.h"
#include "reserr.h"
#include "depend.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "rccore.h"

#include "clibext.h"


/**** forward references ****/
static void AddFontResources( WResID * name, ResMemFlags, const char * filename );


void SemOS2AddSingleLineResource( WResID *name, YYTOKENTYPE type,
                       FullOptFlagsOS2 *fullflags, char *filename )
/*****************************************************************/
{
    ResLocation     start;
    ResMemFlags     flags, flagsMDP, flagsMP;
    char            full_filename[_MAX_PATH];
    static bool     firstIcon = true;
    bool            error;

    error = true;
    if( !ErrorHasOccured ) {
        if( RcFindSourceFile( filename, full_filename ) == -1 ) {
            RcError( ERR_CANT_FIND_FILE, filename );
        } else {
            error = AddDependency( full_filename );
            if( !error ) {
                flagsMDP = MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE | MEMFLAG_PURE;
                flagsMP  = MEMFLAG_MOVEABLE | MEMFLAG_PURE;
                switch( type ) {
                case Y_DEFAULTICON:
                    /* DEFAULTICON doesn't have a name, let's make our own */
                    name = RESALLOC( sizeof( WResID ) );
                    name->IsName = false;
                    name->ID.Num = 999;
                    firstIcon    = true;    /* Trigger a warning if we have one already */
                    /* Note the fallthrough! */
                case Y_POINTER:
                case Y_ICON:
                    if( fullflags != NULL ) {
                        SemOS2CheckResFlags( fullflags, 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE, 0 );
                        flags = fullflags->flags;
                    } else {
                        flags = flagsMDP;
                    }

                    /* Duplicate the first icon encountered as the default icon IFF it
                       has resource ID equal to 1
                    */
                    if( firstIcon && !name->IsName && (name->ID.Num == 999 || name->ID.Num == 1) ) {
                        WResID      *id;

                        id = RESALLOC( sizeof( WResID ) );
                        if( id == NULL )
                            break;

                        firstIcon  = false;
                        id->IsName = false;
                        id->ID.Num = 22;
                        start = SemCopyRawFileOnly( full_filename );
                        SemAddResourceFree( name, WResIDFromNum( OS2_RT_POINTER ), flags, start );

                        start = SemCopyRawFileOnly( full_filename );
                        SemAddResourceFree( id, WResIDFromNum( OS2_RT_DEFAULTICON ), flagsMDP, start );
                    } else {
                        start = SemCopyRawFileOnly( full_filename );
                        SemAddResourceFree( name, WResIDFromNum( OS2_RT_POINTER ), flags, start );
                    }
                    break;
                case Y_BITMAP:
                    if( fullflags != NULL ) {
                        SemOS2CheckResFlags( fullflags, 0, MEMFLAG_MOVEABLE, MEMFLAG_PURE );
                        flags = fullflags->flags;
                    } else {
                        flags = flagsMP;
                    }
                    start = SemCopyRawFileOnly( full_filename );
                    SemAddResourceFree( name, WResIDFromNum( OS2_RT_BITMAP ), flags, start );
                    break;
                case Y_FONT:
                    if( fullflags != NULL ) {
                        SemOS2CheckResFlags( fullflags, 0, MEMFLAG_MOVEABLE | MEMFLAG_DISCARDABLE, MEMFLAG_PURE );
                        flags = fullflags->flags;
                    } else {
                        flags = flagsMDP;
                    }
                    AddFontResources( name, flags, full_filename );
                    break;
                default:
                    RESFREE( name );
                    break;
                }
            }
        }
    }
    if( error ) {
        RESFREE( name );
        ErrorHasOccured = true;
    }
    RESFREE( filename );

} /* SemOS2AddSingleLineResource */

static RcStatus readFontInfo( FILE *fp, FontInfo *info, int *err_code )
/*********************************************************************/
{
    size_t      numread;

    numread = RESREAD( fp, info, sizeof( *info ) );
    if( numread != sizeof( *info ) ) {
        *err_code = errno;
        return( RESIOERR( fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    return( RS_OK );
}

#define FONT_BUFFER_SIZE  0x1000

static RcStatus copyFont( FontInfo *info, FILE *fp, WResID *name,
                                ResMemFlags flags, int *err_code )
/*********************************************************************/
{
    RcStatus            ret;
    char *              buffer;
    ResLocation         loc;
    long                pos;

    buffer = RESALLOC( FONT_BUFFER_SIZE );

    loc.start = SemStartResource();

    if( ResWriteFontInfo( info, CurrResFile.fp ) ) {
        ret = RS_WRITE_ERROR;
        *err_code = LastWresErr();
    } else {
        pos = RESTELL( fp );
        if( pos == -1L ) {
            ret = RS_READ_ERROR;
            *err_code = errno;
        } else {
            ret = SemCopyDataUntilEOF( pos, fp, buffer, FONT_BUFFER_SIZE, err_code );
        }
    }

    loc.len = SemEndResource( loc.start );
    /* add the font to the RES file directory */
    SemAddResourceFree( name, WResIDFromNum( RESOURCE2INT( RT_FONT ) ), flags, loc );

    RESFREE( buffer );

    return( ret );
} /* copyFont */

typedef struct {
    RcStatus    status;
    int         err_code;
}ReadStrErrInfo;

static void * readString( FILE *fp, long offset, ReadStrErrInfo *err )
/********************************************************************/
{
    char    *retstr;


    if( RESSEEK( fp, offset, SEEK_SET ) ) {
        err->status = RS_READ_ERROR;
        err->err_code = errno;
        return( NULL );
    } else {
        retstr = ResReadString( fp, NULL );
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
    FullFontDir     *newdir;

    newdir = RESALLOC( sizeof( FullFontDir ) );
    newdir->Head = NULL;
    newdir->Tail = NULL;
    newdir->NumOfFonts = 0;

    return( newdir );
}

static FullFontDirEntry *NewFontDirEntry( FontInfo *info, char *devicename, char *facename, WResID *fontid )
/**********************************************************************************************************/
{
    FullFontDirEntry        *entry;
    int                     structextra;
    int                     devicelen;
    int                     facelen;

    devicelen = strlen( devicename ) + 1;
    facelen = strlen( facename ) + 1;
    structextra = devicelen + facelen;

    /* -1 for the 1 char in the struct already */
    entry = RESALLOC( sizeof( FullFontDirEntry ) + structextra - 1 );
    entry->Next = NULL;
    entry->Prev = NULL;
    /* -1 for the 1 char in the struct already */
    entry->Entry.StructSize = sizeof( FontDirEntry ) + structextra - 1;
    entry->Entry.FontID = fontid->ID.Num;
    entry->Entry.Info = *info;
    memcpy( &(entry->Entry.DevAndFaceName[0]), devicename, devicelen );
    memcpy( &(entry->Entry.DevAndFaceName[devicelen]), facename, facelen );
    /* set dfDevice and dfFace to be the offset of the strings from the start */
    /* of the FontInfo structure (entry->Entry.Info) */
    entry->Entry.Info.dfDevice = sizeof( FontInfo );
    entry->Entry.Info.dfFace = sizeof( FontInfo ) + devicelen;

    return( entry );
}

static void AddFontToDir( FontInfo *info, char *devicename, char *facename, WResID *fontid )
/******************************************************************************************/
{
    FullFontDirEntry        *entry;

    entry = NewFontDirEntry( info, devicename, facename, fontid );

    if( CurrResFile.FontDir == NULL ) {
        CurrResFile.FontDir = NewFontDir();
    }

    ResAddLLItemAtEnd( (void **)&(CurrResFile.FontDir->Head),
                        (void **)&(CurrResFile.FontDir->Tail), entry );
    CurrResFile.FontDir->NumOfFonts += 1;
}

static void AddFontResources( WResID *name, ResMemFlags flags, const char *filename )
/***********************************************************************************/
{
    FontInfo            info;
    char                *devicename;
    char                *facename;
    FILE                *fp;
    RcStatus            ret;
    int                 err_code;
    ReadStrErrInfo      readstr_err;

    if( name->IsName ) {
        RcError( ERR_FONT_NAME );
        return;
    }

    fp = RcIoOpenInput( filename, false );
    if( fp == NULL)
        goto FILE_OPEN_ERROR;

    ret = readFontInfo( fp, &info, &err_code );
    if( ret != RS_OK)
        goto READ_HEADER_ERROR;

    ret = copyFont( &info, fp, name, flags, &err_code );
    if( ret != RS_OK )
        goto COPY_FONT_ERROR;

    devicename = readString( fp, info.dfDevice, &readstr_err );
    if( devicename == NULL ) {
        ret = readstr_err.status;
        err_code = readstr_err.err_code;
        goto READ_HEADER_ERROR;
    }

    facename = readString( fp, info.dfFace, &readstr_err );
    if( facename == NULL ) {
        ret = readstr_err.status;
        err_code = readstr_err.err_code;
        RESFREE( devicename );
        goto READ_HEADER_ERROR;
    }

    AddFontToDir( &info, devicename, facename, name );

    RESFREE( devicename );
    RESFREE( facename );

    RESCLOSE( fp );

    return;


FILE_OPEN_ERROR:
    RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
    ErrorHasOccured = true;
    RESFREE( name );
    return;

READ_HEADER_ERROR:
    ReportCopyError( ret, ERR_READING_FONT, filename, err_code );
    ErrorHasOccured = true;
    RESFREE( name );
    RESCLOSE( fp );
    return;

COPY_FONT_ERROR:
    ReportCopyError( ret, ERR_READING_FONT, filename, err_code );
    ErrorHasOccured = true;
    RESCLOSE( fp );
    return;
}

static void FreeFontDir( FullFontDir *olddir )
/********************************************/
{
    FullFontDirEntry    *currentry;
    FullFontDirEntry    *nextentry;

    for( currentry = olddir->Head; currentry != NULL; currentry = nextentry ) {
        nextentry = currentry->Next;
        RESFREE( currentry );
    }

    RESFREE( olddir );
}

/* name and memory flags of the font directory resource */
#define FONT_DIR_NAME   "FONTDIR"
#define FONT_DIR_FLAGS  MEMFLAG_MOVEABLE|MEMFLAG_PRELOAD   /* not PURE */

void SemOS2WriteFontDir( void )
/*****************************/
{
    FullFontDirEntry *  currentry;
    ResLocation         loc;
    bool                error;

    if( CurrResFile.FontDir == NULL ) {
        return;
    }

    loc.start = SemStartResource();

    error = ResWriteUint16( CurrResFile.FontDir->NumOfFonts, CurrResFile.fp );
    if( error)
        goto OUTPUT_WRITE_ERROR;

    for( currentry = CurrResFile.FontDir->Head; currentry != NULL; currentry = currentry->Next ) {
        error = ResWriteFontDirEntry( &(currentry->Entry), CurrResFile.fp );
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
