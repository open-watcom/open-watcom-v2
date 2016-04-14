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
* Description:  Standalone disassembler initialization routines.
*
****************************************************************************/


#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#if defined( __WATCOMC__ )
    #include <process.h>
#endif
#include "wio.h"
#include "dis.h"
#include "global.h"
#include "init.h"
#include "buffer.h"
#include "memfuncs.h"
#include "hashtabl.h"
#include "publics.h"
#include "args.h"
#include "print.h"
#include "labproc.h"
#include "refproc.h"
#include "main.h"
#include "identsec.h"
#include "fini.h"
#include "formasm.h"

#include "clibext.h"


struct recognized_struct {
    char                *name;
    section_type        type;
};

typedef struct recognized_struct recognized_struct;

#define SEC_NAME_LEN 8

#define HANDLE_TO_SECTION_TABLE_SIZE 53
#define HANDLE_TO_LIST_TABLE_SIZE 53
#define SYMBOL_TO_LABEL_TABLE_SIZE 53
#define RECOGNITION_TABLE_SIZE 29

#define CPP_COMMENT_STRING "// "
#define MASM_COMMENT_STRING "; "

char    *CommentString  = CPP_COMMENT_STRING;

extern wd_options       Options;
extern char             LabelChar;
extern char             QuoteChar;
extern int              OutputDest;
extern char *           ListFileName;

extern orl_handle       ORLHnd;
extern orl_file_handle  ObjFileHnd;
extern char *           ObjFileName;

extern dis_handle       DHnd;

extern hash_table       HandleToSectionTable;
extern hash_table       HandleToLabelListTable;
extern hash_table       HandleToRefListTable;
extern hash_table       SymbolToLabelTable;
extern hash_table       NameRecognitionTable;
extern hash_table       SkipRefTable;

extern section_list_struct      Sections;
extern publics_struct           Publics;

extern orl_sec_handle           debugHnd;

// sections that require name-checking should be inserted in this array
recognized_struct RecognizedName[] = {
    {".pdata", SECTION_TYPE_PDATA}, {".drectve", SECTION_TYPE_DRECTVE},
    {".bss", SECTION_TYPE_BSS}, {".text", SECTION_TYPE_TEXT},
    {".debug_line", SECTION_TYPE_LINES},
};

static char *intelSkipRefList[] = { "FIWRQQ", // boundary relocs
                                    "FIDRQQ",
                                    "FIERQQ",
                                    "FICRQQ",
                                    "FISRQQ",
                                    "FIARQQ",
                                    "FIFRQQ",
                                    "FIGRQQ",
                                    "FJCRQQ", // boundary + 1 relocs
                                    "FJSRQQ",
                                    "FJARQQ",
                                    "FJFRQQ",
                                    "FJGRQQ",
                                    NULL };

#define NUM_ELTS( a )   (sizeof(a) / sizeof((a)[0]))

static orl_sec_handle           symbolTable;
static orl_sec_handle           dynSymTable;
static orl_sec_handle           drectveSection;
static orl_funcs                oFuncs;
static section_list_struct      relocSections;
static char *                   objFileBuf;
static long                     objFilePos;
static unsigned long            objFileLen;


int IsIntelx86( void )
{
    switch( GetMachineType() ) {
    case ORL_MACHINE_TYPE_I386:
    case ORL_MACHINE_TYPE_I8086:
        return( 1 );
    default:
        return( 0 );
    }
}

orl_file_format GetFormat( void )
{
    return( ORLFileGetFormat( ObjFileHnd ) );
}

static orl_return nopCallBack( char *str, void *cookie  )
{
    str = str;
    cookie = cookie;
    return( ORL_OKAY );
}

