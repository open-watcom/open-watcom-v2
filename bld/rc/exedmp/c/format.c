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
* Description:  PE Dump Utility formatting routines.
*
****************************************************************************/


#include "banner.h"
#include "format.h"
#include "param.h"
#include "read.h"
#include "main.h"
#include "watcom.h"
#include "exepe.h"
#include "formatsd.h"


enum {
    ED_PE_RES_COUNT = 0
    #define ED_PE_RES(e,t)  + 1
    ED_PE_RESS
    #undef ED_PE_RES
};

enum {
    ED_PE_CPU_COUNT = 0
    #define ED_PE_CPU(e,t)  + 1
    ED_PE_CPUS
    #undef ED_PE_CPU
};

enum {
    ED_PE_FLG_COUNT = 0
    #define ED_PE_FLG(e,t)  + 1
    ED_PE_FLGS
    #undef ED_PE_FLG
};

enum {
    ED_PE_SS_COUNT = 0
    #define ED_PE_SS(e,t)   + 1
    ED_PE_SSS
    #undef ED_PE_SS
};

enum {
    ED_PE_DLL_COUNT = 0
    #define ED_PE_DLL(e,t)  + 1
    ED_PE_DLLS
    #undef ED_PE_DLL
};

enum {
    ED_PE_OBJ_COUNT = 0
    #define ED_PE_OBJ(e,t)  + 1
    ED_PE_OBJS
    #undef ED_PE_OBJ
};

const char *resTypes[] = {
    #define ED_PE_RES(e,t)  t,
    ED_PE_RESS
    #undef ED_PE_RES
    NULL
};

static const unsigned_32 cpu_masks_table[] = {
    #define ED_PE_CPU(e,t)  e,
    ED_PE_CPUS
    #undef ED_PE_CPU
};

static const unsigned_32 hdr_masks_table[] = {
    #define ED_PE_FLG(e,t)  e,
    ED_PE_FLGS
    #undef ED_PE_FLG
};

static const unsigned_32 ss_masks_table[] = {
    #define ED_PE_SS(e,t)   e,
    ED_PE_SSS
    #undef ED_PE_SS
};

static const unsigned_32 dll_masks_table[] = {
    #define ED_PE_DLL(e,t)  e,
    ED_PE_DLLS
    #undef ED_PE_DLL
};

static const unsigned_32 obj_masks_table[] = {
    #define ED_PE_OBJ(e,t)  e,
    ED_PE_OBJS
    #undef ED_PE_OBJ
};

static const char *cpu_flags_labels[] = {
    #define ED_PE_CPU(e,t)  t,
    ED_PE_CPUS
    #undef ED_PE_CPU
    NULL
};

static const char *hdr_flags_labels[] = {
    #define ED_PE_FLG(e,t)  t,
    ED_PE_FLGS
    #undef ED_PE_FLG
    NULL
};

static const char *ss_flags_labels[] = {
    #define ED_PE_SS(e,t)   t,
    ED_PE_SSS
    #undef ED_PE_SS
    NULL
};

static const char *dll_flags_labels[] = {
    #define ED_PE_DLL(e,t)  t,
    ED_PE_DLLS
    #undef ED_PE_DLL
    NULL
};

static const char *obj_flags_labels[] = {
    #define ED_PE_OBJ(e,t)  t,
    ED_PE_OBJS
    #undef ED_PE_OBJ
    NULL
};

#define printYes( x )   printf( "%s= %s\n", x, LBL_YES )
#define printNo( x )    printf( "%s= %s\n", x, LBL_NO )

int indentLevel = 0;

/* forward declarations */
void printHexDump( unsigned long addr, unsigned long length, ExeFile *exeFile,
                   Parameters *param );
void printHexBytes( unsigned long addr, unsigned long length, ExeFile *exeFile );


void indentMore( int level )
/***************************/
{
    indentLevel += level;
}

