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
#include <direct.h>
#ifdef __WIDECHAR__
    #include <mbstring.h>
#endif
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

extern unsigned __mkdir_sfn( const char *path );
#pragma aux __mkdir_sfn =   \
        _SET_DSDX           \
        _MOV_AH DOS_MKDIR   \
        _INT_21             \
        _RST_DS             \
        "call __doserror1_" \
    AUX_INFO

#if defined( __WATCOM_LFN__ ) && !defined( __WIDECHAR__ )

#ifdef _M_I86
extern lfn_ret_t __mkdir_lfn( const char *path );
  #ifdef __BIG_DATA__
    #pragma aux __mkdir_lfn =   \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    ax,7139h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "call __lfnerror_0" \
        __parm __caller     [__dx __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux __mkdir_lfn =   \
            "mov    ax,7139h"   \
            "stc"               \
            "int 21h"           \
            "call __lfnerror_0" \
        __parm __caller     [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif
#endif

static lfn_ret_t _mkdir_lfn( const char *path )
/*********************************************/
{
#ifdef _M_I86
    return( __mkdir_lfn( path ) );
#else
    call_struct     dpmi_rm;

    strcpy( RM_TB_PARM1_LINEAR, path );
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.eax = 0x7139;
    return( __dpmi_dos_call_lfn( &dpmi_rm ) );
#endif
}

#endif  /* __WATCOM_LFN__ && !__WIDECHAR__ */

_WCRTLINK int __F_NAME(mkdir,_wmkdir)( const CHAR_TYPE *path )
/************************************************************/
{
#ifdef __WIDECHAR__
    size_t              rcConvert;
    char                mbcsPath[MB_CUR_MAX * _MAX_PATH];
    unsigned char       *p;

    /*** Convert the wide character string to a multibyte string ***/
    rcConvert = wcstombs( mbcsPath, path, sizeof( mbcsPath ) );
    p = _mbsninc( (unsigned char *)mbcsPath, rcConvert );
    *p = '\0';
    return( mkdir( mbcsPath ) );
#else
  #ifdef __WATCOM_LFN__
    if( _RWD_uselfn ) {
        lfn_ret_t   rc;

        rc = _mkdir_lfn( path );
        if( LFN_ERROR( rc ) ) {
            return( __set_errno_dos( LFN_INFO( rc ) ) );
        }
        if( LFN_OK( rc ) ) {
            return( 0 );
        }
    }
  #endif
    return( __mkdir_sfn( path ) );
#endif
}