static orl_return scanTabCallBack( orl_sec_handle sh, orl_sec_offset start,
                                   orl_sec_offset end, void *cookie )
{
    section_ptr         sec;
    hash_data           *dp;
    scantab_ptr         sp;
    scantab_ptr         tmp;
    scantab_struct      senitel;

    cookie = cookie;
    if( !sh ) return( ORL_OKAY );
    if( start >= end ) return( ORL_OKAY );
    dp = HashTableQuery( HandleToSectionTable, (hash_value) sh );
    if( !dp ) return( ORL_OKAY );
    sec = (section_ptr) *dp;
    if( !sec ) return( ORL_OKAY );

    sp = MemAlloc( sizeof( scantab_struct ) );
    if( !sp ) return( ORL_OUT_OF_MEMORY );
    memset( sp, 0, sizeof( scantab_struct ) );
    sp->start = start;
    sp->end = end;

    senitel.next = sec->scan;
    tmp = &senitel;
    while( tmp->next && ( tmp->next->end < start ) ) {
        tmp = tmp->next;
    }

    if( tmp->next ) {
        if( end < tmp->next->start ) {
            sp->next = tmp->next;
            tmp->next = sp;
        } else {
            // The two records will be merged into one
            if( end > tmp->next->end ) {
                tmp->next->end = end;
            }
            if( start < tmp->next->start ) {
                tmp->next->start = start;
            }
            MemFree( sp );

            // check if we must do additional merging
            sp = tmp->next;
            while( sp->next && ( sp->end > sp->next->start ) ) {
                if( sp->end < sp->next->end ) {
                    sp->end = sp->next->end;
                }
                tmp = sp->next;
                sp->next = tmp->next;
                MemFree( tmp );
            }
        }
    } else {
        tmp->next = sp;
    }

    // restore the list
    sec->scan = senitel.next;

    return( ORL_OKAY );
}

static return_val processDrectveSection( orl_sec_handle shnd )
{
    orl_return          o_error;
    orl_note_callbacks  cb;

    if( !shnd )
        return( RC_OKAY );

    cb.export_fn = nopCallBack;
    cb.deflib_fn = nopCallBack;
    cb.entry_fn = nopCallBack;
    cb.scantab_fn = scanTabCallBack;

    o_error = ORLNoteSecScan( shnd, &cb, NULL );
    if( o_error == ORL_OKAY )
        return( RC_OKAY );
    if( o_error == ORL_OUT_OF_MEMORY )
        return( RC_OUT_OF_MEMORY );
    return( RC_ERROR );
}

static return_val addRelocSection( orl_sec_handle shnd )
{
    section_ptr         sec;

    if( relocSections.first && ( GetFormat() == ORL_OMF ) )
        return( RC_OKAY );

    sec = MemAlloc( sizeof( section_struct ) );
    if( sec ) {
        memset( sec, 0, sizeof( section_struct ) );
        sec->shnd = shnd;
        sec->next = NULL;
        if( relocSections.first ) {
            relocSections.last->next = sec;
            relocSections.last = sec;
        } else {
            relocSections.first = sec;
            relocSections.last = sec;
        }
    } else {
        return( RC_OUT_OF_MEMORY );
    }
    return( RC_OKAY );
}

static return_val registerSec( orl_sec_handle shnd, section_type type )
{
    section_ptr         sec;
    return_val          error;

    sec = MemAlloc( sizeof( section_struct ) );
    if( sec ) {
        error = HashTableInsert( HandleToSectionTable, (hash_value) shnd, (hash_data) sec );
        if( error == RC_OKAY ) {
            memset( sec, 0, sizeof( section_struct ) );
            sec->shnd = shnd;
            sec->name = ORLSecGetName( shnd );
            sec->type = type;
            sec->next = NULL;
            if( Sections.first ) {
                Sections.last->next = sec;
                Sections.last = sec;
            } else {
                Sections.first = sec;
                Sections.last = sec;
            }
        } else {
            MemFree( sec );
            return( RC_OUT_OF_MEMORY );
        }
    } else {
        return( RC_OUT_OF_MEMORY );
    }
    return( RC_OKAY );
}

static return_val addListToPublics( label_list list )
{
    label_list_ptr      list_ptr;

    list_ptr = (label_list_ptr) MemAlloc( sizeof( label_list_ptr_struct ) );
    if( list_ptr ) {
        list_ptr->list = list;
        if( Publics.label_lists == NULL ) {
            list_ptr->next = NULL;
            Publics.label_lists = list_ptr;
        } else {
            list_ptr->next = Publics.label_lists;
            Publics.label_lists = list_ptr;
        }
    } else {
        return( RC_OUT_OF_MEMORY );
    }
    return( RC_OKAY );
}

static return_val createLabelList( orl_sec_handle shnd )
{
    label_list          list;
    return_val          error;

    list = MemAlloc( sizeof( label_list_struct ) );
    if( list ) {
        list->first = NULL;
        list->last = NULL;
        error = HashTableInsert( HandleToLabelListTable, (hash_value) shnd, (hash_data) list );
        if( error == RC_OKAY ) {
            if( (Options & PRINT_PUBLICS) && shnd != 0 ) {
                error = addListToPublics( list );
                if( error != RC_OKAY ) {
                    MemFree( list );
                }
            }
        } else {
            MemFree( list );
        }
    } else {
        error = RC_OUT_OF_MEMORY;
    }
    return( error );
}

