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


#ifndef _FINDdotH
#define _FINDdotH

#include <io.h>
#ifdef __NT__
    #include <windows.h>
#else
    #include <dos.h>
#endif


#define FIND_ATTR       (_A_NORMAL | _A_SUBDIR | _A_ARCH | _A_HIDDEN |  \
                         _A_RDONLY | _A_SYSTEM )

// prototypes here since not in 10.x headers

_WCRTLINK extern long _findfirst( const char *__filespec,
                                  struct _finddata_t *__fileinfo );
_WCRTLINK extern long _wfindfirst( const wchar_t *__filespec,
                                   struct _wfinddata_t *__fileinfo );
_WCRTLINK extern long _findfirsti64( const char *__filespec,
                                   struct _finddatai64_t *__fileinfo );
_WCRTLINK extern long _wfindfirsti64( const wchar_t *__filespec,
                                    struct _wfinddatai64_t *__fileinfo );

#ifdef __NT__

    extern void     __nt_finddata_cvt( WIN32_FIND_DATA *ffb,
                                       struct _finddata_t *fileinfo );
    extern void     __nt_wfinddata_cvt( WIN32_FIND_DATA *ffb,
                                        struct _wfinddata_t *fileinfo );
    extern void     __nt_finddatai64_cvt( WIN32_FIND_DATA *ffb,
                                          struct _finddatai64_t *fileinfo );
    extern void     __nt_wfinddatai64_cvt( WIN32_FIND_DATA *ffb,
                                           struct _wfinddatai64_t *fileinfo );
    extern time_t   __nt_filetime_cvt( FILETIME *ft );

#else

    #ifdef __WIDECHAR__
        #define DOSFINDTYPE     struct _wfind_t
    #else
        #define DOSFINDTYPE     struct find_t
    #endif

    extern void     __dos_finddata_cvt( struct find_t *findbuf,
                                        struct _finddata_t *fileinfo );
    extern void     __dos_wfinddata_cvt( struct _wfind_t *findbuf,
                                         struct _wfinddata_t *fileinfo );
    extern void     __dos_finddatai64_cvt( struct find_t *findbuf,
                                           struct _finddatai64_t *fileinfo );
    extern void     __dos_wfinddatai64_cvt( struct _wfind_t *findbuf,
                                            struct _wfinddatai64_t *fileinfo );
    extern time_t   __dos_filetime_cvt( unsigned short ftime,
                                        unsigned short fdate );

#endif


#endif
