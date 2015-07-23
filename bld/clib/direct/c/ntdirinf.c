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


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
#include "libwin32.h"
#include "ntext.h"

void __GetNTDirInfo(DIR_TYPE *dirp, LPWIN32_FIND_DATA ffb )
{
    __MakeDOSDT( &ffb->ftLastWriteTime, &dirp->d_date, &dirp->d_time );
    dirp->d_attr = ffb->dwFileAttributes;
    dirp->d_size = ffb->nFileSizeLow;
    __F_NAME(strncpy,wcsncpy)( dirp->d_name, ffb->cFileName, NAME_MAX );
    dirp->d_name[NAME_MAX] = 0;
}

BOOL __NTFindNextFileWithAttr( HANDLE h, unsigned attr, LPWIN32_FIND_DATA ffb )
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
        if( !__lib_FindNextFile( h, ffb ) ) {
            return( FALSE );
        }
    }
}
