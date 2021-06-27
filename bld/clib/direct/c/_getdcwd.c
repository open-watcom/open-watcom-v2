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
* Description:  Implementation of internal __getdcwd for DOS, WINDOWS
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <dos.h>
#include "liballoc.h"
#include "rtdata.h"
#include "seterrno.h"
#include "doserror.h"
#include "_direct.h"
#include "tinyio.h"
#include "_doslfn.h"


#ifdef _M_I86
  #ifdef __BIG_DATA__
    #define AUX_INFO \
        __parm __caller     [__si __ax] [__dl] \
        __value             [__ax] \
        __modify __exact    [__ax __si]
  #else
    #define AUX_INFO \
        __parm __caller     [__si] [__dl] \
        __value             [__ax] \
        __modify __exact    [__ax]
  #endif
#else
    #define AUX_INFO \
        __parm __caller     [__esi] [__dl] \
        __value             [__eax] \
        __modify __exact    [__eax]
#endif

extern unsigned __getdcwd_sfn( char *buff, unsigned char drv );
#pragma aux __getdcwd_sfn = \
        _SET_DSSI           \
        _MOV_AH DOS_GETCWD  \
        _INT_21             \
        _RST_DS             \
        "call __doserror_"  \
    AUX_INFO

#ifdef __WATCOM_LFN__

#ifdef _M_I86
extern lfn_ret_t ___getdcwd_lfn( char *path, unsigned char drv );
  #ifdef __BIG_DATA__
    #pragma aux ___getdcwd_lfn = \
            "push   ds"         \
            "mov    ds,cx"      \
            "mov    ax,7147h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "call __lfnerror_0" \
        __parm __caller     [__cx __si] [__dl] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux ___getdcwd_lfn = \
            "mov    ax,7147h"   \
            "stc"               \
            "int 21h"           \
            "call __lfnerror_0" \
        __parm __caller     [__si] [__dl] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif
#endif

static lfn_ret_t __getdcwd_lfn( char *buff, unsigned char drv )
/*************************************************************/
{
  #ifdef _M_I86
    return( ___getdcwd_lfn( buff, drv ) );
  #else
    call_struct     dpmi_rm;
    lfn_ret_t       rc;

    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.esi = RM_TB_PARM1_OFFS;
    dpmi_rm.edx = drv;
    dpmi_rm.eax = 0x7147;
    if( (rc = __dpmi_dos_call_lfn( &dpmi_rm )) == 0 ) {
        strcpy( buff, RM_TB_PARM1_LINEAR );
    }
    return( rc );
  #endif
}

#endif  /* __WATCOM_LFN__ */

unsigned __getdcwd( char *buff, unsigned char drv )
/*************************************************/
{
#ifdef __WATCOM_LFN__
    if( _RWD_uselfn ) {
        lfn_ret_t  rc;

        rc = __getdcwd_lfn( buff, drv );
        if( LFN_ERROR( rc ) ) {
            return( __set_errno_dos_reterr( LFN_INFO( rc ) ) );
        }
        if( LFN_OK( rc ) ) {
            return( 0 );
        }
    }
#endif
    return( __getdcwd_sfn( buff, drv ) );
}
