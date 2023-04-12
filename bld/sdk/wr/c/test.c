/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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

#include "clibint.h"


int PASCAL WinMain( HINSTANCE hinstCurrent, HINSTANCE hinstPrevious,
                    LPSTR lpszCmdLine, int nCmdShow )
{
    int                 ret;
    int                 i;
    int                 ltype;
    int                 stype;
    bool                backup;
    WRInfo              *info;
    WRFileType          ftype;
    uint_16             mflags;
    WRSelectImageInfo   *siinfo;
#if 0
    bool                dup;
    char                *data;
    WResID              type;
    WResID              name;
#endif

    hinstCurrent = hinstCurrent;
    hinstPrevious = hinstPrevious;
    lpszCmdLine = lpszCmdLine;
    nCmdShow = nCmdShow;

    info = NULL;
    ret = TRUE;

    WRInit();
    if( _argc == 6 ) {
        ltype = atoi( _argv[2] );
        stype = atoi( _argv[4] );
        backup = (atoi( _argv[5] ) != 0);
        if( stype != 0 ) {
            info = WRLoadResource( _argv[1], ltype );
            if( info != NULL ) {
                info->save_name = _argv[3];
                info->save_type = stype;
                ret = WRSaveResource( info, backup );
            } else {
                ret = FALSE;
            }
        }
#if 0
    } else if( _argc == 2 ) {
        ret = FALSE;
        data = WRMemAlloc( 32 );
        if( data != NULL ) {
            for( i = 0; i < 32; i++ ) {
                data[i] = i;
            }
            type.IsName = name.IsName = false;
            type.ID.Num = 10;
            name.ID.Num = 200;
            ret = WRSaveObjectAs( _argv[1], WR_WIN16M_RES, &type, &name, data, 32, 0 );
            WRMemFree( data );
        }
    } else if( _argc == 3 ) {
        ret = FALSE;
        data = WRMemAlloc( 32 );
        if( data != NULL ) {
            for( i = 0; i < 32; i++ ) {
                data[i] = i;
            }
            type.IsName = name.IsName = false;
            type.ID.Num = 10;
            name.ID.Num = 100;
            ret = WRSaveObjectInto( _argv[1], &type, &name, data, 32, 0, &dup );
            WRMemFree( data );
        }
#endif
    } else {
        for( i = 1; i < _argc; i++ ) {
            WRSelectFileType( HWND_DESKTOP, _argv[i], false, (i % 2) != 0, NULL );
            ftype = WRIdentifyFile( _argv[i] );
        }
        ret = TRUE;
    }

    WRChangeMemFlags( HWND_DESKTOP, "test", &mflags, NULL );

    if( info != NULL ) {
        info->save_name = NULL;
        WRFreeWRInfo( info );
    }

    info = WRLoadResource( _argv[1], WR_DONT_KNOW );
    siinfo = WRSelectImage( HWND_DESKTOP, info, NULL );
    if( siinfo != NULL ) {
        WRFreeSelectImageInfo( siinfo );
    }
    WRFreeWRInfo( info );

    if( !ret ) {
        MessageBox( (HWND)NULL, "Error occurred!", "TEST WR",
                    MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL );
    }

    WRFini();

    return( ret );
}