void indentLess( int level )
/***************************/
{
    indentLevel -= level;
    if( indentLevel < 0 ) indentLevel = 0;
}

void printIndent( bool newline, Parameters *param )
/**************************************************/
{
    int i;
    if( newline ) printf( "\n" );
    for( i = 0; i < indentLevel; i++ ) {
        printf( "%*s", param->indentSpaces, "" );
    }
}

void printRuler( bool indent, bool newline, Parameters *param )
/**************************************************************/
{
    int i;
    if( newline ) printf( "\n" );
    if( param->printRuler ) {
        if( indent ) {
            printIndent( false, param );
            i = SCREENWIDTH - indentLevel*param->indentSpaces;
        } else {
            i = SCREENWIDTH;
        }
        for( ; i > 0; i-- )
            printf( "%s", RULERSTRING );
        printf( "\n" );
    }
}

void printParameters( Parameters *param )
/****************************************/
{
    if( param->dumpOffsets )            printYes( LBL_DUMPOFFSETS );
    else                                printNo ( LBL_DUMPOFFSETS );
    if( param->dumpHexHeaders )         printYes( LBL_DUMPHEXHEADERS );
    else                                printNo ( LBL_DUMPHEXHEADERS );
    if( param->dumpHexData )            printYes( LBL_DUMPHEXDATA );
    else                                printNo ( LBL_DUMPHEXDATA  );
    if( param->dumpInterpretation )     printYes( LBL_DUMPINTERPRETATION );
    else                                printNo ( LBL_DUMPINTERPRETATION );
    if( param->dumpExeHeaderInfo )      printYes( LBL_DUMPEXEHEADERINFO );
    else                                printNo ( LBL_DUMPEXEHEADERINFO );
    if( param->dumpResObjectInfo )      printYes( LBL_DUMPRESOBJECTINFO );
    else                                printNo ( LBL_DUMPRESOBJECTINFO );
    if( param->printRuler )             printYes( LBL_PRINTRULER );
    else                                printNo ( LBL_PRINTRULER );
    if( param->specificType ) {
        printf( "%s= %s\n", LBL_SPECIFICTYPE,
                            resTypes[ param->specificTypeID ] );
    } else {
        printNo( LBL_SPECIFICTYPE );
    }
    printf( "%s= %i\n", LBL_INDENTSPACES, param->indentSpaces );
    printf( "%s= %i\n", LBL_HEXINDENTSPACES, param->hexIndentSpaces );
}

void printDefaultParameters( void )
/**********************************/
{
    Parameters temp;

    printf( LBL_DEFAULTPARAMETERS );
    defaultParam( &temp );
    printParameters( &temp );
}

void printBanner( void )
/***********************/
{
    puts(
        banner1t( "PE ExeDmp Utility" ) "\n"
        banner1v( "1.0" ) "\n"
        banner2 "\n"
        banner2a( 1997 ) "\n"
        banner3 "\n"
        banner3a "\n"
    );
}

void printHelp( void )
/*********************/
{
    int i;

    puts(
        " Usage:\n"
        "       exedmp [options] <filename> [options]\n"
        "\n"
        " Toggle Options:\n"
        " -o    dump offset of each dir header, dir entry, and data entry\n"
        " -h    dump hex header of each dir header, dir entry, and data entry\n"
        " -d    dump hex contents of each data entry\n"
        " -i    print interpretation of each dir header, dir entry, and data entry\n"
        " -x    dump Exe headers\n"
        " -r    dump information about resource object\n"
        " -l    print ruler\n"
        "\n"
        " Other Options:\n"
        " -tX   dump only resource of type X (e.g. -tgroupicon)\n"
        " -sX   indent each level of dir by X spaces\n"
        " -nX   indent hex contents of data entries by X spaces\n"
        "       (use -1 to align hex contents with their data entries)\n"
        "\n"
        " Available Resource Types:\n"
    );
    for( i = 0; resTypes[i] != NULL; i++ ) {
        if( *resTypes[i] != '\0' ) {
            printf( "       %s\n", resTypes[i] );
        }
    }
    printf( "\n" );
    printDefaultParameters();
}

