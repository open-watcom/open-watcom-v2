/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Memory management prototypes for wmake.
*
****************************************************************************/


#ifndef _MEMORY_H
#define _MEMORY_H   1

#include "memfuncs.h"

extern void MemInit( void );
extern void MemFini( void );
extern void MemShrink( void );

extern void *MemCAllocSafe( size_t size ); /* different from stdlib rtn */
extern char *CharToStringSafe( char c );

#ifdef USE_FAR

extern void         FAR *FarMemAlloc( size_t size );
extern void         FAR *FarMemAllocSafe( size_t size );
extern void         FarMemFree( void FAR *p );

#define FarMemCpy       _fmemcpy
#define FarMemCmp       _fmemcmp
#define FarMemSet       _fmemset

#else

#define FarMemAlloc     MemAlloc
#define FarMemAllocSafe MemAllocSafe
#define FarMemFree      MemFree

#define FarMemCpy       memcpy
#define FarMemCmp       memcmp
#define FarMemSet       memset

#endif

#ifdef USE_SCARCE

extern void IfMemScarce( bool (*func)( void ) );

#endif

/*
 * Inline memcpy of any constant number of bytes of near or far memory.
 * The prototype would be:  void ConstMemCpy( void *, void *, size_t );
 * Note that each argument is evaluated only once.
 */
#define ConstMemCpy( d, s, c )                      \
    {                                               \
        typedef struct {                            \
            char _b[ c ];                           \
        } FAR *_cmc_ptr;                            \
        *( _cmc_ptr )( d ) = *( _cmc_ptr )( s );    \
    }

#endif  /* !_MEMORY_H */
