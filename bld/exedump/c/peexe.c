/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#define IN_RANGE(r,o)       (r >= (o).rva && r < ((o).rva + (o).physical_size))

#define PHYS_OFFSET(r,o)    ((o).physical_offset + r - (o).rva)

static  const_string_table pe32_exe_msg1[] = {
    "2cpu type (32-bit)                               = ",
    NULL
};
static  const_string_table pe64_exe_msg1[] = {
    "2cpu type (64-bit)                               = ",
    NULL
};

static  const_string_table pe_exe_msg1[] = {
    "2number of object entries                        = ",
    "4time/date stamp                                 = ",
    "4symbol table                                    = ",
    "4number of symbols                               = ",
    "2no. of bytes in nt header following flags field = ",
    "2flags                                           = ",
    NULL
};

static  const_string_table pe32_exe_msg2[] = {
    "2magic (32-bit)                                  = ",
    NULL
};
static  const_string_table pe64_exe_msg2[] = {
    "2magic (64-bit)                                  = ",
    NULL
};

static  const_string_table pe_exe_msg2[] = {
    "1link major version number                       = ",
    "1link minor version number                       = ",
    "4code size                                       = ",
    "4initialized data size                           = ",
    "4uninititialized data size                       = ",
    "4entrypoint rva                                  = ",
    "4code base                                       = ",
    NULL
};

static  const_string_table pe32_exe_msg3[] = {
    "4data base                                       = ",
    "4image base                                      = ",
    NULL
};
static  const_string_table pe64_exe_msg3[] = {
    "8image base                                      = ",
    NULL
};

static  const_string_table pe_exe_msg3[] = {
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
    NULL
};

static  const_string_table pe32_exe_msg4[] = {
    "4stack reserve size                              = ",
    "4stack commit size                               = ",
    "4size of local heap to reserve                   = ",
    "4heap commit size                                = ",
    "4address of tls index                            = ",
    "4number of tables                                = ",
    NULL
};

static  const_string_table pe64_exe_msg4[] = {
    "8stack reserve size                              = ",
    "8stack commit size                               = ",
    "8size of local heap to reserve                   = ",
    "8heap commit size                                = ",
    "4address of tls index                            = ",
    "4number of tables                                = ",
    NULL
};

