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
#include "i64.h"
#include "widechar.h"
#include <io.h>
#include <string.h>
#ifdef __NT__
    #include <windows.h>
    #include "libwin32.h"
    #include "ntex.h"
#elif defined( __OS2__ )
    #define INCL_LONGLONG
    #include <wos2.h>
    #include <mbstring.h>
    #include "os2fil64.h"
#elif defined( __RDOS__ )
    #include <rdos.h>
    #include "liballoc.h"
#else
    #include "liballoc.h"
    #include "_doslfn.h"
    #include "_dtaxxx.h"
#endif
#include "d2ttime.h"
#include "find.h"
#include "seterrno.h"

#if defined( __NT__ )
  #define CHECK_FIND_NEXT_ATTR    __F_NAME(__NTFindNextFileWithAttr,__wNTFindNextFileWithAttr)
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
/******************************************************************************/
{
#if defined( __NT__ )
    WIN32_FIND_DATA ffb;
    HANDLE          h;

    /*** Initialize the find ***/
    h = __lib_FindFirstFile( filespec, &ffb );
    if( h == INVALID_HANDLE_VALUE ) {
        return( __set_errno_nt() );
    }

    /*** Look for the first file ***/
    if( !CHECK_FIND_NEXT_ATTR( h, FIND_ATTR, &ffb ) ) {
        FindClose( h );
        return( __set_errno_dos( ERROR_FILE_NOT_FOUND ) );
    }

    /*** Got one! ***/
  #ifdef __INT64__
    __F_NAME(__nt_finddatai64_cvt,__nt_wfinddatai64_cvt)( &ffb, fileinfo );
  #else
    __F_NAME(__nt_finddata_cvt,__nt_wfinddata_cvt)( &ffb, fileinfo );
  #endif
    return( (long)h );
#elif defined( __OS2__ )
    APIRET          rc;
    HDIR            h = HDIR_CREATE;
    FF_BUFFER       ffb;
    OS_UINT         searchcount = 1;
 #ifdef __WIDECHAR__
    char            mbFilespec[MB_CUR_MAX * _MAX_PATH];

    if( wcstombs( mbFilespec, filespec, sizeof( mbFilespec ) ) == -1 ) {
        mbFilespec[0] = '\0';
    }
 #endif
    rc = DosFindFirst( (char*)__F_NAME(filespec,mbFilespec), &h, FIND_ATTR,
                                &ffb, sizeof( ffb ), &searchcount, FF_LEVEL );
    if( rc != 0 ) {
        return( -1L );
    }
    /*** Got one! ***/
 #ifdef __INT64__
    __F_NAME(__os2_finddatai64_cvt,__os2_wfinddatai64_cvt)( &ffb, fileinfo );
 #else
    __F_NAME(__os2_finddata_cvt,__os2_wfinddata_cvt)( &ffb, fileinfo );
 #endif
    return( (long)h );
#elif defined( __RDOS__ )
    RDOSFINDTYPE *  findbuf;

    findbuf = (RDOSFINDTYPE*) lib_malloc( sizeof( RDOSFINDTYPE ) );
    if( findbuf == NULL )  return( -1L );

    findbuf->handle = RdosOpenDir( filespec );
    findbuf->entry = 0;

    if( __rdos_finddata_get( findbuf, fileinfo ) )
        return( (long) findbuf );
    else {
        lib_free( findbuf );        
        return( -1 );
    }            
#else   /* DOS */
    DOSFINDTYPE     *findbuf;
    unsigned       rc;

    /*** Start a new find using _dos_findfirst ***/
    findbuf = (DOSFINDTYPE*) lib_malloc( sizeof( DOSFINDTYPE ) );
    if( findbuf == NULL )  return( -1L );
    rc = __F_NAME(_dos_findfirst,_wdos_findfirst)( filespec, FIND_ATTR, findbuf );
    if( rc != 0 ) {
        lib_free( findbuf );
        return( -1L );
    }

    /*** Got one! ***/
  #ifdef __INT64__
    __F_NAME(__dos_finddatai64_cvt,__dos_wfinddatai64_cvt)( findbuf, fileinfo );
  #else
    __F_NAME(__dos_finddata_cvt,__dos_wfinddata_cvt)( findbuf, fileinfo );
  #endif
    return( (long)findbuf );
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
/******************************************************************************/
{
    WORD        d;
    WORD        t;

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
    __MakeDOSDT( &ffb->ftCreationTime, &d, &t );
    fileinfo->time_create = _d2ttime( d, t );
    __MakeDOSDT( &ffb->ftLastAccessTime, &d, &t );
    fileinfo->time_access = _d2ttime( d, t );
    __MakeDOSDT( &ffb->ftLastWriteTime, &d, &t );
    fileinfo->time_write = _d2ttime( d, t );

    /*** Handle the file size ***/
  #ifdef __INT64__
    U64Set( (unsigned_64 *)&fileinfo->size, ffb->nFileSizeLow, ffb->nFileSizeHigh );
  #else
    fileinfo->size = ffb->nFileSizeLow;
  #endif

    /*** Handle the file name ***/
    __F_NAME(strcpy,wcscpy)( fileinfo->name, ffb->cFileName );
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
        mbstowcs( fileinfo->name, ffb->achName, _mbslen( ffb->achName ) + 1 );
    #else
        strcpy( fileinfo->name, ffb->achName );
    #endif
  #if defined( __INT64__ ) && !defined( _M_I86 )
    } else {
        fileinfo->attrib = ((FF_BUFFER_32 *)ffb)->attrFile;
        fileinfo->size = ((FF_BUFFER_32 *)ffb)->cbFile;
    #ifdef __WIDECHAR__
        mbstowcs( fileinfo->name, ((FF_BUFFER_32 *)ffb)->achName, _mbslen( ((FF_BUFFER_32 *)ffb)->achName ) + 1 );
    #else
        strcpy( fileinfo->name, ((FF_BUFFER_32 *)ffb)->achName );
    #endif
    }
  #endif
}

