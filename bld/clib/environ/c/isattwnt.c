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
* Description:  Implementation of isatty() for Win32.
*
****************************************************************************/


#include "variety.h"
#include <unistd.h>
#include <windows.h>
#include "defwin.h"
#include "iomode.h"
#include "fileacc.h"

/*
    DWORD GetFileType(
      HANDLE hFile   // handle to file
    );
    Return value can be:
    FILE_TYPE_CHAR  The specified file is a character file,
                    typically an LPT device or a console
 */

_WCRTLINK int isatty( int hid )
{
#ifdef DEFAULT_WINDOWING
    if( _WindowsIsWindowedHandle != 0 ) {
        if( _WindowsIsWindowedHandle( hid ) ) {
            return( 1 );
        }
    }
#endif
    if( GetFileType( __getOSHandle( hid ) ) == FILE_TYPE_CHAR ) {
        return( 1 );
    }
    return( 0 );
}
