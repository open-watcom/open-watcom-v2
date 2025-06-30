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
#include "widechar.h"
#include <io.h>
#include <string.h>
#if defined( __OS2__ )
    #include <mbstring.h>
#endif
#ifdef __NT__
    #include <windows.h>
#elif defined( __OS2__ )
    #define INCL_LONGLONG
    #include <wos2.h>
    #include "os2fil64.h"
#elif defined( __RDOS__ )
    #include <rdos.h>
    #include <time.h>
#else
    #include <dos.h>
#endif
#ifdef __NT__
    #include "libwin32.h"
    #include "ntext.h"
#elif defined( __OS2__ )
    #include "d2ttime.h"
#elif defined( __RDOS__ )
    #include "liballoc.h"
#else
    #include "liballoc.h"
    #include "rtdata.h"
    #include "tinyio.h"
    #include "_doslfn.h"
    #include "_dtaxxx.h"
    #include "d2ttime.h"
#endif
#include "i64.h"
#include "find.h"
#include "seterrno.h"


#ifdef __WIDECHAR__
 #ifdef __INT64__
  _WCRTLINK intptr_t _wfindfirsti64( const wchar_t *filespec, struct _wfinddatai64_t *fileinfo )
 #else
  _WCRTLINK intptr_t _wfindfirst( const wchar_t *filespec, struct _wfinddata_t *fileinfo )
 #endif
#else
 #ifdef __INT64__
  _WCRTLINK intptr_t _findfirsti64( const char *filespec, struct _finddatai64_t *fileinfo )
 #else
  _WCRTLINK intptr_t _findfirst( const char *filespec, struct _finddata_t *fileinfo )
 #endif
#endif
/******************************************************************************/
{
#if defined( __NT__ )
    WIN32_FIND_DATA ffd;
    HANDLE          osffh;

    /*** Initialize the find ***/
    osffh = __lib_FindFirstFile( filespec, &ffd );
    if( osffh == INVALID_HANDLE_VALUE ) {
        return( __set_errno_nt() );
    }

    /*** Look for the first file ***/
    if( !__NTFindNextFileWithAttr( osffh, NT_FIND_ATTR, &ffd ) ) {
        FindClose( osffh );
        return( __set_errno_dos( ERROR_FILE_NOT_FOUND ) );
    }

    /*** Got one! ***/
  #ifdef __INT64__
    __F_NAME(__nt_finddatai64_cvt,__nt_wfinddatai64_cvt)( &ffd, fileinfo );
  #else
    __F_NAME(__nt_finddata_cvt,__nt_wfinddata_cvt)( &ffd, fileinfo );
  #endif
    return( (intptr_t)osffh );
#elif defined( __OS2__ )
    APIRET          rc;
    HDIR            h = HDIR_CREATE;
    FF_BUFFER       ffb;
    OS_UINT         searchcount = 1;
 #ifdef __WIDECHAR__
    char            mbFilespec[MB_CUR_MAX * _MAX_PATH];

    if( wcstombs( mbFilespec, filespec, sizeof( mbFilespec ) ) == (size_t)-1 ) {
        mbFilespec[0] = '\0';
    }
 #endif
    rc = DosFindFirst( (char*)__F_NAME(filespec,mbFilespec), &h, FIND_ATTR,
                                &ffb, sizeof( ffb ), &searchcount, FF_LEVEL );
    if( rc != 0 ) {
        return( -1 );
    }
    /*** Got one! ***/
 #ifdef __INT64__
    __F_NAME(__os2_finddatai64_cvt,__os2_wfinddatai64_cvt)( &ffb, fileinfo );
 #else
    __F_NAME(__os2_finddata_cvt,__os2_wfinddata_cvt)( &ffb, fileinfo );
 #endif
    return( (intptr_t)h );
#elif defined( __RDOS__ )
    RDOSFINDTYPE       *findbuf;
    struct RdosDirInfo dinf;

    findbuf = (RDOSFINDTYPE *)lib_malloc( sizeof( RDOSFINDTYPE ) );
    if( findbuf == NULL )
        return( -1 );

    findbuf->handle = RdosOpenDir( filespec, &dinf );
    findbuf->entry = 0;
    findbuf->count = dinf.Count;
    findbuf->header_size = dinf.HeaderSize;
    findbuf->chain = (char *)dinf.Entry;

    if( findbuf->handle ) {
        if( __rdos_finddata_get( findbuf, fileinfo ) )
            return( (intptr_t)findbuf );
        else
            RdosCloseDir( findbuf->handle );
    }

    lib_free( findbuf );
    return( -1 );

#else   /* DOS */
    DOSFINDTYPE     *findbuf;
    unsigned       rc;

    /*** Start a new find using _dos_findfirst ***/
    findbuf = (DOSFINDTYPE*)lib_malloc( sizeof( DOSFINDTYPE ) );
    if( findbuf == NULL )
        return( -1 );
    rc = __F_NAME(_dos_findfirst,_wdos_findfirst)( filespec, FIND_ATTR, findbuf );
    if( rc != 0 ) {
        lib_free( findbuf );
        return( -1 );
    }

    /*** Got one! ***/
  #ifdef __INT64__
    __F_NAME(__dos_finddatai64_cvt,__dos_wfinddatai64_cvt)( findbuf, fileinfo );
  #else
    __F_NAME(__dos_finddata_cvt,__dos_wfinddata_cvt)( findbuf, fileinfo );
  #endif
    return( (intptr_t)findbuf );
#endif
}


