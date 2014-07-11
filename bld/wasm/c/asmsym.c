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
* Description:  Assembler symbol table management.
*
****************************************************************************/


#include "asmglob.h"

#include "asmalloc.h"

#ifdef __USE_BSD
#define stricmp strcasecmp
#endif

#if defined( _STANDALONE_ )

#include "directiv.h"
#include "queues.h"
#include "hash.h"
#include "myassert.h"

static struct asm_sym   *sym_table[ HASH_TABLE_SIZE ] = { NULL };
/* initialize the whole table to null pointers */
static unsigned         AsmSymCount;    /* Number of symbols in table */

static char             dots[] = " . . . . . . . . . . . . . . . .";

#ifdef DEBUG_OUT
void    DumpASym( void );   /* Forward declaration */
#endif
extern asm_sym          *FindStructureMember( asm_sym *symbol , const char *name );

#else

static struct asm_sym   *AsmSymHead;

static unsigned short CvtTable[] = {
    MT_BYTE,   // INT1
    MT_WORD,   // INT2
    MT_DWORD,  // INT4
    MT_FWORD,  // INT6
    MT_QWORD,  // INT8
    MT_DWORD,  // FLOAT4
    MT_QWORD,  // FLOAT8
    MT_TBYTE,  // FLOAT10
    MT_NEAR,   // NEAR2
    MT_NEAR,   // NEAR4
    MT_FAR,    // FAR2
    MT_FAR     // FAR4
};

#endif

static char *InitAsmSym( struct asm_sym *sym, const char *name )
/**************************************************************/
{
#if !defined( _STANDALONE_ )
    void    *handle;
#endif

    sym->name = AsmAlloc( strlen( name ) + 1 );
    if( sym->name != NULL ) {
        strcpy( sym->name, name );
        sym->next = NULL;
        sym->fixup = NULL;
#if defined( _STANDALONE_ )
        sym->segment = NULL;
        sym->offset = 0;
        sym->public = FALSE;
        sym->referenced = FALSE;
        sym->langtype = LANG_NONE;
        sym->first_size = 0;
        sym->first_length = 0;
        sym->total_size = 0;
        sym->total_length = 0;
        sym->count = 0;
        sym->mangler = NULL;
        sym->state = SYM_UNDEFINED;
        sym->mem_type = MT_EMPTY;
#else
        sym->addr = 0;
        handle = AsmQuerySymbol( sym->name );
        sym->state = AsmQueryState( handle );
        if( sym->state == SYM_UNDEFINED ) {
            sym->mem_type = MT_EMPTY;
        } else {
            sym->mem_type = CvtTable[ AsmQueryType( handle ) ];
        }
#endif
    }
    return( sym->name );
}

static struct asm_sym *AllocASym( const char *name )
/**************************************************/
{
    struct asm_sym      *sym;

#if defined( _STANDALONE_ )
    sym = AsmAlloc( sizeof( dir_node ) );
#else
    sym = AsmAlloc( sizeof( struct asm_sym ) );
#endif
    if( sym != NULL ) {
        if( InitAsmSym( sym, name ) == NULL ) {
            AsmFree( sym );
            return( NULL );
        }
#if defined( _STANDALONE_ )
        ((dir_node *)sym)->next = NULL;
        ((dir_node *)sym)->prev = NULL;
        ((dir_node *)sym)->line_num = 0;
        ((dir_node *)sym)->e.seginfo = NULL;
#endif
    }
    return sym;
}

static struct asm_sym **AsmFind( const char *name )
/*************************************************/
/* find a symbol in the symbol table, return NULL if not found */
{
    struct asm_sym      **sym;

#if defined( _STANDALONE_ )
    sym = &sym_table[ hashpjw( name ) ];
#else
    sym = &AsmSymHead;
#endif
    for( ; *sym; sym = &((*sym)->next) ) {
#if defined( _STANDALONE_ )
        if( Options.mode & MODE_IDEAL ) {
            if( strcmp( name, (*sym)->name ) == 0 ) {
                break;
            }
        } else {
            if( stricmp( name, (*sym)->name ) == 0 ) {
                break;
            }
        }
#else
        if( stricmp( name, (*sym)->name ) == 0 ) {
            break;
        }
#endif
    }
    return( sym );
}

