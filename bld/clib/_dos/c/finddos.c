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
* Description:  DOS implementation of _dos_find... functions (LFN-enabled)
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include <dos.h>
#include "seterrno.h"
#include "doserror.h"
#include "rtdata.h"
#include "tinyio.h"
#include "_doslfn.h"
#include "_dtaxxx.h"
#include "extender.h"


#define MOV_DTA             \
        "mov    ecx,43"     \
        "rep movsb"

#define MOV_DATA_TO_DTA     \
        "mov    esi,edx"    \
        "mov    edi,ebx"    \
        MOV_DTA

#define MOV_DATA_FROM_DTA   \
        "mov    esi,ebx"    \
        "mov    edi,edx"    \
        "mov    ebx,ds"     \
        "push   es"         \
        "pop    ds"         \
        "mov    es,ebx"     \
        MOV_DTA             \
        "mov    ds,ebx"

//#define CMP_EXTENDER_INTEL  "cmp  _Extender,9"
#define CMP_EXTENDER_INTEL  0x80 0x3D __offset _Extender DOSX_INTEL

extern unsigned __dos_find_first_dta( const char *path, unsigned attrib, struct find_t *fdta );
extern unsigned __dos_find_next_dta( struct find_t *fdta );
extern unsigned __dos_find_close_dta( struct find_t *fdta );

#if defined( _M_I86 )
  #ifdef __BIG_DATA__
    #pragma aux __dos_find_first_dta = \
            _SET_DSDX           \
            "push   es"         \
            "push   bx"         \
            _MOV_AH DOS_SET_DTA \
            _INT_21             \
            "pop    dx"         \
            "pop    ds"         \
            _MOV_AH DOS_FIND_FIRST \
            _INT_21             \
            _RST_DS             \
            "call __doserror_"  \
        __parm __caller     [__es __bx] [__cx] [__dx __ax] \
        __value             [__ax] \
        __modify __exact    [__ax __dx]

    #pragma aux __dos_find_next_dta = \
            _SET_DSDX           \
            _MOV_AH DOS_SET_DTA \
            _INT_21             \
            _RST_DS             \
            _MOV_AH DOS_FIND_NEXT \
            _INT_21             \
            "call __doserror_"  \
        __parm __caller     [__dx __ax] \
        __value             [__ax] \
        __modify __exact    [__ax __dx]

    #pragma aux __dos_find_close_dta = \
            "xor    ax,ax"      \
        __parm __caller     [__dx __ax] \
        __value             [__ax] \
        __modify __exact    [__ax]

  #else                 // 16-bit near data
    #pragma aux __dos_find_first_dta = \
            _MOV_AH DOS_SET_DTA \
            _INT_21             \
            "mov    dx,bx"      \
            _MOV_AH DOS_FIND_FIRST \
            _INT_21             \
            "call __doserror_"  \
        __parm __caller     [__bx] [__cx] [__dx] \
        __value             [__ax] \
        __modify __exact    [__ax __dx]

    #pragma aux __dos_find_next_dta = \
            _MOV_AH DOS_SET_DTA \
            _INT_21             \
            _MOV_AH DOS_FIND_NEXT \
            _INT_21             \
            "call __doserror_"  \
        __parm __caller     [__dx] \
        __value             [__ax] \
        __modify __exact    [__ax]

    #pragma aux __dos_find_close_dta = \
            "xor    ax,ax"      \
        __parm __caller     [__dx] \
        __value             [__ax] \
        __modify __exact    [__ax]

  #endif
#elif defined( __CALL21__ )    // 32-bit near data
    #pragma aux __dos_find_first_dta = \
            _MOV_AH DOS_SET_DTA \
            _INT_21             \
            "mov    edx,ebx"    \
            _MOV_AH DOS_FIND_FIRST \
            _INT_21             \
            "call __doserror_"  \
        __parm __caller     [__ebx] [__ecx] [__edx] \
        __value             [__eax] \
        __modify __exact    [__eax __edx]

    #pragma aux __dos_find_next_dta = \
            _MOV_AH DOS_SET_DTA \
            _INT_21             \
            _MOV_AH DOS_FIND_NEXT \
            _INT_21             \
            "call __doserror_"  \
        __parm __caller     [__edx] \
        __value             [__eax] \
        __modify __exact    [__eax]

    #pragma aux __dos_find_close_dta = \
            "xor    eax,eax"    \
        __parm __caller     [__edx] \
        __value             [__eax] \
        __modify __exact    [__eax]

