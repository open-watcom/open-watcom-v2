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
* Description:  Portable Executable dumping routines.
*
****************************************************************************/


#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "wdglb.h"
#include "wdfunc.h"


static  char    *pe_exe_msg[] = {
    "2cpu type                                        = ",
    "2number of object entries                        = ",
    "4time/date stamp                                 = ",
    "4symbol table                                    = ",
    "4number of symbols                               = ",
    "2no. of bytes in nt header following flags field = ",
    "2flags                                           = ",
    "2reserved                                        = ",
    "1link major version number                       = ",
    "1link minor version number                       = ",
    "4code size                                       = ",
    "4initialized data size                           = ",
    "4uninititialized data size                       = ",
    "4entrypoint rva                                  = ",
    "4code base                                       = ",
    "4data base                                       = ",
    "4image base                                      = ",
    "4object alignment: power of 2, 512 to 256M       = ",
    "4file alignment factor to align image pages      = ",
    "2os major version number                         = ",
    "2os minor version number                         = ",
    "2user major version number                       = ",
    "2user minor version number                       = ",
    "2subsystem major version number                  = ",
    "2subsystem minor version number                  = ",
    "4reserved1                                       = ",
    "4virtual size of the image                       = ",
    "4total header size                               = ",
    "4file checksum                                   = ",
    "2nt subsystem                                    = ",
    "2dll flags                                       = ",
    "4stack reserve size                              = ",
    "4stack commit size                               = ",
    "4size of local heap to reserve                   = ",
    "4heap commit size                                = ",
    "4address of tls index                            = ",
    "4number of tables                                = ",
    NULL
};

static  char    *pe_obj_msg[] = {
    "4          virtual memory size                = ",
    "4          relative virtual address           = ",
    "4          physical size of initialized data  = ",
    "4          physical offset for obj's 1st page = ",
    "4          relocs rva                         = ",
    "4          linnum rva                         = ",
    "2          number of relocs                   = ",
    "2          number of linnums                  = ",
    NULL
};

static  char    *PEHeadFlags[] = {
    "RELOCS_STRIPPED",
    "EXECUTABLE",
    "LINES_STRIPPED",
    "LOCALS_STRIPPED",
    "MINIMAL",
    "UPDATE",
    "16BIT",
    "LITTLE_ENDIAN",
    "32BIT",
    "DEBUG_STRIPPED",
    "PATCH",
    NULL,
    "SYSTEM",
    "DLL",
    NULL,
    "BIG_ENDIAN"
};

static  char    *PEObjFlags[] = {
    "DUMMY",
    "NOLOAD",
    "GROUPED",
    "NOPAD",
    "COPY",
    "CODE",
    "INIT_DATA",
    "UNINIT_DATA",
    "OTHER",
    "LINK_INFO",
    "OVERLAY",
    "REMOVE",
    "COMDAT",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "DISCARDABLE",
    "NOT_CACHED",
    "NOT_PAGED",
    "SHARED",
    "EXECUTABLE",
    "READABLE",
    "WRITABLE"
};

#define IN_RANGE( tbl, pe_obj )                         \
        (Pe_head.table[ tbl ].rva >= (pe_obj).rva       \
         && Pe_head.table[ tbl ].rva < ((pe_obj).rva+(pe_obj).physical_size))

#define PHYS_OFFSET( tbl, pe_obj )              \
        ((pe_obj).physical_offset + Pe_head.table[tbl].rva - (pe_obj).rva)

/*
 * Dump the NT Executable Header, if any.
 */
