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


#include <wwindows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrrdres.h"
#include "wrrdw16.h"
#include "wrrdwnt.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define BITMAP_TYPE             ((((WORD)'M') << 8) + 'B')
#define ICON_FILE_TYPE          1
#define CURSOR_FILE_TYPE        2

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WRFileType WRIdentifyRESFile( const char * );
static WRFileType WRIdentifyEXEFile( const char *, bool );
static WRFileType WRIdentifyWinBMPFile( const char * );
static WRFileType WRIdentifyWinICOFile( const char * );
static WRFileType IDIconOrCursor( FILE * );
static WRFileType WRIdentifyWinRCFile( const char * );

bool WRAPI WRIs32Bit( WRFileType ftype )
{
    if( ftype == WR_WINNTM_RES || ftype == WR_WINNTW_RES ||
        ftype == WR_WINNT_EXE || ftype == WR_WINNT_DLL ) {
        return( true );
    }
    return( false );
}

WRFileType WRAPI WRIdentifyFile( const char *fname )
{
    WRFileType  ftype;
    char        ext[_MAX_EXT];
    bool        ok;

    ok = ( fname != NULL );
    if( ok ) {
        _splitpath( fname, NULL, NULL, NULL, ext );
        if( stricmp( ext, ".bmp" ) == 0 ) {
            ftype = WRIdentifyWinBMPFile( fname );
        } else if( stricmp( ext, ".cur" ) == 0 ) {
            ftype = WRIdentifyWinICOFile( fname );
        } else if( stricmp( ext, ".ico" ) == 0 ) {
            ftype = WRIdentifyWinICOFile( fname );
        } else if( stricmp( ext, ".dlg" ) == 0 ) {
            //ftype = WRIdentifyWinRCFile( fname );
            ftype = WR_WIN_RC_DLG;
        } else if( stricmp( ext, ".rc" ) == 0 ) {
            ftype = WRIdentifyWinRCFile( fname );
        } else if( stricmp( ext, ".str" ) == 0 ) {
            ftype = WR_WIN_RC_STR;
        } else if( stricmp( ext, ".mnu" ) == 0 ) {
            ftype = WR_WIN_RC_MENU;
        } else if( stricmp( ext, ".acc" ) == 0 ) {
            ftype = WR_WIN_RC_ACCEL;
        } else if( stricmp( ext, ".res" ) == 0 ) {
            ftype = WRIdentifyRESFile( fname );
        } else if( stricmp( ext, ".exe" ) == 0 ) {
            ftype = WRIdentifyEXEFile( fname, false );
        } else if( stricmp( ext, ".dll" ) == 0 ) {
            ftype = WRIdentifyEXEFile( fname, true );
        } else {
            ok = false;
        }
    }

    if( ok ) {
        return( ftype );
    } else {
        return( WR_INVALID_FILE );
    }
}

WRFileType WRIdentifyRESFile( const char *file )
{
    WRFileType  ftype;
    WRInfo      info;
    bool        is_wres;
    bool        ok;

    memset( &info, 0, sizeof( WRInfo ) );

    info.file_name = (char *)file;

    ok = WRLoadResDirFromRES( &info, &is_wres );

    if( ok ) {
        switch( WResGetTargetOS( info.dir ) ) {
        case WRES_OS_WIN16:
            if( is_wres ) {
                ftype = WR_WIN16W_RES;
            } else {
                ftype = WR_WIN16M_RES;
            }
            break;
        case WRES_OS_WIN32:
            if( is_wres ) {
                ftype = WR_WINNTW_RES;
            } else {
                ftype = WR_WINNTM_RES;
            }
            break;
        default:
            ftype = WR_INVALID_FILE;
            break;
        }
        if( info.dir != NULL ) {
            WResFreeDir( info.dir );
        }
    }

    if( ok ) {
        return( ftype );
    } else {
        return( WR_INVALID_FILE );
    }
}

WRFileType WRIdentifyEXEFile( const char *fname, bool is_dll )
{
    os2_exe_header  os2_hdr;
    exe_pe_header   pe_hdr;
    WRFileType      ftype;
    WResFileID      fid;

    ftype = WR_INVALID_FILE;

    fid = ResOpenFileRO( fname );
    if ( fid != WRES_NIL_HANDLE ) {
        if( WRReadWin16ExeHeader( fid, &os2_hdr ) != 0 ) {
            if( is_dll ) {
                ftype = WR_WIN16_DLL;
            } else {
                ftype = WR_WIN16_EXE;
            }
        } else {
            if( WRReadWinNTExeHeader( fid, &pe_hdr ) != 0 ) {
                if( is_dll ) {
                    ftype = WR_WINNT_DLL;
                } else {
                    ftype = WR_WINNT_EXE;
                }
            }
        }
        ResCloseFile( fid );
    }

    return( ftype );
}

WRFileType WRIdentifyWinBMPFile( const char *file_name )
{
    WRFileType          ftype;
    FILE                *fp;
    BITMAPFILEHEADER    file_header;
    BOOL                core;
    DWORD               size;

    fp = fopen( file_name, "rb" );
    if( fp == NULL ) {
        return( WR_INVALID_FILE );
    }

    ftype = WR_INVALID_FILE;
    file_header.bfType = 0;
    fread( &file_header, sizeof( BITMAPFILEHEADER ), 1, fp );
    if( file_header.bfType != BITMAP_TYPE ) {
        fclose( fp );
        return( ftype );
    }

    fread( &size, sizeof( size ), 1, fp );
    core = (size == sizeof( BITMAPCOREHEADER ));

    if( !core ) {
        ftype = WR_WIN_BITMAP;
    }

    fclose( fp );

    return( ftype );
}

WRFileType WRIdentifyWinICOFile( const char *file_name )
{
    WRFileType          ftype;
    FILE                *fp;

    fp = fopen( file_name, "rb" );
    if( fp == NULL ) {
        return( WR_INVALID_FILE );
    }

    ftype = IDIconOrCursor( fp );

    fclose( fp );

    return( ftype );
}

WRFileType IDIconOrCursor( FILE *fp )
{
    WRFileType          ftype;
    WORD                w;

    ftype = WR_INVALID_FILE;

    fseek( fp, 0L, SEEK_SET );

    fread( &w, sizeof( w ), 1, fp );
    fread( &w, sizeof( w ), 1, fp );

    switch( w ) {
    case ICON_FILE_TYPE:
        ftype = WR_WIN_ICON;
        break;
    case CURSOR_FILE_TYPE:
        ftype = WR_WIN_CURSOR;
        break;
    }

    return( ftype );
}

WRFileType WRIdentifyWinRCFile( const char *file_name )
{
    _wtouch( file_name );
    return( WR_WIN_RC );
}
