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
* Description:  ORL test program - useful to identify object files.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "trmemcvr.h"
#include "orl.h"


#define MAX_SECS    255

typedef struct _dump_options {
    int         header;
    int         relocs;
    int         symbols;
    int         sections;
    int         sec_contents;
} dump_options;

static dump_options dump = { 0, 0, 0, 0, 0 };

typedef struct buff_entry   *buff_list;
struct buff_entry {
    buff_list   next;
    char        buff[1];
};

static buff_list    buffList = NULL;

static int sectionFound = 0;

static char *machType[] = {
    "NONE",
    "AT&T WE 32100",
    "SPARC",
    "Intel 80386",
    "Intel 80860",
    "Motorola 68000",
    "Motorola 88000",
    "DEC Alpha",
    "MIPS R3000",
    "MIPS R4000",
    "PowerPC 601",
    "Intel 8086",
    "AMD64",
    "SPARC V8+"
};

static orl_return PrintSymbolInfo( orl_symbol_handle symbol )
/****************************************************/
{
    orl_symbol_type                     type;
    char                                *name;
    orl_sec_handle                      sec;

    name = ORLSymbolGetName( symbol );
    //printf( "handle = %x", symbol );
    printf( "%-25s:", name ? name : "" );
    printf( " %8.8llx ", ORLSymbolGetValue( symbol ) );
    switch( ORLSymbolGetBinding( symbol ) ) {
        case ORL_SYM_BINDING_NONE:
            printf( "n/a " );
            break;
        case ORL_SYM_BINDING_LOCAL:
            printf( "locl" );
            break;
        case ORL_SYM_BINDING_WEAK:
            printf( "weak" );
            break;
        case ORL_SYM_BINDING_GLOBAL:
            printf( "glbl" );
            break;
        case ORL_SYM_BINDING_LAZY:
            printf( "lazy" );
            break;
        case ORL_SYM_BINDING_ALIAS:
            printf( "alis" );
            break;
    }
    printf( " " );
    type = ORLSymbolGetType( symbol );
    if( type & ORL_SYM_TYPE_DEBUG ) {
        printf( "debug" );
    } else if( type & ORL_SYM_TYPE_UNDEFINED ) {
        printf( "undef" );
    } else if( type & ORL_SYM_TYPE_COMMON ) {
        printf( "comm " );
    } else if( type & ORL_SYM_TYPE_ABSOLUTE ) {
        printf( "abs  " );
    } else {
        printf( "     " );
    }
    printf( " " );
    if( type & ORL_SYM_TYPE_OBJECT ) {
        printf( "obj " );
    } else if( type & ORL_SYM_TYPE_FUNCTION ) {
        printf( "func" );
    } else if( type & ORL_SYM_TYPE_SECTION ) {
        printf( "sect" );
    } else if( type & ORL_SYM_TYPE_FILE ) {
        printf( "file" );
    } else {
        printf( "none" );
    }
    sec = ORLSymbolGetSecHandle( symbol );
    if( sec ) {
        printf( " (in '%s')", ORLSecGetName( sec ) );
    }
    printf( "\n" );
    return( ORL_OKAY );
}

