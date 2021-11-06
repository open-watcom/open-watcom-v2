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


#include "wrglbl.h"
#include "wrrdres.h"
#include "wrrdw16.h"
#include "wrrdwnt.h"
#include "exedos.h"
#include "pathgrp2.h"

#include "clibext.h"


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
    pgroup2     pg;
    bool        ok;

    ok = ( fname != NULL );
    if( ok ) {
        _splitpath2( fname, pg.buffer, NULL, NULL, NULL, &pg.ext );
        if( CMPFEXT( pg.ext, "bmp" ) ) {
            ftype = WRIdentifyWinBMPFile( fname );
        } else if( CMPFEXT( pg.ext, "cur" ) ) {
            ftype = WRIdentifyWinICOFile( fname );
        } else if( CMPFEXT( pg.ext, "ico" ) ) {
            ftype = WRIdentifyWinICOFile( fname );
        } else if( CMPFEXT( pg.ext, "dlg" ) ) {
            //ftype = WRIdentifyWinRCFile( fname );
            ftype = WR_WIN_RC_DLG;
        } else if( CMPFEXT( pg.ext, "rc" ) ) {
            ftype = WRIdentifyWinRCFile( fname );
        } else if( CMPFEXT( pg.ext, "str" ) ) {
            ftype = WR_WIN_RC_STR;
        } else if( CMPFEXT( pg.ext, "mnu" ) ) {
            ftype = WR_WIN_RC_MENU;
        } else if( CMPFEXT( pg.ext, "acc" ) ) {
            ftype = WR_WIN_RC_ACCEL;
        } else if( CMPFEXT( pg.ext, "res" ) ) {
            ftype = WRIdentifyRESFile( fname );
        } else if( CMPFEXT( pg.ext, "exe" ) ) {
            ftype = WRIdentifyEXEFile( fname, false );
        } else if( CMPFEXT( pg.ext, "dll" ) ) {
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

static bool IdentifyWinExeHeader( FILE *fh, bool win16 )
{
    os2_exe_header  os2_hdr;
    exe_pe_header   pe_hdr;
    uint_16         offset;
    bool            ok;

    ok = ( fh != NULL );

    if( ok ) {
        ok = ( fseek( fh, 0x18, SEEK_SET ) == 0 );
    }

    /* check the reloc offset */
    if( ok ) {
        ok = ( fread( &offset, 1, sizeof( offset ), fh ) == sizeof( offset ) && offset >= 0x0040 );
    }

    if( ok ) {
        ok = ( fseek( fh, NH_OFFSET, SEEK_SET ) == 0 );
    }

    /* check the header offset */
    if( ok ) {
        ok = ( fread( &offset, 1, sizeof( offset ), fh ) == sizeof( offset ) && offset != 0 );
    }

    /* seek to the header */
    if( ok ) {
        ok = ( fseek( fh, offset, SEEK_SET ) == 0 );
    }

    if( ok ) {
        if( win16 ) {
            ok = ( fread( &os2_hdr, 1, sizeof( os2_hdr ), fh ) == sizeof( os2_hdr ));
            /* check for valid Win16 EXE */
            if( ok ) {
                return( WRIsHeaderValidWIN16( &os2_hdr ) );
            }
        } else {
            ok = ( fread( &PE32( pe_hdr ), 1, sizeof( pe_header ), fh ) == sizeof( pe_header ) );
            if( ok && IS_PE64( pe_hdr ) ) {
                /* seek to the header again */
                ok = ( fseek( fh, offset, SEEK_SET ) == 0 );
                if( ok ) {
                    ok = ( fread( &PE64( pe_hdr ), 1, sizeof( pe_header64 ), fh ) == sizeof( pe_header64 ) );
                }
            }
            /* check for valid Win32 EXE */
            if( ok ) {
                return( WRIsHeaderValidWINNT( &pe_hdr ) );
            }
        }
    }

    return( false );
}

WRFileType WRIdentifyEXEFile( const char *fname, bool is_dll )
{
    WRFileType      ftype;
    FILE            *fh;

    ftype = WR_INVALID_FILE;

    fh = fopen( fname, "rb" );
    if( fh != NULL ) {
        if( IdentifyWinExeHeader( fh, true ) ) {
            if( is_dll ) {
                ftype = WR_WIN16_DLL;
            } else {
                ftype = WR_WIN16_EXE;
            }
        } else {
            if( IdentifyWinExeHeader( fh, false ) ) {
                if( is_dll ) {
                    ftype = WR_WINNT_DLL;
                } else {
                    ftype = WR_WINNT_EXE;
                }
            }
        }
        fclose( fh );
    }

    return( ftype );
}

WRFileType WRIdentifyWinBMPFile( const char *file_name )
{
    WRFileType          ftype;
    FILE                *fp;
    BITMAPFILEHEADER    file_header;
    bool                is_core;
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
    is_core = ( size == sizeof( BITMAPCOREHEADER ) );
    if( !is_core ) {
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
    /* unused parameters */ (void)file_name;

    return( WR_WIN_RC );
}
