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
#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(__NT__)
    #include <windows.h>
#elif defined(__OS2__)
    #include <wos2.h>
#endif
#include "fileacc.h"
#include "iomode.h"
#include "rtdata.h"
#include "seterrno.h"


_WCRTLINK int _pipe( int *phandles, unsigned psize, int textmode )
/****************************************************************/
{
#if defined(__NT__)
    HANDLE              hRead, hWrite;
//  HANDLE              osHandle;               // removed by JBS
    BOOL                rc;
    SECURITY_ATTRIBUTES sa;
#elif defined(__OS2__) && defined(__386__)
    HFILE               hRead, hWrite;
    APIRET              rc;
#elif defined(__OS2__) && !defined(__386__)
    HFILE               hRead, hWrite;
    USHORT              rc;
#endif
    int                 hReadPosix, hWritePosix;

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
        rc = CreatePipe( &hRead, &hWrite, &sa, psize );
        if( rc == FALSE ) {
            return( __set_errno_nt() );
        }
    #elif defined(__OS2__)
        if( psize == 0 )  psize = 4096;
        #ifdef __386__
            rc = DosCreatePipe( &hRead, &hWrite, psize );
        #else
            rc = DosMakePipe( &hRead, &hWrite, psize );
        #endif
        if( rc != NO_ERROR ) {
            __set_errno( ENOMEM );
            return( -1 );
        }
    #endif

// removed by JBS - used sa struct instead
//    /*** Make read handle inheritable ***/
//    #ifdef __NT__
//        rc = DuplicateHandle( GetCurrentProcess(), hRead, GetCurrentProcess(),
//                              &osHandle, 0, TRUE, DUPLICATE_SAME_ACCESS );
//        if( rc == FALSE ) {
//            CloseHandle( hRead );
//            CloseHandle( hWrite );
//            return( -1 );
//        }
//        CloseHandle( hRead );
//        hRead = osHandle;
//    #elif defined(__OS2__)
//        /* Handle is inheritable by default */
//    #endif
//
//    /*** Make write handle inheritable ***/
//    #ifdef __NT__
//        rc = DuplicateHandle( GetCurrentProcess(), hWrite, GetCurrentProcess(),
//                              &osHandle, 0, TRUE, DUPLICATE_SAME_ACCESS );
//        if( rc == FALSE ) {
//            CloseHandle( hRead );
//            CloseHandle( hWrite );
//            return( -1 );
//        }
//        CloseHandle( hWrite );
//        hWrite = osHandle;
//    #elif defined(__OS2__)
//        /* Handle is inheritable by default */
//    #endif

    /*** Initialize the POSIX-level handles ***/
    hReadPosix = _hdopen( (int)hRead, textmode|_O_RDONLY );
    hWritePosix = _hdopen( (int)hWrite, textmode|_O_WRONLY );
    if( hReadPosix == -1  ||  hWritePosix == -1 ) {
        if( hReadPosix != -1 ) {
            close( hReadPosix );
        } else {
            #if defined(__NT__)
                CloseHandle( hRead );
            #elif defined(__OS2__)
                DosClose( hRead );
            #endif
        }
        if( hWritePosix != -1 ) {
            close( hWritePosix );
        } else {
            #if defined(__NT__)
                CloseHandle( hWrite );
            #elif defined(__OS2__)
                DosClose( hWrite );
            #endif
        }
        return( -1 );
    }

    /*** Store the new POSIX handles in return buffer ***/
    phandles[0] = hReadPosix;
    phandles[1] = hWritePosix;
    return( 0 );
}
