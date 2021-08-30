/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS and Windows 3.x implementation of fstat().
*
****************************************************************************/


#ifdef __INT64__

#include "variety.h"
#include <sys/types.h>
#include <sys/stat.h>


_WCRTLINK int _fstati64( int handle, struct _stati64 *buf )
{
    struct stat         buf32;
    int                 rc;

    /*** Get the info using non-64bit version ***/
    rc = _fstat( handle, &buf32 );
    if( rc == -1 )
        return( -1 );

    /*** Convert the info to 64-bit equivalent ***/
    buf->st_dev = buf32.st_dev;
    buf->st_ino = buf32.st_ino;
    buf->st_mode = buf32.st_mode;
    buf->st_nlink = buf32.st_nlink;
    buf->st_uid = buf32.st_uid;
    buf->st_gid = buf32.st_gid;
    buf->st_rdev = buf32.st_rdev;
    buf->st_size = (unsigned long)buf32.st_size;
    buf->st_atime = buf32.st_atime;
    buf->st_mtime = buf32.st_mtime;
    buf->st_ctime = buf32.st_ctime;
    buf->st_btime = buf32.st_btime;
    buf->st_attr = buf32.st_attr;
    buf->st_archivedID = buf32.st_archivedID;
    buf->st_updatedID = buf32.st_updatedID;
    buf->st_inheritedRightsMask = buf32.st_inheritedRightsMask;
    buf->st_originatingNameSpace = buf32.st_originatingNameSpace;

    return( rc );
}

#else   /* !__INT64__ */

#include "variety.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <dos.h>
#include "rtdata.h"
#include "iomode.h"
#include "rtcheck.h"
#include "rterrno.h"
#include "seterrno.h"
#include "d2ttime.h"
#include "tinyio.h"
#include "_doslfn.h"


extern long __getfilestamp_sfn( int handle );
#ifdef _M_I86
  #pragma aux __getfilestamp_sfn = \
        _MOV_AX_W _GET_ DOS_FILE_DATE \
        _INT_21             \
        "jnc short L1"      \
        "call __set_errno_dos" \
        "mov  dx,ax"        \
        "jmp short L2"      \
    "L1: mov  ax,cx"        \
    "L2:"                   \
    __parm __caller     [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __cx __dx]
#else
  #pragma aux __getfilestamp_sfn = \
        _MOV_AX_W _GET_ DOS_FILE_DATE \
        _INT_21             \
        "jnc short L1"      \
        "and  eax,0ffffh"   \
        "call __set_errno_dos" \
        "mov  edx,eax"      \
        "jmp short L2"      \
    "L1: shl  edx,16"       \
        "mov  dx,cx"        \
    "L2:"                   \
    __parm __caller     [__ebx] \
    __value             [__edx] \
    __modify __exact    [__eax __ecx __edx]
#endif


#ifdef __WATCOM_LFN__

#ifdef _M_I86
extern long __cvt_stamp2dos_lfn( long long *timestamp );
  #ifdef __BIG_DATA__
    #pragma aux __cvt_stamp2dos_lfn = \
            "push   ds"         \
            "mov    ds,dx"      \
            "xor    bx,bx"      \
            "mov    ax,71A7h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "jc short L1"       \
            "cmp    ax,7100h"   \
            "jz short L2"       \
            "mov    ax,cx"      \
            "jmp short L3"      \
        "L1: cmp    ax,7100h"   \
            "jz short L2"       \
            "call __set_errno_dos" \
        "L2: mov    ax,-1"      \
            "mov    dx,ax"      \
        "L3:"                   \
        __parm __caller     [__dx __si] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __bx __cx __dx]
  #else
    #pragma aux __cvt_stamp2dos_lfn = \
            "xor    bx,bx"      \
            "mov    ax,71A7h"   \
            "stc"               \
            "int 21h"           \
            "jc short L1"       \
            "cmp    ax,7100h"   \
            "jz short L2"       \
            "mov    ax,cx"      \
            "jmp short L3"      \
        "L1: cmp    ax,7100h"   \
            "jz short L2"       \
            "call __set_errno_dos" \
        "L2: mov    ax,-1"      \
            "mov    dx,ax"      \
        "L3:"                   \
        __parm __caller     [__si] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __bx __cx __dx]
  #endif

