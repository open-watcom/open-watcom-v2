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
#include "int64.h"
#include <stdio.h>
#include <windows.h>
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"

/*
    DWORD SetFilePointer(
      HANDLE hFile,                // handle to file
      LONG lDistanceToMove,        // bytes to move pointer
      PLONG lpDistanceToMoveHigh,  // bytes to move pointer
      DWORD dwMoveMethod           // starting point
    );
 */

#define INVALID_SET_FILE_POINTER 0xFFFFFFFF

#ifdef __INT64__
 _WCRTLINK __int64 _telli64( int hid )
#else
 _WCRTLINK long int tell( int hid )
#endif
{
    DWORD               pos;
#ifdef __INT64__
    LONG                highorder = 0;
    INT_TYPE            minusone;
    INT_TYPE            retval;
    int                 error;
#endif

    __handle_check( hid, -1 );

#ifdef __INT64__
    pos = SetFilePointer( __getOSHandle( hid ), 0, &highorder, FILE_CURRENT );
    if( pos == INVALID_SET_FILE_POINTER ) { // this might be OK so
        error = GetLastError();             // check for sure JBS 04-nov-99
        if( error != NO_ERROR ) {
            __set_errno_dos( error );
            MAKE_INT64(minusone,-1,-1);
            RETURN_INT64(minusone);
        }
    }
    MAKE_INT64(retval,highorder,pos);
    RETURN_INT64(retval);
#else
    pos = SetFilePointer( __getOSHandle( hid ), 0, 0, FILE_CURRENT );
    if( pos == INVALID_SET_FILE_POINTER ) {
        return( __set_errno_nt() );
    }
    return( pos );
#endif
}
