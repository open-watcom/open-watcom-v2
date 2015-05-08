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
#include <stddef.h>
#include <stdlib.h>
#include <direct.h>
#include <dos.h>
#define INCL_ERRORS
#include <wos2.h>
#include "rtdata.h"
#include "tinyio.h"
#include "seterrno.h"
#include "_dtaxxx.h"

#if defined(__WARP__)
  #define FF_LEVEL      1
  #define FF_BUFFER     FILEFINDBUF3
#else
  #define FF_LEVEL      0
  #define FF_BUFFER     FILEFINDBUF
#endif


/* we'll use this to copy from a FILEFINDBUF to a DIR in copydir() */
struct name {
    char buf[ NAME_MAX + 1 ];
};


static  void    copydir( struct find_t *buf, FF_BUFFER *dir_buff ) {
/******************************************************************/

    buf->attrib  = dir_buff->attrFile;
    buf->wr_time = *(unsigned short *)&dir_buff->ftimeLastWrite;
    buf->wr_date = *(unsigned short *)&dir_buff->fdateLastWrite;
    buf->size    = dir_buff->cbFile;

    *(struct name *)buf->name = *(struct name *)dir_buff->achName;
}


_WCRTLINK unsigned _dos_findfirst( const char *path, unsigned attr,
                                  struct find_t *buf ) {
/******************************************************/

#if defined(__OS2_286__)
    if( _RWD_osmode == OS2_MODE ) {
#endif
        APIRET      rc;
        FF_BUFFER   dir_buff;
        HDIR        handle = BAD_HANDLE;
        OS_UINT     searchcount;

        searchcount = 1;        /* only one at a time */
        rc = DosFindFirst( (PSZ)path, (PHFILE)&handle, attr,
                (PVOID)&dir_buff, sizeof( dir_buff ), &searchcount, FF_LEVEL );

        if( rc != 0 && rc != ERROR_EAS_DIDNT_FIT ) {
            FIND_HANDLE_OF( buf ) = BAD_HANDLE;
            return( __set_errno_dos_reterr( rc ) );
        }
        FIND_HANDLE_OF( buf ) = handle;
        copydir( buf, &dir_buff );          /* copy in other fields */

#if defined(__OS2_286__)
    } else {                                /* real mode */
        tiny_ret_t      rc;
        void __far *    old_dta;

        old_dta = TinyFarChangeDTA( buf );  /* set our DTA */
        rc = TinyFindFirst( path, attr );
        TinyFarSetDTA( old_dta );           /* restore DTA */
        if( TINY_ERROR( rc ) ) {
            return( __set_errno_dos_reterr( TINY_INFO( rc ) ) );
        }
    }
#endif
    return( 0 );
}


_WCRTLINK unsigned _dos_findnext( struct find_t *buf ) {
/*****************************************************/

#if defined(__OS2_286__)
    if( _RWD_osmode == OS2_MODE ) {        /* protected mode */
#endif
        APIRET  rc;

        FF_BUFFER       dir_buff;
        OS_UINT         searchcount = 1;

        rc = DosFindNext( FIND_HANDLE_OF( buf ), (PVOID)&dir_buff,
                    sizeof( dir_buff ), &searchcount );
        if( rc != 0 ) {
            return( __set_errno_dos_reterr( rc ) );
        }

        copydir( buf, &dir_buff );

#if defined(__OS2_286__)
    } else {            /* real mode */
        tiny_ret_t      rc;
        void __far *    old_dta;

        old_dta = TinyFarChangeDTA( buf );  /* set our DTA */
        rc = TinyFindNext();
        TinyFarSetDTA( old_dta );           /* restore DTA */
        if( TINY_ERROR( rc ) ) {
            return( __set_errno_dos_reterr( TINY_INFO( rc ) ) );
        }
    }
#endif
    return( 0 );
}


_WCRTLINK unsigned _dos_findclose( struct find_t *buf ) {
/******************************************************/

    APIRET      rc;

#if defined(__OS2_286__)
    if( _RWD_osmode == OS2_MODE ) {        /* protected mode */
#endif
        if( FIND_HANDLE_OF( buf ) != BAD_HANDLE ) {
            rc = DosFindClose( FIND_HANDLE_OF( buf ) );
            if( rc != 0 ) {
                return( __set_errno_dos_reterr( rc ) );
            }
        }
#if defined(__OS2_286__)
    } else {
        buf = buf;
    }
#endif
    return( 0 );
}
