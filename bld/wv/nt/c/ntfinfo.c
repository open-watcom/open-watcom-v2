/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "dbgdefn.h"
#include "dbgmem.h"
#include "dbgio.h"
#include "trptypes.h"
#include "filelcl.h"

long LocalFileGetDate( const char *name )
/***************************************/
{
    WIN32_FIND_DATA     ffd;
    HANDLE              h;
    WORD                md,mt;

    h = FindFirstFile( name, &ffd );
    if( h == INVALID_HANDLE_VALUE ) {
        return( -1 );
    }
    FindClose( h );
    FileTimeToDosDateTime( &ffd.ftLastWriteTime, &md, &mt );
    return( ( md << 16 ) + mt );
}

bool LocalFileSetDate( const char *name, long date )
/**************************************************/
{
    HANDLE              h;
    WORD                md,mt;
    FILETIME            ft;

    md = ( date >> 16 ) & 0xFFFF;
    mt = date;
    DosDateTimeToFileTime( md, mt, &ft );
    h = CreateFile( name, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                    OPEN_EXISTING, 0, NULL );
    if( h == INVALID_HANDLE_VALUE ) {
        return( false );
    }
    if( !SetFileTime( h, &ft, &ft, &ft ) ) {
        return( false );
    }
    CloseHandle( h );
    return( true );
}
