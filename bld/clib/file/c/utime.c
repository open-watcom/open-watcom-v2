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
#include "widechar.h"
#include <stddef.h>
#include <sys\types.h>
#include <time.h>
#include <dos.h>
#include "dosfunc.h"
#include <sys\stat.h>
#include <errno.h>
#include <sys\utime.h>
#include "rtdata.h"
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
    #include <stdlib.h>
    #include "mbwcconv.h"
#endif
#ifdef __WINDOWS_386__
    #include <windows.h>
#endif


_WCRTLINK int __F_NAME(utime,_wutime)( CHAR_TYPE const *fn, struct utimbuf const *times )
/**********************************************************************************/
{
    unsigned handle;
    struct tm *split;
    union REGS reg_set;
    time_t curr_time;

#if defined(__386__) && !defined(__WINDOWS_386__)
    #ifdef __WIDECHAR__
        char    mbPath[MB_CUR_MAX*_MAX_PATH];   /* single-byte char */
        __filename_from_wide( mbPath, fn );
    #endif
    reg_set.x.edx = (unsigned) __F_NAME(fn,mbPath);
    reg_set.h.ah = DOS_OPEN;
    reg_set.h.al = 0x01;        /* write access */
    (void) intdos( &reg_set, &reg_set );
    if( reg_set.x.cflag != 0 ) {
        switch( reg_set.w.ax ) {
        case 2:
            __set_errno( ENOENT );
            break;
        case 4:
            __set_errno( EMFILE );
            break;
        case 5:
            __set_errno( EACCES );
            break;
        }
        return( -1 );
    }
    if( times == NULL ) {
        curr_time = time( NULL );
        split = localtime( &curr_time );
    } else {
        split = localtime( &(times->modtime) );
    }
    if( split->tm_year < 80 ) {
        /* DOS file-system cannot handle dates before 1980 */
        __set_errno( EINVAL );
        return( -1 );
    }
    handle = reg_set.w.ax;
    reg_set.w.bx = handle;
    reg_set.w.cx = split->tm_hour*2048 + split->tm_min*32 +
                   (split->tm_sec >> 1);
    reg_set.w.dx = (split->tm_year-80)*512 + ( split->tm_mon + 1 )*32 +
                   split->tm_mday;
    reg_set.h.ah = DOS_FILE_DATE;
    reg_set.h.al = 1;           /* set date & time */
    (void) intdos( &reg_set, &reg_set );
    if( reg_set.x.cflag != 0 ) {
        __set_errno( EACCES );
        return( -1 );
    }
    reg_set.w.bx = handle;
    reg_set.h.ah = DOS_CLOSE;
    (void) intdos( &reg_set, &reg_set );
    if( reg_set.x.cflag != 0 ) {
        __set_errno( EACCES );
        return( -1 );
    }
    return( 0 );

#elif defined(__BIG_DATA__) || defined(__386__)

    struct SREGS sregs;
    #ifdef __WIDECHAR__
        char    mbPath[MB_CUR_MAX*_MAX_PATH];   /* single-byte char */
    #endif
#if defined(__386__)
    unsigned long alias;
    #ifdef __WIDECHAR__
        __filename_from_wide( mbPath, fn );
    #endif
    alias = AllocAlias16( (void *)__F_NAME(fn,mbPath) );
    reg_set.x.dx = alias & 0xffff;
    sregs.ds = alias >> 16;
#else
    #ifdef __WIDECHAR__
        __filename_from_wide( mbPath, fn );
    #endif
    reg_set.x.dx = FP_OFF( __F_NAME(fn,mbPath) );
    sregs.ds = FP_SEG( __F_NAME(fn,mbPath) );
#endif
    sregs.es = sregs.ds;                        /* for DOS/16M */
    reg_set.h.ah = DOS_OPEN;
    reg_set.h.al = 0x01;        /* write access */
    (void) intdosx( &reg_set, &reg_set, &sregs );
#if defined(__386__)
    FreeAlias16( alias );
#endif
    if( reg_set.x.cflag != 0 ) {
        switch( reg_set.x.ax ) {
        case 2:
            __set_errno( ENOENT );
            break;
        case 4:
            __set_errno( EMFILE );
            break;
        case 5:
            __set_errno( EACCES );
            break;
        }
        return( -1 );
    }
    if( times == NULL ) {
        curr_time = time( NULL );
        split = localtime( &curr_time );
    } else {
        split = localtime( &(times->modtime) );
    }
    if( split->tm_year < 80 ) {
        /* DOS file-system cannot handle dates before 1980 */
        __set_errno( EINVAL );
        return( -1 );
    }
    handle = reg_set.x.ax;
    reg_set.x.bx = handle;
    reg_set.x.cx = split->tm_hour*2048 + split->tm_min*32 +
                   (split->tm_sec >> 1);
    reg_set.x.dx = (split->tm_year-80)*512 + ( split->tm_mon + 1 )*32 +
                   split->tm_mday;
    reg_set.h.ah = DOS_FILE_DATE;
    reg_set.h.al = 1;           /* set date & time */
    (void) intdos( &reg_set, &reg_set );
    if( reg_set.x.cflag != 0 ) {
        __set_errno( EACCES );
        return( -1 );
    }
    reg_set.x.bx = handle;
    reg_set.h.ah = DOS_CLOSE;
    (void) intdos( &reg_set, &reg_set );
    if( reg_set.x.cflag != 0 ) {
        __set_errno( EACCES );
        return( -1 );
    }
    return( 0 );

#else

    #ifdef __WIDECHAR__
        char    mbPath[MB_CUR_MAX*_MAX_PATH];   /* single-byte char */
        __filename_from_wide( mbPath, fn );
    #endif
    reg_set.x.dx = (unsigned) __F_NAME(fn,mbPath);
    reg_set.h.ah = DOS_OPEN;
    reg_set.h.al = 0x01;        /* write access */
    (void) intdos( &reg_set, &reg_set );
    if( reg_set.x.cflag != 0 ) {
        switch( reg_set.x.ax ) {
        case 2:
            __set_errno( ENOENT );
            break;
        case 4:
            __set_errno( EMFILE );
            break;
        case 5:
            __set_errno( EACCES );
            break;
        }
        return( -1 );
    }
    if( times == NULL ) {
        curr_time = time( NULL );
        split = localtime( &curr_time );
    } else {
        split = localtime( &(times->modtime) );
    }
    if( split->tm_year < 80 ) {
        /* DOS file-system cannot handle dates before 1980 */
        __set_errno( EINVAL );
        return( -1 );
    }
    handle = reg_set.x.ax;
    reg_set.x.bx = handle;
    reg_set.x.cx = split->tm_hour*2048 + split->tm_min*32 +
                   (split->tm_sec >> 1);
    reg_set.x.dx = (split->tm_year-80)*512 + ( split->tm_mon + 1 )*32 +
                   split->tm_mday;
    reg_set.h.ah = DOS_FILE_DATE;
    reg_set.h.al = 1;           /* set date & time */
    (void) intdos( &reg_set, &reg_set );
    if( reg_set.x.cflag != 0 ) {
        __set_errno( EACCES );
        return( -1 );
    }
    reg_set.x.bx = handle;
    reg_set.h.ah = DOS_CLOSE;
    (void) intdos( &reg_set, &reg_set );
    if( reg_set.x.cflag != 0 ) {
        __set_errno( EACCES );
        return( -1 );
    }
    return( 0 );
#endif
}