#ifdef __NT__


 #ifdef __WIDECHAR__
  #ifdef __INT64__
   void __nt_wfinddatai64_cvt( WIN32_FIND_DATA *ffd, struct _wfinddatai64_t *fileinfo )
  #else
   void __nt_wfinddata_cvt( WIN32_FIND_DATA *ffd, struct _wfinddata_t *fileinfo )
  #endif
 #else
  #ifdef __INT64__
   void __nt_finddatai64_cvt( WIN32_FIND_DATA *ffd, struct _finddatai64_t *fileinfo )
  #else
   void __nt_finddata_cvt( WIN32_FIND_DATA *ffd, struct _finddata_t *fileinfo )
  #endif
 #endif
/******************************************************************************/
{
    /*** Convert attributes ***/
    fileinfo->attrib = NT2DOSATTR( ffd->dwFileAttributes );

    /*** Handle the timestamps ***/
    fileinfo->time_create = __NT_filetime_to_timet( &ffd->ftCreationTime );
    fileinfo->time_access = __NT_filetime_to_timet( &ffd->ftLastAccessTime );
    fileinfo->time_write = __NT_filetime_to_timet( &ffd->ftLastWriteTime );

    /*** Handle the file size ***/
  #ifdef __INT64__
    U64Set( (unsigned_64 *)&fileinfo->size, ffd->nFileSizeLow, ffd->nFileSizeHigh );
  #else
    fileinfo->size = ffd->nFileSizeLow;
  #endif

    /*** Handle the file name ***/
    __F_NAME(strcpy,wcscpy)( fileinfo->name, ffd->cFileName );
}

#elif defined( __OS2__ )


 #ifdef __WIDECHAR__
  #ifdef __INT64__
   void __os2_wfinddatai64_cvt( FF_BUFFER *ffb, struct _wfinddatai64_t *fileinfo )
  #else
   void __os2_wfinddata_cvt( FF_BUFFER *ffb, struct _wfinddata_t *fileinfo )
  #endif
 #else
  #ifdef __INT64__
   void __os2_finddatai64_cvt( FF_BUFFER *ffb, struct _finddatai64_t *fileinfo )
  #else
   void __os2_finddata_cvt( FF_BUFFER *ffb, struct _finddata_t *fileinfo )
  #endif
 #endif
/******************************************************************************/
{
    /*** Handle the timestamps ***/
    fileinfo->time_create = _d2ttime( TODDATE( ffb->fdateCreation ),
                                        TODTIME( ffb->ftimeCreation ) );
    fileinfo->time_access = _d2ttime( TODDATE( ffb->fdateLastAccess ),
                                        TODTIME( ffb->ftimeLastAccess ) );
    fileinfo->time_write  = _d2ttime( TODDATE( ffb->fdateLastWrite ),
                                        TODTIME( ffb->ftimeLastWrite ) );

  #if defined( __INT64__ ) && !defined( _M_I86 )
    if( _FILEAPI64() ) {
  #endif
        fileinfo->attrib = ffb->attrFile;
        fileinfo->size = ffb->cbFile;
    #ifdef __WIDECHAR__
        mbstowcs( fileinfo->name, ffb->achName, _mbslen( (unsigned char *)ffb->achName ) + 1 );
    #else
        strcpy( fileinfo->name, ffb->achName );
    #endif
  #if defined( __INT64__ ) && !defined( _M_I86 )
    } else {
        fileinfo->attrib = ((FF_BUFFER_32 *)ffb)->attrFile;
        fileinfo->size = ((FF_BUFFER_32 *)ffb)->cbFile;
    #ifdef __WIDECHAR__
        mbstowcs( fileinfo->name, ((FF_BUFFER_32 *)ffb)->achName, _mbslen( (unsigned char *)((FF_BUFFER_32 *)ffb)->achName ) + 1 );
    #else
        strcpy( fileinfo->name, ((FF_BUFFER_32 *)ffb)->achName );
    #endif
    }
  #endif
}