#if defined( _STANDALONE_ )
static struct asm_sym *FindLocalLabel( const char *name )
/*******************************************************/
{
    label_list  *curr;

    for( curr = CurrProc->e.procinfo->labellist; curr != NULL; curr = curr->next ) {
        if( strcmp( curr->sym->name, name ) == 0 ) {
            return( curr->sym );
        }
    }
    return( NULL );
}

static bool AddLocalLabel( asm_sym *sym )
/***************************************/
{
    label_list  *label;
    label_list  *curr;
    proc_info   *info;

    if( ( sym->state != SYM_UNDEFINED ) && ( Parse_Pass == PASS_1 ) ) {
        AsmErr( SYMBOL_PREVIOUSLY_DEFINED, sym->name );
        return( RC_ERROR );
    } else {
        sym->state = SYM_INTERNAL;
        sym->mem_type = MT_SHORT;
    }
    info = CurrProc->e.procinfo;
    label = AsmAlloc( sizeof( label_list ) );
    label->label = NULL;
    label->size = 0;
    label->replace = NULL;
    label->factor = 0;
    label->next = NULL;
    label->sym = sym;
    label->is_register = 0;
    if( info->labellist == NULL ) {
        info->labellist = label;
    } else {
        for( curr = info->labellist;; curr = curr->next ) {
            if( curr->next == NULL ) {
                break;
            }
        }
        curr->next = label;
    }
    return( RC_OK );
}
#endif

struct asm_sym *AsmLookup( const char *name )
/*******************************************/
{
    struct asm_sym      **sym_ptr;
    struct asm_sym      *sym;
#if defined( _STANDALONE_ )
    struct asm_sym      *structure;
#endif
    if( strlen( name ) > MAX_ID_LEN ) {
        AsmError( LABEL_TOO_LONG );
        return NULL;
    }
#if defined( _STANDALONE_ )
    if( Options.mode & MODE_IDEAL ) {
        if( Options.locals_len ) {
            if( memcmp( name, Options.locals_prefix, Options.locals_len ) == 0
                && name[Options.locals_len] != '\0' ) {
                if( CurrProc == NULL ) {
                    AsmError( LOCAL_LABEL_OUTSIDE_PROC );
                    return( NULL );
                }
                sym = FindLocalLabel( name );
                if( sym == NULL ) {
                    sym = AllocASym( name );
                    if( sym != NULL) {
                        if( AddLocalLabel( sym ) ) {
                            return( NULL );
                        }
                    }
                }
                return( sym );
            }
        }
        if( Definition.struct_depth != 0 ) {
            structure = &Definition.curr_struct->sym;
            sym = FindStructureMember( structure, name );
            if( sym == NULL )
                return( AllocASym( name ) );
            return sym;
        }
    }
#endif
    sym_ptr = AsmFind( name );
    sym = *sym_ptr;
    if( sym != NULL ) {
#if defined( _STANDALONE_ )
        /* current address operator */
        if( IS_SYM_COUNTER( name ) )
            GetSymInfo( sym );
#endif
        return( sym );
    }

    sym = AllocASym( name );
    if( sym != NULL ) {
        sym->next = *sym_ptr;
        *sym_ptr = sym;

#if defined( _STANDALONE_ )
        ++AsmSymCount;
        if( IS_SYM_COUNTER( name ) ) {
            GetSymInfo( sym );
            sym->state = SYM_INTERNAL;
            sym->mem_type = MT_NEAR;
            return( sym );
        }
#else
        sym->addr = AsmCodeAddress;
#endif
    } else {
        AsmError( NO_MEMORY );
    }
    return( sym );
}

void FreeASym( struct asm_sym *sym )
/*****************************************/
{
#if defined( _STANDALONE_ )
    struct asmfixup     *fixup;

    --AsmSymCount;
    for( ;; ) {
        fixup = sym->fixup;
        if( fixup == NULL )
            break;
        sym->fixup = fixup->next;
        AsmFree( fixup );
    }
#endif
    AsmFree( sym->name );
    AsmFree( sym );
}

#if defined( _STANDALONE_ )

