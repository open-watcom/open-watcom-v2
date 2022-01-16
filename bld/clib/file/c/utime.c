/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implemenation of utime().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <utime.h>
#include <dos.h>
#include <dosfunc.h>
#ifdef __WINDOWS_386__
    #include <windows.h>
#endif
#include "rtdata.h"
#include "rterrno.h"
#include "seterrno.h"
#include "tinyio.h"
#include "_doslfn.h"


#define CVT_TM2DOS_TIME(t)  ((t)->tm_hour*2048+(t)->tm_min*32+(t)->tm_sec/2)
#define CVT_TM2DOS_DATE(t)  (((t)->tm_year-80)*512+((t)->tm_mon+1)*32+(t)->tm_mday)

#define TO_TINY_TIME(x)     (*(tiny_ftime_t *)&(x))
#define TO_TINY_DATE(x)     (*(tiny_fdate_t *)&(x))

typedef struct {
    unsigned    ac_time;
    unsigned    ac_date;
    unsigned    wr_time;
    unsigned    wr_date;
} _dos_tms;


#if !defined( __WIDECHAR__ )

#if defined( __WATCOM_LFN__ )

#ifdef _M_I86
extern lfn_ret_t __dos_utime_lfn( const char *path, unsigned time, unsigned date, unsigned mode );
  #ifdef __BIG_DATA__
    #pragma aux __dos_utime_lfn = \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    ax,7143h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "call __lfnerror_0" \
        __parm __caller     [__dx __ax] [__cx] [__di] [__bx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux __dos_utime_lfn = \
            "mov    ax,7143h"   \
            "stc"               \
            "int 21h"           \
            "call __lfnerror_0" \
        __parm __caller     [__dx] [__cx] [__di] [__bx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif
#endif

static lfn_ret_t _dos_utime_lfn( const char *fname, unsigned time, unsigned date, unsigned mode )
/***********************************************************************************************/
{
  #ifdef _M_I86
    return( __dos_utime_lfn( fname, time, date, mode ) );
  #else
    call_struct     dpmi_rm;

    strcpy( RM_TB_PARM1_LINEAR, fname );
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.ecx = time;
    dpmi_rm.ebx = mode;
    dpmi_rm.edi = date;
    dpmi_rm.eax = 0x7143;
    return( __dpmi_dos_call_lfn( &dpmi_rm ) );
  #endif
}

#endif  /* __WATCOM_LFN__ */

static unsigned _utime_sfn( const char *fname, _dos_tms *dostms )
/***************************************************************/
{
    unsigned        handle;
    union REGS      regs;

#if defined(__386__) && !defined(__WINDOWS_386__)
    regs.x.edx = (unsigned)fname;
    regs.h.ah = DOS_OPEN;
    regs.h.al = 0x01;               /* write access */
    intdos( &regs, &regs );
#elif defined(__BIG_DATA__) || defined(__386__)
    {
        struct SREGS    segregs;
  #if defined(__386__)
        unsigned long   alias;

        alias = AllocAlias16( (void *)fname );
        regs.x.dx = alias & 0xffff;
        segregs.ds = alias >> 16;
  #else
        regs.w.dx = _FP_OFF( fname );
        segregs.ds = _FP_SEG( fname );
  #endif
        segregs.es = segregs.ds;    /* for DOS/16M */
        regs.h.ah = DOS_OPEN;
        regs.h.al = 0x01;        /* write access */
        intdosx( &regs, &regs, &segregs );
  #if defined(__386__)
        FreeAlias16( alias );
  #endif
    }
#else
    regs.w.dx = (unsigned)fname;
    regs.h.ah = DOS_OPEN;
    regs.h.al = 0x01;        /* write access */
    intdos( &regs, &regs );
#endif
    if( regs.x.cflag ) {
        switch( regs.w.ax ) {
        case 2:
            _RWD_errno = ENOENT;
            break;
        case 4:
            _RWD_errno = EMFILE;
            break;
        case 5:
            _RWD_errno = EACCES;
            break;
        }
        return( -1 );
    }
    handle = regs.w.ax;
    regs.w.bx = handle;
    regs.w.cx = dostms->wr_time;
    regs.w.dx = dostms->wr_date;
    regs.h.ah = DOS_FILE_DATE;
    regs.h.al = 1;           /* set date & time */
    intdos( &regs, &regs );
    if( regs.x.cflag ) {
        _RWD_errno = EACCES;
        return( -1 );
    }
    regs.w.bx = handle;
    regs.h.ah = DOS_CLOSE;
    intdos( &regs, &regs );
    if( regs.x.cflag ) {
        _RWD_errno = EACCES;
        return( -1 );
    }
    return( 0 );
}

static int _get_dos_tms( struct utimbuf const *times, _dos_tms *dostms )
/**********************************************************************/
{
    struct tm       *act;
    struct tm       *wrt;

    if( times == NULL ) {
        time_t  curr_time = time( NULL );
        wrt = localtime( &curr_time );
        act = wrt;
    } else {
        wrt = localtime( &(times->modtime) );
        act = localtime( &(times->actime) );
        if( act->tm_year < 80 || wrt->tm_year < 80 ) {
            /* DOS file-system cannot handle dates before 1980 */
            return( -1 );
        }
    }
    dostms->wr_time = CVT_TM2DOS_TIME( wrt );
    dostms->wr_date = CVT_TM2DOS_DATE( wrt );
    dostms->ac_time = CVT_TM2DOS_TIME( act );
    dostms->ac_date = CVT_TM2DOS_DATE( act );
    return( 0 );
}

#endif  /* !__WIDECHAR__ */

_WCRTLINK int __F_NAME(utime,_wutime)( CHAR_TYPE const *fname, struct utimbuf const *times )
/******************************************************************************************/
{
#ifdef __WIDECHAR__
    char        mbPath[MB_CUR_MAX * _MAX_PATH];     /* single-byte char */

    if( wcstombs( mbPath, fname, sizeof( mbPath ) ) == (size_t)-1 ) {
        mbPath[0] = '\0';
    }
    return( utime( mbPath, times ) );
#else
    _dos_tms    dostms;

    if( _get_dos_tms( times, &dostms ) ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }
  #ifdef __WATCOM_LFN__
    if( _RWD_uselfn ) {
        lfn_ret_t   rc;

        rc = _dos_utime_lfn( fname, dostms.wr_time, dostms.wr_date, 3 );
        if( LFN_ERROR( rc ) ) {
            return( __set_errno_dos( LFN_INFO( rc ) ) );
        }
        if( LFN_OK( rc ) ) {
            rc = _dos_utime_lfn( fname, dostms.ac_time, dostms.ac_date, 5 );
            if( LFN_ERROR( rc ) ) {
                return( __set_errno_dos( LFN_INFO( rc ) ) );
            }
            if( LFN_OK( rc ) ) {
                return( 0 );
            }
        }
    }
  #endif
    return( _utime_sfn( fname, &dostms ) );
#endif
}