#else                   // 32-bit near data
    #pragma aux __dos_find_first_dta = \
            "push   edx"        \
            _MOV_AH DOS_SET_DTA \
            _INT_21             \
            "mov    edx,ebx"    \
            _MOV_AH DOS_FIND_FIRST \
            _INT_21             \
            "pop    edx"        \
            "call __doserror_"  \
            "test   eax,eax"    \
            "jnz short L1"      \
            CMP_EXTENDER_INTEL  \
            "jnz short L1"      \
            "push   es"         \
            _MOV_AH DOS_GET_DTA \
            _INT_21             \
            MOV_DATA_FROM_DTA   \
            "pop    es"         \
            "xor    eax,eax"    \
        "L1:"                   \
        __parm __caller     [__ebx] [__ecx] [__edx] \
        __value             [__eax] \
        __modify __exact    [__eax __ebx __ecx __edx __edi __esi]

    #pragma aux __dos_find_next_dta = \
            "push   es"         \
            _MOV_AH DOS_SET_DTA \
            _INT_21             \
            CMP_EXTENDER_INTEL  \
            "jnz short L1"      \
            _MOV_AH DOS_GET_DTA \
            _INT_21             \
            MOV_DATA_TO_DTA     \
        "L1:"                   \
            _MOV_AH DOS_FIND_NEXT \
            _INT_21             \
            "call __doserror_"  \
            "test   eax,eax"    \
            "jnz short L2"      \
            CMP_EXTENDER_INTEL  \
            "jnz short L2"      \
            MOV_DATA_FROM_DTA   \
        "L2: pop    es"         \
        __parm __caller     [__edx] \
        __value             [__eax] \
        __modify __exact    [__eax __ebx __ecx __edi __esi]

    #pragma aux __dos_find_close_dta = \
            "xor    eax,eax"    \
        __parm __caller     [__edx] \
        __value             [__eax] \
        __modify __exact    [__eax]

#endif

#ifdef __WATCOM_LFN__

#if defined( _M_I86 )
extern lfn_ret_t __dos_find_first_lfn( const char *path, unsigned attr, lfnfind_t __far *lfndta );
  #ifdef __BIG_DATA__
    #pragma aux __dos_find_first_lfn = \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    si,1"       \
            "mov    ax,714Eh"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "call __lfnerror_ax" \
        __parm __caller     [__dx __ax] [__cx] [__es __di] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __cx __dx __si]
  #else
    #pragma aux __dos_find_first_lfn = \
            "mov    si,1"       \
            "mov    ax,714Eh"   \
            "stc"               \
            "int 21h"           \
            "call __lfnerror_ax" \
        __parm __caller     [__dx] [__cx] [__es __di] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __cx __dx __si]
  #endif

extern lfn_ret_t __dos_find_next_lfn( unsigned handle, lfnfind_t __far *lfndta );
#pragma aux __dos_find_next_lfn = \
        "mov    si,1"           \
        "mov    ax,714fh"       \
        "stc"                   \
        "int 21h"               \
        "call __lfnerror_0"     \
    __parm __caller     [__bx] [__es __di] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __cx __dx __si]

extern lfn_ret_t __dos_find_close_lfn( unsigned handle );
#pragma aux __dos_find_close_lfn = \
        "mov    ax,71A1h"       \
        "stc"                   \
        "int 21h"               \
        "call __lfnerror_0"     \
    __parm __caller     [__bx]  \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]
#endif

static void convert_to_find_t( struct find_t *fdta, lfnfind_t *lfndta )
/*********************************************************************/
{
    fdta->attrib  = lfndta->attributes;
    DTALFN_CRTIME_OF( fdta->reserved ) = lfndta->creattime;
    DTALFN_CRDATE_OF( fdta->reserved ) = lfndta->creatdate;
    DTALFN_ACTIME_OF( fdta->reserved ) = lfndta->accesstime;
    DTALFN_ACDATE_OF( fdta->reserved ) = lfndta->accessdate;
    fdta->wr_time = lfndta->wrtime;
    fdta->wr_date = lfndta->wrdate;
    fdta->size    = lfndta->lfilesize;
    strcpy( fdta->name, ( *lfndta->lfn != '\0' ) ? lfndta->lfn : lfndta->sfn );
}

