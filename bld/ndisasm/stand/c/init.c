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


#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <process.h>

#include "dis.h"
#include "init.h"
#include "buffer.h"
#include "memfuncs.h"
#include "hashtabl.h"
#include "publics.h"
#include "args.h"
#include "print.h"
#include "labproc.h"
#include "refproc.h"
#include "msg.h"
#include "main.h"
#include "identsec.h"
#include "fini.h"
#include "formasm.h"


typedef struct recognized_struct {
    char *              name;
    section_type        type;
};

typedef struct recognized_struct recognized_struct;

#define SEC_NAME_LEN 8
#define OBJ_FILE_FLAGS O_RDONLY | O_BINARY
#define LIST_FILE_FLAGS O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU

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
static orl_sec_handle           drectveSection;
static orl_funcs                oFuncs;
static section_list_struct      relocSections;
static char *                   objFileBuf;
static unsigned long            objFilePos;
static unsigned long            objFileLen;


int IsIntelx86()
{
    switch( GetMachineType() ) {
    case( ORL_MACHINE_TYPE_I386 ):
    case( ORL_MACHINE_TYPE_I8086 ):
        return( 1 );
    default:
        return( 0 );
    }
}

orl_file_format GetFormat()
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

    if( !shnd ) return( OKAY );

    cb.export_fn = nopCallBack;
    cb.deflib_fn = nopCallBack;
    cb.entry_fn = nopCallBack;
    cb.scantab_fn = scanTabCallBack;

    o_error = ORLNoteSecScan( shnd, &cb, NULL );
    if( o_error != ORL_OKAY ) {
        if( o_error == ORL_OUT_OF_MEMORY ) {
            return( OUT_OF_MEMORY );
        } else {
            return( ERROR );
        }
    }
    return( OKAY );
}

static return_val addRelocSection( orl_sec_handle shnd )
{
    section_ptr         sec;

    if( relocSections.first && ( GetFormat() == ORL_OMF ) ) return( ORL_OKAY );

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
        return( OUT_OF_MEMORY );
    }
    return( OKAY );
}

static return_val registerSec( orl_sec_handle shnd, section_type type )
{
    section_ptr         sec;
    return_val          error;

    sec = MemAlloc( sizeof( section_struct ) );
    if( sec ) {
        error = HashTableInsert( HandleToSectionTable, (hash_value) shnd, (hash_data) sec );
        if( error == OKAY ) {
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
            return( OUT_OF_MEMORY );
        }
    } else {
        return( OUT_OF_MEMORY );
    }
    return( OKAY );
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
        return( OUT_OF_MEMORY );
    }
    return( OKAY );
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
        if( error == OKAY ) {
            if( (Options & PRINT_PUBLICS) && shnd != 0 ) {
                error = addListToPublics( list );
                if( error != OKAY ) {
                    MemFree( list );
                }
            }
        } else {
            MemFree( list );
        }
    } else {
        error = OUT_OF_MEMORY;
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
        if( error != OKAY ) {
            MemFree( list );
        }
    } else {
        error = OUT_OF_MEMORY;
    }
    return( error );
}

