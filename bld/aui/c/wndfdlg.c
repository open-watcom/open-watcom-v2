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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"
#include <stdlib.h>
#include <string.h>
#ifdef UNIX
#include <clibext.h>
#endif

extern gui_window *DlgGetParent();

extern int DlgGetFileName( open_file_name *ofn )
{
    int         rc;
    a_window    *wnd;

    wnd = WndFindActive();
    rc = GUIGetFileName( DlgGetParent(), ofn );
    if( wnd != NULL ) WndToFront( wnd );
    return( rc );
}

bool DlgFileBrowse( char *title, char *filter,
                    char *path, unsigned len, unsigned long flags )
{
    open_file_name      ofn;
    char                buff[_MAX_PATH2];
    char                fname[_MAX_PATH];
    char                cd[_MAX_DRIVE+_MAX_PATH];
    char                *drive;
    char                *dir;
    char                *name;
    char                *ext;
    int                 rc;

    memset( &ofn, 0, sizeof( ofn ) );
    ofn.flags = flags;
    ofn.title = title;
    _splitpath2( path, buff, &drive, &dir, &name, &ext );
    _makepath( cd, drive, dir, ".", NULL );
    _makepath( fname, NULL, NULL, name, ext );
    ofn.initial_dir = cd;
    ofn.file_name = fname;
    ofn.max_file_name = len;
    ofn.filter_list = filter;
    ofn.filter_index = 0;
    rc = DlgGetFileName( &ofn );
    if( rc == OFN_RC_FILE_SELECTED ) {
        strcpy( path, fname );
        return( TRUE );
    }
    return( FALSE );
}