extern lfn_ret_t __getfileinfo_lfn( int handle, lfninfo_t *lfninfo );
  #ifdef __BIG_DATA__
    #pragma aux __getfileinfo_lfn = \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    ax,71A6h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "call __lfnerror_0" \
        __parm __caller     [__bx] [__dx __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux __getfileinfo_lfn = \
            "mov    ax,71A6h"   \
            "stc"               \
            "int 21h"           \
            "call __lfnerror_0" \
        __parm __caller     [__bx] [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif
#endif

static long _cvt_stamp2dos_lfn( long long *timestamp )
{
  #ifdef _M_I86
    return( __cvt_stamp2dos_lfn( timestamp ) );
  #else
    call_struct     dpmi_rm;

    *((long long *)RM_TB_PARM1_LINEAR) = *timestamp;
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.esi = RM_TB_PARM1_OFFS;
    dpmi_rm.eax = 0x71A7;
    if( __dpmi_dos_call_lfn( &dpmi_rm ) == 0 ) {
        return( dpmi_rm.dx << 16 | dpmi_rm.cx );
    }
    return( -1 );
  #endif
}

static lfn_ret_t _getfileinfo_lfn( int handle, lfninfo_t *lfninfo )
{
  #ifdef _M_I86
    return( __getfileinfo_lfn( handle, lfninfo ) );
  #else
    call_struct     dpmi_rm;
    lfn_ret_t       rc;

    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.ebx = handle;
    dpmi_rm.eax = 0x71A6;
    if( (rc = __dpmi_dos_call_lfn( &dpmi_rm )) == 0 ) {
        memcpy( lfninfo, RM_TB_PARM1_LINEAR, sizeof( *lfninfo ) );
    }
    return( rc );
  #endif
}

#endif  /* __WATCOM_LFN__ */

_WCRTLINK int fstat( int handle, struct stat *buf )
{
    {
        unsigned        iomode_flags;
        tiny_ret_t      rc;

        __handle_check( handle, -1 );

        rc = TinyGetDeviceInfo( handle );
        if( TINY_ERROR( rc ) ) {
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
        /* isolate drive number */
        buf->st_dev = TINY_INFO( rc ) & TIO_CTL_DISK_DRIVE_MASK;
        buf->st_rdev = buf->st_dev;

        buf->st_archivedID = 0;
        buf->st_updatedID = 0;
        buf->st_inheritedRightsMask = 0;
        buf->st_originatingNameSpace = 0;
        buf->st_attr = 0;
        buf->st_nlink = 1;
        buf->st_uid = buf->st_gid = 0;
        buf->st_ino = handle;
        buf->st_mode = 0;
        iomode_flags = __GetIOMode( handle );
        if( iomode_flags & _READ ) {
            buf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
        }
        if( iomode_flags & _WRITE ) {
            buf->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
        }
        /* device */
        if( TINY_INFO( rc ) & TIO_CTL_DEVICE ) {
            buf->st_atime = buf->st_ctime = buf->st_btime = buf->st_mtime = 0;
            buf->st_size = 0;
            buf->st_mode |= S_IFCHR;
            return( 0 );
        }
    }
    /* file */
  #ifdef __WATCOM_LFN__
    {
        if( _RWD_uselfn ) {
            lfninfo_t   lfni;
            lfn_ret_t   rc;

            rc = _getfileinfo_lfn( handle, &lfni );
            if( LFN_ERROR( rc ) ) {
                return( __set_errno_dos( LFN_INFO( rc ) ) );
            }
            if( LFN_OK( rc ) ) {
                long    rc1;
                time_t  t;

                t = 0;
                rc1 = _cvt_stamp2dos_lfn( &lfni.writetimestamp );
                if( rc1 != -1 ) {
                    t = _d2ttime( rc1 >> 16, rc1 );
                }
                buf->st_atime = buf->st_ctime = buf->st_btime = buf->st_mtime = t;
                if( lfni.creattimestamp ) {
                    rc1 = _cvt_stamp2dos_lfn( &lfni.creattimestamp );
                    if( rc1 != -1 ) {
                        buf->st_ctime = _d2ttime( rc1 >> 16, rc1 );
                    }
                }
                if( lfni.accesstimestamp ) {
                    rc1 = _cvt_stamp2dos_lfn( &lfni.accesstimestamp );
                    if( rc1 != -1 ) {
                        buf->st_atime = _d2ttime( rc1 >> 16, rc1 );
                    }
                }
                buf->st_size = lfni.lfilesize;
                buf->st_attr = lfni.attributes;
                buf->st_mode |= S_IFREG;
                return( 0 );
            }
        }
    }
  #endif
    {
        long    rc;

        rc = __getfilestamp_sfn( handle );
        if( rc == -1 ) {
            return( -1 );
        }
        buf->st_atime = buf->st_ctime = buf->st_btime = buf->st_mtime = _d2ttime( rc >> 16, rc );
        buf->st_size = _filelength( handle );
        buf->st_mode |= S_IFREG;
        return( 0 );
    }
}

#endif  /* !__INT64__ */