void printDosHeader( ExeFile *exeFile, Parameters *param )
/*********************************************************/
{
    if( param->dumpExeHeaderInfo ) {
        printf( MSG_DOS_HEADER );
        printRuler( false, false, param );
        printf( MSG_DOS_SIGNATURE,      exeFile->dosHdr.signature );
        printf( MSG_DOS_MODSIZE,        exeFile->dosHdr.mod_size );
        printf( MSG_DOS_FILESIZE,       exeFile->dosHdr.file_size );
        printf( MSG_DOS_NUMRELOCS,      exeFile->dosHdr.num_relocs );
        printf( MSG_DOS_HDRSIZE,        exeFile->dosHdr.hdr_size );
        printf( MSG_DOS_MIN16,          exeFile->dosHdr.min_16 );
        printf( MSG_DOS_MAX16,          exeFile->dosHdr.max_16 );
        printf( MSG_DOS_SSOFFSET,       exeFile->dosHdr.SS_offset );
        printf( MSG_DOS_SP,             exeFile->dosHdr.SP );
        printf( MSG_DOS_CHKSUM,         exeFile->dosHdr.chk_sum );
        printf( MSG_DOS_IP,             exeFile->dosHdr.IP );
        printf( MSG_DOS_CSOFFSET,       exeFile->dosHdr.CS_offset );
        printf( MSG_DOS_RELOCOFFSET,    exeFile->dosHdr.reloc_offset );
        printf( MSG_DOS_OVERLAYNUM,     exeFile->dosHdr.overlay_num );
        printf( "\n\n" );
    }
}

static void printFlags( unsigned_32 value,
                 const unsigned_32 masks[], int count,
                 const char *labels[],
                 const char *indentString )
/****************************************************/
{
    int i;
    size_t indentLen;
    size_t labelLen;
    size_t cursor;
    bool first;

    first = true;
    indentLen = strlen( indentString );
    cursor = indentLen;
    printf( indentString );
    for( i = 0; i < count; i++ ) {
        if( value & masks[i] ) {
            if( first ) {
                first = false;
            } else {
                printf( "|" );
            }
            labelLen = strlen( labels[i] );
            if( labelLen + cursor > SCREENWIDTH ) {
                printf( "\n%s", indentString );
                cursor = indentLen;
            }
            printf( labels[i] );
            cursor += labelLen + 1;
        }
    }
    printf( "\n" );
}

