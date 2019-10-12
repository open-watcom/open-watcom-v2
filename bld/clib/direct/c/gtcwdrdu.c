/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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


#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "liballoc.h"
#include <rdos.h>
#include "pathmac.h"


_WCRTLINK char *getcwd( char *buf, size_t size )
{
    int drive;
    char *p;
    char cwd[256];

    if( buf == NULL ) {
        size = sizeof( cwd );
        p = lib_malloc( size );
    } else {
        p = buf;
    }

    drive = RdosGetCurDrive();

    cwd[0] = drive + 'A';
    cwd[1] = DRV_SEP;
    cwd[2] = DIR_SEP;

    if( RdosGetCurDir( drive, &cwd[3] ) ) {
        return( strncpy( p, cwd, size ) );
    } else {
        return( NULL );
    }
}
