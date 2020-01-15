/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
    const char          *name;
    section_type        type;
};

typedef struct recognized_struct recognized_struct;

#define SEC_NAME_LEN 8

#define HANDLE_TO_SECTION_TABLE_SIZE 53
#define HANDLE_TO_LIST_TABLE_SIZE 53
#define SYMBOL_TO_LABEL_TABLE_SIZE 53
#define RECOGNITION_TABLE_SIZE 29

#define CPP_COMMENT_STRING  "// "
#define MASM_COMMENT_STRING "; "

char    *CommentString  = CPP_COMMENT_STRING;

// sections that require name-checking should be inserted in this array
recognized_struct RecognizedName[] = {
    {".pdata",      SECTION_TYPE_PDATA},
    {".drectve",    SECTION_TYPE_DRECTVE},
    {".bss",        SECTION_TYPE_BSS},
    {".text",       SECTION_TYPE_TEXT},
    {".debug_line", SECTION_TYPE_LINES},
};

static const char * const intelSkipRefList[] = {
    #define pick_fp(enum,name,alt_name,win,alt_win,others,alt_others) name,
    #include "fppatche.h"
    #undef pick_fp
    #define pick_fp(enum,name,alt_name,win,alt_win,others,alt_others) alt_name,
    #include "fppatche.h"
    #undef pick_fp
};

#define NUM_ELEMENTS( a )       (sizeof(a) / sizeof((a)[0]))

static orl_sec_handle           symbolTable = ORL_NULL_HANDLE;
static orl_sec_handle           dynSymTable = ORL_NULL_HANDLE;
static orl_sec_handle           drectveSection = ORL_NULL_HANDLE;
static section_list_struct      relocSections;
static char                     *objFileBuf = NULL;
static long                     objFilePos;
static unsigned long            objFileLen;


bool IsIntelx86( void )
{
    switch( GetMachineType() ) {
    case ORL_MACHINE_TYPE_I386:
    case ORL_MACHINE_TYPE_I8086:
    case ORL_MACHINE_TYPE_AMD64:
        return( true );
    default:
        return( false );
    }
}

orl_file_format GetFormat( void )
{
    return( ORLFileGetFormat( ObjFileHnd ) );
}

static orl_return nopCallBack( const char *str, void *cookie  )
{
    /* unused parameters */ (void)str; (void)cookie;

    return( ORL_OKAY );
}