static return_val createRefList( orl_sec_handle shnd )
{
    ref_list    list;
    return_val  error;

    list = MemAlloc( sizeof( ref_list_struct ) );
    if( list ) {
        list->first = NULL;
        list->last = NULL;
        error = HashTableInsert( HandleToRefListTable, (hash_value) shnd, (hash_data) list );
        if( error != RC_OKAY ) {
            MemFree( list );
        }
    } else {
        error = RC_OUT_OF_MEMORY;
    }
    return( error );
}

static return_val textOrDataSectionInit( orl_sec_handle shnd )
{
    return_val          error;
    orl_sec_handle      reloc_sec;

    error = createLabelList( shnd );
    if( error == RC_OKAY ) {
        error = createRefList( shnd );
        if( error == RC_OKAY ) {
            reloc_sec = ORLSecGetRelocTable( shnd );
            if( reloc_sec ) {
                error = addRelocSection( reloc_sec );
            }
        }
    }
    return( error );
}

static orl_return sectionInit( orl_sec_handle shnd )
{
    section_type        type;
    return_val          error = RC_OKAY;

    type = IdentifySec( shnd );
    switch( type ) {
        case SECTION_TYPE_SYM_TABLE:
            symbolTable = shnd;
            // Might have a label or relocation in symbol section
            error = registerSec( shnd, type );
            if( error == RC_OKAY ) {
                error = createLabelList( shnd );
            }
            break;
        case SECTION_TYPE_DYN_SYM_TABLE:
            dynSymTable = shnd;
            // Might have a label or relocation in dynsym section
            error = registerSec( shnd, type );
            if( error == RC_OKAY ) {
                error = createLabelList( shnd );
            }
            break;
        case SECTION_TYPE_DRECTVE:
            if( GetFormat() == ORL_OMF ) {
                drectveSection = shnd;
                break;
            } // else fall through
        case SECTION_TYPE_BSS:
            error = registerSec( shnd, type );
            if( error == RC_OKAY ) {
                error = createLabelList( shnd );
            }
            break;
        case SECTION_TYPE_RELOCS:
            // Ignore OMF relocs section
            break;
        case SECTION_TYPE_LINES:
            debugHnd = shnd;
            type = SECTION_TYPE_DATA;
            // fall through
        case SECTION_TYPE_TEXT:
        case SECTION_TYPE_PDATA:
        case SECTION_TYPE_DATA:
        default: // Just in case we get a label or relocation in these sections
            error = registerSec( shnd, type );
            if( error == RC_OKAY ) {
                error = textOrDataSectionInit( shnd );
            }
            break;
    }
    if( error == RC_OKAY )
        return( ORL_OKAY );
    if( error == RC_OUT_OF_MEMORY )
        return( ORL_OUT_OF_MEMORY );
    return( ORL_ERROR );
}


static void openError( char * file_name )
{
    perror( file_name );
    exit( 1 );
}

static void openFiles( void )
{
    int objhdl;

    objhdl = open( ObjFileName, O_RDONLY | O_BINARY );
    if( objhdl != -1 ) {
        if( ListFileName ) {
            OutputDest = open( ListFileName, O_WRONLY | O_CREAT | O_TRUNC, PMODE_RW );
            if( OutputDest == -1 ) openError( ListFileName );
            ChangePrintDest( OutputDest );
        }
        objFileLen = filelength( objhdl );
        if( objFileLen == 0 ) {
            LeaveProgram( RC_OKAY, WHERE_OBJ_ZERO_LEN );
        }
        objFileBuf = MemAlloc( objFileLen );
        objFilePos = 0;
        if( read( objhdl, objFileBuf, objFileLen ) == -1 ) {
            openError( ObjFileName );
        }
        close( objhdl );
    } else {
        openError( ObjFileName );
    }
}

static void * objRead( void *hdl, size_t len )
/********************************************/
{
    void *      retval;

    hdl = hdl;
    if( (unsigned long)( objFilePos + len ) > objFileLen )
        return NULL;
    retval = objFileBuf + objFilePos;
    objFilePos += (long)len;
    return retval;
}