int AsmChangeName( const char *old, const char *new )
/***************************************************/
{
    struct asm_sym      **sym_ptr;
    struct asm_sym      *sym;

    sym_ptr = AsmFind( old );
    if( *sym_ptr != NULL ) {
        sym = *sym_ptr;
        *sym_ptr = sym->next;
        AsmFree( sym->name );
        sym->name = AsmAlloc( strlen( new ) + 1 );
        strcpy( sym->name, new );

        sym_ptr = AsmFind( new );
        if( *sym_ptr != NULL )
            return( ERROR );

        sym->next = *sym_ptr;
        *sym_ptr = sym;
    }
    return( NOT_ERROR );
}

void AsmTakeOut( const char *name )
/*********************************/
{
    struct asm_sym      *sym;
    struct asm_sym      **sym_ptr;

    sym_ptr = AsmFind( name );
    if( *sym_ptr != NULL ) {
        /* found it -- so take it out */
        sym = *sym_ptr;
        *sym_ptr = sym->next;
        FreeInfo( (dir_node *)sym );
        FreeASym( sym );
    }
    return;
}

static struct asm_sym *AsmSymAdd( struct asm_sym *sym )
/*****************************************************/
{
    struct asm_sym  **location;

    location = AsmFind( sym->name );

    if( *location != NULL ) {
        /* we already have one */
        AsmError( SYMBOL_ALREADY_DEFINED );
        return( NULL );
    }

    sym->next = *location;
    *location = sym;
    ++AsmSymCount;
    return( sym );
}

struct asm_sym *AllocDSym( const char *name, int add_symbol )
/***********************************************************/
/* Create directive symbol and insert it into the symbol table */
{
    struct asm_sym      *new;

    new = AllocASym( name );
    if( new == NULL ) {
        AsmError( NO_MEMORY );
        return( NULL );
    }
    /* add it into the symbol table */
    if( add_symbol ) {
        return( AsmSymAdd( new ) );
    } else {
        return( new );
    }
}

struct asm_sym *AsmGetSymbol( const char *name )
/**********************************************/
{
    struct asm_sym  **sym_ptr, *structure;

#if defined( _STANDALONE_ )
    if( Options.mode & MODE_IDEAL ) {
        if( name[0] == '@' && name[1] == '@' ) {
            if( CurrProc == NULL ) {
                AsmError( LOCAL_LABEL_OUTSIDE_PROC );
                return( NULL );
            }
            return( FindLocalLabel( name ) );
        }
        if( Definition.struct_depth != 0 ) {
            structure = &Definition.curr_struct->sym;
            return( FindStructureMember( structure, name ) );
        }
    }
#endif
    sym_ptr = AsmFind( name );
#if defined( _STANDALONE_ )
    if( ( *sym_ptr != NULL ) && IS_SYM_COUNTER( name ) )
        GetSymInfo( *sym_ptr );
#endif
    return( *sym_ptr );
}
#endif

#if defined( _STANDALONE_ )
void AsmSymInit( void )
/*********************/
{
    AsmLookup( "$" );    // create "$" symbol for current segment counter
}
#endif

void AsmSymFini( void )
/*********************/
{
    struct asm_sym      *sym;
#if defined( _STANDALONE_ )
    dir_node            *dir;
    unsigned            i;

#if defined( DEBUG_OUT )
    DumpASym();
#endif

    FreeAllQueues();

    /* now free the symbol table */
    for( i = 0; i < HASH_TABLE_SIZE; i++ ) {
        struct asm_sym  *next;
        next = sym_table[i];
        for( ;; ) {
            sym = next;
            if( sym == NULL )
                break;
            dir = (dir_node *)sym;
            next = sym->next;
            FreeInfo( dir );
            FreeASym( sym );
        }
    }
    myassert( AsmSymCount == 0 );

#else
    struct asmfixup     *fixup;

    for( ;; ) {
        sym = AsmSymHead;
        if( sym == NULL )
            break;
        AsmSymHead = sym->next;
        FreeASym( sym );
    }
    for( ;; ) {
        fixup = FixupHead;
        if( fixup == NULL )
            break;
        FixupHead = fixup->next;
        AsmFree( fixup );
    }
#endif
}

#if defined( _STANDALONE_ )

