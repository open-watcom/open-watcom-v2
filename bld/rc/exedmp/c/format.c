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
* Description:  PE Dump Utility formatting routines.
*
****************************************************************************/

#include "banner.h"
#include "format.h"
#include "param.h"
#include "read.h"
#include "main.h"
#include "watcom.h"

extern const char *resTypes[];
extern const char *cpu_flags_labels[];
extern const char *ss_flags_labels[];
extern const char *dll_flags_labels[];
extern const char *obj_flags_labels[];
extern const char *hdr_flags_labels[];

const unsigned_32 cpu_masks_table[] = {
    7,          /* number of masks in table */
    PE_CPU_UNKNOWN,
    PE_CPU_386,
    PE_CPU_I860,
    PE_CPU_MIPS_R3000,
    PE_CPU_MIPS_R4000,
    PE_CPU_ALPHA,
    PE_CPU_POWERPC
};

const unsigned_32 hdr_masks_table[] = {
    15,         /* number of masks in table */
    PE_FLG_PROGRAM,
    PE_FLG_RELOCS_STRIPPED,
    PE_FLG_IS_EXECUTABLE,
    PE_FLG_LINNUM_STRIPPED,
    PE_FLG_LOCALS_STRIPPED,
    PE_FLG_MINIMAL_OBJ,
    PE_FLG_UPDATE_OBJ,
    PE_FLG_16BIT_MACHINE,
    PE_FLG_REVERSE_BYTE_LO,
    PE_FLG_32BIT_MACHINE,
    PE_FLG_FIXED,
    PE_FLG_FILE_PATCH,
    PE_FLG_FILE_SYSTEM,
    PE_FLG_LIBRARY,
    PE_FLG_REVERSE_BYTE_HI
};

const unsigned_32 ss_masks_table[] = {
    7,          /* number of masks in table */
    PE_SS_UNKNOWN,
    PE_SS_NATIVE,
    PE_SS_WINDOWS_GUI,
    PE_SS_WINDOWS_CHAR,
    PE_SS_OS2_CHAR,
    PE_SS_POSIX_CHAR,
    PE_SS_PL_DOSSTYLE
};

const unsigned_32 dll_masks_table[] = {
    4,          /* number of masks in table */
    PE_DLL_PERPROC_INIT,
    PE_DLL_PERPROC_TERM,
    PE_DLL_PERTHRD_INIT,
    PE_DLL_PERTHRD_TERM
};

const unsigned_32 obj_masks_table[] = {
    29,         /* number of masks in table */
    PE_OBJ_DUMMY,
    PE_OBJ_NOLOAD,
    PE_OBJ_GROUPED,
    PE_OBJ_NOPAD,
    PE_OBJ_TYPE_COPY,
    PE_OBJ_CODE,
    PE_OBJ_INIT_DATA,
    PE_OBJ_UNINIT_DATA,
    PE_OBJ_OTHER,
    PE_OBJ_LINK_INFO,
    PE_OBJ_OVERLAY,
    PE_OBJ_REMOVE,
    PE_OBJ_COMDAT,
    PE_OBJ_ALIGN_1,
    PE_OBJ_ALIGN_2,
    PE_OBJ_ALIGN_4,
    PE_OBJ_ALIGN_8,
    PE_OBJ_ALIGN_16,
    PE_OBJ_ALIGN_32,
    PE_OBJ_ALIGN_64,
    PE_OBJ_DISCARDABLE,
    PE_OBJ_NOT_CACHED,
    PE_OBJ_NOT_PAGABLE,
    PE_OBJ_SHARED,
    PE_OBJ_EXECUTABLE,
    PE_OBJ_READABLE,
    PE_OBJ_WRITABLE,
    PE_OBJ_ALIGN_MASK,
    PE_OBJ_ALIGN_SHIFT
};

#define printYes( x )   printf( "%s= %s\n", x, LBL_YES )
#define printNo( x )    printf( "%s= %s\n", x, LBL_NO )

int indentLevel = 0;

/* forward declarations */
void printHexDump( long int addr, size_t length, ExeFile *exeFile,
                   Parameters *param );
void printHexBytes( long int addr, size_t length, ExeFile *exeFile );


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
    printf( MSG_BANNER );
}