void printPeHeader( ExeFile *exeFile, Parameters *param )
/********************************************************/
{
    int i;
    int count;

    if( param->dumpExeHeaderInfo ) {
        printf( MSG_PE_HEADER );
        printRuler( false, false, param );
        printf( MSG_PE_SIGNATURE ,              exeFile->pexHdr.signature );
        printf( MSG_PE_CPUTYPE ,                exeFile->pexHdr.fheader.cpu_type );
        count = ED_PE_CPU_COUNT;
        for( i = 0; i < count; i++ ) {
            if( exeFile->pexHdr.fheader.cpu_type == cpu_masks_table[i] ) {
                printf( MSG_PE_CPUTYPETEXT, cpu_flags_labels[i] );
                break;
            }
        }
        if( i >= count ) {
            printf( MSG_PE_CPUTYPENOTRECOGNIZED );
        }
        printf( MSG_PE_NUMOBJECTS ,             exeFile->pexHdr.fheader.num_objects );
        printf( MSG_PE_TIMESTAMP ,              exeFile->pexHdr.fheader.time_stamp );
        printf( MSG_PE_SYMTABLE ,               exeFile->pexHdr.fheader.sym_table );
        printf( MSG_PE_NUMSYMS ,                exeFile->pexHdr.fheader.num_symbols );
        printf( MSG_PE_NTHDRSIZE ,              exeFile->pexHdr.fheader.opt_hdr_size );
        printf( MSG_PE_FLAGS ,                  exeFile->pexHdr.fheader.flags );
        printFlags( exeFile->pexHdr.fheader.flags,
                    hdr_masks_table, ED_PE_FLG_COUNT, hdr_flags_labels,
                    MSG_PE_FLAGSINDENT );
        printf( MSG_PE_MAGIC ,                  PE( exeFile->pexHdr, magic ) );
        printf( MSG_PE_LNKMAJOR ,               PE( exeFile->pexHdr, lnk_major ) );
        printf( MSG_PE_LNKMINOR ,               PE( exeFile->pexHdr, lnk_minor ) );
        printf( MSG_PE_CODESIZE ,               PE( exeFile->pexHdr, code_size ),
                                                PE( exeFile->pexHdr, code_size ) );
        printf( MSG_PE_INITDATASIZE ,           PE( exeFile->pexHdr, init_data_size ),
                                                PE( exeFile->pexHdr, init_data_size ) );
        printf( MSG_PE_UNINITDATASIZE ,         PE( exeFile->pexHdr, uninit_data_size ),
                                                PE( exeFile->pexHdr, uninit_data_size ) );
        printf( MSG_PE_ENTRYRVA ,               PE( exeFile->pexHdr, entry_rva ) );
        printf( MSG_PE_CODEBASE ,               PE( exeFile->pexHdr, code_base ) );
        if( IS_PE64( exeFile->pexHdr ) ) {
            printf( MSG_PE64_IMAGEBASE ,        PE64( exeFile->pexHdr ).image_base );
        } else {
            printf( MSG_PE32_DATABASE ,         PE32( exeFile->pexHdr ).data_base );
            printf( MSG_PE32_IMAGEBASE ,        PE32( exeFile->pexHdr ).image_base );
        }
        printf( MSG_PE_OBJECTALIGN ,            PE( exeFile->pexHdr, object_align ) );
        printf( MSG_PE_FILEALIGN ,              PE( exeFile->pexHdr, file_align ) );
        printf( MSG_PE_OSMAJOR ,                PE( exeFile->pexHdr, os_major ) );
        printf( MSG_PE_OSMINOR ,                PE( exeFile->pexHdr, os_minor ) );
        printf( MSG_PE_USERMAJOR ,              PE( exeFile->pexHdr, user_major ) );
        printf( MSG_PE_USERMINOR ,              PE( exeFile->pexHdr, user_minor ) );
        printf( MSG_PE_SUBSYSMAJOR ,            PE( exeFile->pexHdr, subsys_major ) );
        printf( MSG_PE_SUBSYSMINOR ,            PE( exeFile->pexHdr, subsys_minor ) );
        printf( MSG_PE_RSVD1 ,                  PE( exeFile->pexHdr, rsvd1 ) );
        printf( MSG_PE_IMAGESIZE ,              PE( exeFile->pexHdr, image_size ) );
        printf( MSG_PE_HEADERSIZE ,             PE( exeFile->pexHdr, headers_size ) );
        printf( MSG_PE_FILECHECKSUM ,           PE( exeFile->pexHdr, file_checksum ) );
        printf( MSG_PE_SUBSYSTEM ,              PE( exeFile->pexHdr, subsystem ) );
        count = ED_PE_SS_COUNT;
        for( i = 0; i < count; i++ ) {
            if( PE( exeFile->pexHdr, subsystem ) == ss_masks_table[i] ) {
                printf( MSG_PE_SUBSYSTEMTEXT, ss_flags_labels[i] );
                break;
            }
        }
        if( i >= count ) {
            printf( MSG_PE_SUBSYSTEMNOTRECOGNIZED );
        }
        printf( MSG_PE_DLLFLAGS ,               PE( exeFile->pexHdr, dll_flags ) );
        printFlags( PE( exeFile->pexHdr, dll_flags ),
                    dll_masks_table, ED_PE_DLL_COUNT, dll_flags_labels,
                    MSG_PE_DLLFLAGSINDENT );
        if( IS_PE64( exeFile->pexHdr ) ) {
            printf( MSG_PE64_STACKRESERVESIZE , PE64( exeFile->pexHdr ).stack_reserve_size );
            printf( MSG_PE64_STACKCOMMITSIZE ,  PE64( exeFile->pexHdr ).stack_commit_size );
            printf( MSG_PE64_HEAPRESERVESIZE ,  PE64( exeFile->pexHdr ).heap_reserve_size );
            printf( MSG_PE64_HEAPCOMMITSIZE ,   PE64( exeFile->pexHdr ).heap_commit_size );
        } else {
            printf( MSG_PE32_STACKRESERVESIZE , PE32( exeFile->pexHdr ).stack_reserve_size );
            printf( MSG_PE32_STACKCOMMITSIZE ,  PE32( exeFile->pexHdr ).stack_commit_size );
            printf( MSG_PE32_HEAPRESERVESIZE ,  PE32( exeFile->pexHdr ).heap_reserve_size );
            printf( MSG_PE32_HEAPCOMMITSIZE ,   PE32( exeFile->pexHdr ).heap_commit_size );
        }
        printf( MSG_PE_TLSIDXADDR ,             PE( exeFile->pexHdr, tls_idx_addr ) );
        printf( MSG_PE_NUMTABLES ,              PE( exeFile->pexHdr, num_tables ) );
        printf( "\n\n" );
    }
}