static int compare_fn( const void *p1, const void *p2 )
/*****************************************************/
{
    const struct asm_sym *sym1 = *(const struct asm_sym **)p1;
    const struct asm_sym *sym2 = *(const struct asm_sym **)p2;

    return( strcmp( sym1->name, sym2->name ) );
}

static struct asm_sym **SortAsmSyms( void )
/*****************************************/
{
    struct asm_sym      **syms;
    struct asm_sym      *sym;
    unsigned            i, j;

    syms = AsmAlloc( AsmSymCount * sizeof( struct asm_sym * ) );
    if( syms ) {
        /* copy symbols to table */
        for( i = j = 0; i < HASH_TABLE_SIZE; i++ ) {
            struct asm_sym  *next;

            next = sym_table[i];
            for( ;; ) {
                sym = next;
                if( sym == NULL )
                    break;
                next = sym->next;
                syms[j++] = sym;
            }
        }
        /* sort 'em */
        qsort( syms, AsmSymCount, sizeof( struct asm_sym * ), compare_fn );
    }
    return( syms );
}

const char *get_seg_align( seg_info *seg )
/****************************************/
{
    switch( seg->align ) {
    case ALIGN_ABS:
    case ALIGN_BYTE:
        return( "Byte " );
    case ALIGN_WORD:
        return( "Word " );
    case ALIGN_DWORD:
        return( "DWord" );
    case ALIGN_PARA:
        return( "Para " );
    case ALIGN_PAGE:
        return( "Page " );
    case ALIGN_4KPAGE:
        return( "4K   " );
    default:
        return( "?    " );
    }
}

static const char *get_seg_combine( seg_info *seg )
/*************************************************/
{
    switch( seg->combine ) {
    case COMB_INVALID:
        return( "Private " );
    case COMB_STACK:
        return( "Stack   " );
    case COMB_ADDOFF:
        return( "Public  " );
    default:
        return( "?       " );
    }
}

static void log_segment( struct asm_sym *sym, struct asm_sym *group )
/*******************************************************************/
{
    if( sym->state == SYM_SEG ) {
        dir_node    *dir = (dir_node *)sym;
        seg_info    *seg = dir->e.seginfo;

        if( seg->group == group ) {
            LstMsg( "%s %s        ", sym->name, dots + strlen( sym->name ) + 1 );
            if( seg->use_32 ) {
                LstMsg( "32 Bit   %08lX ", seg->current_loc );
            } else {
                LstMsg( "16 Bit   %04lX     ", seg->current_loc );
            }
            LstMsg( "%s   %s", get_seg_align( seg ), get_seg_combine( seg ) );
            LstMsg( "'%s'\n", seg->class_name->name );
        }
    }
}

static void log_group( struct asm_sym **syms, struct asm_sym *sym )
/*****************************************************************/
{
    unsigned        i;

    if( sym->state == SYM_GRP ) {
        LstMsg( "%s %s        ", sym->name, dots + strlen( sym->name ) + 1 );
        LstMsg( "GROUP\n" );
        for( i = 0; i < AsmSymCount; ++i ) {
            log_segment( syms[i], sym );
        }
    }
}

static const char *get_sym_type( struct asm_sym *sym )
/****************************************************/
{
    switch( sym->mem_type ) {
    case MT_BYTE:
        return( "Byte   " );
    case MT_WORD:
        return( "Word   " );
    case MT_DWORD:
        return( "DWord  " );
    case MT_QWORD:
        return( "QWord  " );
    case MT_FWORD:
        return( "FWord  " );
    case MT_NEAR:
        return( "L Near " );
    case MT_FAR:
        return( "L Far  " );
    default:
        return( "?      " );
    }
}

static const char *get_proc_type( struct asm_sym *sym )
/*****************************************************/
{
    switch( sym->mem_type ) {
    case MT_NEAR:
        return( "P Near " );
    case MT_FAR:
        return( "P Far  " );
    default:
        return( "       " );
    }
}

static const char *get_sym_lang( struct asm_sym *sym )
/****************************************************/
{
    switch( sym->langtype ) {
    case LANG_C:
        return( "C" );
    case LANG_BASIC:
        return( "BASIC" );
    case LANG_FORTRAN:
        return( "FORTRAN" );
    case LANG_PASCAL:
        return( "PASCAL" );
    case LANG_WATCOM_C:
        return( "WATCOM_C" );
    case LANG_STDCALL:
        return( "STDCALL" );
    case LANG_SYSCALL:
        return( "SYSCALL" );
    default:
        return( "" );
    }
}