static char *relocTypes[] = {
    "NONE",             // error type
    "ABSOLUTE",         // ref to a 32-bit absolute address
    "WORD16",           // a direct ref to a 16-bit address
    "WORD32",           // a direct ref to a 32-bit address
    "WORD32NB",         // a direct ref to a 32-bit address (no base added)
    "HALFHI",           // ref to high half of 32-bit address
    "HALFHA",           // ref to high half of 32-bit address adjusted for signed low half
    "HALFLO",           // ref to low half of 32-bit address
    "PAIR",             // reloc connecting a HALF_HI and HALF_LO
    "JUMP",             // ref to the part of a 32-bit address valid for jump
    "SECTION",          // ref to an offset from a section address
    "SECREL",           // direct ref to a 32-bit address relative to the image base
    "REL16",            // relative reference to 16-bit address
    "REL21SH",          // relative ref. to a 21-bit address shifted 2
    "WORD64",           // NYI: direct ref to a 64-bit address
    "SEGMENT",          // 16-bit segment relocation
    "WORD14",           // a direct ref to a 14-bit address shifted 2
    "WORD24",           // a direct ref to a 24-bit address shifted 2
    "WORD26",           // a direct ref to a 28-bit address shifted 2
    "REL14",            // relative ref to a 14-bit address shifted 2
    "REL24",            // relative ref to a 24-bit address shifted 2
    "REL32",            // relative ref to a 32-bit address
    "REL32NOA",         // relative ref to a 32-bit address without -4 adjustment
    "TOCREL16",         // relative ref to a 16-bit offset from TOC base
    "TOCREL14",         // relative ref to a 14-bit offset from TOC base shl 2
    "TOCVREL16",        // like TOCREL16, data explicitly defined in .tocd
    "TOCVREL14",        // like TOCREL14, data explicitly defined in .tocd
    "GOT32",            // direct ref to 32-bit offset from GOT base
    "GOT16",            // direct ref to 16-bit offset from GOT base
    "GOT16HI",          // direct ref to hi 16 bits of offset from GOT base
    "GOT16HA",          // ditto adjusted for signed low 16 bits
    "GOT16LO",          // direct ref to lo 16 bits of offset from GOT base
    "PLTREL24",         // relative ref to 24-bit offset from PLT base
    "PLTREL32",         // relative ref to 32-bit offset from PLT base
    "PLT32",            // direct ref to 32-bit offset from PLT base
    "PLT16HI",          // direct ref to hi 16 bits of offset from PLT base
    "PLT16HA",          // ditto adjusted for signed low 16 bits
    "PLT16LO",          // direct ref to lo 16 bits of offset from PLT base
    "IFGLUE",           // substitute TOC restore instruction iff symbol is glue code
    "IMGLUE",           // symbol is glue code; VA is TOC restore instruction
    "OFS8LO",           // low byte of 16-bit offset
    "REL8LO",           // low byte of 16-bit relative reference
    "OFS16SEG",         // 16:16 segment:offset
    "REL16SEG",         // relative reference 16:16 segment:offset
    "OFS8HI",           // high byte of 16-bit offset
    "REL8HI",           // high byte of 16-bit relative reference
    "OFS32SEG",         // 16:32 segment:offset
    "REL32SEG",         // relative reference 16:32 segment:offset
};

static orl_return PrintRelocInfo( orl_reloc *reloc )
/*******************************************/
{
    printf( " tag=%8.8x ", reloc->offset );
    if( reloc->type < sizeof( relocTypes ) / sizeof( *relocTypes ) ) {
        printf( "%-8s", relocTypes[reloc->type] );
    } else {
        printf( "??? (%2.2x)", reloc->type );
    }
    printf( " addend=%8.8x ", reloc->addend );
    if( reloc->symbol ) {
        printf( " [%s]\n", ORLSymbolGetName( reloc->symbol ) );
    } else {
        printf( "\n" );
    }
    return( ORL_OKAY );
}

