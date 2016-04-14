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
* Description:  DWARF reader library virtual memory interface.
*
****************************************************************************/


extern unsigned_32      ReadLEB128( dr_handle *, bool );

extern void             DWRVMInit( void );
extern void             DWRVMDestroy( void );
extern void             DWRVMReset( void );
extern bool             DWRVMSectDone( dr_handle base, unsigned_32 size );
// DWRCurrNode must be set for alloc, free
extern dr_handle        DWRVMAlloc( unsigned long, int );
extern char             *DWRVMCopyString( dr_handle * );
extern unsigned         DWRVMGetStrBuff( dr_handle drstr, char *buf, unsigned max );
extern unsigned_16      DWRVMReadWord( dr_handle );
extern unsigned_32      DWRVMReadDWord( dr_handle );

#define DWRVMReadSLEB128(__h) (signed_32)ReadLEB128( __h, true )

#if defined( USE_VIRTMEM )

extern size_t           DWRVMStrLen( dr_handle );
extern void             DWRVMSwap( dr_handle, unsigned_32, bool *ret );
extern void             DWRVMSkipLEB128( dr_handle * );
extern void             DWRVMRead( dr_handle, void *, unsigned );
extern unsigned_8       DWRVMReadByte( dr_handle );

#define DWRVMReadULEB128(__h) ReadLEB128( __h, false )

#else   /* !USE_VIRTMEM */

#define DWRVMStrLen(__h)          strlen((const char *)__h)
#define DWRVMSwap(__ig1,__ig2,__ig3)
#define DWRVMSkipLEB128(__h)          \
    {                                 \
        const char *p = (const char *)*(__h); \
        while( (*p++ & 0x80) != 0 );  \
        *(__h) = (dr_handle)p;        \
    }
#define DWRVMRead(__h,__b,__l)  (void)memcpy( __b, (void *)__h, __l )
#define DWRVMReadByte(__h)      *((unsigned_8 *)(__h))

#if 0   // defined(__386__)   need to figure out

#define DWRVMReadULEB128(__h)   ReadULEB128( __h )

extern unsigned_32      ReadULEB128( dr_handle * );         /* inline */
/* warning -- this function only works if the ULEB128 is <= 0x0fffffff */
/* if greater numbers are used, the lowest nibble is zeroed. */
#pragma aux ReadULEB128 =                                           \
    "       push    ecx"                                            \
    "       push    esi"                                            \
                                                                    \
    "       mov     esi,[edx]"  /* esi points to start of buffer */ \
    "       mov     cl,27H"     /* #bits to rotate eax at end */    \
    "       xor     eax,eax"    /* clear eax */                     \
                                                                    \
    "L1:    sub     cl,07H"     /* 7 bits less to rotate at end */  \
    "       ror     eax,07H"    /* accept bottom seven bits */      \
    "       lodsb"              /* load next buffer byte into eax */\
    "       test    al,80H"     /* is sign bit on for last load? */ \
    "       jnz     L1"         /*   if so, continue loop */        \
                                                                    \
    "       ror     eax,cl"     /* rotate eax rest of way */        \
    "       mov     [edx],esi"  /* update buffer pointer */         \
                                                                    \
    "       pop     esi"                                            \
    "       pop     ecx"                                            \
        parm [edx] value [eax] modify nomemory

#else

#define DWRVMReadULEB128(__h)   ReadLEB128( __h, false )

#endif

#endif  /* !USE_VIRTMEM */
