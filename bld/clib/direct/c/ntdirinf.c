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


#include "variety.h"
#include "widechar.h"
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
#include "libwin32.h"
#include "ntex.h"

#ifndef __WIDECHAR__    /* same for wide char, so only compile this once */
void __MakeDOSDT( FILETIME *NT_stamp, unsigned short *d, unsigned short *t )
{
    FILETIME local_ft;

    FileTimeToLocalFileTime( NT_stamp, &local_ft );
    FileTimeToDosDateTime( &local_ft, d, t );
}

void __FromDOSDT( unsigned short d, unsigned short t, FILETIME *NT_stamp )
{
    FILETIME local_ft;

    DosDateTimeToFileTime( d, t, &local_ft );
    LocalFileTimeToFileTime( &local_ft, NT_stamp );
}
#endif

void __F_NAME(__GetNTDirInfo,__wGetNTDirInfo)(DIR_TYPE *dirp, LPWIN32_FIND_DATA ffb )
{
    __MakeDOSDT( &ffb->ftLastWriteTime, &dirp->d_date, &dirp->d_time );
    dirp->d_attr = ffb->dwFileAttributes;
    dirp->d_size = ffb->nFileSizeLow;
#ifndef __WIDECHAR__
    strncpy( dirp->d_name, ffb->cFileName, NAME_MAX );
#else
    wcsncpy( dirp->d_name, ffb->cFileName, NAME_MAX );
#endif
    dirp->d_name[NAME_MAX] = 0;
}

BOOL __F_NAME(__NTFindNextFileWithAttr,__wNTFindNextFileWithAttr)(
                HANDLE h, DWORD attr, LPWIN32_FIND_DATA ffb )
{
    for(;;) {
        if( ffb->dwFileAttributes == 0 ) {
            // Win95 seems to return 0 for the attributes sometimes?
            // In that case, treat as a normal file
            ffb->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
        }
        if( (attr & _A_HIDDEN) || (ffb->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0 ) {
            if( (attr & _A_SYSTEM) || (ffb->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) == 0 ) {
                if( (attr & _A_SUBDIR) || (ffb->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )  {
                    return ( TRUE );
                }
            }
        }
#ifdef __WIDECHAR__
        if( !__lib_FindNextFileW( h, ffb ) ) {
            return( FALSE );
        }
#else
        if( !FindNextFileA( h, ffb ) ) {
            return( FALSE );
        }
#endif
    }
}
