/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DWARF reader library virtual memory interface.
*
****************************************************************************/


extern void             DR_VMInit( void );
extern void             DR_VMDestroy( void );
extern void             DR_VMReset( void );
extern bool             DR_VMSectDone( drmem_hdl base, unsigned_32 size );
// DR_CurrNode must be set for alloc, free
extern drmem_hdl        DR_VMAlloc( unsigned long, int );
extern void             DR_VMFree( drmem_hdl hdl );
extern char             *DR_VMCopyString( drmem_hdl * );
extern size_t           DR_VMGetStrBuff( drmem_hdl drstr, char *buf, size_t max );
extern unsigned_16      DR_VMReadWord( drmem_hdl );
extern unsigned_32      DR_VMReadDWord( drmem_hdl );
extern int_64           DR_VMReadSLEB128( drmem_hdl *vmptr );
extern uint_64          DR_VMReadULEB128( drmem_hdl *vmptr );

#if defined( USE_VIRTMEM )

extern size_t           DR_VMStrLen( drmem_hdl );
extern void             DR_VMSwap( drmem_hdl, unsigned_32, bool *ret );
extern void             DR_VMSkipLEB128( drmem_hdl * );
extern void             DR_VMRead( drmem_hdl, void *, size_t );
extern unsigned_8       DR_VMReadByte( drmem_hdl );

#else   /* !USE_VIRTMEM */

#define DR_VMStrLen(__h)        strlen((const char *)__h)
#define DR_VMSwap(__ig1,__ig2,__ig3)
#define DR_VMSkipLEB128(__h)          \
    {                                 \
        const char *p = (const char *)*(__h); \
        while( (*p++ & 0x80) != 0 );  \
        *(__h) = (drmem_hdl)p;        \
    }
#define DR_VMRead(__h,__b,__l)  (void)memcpy( __b, (void *)__h, __l )
#define DR_VMReadByte(__h)      *((unsigned_8 *)(__h))

#if 0   // defined(__386__)   need to figure out

#define DR_VMReadULEB128(__h)   ReadULEB128( __h )

extern unsigned_32      ReadULEB128( drmem_hdl * );         /* inline */
/* warning -- this function only works if the ULEB128 is <= 0x0fffffff */
/* if greater numbers are used, the lowest nibble is zeroed. */
#pragma aux ReadULEB128 =                                           \
        "push    ecx"                                            \
        "push    esi"                                            \
                                                                    \
        "mov     esi,[edx]"  /* esi points to start of buffer */ \
        "mov     cl,27H"     /* #bits to rotate eax at end */    \
        "xor     eax,eax"    /* clear eax */                     \
                                                                    \
    "L1: sub     cl,07H"     /* 7 bits less to rotate at end */  \
        "ror     eax,07H"    /* accept bottom seven bits */      \
        "lodsb"              /* load next buffer byte into eax */\
        "test    al,80H"     /* is sign bit on for last load? */ \
        "jnz     L1"         /*   if so, continue loop */        \
                                                                    \
        "ror     eax,cl"     /* rotate eax rest of way */        \
        "mov     [edx],esi"  /* update buffer pointer */         \
                                                                    \
        "pop     esi"                                            \
        "pop     ecx"                                            \
    __parm          [__edx] \
    __value         [__eax] \
    __modify __nomemory

#endif

#endif  /* !USE_VIRTMEM */