void printResObject( ExeFile *exeFile, Parameters *param )
/*********************************************************/
{
    if( param->dumpResObjectInfo ) {
        printf( MSG_RESOBJ_HEADING );
        printRuler( true, false, param );
        printf( MSG_RESOBJ_VIRTUALSIZE,         exeFile->resObj.virtual_size );
        printf( MSG_RESOBJ_RVA,                 exeFile->resObj.rva );
        printf( MSG_RESOBJ_PHYSICALSIZE,        exeFile->resObj.physical_size );
        printf( MSG_RESOBJ_PHYSICALOFFSET,      exeFile->resObj.physical_offset );
        printf( MSG_RESOBJ_RELOCSRVA,           exeFile->resObj.relocs_rva );
        printf( MSG_RESOBJ_LINNUMRVA,           exeFile->resObj.linnum_rva );
        printf( MSG_RESOBJ_NUMRELOCS,           exeFile->resObj.num_relocs );
        printf( MSG_RESOBJ_NUMLINNUMS,          exeFile->resObj.num_linnums );
        printf( MSG_RESOBJ_FLAGS,               exeFile->resObj.flags );
        printFlags( exeFile->resObj.flags,
                    obj_masks_table, ED_PE_OBJ_COUNT, obj_flags_labels,
                    MSG_RESOBJ_FLAGSINDENT );
        printf( "\n" );
        printHexDump( exeFile->resObjAddr, sizeof( pe_object ),
                      exeFile, param );
        printf( "\n" );
        printRuler( true, false, param );
        printf( "\n\n" );
    }
}

void printTableContents( ResTableEntry *table, ExeFile *exeFile,
                         Parameters *param, long addr, int depth )
