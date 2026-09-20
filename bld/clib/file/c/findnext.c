/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2026 The Open Watcom Contributors. All Rights Reserved.
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
#include "seterrno.h"
#include <io.h>
#if defined( __NT__ )
    #include <windows.h>
    #include "libwin32.h"
    #include "ntext.h"
#elif defined( __OS2__ )
    #include <wos2.h>
#elif defined( __RDOS__ )
    #include <rdos.h>
#else
    #include <dos.h>
#endif
#include "find.h"
#include "filei64.h"


_WCRTLINK int __64_NAME(__F_NAME(_findnext,_wfindnext),__F_NAME(_findnexti64,_wfindnexti64))(
    intptr_t osffh,
    struct __64_NAME(__F_NAME(_finddata_t,_wfinddata_t),__F_NAME(_finddatai64_t,_wfinddatai64_t)) *fileinfo )
{
#ifdef __NT__
    WIN32_FIND_DATA ffd;

    /*** Try to find another matching file ***/
    if( __lib_FindNextFile( (HANDLE)osffh, &ffd ) == 0 ) {
        return( __set_errno_nt() );
    }
    if( !__NTFindNextFileWithAttr( (HANDLE)osffh, NT_FIND_ATTR, &ffd ) ) {
        return( __set_errno_dos( ERROR_FILE_NOT_FOUND ) );
    }
    /*** Got one! ***/
    __64_NAME(__F_NAME(__nt_finddata_cvt,__nt_wfinddata_cvt),__F_NAME(__nt_finddatai64_cvt,__nt_wfinddatai64_cvt))( &ffd, fileinfo );
#elif defined( __OS2__ )
    APIRET          rc;
    FF_BUFFER       ffb;
    OS_UINT         searchcount = 1;

    rc = DosFindNext( (HDIR)osffh, &ffb, sizeof( ffb ), &searchcount );
    if( rc != 0 ) {
        return( __set_errno_dos( rc ) );
    }
    /*** Got one! ***/
    __64_NAME(__F_NAME(__os2_finddata_cvt,__os2_wfinddata_cvt),__F_NAME(__os2_finddatai64_cvt,__os2_wfinddatai64_cvt))( &ffb, fileinfo );

#elif defined( __RDOS__ )
    RDOSFINDTYPE    *findbuf;

    findbuf = (RDOSFINDTYPE *)osffh;
    findbuf->entry++;
    if( __rdos_finddata_get( findbuf, fileinfo ) ) {
        return( 0 );
    } else {
        return( -1 );
    }

#else   /* DOS */
    if( __F_NAME(_dos_findnext,_wdos_findnext)( (DOSFINDTYPE *)osffh ) ) {
        return( -1 );
    }
    __64_NAME(__F_NAME(__dos_finddata_cvt,__dos_wfinddata_cvt),__F_NAME(__dos_finddatai64_cvt,__dos_wfinddatai64_cvt))( (DOSFINDTYPE *)osffh, fileinfo );
#endif
    return( 0 );
}
