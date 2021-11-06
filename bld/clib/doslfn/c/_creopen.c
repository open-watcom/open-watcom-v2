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
* Description:  DOS implementation of interrnal function
*                 _dos_create_open_ex_lfn (LFN-enabled)
*
****************************************************************************/


#define __WATCOM_LFN__
#include <string.h>
#include <dos.h>
#include "tinyio.h"
#include "_doslfn.h"


#ifdef _M_I86
extern lfn_ret_t __dos_create_open_ex_lfn( const char *name, unsigned mode, unsigned attrib, unsigned action );
  #ifdef __BIG_DATA__
    #pragma aux __dos_create_open_ex_lfn = \
            "push   ds"         \
            "xchg   ax,si"      \
            "mov    ds,ax"      \
            "mov    ax,716Ch"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "call __lfnerror_ax" \
        __parm __caller     [__si __ax] [__bx] [__cx] [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __cx __dx __si]
  #else
    #pragma aux __dos_create_open_ex_lfn = \
            "mov    ax,716Ch"   \
            "stc"               \
            "int 21h"           \
            "call __lfnerror_ax" \
        __parm __caller     [__si] [__bx] [__cx] [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __cx __dx]
  #endif
#endif


lfn_ret_t _dos_create_open_ex_lfn( const char *path, unsigned mode, unsigned attrib, unsigned action )
/****************************************************************************************************/
{
#ifdef _M_I86
    return( __dos_create_open_ex_lfn( path, mode, attrib, action ) );
#else
    call_struct     dpmi_rm;

    strcpy( RM_TB_PARM1_LINEAR, path );
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.esi = RM_TB_PARM1_OFFS;
    dpmi_rm.edx = action;
    dpmi_rm.ecx = attrib;
    dpmi_rm.ebx = mode;
    dpmi_rm.eax = 0x716C;
    return( __dpmi_dos_call_lfn_ax( &dpmi_rm ) );
#endif
}