static const char *get_sym_seg_name( struct asm_sym *sym )
/********************************************************/
{
    if( sym->segment ) {
        return( sym->segment->name );
    } else {
        return( "No Seg" );
    }
}

static void log_symbol( struct asm_sym *sym )
/*******************************************/
{
    if( sym->state == SYM_CONST ) {
        dir_node    *dir = (dir_node *)sym;
        const_info  *cst = dir->e.constinfo;

        LstMsg( "%s %s        ", sym->name, dots + strlen( sym->name ) + 1 );
        if( cst->count && cst->data[0].class != TC_NUM ) {
            LstMsg( "Text     %s\n", cst->data[0].string_ptr );
        } else {
            LstMsg( "Number   %04Xh\n", cst->count ? cst->data[0].u.value : 0 );
        }
    } else if( sym->state == SYM_INTERNAL && !IS_SYM_COUNTER( sym->name ) ) {
        LstMsg( "%s %s        ", sym->name, dots + strlen( sym->name ) + 1 );
        LstMsg( "%s  %04X     ", get_sym_type( sym ), sym->offset );
        LstMsg( "%s\t", get_sym_seg_name( sym ) );
        if( sym->public ) {
            LstMsg( "Public " );
        }
        LstMsg( "%s", get_sym_lang( sym ) );
        LstMsg( "\n" );
    } else if( sym->state == SYM_EXTERNAL ) {
        LstMsg( "%s %s        ", sym->name, dots + strlen( sym->name ) + 1 );
        LstMsg( "%s  %04X     ", get_sym_type( sym ), sym->offset );
        LstMsg( "%s\t", get_sym_seg_name( sym ) );
        LstMsg( "External " );
        LstMsg( "%s", get_sym_lang( sym ) );
        LstMsg( "\n" );
    }
}

static void log_proc( struct asm_sym *sym )
/*****************************************/
{
    if( sym->state == SYM_PROC ) {
        LstMsg( "%s %s        ", sym->name, dots + strlen( sym->name ) + 1 );
        LstMsg( "%s  %08X ", get_proc_type( sym ), sym->offset );
        LstMsg( "%s\t", get_sym_seg_name( sym ) );
        LstMsg( "Length= %08X ", sym->total_size );
        if( sym->public ) {
            LstMsg( "Public " );
        } else {
            LstMsg( "Private " );
        }
        LstMsg( "%s", get_sym_lang( sym ) );
        LstMsg( "\n" );
    }
}

void WriteListing( void )
/***********************/
{
    struct asm_sym  **syms;
    unsigned        i;

    if( AsmFiles.file[LST] == NULL ) {
        return; // no point going through the motions if lst file isn't open
    }
    syms = SortAsmSyms();
    if( syms ) {
        /* first write out the segments */
        LstMsg( "\n\nSegments and Groups:\n\n" );
        LstMsg( "                N a m e                 Size" );
        LstMsg( "     Length   Align   Combine Class\n\n" );
        /* write out groups with associated segments */
        for( i = 0; i < AsmSymCount; ++i ) {
            log_group( syms, syms[i] );
        }
        /* write out remaining segments, outside any group */
        for( i = 0; i < AsmSymCount; ++i ) {
            log_segment( syms[i], NULL );
        }
        LstMsg( "\n" );

        /* next write out procedures and stuff */
        LstMsg( "\n\nProcedures:\n\n" );
        LstMsg( "                N a m e                 Type" );
        LstMsg( "     Value    Attr\n\n" );
        for( i = 0; i < AsmSymCount; ++i ) {
            log_proc( syms[i] );
        }
        LstMsg( "\n" );

        /* next write out symbols */
        LstMsg( "\n\nSymbols:\n\n" );
        LstMsg( "                N a m e                 Type" );
        LstMsg( "     Value    Attr\n\n" );
        for( i = 0; i < AsmSymCount; ++i ) {
            log_symbol( syms[i] );
        }
        LstMsg( "\n" );

        /* free the sorted symbols */
        AsmFree( syms );
    }
}

