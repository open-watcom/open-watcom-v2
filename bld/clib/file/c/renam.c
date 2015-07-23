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


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "seterrno.h"
#include "_doslfn.h"

#ifdef _M_I86
  #ifdef __BIG_DATA__
    #define AUX_INFO    \
        parm caller     [dx ax] [es di] \
        modify exact    [ax dx];
  #else
    #define AUX_INFO    \
        parm caller     [dx] [di] \
        modify exact    [ax];
  #endif
#else
    #define AUX_INFO    \
        parm caller     [edx] [edi] \
        modify exact    [eax];
#endif

extern unsigned __rename_sfn( const char *old, const char *new );
#pragma aux __rename_sfn = \
        _SET_ESDI       \
        _SET_DSDX       \
        _MOV_AH DOS_RENAME \
        _INT_21         \
        _RST_DS         \
        _RST_ES         \
        "call __doserror1_" \
        AUX_INFO

#if !defined( __WIDECHAR__ ) && defined( __WATCOM_LFN__ )
static tiny_ret_t _rename_lfn( const char *old, const char *new )
/***************************************************************/
{
#ifdef _M_I86
    return( __rename_lfn( old, new ) );
#else
    call_struct     dpmi_rm;

    strcpy( RM_TB_PARM1_LINEAR, old );
    strcpy( RM_TB_PARM2_LINEAR, new );
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.es  = RM_TB_PARM2_SEGM;
    dpmi_rm.edi = RM_TB_PARM2_OFFS;
    dpmi_rm.eax = 0x7156;
    dpmi_rm.flags = 1;
    if( __dpmi_dos_call( &dpmi_rm ) ) {
        return( -1 );
    }
    if( dpmi_rm.flags & 1 ) {
        return( dpmi_rm.ax | ~ 0xFFFF );
    }
    return( 0 );
#endif
}
#endif

_WCRTLINK int __F_NAME(rename,_wrename)( const CHAR_TYPE *old, const CHAR_TYPE *new )
/***********************************************************************************/
{
#ifdef __WIDECHAR__
    char        mbOld[MB_CUR_MAX * _MAX_PATH];      /* single-byte char */
    char        mbNew[MB_CUR_MAX * _MAX_PATH];      /* single-byte char */

    if( wcstombs( mbOld, old, sizeof( mbOld ) ) == -1 ) {
        mbOld[0] = '\0';
    }
    if( wcstombs( mbNew, new, sizeof( mbNew ) ) == -1 ) {
        mbNew[0] = '\0';
    }
    return( rename( mbOld, mbNew ) );
#else
  #if defined( __WATCOM_LFN__ )
    tiny_ret_t  rc = 0;

    if( _RWD_uselfn && TINY_OK( rc = _rename_lfn( old, new ) ) ) {
        return( 0 );
    }
    if( IS_LFN_ERROR( rc ) ) {
        return( __set_errno_dos( TINY_INFO( rc ) ) );
    }
  #endif
    return( __rename_sfn( old, new ) );
#endif
}

