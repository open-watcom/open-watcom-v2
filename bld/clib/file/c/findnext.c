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
#include <io.h>
#if defined( __NT__ )
    #include <windows.h>
    #include "libwin32.h"
    #include "ntex.h"
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "find.h"
#include "seterrno.h"

#ifdef __NT__
  #define CHECK_FIND_NEXT_ATTR    __F_NAME(__NTFindNextFileWithAttr,__wNTFindNextFileWithAttr)
#endif


#ifdef __WIDECHAR__
 #ifdef __INT64__
  _WCRTLINK int _wfindnexti64( long handle, struct _wfinddatai64_t *fileinfo )
 #else
  _WCRTLINK int _wfindnext( long handle, struct _wfinddata_t *fileinfo )
 #endif
#else
 #ifdef __INT64__
  _WCRTLINK int _findnexti64( long handle, struct _finddatai64_t *fileinfo )
 #else
  _WCRTLINK int _findnext( long handle, struct _finddata_t *fileinfo )
 #endif
#endif
{
#ifdef __NT__
    WIN32_FIND_DATA ffb;
    BOOL            rc;

    /*** Try to find another matching file ***/
    rc = __lib_FindNextFile( (HANDLE)handle, &ffb );
    if( rc == FALSE ) {
        return( __set_errno_nt() );
    }
    if( !CHECK_FIND_NEXT_ATTR( (HANDLE)handle, FIND_ATTR, &ffb ) ) {
        return( __set_errno_dos( ERROR_FILE_NOT_FOUND ) );
    }
    /*** Got one! ***/
  #ifdef __INT64__
    __F_NAME(__nt_finddatai64_cvt,__nt_wfinddatai64_cvt)( &ffb, fileinfo );
  #else
    __F_NAME(__nt_finddata_cvt,__nt_wfinddata_cvt)( &ffb, fileinfo );
  #endif
#elif defined( __OS2__ )
    APIRET          rc;
    FF_BUFFER       ffb;
    OS_UINT         searchcount = 1;

    rc = DosFindNext( (HDIR)handle, &ffb, sizeof( ffb ), &searchcount );
    if( rc != 0 ) {
        return( __set_errno_dos( rc ) );
    }
    /*** Got one! ***/
  #ifdef __INT64__
    __F_NAME(__os2_finddatai64_cvt,__os2_wfinddatai64_cvt)( &ffb, fileinfo );
  #else
    __F_NAME(__os2_finddata_cvt,__os2_wfinddata_cvt)( &ffb, fileinfo );
  #endif

#elif defined( __RDOS__ )
    RDOSFINDTYPE *   findbuf = (RDOSFINDTYPE*) handle;

    findbuf->entry++;

    if( __rdos_finddata_get( findbuf, fileinfo ) )
        return( 0 );
    else
        return( -1 );

#else   /* DOS */
    if( __F_NAME(_dos_findnext,_wdos_findnext)( (DOSFINDTYPE *)handle ) ) {
        return( -1 );
    }
  #ifdef __INT64__
    __F_NAME(__dos_finddatai64_cvt,__dos_wfinddatai64_cvt)( (DOSFINDTYPE *)handle, fileinfo );
  #else
    __F_NAME(__dos_finddata_cvt,__dos_wfinddata_cvt)( (DOSFINDTYPE *)handle, fileinfo );
  #endif
#endif
    return( 0 );
}