bool Dmp_pe_head( void )
/**********************/
{
    unsigned_16     i;
    unsigned_16     signature;

    Exp_off = 0;
    Imp_off = 0;
    Res_off = 0;
    Fix_off = 0;
    Wlseek( New_exe_off );
    Wread( &signature, sizeof( signature ) );
    switch( signature ) {
    case PE_SIGNATURE:
        Banner( "Windows NT EXE Header" );
        break;
    case PL_SIGNATURE:
        Banner( "PharLap TNT DosStyle Header" );
        break;
    default:
        return( 0 );
    }
    Wlseek( New_exe_off );
    Wread( &Pe_head, sizeof( pe_header ) );
    Wdputs( "file offset = " );
    Puthex( New_exe_off, 8 );
    Wdputslc( "H\n" );
    Wdputslc( "\n" );
    Dump_header( (char *)&Pe_head.cpu_type, pe_exe_msg );
    DumpCoffHdrFlags( Pe_head.flags );
    for( i = 0; i < Pe_head.num_tables; i++ ) {
        Wdputs( "  Table Entry  " );
        Putdecl( i, 2 );
        Wdputs( "    rva = " );
        Puthex( Pe_head.table[i].rva, 8 );
        Wdputs( "H    size = " );
        Puthex( Pe_head.table[i].size, 8 );
        Wdputslc( "H\n" );
    }
    Wdputslc( "\n" );
    Wlseek( New_exe_off + offsetof( pe_header, magic ) + Pe_head.nt_hdr_size );
    dmp_objects( Pe_head.num_objects );
    if( Exp_off != 0 ) {
        Dmp_exports();
    }
    if( Imp_off != 0 ) {
        Dmp_imports();
    }
    if( Res_off != 0 ) {
        Dmp_resources();
    }
    if( Fix_off != 0 ) {
        Dmp_fixups();
    }
    return( 1 );
}

static void DumpSection( pe_object *hdr )
/***************************************/
{
    coff_reloc          reloc;
    coff_line_num       linnum;
    int                 i;

    Wdputs( "Section data size = " );
    Puthex( hdr->physical_size, 8 );
    Wdputslc( "H\n" );
    if( Options_dmp & OS2_SEG_DMP ) {
        if( hdr->physical_offset != 0 ) {
            Dmp_seg_data( Coff_off + hdr->physical_offset, hdr->physical_size );
        }
        if( hdr->num_linnums > 0 ) {
            Wdputs( "Number of lines = " );
            Puthex( hdr->num_linnums, 8 );
            Wdputslc( "H\n" );
            Wlseek( Coff_off + hdr->linnum_rva );
            Wdputslc( "Idx/RVA  Num   Idx/RVA  Num   Idx/RVA  Num   Idx/RVA  Num   Idx/RVA  Num\n" );
            for( i = 0; i < hdr->num_linnums; i++ ) {
                Wread( &linnum, sizeof(coff_line_num) );
                Puthex( linnum.ir.RVA, 8 );
                Wdputc( ' ' );
                Putdecbz( linnum.line_number, 5 );
                if( i % 5 == 4 ) {
                    Wdputslc( "\n" );
                } else {
                    Wdputc( ' ' );
                }
            }
            if( i % 5 != 0 ) {
                Wdputslc( "\n" );
            }
        }
    }
    if( Options_dmp & FIX_DMP ) {
        Wlseek( Coff_off + hdr->relocs_rva );
        Wdputs( "Number of relocations = " );
        Puthex( hdr->num_relocs, 8 );
        Wdputslc( "H\n" );
        if( hdr->num_relocs != 0 ) {
            Wdputslc( "Offset   Sym Idx  Type  Offset   Sym Idx  Type  Offset   Sym Idx  Type\n" );
        }
        for( i = 0; i < hdr->num_relocs; i++ ) {
            Wread( &reloc, sizeof( coff_reloc ) );
            Puthex( reloc.offset, 8 );
            Wdputc( ' ' );
            Puthex( reloc.sym_tab_index, 8 );
            Wdputc( ' ' );
            Putdecbz( reloc.type, 5 );
            if( i % 3 == 2 ) {
                Wdputslc( "\n" );
            } else {
                Wdputc( ' ' );
            }
        }
        if( i % 3 != 0 ) {
            Wdputslc( "\n" );
        }
        Wdputslc( "\n" );
    }
}

extern void DumpCoffHdrFlags( unsigned_16 flags )
/***********************************************/
{
    DumpFlags( flags, 0, PEHeadFlags, "" );
}


static void DumpPEObjFlags( unsigned_32 flags )
/*********************************************/
{
    unsigned    alignval;
    char        buf[8];

    alignval = (flags & PE_OBJ_ALIGN_MASK) >> PE_OBJ_ALIGN_SHIFT;
    if( alignval != 0 ) {
        memcpy( buf, "ALIGN", 5 );
        utoa( 1 << (alignval - 1), buf + 5, 10 );
    } else {
        buf[0] = '\0';
    }
    DumpFlags( flags, PE_OBJ_ALIGN_MASK, PEObjFlags, buf );
}


