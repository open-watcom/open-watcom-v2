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
* Description:  DOS implementation of fstat().
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "rtdata.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "d2ttime.h"
#include "_doslfn.h"
#include "int64.h"

extern long __getfilestamp_sfn( int handle );
#ifdef _M_I86
#pragma aux __getfilestamp_sfn = \
        _MOV_AX_W _GET_ DOS_FILE_DATE \
        _INT_21         \
        "jnc short L1"   \
        "call __doserror_" \
        "mov  cx,-1"    \
        "mov  dx,cx"    \
"L1:     mov  ax,cx"    \
        parm caller     [bx] \
        value           [dx ax] \
        modify exact    [ax cx dx];
#else
#pragma aux __getfilestamp_sfn = \
        _MOV_AX_W _GET_ DOS_FILE_DATE \
        _INT_21         \
        "jnc short L1"   \
        "call __doserror_" \
        "mov  cx,-1"    \
        "mov  dx,cx"    \
"L1:     shl  edx,16"   \
        "mov  dx,cx"    \
        parm caller     [ebx] \
        value           [edx] \
        modify exact    [eax ecx edx];
#endif


#ifdef __INT64__

_WCRTLINK int _fstati64( int handle, struct _stati64 *buf )
{
    struct _stat        buf32;
    int                 rc;
    INT_TYPE            tmp;

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
    _clib_U32ToU64( buf32.st_size, tmp );
    buf->st_size = GET_REALINT64(tmp);
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

#else

#ifdef __WATCOM_LFN__
static time_t _cvt_stamp2ttime_lfn( long long *timestamp )
{
#ifdef _M_I86
    long            rc;

    rc = __cvt_stamp2dos_lfn( timestamp );
    if( rc == -1 ) {
        return( 0 );
    }
    return( _d2ttime( (unsigned long)rc >> 16, rc ) );
#else
    call_struct     dpmi_rm;

    *((long long *)RM_TB_PARM1_LINEAR) = *timestamp;
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.esi = RM_TB_PARM1_OFFS;
    dpmi_rm.ebx = 0;
    dpmi_rm.eax = 0x71A7;
    dpmi_rm.flags = 1;
    if( __dpmi_dos_call( &dpmi_rm ) ) {
        return( 0 );
    }
    if( dpmi_rm.flags & 1 ) {
        __set_errno_dos( (unsigned short)dpmi_rm.eax );
        return( 0 );
    }
    return( _d2ttime( dpmi_rm.edx, dpmi_rm.ecx ) );
#endif
}

static unsigned _getfileinfo_lfn( int handle, lfninfo_t *lfninfo )
{
#ifdef _M_I86
    return( __getfileinfo_lfn( handle, lfninfo ) );
#else
    call_struct     dpmi_rm;

    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.ebx = handle;
    dpmi_rm.eax = 0x71A6;
    dpmi_rm.flags = 1;
    if( __dpmi_dos_call( &dpmi_rm ) ) {
        return( -1 );
    }
    if( dpmi_rm.flags & 1 ) {
        return( __set_errno_dos_reterr( (unsigned short)dpmi_rm.eax ) );
    }
    memcpy( lfninfo, RM_TB_PARM1_LINEAR, sizeof( *lfninfo ) );
    return( 0 );
#endif
}
#endif

_WCRTLINK int fstat( int handle, struct stat *buf )
{
    unsigned        iomode_flags;
    tiny_ret_t      rc;

    __handle_check( handle, -1 );

    rc = TinyGetDeviceInfo( handle );
    if( TINY_ERROR(rc) ) {
        return( __set_errno_dos( TINY_INFO(rc) ) );
    }
    /* isolate drive number */
    buf->st_dev = TINY_INFO(rc) & TIO_CTL_DISK_DRIVE_MASK;
    buf->st_rdev = buf->st_dev;

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
    if( TINY_INFO(rc) & TIO_CTL_DEVICE ) {
        buf->st_size = 0;
        buf->st_atime = buf->st_ctime = buf->st_mtime = 0;
        buf->st_mode |= S_IFCHR;
    } else {                /* file */
#ifdef __WATCOM_LFN__
        lfninfo_t       lfni;
        unsigned        rc1 = 0;

        if( _RWD_uselfn && (rc1 = _getfileinfo_lfn( handle, &lfni )) == 0 ) {
            buf->st_mtime = _cvt_stamp2ttime_lfn( &lfni.writetimestamp );
            if( lfni.creattimestamp ) {
                buf->st_ctime = _cvt_stamp2ttime_lfn( &lfni.creattimestamp );
            } else {
                buf->st_ctime = buf->st_mtime;
            }
            if( lfni.accesstimestamp ) {
                buf->st_atime = _cvt_stamp2ttime_lfn( &lfni.accesstimestamp );
            } else {
                buf->st_atime = buf->st_mtime;
            }
            buf->st_size = lfni.lfilesize;
            buf->st_attr = lfni.attributes;
        } else if( IS_LFN_ERROR( rc1 ) ) {
            return( -1 );
        } else {
#endif
            long    rc;

            rc = __getfilestamp_sfn( handle );
            if( rc == -1 ) {
                return( rc );
            }
            buf->st_mtime = _d2ttime( (unsigned long)rc >> 16, rc );
            buf->st_atime = buf->st_mtime;
            buf->st_ctime = buf->st_mtime;
            buf->st_size = filelength( handle );
            buf->st_attr = 0;
#ifdef __WATCOM_LFN__
        }
#endif
        buf->st_mode |= S_IFREG;
    }
    buf->st_btime = buf->st_mtime;
    buf->st_archivedID = 0;
    buf->st_updatedID = 0;
    buf->st_inheritedRightsMask = 0;
    buf->st_originatingNameSpace = 0;
    return( 0 );
}

#endif

