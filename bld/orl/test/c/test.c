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
#include "bool.h"
#include "trmemcvr.h"
#include "orl.h"


#define MAX_SECS    255

typedef struct _dump_options {
    bool        header;
    bool        relocs;
    bool        symbols;
    bool        sections;
    bool        sec_contents;
} dump_options;

static dump_options dump = { false, false, false, false, false };

typedef struct buff_entry   *buff_list;
struct buff_entry {
    buff_list   next;
    char        buff[1];
};

static buff_list    buffList = NULL;

static bool sectionFound = false;

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
/***********************************************************/
{
    orl_symbol_type                     type;
    const char                          *name;
    orl_sec_handle                      section;
    unsigned_64                         val64;

    name = ORLSymbolGetName( symbol );
    //printf( "handle = %x", symbol );
    printf( "%-25s:", name ? name : "" );
    ORLSymbolGetValue( symbol, &val64 );
#ifdef _M_I86
    printf( " %8.8lx%8.8lx ", val64.u._32[I64HI32], val64.u._32[I64LO32] );
#else
    printf( " %8.8x%8.8x ", val64.u._32[I64HI32], val64.u._32[I64LO32] );
#endif
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
    section = ORLSymbolGetSecHandle( symbol );
    if( section != ORL_NULL_HANDLE ) {
        printf( " (in '%s')", ORLSecGetName( section ) );
    }
    printf( "\n" );
    return( ORL_OKAY );
}

static char *relocTypes[] = {
    #define pick(enum,text) text,
    #include "orlreloc.h"
    #undef pick
};