/****************************************************************/
{
    int         i;
    int         entriesCount;

    printIndent( false, param );
    printf( LBL_DIRHEADER );
    printf( ":   " );
    if( param->dumpOffsets ) {
        printf( "0x%-8.8X  ", addr );
    }
    if( param->dumpHexHeaders ) {
        printHexBytes( addr, sizeof( resource_dir_header ), exeFile );
    }
    printRuler( true, true, param );
    if( param->dumpInterpretation ) {
        printIndent( false, param );
        printf( MSG_TABLE_TIMESTAMP,            table->header.time_stamp );
        printIndent( false, param );
        printf( MSG_TABLE_MAJOR,                table->header.major );
        printIndent( false, param );
        printf( MSG_TABLE_MINOR,                table->header.minor );
        printIndent( false, param );
        printf( MSG_TABLE_NUMNAMEENTRIES,       table->header.num_name_entries,
                                                table->header.num_name_entries );
        printIndent( false, param );
        printf( MSG_TABLE_NUMIDENTRIES,         table->header.num_id_entries,
                                                table->header.num_id_entries );
        printIndent( false, param );
        printf( MSG_TABLE_FLAGS,                table->header.flags );
        /* interpretation of flags: flags field currently always
           set to 0. */
    }
    printf( "\n" );

    entriesCount = table->header.num_name_entries + table->header.num_id_entries;
    indentMore( 1 );
    for( i = 0; i < entriesCount; i++ ) {
        printDirContents( &table->dirs[ i ], exeFile, param,
                          addr + sizeof( resource_dir_header )
                               + sizeof( resource_dir_entry ) * i,
                          depth + 1 );
    }
    indentLess( 1 );
    printf( "\n" );
}

void printDirContents( ResDirEntry *dir, ExeFile *exeFile,
                       Parameters  *param, long addr, int depth )
/***************************************************************/
{
    int i;

    if( depth == 1 && param->specificType &&
        param->specificTypeID != dir->dir.id_name ) {
        return;
    }

    printIndent( false, param );
    printf( LBL_DIRENTRY );
    printf( ":     " );
    if( param->dumpOffsets ) {
        printf( "0x%-8.8X   ", addr );
    }
    if( param->dumpHexHeaders ) {
        printHexBytes( addr, sizeof( resource_dir_entry ), exeFile );
    }
    printRuler( true, true, param );
    if( param->dumpInterpretation ) {
        printIndent( false, param );
        if( dir->nameID == NAME ) {
            printf( MSG_DIR_NAME );
            for( i = 0; i < dir->nameSize; i++ ) {
                printf( "%1s", (char *)&dir->name[ i ] );
            }
            printIndent( true, param );
            printf( MSG_DIR_NAMEADDRESS, getDirNameAbs( dir, exeFile ) );
            printIndent( false, param );
            printf( MSG_DIR_NAMESIZE, dir->nameSize );
        } else {
            printf( MSG_DIR_ID, dir->dir.id_name );
            if( depth == 1 && dir->dir.id_name < ED_PE_RES_COUNT ) {
                printIndent( false, param );
                printf( MSG_DIR_IDTYPE, resTypes[ dir->dir.id_name ] );
            }
        }
        printIndent( false, param );
        printf( MSG_DIR_ENTRYRVA, dir->dir.entry_rva & PE_RESOURCE_MASK );
    }
    printf( "\n" );

    if( dir->entryType == TABLE ) {
        printTableContents( dir->table, exeFile, param, getDirChildAbs( dir, exeFile ), depth );
    } else {
        printDataContents( dir->data, exeFile, param, getDirChildAbs( dir, exeFile ) );
    }
    printf( "\n" );
}

void printDataContents( ResDataEntry *data, ExeFile *exeFile,
                        Parameters   *param, long addr )
/***********************************************************/
{
    printIndent( false, param );
    printf( LBL_DATAENTRY );
    printf( ":    " );
    if( param->dumpOffsets ) {
        printf( "0x%-8.8X   ", addr );
    }
    if( param->dumpHexHeaders ) {
        printHexBytes( addr, sizeof( resource_entry ), exeFile );
    }
    printRuler( true, true, param );

    if( param->dumpInterpretation ) {
        printIndent( false, param );
        printf( MSG_DATA_RVA, getResDataRva( data, exeFile ) );
        printIndent( false, param );
        printf( MSG_DATA_SIZE, data->entry.size, data->entry.size );
        printIndent( false, param );
        printf( MSG_DATA_CODEPAGE, data->entry.code_page );
        printIndent( false, param );
        printf( MSG_DATA_RSVD, data->entry.rsvd );
    }
    printf( "\n" );

    if( param->dumpHexData ) {
        printHexDump( getResDataAbs( data, exeFile ),
                      (size_t) data->entry.size,
                      exeFile, param );
    }
}

