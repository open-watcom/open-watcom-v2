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
#include "widechar.h"
#include <io.h>
#include "find.h"
#include "seterrno.h"
#ifdef __NT__
    #include <windows.h>
    #include "libwin32.h"
    #include "ntex.h"
#else
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
        #ifdef __WIDECHAR__
            rc = __lib_FindNextFileW( (HANDLE)handle, &ffb );
        #else
            rc = FindNextFileA( (HANDLE)handle, &ffb );
        #endif
        if( rc == FALSE ) {
            __set_errno_nt();
            return( -1 );
        }
        if( !__NTFindNextFileWithAttr( (HANDLE)handle, FIND_ATTR, &ffb ) ) {
            __set_errno_dos( ERROR_FILE_NOT_FOUND );
            return( -1 );
        }

        /*** Got one! ***/
        #ifdef __INT64__
            __F_NAME(__nt_finddatai64_cvt,__nt_wfinddatai64_cvt)( &ffb, fileinfo );
        #else
            __F_NAME(__nt_finddata_cvt,__nt_wfinddata_cvt)( &ffb, fileinfo );
        #endif
        return( 0 );
    #else
        DOSFINDTYPE *   findbuf = (DOSFINDTYPE*) handle;
        unsigned        rc;

        rc = __F_NAME(_dos_findnext,_wdos_findnext)( findbuf );
        if( rc != 0 ) {
            return( -1L );
        } else {
            #ifdef __INT64__
                __F_NAME(__dos_finddatai64_cvt,__dos_wfinddatai64_cvt)( findbuf, fileinfo );
            #else
                __F_NAME(__dos_finddata_cvt,__dos_wfinddata_cvt)( findbuf, fileinfo );
            #endif
            return( 0 );
        }
    #endif
}
