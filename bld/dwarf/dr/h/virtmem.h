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


extern void      DWRVMInit( void );
extern void      DWRVMDestroy( void );
extern void      DWRVMReset( void );
extern int       DWRVMSectDone( dr_handle base, unsigned_32 size );

// DWRCurrNode must be set for alloc, free
extern dr_handle DWRVMAlloc( unsigned long, int );

extern unsigned  DWRStrLen( dr_handle );
extern void      DWRGetString( char *, dr_handle * );
extern unsigned  DWRGetStrBuff(  dr_handle drstr, char *buf, unsigned max );

#if defined( INLINE_VMEM )


#define DWRVMSwap( __ig1, __ig2, __ig3 ) ((void)((__ig1) == (__ig2) == *(__ig3)))  /* ignored */

#define DWRVMRead( __hdl, __blk, __len ) ( (void) memcpy( (void *) __blk, (const void *) __hdl, (unsigned) __len ) )
#define DWRVMReadByte( __hdl )           ( *((unsigned_8 *) (__hdl)) )
#define DWRVMReadSLEB128( __hdlp )       ( (signed_32)  ReadLEB128( (dr_handle *) (__hdlp), TRUE ) )
#define DWRVMSkipLEB128( __hdl )                        \
         { unsigned_8 *p = (unsigned_8 *)*(__hdl);      \
                do { } while( *p++ & 0x80 );            \
                *(__hdl) = (dr_handle)p;                \
         }

extern unsigned_16      DWRVMReadWord( dr_handle );
extern unsigned_32      DWRVMReadDWord( dr_handle );
extern unsigned_32      ReadLEB128( dr_handle *, bool );

#if   0  // defined(__386__)   need to figure out

extern unsigned_32      ReadULEB128( dr_handle * );         /* inline */

#define DWRVMReadULEB128( __hdlp )       ( (uint_32) ReadULEB128( (dr_handle *) (__hdlp) ) )

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

#define DWRVMReadULEB128( __hdlp )       ( (signed_32)  ReadLEB128( (dr_handle *) (__hdlp), FALSE ) )

#endif

#else

extern void        DWRVMRead( dr_handle, void *, unsigned );
extern unsigned_8  DWRVMReadByte( dr_handle );
extern unsigned_16 DWRVMReadWord( dr_handle );
extern unsigned_32 DWRVMReadDWord( dr_handle );
extern signed_32   DWRVMReadSLEB128( dr_handle * );
extern unsigned_32 DWRVMReadULEB128( dr_handle * );
extern void        DWRVMSkipLEB128( dr_handle * );
extern void        DWRVMSwap( dr_handle, unsigned_32, int *ret );


#endif // INLINE_VMEM