void printHexBytes( unsigned long addr, unsigned long length, ExeFile *exeFile )
/********************************************************************/
{
    long            prevPos;
    unsigned_8      *buffer;
    unsigned long   count;
    unsigned long   i;

    count = (length / sizeof( unsigned_8 ) );
    buffer = (unsigned_8 *)malloc( count );

    if( buffer != NULL ) {
        prevPos = ftell( exeFile->file );
        if( fseek( exeFile->file, addr, SEEK_SET ) == 0 ) {
            if( fread( buffer, sizeof( unsigned_8 ), count, exeFile->file ) == count ) {
                for( i = 0; i < count; i++ ) {
                    printf( "%2.2X ", buffer[ i ] );
                }
                free( buffer );
                fseek( exeFile->file, prevPos, SEEK_SET );
                return;
            }
        }
        free( (void *)buffer );
        fseek( exeFile->file, prevPos, SEEK_SET );
    }
    printf( ERR_FORMAT_CANNOT_DUMP_HEX );
}

static bool isPrintable( unsigned_8 buffer )
/******************************************/
{
    if( buffer == '\0' ||
        buffer == '\n' ||
        buffer == '\r' ||
        buffer == '\f' ||
        buffer == '\t' ||
        buffer == '\v' ||
        buffer == '\b' ||
        buffer == '\a' ) {
        return( false );
    } else {
        return( true );
    }
}

static void printHexLine( unsigned long lower, unsigned long upper, ExeFile *exeFile,
                   const char *mask, const char *emptyMask,
                   const char *unprintableMask, bool testPrintable,
                   bool splitAtEight )
/********************************************************************/
{
    long            prevPos;
    unsigned_8      buffer;
    unsigned long   i;

    prevPos = ftell( exeFile->file );
    if( fseek( exeFile->file, ( lower / 16L ) * 16L, SEEK_SET ) ) {
        printf( ERR_FORMAT_CANNOT_DUMP_HEX );
        printf( "\n" );
        return;
    }

    for( i = 0; i < 16; i++ ) {
        if( fread( &buffer, sizeof( unsigned_8 ),
                   1, exeFile->file ) != 1 ) {
            printf( ERR_FORMAT_CANNOT_DUMP_HEX );
            printf( "\n" );
            return;
        }
        if( i == 8 && splitAtEight ) {
            printf( " " );
        }
        if( i + lower < upper ) {
            if( i < lower % 16L ) {
                printf( emptyMask );
            } else {
                if( !testPrintable || isPrintable( buffer ) ) {
                    printf( mask, buffer );
                } else {
                    printf( unprintableMask );
                }
            }
        } else {
            printf( emptyMask );
        }
    }
    fseek( exeFile->file, prevPos, SEEK_SET );
}

void printHexDump( unsigned long addr, unsigned long length, ExeFile *exeFile,
                   Parameters *param )
/*****************************************************************/
{
    unsigned long   i;

    for( i = addr; i < addr + length; i = ( i / 16L + 1L ) * 16 ) {
        if( param->hexIndentSpaces == -1 ) {
            printIndent( false, param );
        } else {
            printf( "%*s", param->hexIndentSpaces, "" );
        }
        printf( "0x%8.8X  ", i );

        printHexLine( i, addr + length, exeFile,
                      "%2.2X ", "-- ", "", false, true );
        printf( " " );
        printHexLine( i, addr + length, exeFile,
                      "%c", " ", ".", true, false );
        printf( "\n" );
    }
}