static orl_return PrintRelocInfo( orl_reloc reloc )
/*************************************************/
{
    printf( " tag=%8.8x ", reloc->offset );
    if( reloc->type < sizeof( relocTypes ) / sizeof( *relocTypes ) ) {
        printf( "%-9s", relocTypes[reloc->type] );
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

static orl_return PrintSecInfo( orl_sec_handle section )
/******************************************************/
{
    const char                  *buf;
    unsigned                    i;
    unsigned                    i2;
    orl_sec_size                size;
    orl_sec_type                sec_type;
    orl_sec_flags               sec_flags;
    orl_sec_handle              reloc_section;
    orl_sec_handle              symbol_table;
    orl_sec_handle              string_table;
    int                         sep;
    bool                        segname_printed;

    size = 0;   // just because gcc is a little retarded
    sectionFound = true;
    segname_printed = false;
    if( dump.sections || dump.sec_contents ) {
        buf = ORLSecGetName( section );
        printf( "[%s]\n", buf );
        segname_printed = true;
        size = ORLSecGetSize( section );
    }
    if( dump.sections ) {
        //printf( "\nSection Handle:\t0x%x\n", section );
        printf( "Size=%8.8x ", size );
        sec_type = ORLSecGetType( section );
        printf( "Align=%4.4x ", ORLSecGetAlignment( section ) );
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
        sec_flags = ORLSecGetFlags( section );
        printf( "ORL_flags=0x%x\n", sec_flags );
        sep = 0;
        if( sec_flags )
            printf( " " );
        if( sec_flags & ORL_SEC_FLAG_EXEC ) {
            printf( "executable code" );
            sep++;
        }
        if( sec_flags & ORL_SEC_FLAG_INITIALIZED_DATA ) {
            if( sep++ )
                printf( ", " );
            printf( "initialized data" );
        }
        if( sec_flags & ORL_SEC_FLAG_UNINITIALIZED_DATA ) {
            if( sep++ )
                printf( ", " );
            printf( "uninitialized data" );
        }
        if( sec_flags & ORL_SEC_FLAG_GROUPED ) {
            if( sep++ )
                printf( ", " );
            printf( "grouped section" );
        }
        if( sec_flags & ORL_SEC_FLAG_NO_PADDING ) {
            if( sep++ )
                printf( ", " );
            printf( "no padding to next boundary" );
        }
        if( sec_flags & ORL_SEC_FLAG_OVERLAY ) {
            if( sep++ )
                printf( ", " );
            printf( "contains an overlay" );
        }
        if( sec_flags & ORL_SEC_FLAG_REMOVE ) {
            if( sep++ )
                printf( ", " );
            printf( "remove at link-time" );
        }
        if( sec_flags & ORL_SEC_FLAG_COMDAT ) {
            if( sep++ )
                printf( ", " );
            printf( "communal data" );
        }
        if( sec_flags & ORL_SEC_FLAG_DISCARDABLE ) {
            if( sep++ )
                printf( ", " );
            printf( "discardable" );
        }
        if( sec_flags & ORL_SEC_FLAG_NOT_CACHED ) {
            if( sep++ )
                printf( ", " );
            printf( "cannot be cached" );
        }
        if( sec_flags & ORL_SEC_FLAG_NOT_PAGEABLE) {
            if( sep++ )
                printf( ", " );
            printf( "not pageable" );
        }
        if( sec_flags & ORL_SEC_FLAG_SHARED ) {
            if( sep++ )
                printf( ", " );
            printf( "shared in memory" );
        }
        if( sec_flags & ORL_SEC_FLAG_EXECUTE_PERMISSION ) {
            if( sep++ )
                printf( ", " );
            printf( "execute permission" );
        }
        if( sec_flags & ORL_SEC_FLAG_READ_PERMISSION ) {
            if( sep++ )
                printf( ", " );
            printf( "read permission" );
        }
        if( sec_flags & ORL_SEC_FLAG_WRITE_PERMISSION) {
            if( sep++ )
                printf( ", " );
            printf( "write permission" );
        }
        if( sep ) {
            printf( "\n" );
        }
    }
    if( dump.sec_contents ) {
        if( ORLSecGetContents( section, (unsigned_8 **)&buf ) == ORL_OKAY ) {
            printf( "Contents:\n" );
            for( i = 0; i < size; i += 16 ) {
                printf( "0x%8.8x: ", i );
                for( i2 = 0; i2 < 16; i2++ ) {
                    if( i + i2 < size ) {
                        printf( "%2.2x ", buf[i + i2] );
                    } else {
                        printf( "   " );
                    }
                    if( i2 == 7 ) {
                        printf( " " );
                    }
                }
                printf( " " );
                for( i2 = 0; i2 < 16 && i + i2 < size; i2++ ) {
                    if( buf[i + i2] >= 32 && buf[i + i2] <= 122 ) {
                        printf( "%c", buf[i + i2] );
                    } else {
                        printf( "." );
                    }
                    if( i2 == 7 ) {
                        printf( " " );
                    }
                }
                printf( "\n" );
            }
        }
    }
    if( dump.relocs ) {
        reloc_section = ORLSecGetRelocTable( section );
        if( reloc_section != ORL_NULL_HANDLE ) {
            if( !segname_printed ) {
                buf = ORLSecGetName( section );
                printf( "[%s]\n", buf );
                segname_printed = true;
            }
            printf( "Relocs in [%s], ", ORLSecGetName( reloc_section ) );
            symbol_table = ORLSecGetSymbolTable( reloc_section );
            if( symbol_table != ORL_NULL_HANDLE ) {
                printf( "symtab='%s', ", ORLSecGetName( symbol_table ) );
                string_table = ORLSecGetStringTable( symbol_table );
                if( string_table != ORL_NULL_HANDLE ) {
                    printf( "strtab='%s'.\n", ORLSecGetName( string_table ) );
                } else {
                    printf( "strtab=none.\n" );
                }
            } else {
                printf( "symtab=none, strtab=none.\n" );
            }
            //printf( "Relocs:\n" );
            ORLRelocSecScan( reloc_section, PrintRelocInfo );
        }
    }
    return( ORL_OKAY );
}

static orl_return PrintSymTable( orl_sec_handle section )
/*******************************************************/
{
    if( dump.symbols ) {
        printf( "\nSymbol table\n" );
        printf( "~~~~~~~~~~~~\n" );
        ORLSymbolSecScan( section, PrintSymbolInfo );
    }
    return( ORL_OKAY );
}

static void *objRead( FILE *fp, size_t len )
/******************************************/
{
    buff_list   ptr;

    ptr = TRMemAlloc( sizeof( *buffList ) + len - 1 );
    ptr->next = buffList;
    if( fread( ptr->buff, 1, len, fp ) != len ) {
        TRMemFree( ptr );
        return( NULL );
    }
    buffList = ptr;
    return( ptr->buff );
}

static int objSeek( FILE *fp, long pos, int where )
/*************************************************/
{
    return( fseek( fp, pos, where ) );
}

static void freeBuffList( void )
/******************************/
{
    buff_list   next;

    while( buffList != NULL ) {
        next = buffList->next;
        TRMemFree( buffList );
        buffList = next;
    }
}

int main( int argc, char *argv[] )
/********************************/
{
    orl_handle                  o_handle;
    orl_file_handle             o_file_handle;
    orl_file_flags              o_file_flags;
    orl_machine_type            o_machine_type;
    orl_file_type               o_file_type;
    orl_file_format             o_file_format;
    FILE                        *fp;
    int                         c;
    int                         sep;
    char                        *secs[MAX_SECS];
    int                         num_secs = 0;
    ORLSetFuncs( orl_cli_funcs, objRead, objSeek, TRMemAlloc, TRMemFree );

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
            dump.relocs = true;
            dump.header = true;
            dump.symbols = true;
            dump.sections = true;
            break;
        case 'x':
            dump.sec_contents = true;
            break;
        case 'h':
            dump.header = true;
            break;
        case 'r':
            dump.relocs = true;
            break;
        case 's':
            dump.symbols = true;
            break;
        case 'S':
            dump.sections = true;
            break;
        case 'o':
            secs[num_secs++] = optarg;
            break;
        default:
            // error occured
            exit(1);
        }
    }
    if( optind != argc - 1 ) {
        fprintf( stderr, "must specify 1 filename\n" );
        exit(1);
    }

    fp = fopen( argv[optind], "rb" );
    if( fp == NULL ) {
        printf( "Error opening file.\n" );
        return( 2 );
    }
    TRMemOpen();
    o_handle = ORLInit( &orl_cli_funcs );
    if( o_handle == NULL ) {
        printf( "Got NULL orl_handle.\n" );
        return( 2 );
    }
    o_file_format = ORLFileIdentify( o_handle, fp );
    if( o_file_format == ORL_UNRECOGNIZED_FORMAT ) {
        printf( "The object file is not in either ELF, COFF or OMF format." );
        return( 1 );
    }
    switch( o_file_format ) {
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
    o_file_handle = ORLFileInit( o_handle, fp, o_file_format );
    if( o_file_handle == NULL ) {
        printf( "Got NULL orl_file_handle.\n" );
        return( 2 );
    }
    if( dump.header ) {
        printf( "File %s:\n", argv[optind] );
        o_machine_type = ORLFileGetMachineType( o_file_handle );
        printf( "Machine Type: " );
        if( o_machine_type >= ( sizeof( machType ) / sizeof( *machType ) ) ) {
            // We've probably added some new types?
            printf( "?(%d)", o_machine_type );
        } else {
            printf( "%s", machType[o_machine_type] );
        }
        o_file_type = ORLFileGetType( o_file_handle );
        printf( " (" );
        switch( o_file_type ) {
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
        o_file_flags = ORLFileGetFlags( o_file_handle );
        //printf(" File flags=0x%x\n", o_file_flags );
        sep = 0;
        if( o_file_flags & ORL_FILE_FLAG_LINE_NUMS_STRIPPED ) {
            printf( "line number info stripped" );
            sep++;
        }
        if( o_file_flags & ORL_FILE_FLAG_RELOCS_STRIPPED ) {
            if( sep++ )
                printf( ", " );
            printf( "relocs stripped" );
        }
        if( o_file_flags & ORL_FILE_FLAG_LOCAL_SYMS_STRIPPED ) {
            if( sep++ )
                printf( ", " );
            printf( "local symbols stripped" );
        }
        if( o_file_flags & ORL_FILE_FLAG_DEBUG_STRIPPED ) {
            if( sep++ )
                printf( ", " );
            printf( "debug info stripped" );
        }
        if( o_file_flags & ORL_FILE_FLAG_16BIT_MACHINE ) {
            if( sep++ )
                printf( ", " );
            printf( "for 16-bit machine" );
        }
        if( o_file_flags & ORL_FILE_FLAG_32BIT_MACHINE ) {
            if( sep++ )
                printf( ", " );
            printf( "for 32-bit machine" );
        }
        if( o_file_flags & ORL_FILE_FLAG_64BIT_MACHINE ) {
            if( sep++ )
                printf( ", " );
            printf( "for 64-bit machine" );
        }
        if( o_file_flags & ORL_FILE_FLAG_LITTLE_ENDIAN ) {
            if( sep++ )
                printf( ", " );
            printf( "little-endian byte order" );
        }
        if( o_file_flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
            if( sep++ )
                printf( ", " );
            printf( "big-endian byte order" );
        }
        if( o_file_flags & ORL_FILE_FLAG_SYSTEM ) {
            if( sep++ )
                printf( ", " );
            printf( "system file" );
        }
        if( sep ) {
            printf( "\n" );
        }
    }
    if( num_secs ) {
        for( c = 0; c < num_secs; c++ ) {
            sectionFound = false;
            if( ORLFileScan( o_file_handle, secs[c], PrintSecInfo ) != ORL_OKAY ) {
                printf( "Error occured in scanning section '%s'.\n", secs[c] );
            }
            if( !sectionFound ) {
                printf( "Section '%s' not found in object.\n", secs[c] );
            }
        }
    } else {
        if( ORLFileScan( o_file_handle, NULL, PrintSecInfo ) != ORL_OKAY ) {
            printf( "Error occured in scanning file.\n" );
            return( 2 );
        }
    }
    if( ORLFileScan( o_file_handle, ".symtab", PrintSymTable ) != ORL_OKAY ) {
        printf( "Error occured in scanning file for symbol table\n" );
        return( 2 );
    }
    if( ORLFileFini( o_file_handle ) != ORL_OKAY ) {
        printf( "Error calling ORLFileFini.\n" );
        return( 2 );
    }
    if( fclose( fp ) ) {
        printf( "Error closing file.\n" );
        return( 2 );
    }
    if( ORLFini( o_handle ) != ORL_OKAY ) {
        printf( "Error calling ORLFini.\n" );
    }
    freeBuffList();
#ifdef TRMEM
    TRMemPrtList();
#endif
    TRMemClose();
    return( 0 );
}
