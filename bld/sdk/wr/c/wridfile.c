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


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wrrdres.h"
#include "wrrdw16.h"
#include "wrrdwnt.h"
#include "wrinfo.h"
#include "wridfile.h"

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
static WRFileType WRIdentifyEXEFile( int, int );
static WRFileType WRIdentifyWinBMPFile( const char * );
static WRFileType WRIdentifyWinICOFile( const char * );
static WRFileType IDIconOrCursor( FILE * );
static WRFileType WRIdentifyWinRCFile( const char * );

int WR_EXPORT WRIs32Bit( WRFileType ftype )
{
    if( ftype == WR_WINNTM_RES || ftype == WR_WINNTW_RES ||
        ftype == WR_WINNT_EXE || ftype == WR_WINNT_DLL ) {
        return( TRUE );
    }
    return( FALSE );
}

WRFileType WR_EXPORT WRIdentifyFile( const char *file )
{
    WRFileType  ftype;
    char        ext[_MAX_EXT];
    int         fh;
    int         ok;

    fh = -1;

    ok = (file != NULL);

    if( ok ) {
        _splitpath( file, NULL, NULL, NULL, ext );
        if( !stricmp( ext, ".bmp" ) ) {
            ftype = WRIdentifyWinBMPFile( file );
        } else if( !stricmp( ext, ".cur" ) ) {
            ftype = WRIdentifyWinICOFile( file );
        } else if( !stricmp( ext, ".ico" ) ) {
            ftype = WRIdentifyWinICOFile( file );
        } else if( !stricmp( ext, ".dlg" ) ) {
            //ftype = WRIdentifyWinRCFile( file );
            ftype = WR_WIN_RC_DLG;
        } else if( !stricmp( ext, ".rc" ) ) {
            ftype = WRIdentifyWinRCFile( file );
        } else if( !stricmp( ext, ".str" ) ) {
            ftype = WR_WIN_RC_STR;
        } else if( !stricmp( ext, ".mnu" ) ) {
            ftype = WR_WIN_RC_MENU;
        } else if( !stricmp( ext, ".acc" ) ) {
            ftype = WR_WIN_RC_ACCEL;
        } else if( !stricmp( ext, ".res" ) ) {
            ftype = WRIdentifyRESFile( file );
        } else if( !stricmp( ext, ".exe" ) ) {
            ok = ((fh = ResOpenFileRO( file )) != -1);
            if( ok ) {
                ftype = WRIdentifyEXEFile( fh, FALSE );
            }
        } else if( !stricmp( ext, ".dll" ) ) {
            ok = ((fh = ResOpenFileRO( file )) != -1);
            if ( ok ) {
                ftype = WRIdentifyEXEFile( fh, TRUE );
            }
        } else {
            ok = FALSE;
        }
    }

    if( fh != -1 ) {
        ResCloseFile( fh );
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
    int         is_wres;
    int         ok;

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

WRFileType WRIdentifyEXEFile( int fh, int is_dll )
{
    os2_exe_header  os2_hdr;
    pe_header       pe_hdr;
    WRFileType      ftype;

    ftype = WR_INVALID_FILE;

    if( WRReadWin16ExeHeader( fh, &os2_hdr ) != 0 ) {
        if( is_dll ) {
            ftype = WR_WIN16_DLL;
        } else {
            ftype = WR_WIN16_EXE;
        }
    } else {
        if( WRReadWinNTExeHeader( fh, &pe_hdr ) != 0 ) {
            if( is_dll ) {
                ftype = WR_WINNT_DLL;
            } else {
                ftype = WR_WINNT_EXE;
            }
        }
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