static long objSeek( void *hdl, long pos, int where )
/***************************************************/
{
    hdl = hdl;
    if( where == SEEK_SET ) {
        objFilePos = pos;
    } else if( where == SEEK_CUR ) {
        objFilePos += pos;
    } else {
        objFilePos = objFileLen - (unsigned long)pos;
    }
    return objFilePos;
}

extern void CloseObjFile( void )
/******************************/
{
    MemFree( ObjFileName );
    MemFree( objFileBuf );
}

static void initGlobals( void )
{
    Sections.first = NULL;
    Sections.last = NULL;
    Publics.label_lists = NULL;
    Publics.public_symbols = NULL;
    Publics.number = 0;
}

static return_val createHashTables( void )
{
    HandleToSectionTable = HashTableCreate( HANDLE_TO_SECTION_TABLE_SIZE, HASH_NUMBER, NumberCmp );
    if( HandleToSectionTable ) {
        HandleToLabelListTable = HashTableCreate( HANDLE_TO_LIST_TABLE_SIZE, HASH_NUMBER, NumberCmp );
        if( HandleToLabelListTable ) {
            HandleToRefListTable = HashTableCreate( HANDLE_TO_LIST_TABLE_SIZE, HASH_NUMBER, NumberCmp );
            if( HandleToRefListTable ) {
                SymbolToLabelTable = HashTableCreate( SYMBOL_TO_LABEL_TABLE_SIZE, HASH_NUMBER, NumberCmp );
                if( SymbolToLabelTable ) {
                    NameRecognitionTable = HashTableCreate( RECOGNITION_TABLE_SIZE, HASH_STRING, (hash_table_comparison_func) stricmp );
                    if( !NameRecognitionTable ) {
                        HashTableFree( HandleToSectionTable );
                        HashTableFree( HandleToSectionTable );
                        HashTableFree( HandleToLabelListTable );
                        HashTableFree( HandleToRefListTable );
                        HashTableFree( SymbolToLabelTable );
                        return( RC_OUT_OF_MEMORY );
                    }
                } else {
                    HashTableFree( HandleToSectionTable );
                    HashTableFree( HandleToLabelListTable );
                    HashTableFree( HandleToRefListTable );
                    return( RC_OUT_OF_MEMORY );
                }
            } else {
                HashTableFree( HandleToSectionTable );
                HashTableFree( HandleToLabelListTable );
                return( RC_OUT_OF_MEMORY );
            }
        } else {
            HashTableFree( HandleToSectionTable );
            return( RC_OUT_OF_MEMORY );
        }
    } else {
        return( RC_OUT_OF_MEMORY );
    }
    return( RC_OKAY );
}

static return_val initHashTables( void )
{
    int         loop;
    return_val  error;

    error = createHashTables();
    if( error == RC_OKAY ) {
        for( loop = 0; loop < NUM_ELTS( RecognizedName ); loop++ ) {
            HashTableInsert( NameRecognitionTable, (hash_value) RecognizedName[loop].name, RecognizedName[loop].type );
        }
    }
    return( error );
}

orl_machine_type GetMachineType( void )
{
    return( ORLFileGetMachineType( ObjFileHnd ) );
}

/*
 *  Functions to convert data from the file format to the host format where the data may be byte swapped.
 *  If the ORL file is not marked as the opposite endianness as that of the host, then the data will
 *  not be byte swapped. This may not always be the correct behaviour, but if the data is not marked as
 *  a particular endianness, what are we to do about it?
 */

#ifdef __BIG_ENDIAN__
#define ENDIANNESS_TEST     ORL_FILE_FLAG_LITTLE_ENDIAN
#else
#define ENDIANNESS_TEST     ORL_FILE_FLAG_BIG_ENDIAN
#endif 
 
unsigned_16 FileU16toHostU16(unsigned_16 value)
{
    orl_file_flags  flags = ORLFileGetFlags( ObjFileHnd );
    if( flags & ENDIANNESS_TEST )
        return ( SWAPNC_16( value ) );
    return value;
}

unsigned_32 FileU32toHostU32(unsigned_32 value)
{
    orl_file_flags  flags = ORLFileGetFlags( ObjFileHnd );
    if( flags & ENDIANNESS_TEST )
        return ( SWAPNC_32( value ) );
    return value;
}

unsigned_64 FileU64toHostU64(unsigned_64 value)
{
    orl_file_flags  flags = ORLFileGetFlags( ObjFileHnd );
    if( flags & ENDIANNESS_TEST ){
        unsigned_64 new_value;        
        new_value.u._64[0] = SWAPNC_64( value.u._64[0] );
        return new_value;
    }
    return value;
}