static orl_return PrintSecInfo( orl_sec_handle o_shnd )
/**********************************************/
{
    char                        *buf;
    int                         loop;
    int                         loop2;
    orl_sec_size                size;
    orl_sec_type                sec_type;
    orl_sec_flags               sec_flags;
    orl_sec_handle              reloc_section;
    orl_sec_handle              symbol_table;
    orl_sec_handle              string_table;
    int                         sep;
    int                         segname_printed = 0;

    size = 0;   // just because gcc is a little retarded
    sectionFound = 1;
    if( dump.sections || dump.sec_contents ) {
        buf = ORLSecGetName( o_shnd );
        printf( "[%s]\n", buf );
        segname_printed = 1;
        size = ORLSecGetSize( o_shnd );
    }
    if( dump.sections ) {
        //printf( "\nSection Handle:\t0x%x\n", o_shnd );
        printf( "Size=%8.8x ", size );
        sec_type = ORLSecGetType( o_shnd );
        printf( "Align=%4.4x ", ORLSecGetAlignment( o_shnd ) );
        //printf( "Section Type:\t%d\n", sec_type );
        printf( "(" );
        switch( sec_type ) {
            case ORL_SEC_TYPE_NONE:
                printf( "sec_type_none" );
                break;
            case ORL_SEC_TYPE_NO_BITS:
                printf( "no bits" );
                break;
            case ORL_SEC_TYPE_PROG_BITS:
                printf( "program bits" );
                break;
            case ORL_SEC_TYPE_SYM_TABLE:
                printf( "symbol table" );
                break;
            case ORL_SEC_TYPE_DYN_SYM_TABLE:
                printf( "dynamic symbol table" );
                break;
            case ORL_SEC_TYPE_STR_TABLE:
                printf( "string table" );
                break;
            case ORL_SEC_TYPE_RELOCS:
                printf( "relocs" );
                break;
            case ORL_SEC_TYPE_RELOCS_EXPADD:
                printf( "relocs with explicit addends" );
                break;
            case ORL_SEC_TYPE_HASH:
                printf( "hash table" );
                break;
            case ORL_SEC_TYPE_DYNAMIC:
                printf( "dynamic linking information" );
                break;
            case ORL_SEC_TYPE_NOTE:
                printf( "note or comment" );
                break;
            default:
                printf( "unknown type? %s", sec_type );
                break;
        }
        printf( ") " );
        sec_flags = ORLSecGetFlags( o_shnd );
        printf( "ORL_flags=0x%x\n", sec_flags );
        sep = 0;
        if( sec_flags ) printf( " " );
        if( sec_flags & ORL_SEC_FLAG_EXEC ) {
            printf( "executable code" );
            sep++;
        }
        if( sec_flags & ORL_SEC_FLAG_INITIALIZED_DATA ) {
            if( sep++ ) printf( ", " );
            printf( "initialized data" );
        }
        if( sec_flags & ORL_SEC_FLAG_UNINITIALIZED_DATA ) {
            if( sep++ ) printf( ", " );
            printf( "uninitialized data" );
        }
        if( sec_flags & ORL_SEC_FLAG_GROUPED ) {
            if( sep++ ) printf( ", " );
            printf( "grouped section" );
        }
        if( sec_flags & ORL_SEC_FLAG_NO_PADDING ) {
            if( sep++ ) printf( ", " );
            printf( "no padding to next boundary" );
        }
        if( sec_flags & ORL_SEC_FLAG_OVERLAY ) {
            if( sep++ ) printf( ", " );
            printf( "contains an overlay" );
        }
        if( sec_flags & ORL_SEC_FLAG_REMOVE ) {
            if( sep++ ) printf( ", " );
            printf( "remove at link-time" );
        }
        if( sec_flags & ORL_SEC_FLAG_COMDAT ) {
            if( sep++ ) printf( ", " );
            printf( "communal data" );
        }
        if( sec_flags & ORL_SEC_FLAG_DISCARDABLE ) {
            if( sep++ ) printf( ", " );
            printf( "discardable" );
        }
        if( sec_flags & ORL_SEC_FLAG_NOT_CACHED ) {
            if( sep++ ) printf( ", " );
            printf( "cannot be cached" );
        }
        if( sec_flags & ORL_SEC_FLAG_NOT_PAGEABLE) {
            if( sep++ ) printf( ", " );
            printf( "not pageable" );
        }
        if( sec_flags & ORL_SEC_FLAG_SHARED ) {
            if( sep++ ) printf( ", " );
            printf( "shared in memory" );
        }
        if( sec_flags & ORL_SEC_FLAG_EXECUTE_PERMISSION ) {
            if( sep++ ) printf( ", " );
            printf( "execute permission" );
        }
        if( sec_flags & ORL_SEC_FLAG_READ_PERMISSION ) {
            if( sep++ ) printf( ", " );
            printf( "read permission" );
        }
        if( sec_flags & ORL_SEC_FLAG_WRITE_PERMISSION) {
            if( sep++ ) printf( ", " );
            printf( "write permission" );
        }
        if( sep ) printf( "\n" );
    }
    if( dump.sec_contents ) {
        if( ORLSecGetContents( o_shnd, (unsigned_8 **)&buf ) == ORL_OKAY ) {
            printf( "Contents:\n" );
            for( loop = 0; loop < size; loop += 16 ) {
                printf( "0x%8.8x: ", loop );
                for( loop2 = 0; loop2 < 16; loop2++ ) {
                    if( loop + loop2 < size ) {
                        printf( "%2.2x ", buf[loop+loop2] );
                    } else {
                        printf( "   " );
                    }
                    if( loop2 == 7 ) {
                        printf( " " );
                    }
                }
                printf( " " );
                for( loop2 = 0; loop2 < 16 && loop + loop2 < size; loop2++ ) {
                    if( buf[loop+loop2] >= 32 && buf[loop+loop2] <= 122 ) {
                        printf( "%c", buf[loop+loop2] );
                    } else {
                        printf( "." );
                    }
                    if( loop2 == 7 ) {
                        printf( " " );
                    }
                }
                printf( "\n" );
            }
        }
    }
    if( dump.relocs ) {
        reloc_section = ORLSecGetRelocTable( o_shnd );
        if( reloc_section ) {
            if( !segname_printed++ ) {
                buf = ORLSecGetName( o_shnd );
                printf( "[%s]\n", buf );
            }
            printf( "Relocs in [%s], ", ORLSecGetName( reloc_section ) );
            symbol_table = ORLSecGetSymbolTable( reloc_section );
            if( symbol_table ) {
                printf( "symtab='%s', ", ORLSecGetName( symbol_table ) );
                string_table = ORLSecGetStringTable( symbol_table );
                if( string_table ) {
                    printf( "strtab='%s'.\n", ORLSecGetName( string_table ) );
                } else {
                    printf( "strtab=none.\n" );
                }
            } else {
                printf( "symtab=none, strtab=none.\n" );
            }
            //printf( "Relocs:\n" );
            ORLRelocSecScan( reloc_section, &PrintRelocInfo );
        }
    }
    return( ORL_OKAY );
}

