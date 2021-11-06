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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dos.h>
#include "seterrno.h"
#include "doserror.h"
#include "rtdata.h"
#include "tinyio.h"
#include "_doslfn.h"


#ifdef _M_I86
  #ifdef __BIG_DATA__
    #define AUX_INFO \
        __parm __caller     [__dx __ax] \
        __value             [__ax] \
        __modify __exact    [__ax __dx]
  #else
    #define AUX_INFO \
        __parm __caller     [__dx] \
        __value             [__ax] \
        __modify __exact    [__ax]
  #endif
#else
    #define AUX_INFO \
        __parm __caller     [__edx] \
        __value             [__eax] \
        __modify __exact    [__eax]
#endif

extern unsigned __unlink_sfn( const char *filename );
#pragma aux __unlink_sfn =  \
        _SET_DSDX           \
        _MOV_AH DOS_UNLINK  \
        _INT_21             \
        _RST_DS             \
        "call __doserror1_" \
    AUX_INFO

#if defined( __WATCOM_LFN__ ) && !defined( __WIDECHAR__ )

#ifdef _M_I86
extern lfn_ret_t __unlink_lfn( const char *filename );
  #ifdef __BIG_DATA__
    #pragma aux __unlink_lfn =  \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "xor    si,si"      \
            "mov    ax,7141h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "call __lfnerror_0" \
        __parm __caller     [__dx __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx __si]
  #else
    #pragma aux __unlink_lfn =  \
            "xor    si,si"      \
            "mov    ax,7141h"   \
            "stc"               \
            "int 21h"           \
            "call __lfnerror_0" \
        __parm __caller     [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx __si]
  #endif
#endif

static lfn_ret_t _unlink_lfn( const char *filename )
/**************************************************/
{
#ifdef _M_I86
    return( __unlink_lfn( filename ) );
#else
    call_struct     dpmi_rm;

    strcpy( RM_TB_PARM1_LINEAR, filename );
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.eax = 0x7141;
    return( __dpmi_dos_call_lfn( &dpmi_rm ) );
#endif
}

#endif  /* __WATCOM_LFN__ && !__WIDECHAR__ */

_WCRTLINK int __F_NAME(unlink,_wunlink)( const CHAR_TYPE *filename )
/******************************************************************/
{
#ifdef __WIDECHAR__
    char    mbFilename[MB_CUR_MAX * _MAX_PATH]; /* single-byte char */

    if( wcstombs( mbFilename, filename, sizeof( mbFilename ) ) == (size_t)-1 ) {
        mbFilename[0] = '\0';
    }
    return( unlink( mbFilename ) );
#else
  #ifdef __WATCOM_LFN__
    if( _RWD_uselfn ) {
        lfn_ret_t   rc;

        rc = _unlink_lfn( filename );
        if( LFN_ERROR( rc ) ) {
            return( __set_errno_dos( LFN_INFO( rc ) ) );
        }
        if( LFN_OK( rc ) ) {
            return( 0 );
        }
    }
  #endif
    return( __unlink_sfn( filename ) );
#endif
}
