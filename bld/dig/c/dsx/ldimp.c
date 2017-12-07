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
* Description:  Pharlap executable Loader (used by 32-bit code only)
*
*               used also for TRAP/DIP/MAD load on Linux/QNX/OSX host OS
*               until native shared libraries will be supported by OW
*
****************************************************************************/


/* Usage of the Prarlap executable Loader
 *
 *  Host OS     TRAP    MAD     DIP
 *
 *  DOS         -       yes     yes
 *  QNX         yes     yes     yes
 *  LINUX       -       yes     yes
 *  OSX         -       yes     yes
 */

#include <stdio.h>
#include <string.h>
#ifdef __LINUX__
#include <sys/mman.h>
#endif
#include "watcom.h"
#include "exephar.h"
#include "digtypes.h"
#include "digcli.h"
#include "digld.h"
#include "ldimp.h"


#define RELOC_BUFF_SIZE 64

imp_header *ReadInImp( FILE *fp )
{
    simple_header       hdr;
    unsigned            size;
    unsigned            hdr_size;
    unsigned            bss_size;
    unsigned            reloc_size;
    unsigned            bunch;
    unsigned            i;
    unsigned_32         *fixup_loc;
    unsigned            buff[RELOC_BUFF_SIZE];
    unsigned_8          *imp_start;

    if( DIGLoader( Read )( fp, &hdr, sizeof( hdr ) ) )
        return( NULL );
    if( hdr.signature != REX_SIGNATURE )
        return( NULL );
    hdr_size = hdr.hdr_size * 16;
    size = (hdr.file_size * 0x200) - (-hdr.mod_size & 0x1ff) - hdr_size;
    bss_size = hdr.min_data * 4096;
    imp_start = DIGCli( Alloc )( size + bss_size );
    if( imp_start == NULL )
        return( NULL );
    DIGLoader( Seek )( fp, hdr_size, DIG_ORG );
    if( DIGLoader( Read )( fp, imp_start, size ) ) {
        DIGCli( Free )( imp_start );
        return( NULL );
    }
    DIGLoader( Seek )( fp, hdr.reloc_offset, DIG_ORG );
    bunch = RELOC_BUFF_SIZE;
    reloc_size = RELOC_BUFF_SIZE * sizeof( buff[0] );
    while( hdr.num_relocs != 0 ) {
        if( RELOC_BUFF_SIZE > hdr.num_relocs ) {
            bunch = hdr.num_relocs;
            reloc_size = bunch * sizeof( buff[0] );
        }
        if( DIGLoader( Read )( fp, buff, reloc_size ) ) {
            DIGCli( Free )( imp_start );
            return( NULL );
        }
        for( i = 0; i < bunch; ++i ) {
            fixup_loc = (void *)(imp_start + (buff[i] & ~0x80000000));
            *fixup_loc += (unsigned_32)imp_start;
        }
        hdr.num_relocs -= bunch;
    }
#ifdef __LINUX__
    /* On some platforms (such as AMD64 or x86 with NX bit), it is required
     * to map the code pages loaded from the BPD as executable, otherwise
     * a segfault will occur when attempting to run any BPD code.
     */
    mprotect((void*)((u_long)imp_start & ~4095), ( size + 4095 ) & ~4095, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
    memset( imp_start + size, 0, bss_size );
    return( (imp_header *)imp_start );
}