#elif __RDOS__

int __rdos_finddata_get( RDOSFINDTYPE *findbuf, struct _finddata_t *fileinfo )
{
    long            FileSize;
    int             Attribute;
    unsigned long   MsbTime;
    unsigned long   LsbTime;
    int             stat;

    stat = RdosReadDir( findbuf->handle, 
                        findbuf->entry, 
                        _MAX_PATH, 
                        fileinfo->name, 
                        &FileSize, 
                        &Attribute, 
                        &MsbTime, 
                        &LsbTime );

    if( stat ) {
        fileinfo->time_create = -1L;
        fileinfo->time_access = -1L;
        fileinfo->time_write = __rdos_filetime_cvt( MsbTime, LsbTime );
        fileinfo->size = FileSize;

        fileinfo->attrib = 0;
        if( Attribute & FILE_ATTRIBUTE_ARCHIVE ) {
            fileinfo->attrib |= _A_ARCH;
        }
        if( Attribute & FILE_ATTRIBUTE_DIRECTORY ) {
            fileinfo->attrib |= _A_SUBDIR;
        }
        if( Attribute & FILE_ATTRIBUTE_HIDDEN ) {
            fileinfo->attrib |= _A_HIDDEN;
        }
        if( Attribute & FILE_ATTRIBUTE_NORMAL ) {
            fileinfo->attrib |= _A_NORMAL;
        }
        if( Attribute & FILE_ATTRIBUTE_READONLY ) {
            fileinfo->attrib |= _A_RDONLY;
        }
        if( Attribute & FILE_ATTRIBUTE_SYSTEM ) {
            fileinfo->attrib |= _A_SYSTEM;
        }
    }
    return( stat );
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
    if( IS_LFN( findbuf ) && LFN_CRTIME_OF( findbuf ) ) {
        fileinfo->time_create = _d2ttime( LFN_CRDATE_OF( findbuf ), LFN_CRTIME_OF( findbuf ) );
        fileinfo->time_access = _d2ttime( LFN_ACDATE_OF( findbuf ), LFN_ACTIME_OF( findbuf ) );
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