void printHelp( void )
/*********************/
{
    int i;

    printf( MSG_HELP );
    for( i = 0; resTypes[ i ] != NULL; i++ ) {
        if( *resTypes[ i ] != '\0' ) {
            printf( "       %s\n", resTypes[ i ] );
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

void printFlags( unsigned_32 value,
                 const unsigned_32 masks[], const char *labels[],
                 const char *indentString )
/****************************************************************/
{
    int i;
    int indentLen;
    int labelLen;
    int cursor;
    bool first;

    first = true;
    indentLen = strlen( indentString );
    cursor = indentLen;
    printf( indentString );
    for( i = 1; i <= masks[0]; i++ ) {
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

    if( param->dumpExeHeaderInfo ) {
        printf( MSG_PE_HEADER );
        printRuler( false, false, param );
        printf( MSG_PE_SIGNATURE ,              exeFile->pexHdr.signature );
        printf( MSG_PE_CPUTYPE ,                exeFile->pexHdr.cpu_type );
        for( i = 1; i <= cpu_masks_table[0]; i++ ) {
            if( exeFile->pexHdr.cpu_type == cpu_masks_table[i] ) {
                printf( MSG_PE_CPUTYPETEXT, cpu_flags_labels[i] );
                break;
            }
        }
        if( i > cpu_masks_table[0] ) {
            printf( MSG_PE_CPUTYPENOTRECOGNIZED );
        }
        printf( MSG_PE_NUMOBJECTS ,             exeFile->pexHdr.num_objects );
        printf( MSG_PE_TIMESTAMP ,              exeFile->pexHdr.time_stamp );
        printf( MSG_PE_SYMTABLE ,               exeFile->pexHdr.sym_table );
        printf( MSG_PE_NUMSYMS ,                exeFile->pexHdr.num_syms );
        printf( MSG_PE_NTHDRSIZE ,              exeFile->pexHdr.nt_hdr_size );
        printf( MSG_PE_FLAGS ,                  exeFile->pexHdr.flags );
        printFlags( exeFile->pexHdr.flags,
                    hdr_masks_table, hdr_flags_labels,
                    MSG_PE_FLAGSINDENT );
        printf( MSG_PE_MAGIC ,                  exeFile->pexHdr.magic );
        printf( MSG_PE_LNKMAJOR ,               exeFile->pexHdr.lnk_major );
        printf( MSG_PE_LNKMINOR ,               exeFile->pexHdr.lnk_minor );
        printf( MSG_PE_CODESIZE ,               exeFile->pexHdr.code_size,
                                                exeFile->pexHdr.code_size );
        printf( MSG_PE_INITDATASIZE ,           exeFile->pexHdr.init_data_size,
                                                exeFile->pexHdr.init_data_size );
        printf( MSG_PE_UNINITDATASIZE ,         exeFile->pexHdr.uninit_data_size,
                                                exeFile->pexHdr.uninit_data_size );
        printf( MSG_PE_ENTRYRVA ,               exeFile->pexHdr.entry_rva );
        printf( MSG_PE_CODEBASE ,               exeFile->pexHdr.code_base );
        printf( MSG_PE_DATABASE ,               exeFile->pexHdr.data_base );
        printf( MSG_PE_IMAGEBASE ,              exeFile->pexHdr.image_base );
        printf( MSG_PE_OBJECTALIGN ,            exeFile->pexHdr.object_align );
        printf( MSG_PE_FILEALIGN ,              exeFile->pexHdr.file_align );
        printf( MSG_PE_OSMAJOR ,                exeFile->pexHdr.os_major );
        printf( MSG_PE_OSMINOR ,                exeFile->pexHdr.os_minor );
        printf( MSG_PE_USERMAJOR ,              exeFile->pexHdr.user_major );
        printf( MSG_PE_USERMINOR ,              exeFile->pexHdr.user_minor );
        printf( MSG_PE_SUBSYSMAJOR ,            exeFile->pexHdr.subsys_major );
        printf( MSG_PE_SUBSYSMINOR ,            exeFile->pexHdr.subsys_minor );
        printf( MSG_PE_RSVD1 ,                  exeFile->pexHdr.rsvd1 );
        printf( MSG_PE_IMAGESIZE ,              exeFile->pexHdr.image_size );
        printf( MSG_PE_HEADERSIZE ,             exeFile->pexHdr.header_size );
        printf( MSG_PE_FILECHECKSUM ,           exeFile->pexHdr.file_checksum );
        printf( MSG_PE_SUBSYSTEM ,              exeFile->pexHdr.subsystem );
        for( i = 1; i <= ss_masks_table[0]; i++ ) {
            if( exeFile->pexHdr.subsystem == ss_masks_table[i] ) {
                printf( MSG_PE_SUBSYSTEMTEXT, ss_flags_labels[i] );
                break;
            }
        }
        if( i > ss_masks_table[0] ) {
            printf( MSG_PE_SUBSYSTEMNOTRECOGNIZED );
        }
        printf( MSG_PE_DLLFLAGS ,               exeFile->pexHdr.dll_flags );
        printFlags( exeFile->pexHdr.dll_flags,
                    dll_masks_table, dll_flags_labels,
                    MSG_PE_DLLFLAGSINDENT );
        printf( MSG_PE_STACKRESERVESIZE ,       exeFile->pexHdr.stack_reserve_size );
        printf( MSG_PE_STACKCOMMITSIZE ,        exeFile->pexHdr.stack_commit_size );
        printf( MSG_PE_HEAPRESERVESIZE ,        exeFile->pexHdr.heap_reserve_size );
        printf( MSG_PE_HEAPCOMMITSIZE ,         exeFile->pexHdr.heap_commit_size );
        printf( MSG_PE_TLSIDXADDR ,             exeFile->pexHdr.tls_idx_addr );
        printf( MSG_PE_NUMTABLES ,              exeFile->pexHdr.num_tables );
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
                    obj_masks_table, obj_flags_labels,
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
                         Parameters *param, long int addr, int depth )
/*********************************************************************/
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
                       Parameters  *param, long int addr, int depth )
/********************************************************************/
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
                printf( "%1s", &dir->name[ i ] );
            }
            printIndent( true, param );
            printf( MSG_DIR_NAMEADDRESS, getDirNameAbs( dir, exeFile ) );
            printIndent( false, param );
            printf( MSG_DIR_NAMESIZE, dir->nameSize );
        } else {
            printf( MSG_DIR_ID, dir->dir.id_name );
            if( depth == 1 && dir->dir.id_name < RT_COUNT ) {
                printIndent( false, param );
                printf( MSG_DIR_IDTYPE, resTypes[ dir->dir.id_name ] );
            }
        }
        printIndent( false, param );
        printf( MSG_DIR_ENTRYRVA, dir->dir.entry_rva & PE_RESOURCE_MASK );
    }
    printf( "\n" );

    if( dir->entryType == TABLE ) {
        printTableContents( dir->table, exeFile, param,
                            getDirChildAbs( dir, exeFile ), depth );
    } else {
        printDataContents( dir->data, exeFile, param,
                           getDirChildAbs( dir, exeFile ) );
    }
    printf( "\n" );
}

void printDataContents( ResDataEntry *data, ExeFile *exeFile,
                        Parameters   *param, long int addr )
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

void printHexBytes( long int addr, size_t length, ExeFile *exeFile )
/********************************************************************/
{
    long int     prevPos;
    unsigned_8  *buffer;
    size_t       count;
    int          i;

    count = (size_t) (length / sizeof( unsigned_8 ) );
    buffer = (unsigned_8 *) malloc( count );

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
    }
    free( (void *) buffer );
    fseek( exeFile->file, prevPos, SEEK_SET );
    printf( ERR_FORMAT_CANNOT_DUMP_HEX );
}

bool isPrintable( unsigned_8 buffer )
/************************************/
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

void printHexLine( long int lower, long int upper, ExeFile *exeFile,
                   const char *mask, const char *emptyMask,
                   const char *unprintableMask, bool testPrintable,
                   bool splitAtEight )
/********************************************************************/
{
    long int    prevPos;
    unsigned_8  buffer;
    long int    i;

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

void printHexDump( long int addr, size_t length, ExeFile *exeFile,
                   Parameters *param )
/*****************************************************************/
{
    long int    i;

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
