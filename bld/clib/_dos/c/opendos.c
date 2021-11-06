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
* Description:  DOS implementation of _dos_open (LFN-enabled)
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
    #define SAVE_VALUE  "mov es:[bx],ax"
    #define RETURN_CY   "sbb ax,ax"
    #define AUX_INFO \
        __parm __caller     [__dx __ax] [__cx] [__es __bx] \
        __value             [__ax] \
        __modify __exact    [__ax __dx]
  #else
    #define INIT_VALUE
    #define SAVE_VALUE  "mov [bx],ax"
    #define RETURN_CY   "sbb ax,ax"
    #define AUX_INFO \
        __parm __caller     [__dx] [__cx] [__bx] \
        __value             [__ax] \
        __modify __exact    [__ax __dx]
  #endif
#else
    #define INIT_VALUE  "xor eax,eax"
    #define SAVE_VALUE  "mov [ebx],eax"
    #define RETURN_CY   "sbb eax,eax"
    #define AUX_INFO \
        __parm __caller     [__edx] [__ecx] [__ebx] \
        __value             [__eax] \
        __modify __exact    [__eax __edx]
#endif

#define __DOS_OPEN_SFN      \
        _SET_DSDX           \
        INIT_VALUE          \
        "mov    al,cl"      \
        _MOV_AH DOS_OPEN    \
        _INT_21             \
        _RST_DS

extern unsigned __dos_open_sfn_err( const char *name, unsigned mode, int *handle );
#pragma aux __dos_open_sfn_err = \
        __DOS_OPEN_SFN      \
        "jc short L1"       \
        SAVE_VALUE          \
    "L1: call __doserror_"  \
    AUX_INFO

#ifdef __WATCOM_LFN__

extern unsigned __dos_open_sfn_chk( const char *name, unsigned mode, int *handle );
#pragma aux __dos_open_sfn_chk = \
        __DOS_OPEN_SFN      \
        "jc short L1"       \
        SAVE_VALUE          \
    "L1:"                   \
        RETURN_CY           \
    AUX_INFO

static lfn_ret_t __dos_open_lfn( const char *path, unsigned mode )
/****************************************************************/
{
    char        short_name[128];
    int         handle;

    // try to open with SFN
    if( _lfntosfn( path, short_name ) != NULL ) {
        if( short_name[0] != '\0' ) {
            if( __dos_open_sfn_chk( short_name, mode, &handle ) == 0 ) {
                return( (unsigned)handle );
            }
        }
    }
    return( _dos_create_open_ex_lfn( path, mode, _A_NORMAL, EX_LFN_OPEN ) );
}

#endif  /* __WATCOM_LFN__ */

_WCRTLINK unsigned _dos_open( const char *path, unsigned mode, int *handle )
/**************************************************************************/
{
#ifdef __WATCOM_LFN__
    if( _RWD_uselfn ) {
        lfn_ret_t   rc;

        rc = __dos_open_lfn( path, mode );
        if( LFN_ERROR( rc ) ) {
            return( __set_errno_dos_reterr( LFN_INFO( rc ) ) );
        }
        if( LFN_OK( rc ) ) {
            *handle = LFN_INFO( rc );
            return( 0 );
        }
    }
#endif
    return( __dos_open_sfn_err( path, mode, handle ) );
}
