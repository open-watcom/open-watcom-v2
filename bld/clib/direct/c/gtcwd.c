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


#include "variety.h"
#include "widechar.h"
#include <string.h>
#ifdef __WIDECHAR__
    #include <mbstring.h>
#endif
#include "liballoc.h"
#include "seterrno.h"
#include "_doslfn.h"

#ifdef _M_I86
  #ifdef __BIG_DATA__
    #define AUX_INFO    \
        parm caller     [si ax] [dl] \
        modify exact    [ax si];
  #else
    #define AUX_INFO    \
        parm caller     [si] [dl] \
        modify exact    [ax];
  #endif
#else
    #define AUX_INFO    \
        parm caller     [esi] [dl] \
        modify exact    [eax];
#endif

extern unsigned __getdcwd_sfn( const char *buff, unsigned char drv );
#pragma aux __getdcwd_sfn = \
        _SET_DSSI       \
        _MOV_AH DOS_GETCWD \
        _INT_21         \
        _RST_DS         \
        "call __doserror_" \
        AUX_INFO

#ifdef __WATCOM_LFN__
static unsigned _getdcwd_lfn( const char *buff, unsigned char drv )
/*****************************************************************/
{
  #ifdef _M_I86
    return( __getdcwd_lfn( buff, drv ) );
  #else
    call_struct     dpmi_rm;

    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.esi = RM_TB_PARM1_OFFS;
    dpmi_rm.edx = drv;
    dpmi_rm.eax = 0x7147;
    dpmi_rm.flags = 1;
    if( __dpmi_dos_call( &dpmi_rm ) ) {
        return( -1 );
    }
    if( dpmi_rm.flags & 1 ) {
        return( __set_errno_dos_reterr( (unsigned short)dpmi_rm.eax ) );
    }
    strcpy( (char *)buff, RM_TB_PARM1_LINEAR );
    return( 0 );
  #endif
}
#endif

static unsigned __getdcwd( const char *buff, unsigned char drv )
/**************************************************************/
{
#ifdef __WATCOM_LFN__
    unsigned        rc = 0;

    if( _RWD_uselfn && (rc = _getdcwd_lfn( buff, drv )) == 0 ) {
        return( rc );
    }
    if( IS_LFN_ERROR( rc ) ) {
        return( rc );
    }
#endif
    return( __getdcwd_sfn( buff, drv ) );
}


_WCRTLINK CHAR_TYPE *__F_NAME(getcwd,_wgetcwd)( CHAR_TYPE *buf, size_t size )
/***************************************************************************/
{
    int         len;
#ifdef __WIDECHAR__
    char        cwd[MB_CUR_MAX * _MAX_PATH];       /* multi-byte chars */
#else
    char        cwd[_MAX_PATH];                    /* single-byte chars */
#endif

    __null_check( buf, 1 );
    if( __getdcwd( &cwd[3], 0 ) ) {
        __set_errno( ENOENT );      /* noent? */
        return( NULL );
    }

    /* get current drive and insert into cwd[0] */
    cwd[0] = TinyGetCurrDrive() + 'A';
    cwd[1] = ':';
    cwd[2] = '\\';
    len = __F_NAME(strlen,_mbslen)( cwd ) + 1;
    if( buf == NULL ) {
        if( (buf = lib_malloc( max( size, len ) * CHARSIZE )) == NULL ) {
            __set_errno( ENOMEM );
            return( NULL );
        }
        size = len;
    }

    /*** Copy the pathname into a buffer and quit ***/
#ifdef __WIDECHAR__
    if( mbstowcs( buf, cwd, size ) == -1 ) {
        return( NULL );
    }
    return( buf );
#else
    return( strncpy( buf, cwd, size ) );
#endif
}