static return_val initORL( void )
{
    orl_file_flags      flags;
    orl_machine_type    machine_type;
    orl_return          o_error = ORL_OKAY;
    orl_file_format     type;
    bool                byte_swap;

    oFuncs.alloc = &MemAlloc;
    oFuncs.free = &MemFree;
    oFuncs.read = &objRead;
    oFuncs.seek = &objSeek;
    ORLHnd = ORLInit( &oFuncs );
    if( ORLHnd ) {
        type = ORLFileIdentify( ORLHnd, NULL );
        if( type == ORL_UNRECOGNIZED_FORMAT ) {
            PrintErrorMsg( RC_OKAY, WHERE_NOT_COFF_ELF );
            return( RC_ERROR );
        }
        ObjFileHnd = ORLFileInit( ORLHnd, NULL, type );
        if( ObjFileHnd ) {
            // check byte order
            flags = ORLFileGetFlags( ObjFileHnd );
            byte_swap = false;
#ifdef __BIG_ENDIAN__
            if( flags & ORL_FILE_FLAG_LITTLE_ENDIAN ) {
                byte_swap = true;
            }
#else
            if( flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
                byte_swap = true;
            }
#endif

            // check intended machine type
            machine_type = GetMachineType();
            switch( machine_type ) {
            // If there's no machine specific code, the CPU we choose shouldn't
            // matter; there are some object files like this.
            case ORL_MACHINE_TYPE_NONE:
            case ORL_MACHINE_TYPE_ALPHA:
                if( DisInit( DISCPU_axp, &DHnd, byte_swap ) != DR_OK ) {
                    ORLFini( ORLHnd );
                    PrintErrorMsg( RC_OKAY, WHERE_UNSUPPORTED_PROC );
                    return( RC_ERROR );
                }
                break;
            case ORL_MACHINE_TYPE_PPC601:
                if( DisInit( DISCPU_ppc, &DHnd, byte_swap ) != DR_OK ) {
                    ORLFini( ORLHnd );
                    PrintErrorMsg( RC_OKAY, WHERE_UNSUPPORTED_PROC );
                    return( RC_ERROR );
                }
                // PAS assembler expects "", not \" for quotes.
                if( !(Options & METAWARE_COMPATIBLE) ) {
                    QuoteChar = '\"';
                }
                break;
            case ORL_MACHINE_TYPE_R3000:
            case ORL_MACHINE_TYPE_R4000:
                if( DisInit( DISCPU_mips, &DHnd, byte_swap ) != DR_OK ) {
                    ORLFini( ORLHnd );
                    PrintErrorMsg( RC_OKAY, WHERE_UNSUPPORTED_PROC );
                    return( RC_ERROR );
                }
                break;
            case ORL_MACHINE_TYPE_I386:
            case ORL_MACHINE_TYPE_I8086:
                if( DisInit( DISCPU_x86, &DHnd, byte_swap ) != DR_OK ) {
                    ORLFini( ORLHnd );
                    PrintErrorMsg( RC_OKAY, WHERE_UNSUPPORTED_PROC );
                    return( RC_ERROR );
                }
                break;
            case ORL_MACHINE_TYPE_AMD64:
                if( DisInit( DISCPU_x64, &DHnd, byte_swap ) != DR_OK ) {
                    ORLFini( ORLHnd );
                    PrintErrorMsg( RC_OKAY, WHERE_UNSUPPORTED_PROC );
                    return( RC_ERROR );
                }
                break;
            case ORL_MACHINE_TYPE_SPARC:
            case ORL_MACHINE_TYPE_SPARCPLUS:
                if( DisInit( DISCPU_sparc, &DHnd, byte_swap ) != DR_OK ) {
                    ORLFini( ORLHnd );
                    PrintErrorMsg( RC_OKAY, WHERE_UNSUPPORTED_PROC );
                    return( RC_ERROR );
                }
                break;
            default:
                    ORLFini( ORLHnd );
                    PrintErrorMsg( RC_OKAY, WHERE_UNSUPPORTED_PROC );
                    return( RC_ERROR );
            }
            return( RC_OKAY );
        } else {
            o_error = ORLGetError( ORLHnd );
            ORLFini( ORLHnd );
            // An "out of memory" error is not necessarily what it seems.
            // The ORL returns this error when encountering a bad or
            // unrecognized object file record.
            if( o_error == ORL_OUT_OF_MEMORY ) {
                PrintErrorMsg( RC_OUT_OF_MEMORY, WHERE_OPENING_ORL );
                return( RC_OUT_OF_MEMORY );
            } else {
                PrintErrorMsg( RC_ERROR, WHERE_OPENING_ORL );
                return( RC_ERROR );
            }
        }
    } else {
        return( RC_OUT_OF_MEMORY );
    }
}

