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
* Description:  Memory management prototypes for wmake.
*
****************************************************************************/


#ifndef _MEMORY_H
#define _MEMORY_H   1

extern void *MallocUnSafe( size_t size );
extern void *MallocSafe( size_t size );
extern void *CallocSafe( size_t size ); /* different from stdlib rtn */
extern void FreeSafe( void *ptr );
extern char *StrDupSafe( const char *str );
extern void MemInit( void );
extern void MemFini( void );
extern void MemShrink( void );

#ifdef USE_FAR

extern void FAR *FarMallocUnSafe( size_t size );
extern void FAR *FarMallocSafe( size_t size );
extern void     FarFreeSafe( void FAR *p );

#define FarMemCpy           _fmemcpy
#define FarMemCmp           _fmemcmp
#define FarMemSet           _fmemset

#else

#define FarMallocUnSafe     MallocUnSafe
#define FarMallocSafe       MallocSafe
#define FarFreeSafe         FreeSafe

#define FarMemCpy           memcpy
#define FarMemCmp           memcmp
#define FarMemSet           memset

#endif

#ifdef USE_SCARCE

extern void IfMemScarce( RET_T (*func)( void ) );

#endif

/*
 * Inline memcpy of any constant number of bytes of near or far memory.
 * The prototype would be:  void ConstMemCpy( void *, void *, size_t );
 * Note that each argument is evaluated only once.
 */
#define ConstMemCpy( d, s, c )                       \
     {                                               \
         typedef struct {                            \
             char _b[ c ];                           \
         } FAR *_cmc_ptr;                            \
         *( _cmc_ptr )( d ) = *( _cmc_ptr )( s );    \
     }

#endif  /* !_MEMORY_H */
