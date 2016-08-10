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


#include <string.h>
#include "exephar.h"
#ifdef __LINUX__
#include <sys/mman.h>
#endif

typedef struct {
#ifdef __WATCOMC__
    unsigned long       sig;
#endif
    unsigned_8          init_rtn[1];    /* offset is start of routine */
} imp_header;


#define RELOC_BUFF_SIZE 64

static imp_header *ReadInImp( dig_fhandle dfh )
{
    simple_header       hdr;
    unsigned long       size;
    unsigned long       hdr_size;
    unsigned long       bss_size;
    unsigned            reloc_size;
    unsigned            bunch;
    unsigned            i;
    unsigned long       *fixup_loc;
    unsigned long       buff[RELOC_BUFF_SIZE];
    unsigned_8          *imp_start;

    if( DIGCliRead( dfh, &hdr, sizeof( hdr ) ) != sizeof( hdr ) )
        return( NULL );
    if( hdr.signature != REX_SIGNATURE )
        return( NULL );
    hdr_size = hdr.hdr_size * 16;
    size = (hdr.file_size * 0x200) - (-hdr.mod_size & 0x1ff) - hdr_size;
    bss_size = hdr.min_data * 4096;
    imp_start = DIGCliAlloc( size + bss_size );
    if( imp_start == NULL )
        return( NULL );
    DIGCliSeek( dfh, hdr_size, DIG_ORG );
    if( DIGCliRead( dfh, imp_start, size ) != size ) {
        DIGCliFree( imp_start );
        return( NULL );
    }
    DIGCliSeek( dfh, hdr.reloc_offset, DIG_ORG );
    while( hdr.num_relocs != 0 ) {
        bunch = hdr.num_relocs;
        if( bunch > RELOC_BUFF_SIZE )
            bunch = RELOC_BUFF_SIZE;
        reloc_size = bunch * sizeof( buff[0] );
        if( DIGCliRead( dfh, buff, reloc_size ) != reloc_size ) {
            DIGCliFree( imp_start );
            return( NULL );
        }
        for( i = 0; i < bunch; ++i ) {
            fixup_loc = (void *)(imp_start + (buff[i] & ~0x80000000));
            *fixup_loc += (unsigned long)imp_start;
        }
        hdr.num_relocs -= bunch;
    }
#ifdef __LINUX__
    /* On some platforms (such as AMD64 or x86 with NX bit), it is required
     * to map the code pages loaded from the BPD as executable, otherwise
     * a segfault will occur when attempting to run any BPD code.
     */
    mprotect((void*)((u_long)imp_start & ~4095), (size+4095) & ~4095, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
    memset( imp_start + size, 0, bss_size );
    return( (imp_header *)imp_start );
}
