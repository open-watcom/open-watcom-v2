/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS LFN specific constant definitions.
*
****************************************************************************/


#ifndef __DOSLFN_H_INCLUDED
#define __DOSLFN_H_INCLUDED

#include <dos.h>
#include "tinyio.h"
#include "rtdata.h"


#define EX_LFN_OPEN         0x01
#define EX_LFN_CREATE       0x12
#define EX_LFN_CREATE_NEW   0x10

#define _XCHG_AX_DX         0x92
#define _XCHG_AX_SI         0x96

#ifdef _M_I86
  #ifdef __BIG_DATA__
    #define _SET_DSDX       _PUSH_DS _XCHG_AX_DX _MOV_DS_AX
    #define _SET_DSSI       _PUSH_DS _XCHG_AX_SI _MOV_DS_AX
    #define _SET_ES
    #define _RST_DS         _POP_DS
    #define _RST_ES
  #else
    #define _SET_DSDX
    #define _SET_DSSI
    #define _SET_ES         _PUSH_ES _PUSH_DS _POP_ES
    #define _RST_DS
    #define _RST_ES         _POP_ES
  #endif
#else
    #define _SET_DSDX
    #define _SET_DSSI
    #define _SET_ES
    #define _RST_DS
    #define _RST_ES
#endif

#include "pushpck1.h"
/* The find block for the LFN find */
typedef struct {
    long            attributes;
    unsigned short  creattime;
    unsigned short  creatdate;
    long            cr_pad;
    unsigned short  accesstime;
    unsigned short  accessdate;
    long            ac_pad;
    unsigned short  wrtime;
    unsigned short  wrdate;
    long            wr_pad;
    long            hfilesize;
    long            lfilesize;
    long            reserved[2];
    char            lfn[NAME_MAX + 1];
    char            sfn[14];
} lfnfind_t;

typedef struct {
    long        attributes;
    long long   creattimestamp;
    long long   accesstimestamp;
    long long   writetimestamp;
    long        volid;
    long        hfilesize;
    long        lfilesize;
    long        linksno;
    long        hfileid;
    long        lfileid;
} lfninfo_t;
#include "poppck.h"

extern unsigned __doserror_( unsigned );
#pragma aux __doserror_ "*" __parm __caller

extern unsigned __doserror1_( unsigned );
#pragma aux __doserror1_ "*" __parm __caller

#ifdef _M_I86

extern tiny_ret_t __dos_create_ex_lfn( const char *name, unsigned mode, unsigned attrib, unsigned action );
  #ifdef __BIG_DATA__
    #pragma aux __dos_create_ex_lfn = \
            "push   ds"         \
            "xchg   ax,si"      \
            "mov    ds,ax"      \
            "mov    ax,716Ch"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
        __parm __caller     [__si __ax] [__bx] [__cx] [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __cx __dx __si]
  #else
    #pragma aux __dos_create_ex_lfn = \
            "mov    ax,716Ch"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
        __parm __caller     [__si] [__bx] [__cx] [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __cx __dx]
  #endif

extern tiny_ret_t __dos_find_first_lfn( const char *path, unsigned attr, lfnfind_t __far *lfndta );
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
            "sbb    dx,dx"      \
        __parm __caller     [__dx __ax] [__cx] [__es __di] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __cx __dx __si]
  #else
    #pragma aux __dos_find_first_lfn = \
            "mov    si,1"       \
            "mov    ax,714Eh"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
        __parm __caller     [__dx] [__cx] [__es __di] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __cx __dx __si]
  #endif

extern tiny_ret_t __dos_find_next_lfn( unsigned handle, lfnfind_t __far *lfndta );
#pragma aux __dos_find_next_lfn = \
        "mov    si,1"       \
        "mov    ax,714fh"   \
        "stc"               \
        "int 21h"           \
        "sbb    dx,dx"      \
        "jnz short L1"      \
        "xor    ax,ax"      \
    "L1:"                   \
    __parm __caller     [__bx] [__es __di] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __cx __dx __si]

extern tiny_ret_t __dos_find_close_lfn( unsigned handle );
#pragma aux __dos_find_close_lfn = \
        "mov    ax,71A1h"   \
        "stc"               \
        "int 21h"           \
        "sbb    dx,dx"      \
        "jnz short L1"      \
        "xor    ax,ax"      \
    "L1:"                   \
    __parm __caller     [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

extern tiny_ret_t __dos_getfileattr_lfn( const char *path );
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
            "jnz short L1"      \
            "mov    ax,cx"      \
        "L1:"                   \
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
            "jnz short L1"      \
            "mov    ax,cx"      \
        "L1:"                   \
        __parm __caller     [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __bl __cx __dx]
  #endif

extern tiny_ret_t __dos_setfileattr_lfn( const char *path, unsigned attr );
  #ifdef __BIG_DATA__
    #pragma aux __dos_setfileattr_lfn = \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    bl,1"       \
            "mov    ax,7143h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx __ax] [__cx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __bl __dx]
  #else
    #pragma aux __dos_setfileattr_lfn = \
            "mov    bl,1"       \
            "mov    ax,7143h"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx] [__cx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __bl __dx]
  #endif

extern tiny_ret_t ___getdcwd_lfn( char *path, unsigned char drv );
  #ifdef __BIG_DATA__
    #pragma aux ___getdcwd_lfn = \
            "push   ds"         \
            "mov    ds,cx"      \
            "mov    ax,7147h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__cx __si] [__dl] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux ___getdcwd_lfn = \
            "mov    ax,7147h"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__si] [__dl] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif

