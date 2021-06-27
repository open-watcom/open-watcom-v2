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
* Description:  DOS implementation of get file attributes (LFN-enabled)
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <fcntl.h>
#include <dos.h>
#include "seterrno.h"
#include "doserror.h"
#include "rtdata.h"
#include "tinyio.h"
#include "_doslfn.h"


#ifdef _M_I86
  #ifdef __BIG_DATA__
    #define INIT_VALUE
    #define SAVE_VALUE  "mov es:[bx],cx"
    #define AUX_INFO \
        __parm __caller     [__dx __ax] [__es __bx] \
        __value             [__ax] \
        __modify __exact    [__ax __cx __dx]
  #else
    #define INIT_VALUE
    #define SAVE_VALUE  "mov [bx],cx"
    #define AUX_INFO \
        __parm __caller     [__dx] [__bx] \
        __value             [__ax] \
        __modify __exact    [__ax __cx]
  #endif
#else
    #define INIT_VALUE  "xor ecx,ecx"
    #define SAVE_VALUE  "mov [ebx],ecx"
    #define AUX_INFO \
        __parm __caller     [__edx] [__ebx] \
        __value             [__eax] \
        __modify __exact    [__eax __ecx]
#endif

extern unsigned __dos_getfileattr_sfn( const char *path, unsigned *attrib );
#pragma aux __dos_getfileattr_sfn = \
        _SET_DSDX           \
        INIT_VALUE          \
        _MOV_AX_W   _GET_ DOS_CHMOD \
        _INT_21             \
        _RST_DS             \
        "jc short L1"       \
        SAVE_VALUE          \
    "L1: call __doserror_"  \
    AUX_INFO

#ifdef __WATCOM_LFN__

#ifdef _M_I86
extern lfn_ret_t __dos_getfileattr_lfn( const char *path );
  #ifdef __BIG_DATA__
    #pragma aux __dos_getfileattr_lfn = \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "xor    bl,bl"      \
            "mov    ax,7143h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
            "jnz short L2"      \
            "cmp    ax,7100h"   \
            "jz short L1"       \
            "mov    ax,cx"      \
            "jmp short L2"      \
        "L1: mov    dx,-1"      \
        "L2:"                   \
        __parm __caller     [__dx __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __bl __cx __dx]
  #else
    #pragma aux __dos_getfileattr_lfn = \
            "xor    bl,bl"      \
            "mov    ax,7143h"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
            "jnz short L2"      \
            "cmp    ax,7100h"   \
            "jz short L1"       \
            "mov    ax,cx"      \
            "jmp short L2"      \
        "L1: mov    dx,-1"      \
        "L2:"                   \
        __parm __caller     [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __bl __cx __dx]
  #endif
#endif

static lfn_ret_t _dos_getfileattr_lfn( const char *path )
/*******************************************************/
{
  #ifdef _M_I86
    return( __dos_getfileattr_lfn( path ) );
  #else
    call_struct     dpmi_rm;
    lfn_ret_t       rc;

    strcpy( RM_TB_PARM1_LINEAR, path );
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.eax = 0x7143;
    if( (rc = __dpmi_dos_call_lfn( &dpmi_rm )) == 0 ) {
        return( dpmi_rm.cx );
    }
    return( rc );
  #endif
}

#endif  /* __WATCOM_LFN__ */

_WCRTLINK unsigned _dos_getfileattr( const char *path, unsigned *attrib )
/***********************************************************************/
{
#ifdef __WATCOM_LFN__
    if( _RWD_uselfn ) {
        lfn_ret_t   rc;

        rc = _dos_getfileattr_lfn( path );
        if( LFN_ERROR( rc ) ) {
            return( __set_errno_dos_reterr( LFN_INFO( rc ) ) );
        }
        if( LFN_OK( rc ) ) {
            *attrib = LFN_INFO( rc );
            return( 0 );
        }
    }
#endif
    return( __dos_getfileattr_sfn( path, attrib ) );
}