static lfn_ret_t _dos_find_first_lfn( const char *path, unsigned attrib, lfnfind_t *lfndta )
/******************************************************************************************/
{
#ifdef _M_I86
    return( __dos_find_first_lfn( path, attrib, lfndta ) );
#else
    call_struct     dpmi_rm;
    lfn_ret_t       rc;

    strcpy( RM_TB_PARM1_LINEAR, path );
    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ds  = RM_TB_PARM1_SEGM;
    dpmi_rm.edx = RM_TB_PARM1_OFFS;
    dpmi_rm.es  = RM_TB_PARM2_SEGM;
    dpmi_rm.edi = RM_TB_PARM2_OFFS;
    dpmi_rm.ecx = attrib;
    dpmi_rm.esi = 1;
    dpmi_rm.eax = 0x714E;
    if( (rc = __dpmi_dos_call_lfn_ax( &dpmi_rm )) >= 0 ) {
        memcpy( lfndta, RM_TB_PARM2_LINEAR, sizeof( *lfndta ) );
    }
    return( rc );
#endif
}

static lfn_ret_t _dos_find_next_lfn( unsigned handle, lfnfind_t *lfndta )
/***********************************************************************/
{
#ifdef _M_I86
    return( __dos_find_next_lfn( handle, lfndta ) );
#else
    call_struct     dpmi_rm;
    lfn_ret_t       rc;

    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.es  = RM_TB_PARM1_SEGM;
    dpmi_rm.edi = RM_TB_PARM1_OFFS;
    dpmi_rm.ebx = handle;
    dpmi_rm.esi = 1;
    dpmi_rm.eax = 0x714F;
    if( (rc = __dpmi_dos_call_lfn( &dpmi_rm )) == 0 ) {
        memcpy( lfndta, RM_TB_PARM1_LINEAR, sizeof( *lfndta ) );
    }
    return( rc );
#endif
}

static lfn_ret_t _dos_find_close_lfn( unsigned handle )
/*****************************************************/
{
#ifdef _M_I86
    return( __dos_find_close_lfn( handle ) );
#else
    call_struct     dpmi_rm;

    memset( &dpmi_rm, 0, sizeof( dpmi_rm ) );
    dpmi_rm.ebx = handle;
    dpmi_rm.eax = 0x71A1;
    return( __dpmi_dos_call_lfn( &dpmi_rm ) );
#endif
}

#endif /* __WATCOM_LFN__ */

_WCRTLINK unsigned _dos_findfirst( const char *path, unsigned attrib,
                                                           struct find_t *fdta )
/******************************************************************************/
{
#ifdef __WATCOM_LFN__
    DTALFN_SIGN_OF( fdta->reserved )   = 0;
    DTALFN_HANDLE_OF( fdta->reserved ) = 0;
    if( _RWD_uselfn ) {
        lfnfind_t   lfndta;
        lfn_ret_t   rc;

        rc = _dos_find_first_lfn( path, attrib, &lfndta );
        if( LFN_ERROR( rc ) ) {
            return( __set_errno_dos_reterr( LFN_INFO( rc ) ) );
        }
        if( LFN_OK( rc ) ) {
            convert_to_find_t( fdta, &lfndta );
            DTALFN_SIGN_OF( fdta->reserved )   = _LFN_SIGN;
            DTALFN_HANDLE_OF( fdta->reserved ) = LFN_INFO( rc );
            return( 0 );
        }
    }
#endif
    return( __dos_find_first_dta( path, attrib, fdta ) );
}


_WCRTLINK unsigned _dos_findnext( struct find_t *fdta )
/*****************************************************/
{
#ifdef __WATCOM_LFN__
    if( IS_LFN( fdta->reserved ) ) {
        lfnfind_t   lfndta;
        lfn_ret_t   rc;

        rc = _dos_find_next_lfn( DTALFN_HANDLE_OF( fdta->reserved ), &lfndta );
        if( LFN_ERROR( rc ) ) {
            return( __set_errno_dos_reterr( LFN_INFO( rc ) ) );
        }
        if( LFN_OK( rc ) ) {
            convert_to_find_t( fdta, &lfndta );
            return( 0 );
        }
    }
#endif
    return( __dos_find_next_dta( fdta ) );
}


_WCRTLINK unsigned _dos_findclose( struct find_t *fdta )
/******************************************************/
{
#if defined( __WATCOM_LFN__ )
    if( IS_LFN( fdta->reserved ) ) {
        lfn_ret_t   rc;

        rc = _dos_find_close_lfn( DTALFN_HANDLE_OF( fdta->reserved ) );
        if( LFN_ERROR( rc ) ) {
            return( __set_errno_dos_reterr( LFN_INFO( rc ) ) );
        }
        if( LFN_OK( rc ) ) {
            return( 0 );
        }
    }
#else

    /* unused parameters */ (void)fdta;

#endif
    return( 0 );
}
