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
* Description:  Memory management routines for linker.
*
****************************************************************************/


#define DUPBUF_STACK(p,s,l) {void *x=alloca(l);memcpy(x,s,l);p=x;}
#define DUPSTR_STACK(p,s,l) {char *x=alloca(l+1);memcpy(x,s,l);x[l]='\0';p=x;}

extern void     *ChkLAlloc( size_t );
extern void     *LAlloc( size_t );
extern void     *TryAlloc( size_t );
extern void     *LnkReAlloc( void *, size_t );
extern void     LFree( void * );
extern void     *PermAlloc( size_t );
extern void     *Pass1Alloc( size_t );
extern void     *LnkExpand( void *, size_t );

extern void     LnkMemInit( void );
extern void     LnkMemFini( void );
extern bool     FreeUpMemory( void );
extern void     MakePass1Blocks( void );
extern bool     PermShrink( void );
extern void     ReleasePass1( void );

#define _ChkAlloc( dest, size )         dest = ChkLAlloc( size )
#define _LnkAlloc( dest, size )         dest = LAlloc( size )
#define _TryAlloc( dest, size )         dest = TryAlloc( size )
#define _PermAlloc( dest, size )        dest = PermAlloc( size );
#define _LnkReAlloc( dest, src, size )  dest = LnkReAlloc( src, size );
#define _LnkFree( ptr )                 LFree( ptr )
#define _PermFree( ptr )                /* nothing to do */
#define _Pass1Alloc( dest, size )       dest = Pass1Alloc( size );
#define _LnkExpand( dest, src, size )   dest = LnkExpand( src, size );

extern int      ValidateMem( void );    // just for debugging

#ifndef NDEBUG
    void DbgZapAlloc( void* tgt, size_t size );
    void DbgZapFreed( void* tgt, size_t size );
#else
    #define DbgZapAlloc( tgt, size )
    #define DbgZapFreed( tgt, size )
#endif