static return_val textOrDataSectionInit( orl_sec_handle shnd )
{
    return_val          error;
    orl_sec_handle      reloc_sec;

    error = createLabelList( shnd );
    if( error == OKAY ) {
        error = createRefList( shnd );
        if( error == OKAY ) {
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
    return_val          error = OKAY;

    type = IdentifySec( shnd );
    switch( type ) {
        case SECTION_TYPE_SYM_TABLE:
            symbolTable = shnd;
            break;
        case SECTION_TYPE_DRECTVE:
            if( GetFormat() == ORL_OMF ) {
                drectveSection = shnd;
                break;
            } // else fall through
        case SECTION_TYPE_BSS:
            error = registerSec( shnd, type );
            if( error == OKAY ) {
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
            if( error == OKAY ) {
                error = textOrDataSectionInit( shnd );
            }
            break;
    }
    switch( error ) {
        case OUT_OF_MEMORY:
           return( ORL_OUT_OF_MEMORY );
        case ERROR:
           return( ORL_ERROR );
    }
    return( ORL_OKAY );
}


static void openError( char * file_name ) {
    perror( file_name );
    exit( 1 );
}

static void openFiles( void )
{
    int objhdl;

    objhdl = open( ObjFileName, OBJ_FILE_FLAGS );
    if( objhdl != -1 ) {
        if( ListFileName ) {
            OutputDest = open( ListFileName, LIST_FILE_FLAGS, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP );
            if( OutputDest == -1 ) openError( ListFileName );
            ChangePrintDest( OutputDest );
        } else {
            OutputDest = STDOUT_FILENO;
        }
        objFileLen = filelength( objhdl );
        if( objFileLen == 0 ) {
            LeaveProgram( OKAY, WHERE_OBJ_ZERO_LEN );
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

static void * objRead( void *hdl, int len )
/*****************************************/
{
    void *      retval;

    hdl = hdl;
    if( objFilePos + len > objFileLen ) return NULL;
    retval = objFileBuf + objFilePos;
    objFilePos += len;
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
        objFilePos = objFileLen - pos;
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
                        return( OUT_OF_MEMORY );
                    }
                } else {
                    HashTableFree( HandleToSectionTable );
                    HashTableFree( HandleToLabelListTable );
                    HashTableFree( HandleToRefListTable );
                    return( OUT_OF_MEMORY );
                }
            } else {
                HashTableFree( HandleToSectionTable );
                HashTableFree( HandleToLabelListTable );
                return( OUT_OF_MEMORY );
            }
        } else {
            HashTableFree( HandleToSectionTable );
            return( OUT_OF_MEMORY );
        }
    } else {
        return( OUT_OF_MEMORY );
    }
    return( OKAY );
}

static return_val initHashTables( void )
{
    int         loop;
    return_val  error;

    error = createHashTables();
    if( error == OKAY ) {
        for( loop = 0; loop < NUM_ELTS( RecognizedName ); loop++ ) {
            HashTableInsert( NameRecognitionTable, (hash_value) RecognizedName[loop].name, RecognizedName[loop].type );
        }
    }
    return( error );
}

orl_machine_type GetMachineType()
{
    return( ORLFileGetMachineType( ObjFileHnd ) );
}

static return_val initORL( void )
{
    orl_file_flags      flags;
    orl_machine_type    machine_type;
    orl_return          error = OKAY;
    orl_file_format     type;

    oFuncs.alloc = &MemAlloc;
    oFuncs.free = &MemFree;
    oFuncs.read = &objRead;
    oFuncs.seek = &objSeek;
    ORLHnd = ORLInit( &oFuncs );
    if( ORLHnd ) {
        type = ORLFileIdentify( ORLHnd, NULL );
        if( type == ORL_UNRECOGNIZED_FORMAT ) {
            PrintErrorMsg( OKAY, WHERE_NOT_COFF_ELF );
            return( ERROR );
        }
        ObjFileHnd = ORLFileInit( ORLHnd, NULL, type );
        if( ObjFileHnd ) {
            // check intended machine type
            machine_type = GetMachineType();
            switch( machine_type ) {
            case ORL_MACHINE_TYPE_ALPHA:
                if( DisInit( DISCPU_axp, &DHnd ) != DR_OK ) {
                    ORLFini( ORLHnd );
                    PrintErrorMsg( OKAY, WHERE_UNSUPPORTED_PROC );
                    return( ERROR );
                }
                break;
            case ORL_MACHINE_TYPE_PPC601:
                if( DisInit( DISCPU_ppc, &DHnd ) != DR_OK ) {
                    ORLFini( ORLHnd );
                    PrintErrorMsg( OKAY, WHERE_UNSUPPORTED_PROC );
                    return( ERROR );
                }
                // PAS assembler expects "", not \" for quotes.
                if( !(Options & METAWARE_COMPATIBLE) ) {
                    QuoteChar = '\"';
                }
                break;
            case ORL_MACHINE_TYPE_I386:
            case ORL_MACHINE_TYPE_I8086:
                if( DisInit( DISCPU_x86, &DHnd ) != DR_OK ) {
                    ORLFini( ORLHnd );
                    PrintErrorMsg( OKAY, WHERE_UNSUPPORTED_PROC );
                    return( ERROR );
                }
                break;
            default:
                    ORLFini( ORLHnd );
                    PrintErrorMsg( OKAY, WHERE_UNSUPPORTED_PROC );
                    return( ERROR );
            }
            // check byte order
            flags = ORLFileGetFlags( ObjFileHnd );
#if 0   /* MS doesn't set the flags consistently :-( */
            if( !(flags & ORL_FILE_FLAG_LITTLE_ENDIAN) ) {
                PrintErrorMsg( OKAY, WHERE_BIT_ENDIAN );
                error = ERROR;
            }
#endif
        } else {
            error = ORLGetError( ORLHnd );
            // An "out of memory" error is not necessarily what it seems.
            // The ORL returns this error when encountering a bad or
            // unrecognized object file record.
            if( error == ORL_OUT_OF_MEMORY ) {
                PrintErrorMsg( OUT_OF_MEMORY, WHERE_OPENING_ORL );
            } else {
                PrintErrorMsg( ERROR, WHERE_OPENING_ORL );
            }
        }
        if( error != OKAY ) {
            ORLFini( ORLHnd );
        }
    } else {
        error = OUT_OF_MEMORY;
    }
    return( error );
}

static return_val initServicesUsed()
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
    if( error == OKAY ) {
        o_error = ORLFileScan( ObjFileHnd, NULL, &sectionInit );
        if( o_error == ORL_OKAY && symbolTable ) {
            o_error = DealWithSymbolSection( symbolTable );
            if( o_error == ORL_OKAY ) {
                sec = relocSections.first;
                while( sec ) {
                    o_error = DealWithRelocSection( sec->shnd );
                    if( o_error != ORL_OKAY ) {
                        if( o_error == ORL_OUT_OF_MEMORY ) {
                            return( OUT_OF_MEMORY );
                        } else {
                            return( ERROR );
                        }
                    }
                    relocSections.first = sec->next;
                    MemFree( sec );
                    sec = relocSections.first;
                }
                error = processDrectveSection( drectveSection );
            } else {
                if( o_error == ORL_OUT_OF_MEMORY ) {
                    return( OUT_OF_MEMORY );
                } else {
                    return( ERROR );
                }
            }
        }
    }
    return( error );
}

void PrintErrorMsg( return_val exit_code, int where )
{
    ChangePrintDest( STDERR_FILENO );
    if( exit_code == OUT_OF_MEMORY ) {
        BufferMsg( OUT_OF_MEMORY );
    } else if( exit_code == ERROR ) {
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
    if( error == OKAY ) {
        error = initServicesUsed();
        if( error == OKAY ) {
            error = initSectionTables();
            if( error != OKAY ) {
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
                if( error != OKAY ) break;
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
