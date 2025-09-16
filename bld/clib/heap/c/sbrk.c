/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of sbrk() for DOS and Windows.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#if defined(__WINDOWS__)
    #include <windows.h>
  #if defined(__WINDOWS_386__)
    #include "windpmi.h"
  #endif
#elif defined(__DOS__)
    #include <dos.h>
    #include "tinyio.h"
    #include "dpmi.h"
#endif
#include "roundmac.h"
#include "rtstack.h"
#include "seterrno.h"
#include "rtdata.h"
#include "heap.h"


#if defined( __DOS__ )

#ifdef _M_I86

extern  unsigned short SS_Reg( void );
#pragma aux SS_Reg = \
        "mov ax,ss"     \
    __parm __caller     [] \
    __value             [__ax] \
    __modify __exact    [__ax]

#else

extern  unsigned short  GetDS( void );
#pragma aux GetDS = \
        "mov    ax,ds"  \
    __parm __caller     [] \
    __value             [__ax] \
    __modify __exact    [__ax]

extern  int SegInfo( unsigned short selector );
#pragma aux SegInfo = \
        "mov    ah,0edH"    \
        __INT_21            \
        "shl    eax,31"     \
        "mov    ax,di"      \
    __parm __caller     [__ebx] \
    __value             [__eax] \
    __modify __exact    [__eax __ecx __edx __esi __ebx __edi]

extern void *CodeBuilderAlloc( unsigned );
#pragma aux CodeBuilderAlloc = \
        "mov  eax,80004800h" \
        __INT_21        \
        "sbb  ebx,ebx"  \
        "not  ebx"      \
        "and  eax,ebx"  \
    __parm __caller     [__ebx] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx]

#endif

#endif

#if defined( __WINDOWS__ )

_WCRTLINK void_nptr sbrk( int increment )
{
  #if defined( _M_I86 )
    HANDLE hmem;

    if( increment > 0 ) {
        hmem = LocalAlloc( LMEM_FIXED, increment );
        if( hmem != NULL ) {
            return( (void_nptr)hmem );
        }
        _RWD_errno = ENOMEM;
    } else {
        _RWD_errno = EINVAL;
    }
    return( (void_nptr)-1 );
  #else
    increment = __ROUND_UP_SIZE_4K( increment );
    return( (void_nptr)WDPMIAlloc( increment ) );
  #endif
}

#else       /* __DOS__ */

_WCRTLINK void_nptr __brk( unsigned brk_value )
{
    unsigned        old_brk_value;
    unsigned short  segm;
    unsigned        num_of_paras;

    if( brk_value < _STACKTOP ) {
        _RWD_errno = ENOMEM;
        return( (void_nptr)-1 );
    }
    segm = _DGroup();
  #ifdef _M_I86
    num_of_paras = __ROUND_UP_SIZE_TO_PARA( brk_value );
    if( num_of_paras == 0 ) {
        num_of_paras = PARAS_IN_64K;
    }
    /* try setting the block of memory */
    if( _osmode_REALMODE() ) {
        num_of_paras += SS_Reg() - _RWD_psp;    /* add in code size (in paragraphs) */
        segm = _RWD_psp;
    }
  #else
    if( _IsOS386() ) {
        int parent;

        num_of_paras = __ROUND_UP_SIZE_TO_PARA( brk_value );
        if( num_of_paras == 0 )
            num_of_paras = 0x0FFFFFFF;
        parent = SegInfo( segm );
        if( parent < 0 ) {
            if( TINY_ERROR( TinySetBlock( parent & 0xffff, num_of_paras ) ) ) {
                _RWD_errno = ENOMEM;
                return( (void_nptr)-1 );
            }
        }
    } else {        /* _IsPharLap() || IsRationalNonZeroBase() */
        num_of_paras = __ROUND_UP_SIZE_TO_4K( brk_value );
        if( num_of_paras == 0 )
            num_of_paras = 0x000FFFFF;
        if( _IsRationalNonZeroBase() ) {
            // convert from 4k pages to paragraphs
            num_of_paras *= 256U;
        }
    }
  #endif
    if( TINY_ERROR( TinySetBlock( segm, num_of_paras ) ) ) {
        _RWD_errno = ENOMEM;
        return( (void_nptr)-1 );
    }
    old_brk_value = _curbrk;        /* return old value of _curbrk */
    _curbrk = brk_value;            /* set new break value */

    return( (void_nptr)old_brk_value );
}

_WCRTLINK void_nptr sbrk( int increment )
{
  #ifdef __386__
    if( _IsRationalZeroBase()
      || _IsCodeBuilder() ) {
        void_nptr   cstg;

        if( increment > 0 ) {
            increment = __ROUND_UP_SIZE_4K( increment );
            if( _IsRational() ) {
                dpmi_mem_block  dpmimem;

                if( DPMIAllocateMemoryBlock( &dpmimem, increment ) ) {
                    cstg = NULL;
                } else {
                    cstg = (void_nptr)dpmimem.linear;
                    ((dpmi_hdr *)dpmimem.linear)->dpmi_handle = dpmimem.handle;
                }
            } else {
                cstg = CodeBuilderAlloc( increment );
            }
            if( cstg == NULL ) {
                _RWD_errno = ENOMEM;
                cstg = (void_nptr)-1;
            }
        } else {
            _RWD_errno = EINVAL;
            cstg = (void_nptr)-1;
        }
        return( cstg );
    } else if( _IsPharLap() ) {
        _curbrk = GetDataSelectorSize();
    }
  #endif
    return( __brk( _curbrk + increment ) );
}

#endif      /* __DOS__ */