static return_val initServicesUsed( void )
{
    return( initORL() );
}

static return_val initSectionTables( void )
{
    return_val          error;
    orl_return          o_error;
    section_ptr         sec;

    // list for references to external functions, etc.
    error = createLabelList( 0 );
    if( error == RC_OKAY ) {
        o_error = ORLFileScan( ObjFileHnd, NULL, &sectionInit );
        if( o_error == ORL_OKAY && symbolTable ) {
            o_error = DealWithSymbolSection( symbolTable );
            if( o_error == ORL_OKAY && dynSymTable ) {
                o_error = DealWithSymbolSection( dynSymTable );
            }
            if( o_error == ORL_OKAY ) {
                sec = relocSections.first;
                while( sec ) {
                    o_error = DealWithRelocSection( sec->shnd );
                    if( o_error != ORL_OKAY ) {
                        if( o_error == ORL_OUT_OF_MEMORY ) {
                            return( RC_OUT_OF_MEMORY );
                        } else {
                            return( RC_ERROR );
                        }
                    }
                    relocSections.first = sec->next;
                    MemFree( sec );
                    sec = relocSections.first;
                }
                error = processDrectveSection( drectveSection );
            } else {
                if( o_error == ORL_OUT_OF_MEMORY ) {
                    return( RC_OUT_OF_MEMORY );
                } else {
                    return( RC_ERROR );
                }
            }
        }
    }
    return( error );
}

void PrintErrorMsg( return_val exit_code, int where )
{
    ChangePrintDest( STDERR_FILENO );
    if( exit_code == RC_OUT_OF_MEMORY ) {
        BufferMsg( OUT_OF_MEMORY );
    } else if( exit_code == RC_ERROR ) {
        BufferMsg( ERROR_OCCURRED );
    }
    BufferMsg( where );
    BufferConcatNL();
    BufferPrint();
    ChangePrintDest( OutputDest );
}

void LeaveProgram( return_val exit_code, int message )
{
    PrintErrorMsg( exit_code, message );
    exit( exit_code );
}

void Init( void )
{
    char                cmd_line[ CMD_LINE_LEN ];
    return_val          error;
    char                **list;

    OutputDest = STDOUT_FILENO;
    ChangePrintDest( OutputDest );

    relocSections.first = NULL;
    relocSections.last = NULL;
    if( !MsgInit() ) {
        // MsgInit does its own error message printing
        exit( -1 );
    }
    MemOpen();

    getcmd( cmd_line );
    HandleArgs( cmd_line );

    openFiles();
    initGlobals();
    error = initHashTables();
    if( error == RC_OKAY ) {
        error = initServicesUsed();
        if( error == RC_OKAY ) {
            error = initSectionTables();
            if( error != RC_OKAY ) {
                // free hash tables and services
                MemClose();
                LeaveProgram( error, WHERE_CREATE_SEC_TABLES );
            }
        } else {
            // free hash tables
            CloseFiles();
            FreeHashTables();
            // initServicesUsed does its own error message printing
            exit( error );
        }
    } else {
        CloseFiles();
        MemClose();
        LeaveProgram( error, WHERE_INIT_HASH_TABLES );
    }
    if( Options & PRINT_PUBLICS ) {
        CreatePublicsArray();
    }
    if( IsMasmOutput() ) {
        CommentString = MASM_COMMENT_STRING;
    }
    if( IsIntelx86() ) {
        SkipRefTable = HashTableCreate( RECOGNITION_TABLE_SIZE, HASH_STRING,
                                        (hash_table_comparison_func) stricmp );
        if( SkipRefTable ) {
            list = intelSkipRefList;
            while( *list ) {
                error = HashTableInsert( SkipRefTable, (hash_value) *list,
                                         (hash_data) *list );
                if( error != RC_OKAY ) break;
                list++;
            }
        }
    }

    if( !LabelChar ) {
        if( IsMasmOutput() ) {
            LabelChar = 'L';
        } else {
            LabelChar = 'X';
        }
    }
}