static orl_return PrintSymTable( orl_sec_handle orl_sec_hnd )
/****************************************************/
{
    if( dump.symbols ) {
        printf( "\nSymbol table\n" );
        printf( "~~~~~~~~~~~~\n" );
        ORLSymbolSecScan( orl_sec_hnd, &PrintSymbolInfo );
    }
    return( ORL_OKAY );
}

static void * objRead( void *hdl, size_t len )
/********************************************/
{
    buff_list   ptr;

    ptr = TRMemAlloc( sizeof( *buffList ) + len - 1 );
    ptr->next = buffList;
    if( read( (int)hdl, ptr->buff, len ) != len ) {
        TRMemFree( ptr );
        return( NULL );
    }
    buffList = ptr;
    return( ptr->buff );
}

static long objSeek( void *hdl, long pos, int where )
/***************************************************/
{
    return( lseek( (int)hdl, pos, where ) );
}

static void freeBuffList( void )
/***********************/
{
    buff_list   next;

    while( buffList ) {
        next = buffList->next;
        TRMemFree( buffList );
        buffList = next;
    }
}

int main( int argc, char *argv[] )
/********************************/
{
    orl_handle                  o_hnd;
    orl_file_handle             o_fhnd;
    int                         file;
    orl_file_flags              file_flags;
    orl_machine_type            machine_type;
    orl_file_type               file_type;
    orl_file_format             type;
    int                         c;
    int                         sep;
    char                        *secs[MAX_SECS];
    int                         num_secs = 0;
    OrlSetFuncs( orl_cli_funcs, objRead, objSeek, TRMemAlloc, TRMemFree );

    if( argc < 2 ) {
        printf( "Usage:  objread [-ahrsSx] [-o<section>] <objfile>\n" );
        printf( "Where <objfile> is a COFF, ELF or OMF object file\n" );
        printf( "objread reads and dumps an object using ORL\n" );
        printf( "Options: -a     dumps all information (except hex dump)\n" );
        printf( "         -h     dumps file header information\n" );
        printf( "         -r     dumps relocation information\n" );
        printf( "         -s     dumps symbol table\n" );
        printf( "         -S     dumps section information\n" );
        printf( "         -x     get hex dump of section content\n" );
        printf( "         -o     only scan <section> for info\n" );
        return( 1 );
    }
    while( (c = getopt( argc, argv, "axhrsSo:" )) != EOF ) {
        switch( c ) {
            case 'a':
                dump.relocs++;
                dump.header++;
                dump.symbols++;
                dump.sections++;
                break;
            case 'x':
                dump.sec_contents++;
                break;
            case 'h':
                dump.header++;
                break;
            case 'r':
                dump.relocs++;
                break;
            case 's':
                dump.symbols++;
                break;
            case 'S':
                dump.sections++;
                break;
            case 'o':
                secs[num_secs++] = optarg;
                break;
            default:
                // error occured
                exit(1);
        };
    };
    if( optind != argc - 1 ) {
        fprintf( stderr, "must specify 1 filename\n" );
        exit(1);
    }

    file = open( argv[optind], O_BINARY | O_RDONLY );
    if( file == -1 ) {
        printf( "Error opening file.\n" );
        return( 2 );
    }
    TRMemOpen();
    o_hnd = ORLInit( &orl_cli_funcs );
    if( o_hnd == NULL ) {
        printf( "Got NULL orl_handle.\n" );
        return( 2 );
    }
    type = ORLFileIdentify( o_hnd, (void *)file );
    if( type == ORL_UNRECOGNIZED_FORMAT ) {
        printf( "The object file is not in either ELF, COFF or OMF format." );
        return( 1 );
    }
    switch( type ) {
    case ORL_ELF:
        printf( "ELF" );
        break;
    case ORL_COFF:
        printf( "COFF" );
        break;
    case ORL_OMF:
        printf( "OMF" );
        break;
    default:
        printf( "Unknown" );
        break;
    }
    printf( " object file.\n" );
    o_fhnd = ORLFileInit( o_hnd, (void *)file, type );
    if( o_fhnd == NULL ) {
        printf( "Got NULL orl_file_handle.\n" );
        return( 2 );
    }
    if( dump.header ) {
        printf( "File %s:\n", argv[optind] );
        machine_type = ORLFileGetMachineType( o_fhnd );
        printf( "Machine Type: " );
        if( machine_type >= ( sizeof( machType ) / sizeof( *machType ) ) ) {
            // We've probably added some new types?
            printf( "?(%d)", machine_type );
        } else {
            printf( "%s", machType[ machine_type ] );
        }
        file_type = ORLFileGetType( o_fhnd );
        printf( " (" );
        switch( file_type ) {
            case ORL_FILE_TYPE_NONE:
                printf( "file_type_none" );
                break;
            case ORL_FILE_TYPE_OBJECT:
                printf( "object file" );
                break;
            case ORL_FILE_TYPE_EXECUTABLE:
                printf( "executable" );
                break;
            case ORL_FILE_TYPE_SHARED_OBJECT:
                printf( "shared object" );
                break;
            case ORL_FILE_TYPE_DLL:
                printf( "DLL" );
                break;
            default:
                printf( "unknown file type?" );
                break;
        }
        printf( ")\n" );
        file_flags = ORLFileGetFlags( o_fhnd );
        //printf(" File flags=0x%x\n", file_flags );
        sep = 0;
        if( file_flags & ORL_FILE_FLAG_LINE_NUMS_STRIPPED ) {
            printf( "line number info stripped" );
            sep = 1;
        }
        if( file_flags & ORL_FILE_FLAG_RELOCS_STRIPPED ) {
            if( sep++ ) printf( ", " );
            printf( "relocs stripped" );
        }
        if( file_flags & ORL_FILE_FLAG_LOCAL_SYMS_STRIPPED ) {
            if( sep++ ) printf( ", " );
            printf( "local symbols stripped" );
        }
        if( file_flags & ORL_FILE_FLAG_DEBUG_STRIPPED ) {
            if( sep++ ) printf( ", " );
            printf( "debug info stripped" );
        }
        if( file_flags & ORL_FILE_FLAG_16BIT_MACHINE ) {
            if( sep++ ) printf( ", " );
            printf( "for 16-bit machine" );
        }
        if( file_flags & ORL_FILE_FLAG_32BIT_MACHINE ) {
            if( sep++ ) printf( ", " );
            printf( "for 32-bit machine" );
        }
        if( file_flags & ORL_FILE_FLAG_64BIT_MACHINE ) {
            if( sep++ ) printf( ", " );
            printf( "for 64-bit machine" );
        }
        if( file_flags & ORL_FILE_FLAG_LITTLE_ENDIAN ) {
            if( sep++ ) printf( ", " );
            printf( "little-endian byte order" );
        }
        if( file_flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
            if( sep++ ) printf( ", " );
            printf( "big-endian byte order" );
        }
        if( file_flags & ORL_FILE_FLAG_SYSTEM ) {
            if( sep++ ) printf( ", " );
            printf( "system file" );
        }
        if( sep ) printf( "\n" );
    }
    if( num_secs ) {
        for( c = 0; c < num_secs; c++ ) {
            sectionFound = 0;
            if( ORLFileScan( o_fhnd, secs[c], &PrintSecInfo ) != ORL_OKAY ) {
                printf( "Error occured in scanning section '%s'.\n", secs[c] );
            }
            if( !sectionFound ) {
                printf( "Section '%s' not found in object.\n", secs[c] );
            }
        }
    } else {
        if( ORLFileScan( o_fhnd, NULL, &PrintSecInfo ) != ORL_OKAY ) {
            printf( "Error occured in scanning file.\n" );
            return( 2 );
        }
    }
    if( ORLFileScan( o_fhnd, ".symtab", &PrintSymTable ) != ORL_OKAY ) {
        printf( "Error occured in scanning file for symbol table\n" );
        return( 2 );
    }
    if( ORLFileFini( o_fhnd ) != ORL_OKAY ) {
        printf( "Error calling ORLFileFini.\n" );
        return( 2 );
    }
    if( close( file ) == -1 ) {
        printf( "Error closing file.\n" );
        return( 2 );
    }
    if( ORLFini( o_hnd ) != ORL_OKAY ) {
        printf( "Error calling ORLFini.\n" );
    }
    freeBuffList();
#ifdef TRMEM
    TRMemPrtList();
#endif
    TRMemClose();
    return( 0 );
}