/*
 * Dump the Object Table.
 */
extern void dmp_objects( unsigned num_objects )
/*********************************************/
{
    unsigned_16 i;
    pe_object * pe_obj;
    pe_object * start;
    char        save;

    if( num_objects == 0 ) return;
    Banner( "Section Table" );
    pe_obj = Wmalloc( num_objects * sizeof(pe_object) );
    Wread( pe_obj, num_objects * sizeof(pe_object) );
    start = pe_obj;
    for( i = 1; i <= num_objects; i++ ) {
        Wdputs( "object " );
        Putdec( i );
        Wdputs( ": name = " );
        save = pe_obj->name[8];
        pe_obj->name[8] = '\0';
        Wdputs(  pe_obj->name );
        if( pe_obj->name[0] == '/' ) {
            Wdputs( " (" );
            Wdputs( Coff_obj_name( pe_obj->name ) );
            Wdputs( ")" );
        }
        Wdputslc( "\n" );
        pe_obj->name[8] = save;
        Dump_header( (char *)&pe_obj->virtual_size, pe_obj_msg );
        DumpPEObjFlags( pe_obj->flags );
        Wdputslc( "\n" );
        if( Options_dmp & (OS2_SEG_DMP|FIX_DMP) ) {
            DumpSection( pe_obj );
        }
        if( pe_obj->physical_size != 0 ) {
            if( IN_RANGE( PE_TBL_EXPORT, *pe_obj ) ) {
                Exp_off = PHYS_OFFSET( PE_TBL_EXPORT, *pe_obj );
            }
            if( IN_RANGE( PE_TBL_IMPORT, *pe_obj ) ) {
                Imp_off = PHYS_OFFSET( PE_TBL_IMPORT, *pe_obj );
            }
            if( IN_RANGE( PE_TBL_RESOURCE, *pe_obj ) ) {
                Res_off = PHYS_OFFSET( PE_TBL_RESOURCE, *pe_obj );
            }
            if( IN_RANGE( PE_TBL_FIXUP, *pe_obj ) ) {
                Fix_off = PHYS_OFFSET( PE_TBL_FIXUP, *pe_obj );
            }
        }
        pe_obj++;
    }
    free( start );
}


/*
 * Dump the Pe export table (for .dll), if any.
 */
bool Dmp_pe_tab( void )
/*********************/
{
    unsigned_32     cur_pos;        /* current offset position */
    unsigned_16     i;
    pe_object       pe_obj;

    Wread( &Dos_head, sizeof( Dos_head ) );
    if( Dos_head.signature != DOS_SIGNATURE ) {
        return( 0 );
    }
    if( Dos_head.reloc_offset != OS2_EXE_HEADER_FOLLOWS ) {
        return( 0 );
    }
    Wlseek( OS2_NE_OFFSET );
    Wread( &New_exe_off, sizeof( New_exe_off ) );
    Wlseek( New_exe_off );
    Wread( &Pe_head, sizeof( pe_header ) );
    switch( Pe_head.signature ) {
    case PE_SIGNATURE:
    case PL_SIGNATURE:
        break;
    default:
        return( 0 );
    }
    Exp_off = 0;
    cur_pos = New_exe_off  + sizeof( Pe_head );
    for( i = 0; i < Pe_head.num_objects; i++ ) {
        Wlseek( cur_pos );
        Wread( &pe_obj, sizeof( pe_object ) );
        cur_pos += sizeof( pe_object );
        if( Pe_head.table[ PE_TBL_EXPORT ].rva >= pe_obj.rva
         && Pe_head.table[ PE_TBL_EXPORT ].rva < (pe_obj.rva+pe_obj.physical_size) ) {
            Exp_off = pe_obj.physical_offset
                    + Pe_head.table[PE_TBL_EXPORT].rva - pe_obj.rva;
        }
    }
    if( Exp_off != 0 ) {
        Dmp_exp_tab();
    } else {
        Wdputslc( "no export table\n" );
    }
    return( 1 );
}