#if defined( DEBUG_OUT )

static void DumpSymbol( struct asm_sym *sym )
/*******************************************/
{
    dir_node    *dir;
    char        *type;
    char        value[512];
    const char  *langtype;
    char        *public;

    dir = (dir_node *)sym;
    *value = 0;
    switch( sym->state ) {
    case SYM_SEG:
        type = "SEGMENT";
//        dir->e.seginfo = AsmAlloc( sizeof( seg_info ) );
//        dir->e.seginfo->idx = 0;
//        dir->e.seginfo->grpidx = 0;
//        dir->e.seginfo->segrec = NULL;
        break;
    case SYM_GRP:
        type = "GROUP";
//        dir->e.grpinfo = AsmAlloc( sizeof( grp_info ) );
//        dir->e.grpinfo->idx = 0;
//        dir->e.grpinfo->seglist = NULL;
//        dir->e.grpinfo->numseg = 0;
        break;
    case SYM_EXTERNAL:
        type = "EXTERNAL";
//        dir->e.extinfo = AsmAlloc( sizeof( ext_info ) );
//        dir->e.extinfo->idx = 0;
//        dir->e.extinfo->use32 = Use32;
//        dir->e.extinfo->comm = 0;
//        dir->e.extinfo->global = 0;
        break;
//    case TAB_COMM:
//        sym->state = SYM_EXTERNAL;
//        dir->e.comminfo = AsmAlloc( sizeof( comm_info ) );
//        dir->e.comminfo->idx = 0;
//        dir->e.comminfo->use32 = Use32;
//        dir->e.comminfo->comm = 1;
//        break;
    case SYM_CONST:
        type = "CONSTANT";
//        dir->e.constinfo = AsmAlloc( sizeof( const_info ) );
//        dir->e.constinfo->data = NULL;
//        dir->e.constinfo->count = 0;
        break;
    case SYM_PROC:
        type = "PROCEDURE";
//        dir->e.procinfo = AsmAlloc( sizeof( proc_info ) );
//        dir->e.procinfo->regslist = NULL;
//        dir->e.procinfo->paralist = NULL;
//        dir->e.procinfo->locallist = NULL;
        break;
    case SYM_MACRO:
        type = "MACRO";
//        dir->e.macroinfo = AsmAlloc( sizeof( macro_info ) );
//        dir->e.macroinfo->parmlist = NULL;
//        dir->e.macroinfo->data = NULL;
//        dir->e.macroinfo->filename = NULL;
        break;
    case SYM_CLASS_LNAME:
        type = "CLASS";
//        dir->e.lnameinfo = AsmAlloc( sizeof( lname_info ) );
//        dir->e.lnameinfo->idx = 0;
        break;
    case SYM_STRUCT:
        type = "STRUCTURE";
//        dir->e.structinfo = AsmAlloc( sizeof( struct_info ) );
//        dir->e.structinfo->size = 0;
//        dir->e.structinfo->alignment = 0;
//        dir->e.structinfo->head = NULL;
//        dir->e.structinfo->tail = NULL;
        break;
    case SYM_STRUCT_FIELD:
        type = "STRUCTURE FIELD";
        break;
    case SYM_LIB:
        type = "LIBRARY";
        break;
    case SYM_UNDEFINED:
        type = "UNDEFINED";
        break;
    case SYM_INTERNAL:
        type = "INTERNAL";
        break;
    default:
        type = "UNKNOWN";
        break;
    }
    if( sym->public ) {
        public = "PUBLIC ";
    } else {
        public = "";
    }
    langtype = get_sym_lang( sym );
    DoDebugMsg( "%-30s\t%s\t%s%s\t%8X\t%s\n", sym->name, type, public, langtype, sym->offset, value );
}

void DumpASym( void )
/*******************/
{
    struct asm_sym      *sym;
    unsigned            i;

    LstMsg( "\n" );
    for( i = 0; i < HASH_TABLE_SIZE; i++ ) {
        struct asm_sym  *next;
        next = sym_table[i];
        for( ;; ) {
            sym = next;
            if( sym == NULL )
                break;
            next = sym->next;
            DumpSymbol( sym );
        }
    }
}
#endif

#endif
