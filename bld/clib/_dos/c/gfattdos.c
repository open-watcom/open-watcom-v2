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
* Description:  DOS implementation of get file attributes (LFN-enabled)
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <fcntl.h>
#include "seterrno.h"
#include "_doslfn.h"

#ifdef _M_I86
  #ifdef __BIG_DATA__
    #define INIT_VALUE
    #define SAVE_VALUE  "mov es:[bx],cx"
    #define AUX_INFO    \
        parm caller     [dx ax] [es bx] \
        modify exact    [ax cx dx];
  #else
    #define INIT_VALUE
    #define SAVE_VALUE  "mov [bx],cx"
    #define AUX_INFO    \
        parm caller     [dx] [bx] \
        modify exact    [ax cx];
  #endif
#else
    #define INIT_VALUE  "xor  ecx,ecx"
    #define SAVE_VALUE  "mov [ebx],ecx"
    #define AUX_INFO    \
        parm caller     [edx] [ebx] \
        modify exact    [eax ecx];
#endif

extern unsigned __dos_getfileattr_sfn( const char *path, unsigned *attrib );
#pragma aux __dos_getfileattr_sfn = \
        _SET_DSDX       \
        INIT_VALUE      \
        _MOV_AX_W _GET_ DOS_CHMOD \
        _INT_21         \
        _RST_DS         \
        RETURN_VALUE    \
        "call __doserror_" \
        AUX_INFO

#ifdef __WATCOM_LFN__
static unsigned _dos_getfileattr_lfn( const char *path, unsigned *attrib )
/************************************************************************/
{
  #ifdef _M_I86
    return( __dos_getfileattr_lfn( path, attrib ) );
  #else
    call_struct     dpmi_rm;

    strcpy( RM_TB_PARM1_LINEAR, path );
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.ecx = 0;
    dpmi_rm.ebx = 0;
    dpmi_rm.eax = 0x7143;
    dpmi_rm.flags = 1;
    if( __dpmi_dos_call( &dpmi_rm ) ) {
        return( -1 );
    }
    if( dpmi_rm.flags & 1 ) {
        return( __set_errno_dos_reterr( (unsigned short)dpmi_rm.eax ) );
    }
    *attrib = dpmi_rm.ecx;
    return( 0 );
  #endif
}
#endif

_WCRTLINK unsigned _dos_getfileattr( const char *path, unsigned *attrib )
/***********************************************************************/
{
#ifdef __WATCOM_LFN__
    unsigned    rc = 0;

    if( _RWD_uselfn && (rc = _dos_getfileattr_lfn( path, attrib )) == 0 ) {
        return( rc );
    }
    if( IS_LFN_ERROR( rc ) ) {
        return( rc );
    }
#endif
    return( __dos_getfileattr_sfn( path, attrib ) );
}
