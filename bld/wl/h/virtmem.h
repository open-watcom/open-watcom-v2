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
* Description:  Linker virtual memory interface.
*
****************************************************************************/


#if defined( USE_VIRTMEM )

extern void ReadInfo( virt_mem, void *, virt_mem_size );
extern void PutInfo( virt_mem, void *, virt_mem_size );
extern void CopyInfo( virt_mem, virt_mem, unsigned );
extern void WriteInfoLoad( virt_mem, virt_mem_size );
extern void PutInfoNulls( virt_mem, virt_mem_size );
extern bool CompareInfo( virt_mem, void *, virt_mem_size );

#define GET32INFO( v, b ) ReadInfo( v, &(b), sizeof(unsigned_32) )
#define GET16INFO( v, b ) ReadInfo( v, &(b), sizeof(unsigned_16) )
#define PUT32INFO( v, b ) PutInfo( v, &(b), sizeof(unsigned_32) )
#define PUT16INFO( v, b ) PutInfo( v, &(b), sizeof(unsigned_16) )

#else

#define ReadInfo( v, b, l ) memcpy( b, v, l )
#define PutInfo( v, b, l ) memcpy( v, b, l )
#define CopyInfo( v1, v2, l ) memcpy( v1, v2, l )
#define WriteInfoLoad( v, l ) WriteLoad( v, l )
#define PutInfoNulls( v, l ) memset( v, 0, l )
#define CompareInfo( v, b, l ) (memcmp( v, b, l ) == 0)

#define GET32INFO( v, b ) b = *((unsigned_32 *)(v))
#define GET16INFO( v, b ) b = *((unsigned_16 *)(v))
#define PUT32INFO( v, b ) *((unsigned_32 *)(v)) = b
#define PUT16INFO( v, b ) *((unsigned_16 *)(v)) = b

#endif

extern void     VirtMemInit( void );
extern virt_mem AllocStg( virt_mem_size );
extern void     ReleaseInfo( virt_mem );
extern bool     SwapOutVirt( void );
extern void     FreeVirtMem( void );
