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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#if _OS == _NT || _OS == _OS2V2 && !defined(__OSI__)

extern void WriteLoad( void *, unsigned long );

#define ReadInfo( v, b, l ) memcpy( b, (void *)(v), l )
#define PutInfo( v, b, l ) memcpy( (void *)(v), b, l )
#define CopyInfo( v1, v2, l ) memcpy( (void *)(v1), (void *)(v2), l )
#define WriteInfo( v, l ) WriteLoad( (void *)(v), l )
#define PutNulls( v, l ) memset( (void *)(v), 0, l )
#define CompareInfo( v, b, l ) (memcmp( (void *)(v), b, l ) == 0)
#define GET32INFO( v, b ) b = *((unsigned_32 *)(v))
#define GET16INFO( v, b ) b = *((unsigned_16 *)(v))
#define PUT32INFO( v, b ) *((unsigned_32 *)(v)) = b
#define PUT16INFO( v, b ) *((unsigned_16 *)(v)) = b

#else

extern void ReadInfo( virt_mem, void *, unsigned );
extern void PutInfo( virt_mem, void *, unsigned );
extern void CopyInfo( virt_mem, virt_mem, unsigned );
extern void WriteInfo( virt_mem, unsigned long );
extern void PutNulls( virt_mem, unsigned long );
extern bool CompareInfo( virt_mem, void *, unsigned );

#define GET32INFO( v, b ) ReadInfo( v, &(b), sizeof(unsigned_32) )
#define GET16INFO( v, b ) ReadInfo( v, &(b), sizeof(unsigned_16) )
#define PUT32INFO( v, b ) PutInfo( v, &(b), sizeof(unsigned_32) )
#define PUT16INFO( v, b ) PutInfo( v, &(b), sizeof(unsigned_16) )

#endif

extern void     VirtMemInit( void );
extern virt_mem AllocStg( unsigned long );
extern void     ReleaseInfo( virt_mem );
extern bool     SwapOutVirt( void );
extern void     FreeVirtMem( void );