extern tiny_ret_t __chdir_lfn( const char *path );
  #ifdef __BIG_DATA__
    #pragma aux __chdir_lfn =   \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    ax,713Bh"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux __chdir_lfn =   \
            "mov    ax,713Bh"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif

extern tiny_ret_t __mkdir_lfn( const char *path );
  #ifdef __BIG_DATA__
    #pragma aux __mkdir_lfn =   \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    ax,7139h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux __mkdir_lfn =   \
            "mov    ax,7139h"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif

extern tiny_ret_t __rmdir_lfn( const char *path );
  #ifdef __BIG_DATA__
    #pragma aux __rmdir_lfn =   \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    ax,713Ah"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux __rmdir_lfn =   \
            "mov    ax,713Ah"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif

extern tiny_ret_t __rename_lfn( const char *old, const char *new );
  #ifdef __BIG_DATA__
    #pragma aux __rename_lfn =  \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    ax,7156h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx __ax] [__es __di] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux __rename_lfn =  \
            "push   es"         \
            "mov    ax,ds"      \
            "mov    es,ax"      \
            "mov    ax,7156h"   \
            "stc"               \
            "int 21h"           \
            "pop    es"         \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx] [__di] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif

extern tiny_ret_t __dos_utime_lfn( const char *path, unsigned time, unsigned date, unsigned mode );
  #ifdef __BIG_DATA__
    #pragma aux __dos_utime_lfn = \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    ax,7143h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx __ax] [__cx] [__di] [__bx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux __dos_utime_lfn = \
            "mov    ax,7143h"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx] [__cx] [__di] [__bx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif

extern tiny_ret_t __unlink_lfn( const char *filename );
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
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx __si]
  #else
    #pragma aux __unlink_lfn =  \
            "xor    si,si"      \
            "mov    ax,7141h"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx __si]
  #endif

extern tiny_ret_t __getfileinfo_lfn( int handle, lfninfo_t *lfninfo );
  #ifdef __BIG_DATA__
    #pragma aux __getfileinfo_lfn = \
            "push   ds"         \
            "xchg   ax,dx"      \
            "mov    ds,ax"      \
            "mov    ax,71A6h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__bx] [__dx __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #else
    #pragma aux __getfileinfo_lfn = \
            "mov    ax,71A6h"   \
            "stc"               \
            "int 21h"           \
            "sbb    dx,dx"      \
            "jnz short L1"      \
            "xor    ax,ax"      \
        "L1:"                   \
        __parm __caller     [__bx] [__dx] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __dx]
  #endif

extern long __cvt_stamp2dos_lfn( long long *timestamp );
  #ifdef __BIG_DATA__
    #pragma aux __cvt_stamp2dos_lfn = \
            "push   ds"         \
            "xchg   ax,si"      \
            "mov    ds,ax"      \
            "xor    bx,bx"      \
            "mov    ax,71A7h"   \
            "stc"               \
            "int 21h"           \
            "pop    ds"         \
            "jnc short L1"      \
            "call __doserror_"  \
            "mov    cx,-1"      \
            "mov    dx,cx"      \
        "L1: mov    ax,cx"      \
        __parm __caller     [__si __ax] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __bx __cx __dx __si]
  #else
    #pragma aux __cvt_stamp2dos_lfn = \
            "xor    bx,bx"      \
            "mov    ax,71A7h"   \
            "stc"               \
            "int 21h"           \
            "jnc short L1"      \
            "call __doserror_"  \
            "mov    cx,-1"      \
            "mov    dx,cx"      \
        "L1: mov    ax,cx"      \
        __parm __caller     [__si] \
        __value             [__dx __ax] \
        __modify __exact    [__ax __bx __cx __dx]
  #endif

#endif

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

#define RETURN_VALUE        \
        "jc short LX"       \
        SAVE_VALUE          \
    "LX:"

#define _LFN_SIGN           0x004e464cUL    // "LFN"

#define IS_LFN_ERROR(x)     ((int_32)(x) < 0 && (x)!= 0xFFFF7100)

#define IS_LFN(x)           (_RWD_uselfn && DTALFN_SIGN_OF(x) == _LFN_SIGN && DTALFN_HANDLE_OF(x))

#define TINY_RET_ERROR(e)   ((e) | ~0xFFFF)

#if defined( __WATCOM_LFN__ ) && !defined( _M_I86 )

extern char             * const __lfn_rm_tb_linear;
extern unsigned short   const __lfn_rm_tb_segment;

#define RM_TB_PARM1_SIZE    ( MB_CUR_MAX * _MAX_PATH )
#define RM_TB_PARM2_SIZE    ( MB_CUR_MAX * _MAX_PATH )
#define RM_TB_PARM1_SEGM    __lfn_rm_tb_segment
#define RM_TB_PARM1_OFFS    0
#define RM_TB_PARM1_LINEAR  __lfn_rm_tb_linear
#define RM_TB_PARM2_SEGM    __lfn_rm_tb_segment
#define RM_TB_PARM2_OFFS    RM_TB_PARM1_SIZE
#define RM_TB_PARM2_LINEAR  (__lfn_rm_tb_linear + RM_TB_PARM1_SIZE)

extern unsigned __dpmi_dos_call( call_struct __far *cs );
#pragma aux __dpmi_dos_call = \
        "push   es"         \
        "mov    es,edx"     \
        "xor    ecx,ecx"    \
        "mov    bx,21h"     \
        "mov    ax,300h"    \
        "int 31h"           \
        "pop    es"         \
        "sbb    eax,eax"    \
    __parm __caller     [__dx __edi] \
    __value             [__eax] \
    __modify __exact    [__eax __bx __ecx]

#endif

#endif