static orl_return scanTabCallBack( orl_sec_handle shnd, const orl_sec_offset *pstart, const orl_sec_offset *pend, void *cookie )
{
    section_ptr         section;
    hash_data           *h_data;
    scantab_ptr         sp;
    scantab_ptr         tmp;
    scantab_struct      senitel;
    dis_sec_offset      start;
    dis_sec_offset      end;
    hash_key            h_key;

    /* unused parameters */ (void)cookie;

    if( shnd == ORL_NULL_HANDLE )
        return( ORL_OKAY );
    start = *pstart;
    end = *pend;
    if( start >= end )
        return( ORL_OKAY );
    h_key.u.sec_handle = shnd;
    h_data = HashTableQuery( HandleToSectionTable, h_key );
    if( h_data == NULL )
        return( ORL_OKAY );
    section = h_data->u.section;
    if( section == NULL )
        return( ORL_OKAY );

    sp = MemAlloc( sizeof( scantab_struct ) );
    if( sp == NULL )
        return( ORL_OUT_OF_MEMORY );
    memset( sp, 0, sizeof( scantab_struct ) );
    sp->start = start;
    sp->end = end;

    senitel.next = section->scan;
    for( tmp = &senitel; tmp->next != NULL; tmp = tmp->next ) {
        if( tmp->next->end >= start ) {
            break;
        }
    }

    if( tmp->next != NULL ) {
        if( end < tmp->next->start ) {
            sp->next = tmp->next;
            tmp->next = sp;
        } else {
            // The two records will be merged into one
            if( tmp->next->end < end ) {
                tmp->next->end = end;
            }
            if( tmp->next->start > start ) {
                tmp->next->start = start;
            }
            MemFree( sp );

            // check if we must do additional merging
            sp = tmp->next;
            while( sp->next != NULL && ( sp->end > sp->next->start ) ) {
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
    section->scan = senitel.next;

    return( ORL_OKAY );
}

static return_val processDrectveSection( orl_sec_handle shnd )
{
    orl_return          o_error;
    orl_note_callbacks  cb;

    if( shnd == ORL_NULL_HANDLE )
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
    section_ptr     section;

    if( relocSections.first != NULL && ( GetFormat() == ORL_OMF ) )
        return( RC_OKAY );

    section = MemAlloc( sizeof( section_struct ) );
    if( section != NULL ) {
        memset( section, 0, sizeof( section_struct ) );
        section->shnd = shnd;
        section->next = NULL;
        if( relocSections.first != NULL ) {
            relocSections.last->next = section;
            relocSections.last = section;
        } else {
            relocSections.first = section;
            relocSections.last = section;
        }
    } else {
        return( RC_OUT_OF_MEMORY );
    }
    return( RC_OKAY );
}

static return_val registerSec( orl_sec_handle shnd, section_type type )
{
    section_ptr         section;
    return_val          error;
    hash_entry_data     key_entry;

    section = MemAlloc( sizeof( section_struct ) );
    if( section != NULL ) {
        key_entry.key.u.sec_handle = shnd;
        key_entry.data.u.section = section;
        error = HashTableInsert( HandleToSectionTable, &key_entry );
        if( error == RC_OKAY ) {
            memset( section, 0, sizeof( section_struct ) );
            section->shnd = shnd;
            section->name = ORLSecGetName( shnd );
            section->type = type;
            section->next = NULL;
            if( Sections.first != NULL ) {
                Sections.last->next = section;
                Sections.last = section;
            } else {
                Sections.first = section;
                Sections.last = section;
            }
        } else {
            MemFree( section );
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

    list_ptr = (label_list_ptr)MemAlloc( sizeof( label_list_ptr_struct ) );
    if( list_ptr != NULL ) {
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
    hash_entry_data     key_entry;

    list = MemAlloc( sizeof( label_list_struct ) );
    if( list != NULL ) {
        list->first = NULL;
        list->last = NULL;
        key_entry.key.u.sec_handle = shnd;
        key_entry.data.u.sec_label_list = list;
        error = HashTableInsert( HandleToLabelListTable, &key_entry );
        if( error == RC_OKAY ) {
            if( (Options & PRINT_PUBLICS) && shnd != ORL_NULL_HANDLE ) {
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
    ref_list        list;
    return_val      error;
    hash_entry_data key_entry;

    list = MemAlloc( sizeof( ref_list_struct ) );
    if( list != NULL ) {
        list->first = NULL;
        list->last = NULL;
        key_entry.key.u.sec_handle = shnd;
        key_entry.data.u.sec_ref_list = list;
        error = HashTableInsert( HandleToRefListTable, &key_entry );
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
            if( reloc_sec != ORL_NULL_HANDLE ) {
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
        DebugHnd = shnd;
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


static return_val openFiles( void )
{
    FILE *objfp;

    objfp = fopen( ObjFileName, "rb" );
    if( objfp == NULL ) {
        perror( ObjFileName );
        return( RC_ERROR );
    }
    fseek( objfp, 0, SEEK_END );
    objFileLen = ftell( objfp );
    if( objFileLen == 0 ) {
        fclose( objfp );
        PrintErrorMsg( RC_OKAY, WHERE_OBJ_ZERO_LEN );
        exit( RC_OKAY );
    }
    objFileBuf = MemAlloc( objFileLen );
    fseek( objfp, 0, SEEK_SET );
    objFilePos = 0;
    if( fread( objFileBuf, 1, objFileLen, objfp ) != objFileLen ) {
        fclose( objfp );
        PrintErrorMsg( RC_ERROR, WHERE_OPENING_ORL );
        return( RC_ERROR );
    }
    fclose( objfp );
    if( ListFileName != NULL ) {
        OutputDest = fopen( ListFileName, "w" );
        if( OutputDest == NULL ) {
            perror( ListFileName );
            return( RC_ERROR );
        }
        ChangePrintDest( OutputDest );
    }
    return( RC_OKAY );
}

static void *objRead( FILE *fp, size_t len )
/******************************************/
{
    void        *retval;

    /* unused parameters */ (void)fp;

    if( (unsigned long)( objFilePos + len ) > objFileLen )
        return NULL;
    retval = objFileBuf + objFilePos;
    objFilePos += (long)len;
    return retval;
}

static int objSeek( FILE *fp, long pos, int where )
/*************************************************/
{
    /* unused parameters */ (void)fp;

    if( where == SEEK_SET ) {
        objFilePos = pos;
    } else if( where == SEEK_CUR ) {
        objFilePos += pos;
    } else {
        objFilePos = objFileLen - (unsigned long)pos;
    }
    return( 0 );
}

void CloseObjFile( void )
/***********************/
{
    if( ObjFileName != NULL )
        MemFree( ObjFileName );
    if( objFileBuf != NULL ) {
        MemFree( objFileBuf );
    }
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
    HandleToSectionTable = HashTableCreate( HANDLE_TO_SECTION_TABLE_SIZE, HASH_HANDLE );
    if( HandleToSectionTable != NULL ) {
        HandleToLabelListTable = HashTableCreate( HANDLE_TO_LIST_TABLE_SIZE, HASH_HANDLE );
        if( HandleToLabelListTable != NULL ) {
            HandleToRefListTable = HashTableCreate( HANDLE_TO_LIST_TABLE_SIZE, HASH_HANDLE );
            if( HandleToRefListTable != NULL ) {
                SymbolToLabelTable = HashTableCreate( SYMBOL_TO_LABEL_TABLE_SIZE, HASH_HANDLE );
                if( SymbolToLabelTable != NULL ) {
                    NameRecognitionTable = HashTableCreate( RECOGNITION_TABLE_SIZE, HASH_STRING_IGNORECASE );
                    if( NameRecognitionTable == NULL ) {
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
    int             i;
    return_val      error;
    hash_entry_data key_entry;

    error = createHashTables();
    if( error == RC_OKAY ) {
        for( i = 0; i < NUM_ELEMENTS( RecognizedName ); i++ ) {
            key_entry.key.u.string = RecognizedName[i].name;
            key_entry.data.u.sec_type = RecognizedName[i].type;
            HashTableInsert( NameRecognitionTable, &key_entry );
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
    ORLSetFuncs( orl_cli_funcs, objRead, objSeek, MemAlloc, MemFree );

    ORLHnd = ORLInit( &orl_cli_funcs );
    if( ORLHnd != ORL_NULL_HANDLE ) {
        type = ORLFileIdentify( ORLHnd, NULL );
        if( type == ORL_UNRECOGNIZED_FORMAT ) {
            PrintErrorMsg( RC_OKAY, WHERE_NOT_COFF_ELF );
            return( RC_ERROR );
        }
        ObjFileHnd = ORLFileInit( ORLHnd, NULL, type );
        if( ObjFileHnd != ORL_NULL_HANDLE ) {
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
                if( (Options & METAWARE_COMPATIBLE) == 0 ) {
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
    section_ptr         section;

    // list for references to external functions, etc.
    error = createLabelList( 0 );
    if( error == RC_OKAY ) {
        o_error = ORLFileScan( ObjFileHnd, NULL, sectionInit );
        if( o_error == ORL_OKAY && symbolTable != ORL_NULL_HANDLE ) {
            o_error = DealWithSymbolSection( symbolTable );
            if( o_error == ORL_OKAY && dynSymTable != ORL_NULL_HANDLE ) {
                o_error = DealWithSymbolSection( dynSymTable );
            }
            if( o_error == ORL_OKAY ) {
                while( (section = relocSections.first) != NULL ) {
                    o_error = DealWithRelocSection( section->shnd );
                    if( o_error != ORL_OKAY ) {
                        if( o_error == ORL_OUT_OF_MEMORY ) {
                            return( RC_OUT_OF_MEMORY );
                        } else {
                            return( RC_ERROR );
                        }
                    }
                    relocSections.first = section->next;
                    MemFree( section );
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
    ChangePrintDest( stderr );
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

return_val Init( void )
{
    return_val          error;
    int                 i;
    const char          *name;
    hash_entry_data     key_entry;

    error = RC_OKAY;

    OutputDest = stdout;
    ChangePrintDest( OutputDest );

    relocSections.first = NULL;
    relocSections.last = NULL;

    MemOpen();
    if( !MsgInit() ) {
        // MsgInit does its own error message printing
        return( RC_ERROR );
    }

    error = HandleArgs();
    if( error != RC_OKAY ) {
        return( error );
    }

    error = openFiles();
    if( error != RC_OKAY ) {
        return( error );
    }
    initGlobals();
    error = initHashTables();
    if( error != RC_OKAY ) {
        PrintErrorMsg( error, WHERE_INIT_HASH_TABLES );
        return( error );
    }
    error = initServicesUsed();
    if( error != RC_OKAY ) {
        // initServicesUsed does its own error message printing
        return( error );
    }
    error = initSectionTables();
    if( error != RC_OKAY ) {
        PrintErrorMsg( error, WHERE_CREATE_SEC_TABLES );
        return( error );
    }
    if( Options & PRINT_PUBLICS ) {
        CreatePublicsArray();
    }
    if( IsMasmOutput() ) {
        CommentString = MASM_COMMENT_STRING;
    }
    if( IsIntelx86() ) {
        SkipRefTable = HashTableCreate( RECOGNITION_TABLE_SIZE, HASH_STRING_IGNORECASE );
        if( SkipRefTable != NULL ) {
            for( i = 0; i < NUM_ELEMENTS( intelSkipRefList ); i++ ) {
                if( (name = intelSkipRefList[i]) == NULL )
                    continue;
                key_entry.key.u.string = name;
                key_entry.data.u.string = name;
                error = HashTableInsert( SkipRefTable, &key_entry );
                if( error != RC_OKAY ) {
                    break;
                }
            }
        }
    }
    if( LabelChar == 0 ) {
        if( IsMasmOutput() ) {
            LabelChar = 'L';
        } else {
            LabelChar = 'X';
        }
    }
    return( error );
}