#elif defined( __RDOS__ )

time_t __rdos_filetime_cvt( unsigned long long tics )
{
    unsigned long        msb = (tics >> 32) & 0xFFFFFFFF;
    unsigned long        lsb = tics & 0xFFFFFFFF;
    int                  ms;
    int                  us;
    struct tm            tm;

    RdosDecodeMsbTics( msb,
                       &tm.tm_year,
                       &tm.tm_mon,
                       &tm.tm_mday,
                       &tm.tm_hour );

    RdosDecodeLsbTics( lsb,
                       &tm.tm_min,
                       &tm.tm_sec,
                       &ms,
                       &us );

    tm.tm_year -= 1900;
    tm.tm_mon--;
    tm.tm_isdst = -1;
    tm.tm_wday = -1;
    tm.tm_yday = -1;

    return mktime( &tm );
}

int __rdos_finddata_get( RDOSFINDTYPE *findbuf, struct _finddata_t *fileinfo )
{
    struct RdosDirEntry *entry;

    if( findbuf->entry < findbuf->count ) {
        entry = (struct RdosDirEntry *)findbuf->chain;

        findbuf->chain += findbuf->header_size;
        findbuf->chain += entry->PathNameSize;
        findbuf->entry++;

        fileinfo->time_create = __rdos_filetime_cvt( entry->CreateTime );
        fileinfo->time_access = __rdos_filetime_cvt( entry->AccessTime );
        fileinfo->time_write = __rdos_filetime_cvt( entry->ModifyTime );
        fileinfo->size = entry->Size;

        fileinfo->attrib = 0;
        if( entry->Attrib & FILE_ATTRIBUTE_ARCHIVE ) {
            fileinfo->attrib |= _A_ARCH;
        }
        if( entry->Attrib & FILE_ATTRIBUTE_DIRECTORY ) {
            fileinfo->attrib |= _A_SUBDIR;
        }
        if( entry->Attrib & FILE_ATTRIBUTE_HIDDEN ) {
            fileinfo->attrib |= _A_HIDDEN;
        }
        if( entry->Attrib & FILE_ATTRIBUTE_NORMAL ) {
            fileinfo->attrib |= _A_NORMAL;
        }
        if( entry->Attrib & FILE_ATTRIBUTE_READONLY ) {
            fileinfo->attrib |= _A_RDONLY;
        }
        if( entry->Attrib & FILE_ATTRIBUTE_SYSTEM ) {
            fileinfo->attrib |= _A_SYSTEM;
        }
        return( 1 );
    }
    return( 0 );
}

#else   /* DOS */


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
/******************************************************************************/
{
    /*** Handle attributes ***/
    fileinfo->attrib = findbuf->attrib;

    /*** Handle the timestamps ***/
  #ifdef __WATCOM_LFN__
    if( IS_LFN( findbuf->reserved ) && DTALFN_CRTIME_OF( findbuf->reserved ) ) {
        fileinfo->time_create = _d2ttime( DTALFN_CRDATE_OF( findbuf->reserved ), DTALFN_CRTIME_OF( findbuf->reserved ) );
        fileinfo->time_access = _d2ttime( DTALFN_ACDATE_OF( findbuf->reserved ), DTALFN_ACTIME_OF( findbuf->reserved ) );
    } else {
  #endif
        fileinfo->time_create = -1L;
        fileinfo->time_access = -1L;
  #ifdef __WATCOM_LFN__
    }
  #endif
    fileinfo->time_write = _d2ttime( findbuf->wr_date, findbuf->wr_time );
    /*** Handle the file size ***/
  #ifdef __INT64__
    U64Set( (unsigned_64 *)&fileinfo->size, findbuf->size, 0 );
  #else
    fileinfo->size = findbuf->size;
  #endif

    /*** Handle the file name ***/
    __F_NAME(strcpy,wcscpy)( fileinfo->name, findbuf->name );
}

#endif