static  const_string_table pe_obj_msg[] = {
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

static  const_string_table PEHeadFlags[] = {
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

static  const_string_table PEObjFlags[] = {
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

/*
 * Dump the NT Executable Header, if any.
 */
bool Dmp_pe_head( void )
/**********************/
{
    unsigned_16         i;
    unsigned_32         num_tables;
    pe_dir_entry        *tbl_entry;
    unsigned_32         offset;
    const unsigned char *data;

    Exp_off = 0;
    Imp_off = 0;
    Res_off = 0;
    Fix_off = 0;
    Wlseek( New_exe_off );
    Wread( &Pe_head, PE_HDR_SIZE );
    switch( Pe_head.signature ) {
    case EXESIGN_PE:
        Banner( "Windows NT EXE Header" );
        break;
    case EXESIGN_PL:
        Banner( "PharLap TNT DosStyle Header" );
        break;
    default:
        return( false );
    }
    Wread( (char *)&Pe_head + PE_HDR_SIZE, PE_OPT_SIZE( Pe_head ) );
    Wdputs( "file offset = " );
    Puthex( New_exe_off, 8 );
    Wdputslc( "H\n" );
    Wdputslc( "\n" );
    data = (const unsigned char *)&Pe_head.fheader.cpu_type;
    if( IS_PE64( Pe_head ) ) {
        Dump_header( data, pe64_exe_msg1, 8 );
        data += 2;
        Dump_header( data, pe_exe_msg1,   8 );
        data += 18;
        Dump_header( data, pe64_exe_msg2, 8 );
        data += 2;
        Dump_header( data, pe_exe_msg2,   8 );
        data += 22;
        Dump_header( data, pe64_exe_msg3, 8 );
        data += 8;
        Dump_header( data, pe_exe_msg3,   8 );
        data += 40;
        Dump_header( data, pe64_exe_msg4, 8 );
        DumpCoffHdrFlags( Pe_head.fheader.flags );
        tbl_entry = PE64( Pe_head ).table;
        num_tables = PE64( Pe_head ).num_tables;
    } else {
        Dump_header( data, pe32_exe_msg1, 4 );
        data += 2;
        Dump_header( data, pe_exe_msg1,   4 );
        data += 18;
        Dump_header( data, pe32_exe_msg2, 4 );
        data += 2;
        Dump_header( data, pe_exe_msg2,   4 );
        data += 22;
        Dump_header( data, pe32_exe_msg3, 4 );
        data += 8;
        Dump_header( data, pe_exe_msg3,   4 );
        data += 40;
        Dump_header( data, pe32_exe_msg4, 4 );
        DumpCoffHdrFlags( Pe_head.fheader.flags );
        tbl_entry = PE32( Pe_head ).table;
        num_tables = PE32( Pe_head ).num_tables;
    }
    for( i = 0; i < num_tables; i++ ) {
        Wdputs( "  Table Entry  " );
        Putdecl( i, 2 );
        Wdputs( "    rva = " );
        Puthex( tbl_entry->rva, 8 );
        Wdputs( "H    size = " );
        Puthex( tbl_entry->size, 8 );
        if( Pe_head.signature == EXESIGN_PE ) {
            switch( i ) {
            case 0: Wdputslc( "H   (Export Directory)\n" ); break;
            case 1: Wdputslc( "H   (Import Directory)\n" ); break;
            case 2: Wdputslc( "H   (Resource Directory)\n" ); break;
            case 3: Wdputslc( "H   (Exception Directory)\n" ); break;
            case 4: Wdputslc( "H   (Certificates Directory)\n" ); break;
            case 5: Wdputslc( "H   (Base Relocation Directory)\n" ); break;
            case 6: Wdputslc( "H   (Debug Directory)\n" ); break;
            case 7: Wdputslc( "H   (Architecture Directory)\n" ); break;
            case 8: Wdputslc( "H   (Global Pointer Directory)\n" ); break;
            case 9: Wdputslc( "H   (Thread Storage Directory)\n" ); break;
            case 10: Wdputslc( "H   (Load Configuration Directory)\n" ); break;
            case 11: Wdputslc( "H   (Bound Import Directory)\n" ); break;
            case 12: Wdputslc( "H   (Import Address Table Directory)\n" ); break;
            case 13: Wdputslc( "H   (Delay Import Directory)\n" ); break;
            case 14: Wdputslc( "H   (COM Descriptor Directory)\n" ); break;
            case 15: Wdputslc( "H   (Reserved Directory)\n" ); break;
            default: Wdputslc( "H\n" ); break;
            }
        } else {
            Wdputslc( "H\n" );
        }
        tbl_entry++;
    }
    Wdputslc( "\n" );
    offset = New_exe_off + PE_SIZE( Pe_head );
    Wlseek( offset );
    dmp_objects( Pe_head.fheader.num_objects );
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
    return( true );
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
            Puthex( reloc.sym_tab_index + 1, 8 );
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

void DumpCoffHdrFlags( unsigned_16 flags )
/****************************************/
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
        sprintf( buf, "ALIGN%u", 1 << (alignval - 1) );
    } else {
        buf[0] = '\0';
    }
    DumpFlags( flags, PE_OBJ_ALIGN_MASK, PEObjFlags, buf );
}


/*
 * Dump the Object Table.
 */
void dmp_objects( unsigned num_objects )
/**************************************/
{
    unsigned_16 i;
    pe_object   *pe_obj;
    pe_object   *start;
    char        pe_obj_name[PE_OBJ_NAME_LEN + 1];
    unsigned_32 export_rva;
    unsigned_32 import_rva;
    unsigned_32 resource_rva;
    unsigned_32 fixup_rva;

    if( num_objects == 0 )
        return;
    Banner( "Section Table" );
    pe_obj = Wmalloc( num_objects * sizeof(pe_object) );
    Wread( pe_obj, num_objects * sizeof(pe_object) );
    start = pe_obj;
    export_rva = PE_DIRECTORY( Pe_head, PE_TBL_EXPORT ).rva;
    import_rva = PE_DIRECTORY( Pe_head, PE_TBL_IMPORT ).rva;
    resource_rva = PE_DIRECTORY( Pe_head, PE_TBL_RESOURCE ).rva;
    fixup_rva = PE_DIRECTORY( Pe_head, PE_TBL_FIXUP ).rva;
    for( i = 1; i <= num_objects; i++ ) {
        Wdputs( "object " );
        Putdec( i );
        Wdputs( ": name = " );
        memcpy( pe_obj_name, pe_obj->name, PE_OBJ_NAME_LEN );
        pe_obj_name[PE_OBJ_NAME_LEN] = '\0';
        Wdputs( pe_obj_name );
        if( pe_obj_name[0] == '/' ) {
            Wdputs( " (" );
            Wdputs( Coff_obj_name( pe_obj_name ) );
            Wdputs( ")" );
        }
        Wdputslc( "\n" );
        Dump_header( (char *)&pe_obj->virtual_size, pe_obj_msg, 4 );
        DumpPEObjFlags( pe_obj->flags );
        Wdputslc( "\n" );
        if( Options_dmp & (OS2_SEG_DMP|FIX_DMP) ) {
            DumpSection( pe_obj );
        }
        if( pe_obj->physical_size != 0 ) {
            if( IN_RANGE( export_rva, *pe_obj ) ) {
                Exp_off = PHYS_OFFSET( export_rva, *pe_obj );
            }
            if( IN_RANGE( import_rva, *pe_obj ) ) {
                Imp_off = PHYS_OFFSET( import_rva, *pe_obj );
            }
            if( IN_RANGE( resource_rva, *pe_obj ) ) {
                Res_off = PHYS_OFFSET( resource_rva, *pe_obj );
            }
            if( IN_RANGE( fixup_rva, *pe_obj ) ) {
                Fix_off = PHYS_OFFSET( fixup_rva, *pe_obj );
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
    unsigned_16     i;
    pe_object       pe_obj;
    unsigned_32     export_rva;
    unsigned_32     num_objects;

    Wread( &Dos_head, sizeof( Dos_head.hdr ) );
    if( ( Dos_head.hdr.signature != EXESIGN_DOS )
      || !NE_HEADER_FOLLOWS( Dos_head.hdr.reloc_offset ) ) {
        return( false );
    }
    Wlseek( NE_HEADER_OFFSET );
    Wread( &New_exe_off, sizeof( New_exe_off ) );
    Wlseek( New_exe_off );
    Wread( &Pe_head, PE_HDR_SIZE );
    switch( Pe_head.signature ) {
    case EXESIGN_PE:
    case EXESIGN_PL:
        break;
    default:
        return( false );
    }
    Wread( (char *)&Pe_head + PE_HDR_SIZE, PE_OPT_SIZE( Pe_head) );
    Exp_off = 0;
    export_rva = PE_DIRECTORY( Pe_head, PE_TBL_EXPORT ).rva;
    num_objects = Pe_head.fheader.num_objects;
    for( i = 0; i < num_objects; i++ ) {
        Wread( &pe_obj, sizeof( pe_object ) );
        if( IN_RANGE( export_rva, pe_obj ) ) {
            Exp_off = PHYS_OFFSET( export_rva, pe_obj );
        }
    }
    if( Exp_off != 0 ) {
        Dmp_exp_tab();
    } else {
        Wdputslc( "no export table\n" );
    }
    return( true );
}
