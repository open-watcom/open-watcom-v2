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
#ifdef __NT__
    #include <windows.h>
#endif
/* gross hack for building 11.0 libraries with 10.6 compiler */
#ifndef __WATCOM_INT64__
    #include <limits.h>         /* a gross hack to make a gross hack work */
    #define __WATCOM_INT64__
    #define __int64             double
#endif
/* most includes should go after this line */
#include <io.h>
#include <string.h>
#include "find.h"
#include "seterrno.h"
#ifdef __NT__
    #include "libwin32.h"
    #include "ntex.h"
#else
    #include <dos.h>
    #include "liballoc.h"
#endif


#ifdef __WIDECHAR__
 #ifdef __INT64__
  _WCRTLINK long _wfindfirsti64( const wchar_t *filespec, struct _wfinddatai64_t *fileinfo )
 #else
  _WCRTLINK long _wfindfirst( const wchar_t *filespec, struct _wfinddata_t *fileinfo )
 #endif
#else
 #ifdef __INT64__
  _WCRTLINK long _findfirsti64( const char *filespec, struct _finddatai64_t *fileinfo )
 #else
  _WCRTLINK long _findfirst( const char *filespec, struct _finddata_t *fileinfo )
 #endif
#endif
{
    #ifdef __NT__
        WIN32_FIND_DATA ffb;
        HANDLE          h;

        /*** Initialize the find ***/
        h = __F_NAME(FindFirstFileA,__lib_FindFirstFileW)( filespec, &ffb );
        if( h == INVALID_HANDLE_VALUE ) {
            __set_errno_nt();
            return( -1 );
        }

        /*** Look for the first file ***/
        if( !__NTFindNextFileWithAttr( h, FIND_ATTR, &ffb ) ) {
            FindClose( h );
            __set_errno_dos( ERROR_FILE_NOT_FOUND );
            return( -1 );
        }

        /*** Got one! ***/
        #ifdef __INT64__
            __F_NAME(__nt_finddatai64_cvt,__nt_wfinddatai64_cvt)( &ffb, fileinfo );
        #else
            __F_NAME(__nt_finddata_cvt,__nt_wfinddata_cvt)( &ffb, fileinfo );
        #endif
        return( (long)h );
    #else
        DOSFINDTYPE *   findbuf;
        unsigned        rc;

        /*** Start a new find using _dos_findfirst ***/
        findbuf = (DOSFINDTYPE*) lib_malloc( sizeof( DOSFINDTYPE ) );
        if( findbuf == NULL )  return( -1L );
        rc = __F_NAME(_dos_findfirst,_wdos_findfirst)( filespec, FIND_ATTR,
                                                       findbuf );
        if( rc != 0 ) {
            lib_free( findbuf );
            return( -1L );
        }

        /*** Got one! ***/
        #ifdef __INT64__
            __F_NAME(__dos_finddatai64_cvt,__dos_wfinddatai64_cvt)( findbuf,
                                                                    fileinfo );
        #else
            __F_NAME(__dos_finddata_cvt,__dos_wfinddata_cvt)( findbuf,
                                                              fileinfo );
        #endif
        return( (long) findbuf );
    #endif
}


#ifdef __NT__


#ifdef __WIDECHAR__
 #ifdef __INT64__
  void __nt_wfinddatai64_cvt( WIN32_FIND_DATA *ffb, struct _wfinddatai64_t *fileinfo )
 #else
  void __nt_wfinddata_cvt( WIN32_FIND_DATA *ffb, struct _wfinddata_t *fileinfo )
 #endif
#else
 #ifdef __INT64__
  void __nt_finddatai64_cvt( WIN32_FIND_DATA *ffb, struct _finddatai64_t *fileinfo )
 #else
  void __nt_finddata_cvt( WIN32_FIND_DATA *ffb, struct _finddata_t *fileinfo )
 #endif
#endif
{
    /*** Convert attributes ***/
    fileinfo->attrib = 0;
    if( ffb->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE ) {
        fileinfo->attrib |= _A_ARCH;
    }
    if( ffb->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
        fileinfo->attrib |= _A_SUBDIR;
    }
    if( ffb->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) {
        fileinfo->attrib |= _A_HIDDEN;
    }
    if( ffb->dwFileAttributes & FILE_ATTRIBUTE_NORMAL ) {
        fileinfo->attrib |= _A_NORMAL;
    }
    if( ffb->dwFileAttributes & FILE_ATTRIBUTE_READONLY ) {
        fileinfo->attrib |= _A_RDONLY;
    }
    if( ffb->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) {
        fileinfo->attrib |= _A_SYSTEM;
    }

    /*** Handle the timestamps ***/
    fileinfo->time_create = __nt_filetime_cvt( &ffb->ftCreationTime );
    fileinfo->time_access = __nt_filetime_cvt( &ffb->ftLastAccessTime );
    fileinfo->time_write = __nt_filetime_cvt( &ffb->ftLastWriteTime );

    /*** Handle the file size ***/
    #ifdef __INT64__
    {
        INT_TYPE        tmp;

        MAKE_INT64(tmp,ffb->nFileSizeHigh,ffb->nFileSizeLow);
        fileinfo->size = GET_REALINT64(tmp);
    }
    #else
        fileinfo->size = ffb->nFileSizeLow;
    #endif

    /*** Handle the file name ***/
    __F_NAME(strcpy,wcscpy)( fileinfo->name, ffb->cFileName );
}


#else   /* __NT__ */


#ifdef __WIDECHAR__
 #ifdef __INT64__
  void __dos_wfinddatai64_cvt( struct _wfind_t *findbuf, struct _wfinddatai64_t *fileinfo )
 #else
  void __dos_wfinddata_cvt( struct _wfind_t *findbuf, struct _wfinddata_t *fileinfo )
 #endif
#else
 #ifdef __INT64__
  void __dos_finddatai64_cvt( struct find_t *findbuf, struct _finddatai64_t *fileinfo )
 #else
  void __dos_finddata_cvt( struct find_t *findbuf, struct _finddata_t *fileinfo )
 #endif
#endif
{
    fileinfo->attrib = findbuf->attrib;
    fileinfo->time_create = -1L;
    fileinfo->time_access = -1L;
    fileinfo->time_write = __dos_filetime_cvt( findbuf->wr_time,
                                               findbuf->wr_date );
    #ifdef __INT64__
    {
        INT_TYPE        tmp;

        _clib_U32ToU64(findbuf->size,tmp);
        fileinfo->size = GET_REALINT64(tmp);
    }
    #else
        fileinfo->size = (_fsize_t) findbuf->size;
    #endif
    __F_NAME(strcpy,wcscpy)( fileinfo->name, findbuf->name );
}


#endif  /* __NT__ */
