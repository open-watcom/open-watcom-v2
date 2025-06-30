/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(__NT__)
    #include <windows.h>
#elif defined(__OS2__)
    #include <wos2.h>
#endif
#include "rterrno.h"
#include "fileacc.h"
#include "iomode.h"
#include "seterrno.h"
#include "thread.h"


_WCRTLINK int _pipe( int *phandles, unsigned psize, int textmode )
/****************************************************************/
{
#if defined(__NT__)
    HANDLE              osfh_read;
    HANDLE              osfh_write;
//  HANDLE              osHandle;
    SECURITY_ATTRIBUTES sa;
#elif defined(__OS2__)
    HFILE               osfh_read;
    HFILE               osfh_write;
    APIRET              rc;
#endif
    int                 handle_read;
    int                 handle_write;

// removed by JBS - allow O_NOINHERIT
//  /*** Sanity check ***/
//  if( textmode != 0  &&  textmode != _O_TEXT  &&  textmode != _O_BINARY ) {
//      return( -1 );
//  }

    /*** Create the pipes (note that psize==0 ==> use default size) ***/
#if defined(__NT__)
    sa.nLength = sizeof( SECURITY_ATTRIBUTES );
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = (((textmode & O_NOINHERIT)==O_NOINHERIT)?FALSE:TRUE);
    if( CreatePipe( &osfh_read, &osfh_write, &sa, psize ) == 0 ) {
        return( __set_errno_nt() );
    }
#elif defined(__OS2__)
    if( psize == 0 )
        psize = 4096;
  #ifdef _M_I86
    rc = DosMakePipe( &osfh_read, &osfh_write, psize );
  #else
    rc = DosCreatePipe( &osfh_read, &osfh_write, psize );
  #endif
    if( rc != NO_ERROR ) {
        _RWD_errno = ENOMEM;
        return( -1 );
    }
#endif

// removed by JBS - used sa struct instead
//    /*** Make read handle inheritable ***/
//    #ifdef __NT__
//        rc = DuplicateHandle( GetCurrentProcess(), osfh_read, GetCurrentProcess(),
//                              &osHandle, 0, TRUE, DUPLICATE_SAME_ACCESS );
//        if( rc == FALSE ) {
//            CloseHandle( osfh_read );
//            CloseHandle( osfh_write );
//            return( -1 );
//        }
//        CloseHandle( osfh_read );
//        osfh_read = osHandle;
//    #elif defined(__OS2__)
//        /* Handle is inheritable by default */
//    #endif
//
//    /*** Make write handle inheritable ***/
//    #ifdef __NT__
//        rc = DuplicateHandle( GetCurrentProcess(), osfh_write, GetCurrentProcess(),
//                              &osHandle, 0, TRUE, DUPLICATE_SAME_ACCESS );
//        if( rc == FALSE ) {
//            CloseHandle( osfh_read );
//            CloseHandle( osfh_write );
//            return( -1 );
//        }
//        CloseHandle( osfh_write );
//        osfh_write = osHandle;
//    #elif defined(__OS2__)
//        /* Handle is inheritable by default */
//    #endif

    /*** Initialize the POSIX-level handles ***/
    handle_read = _hdopen( (int)osfh_read, textmode | _O_RDONLY );
    handle_write = _hdopen( (int)osfh_write, textmode | _O_WRONLY );
    if( handle_read == -1
      || handle_write == -1 ) {
        if( handle_read != -1 ) {
            close( handle_read );
        } else {
#if defined(__NT__)
            CloseHandle( osfh_read );
#elif defined(__OS2__)
            DosClose( osfh_read );
#endif
        }
        if( handle_write != -1 ) {
            close( handle_write );
        } else {
#if defined(__NT__)
            CloseHandle( osfh_write );
#elif defined(__OS2__)
            DosClose( osfh_write );
#endif
        }
        return( -1 );
    }

    /*** Store the new POSIX handles in return buffer ***/
    phandles[0] = handle_read;
    phandles[1] = handle_write;
    return( 0 );
}
