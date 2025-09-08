/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Processing of assembly directives.
*
****************************************************************************/


#include "asmglob.h"
#include <ctype.h>
#if defined( __WATCOMC__ ) && defined( DEBUG_OUT )
    #include <malloc.h>
#endif
#include "roundmac.h"
#include "asmalloc.h"
#include "directiv.h"
#include "queues.h"
#include "asmexpnd.h"
#include "asmfixup.h"
#include "mangle.h"
#include "asmlabel.h"
#include "asminput.h"
#include "asmeval.h"
#include "myassert.h"
#include "asmdefs.h"
#include "pathgrp2.h"
#include "pcobj.h"

#include "clibext.h"


#define BIT16           0
#define BIT32           1

#define MAGIC_FLAT_GROUP        ModuleInfo.flat_grp

#define INIT_ALIGN      0x1
#define INIT_COMBINE    0x2
#define INIT_USE        0x4
#define INIT_CLASS      0x8
#define INIT_MEMORY     0x10
#define INIT_STACK      0x20

#define TOK_INVALID     -1

typedef struct {
    const char      *string;    // the token string
    uint            value;      // value connected to this token
    uint            init;       // explained in direct.c ( look at SegDef() )
} typeinfo;

typedef struct {
    int             param_number;
    int             unused_stack_space;
    bool            on_stack;
} paramsinfo;

typedef struct {
    asm_sym_handle  symbol;     // segment or group that is to be associated with the register
    bool            error;      // the register is assumed to ERROR
    bool            flat;       // the register is assumed to FLAT
} assume_info;

typedef enum {
    SIM_CODE = 0,           // .CODE or CODESEG
    SIM_STACK,              // .STACK or STACK
    SIM_DATA,               // .DATA or DATASEG
    SIM_DATA_UN,            // .DATA? or UDATASEG
    SIM_FARDATA,            // .FARDATA or FARDATA
    SIM_FARDATA_UN,         // .FARDATA? or UFARDATA
    SIM_CONST,              // .CONST or CONST
    SIM_NONE,
} sim_seg;
#define SIM_LAST    SIM_NONE

typedef struct {
    sim_seg     seg;                    // segment id
    char        close[MAX_LINE];        // closing line for this segment
    int_16      stack_size;             // size of stack segment
} last_seg_info;        // information about last opened simplified segment

enum {
    #define pick(token,string,value,init_val)   token
    #include "directd.h"
    #undef pick
};

static typeinfo TypeInfo[] = {
    #define pick(token,string,value,init_val)   { string, value, init_val }
    #include "directd.h"
    #undef pick
};

#define LOCAL_DEFAULT_SIZE      2
#define DEFAULT_STACK_SIZE      1024

#define ARGUMENT_STRING          "[bp+"
#define ARGUMENT_STRING_32       "[ebp+"
#define LOCAL_STRING             "[bp-"
#define LOCAL_STRING_32          "[ebp-"
#define IDEAL_ARGUMENT_STRING    "bp+"
#define IDEAL_ARGUMENT_STRING_32 "ebp+"
#define IDEAL_LOCAL_STRING       "bp-"
#define IDEAL_LOCAL_STRING_32    "ebp-"

extern bool             write_to_file;  // write if there is no error
extern unsigned         BufSize;
extern bool             DefineProc;     // true if the definition of procedure
                                        // has not ended
extern asm_sym_handle   SegOverride;
extern asmfixup         *ModendFixup;   // start address fixup

bool                    EndDirectiveFound = false;
bool                    EndDirectiveProc = false;

seg_list                *CurrSeg;       // points to stack of opened segments

bool                    in_prologue;

static assume_info      AssumeTable[ASSUME_LAST];

symbol_queue            Tables[TAB_SIZE];// tables of definitions
module_info             ModuleInfo;

static seg_list         *ProcStack = NULL;

static const char * const StartupDosNear[] = {
    " mov     dx,DGROUP",
    " mov     ds,dx",
    " mov     bx,ss",
    " sub     bx,dx",
    " shl     bx,1",
    " shl     bx,1",
    " shl     bx,1",
    " shl     bx,1",
    " cli     ",
    " mov     ss,dx",
    " add     sp,bx",
    " sti     ",
    NULL
};
static const char * const StartupDosFar[] = {
    " mov     dx,DGROUP",
    " mov     ds,dx",
    NULL
};
static const char * const ExitOS2[] = { /* mov al,retval  followed by: */
    " xor ah,ah",
    " push 01h",
    " push ax",
    " call far DOSEXIT",
    NULL
};
static const char * const ExitDos[] = { /* mov al, retval  followed by: */
    " mov     ah,4ch",
    " int     21h",
    NULL
};
static const char * const RetVal = " mov    al,";

static const char * const StartAddr = "`symbol_reserved_for_start_address`";
static bool StartupDirectiveFound = false;

/*
 * Code generated by simplified segment definitions
 */
static last_seg_info    lastseg;        // last opened simplified segment

static char         code_segment_name[MAX_LINE_LEN];

static asm_tok      const_CodeSize =  { TC_NUM, NULL,              0 };
static asm_tok      const_DataSize =  { TC_NUM, NULL,              0 };
static asm_tok      const_Model =     { TC_NUM, NULL,              0 };
static asm_tok      const_Interface = { TC_NUM, NULL,              0 };
static asm_tok      const_data =      { TC_ID,  NULL,              0 };
static asm_tok      const_code =      { TC_ID,  code_segment_name, 0 };

static const_info   info_CodeSize =  { &const_CodeSize,  1, true, false, false };
static const_info   info_DataSize =  { &const_DataSize,  1, true, false, false };
static const_info   info_Model =     { &const_Model,     1, true, false, false };
static const_info   info_Interface = { &const_Interface, 1, true, false, false };
static const_info   info_data =      { &const_data,      1, true, false, false };
static const_info   info_code =      { &const_code,      1, true, false, false };

static const char *get_sim_name( sim_seg seg, const char *name )
{
    if( name != NULL ) {
        return( name );
    } else {
        switch( seg ) {
        case SIM_CODE:              // .CODE or CODESEG
            return( "_TEXT" );
        case SIM_STACK:             // .STACK or STACK
            return( "STACK" );
        case SIM_DATA:              // .DATA or DATASEG
            return( "_DATA" );
        case SIM_DATA_UN:           // .DATA? or UDATASEG
            return( "_BSS" );
        case SIM_FARDATA:           // .FARDATA or FARDATA
            return( "FAR_DATA" );
        case SIM_FARDATA_UN:        // .FARDATA? or UFARDATA
            return( "FAR_BSS" );
        case SIM_CONST:             // .CONST or CONST
            return( "CONST" );
        default:
            return( "" );
        }
    }
}

static const char *get_sim_class( sim_seg seg )
{
    switch( seg ) {
    case SIM_CODE:                  // .CODE or CODESEG
        if( Options.code_class != NULL )
            return( Options.code_class );
        return( "CODE" );
    case SIM_STACK:                 // .STACK or STACK
        return( "STACK" );
    case SIM_DATA:                  // .DATA or DATASEG
        return( "DATA" );
    case SIM_DATA_UN:               // .DATA? or UDATASEG
        return( "BSS" );
    case SIM_FARDATA:               // .FARDATA or FARDATA
        return( "FAR_DATA" );
    case SIM_FARDATA_UN:            // .FARDATA? or UFARDATA
        return( "FAR_BSS" );
    case SIM_CONST:                 // .CONST or CONST
        return( "CONST" );
    default:
        return( "" );
    }
}

static const char *get_sim_align( sim_seg seg, bool use32 )
{
    if( use32 ) {
        return( "DWORD USE32" );
    } else {
        switch( seg ) {
        case SIM_CODE:              // .CODE or CODESEG
        case SIM_DATA:              // .DATA or DATASEG
        case SIM_DATA_UN:           // .DATA? or UDATASEG
        case SIM_CONST:             // .CONST or CONST
            return( "WORD" );
        case SIM_STACK:             // .STACK or STACK
        case SIM_FARDATA:           // .FARDATA or FARDATA
        case SIM_FARDATA_UN:        // .FARDATA? or UFARDATA
            return( "PARA" );
        default:
            return( "" );
        }
    }
}

static const char *get_sim_access( sim_seg seg )
{
    switch( seg ) {
    case SIM_CODE:                  // .CODE or CODESEG
    case SIM_DATA:                  // .DATA or DATASEG
    case SIM_DATA_UN:               // .DATA? or UDATASEG
    case SIM_CONST:                 // .CONST or CONST
        return( "PUBLIC" );
    case SIM_FARDATA:               // .FARDATA or FARDATA
    case SIM_FARDATA_UN:            // .FARDATA? or UFARDATA
        return( "PRIVATE" );
    case SIM_STACK:                 // .STACK or STACK
        return( "STACK" );
    default:
        return( "" );
    }
}

static bool AddPredefinedConstant( char *name, const_info *info )
/***************************************************************/
{
    dir_node_handle dir;

    dir = (dir_node_handle)AsmGetSymbol( name );
    if( dir == NULL ) {
        dir = dir_insert( name, TAB_CONST );
        if( dir == NULL ) {
            return( RC_ERROR );
        }
    } else if( dir->sym.state == SYM_UNDEFINED ) {
        dir_change( dir, TAB_CONST );
    } else if( dir->sym.state != SYM_CONST ) {
        AsmErr( LABEL_ALREADY_DEFINED, dir->sym.name );
        return( RC_ERROR );
    }
    if( !dir->e.constinfo->predef ) {
        FreeInfo( dir );
        dir->e.constinfo = info;
    }
    return( RC_OK );
}

static bool get_watcom_argument( int size, paramsinfo *params )
/**************************************************************
 * returns status for a parameter using the watcom register parm passing
 * conventions ( A D B C )
 */
{
    if( params->param_number > 3 ) {
        params->on_stack = true;
    } else {
        switch( size ) {
        case 1:
        case 2:
            break;
        case 4:
            if( !Use32 ) {
                if( params->param_number > 2 ) {
                    params->on_stack = true;
                    break;
                }
                params->param_number++;
            }
            break;
        case 10:
            AsmErr( TBYTE_NOT_SUPPORTED );
            return( RC_ERROR );
        case 6:
        case 8:
            if( Use32 ) {
                if( params->param_number > 2 ) {
                    params->on_stack = true;
                    break;
                }
                params->param_number++;
                break;
            }
        default:
            /*
             * something wierd
             */
            AsmError( STRANGE_PARM_TYPE );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

static bool get_watcom_argument_string( char *buffer, int size, paramsinfo *params )
/***********************************************************************************
 * get the register for params 0 to 3,
 * using the watcom register parm passing conventions ( A D B C )
 */
{
    if( params->param_number > 3 ) {
        params->on_stack = true;
    } else {
        switch( size ) {
        case 1:
            sprintf( buffer, "%s", regs[A_BYTE][params->param_number] );
            break;
        case 2:
            sprintf( buffer, "%s", regs[A_WORD][params->param_number] );
            break;
        case 4:
            if( Use32 ) {
                sprintf( buffer, "%s", regs[A_DWORD][params->param_number] );
            } else {
                if( params->param_number > 2 ) {
                    params->on_stack = true;
                    break;
                }
                sprintf( buffer, " %s %s", regs[A_WORD][params->param_number], regs[A_WORD][params->param_number + 1] );
                params->param_number++;
            }
            break;
        case 10:
            AsmErr( TBYTE_NOT_SUPPORTED );
            return( RC_ERROR );
        case 6:
        case 8:
            if( Use32 ) {
                if( params->param_number > 2 ) {
                    params->on_stack = true;
                    break;
                }
                if( size == 6 ) {
                    sprintf( buffer, " %s %s", regs[A_DWORD][params->param_number], regs[A_WORD][params->param_number + 1] );
                } else {
                    sprintf( buffer, " %s %s", regs[A_DWORD][params->param_number], regs[A_DWORD][params->param_number + 1] );
                }
                params->param_number++;
                break;
            }
            /* fall down */
        default:
            /*
             * something wierd
             */
            AsmError( STRANGE_PARM_TYPE );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

#ifdef DEBUG_OUT
void heap( char *func )
/**********************
 * for debugging only
 */
{
#ifdef __WATCOMC__
    switch(_heapchk()) {
    case _HEAPBADNODE:
    case _HEAPBADBEGIN:
    DebugMsg(("Function : %s - ", func ));
        DebugMsg(("ERROR - heap is damaged\n"));
        exit( EXIT_ERROR );
        break;
    default:
        break;
    }
#endif
}
#endif

static bool SetAssumeCSCurrSeg( void )
/************************************/
{
    assume_info     *info;

    info = &(AssumeTable[ASSUME_CS]);
    if( CurrSeg == NULL ) {
        info->symbol = NULL;
        info->flat = false;
        info->error = true;
    } else {
        if( CurrSeg->seg->e.seginfo->group != NULL ) {
            info->symbol = GetGrp( &CurrSeg->seg->sym );
        } else {
            info->symbol = &CurrSeg->seg->sym;
        }
        info->flat = false;
        info->error = false;
    }
    return( RC_OK );
}

void push( void *stk, void *elt )
/*******************************/
{
    void        **stack = stk;
    stacknode   *node;

    node = AsmAlloc( sizeof( stacknode ));
    node->next = *stack;
    node->elt = elt;
    *stack = node;
}

void *pop( void *stk )
/********************/
{
    void        **stack = stk;
    stacknode   *node;
    void        *elt;

    node = (stacknode *)(*stack);
    *stack = node->next;
    elt = node->elt;
    AsmFree( node );
    return( elt );
}

static bool push_seg( dir_node_handle seg )
/******************************************
 * Push a segment into the current segment stack
 */
{
    seg_list    *curr;

    for( curr = CurrSeg; curr != NULL; curr = curr->next ) {
        if( curr->seg == seg ) {
            AsmError( BLOCK_NESTING_ERROR );
            return( RC_ERROR );
        }
    }
    push( &CurrSeg, seg );
    SetAssumeCSCurrSeg();
    return( RC_OK );
}

static dir_node_handle pop_seg( void )
/*************************************
 * Pop a segment out of the current segment stack
 */
{
    dir_node_handle seg;

    /**/myassert( CurrSeg != NULL );
    seg = pop( &CurrSeg );
    SetAssumeCSCurrSeg();
    return( seg );
}

static void push_proc( dir_node_handle proc )
/*******************************************/
{
    push( &ProcStack, proc );
    return;
}

static dir_node_handle pop_proc( void )
/*************************************/
{
    if( ProcStack == NULL )
        return( NULL );
    return( (dir_node_handle)pop( &ProcStack ) );
}

static void dir_add( dir_node_handle new, int tab )
/*************************************************/
{
    /*
     * note: this is only for those above which do NOT return right away
     * put the new entry into the queue for its type of symbol
     *
     * add to the tail of linked list
     */
    new->next = NULL;
    new->prev = Tables[tab].tail;
    if( Tables[tab].head == NULL ) {
        Tables[tab].head = new;
    } else {
        Tables[tab].tail->next = new;
    }
    Tables[tab].tail = new;
}

void dir_init( dir_node_handle dir, int tab )
/********************************************
 * Change node and insert it into the table specified by tab
 */
{
    dir->line_num = LineNumber;
    dir->next = NULL;
    dir->prev = NULL;

    switch( tab ) {
    case TAB_SEG:
        dir->sym.state = SYM_SEG;
        dir->e.seginfo = AsmAlloc( sizeof( seg_info ) );
        dir->e.seginfo->idx = 0;
        dir->e.seginfo->group = NULL;
        break;
    case TAB_GRP:
        dir->sym.state = SYM_GRP;
        dir->e.grpinfo = AsmAlloc( sizeof( grp_info ) );
        dir->e.grpinfo->idx = 0;
        dir->e.grpinfo->seglist = NULL;
        dir->e.grpinfo->numseg = 0;
        break;
    case TAB_EXT:
    case TAB_FPPATCH:
        dir->sym.state = SYM_EXTERNAL;
        dir->e.extinfo = AsmAlloc( sizeof( ext_info ) );
        dir->e.extinfo->idx = 0;
        dir->e.extinfo->use32 = Use32;
        dir->e.extinfo->comm = false;
        dir->e.extinfo->global = false;
        dir->e.extinfo->comm_size = 0;
        dir->e.extinfo->comm_distance = 0;
        break;
    case TAB_CONST:
        dir->sym.state = SYM_CONST;
        dir->sym.segment = NULL;
        dir->sym.offset = 0;
        dir->e.constinfo = AsmAlloc( sizeof( const_info ) );
        dir->e.constinfo->tokens = NULL;
        dir->e.constinfo->count = 0;
        dir->e.constinfo->predef = false;
        return;
    case TAB_PROC:
        dir->sym.state = SYM_PROC;
        dir->e.procinfo = AsmAlloc( sizeof( proc_info ) );
        dir->e.procinfo->regslist = NULL;
        dir->e.procinfo->params.head = NULL;
        dir->e.procinfo->params.tail = NULL;
        dir->e.procinfo->locals.head = NULL;
        dir->e.procinfo->locals.tail = NULL;
        dir->e.procinfo->labels.head = NULL;
        dir->e.procinfo->labels.tail = NULL;
        break;
    case TAB_MACRO:
        dir->sym.state = SYM_MACRO;
        dir->e.macroinfo = AsmAlloc( sizeof( macro_info ) );
        dir->e.macroinfo->params.head = NULL;
        dir->e.macroinfo->params.tail = NULL;
        dir->e.macroinfo->labels.head = NULL;
        dir->e.macroinfo->labels.tail = NULL;
        dir->e.macroinfo->lines.head = NULL;
        dir->e.macroinfo->lines.tail = NULL;
        dir->e.macroinfo->srcfile = NULL;
        break;
    case TAB_CLASS_LNAME:
        dir->sym.state = SYM_CLASS_LNAME;
        dir->e.lnameinfo = AsmAlloc( sizeof( lname_info ) );
        dir->e.lnameinfo->idx = 0;
        break;
    case TAB_STRUCT:
        dir->sym.state = SYM_STRUCT;
        dir->e.structinfo = AsmAlloc( sizeof( struct_info ) );
        dir->e.structinfo->size = 0;
        dir->e.structinfo->alignment = 0;
        dir->e.structinfo->fields.head = NULL;
        dir->e.structinfo->fields.tail = NULL;
        return;
    case TAB_LIB:
        break;
    default:
        // unknown table
        /**/myassert( 0 );
        break;
    }
    dir_add( dir, tab );
    return;
}

static void dir_move_to_tail( dir_node_handle dir, int tab )
/***********************************************************
 * move item which is already in the list to the end of list
 * expect the item is not last item that the list contains
 * at minimum two items
 */
{
    /*
     * already on tail, no action
     */
    if( dir->next == NULL )
        return;
    /*
     * first remove it from list
     */
    dir->next->prev = dir->prev;
    if( dir->prev == NULL ) {
        Tables[tab].head = dir->next;
    } else {
        dir->prev->next = dir->next;
    }
    /*
     * add to the tail of linked list
     */
    dir->next = NULL;
    dir->prev = Tables[tab].tail;
    dir->prev->next = dir;
    Tables[tab].tail = dir;
}

static int get_dir_tab( dir_node_handle dir )
{
    int         tab;

    switch( dir->sym.state ) {
    case SYM_EXTERNAL:
        tab = TAB_EXT;
        break;
    case SYM_SEG:
        tab = TAB_SEG;
        break;
    case SYM_GRP:
        tab = TAB_GRP;
        break;
    case SYM_PROC:
        tab = TAB_PROC;
        break;
    case SYM_MACRO:
        tab = TAB_MACRO;
        break;
    case SYM_STRUCT:
        tab = TAB_STRUCT;
        break;
    case SYM_CONST:
        tab = TAB_CONST;
        break;
    default:
        tab = -1;
        break;
    }
    return( tab );
}

static dir_node_handle dir_reset( dir_node_handle dir )
/*****************************************************/
{
    if( dir->prev != NULL
      && dir->next != NULL ) {
        dir->next->prev = dir->prev;
        dir->prev->next = dir->next;
    } else {
        int     tab;

        tab = get_dir_tab( dir );
        if( tab == -1 ) {
            return( NULL );
        }
        if( dir->next != NULL ) {
            dir->next->prev = NULL;
            Tables[tab].head = dir->next;
        } else if( dir->prev != NULL ) {
            dir->prev->next = NULL;
            Tables[tab].tail = dir->prev;
        } else {
            Tables[tab].head = NULL;
            Tables[tab].tail = NULL;
        }
    }
    return( dir->next );
}

void dir_to_sym( dir_node_handle dir )
/*************************************
 * Remove node type/info to be symbol only again
 */
{
    FreeInfo( dir );
    dir_reset( dir );
    dir->sym.state = SYM_UNDEFINED;
}

void dir_change( dir_node_handle dir, int tab )
/**********************************************
 * Change node type and insert it into the table specified by tab
 */
{
    FreeInfo( dir );
    dir_reset( dir );
    dir_init( dir, tab );
}

dir_node_handle dir_insert( const char *name, int tab )
/************************************************
 * Insert a node into the table specified by tab
 */
{
    dir_node_handle new;

    /**/myassert( name != NULL );

    /*
     * don't put class lnames into the symbol table - separate name space
     */
    new = (dir_node_handle)AllocDSym( name );
    if( new != NULL )
        dir_init( new, tab );
    return( new );
}

static void set_macro__at_data( char *name )
{
    const_data.string_ptr = name;
}

static void set_macro__at_code( const char *name )
{
    strcpy( code_segment_name, get_sim_name( SIM_CODE, name ) );
}

void FreeInfo( dir_node_handle dir )
/**********************************/
{
    int i;

    switch( dir->sym.state ) {
    case SYM_GRP:
        {
            seg_list    *segcurr;
            seg_list    *segnext;

            for( segcurr = dir->e.grpinfo->seglist; segcurr != NULL; segcurr = segnext ) {
                segnext = segcurr->next;
                AsmFree( segcurr );
            }
            AsmFree( dir->e.grpinfo );
        }
        break;
    case SYM_SEG:
        AsmFree( dir->e.seginfo );
        break;
    case SYM_EXTERNAL:
        AsmFree( dir->e.extinfo );
        break;
    case SYM_CLASS_LNAME:
        AsmFree( dir->e.lnameinfo );
        break;
    case SYM_CONST:
        if( !dir->e.constinfo->predef ) {
#ifdef DEBUG_OUT
            if( ( dir->e.constinfo->count > 0 )
              && ( dir->e.constinfo->tokens[0].class != TC_NUM ) ) {
                DebugMsg( ( "freeing const(String): %s = ", dir->sym.name ) );
            } else {
                DebugMsg( ( "freeing const(Number): %s = ", dir->sym.name ) );
            }
#endif
            for( i = 0; i < dir->e.constinfo->count; i++ ) {
#ifdef DEBUG_OUT
                if( dir->e.constinfo->tokens[i].class == TC_NUM ) {
                    DebugMsg(( "%d ", dir->e.constinfo->tokens[i].u.value ));
                } else {
                    DebugMsg(( "%s ", dir->e.constinfo->tokens[i].string_ptr ));
                }
#endif
                AsmFree( dir->e.constinfo->tokens[i].string_ptr );
            }
            DebugMsg(( "\n" ));
            AsmFree( dir->e.constinfo->tokens );
            AsmFree( dir->e.constinfo );
        }
        break;
    case SYM_PROC:
        {
            label_list  *labelcurr;
            label_list  *labelnext;
            regs_list   *regcurr;
            regs_list   *regnext;

            for( labelcurr = dir->e.procinfo->params.head; labelcurr != NULL; labelcurr = labelnext ) {
                labelnext = labelcurr->next;
                AsmFree( labelcurr->label );
                AsmFree( labelcurr->replace );
                AsmFree( labelcurr );
            }

            for( labelcurr = dir->e.procinfo->locals.head; labelcurr != NULL; labelcurr = labelnext ) {
                labelnext = labelcurr->next;
                AsmFree( labelcurr->label );
                AsmFree( labelcurr->replace );
                AsmFree( labelcurr );
            }

            for( labelcurr = dir->e.procinfo->labels.head; labelcurr != NULL; labelcurr = labelnext ) {
                labelnext = labelcurr->next;
                AsmFree( labelcurr->label );
                AsmFree( labelcurr->replace );
                if( labelcurr->sym != NULL )
                    FreeASym( labelcurr->sym );
                AsmFree( labelcurr );
            }

            for( regcurr = dir->e.procinfo->regslist; regcurr != NULL; regcurr = regnext ) {
                regnext = regcurr->next;
                AsmFree( regcurr->reg );
                AsmFree( regcurr );
            }
            AsmFree( dir->e.procinfo );
        }
        break;
    case SYM_MACRO:
        {
            parm_list       *labelcurr;
            parm_list       *labelnext;
            asmline         *linecurr;
            asmline         *linenext;
            local_label     *localcurr;
            local_label     *localnext;

            /*
             * free the parm list
             */
            for( labelcurr = dir->e.macroinfo->params.head; labelcurr != NULL; labelcurr = labelnext ) {
                labelnext = labelcurr->next;
                AsmFree( labelcurr->label );
                if( labelcurr->replace != NULL ) {
                    AsmFree( labelcurr->replace );
                }
                if( labelcurr->def != NULL ) {
                    AsmFree( labelcurr->def );
                }
                AsmFree( labelcurr );
            }
            /*
             * free the labels list
             */
            for( localcurr = dir->e.macroinfo->labels.head; localcurr != NULL; localcurr = localnext ) {
                localnext = localcurr->next;
                AsmFree( localcurr->label );
                AsmFree( localcurr );
            }
            /*
             * free the lines list
             */
            for( linecurr = dir->e.macroinfo->lines.head; linecurr != NULL; linecurr = linenext ) {
                linenext = linecurr->next;
                AsmFree( linecurr->line );
                AsmFree( linecurr );
            }

            AsmFree( dir->e.macroinfo );
        }
        break;
    case SYM_STRUCT:
        {
            field_list      *ptr;
            field_list      *next;

            for( ptr = dir->e.structinfo->fields.head; ptr != NULL; ptr = next ) {
                next = ptr->next;
                AsmFree( ptr->initializer );
                AsmFree( ptr->value );
                if( ptr->sym != NULL )
                    FreeASym( ptr->sym );
                AsmFree( ptr );
            }
            AsmFree( dir->e.structinfo );
        }
        break;
    default:
        break;
    }
}

static asm_sym_handle InsertClassLname( const char *name )
/********************************************************/
{
    dir_node_handle dir;

    if( strlen( name ) > MAX_LNAME ) {
        AsmError( LNAME_TOO_LONG );
        return( NULL );
    }
    for( dir = Tables[TAB_CLASS_LNAME].head; dir != NULL; dir = dir->next ) {
        if( stricmp( dir->sym.name, name ) == 0 ) {
            return( &dir->sym );
        }
    }
    dir = AllocD( name );
    if( dir == NULL )
        return( NULL );
    dir_init( dir, TAB_CLASS_LNAME );
    AddLnameData( dir );
    return( &dir->sym );
}

void wipe_space( char *token )
/*****************************
 * wipe out the spaces at the beginning of a token
 */
{
    char        *start;

    if( token == NULL )
        return;
    if( strlen( token ) == 0 )
        return;

    for( start = token;; start++ ){
        if( *start != ' '
          && *start != '\t' ) {
            break;
        }
    }
    if( start == token )
        return;

    memmove( token, start, strlen( start ) + 1 );
}

uint_32 GetCurrAddr( void )
/*************************/
{
    if( CurrSeg == NULL )
        return( 0 );
    return( CurrSeg->seg->e.seginfo->current_loc );
}

dir_node_handle GetCurrSeg( void )
/********************************/
{
    if( CurrSeg == NULL )
        return( NULL );
    return( CurrSeg->seg );
}

uint_32 GetCurrSegAlign( void )
/*****************************/
{
    if( CurrSeg == NULL )
        return( 0 );
    switch( CurrSeg->seg->e.seginfo->align ) {
    case ALIGN_BYTE:
        return( 1 );
    case ALIGN_WORD:
        return( 2 );
    case ALIGN_DWORD:
        return( 4 );
    case ALIGN_ABS: /* Paragraph aligned by definition. */
    case ALIGN_PARA:
        return( 16 );
    case ALIGN_PAGE:
        return( 256 );
    case ALIGN_4KPAGE:
        return( 4096 );
    default:
        return( 0 );
    }
}

uint_32 GetCurrSegStart( void )
/*****************************/
{
    if( CurrSeg == NULL )
        return( 0 );
    /**/myassert( !write_to_file
                || ( CurrSeg->seg->e.seginfo->current_loc - BufSize )
                     == CurrSeg->seg->e.seginfo->start_loc );
    return( CurrSeg->seg->e.seginfo->start_loc );
}

static int token_cmp( const char *token, int start, int end )
/************************************************************
 * compare token against those specified in TypeInfo[start...end]
 */
{
    int         i;
    const char  *tkn;

    for( i = start; i <= end; i++ ) {
        tkn = TypeInfo[i].string;
        if( tkn == NULL )
            continue;
        if( stricmp( tkn, token ) == 0 ) {
            /*
             * type is found
             */
            return( i );
        }
    }
    return( TOK_INVALID );      // No type is found
}

bool CheckForLang( token_buffer *tokbuf, token_idx i, int *lang )
/***************************************************************/
{
    const char  *token;
    int         lang_idx;

    if( tokbuf->tokens[i].class == TC_ID ) {
        token = tokbuf->tokens[i].string_ptr;
        /*
         * look up the type of token
         */
        if( Options.mode & MODE_TASM ) {
            lang_idx = token_cmp( token, TOK_LANG_NOLANG, TOK_LANG_SYSCALL );
        } else {
            lang_idx = token_cmp( token, TOK_LANG_BASIC, TOK_LANG_SYSCALL );
        }
        if( lang_idx != TOK_INVALID ) {
            *lang = TypeInfo[lang_idx].value;
            return( RC_OK );
        }
    }
    return( RC_ERROR );
}

static int GetLangType( token_buffer *tokbuf, token_idx *i )
/**********************************************************/
{
    int         lang_type;

    if( CheckForLang( tokbuf, *i, &lang_type ) ) {
        return( ModuleInfo.langtype );
    }
    (*i)++;
    return( lang_type );
}

static const char *Check4Mangler( token_buffer *tokbuf, token_idx *i )
/********************************************************************/
{
    const char  *mangle_type = NULL;

    if( tokbuf->tokens[*i].class == TC_STRING ) {
        mangle_type = tokbuf->tokens[*i].string_ptr;
        (*i)++;
        if( tokbuf->tokens[*i].class != TC_COMMA ) {
            AsmWarn( 2, EXPECTING_COMMA );
        } else {
            (*i)++;
        }
    }
    return( mangle_type );
}

bool ExtDef( token_buffer *tokbuf, token_idx i, bool glob_def )
/*************************************************************/
{
    const char      *name;
    const char      *typetoken;
    const char      *mangle_type = NULL;
    int             type;
    memtype         mem_type;
    dir_node_handle dir;
    int             lang_type;

    mangle_type = Check4Mangler( tokbuf, &i );
    for( ; i < tokbuf->count; i++ ) {
        /*
         * get the symbol language type if present
         */
        lang_type = GetLangType( tokbuf, &i );
        /*
         * get the symbol name
         */
        name = tokbuf->tokens[i].string_ptr;
        i++;    /* skip symbol name */
        if( tokbuf->tokens[i].class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;    /* skip ':' character */

        typetoken = tokbuf->tokens[i].string_ptr;
        type = token_cmp( typetoken, TOK_EXT_NEAR, TOK_EXT_ABS );
        if( type == TOK_INVALID ) {
            if( glob_def
              || !IsLabelStruct( typetoken ) ) {
                AsmError( INVALID_QUALIFIED_TYPE );
                return( RC_ERROR );
            }
            mem_type = MT_STRUCT;
        } else {
            mem_type = TypeInfo[type].value;
        }
        for( ; i < tokbuf->count && tokbuf->tokens[i].class != TC_COMMA; i++ )
            {}

        dir = (dir_node_handle)AsmGetSymbol( name );
        if( dir == NULL ) {
            dir = dir_insert( name, TAB_EXT );
            if( dir == NULL ) {
                return( RC_ERROR );
            }
        } else if( dir->sym.state == SYM_UNDEFINED ) {
            dir_change( dir, TAB_EXT );
        } else if( dir->sym.mem_type != mem_type ) {
            AsmError( EXT_DEF_DIFF );
            return( RC_ERROR );
        } else if( dir->sym.state != SYM_EXTERNAL ) {
            SetMangler( &dir->sym, mangle_type, lang_type );
            if( glob_def
              && !dir->sym.public ) {
                AddPublicData( dir );
            }
            return( RC_OK );
        } else {
            SetMangler( &dir->sym, mangle_type, lang_type );
            return( RC_OK );
        }

        if( glob_def ) {
            dir->e.extinfo->global = true;
        } else {
            dir->sym.referenced = true;
        }
        GetSymInfo( &dir->sym );
        dir->sym.offset = 0;
        // FIXME !! symbol can have different type
        dir->sym.mem_type = mem_type;
        SetMangler( &dir->sym, mangle_type, lang_type );
    }
    return( RC_OK );
}

bool PubDef( token_buffer *tokbuf, token_idx i )
/**********************************************/
{
    const char      *name;
    const char      *mangle_type = NULL;
    dir_node_handle dir;
    int             lang_type;

    mangle_type = Check4Mangler( tokbuf, &i );
    for( ; i < tokbuf->count; i += 2 ) {
        /*
         * get the symbol language type if present
         */
        lang_type = GetLangType( tokbuf, &i );
        /*
         * get the symbol name
         */
        name = tokbuf->tokens[i].string_ptr;
        /*
         * Add the public name
         */
        dir = (dir_node_handle)AsmGetSymbol( name );
        if( dir == NULL ) {
            dir = (dir_node_handle)AllocDSym( name );
            AddPublicData( dir );
        } else if( dir->sym.state == SYM_CONST ) {
            /*
             * check if the symbol expands to another symbol,
             * and if so, expand it
             */
            if( dir->e.constinfo->tokens[0].class == TC_ID ) {
                ExpandTheWorld( tokbuf, i, false, true );
                return( PubDef( tokbuf, i ) );
            }
        }
        SetMangler( &dir->sym, mangle_type, lang_type );
        if( !dir->sym.public ) {
            /*
             * put it into the pub table
             */
            AddPublicData( dir );
        }
    }
    return( RC_OK );
}

static dir_node_handle CreateGroup( const char *name )
/****************************************************/
{
    dir_node_handle grp;

    grp = (dir_node_handle)AsmGetSymbol( name );

    if( grp == NULL ) {
        grp = dir_insert( name, TAB_GRP );
        AddLnameData( grp );
    } else if( grp->sym.state == SYM_UNDEFINED ) {
        dir_change( grp, TAB_GRP );
        AddLnameData( grp );
    } else if( grp->sym.state != SYM_GRP ) {
        AsmErr( SYMBOL_PREVIOUSLY_DEFINED, name );
        grp = NULL;
    }
    return( grp );
}

bool GrpDef( token_buffer *tokbuf, token_idx i )
/**********************************************/
{
    const char      *name;
    dir_node_handle grp;
    seg_list        *new;
    seg_list        *curr;
    dir_node_handle seg;
    token_idx       n;

    if( Options.mode & MODE_TASM ) {
        if( i > 0 ) {
            n = i++ - 1;
        } else if( tokbuf->tokens[i + 1].class != TC_FINAL ) {
            n = ++i;
            i++;
        } else {
            n = INVALID_IDX;
        }
    } else if( i > 0 ) {
        n = i++ - 1;
    } else {
        n = INVALID_IDX;
    }
    if( ISINVALID_IDX( n ) ) {    /* name present? */
        AsmError( GRP_NAME_MISSING );
        return( RC_ERROR );
    }
    name = tokbuf->tokens[n].string_ptr;
    grp = CreateGroup( name );
    if( grp == NULL )
        return( RC_ERROR );

    for( ; i < tokbuf->count;     // stop at the end of the line
         i += 2 ) {             // skip over commas
        name = tokbuf->tokens[i].string_ptr;
        /*
         * Add the segment name
         */
        seg = (dir_node_handle)AsmGetSymbol( name );
        if( seg == NULL ) {
            seg = dir_insert( name, TAB_SEG );
        } else if( seg->sym.state == SYM_UNDEFINED ) {
            dir_change( seg, TAB_SEG );
        } else if( seg->sym.state != SYM_SEG ) {
            AsmErr( SYMBOL_PREVIOUSLY_DEFINED, name );
            return( RC_ERROR );
        }
        if( seg->e.seginfo->group == &grp->sym )    // segment is in group already
            continue;
        if( seg->e.seginfo->group != NULL ) {      // segment is in another group
            AsmError( SEGMENT_IN_GROUP );
            return( RC_ERROR );
        }
        /*
         * set the grp index of the segment
         */
        seg->e.seginfo->group = &grp->sym;

        new = AsmAlloc( sizeof( seg_list ) );
        new->seg = seg;
        new->next = NULL;
        grp->e.grpinfo->numseg++;
        /*
         * insert the segment at the end of linked list
         */
        if( grp->e.grpinfo->seglist == NULL ) {
            grp->e.grpinfo->seglist = new;
        } else {
            curr = grp->e.grpinfo->seglist;
            while( curr->next != NULL ) {
                curr = curr->next;
            }
            curr->next = new;
        }
    }
    return( RC_OK );
}

static bool SetUse32( void )
/**************************/
{
    if( CurrSeg == NULL ) {
        Use32 = ModuleInfo.def_use32;
    } else {
        Globals.code_seg = SEGISCODE( CurrSeg );
        Use32 = CurrSeg->seg->e.seginfo->use32;
        if( Use32
          && ( (Code->info.cpu & P_CPU_MASK) < P_386 ) ) {
            AsmError( WRONG_CPU_FOR_32BIT_SEGMENT );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

bool SetUse32Def( bool use32 )
/****************************/
{
    if( CurrSeg == NULL ) {                     // outside any segments
        if( ModuleInfo.model == MOD_NONE ) {    // model undefined
            ModuleInfo.def_use32 = use32;
            if( LineNumber == 0 ) {
                ModuleInfo.def_use32_init = use32;
            }
        }
    }
    return( SetUse32() );
}

bool SetCurrSeg( token_buffer *tokbuf, token_idx i )
/**************************************************/
{
    char            *name;
    dir_node_handle seg;

    switch( tokbuf->tokens[i].u.token ) {
    case T_SEGMENT:
        FlushCurrSeg();
        if( Options.mode & MODE_IDEAL ) {
            name = tokbuf->tokens[i + 1].string_ptr;
        } else {
            name = tokbuf->tokens[i-1].string_ptr;
        }
        seg = (dir_node_handle)AsmGetSymbol( name );
        /**/ myassert( seg != NULL );
        push_seg( seg );
        break;
    case T_ENDS:
        FlushCurrSeg();
        pop_seg();
        break;
    default:
        break;
    }
    return( SetUse32() );
}

static seg_type ClassNameType( const char *name )
/***********************************************/
{
    size_t  slen;
    char    uname[257];

    if( name == NULL )
        return( SEGTYPE_UNDEF );
    if( ModuleInfo.model != MOD_NONE ) {
        if( Options.code_class != NULL
          && stricmp( name, Options.code_class ) == 0 ) {
            return( SEGTYPE_ISCODE );
        }
    }
    slen = strlen( name );
    memcpy( uname, name, slen + 1 );
    strupr( uname );
    if( slen >= 3 ) {
        // 'CONST'
        if( CMPSIM( uname, get_sim_class( SIM_CONST ) ) == 0 )
            return( SEGTYPE_ISDATA );
        // 'STACK'
        if( CMPSIM( uname, get_sim_class( SIM_STACK ) ) == 0 )
            return( SEGTYPE_ISDATA );
        // 'DBTYP'
        if( CMPLIT( uname, "DBTYP" ) == 0 )
            return( SEGTYPE_ISDATA );
        // 'DBSYM'
        if( CMPLIT( uname, "DBSYM" ) == 0 )
            return( SEGTYPE_ISDATA );
        // 'CODE'
        if( CMPSIM( uname , get_sim_class( SIM_CODE ) ) == 0 )
            return( SEGTYPE_ISCODE );
        // '...DATA'
        if( CMPSIMEND( uname + slen, get_sim_class( SIM_DATA ) ) == 0 )
            return( SEGTYPE_ISDATA );
        // '...BSS'
        if( CMPSIMEND( uname + slen, get_sim_class( SIM_DATA_UN ) ) == 0 ) {
            return( SEGTYPE_ISDATA );
        }
    }
    return( SEGTYPE_UNDEF );
}

static seg_type SegmentNameType( char *name )
/*******************************************/
{
    size_t  slen;
    char    uname[257];

    slen = strlen( name );
    memcpy( uname, name, slen + 1 );
    strupr( uname );
    if( slen >= 4 ) {
        // '..._TEXT'
        if( CMPSIMEND( uname + slen, get_sim_name( SIM_CODE, NULL ) ) == 0 )
            return( SEGTYPE_ISCODE );
        // '..._DATA'
        if( CMPSIMEND( uname + slen, get_sim_name( SIM_DATA, NULL ) ) == 0 )
            return( SEGTYPE_ISDATA );
        // 'CONST...'
        if( CMPSIMBEG( uname, get_sim_name( SIM_CONST, NULL ) ) == 0 )
            return( SEGTYPE_ISDATA );
        // '..._BSS'
        if( CMPSIMEND( uname + slen, get_sim_name( SIM_DATA_UN, NULL ) ) == 0 ) {
            return( SEGTYPE_ISDATA );
        }
    }
    return( SEGTYPE_UNDEF );
}

bool SegDef( token_buffer *tokbuf, token_idx i )
/**********************************************/
{
    const char      *token;
    seg_info        *new_info;
    seg_info        *old_info;
    int             type;       // type of option
    uint            initstate;  // to show if a field is initialized
    dir_node_handle seg;
    char            *name;
    token_idx       n;

    if( Options.mode & MODE_IDEAL ) {
        if( i > 0 ) {
            n = INVALID_IDX;
        } else if( tokbuf->tokens[i].u.token == T_ENDS
          || tokbuf->tokens[i + 1].class != TC_FINAL ) {
            n = i + 1;
        } else {
            n = INVALID_IDX;
        }
    } else {
        if( i > 0 ) {
            n = i - 1;
        } else {
            n = INVALID_IDX;
        }
    }
    if( ISINVALID_IDX( n ) ) {
        AsmError( SEG_NAME_MISSING );
        return( RC_ERROR );
    }
    switch( tokbuf->tokens[i].u.token ) {
    case T_SEGMENT:
        name = tokbuf->tokens[n].string_ptr;
        /*
         * Check to see if the segment is already defined
         */
        seg = (dir_node_handle)AsmGetSymbol( name );
        if( seg == NULL ) {
            /*
             * segment is not defined
             */
            seg = dir_insert( name, TAB_SEG );
            old_info = new_info = seg->e.seginfo;
        } else if ( seg->sym.state == SYM_SEG ) {
            /*
             * segment already defined
             */
            old_info = seg->e.seginfo;
            if( seg->sym.segment == NULL ) {
                /*
                 * segment was mentioned in a group statement,
                 * but not really set up
                 */
                new_info = old_info;
            } else {
                new_info = AsmAlloc( sizeof( seg_info ) );
            }
        } else {
            /*
             * symbol is different kind, change to segment
             */
            dir_change( seg, TAB_SEG );
            old_info = new_info = seg->e.seginfo;
        }
        /*
         * Setting up default values
         */
        if( new_info == old_info ) {
            new_info->align = ALIGN_PARA;
            new_info->combine = COMB_INVALID;
            new_info->use32 = ModuleInfo.def_use32;
            new_info->class_name = InsertClassLname( "" );
            new_info->readonly = false;
            new_info->iscode = SEGTYPE_UNDEF;
        } else {
            new_info->readonly = old_info->readonly;
            new_info->iscode = old_info->iscode;
            new_info->align = old_info->align;
            new_info->combine = old_info->combine;
            if( !old_info->ignore ) {
                new_info->use32 = old_info->use32;
            } else {
                new_info->use32 = ModuleInfo.def_use32;
            }
            new_info->class_name = old_info->class_name;
        }
        new_info->ignore = false; // always false unless set explicitly
        new_info->length = 0;

        if( lastseg.stack_size > 0 ) {
            new_info->length = lastseg.stack_size;
            lastseg.stack_size = 0;
        }

        initstate = 0;
        if( Options.mode & MODE_IDEAL ) {
            /*
             * Go past SEGMENT and name
             */
            i += 2;
        } else {
            /*
             * Go past SEGMENT
             */
            i++;
        }
        for( ; i < tokbuf->count; i++ ) {
            if( tokbuf->tokens[i].class == TC_STRING ) {
                /*
                 * the class name - the only token which is of type STRING
                 */
                token = tokbuf->tokens[i].string_ptr;
                new_info->class_name = InsertClassLname( token );
                if( new_info->class_name == NULL ) {
                    goto error;
                }
                continue;
            }
            /*
             * go through all tokens EXCEPT the class name
             */
            token = tokbuf->tokens[i].string_ptr;
            /*
             * look up the type of token
             */
            type = token_cmp( token, TOK_READONLY, TOK_AT );
            if( type == TOK_INVALID ) {
                AsmError( UNDEFINED_SEGMENT_OPTION );
                goto error;
            }
            /*
             * initstate is used to check if any field is already
             * initialized
             */
            if( initstate & TypeInfo[type].init ) {
                /*
                 * initialized already
                 */
                AsmError( SEGMENT_PARA_DEFINED );
                goto error;
            } else {
                /*
                 * mark it initialized
                 */
                initstate |= TypeInfo[type].init;
            }
            switch( type ) {
            case TOK_READONLY:
                new_info->readonly = true;
                break;
            case TOK_BYTE:
            case TOK_WORD:
            case TOK_DWORD:
            case TOK_PARA:
            case TOK_PAGE:
                new_info->align = (uint_8)TypeInfo[type].value;
                break;
            case TOK_PRIVATE:
            case TOK_PUBLIC:
            case TOK_STACK:
            case TOK_COMMON:
            case TOK_MEMORY:
                new_info->combine = (uint_8)TypeInfo[type].value;
                break;
            case TOK_USE16:
            case TOK_USE32:
                new_info->use32 = ( TypeInfo[type].value != 0 );
                break;
            case TOK_IGNORE:
                new_info->ignore = true;
                break;
            case TOK_AT:
                new_info->align = SEGDEF_ALIGN_ABS;
                ExpandTheWorld( tokbuf, i + 1, false, true );
                if( tokbuf->tokens[i + 1].class == TC_NUM ) {
                    i++;
                    new_info->abs_frame = (uint_16)tokbuf->tokens[i].u.value;
                } else {
                    AsmError( UNDEFINED_SEGMENT_OPTION );
                    goto error;
                }
                break;
            default:
                AsmError( UNDEFINED_SEGMENT_OPTION );
                goto error;
            }
        }
        token = new_info->class_name->name;
        if( new_info->iscode != SEGTYPE_ISCODE ) {
            seg_type res;

            res = ClassNameType( token );
            if( res != SEGTYPE_UNDEF ) {
                new_info->iscode = res;
            } else {
                res = SegmentNameType( name );
                new_info->iscode = res;
            }
        }

        if( new_info == old_info ) {
            /*
             * A new_info definition
             */
            new_info->start_loc = 0;
            new_info->current_loc = 0;
            seg->sym.segment = &seg->sym;
            seg->sym.offset = 0;
            dir_move_to_tail( seg, TAB_SEG );
            AddLnameData( seg );
        } else if( !old_info->ignore
          && !new_info->ignore ) {
            /*
             * Check if new_info definition is different from previous one
             */
            if( ( old_info->readonly != new_info->readonly )
              || ( old_info->align != new_info->align )
              || ( old_info->combine != new_info->combine )
              || ( old_info->use32 != new_info->use32 )
              || ( old_info->class_name != new_info->class_name ) ) {
                AsmError( SEGDEF_CHANGED );
                goto error;
            } else {
                /*
                 * definition is the same as before
                 */
            }
        } else if( old_info->ignore ) {
            /*
             * reset to the new_info values
             */
            old_info->align = new_info->align;
            old_info->combine = new_info->combine;
            old_info->readonly = new_info->readonly;
            old_info->iscode = new_info->iscode;
            old_info->ignore = new_info->ignore;
            old_info->use32 = new_info->use32;
            old_info->class_name = new_info->class_name;
        } else {    // new_info->ignore
            /*
             * keep the old_info values
             */
            new_info->align = old_info->align;
            new_info->combine = old_info->combine;
            new_info->readonly = old_info->readonly;
            new_info->iscode = old_info->iscode;
            new_info->ignore = old_info->ignore;
            new_info->use32 = old_info->use32;
            new_info->class_name = old_info->class_name;
        }
        if( !ModuleInfo.mseg
          && ( seg->e.seginfo->use32 != ModuleInfo.use32 ) ) {
            ModuleInfo.mseg = true;
        }
        if( seg->e.seginfo->use32 ) {
            ModuleInfo.use32 = true;
        }
        if( new_info != old_info )
            AsmFree( new_info );
        if( push_seg( seg ) ) {
            return( RC_ERROR );
        }
        break;
    case T_ENDS:
        if( CurrSeg == NULL ) {
            AsmError( SEGMENT_NOT_OPENED );
            return( RC_ERROR );
        }
        if( tokbuf->tokens[n].class != TC_FINAL ) {
            name = tokbuf->tokens[n].string_ptr;
            seg = (dir_node_handle)AsmGetSymbol( name );
            if( seg == NULL ) {
                AsmErr( SEG_NOT_DEFINED, name );
                return( RC_ERROR );
            } else if( seg != CurrSeg->seg ) {
                AsmError( BLOCK_NESTING_ERROR );
                return( RC_ERROR );
            }
        }
        pop_seg();
        break;
    default:
        return( RC_ERROR );
    }
    return( SetUse32() );

error:
    if( new_info != old_info )
        AsmFree( new_info );
    return( RC_ERROR );
}

bool Include( token_buffer *tokbuf, token_idx i )
/***********************************************/
{
    switch( tokbuf->tokens[i].class ) {
    case TC_ID:
    case TC_STRING:
    case TC_PATH:
        return( InputQueueFile( tokbuf->tokens[i].string_ptr ) );
    default:
        AsmError( EXPECTED_FILE_NAME );
        return( RC_ERROR );
    }
}

bool IncludeLib( token_buffer *tokbuf, token_idx i )
/**************************************************/
{
    char            *name;
    asm_sym_handle  sym;
    dir_node_handle dir;

    name = tokbuf->tokens[i].string_ptr;
    if( name == NULL ) {
        AsmError( LIBRARY_NAME_MISSING );
        return( RC_ERROR );
    }

    sym = AsmGetSymbol( name );
    if( sym == NULL ) {
        // fixme
        dir = AllocD( name );
        if( dir == NULL ) {
            return( RC_ERROR );
        }
        dir_init( dir, TAB_LIB );
    }
    return( RC_OK );
}

static char *input_dgroup( char *name, sim_seg seg, char *buffer )
/*****************************************************************
 * emit any GROUP instruction
 */
{
    if( Options.mode & MODE_IDEAL ) {
        strcpy( buffer, "GROUP DGROUP " );
    } else {
        strcpy( buffer, "DGROUP GROUP " );
    }
    switch( seg ) {
    case SIM_NONE:
        if( ModuleInfo.model == MOD_TINY ) {
            strcat( buffer, get_sim_name( SIM_CODE, Options.text_seg ) );
            strcat( buffer, "," );
        }
        strcat( buffer, get_sim_name( SIM_DATA, Options.data_seg ) );
        break;
    case SIM_CODE:
    case SIM_FARDATA:
    case SIM_FARDATA_UN:
        seg = SIM_NONE;
        /* fall through */
    default:
        strcat( buffer, get_sim_name( seg, name ) );
    }
    return( buffer );
}

static void close_lastseg( void )
/********************************
 * close the last opened simplified segment
 */
{
    if( lastseg.seg != SIM_NONE ) {
        lastseg.seg = SIM_NONE;
        if( ( CurrSeg == NULL )
          && ( *lastseg.close != '\0' ) ) {
            AsmError( DONT_MIX_SIM_WITH_REAL_SEGDEFS );
            return;
        }
        InputQueueLine( lastseg.close );
    }
}

bool Startup( token_buffer *tokbuf, token_idx i )
/************************************************
 * handles .STARTUP/STARTUPCODE
 * and .EXIT/EXITCODE directives
 */
{

    int         count;
    char        buffer[MAX_LINE_LEN];
    char        *p;
    size_t      len;

    if( ModuleInfo.model == MOD_NONE ) {
        AsmError( MODEL_IS_NOT_DECLARED );
        return( RC_ERROR );
    }
    ModuleInfo.model_cmdline = false;

    switch( tokbuf->tokens[i].u.token ) {
    case T_DOT_STARTUP:
    case T_STARTUPCODE:
        count = 0;
        len = strlen( StartAddr );
        p = CATSTR( buffer, StartAddr, len );
        *p++ = ':';
        *p = '\0';
        InputQueueLine( buffer );
        if( ModuleInfo.ostype == OPSYS_DOS ) {
            if( ModuleInfo.distance == STACK_NEAR ) {
                while( StartupDosNear[count] != NULL ) {
                    InputQueueLine( StartupDosNear[count++] );
                }
            } else {
                while( StartupDosFar[count] != NULL ) {
                    InputQueueLine( StartupDosFar[count++] );
                }
            }
        }
        StartupDirectiveFound = true;
        break;
    case T_DOT_EXIT:
    case T_EXITCODE:
        i++;
        if( ( tokbuf->tokens[i].string_ptr != NULL )
          && ( tokbuf->tokens[i].string_ptr[0] != '\0' ) ) {
            len = strlen( RetVal );
            p = CATSTR( buffer, RetVal, len );
            len = strlen( tokbuf->tokens[i].string_ptr );
            p = CATSTR( p, tokbuf->tokens[i].string_ptr, len );
            *p = '\0';
            InputQueueLine( buffer );
        }
        count = 0;
        if( ModuleInfo.ostype == OPSYS_DOS ) {
            while( ExitDos[count] != NULL ) {
                InputQueueLine( ExitDos[count++] );
            }
        } else {
            while( ExitOS2[count] != NULL ) {
                InputQueueLine( ExitOS2[count++] );
            }
        }
        break;
    default:
        break;
    }
    return( RC_OK );
}

static char *get_sim_segment_beg( char *buffer, char *name, sim_seg seg, bool ignore )
/************************************************************************************/
{
    const char  *cons;
    const char  *ign;
    const char  *fmt;

    cons = ( seg == SIM_CONST ) ? "READONLY" : "";
    ign = ( ignore ) ? "IGNORE" : "";
    fmt = ( Options.mode & MODE_IDEAL ) ? "SEGMENT %s %s %s '%s' %s %s" : "%s SEGMENT %s %s '%s' %s %s";
    sprintf( buffer, fmt,
        get_sim_name( seg, name ),
        get_sim_align( seg, ModuleInfo.def_use32 ),
        get_sim_access( seg ),
        get_sim_class( seg ),
        cons, ign );
    return( buffer );
}

static char *get_sim_segment_end( char *buffer, char *name, sim_seg seg )
/***********************************************************************/
{
    const char  *fmt;

    fmt = ( Options.mode & MODE_IDEAL ) ? "ENDS %s" : "%s ENDS";
    sprintf( buffer, fmt, get_sim_name( seg, name ) );
    return( buffer );
}

static char *get_sim_assume_code_reg( char *buffer, char *name, sim_seg seg )
/***************************************************************************/
{
    if( seg == SIM_CODE ) {
        if( ModuleInfo.model == MOD_TINY ) {
            strcpy( buffer, "ASSUME CS:DGROUP" );
        } else {
            strcpy( buffer, "ASSUME CS:" );
            strcat( buffer, get_sim_name( SIM_CODE, name ) );
        }
    } else {
        strcpy( buffer, "ASSUME CS:ERROR" );
    }
    return( buffer );
}

bool SimSeg( token_buffer *tokbuf, token_idx i )
/***********************************************
 * Handles simplified segment, based on optasm pg. 142-146
 */
{
    char        buffer[ MAX_LINE_LEN ];
    char        *name;
    asm_token   type;
    sim_seg     seg;

    if( ModuleInfo.model == MOD_NONE ) {
        AsmError( MODEL_IS_NOT_DECLARED );
        return( RC_ERROR );
    }
    ModuleInfo.model_cmdline = false;
    close_lastseg();
    type = tokbuf->tokens[i].u.token;
    /*
     * get past the directive token
     */
    i++;
    name = NULL;
    if( i < tokbuf->count ) {
        name = tokbuf->tokens[i].string_ptr;
    }
    switch( type ) {
    case T_DOT_CODE:
    case T_CODESEG:
        seg = SIM_CODE;
        if( name == NULL )
            name = Options.text_seg;
        set_macro__at_code( name );
        InputQueueLine( get_sim_segment_beg( buffer, name, seg, false ) );
        InputQueueLine( get_sim_assume_code_reg( buffer, name, seg ) );
        get_sim_segment_end( lastseg.close, name, seg );
        lastseg.seg = seg;
        break;
    case T_DOT_STACK:
    case T_STACK:
        seg = SIM_STACK;
        name = NULL;
        InputQueueLine( get_sim_segment_beg( buffer, name, seg, false ) );
        InputQueueLine( get_sim_segment_end( buffer, name, seg ) );
        if( ModuleInfo.model != MOD_FLAT ) {
            InputQueueLine( input_dgroup( name, seg, buffer ) );
        }
        if( i < tokbuf->count ) {
            if( tokbuf->tokens[i].class != TC_NUM ) {
                AsmError( CONSTANT_EXPECTED );
                return( RC_ERROR );
            }
            lastseg.stack_size = (int_16)tokbuf->tokens[i].u.value;
        } else {
            lastseg.stack_size = DEFAULT_STACK_SIZE;
        }
        break;
    case T_DOT_DATA:
    case T_DOT_DATA_UN:             // .data?
    case T_DOT_CONST:
    case T_DATASEG:
    case T_UDATASEG:
    case T_CONST:
        if( ( type == T_DOT_DATA )
          || ( type == T_DATASEG ) ) {
            seg = SIM_DATA;
        } else if( ( type == T_DOT_DATA_UN )
          || ( type == T_UDATASEG ) ) {
            seg = SIM_DATA_UN;
        } else {
            seg = SIM_CONST;
        }
        if( seg == SIM_DATA ) {
            if( name == NULL ) {
                name = Options.data_seg;
            }
        } else {
            name = NULL;
        }
        if( ModuleInfo.model != MOD_FLAT ) {
            InputQueueLine( get_sim_segment_beg( buffer, name, seg, false ) );
            InputQueueLine( get_sim_segment_end( buffer, name, seg ) );
            InputQueueLine( input_dgroup( name, seg, buffer ) );
        }
        InputQueueLine( get_sim_segment_beg( buffer, name, seg, false ) );
        InputQueueLine( get_sim_assume_code_reg( buffer, name, seg ) );
        get_sim_segment_end( lastseg.close, name, seg );
        lastseg.seg = seg;
        break;
    case T_DOT_FARDATA:
    case T_DOT_FARDATA_UN:  // .fardata?
    case T_FARDATA:
    case T_UFARDATA:
        if( ( type == T_DOT_FARDATA )
          || ( type == T_FARDATA ) ) {
            seg = SIM_FARDATA;
        } else {
            seg = SIM_FARDATA_UN;
        }
        InputQueueLine( get_sim_segment_beg( buffer, name, seg, false ) );
        InputQueueLine( get_sim_assume_code_reg( buffer, name, seg ) );
        get_sim_segment_end( lastseg.close, name, seg );
        lastseg.seg = seg;
        break;
    default:
        /**/myassert( 0 );
        break;
    }
    return( RC_OK );
}

static void ModelAssumeInit( void )
/*********************************/
{
    char        buffer[ MAX_LINE_LEN ];

    /*
     * Generates codes for assume
     */
    switch( ModuleInfo.model ) {
    case MOD_FLAT:
        InputQueueLine( "ASSUME CS:FLAT,DS:FLAT,SS:FLAT,ES:FLAT,FS:ERROR,GS:ERROR");
        break;
    case MOD_TINY:
        InputQueueLine( "ASSUME CS:DGROUP, DS:DGROUP, ES:DGROUP, SS:DGROUP" );
        break;
    case MOD_SMALL:
    case MOD_COMPACT:
    case MOD_MEDIUM:
    case MOD_LARGE:
    case MOD_HUGE:
        sprintf( buffer, "ASSUME CS:%s, DS:DGROUP, SS:DGROUP", get_sim_name( SIM_CODE, Options.text_seg ) );
        InputQueueLine( buffer );
        break;
    }
}

static void module_prologue( void )
/**********************************
 * Generates codes for .MODEL; based on optasm pg.142-146
 */
{
    char        buffer[ MAX_LINE_LEN ];

    set_macro__at_data( ( ModuleInfo.model == MOD_FLAT ) ? "FLAT" : "DGROUP" );
    AddPredefinedConstant( "@data", &info_data );
    set_macro__at_code( Options.text_seg );
    AddPredefinedConstant( "@code", &info_code );
    /*
     * Generates codes for code segment
     */
    InputQueueLine( get_sim_segment_beg( buffer, Options.text_seg, SIM_CODE, true ) );
    InputQueueLine( get_sim_segment_end( buffer, Options.text_seg, SIM_CODE ) );
    /*
     * Generates codes for data segment
     */
    InputQueueLine( get_sim_segment_beg( buffer, NULL, SIM_DATA, true ) );
    InputQueueLine( get_sim_segment_end( buffer, NULL, SIM_DATA ) );

    if( ModuleInfo.model != MOD_FLAT ) {
        InputQueueLine( input_dgroup( NULL, SIM_NONE, buffer ) );
    }

    ModelAssumeInit();

// FIXME !!
// it is temporary fix for PROC parameters
// but PTR operator should be resolved globaly
    /*
     * Fix up PTR size
     */
    switch( ModuleInfo.model ) {
    case MOD_COMPACT:
    case MOD_LARGE:
    case MOD_HUGE:
    case MOD_FLAT:
        TypeInfo[TOK_EXT_PTR].value = MT_DWORD;
        break;
    }
    /*
     * Set @CodeSize
     */
    switch( ModuleInfo.model ) {
    case MOD_MEDIUM:
    case MOD_LARGE:
    case MOD_HUGE:
        const_CodeSize.u.value = 1;
        break;
    default:
        const_CodeSize.u.value = 0;
        break;
    }
    AddPredefinedConstant( "@CodeSize", &info_CodeSize );
    /*
     * Set @DataSize
     */
    switch( ModuleInfo.model ) {
    case MOD_COMPACT:
    case MOD_LARGE:
        const_DataSize.u.value = 1;
        break;
    case MOD_HUGE:
        const_DataSize.u.value = 2;
        break;
    default:
        const_DataSize.u.value = 0;
        break;
    }
    AddPredefinedConstant( "@DataSize", &info_DataSize );
    /*
     * Set @Model
     */
    const_Model.u.value = ModuleInfo.model;
    AddPredefinedConstant( "@Model", &info_Model );
    /*
     * Set @Interface
     */
    const_Interface.u.value = ModuleInfo.langtype;
    AddPredefinedConstant( "@Interface", &info_Interface );
}

void ModuleInit( void )
/*********************/
{
    ModuleInfo.model = MOD_NONE;
    ModuleInfo.distance = STACK_NONE;
    ModuleInfo.langtype = WASM_LANG_NONE;
    ModuleInfo.ostype = OPSYS_DOS;
    ModuleInfo.cpu_init = P_86 | P_87;
    ModuleInfo.use32 = false;
    ModuleInfo.def_use32 = false;
    ModuleInfo.def_use32_init = false;
    ModuleInfo.model_cmdline = false;
    ModuleInfo.mseg = false;
    ModuleInfo.flat_grp = NULL;
    /*
     * add source file to autodependency list
     */
    ModuleInfo.srcfile = AddFlist( AsmFiles.fname[ASM] );
}

void ModuleFini( void )
/*********************/
{
}

static void set_def_seg_name( int type )
/**************************************/
{
    char        buffer[ MAX_LINE_LEN ];

    /*
     * set Options.text_seg based on module name
     */
    if( Options.text_seg == NULL ) {
        switch( type ) {
        case TOK_MEDIUM:
        case TOK_LARGE:
        case TOK_HUGE:
            sprintf( buffer, "%s%s", GetModuleName(), get_sim_name( SIM_CODE, NULL ) );
            Options.text_seg = AsmStrDup( buffer );
            break;
        default:
            break;
        }
    }
}

void DefFlatGroup( void )
/***********************/
{
    if( MAGIC_FLAT_GROUP == NULL ) {
        MAGIC_FLAT_GROUP = &CreateGroup( "FLAT" )->sym;
    }
}

bool Model( token_buffer *tokbuf, token_idx i )
/*********************************************/
{
    const char  *token;
    int         initstate = 0;
    int         type;           // type of option

    if( Parse_Pass != PASS_1 ) {
        ModelAssumeInit();
        return( RC_OK );
    }

    if( ModuleInfo.model != MOD_NONE
      && !ModuleInfo.model_cmdline ) {
        AsmError( MODEL_DECLARED_ALREADY );
        return( RC_ERROR );
    }
    ModuleInfo.model_cmdline = false;

    if( Options.mode & MODE_IDEAL ) {
        token = tokbuf->tokens[i + 1].string_ptr;
        type = token_cmp( token, TOK_USE16, TOK_USE32 );
        if( type != TOK_INVALID ) {
            SetUse32Def( TypeInfo[type].value != 0 );
            i++;
        }
    }
    i++;
    for( ; i < tokbuf->count; i++ ) {

        token = tokbuf->tokens[i].string_ptr;
        /*
         * Add the public name
         *
         * look up the type of token
         */
        type = token_cmp( token, TOK_TINY, TOK_FARSTACK );
        if( type == TOK_INVALID ) {
            if( Options.mode & MODE_TASM ) {
                type = token_cmp( token, TOK_LANG_NOLANG, TOK_LANG_SYSCALL );
            } else {
                type = token_cmp( token, TOK_LANG_BASIC, TOK_LANG_SYSCALL );
            }
            if( type == TOK_INVALID ) {
                type = token_cmp( token, TOK_OS_OS2, TOK_OS_DOS );
                if( type == TOK_INVALID ) {
                    AsmError( UNDEFINED_MODEL_OPTION );
                    return( RC_ERROR );
                }
            }
        }
        MakeConstantUnderscored( TypeInfo[type].string );

        if( initstate & TypeInfo[type].init ) {
            AsmError( MODEL_PARA_DEFINED ); // initialized already
            return( RC_ERROR );
        }
        initstate |= TypeInfo[type].init; // mark it initialized
        switch( type ) {
        case TOK_FLAT:
            DefFlatGroup();
            if( (Code->info.cpu & P_CPU_MASK) < P_386 ) {
                AsmError( CPU_OR_MODEL_MISMATCH );
                cpu_directive( T_DOT_386P );
            }
            // fall through
        case TOK_TINY:
        case TOK_SMALL:
        case TOK_COMPACT:
        case TOK_MEDIUM:
        case TOK_LARGE:
        case TOK_HUGE:
            set_def_seg_name( type );
            if( ModuleInfo.model != MOD_NONE
              && ModuleInfo.model != (mod_type)TypeInfo[type].value ) {
                AsmError( CPU_OR_MODEL_MISMATCH );
            }
            ModuleInfo.model = TypeInfo[type].value;
            break;
        case TOK_NEARSTACK:
        case TOK_FARSTACK:
            ModuleInfo.distance = TypeInfo[type].value;
            break;
        case TOK_LANG_BASIC:
        case TOK_LANG_FORTRAN:
        case TOK_LANG_PASCAL:
        case TOK_LANG_C:
        case TOK_LANG_WATCOM_C:
        case TOK_LANG_STDCALL:
        case TOK_LANG_SYSCALL:
        case TOK_LANG_NOLANG:
            ModuleInfo.langtype = TypeInfo[type].value;
            break;
        case TOK_OS_DOS:
        case TOK_OS_OS2:
            ModuleInfo.ostype = TypeInfo[type].value;
            break;
        }
        i++;
        /*
         * go past comma
         */
        if( ( i < tokbuf->count )
          && ( tokbuf->tokens[i].class != TC_COMMA ) ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }
    }

    if( (initstate & INIT_MEMORY) == 0 ) {
        AsmError( MEMORY_NOT_FOUND );
        return( RC_ERROR );
    }

    module_prologue();
    lastseg.seg = SIM_NONE;
    lastseg.stack_size = 0;
    ModuleInfo.model_cmdline = ( LineNumber == 0 );
    return( RC_OK );
}

void AssumeInit( void )
/*********************/
{
    assume_reg  reg;

    for( reg = ASSUME_FIRST; reg < ASSUME_LAST; reg++ ) {
        AssumeTable[reg].symbol = NULL;
        AssumeTable[reg].error = false;
        AssumeTable[reg].flat = false;
    }
}

bool SetAssume( token_buffer *tokbuf, token_idx i )
/**************************************************
 * Handles ASSUME statement
 */
{
    const char      *token;
    char            *segloc;
    int             reg;
    assume_info     *info;
    asm_sym_handle  sym;

    i++;    /* skip ASSUME word */
    for( ; i < tokbuf->count; i++ ) {

        token = tokbuf->tokens[i].string_ptr;
        if( token_cmp( token, TOK_NOTHING, TOK_NOTHING ) != TOK_INVALID ) {
            AssumeInit();
            continue;
        }

        i++;

        if( tokbuf->tokens[i].class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;

        if( ( tokbuf->tokens[i].class == TC_UNARY_OPERATOR )
          && ( tokbuf->tokens[i].u.token == T_SEG ) ) {
            i++;
        }

        segloc = tokbuf->tokens[i].string_ptr;
        i++;
        if( *segloc == '\0' ) {
            AsmError( SEGLOCATION_EXPECTED );
            return( RC_ERROR );
        }
        /*
         * Now store the information
         */
        reg = token_cmp( token, TOK_DS, TOK_CS );
        if( reg == TOK_INVALID ) {
            AsmError( INVALID_REGISTER );
            return( RC_ERROR );
        }
        if( ( (Code->info.cpu & P_CPU_MASK) < P_386 )
          && ( ( reg == TOK_FS )
          || ( reg == TOK_GS ) ) ) {
            AsmError( INVALID_REGISTER );
            return( RC_ERROR );
        }

        info = &(AssumeTable[TypeInfo[reg].value]);

        if( token_cmp( segloc, TOK_ERROR, TOK_ERROR ) != TOK_INVALID ) {
            info->error = true;
            info->flat = false;
            info->symbol = NULL;
        } else if( token_cmp( segloc, TOK_FLAT, TOK_FLAT ) != TOK_INVALID ) {
            DefFlatGroup();
            info->flat = true;
            info->error = false;
            info->symbol = NULL;
        } else if( token_cmp( segloc, TOK_NOTHING, TOK_NOTHING ) != TOK_INVALID ) {
            info->flat = false;
            info->error = false;
            info->symbol = NULL;
        } else {
            sym = AsmLookup( segloc );
            if( sym == NULL )
                return( RC_ERROR );
            if( ( Parse_Pass != PASS_1 )
              && ( sym->state == SYM_UNDEFINED ) ) {
                AsmErr( SYMBOL_NOT_DEFINED, segloc );
                return( RC_ERROR );
            }
            info->symbol = sym;
            info->flat = false;
            info->error = false;
        }
        /*
         * go past comma
         */
        if( ( i < tokbuf->count )
          && ( tokbuf->tokens[i].class != TC_COMMA ) ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

asm_sym_handle GetGrp( asm_sym_handle sym )
/******************************************
 * get ptr to sym's group sym
 */
{
    dir_node_handle curr;

    curr = GetSeg( sym );
    if( curr != NULL )
        return( curr->e.seginfo->group );
    return( NULL );
}

bool SymIs32( asm_sym_handle sym )
/*********************************
 * get sym's segment size
 */
{
    dir_node_handle curr;

    curr = GetSeg( sym );
    if( curr != NULL ) {
        return( curr->e.seginfo->use32 );
    }
    if( sym->state == SYM_EXTERNAL ) {
        if( ModuleInfo.mseg ) {
            curr = (dir_node_handle)sym;
            return( curr->e.extinfo->use32 );
        }
        return( ModuleInfo.use32 );
    }
    return( RC_OK );
}

bool FixOverride( token_buffer *tokbuf, token_idx index )
/********************************************************
 * Fix segment or group override
 */
{
    asm_sym_handle  sym;

    sym = AsmLookup( tokbuf->tokens[index].string_ptr );
    /**/myassert( sym != NULL );
    if( sym->state == SYM_GRP ) {
        SegOverride = sym;
        return( RC_OK );
    } else if( sym->state == SYM_SEG ) {
        SegOverride = sym;
        return( RC_OK );
    }
    AsmError( SYNTAX_ERROR );
    return( RC_ERROR );
}

static assume_reg search_assume( asm_sym_handle sym,
                            assume_reg def, int override )
/********************************************************/
{
    if( sym == NULL )
        return( ASSUME_NOTHING );
    if( def < ASSUME_NOTHING ) {
        if( AssumeTable[def].symbol != NULL ) {
            if( AssumeTable[def].symbol == sym )
                return( def );
            if( !override
              && ( AssumeTable[def].symbol == GetGrp( sym ) ) ) {
                return( def );
            }
        }
    }
    for( def = ASSUME_FIRST; def < ASSUME_LAST; def++ ) {
        if( AssumeTable[def].symbol == sym ) {
            return( def );
        }
    }
    if( override )
        return( ASSUME_NOTHING );

    for( def = ASSUME_FIRST; def < ASSUME_LAST; def++ ) {
        if( AssumeTable[def].symbol == NULL )
            continue;
        if( AssumeTable[def].symbol == GetGrp( sym ) ) {
            return( def );
        }
    }

    return( ASSUME_NOTHING );
}

#if 0
int Use32Assume( assume_reg prefix )
/**********************************/
{
    dir_node_handle dir;
    seg_list        *seg_l;
    asm_sym_handle  sym_assume;

    if( AssumeTable[prefix].flat )
        return( 1 );
    sym_assume = AssumeTable[prefix].symbol;
    if( sym_assume == NULL )
        return( EMPTY );
    if( sym_assume->state == SYM_SEG ) {
        dir = (dir_node_handle)sym_assume;
        return( dir->e.seginfo->use32 );
    } else if( sym_assume->state == SYM_GRP ) {
        dir = (dir_node_handle)sym_assume;
        seg_l = dir->e.grpinfo->seglist;
        dir = seg_l->seg;
        return( dir->e.seginfo->use32 );
    }
    return( EMPTY );
}
#endif

assume_reg GetPrefixAssume( asm_sym_handle sym, assume_reg prefix )
/*****************************************************************/
{
    asm_sym_handle  sym_assume;

    if( Parse_Pass == PASS_1 )
        return( prefix );

    if( AssumeTable[prefix].flat ) {
        Frame = MAGIC_FLAT_GROUP;
        return( prefix );
    }
    sym_assume = AssumeTable[prefix].symbol;
    if( sym_assume == NULL ) {
        if( sym->state == SYM_EXTERNAL ) {
#if 0 //NYI: Don't know what's going on here
            type = GetCurrGrp();
            if( type != 0 ) {
                Frame = FRAME_GRP;
            } else {
                type = GetSegIdx( GetCurrSeg() );
                /**/myassert( type != 0 );
                Frame = FRAME_SEG;
            }
            Frame_Datum = type;
#endif
            return( prefix );
        }
        return( ASSUME_NOTHING );
    }

    if( sym_assume->state == SYM_SEG ) {
        Frame = sym_assume->segment;
    } else if( sym_assume->state == SYM_GRP ) {
        Frame = sym_assume;
    }
    if( ( sym->segment == sym_assume )
      || ( GetGrp( sym ) == sym_assume )
      || ( sym->state == SYM_EXTERNAL ) ) {
        return( prefix );
    }
    return( ASSUME_NOTHING );
}

assume_reg GetAssume( asm_sym_handle sym, assume_reg def )
/********************************************************/
{
    assume_reg      reg;

    if( ( def < ASSUME_NOTHING )
      && AssumeTable[def].flat ) {
        Frame = MAGIC_FLAT_GROUP;
        return( def );
    }
    if( SegOverride != NULL ) {
        reg = search_assume( SegOverride, def, 1 );
    } else {
        reg = search_assume( sym->segment, def, 0 );
    }
    if( reg == ASSUME_NOTHING ) {
        if( sym->state == SYM_EXTERNAL
          && sym->segment == NULL ) {
            reg = def;
        }
    }
    if( reg < ASSUME_NOTHING ) {
        if( AssumeTable[reg].symbol == NULL ) {
        } else if( AssumeTable[reg].symbol->state == SYM_SEG ) {
            Frame = AssumeTable[reg].symbol;
        } else {
            Frame = AssumeTable[reg].symbol;
        }
        return( reg );
    }
    return( ASSUME_NOTHING );
}

bool ModuleEnd( token_buffer *tokbuf )
/************************************/
{
    char        buffer[MAX_LINE_LEN];
    token_idx   i;
    char        *p;
    size_t      len;

    if( lastseg.seg != SIM_NONE ) {
        close_lastseg();
    }

    if( !EndDirectiveProc ) {
        EndDirectiveProc = true;
        p = CATLIT( buffer, "END" );
        if( StartupDirectiveFound ) {
            StartupDirectiveFound = false;
            if( tokbuf->count > 1 ) {
                AsmError( SYNTAX_ERROR );
            }
            *p++ = ' ';
            len = strlen( StartAddr );
            p = CATSTR( p, StartAddr, len );
        } else {
            for( i = 1; i < tokbuf->count; ++i ) {
                *p++ = ' ';
                len = strlen( tokbuf->tokens[i].string_ptr );
                p = CATSTR( p, tokbuf->tokens[i].string_ptr, len );
            }
        }
        *p = '\0';
        InputQueueLine( buffer );
        return( RC_OK );
    }

    EndDirectiveFound = true;

    if( tokbuf->count == 1 ) {
        return( RC_OK );
    }

    if( tokbuf->tokens[1].class != TC_ID ) {
        AsmError( INVALID_START_ADDRESS );
        return( RC_ERROR );
    }
    ModendFixup = InsFixups[OPND1];
    return( RC_OK );
}

static int find_size( int type )
/******************************/
{
    int         ptr_size;

    switch( type ) {
    case TOK_EXT_BYTE:
    case TOK_EXT_SBYTE:
        return( 1 );
    case TOK_EXT_WORD:
    case TOK_EXT_SWORD:
        return( 2 );
    case TOK_EXT_DWORD:
    case TOK_EXT_SDWORD:
        return( 4 );
    case TOK_EXT_FWORD:
    case TOK_EXT_PWORD:
        return( 6 );
    case TOK_EXT_QWORD:
        return( 8 );
    case TOK_EXT_TBYTE:
        return( 10 );
    case TOK_EXT_OWORD:
        return( 16 );
    case TOK_EXT_PTR:
        /*
         * first determine offset size
         */
        if( (Code->info.cpu & P_CPU_MASK) >= P_386 ) {
            ptr_size = 4;
        } else {
            ptr_size = 2;
        }
        if( (ModuleInfo.model == MOD_COMPACT)
          || (ModuleInfo.model == MOD_LARGE)
          || (ModuleInfo.model == MOD_HUGE) ) {
            ptr_size += 2;      /* add segment for far data pointers */
        }
        return( ptr_size );
    case TOK_PROC_VARARG:
        return( 0 );
    default:
        return( -1 );
    }
}

static void size_override( char *buffer, int size )
/*************************************************/
{
    if( Options.mode & MODE_IDEAL ) {
        switch( size ) {
        default:
        case 0:
            buffer[0] = '\0';
            break;
        case 1:
            strcpy( buffer, "byte " );
            break;
        case 2:
            strcpy( buffer, "word " );
            break;
        case 4:
            strcpy( buffer, "dword " );
            break;
        case 6:
            strcpy( buffer, "fword " );
            break;
        case 8:
            strcpy( buffer, "qword " );
            break;
        case 10:
            strcpy( buffer, "tbyte " );
            break;
        case 16:
            strcpy( buffer, "oword " );
            break;
        }
    } else {
        switch( size ) {
        default:
        case 0:
            buffer[0] = '\0';
            break;
        case 1:
            strcpy( buffer, "byte ptr " );
            break;
        case 2:
            strcpy( buffer, "word ptr " );
            break;
        case 4:
            strcpy( buffer, "dword ptr " );
            break;
        case 6:
            strcpy( buffer, "fword ptr " );
            break;
        case 8:
            strcpy( buffer, "qword ptr " );
            break;
        case 10:
            strcpy( buffer, "tbyte ptr " );
            break;
        case 16:
            strcpy( buffer, "oword ptr " );
            break;
        }
    }
}

bool LocalDef( token_buffer *tokbuf, token_idx i )
/*************************************************
 *
 * LOCAL symbol[,symbol]...[=symbol]
 *
 * symbol:
 *       name [[count]] [:[type]]
 * count:
 *       number of array elements, default is 1
 * type:
 *       one of BYTE,SBYTE,WORD,SWORD,DWORD,SDWORD,PWORD,FWORD,TBYTE, default is WORD
 */
{
    int             type;
    label_list      *local;
    proc_info       *info;
    asm_sym_handle  sym;
    asm_sym_handle  tmp;
    char            *name;
    int             factor;
    int             size;

    if( !DefineProc ) {
        AsmError( LOCAL_VAR_MUST_FOLLOW_PROC );
        return( RC_ERROR );
    }
    i++;    /* skip LOCAL word */

    /**/myassert( CurrProc != NULL );

    info = CurrProc->e.procinfo;
    for( ; i < tokbuf->count; ) {
        if( tokbuf->tokens[i].class != TC_ID ) {
            AsmError( LABEL_IS_EXPECTED );
            return( RC_ERROR );
        }

        name = tokbuf->tokens[i].string_ptr;
        sym = AsmLookup( name );
        if( sym == NULL )
            return( RC_ERROR );

        if( sym->state != SYM_UNDEFINED ) {
            AsmErr( SYMBOL_PREVIOUSLY_DEFINED, name );
            return( RC_ERROR );
        }
        i++;    /* skip symbol name */

        sym->state = SYM_INTERNAL;
        sym->mem_type = MT_WORD;
        size = LOCAL_DEFAULT_SIZE;
        factor = 1;
        tmp = NULL;

        if( i < tokbuf->count ) {
            if( tokbuf->tokens[i].class == TC_OP_SQ_BRACKET ) {
                i++;    /* skip '[' character */
                if( ( tokbuf->tokens[i].class != TC_NUM )
                  || ( i >= tokbuf->count ) ) {
                    AsmError( SYNTAX_ERROR );
                    return( RC_ERROR );
                }
                factor = tokbuf->tokens[i].u.value;
                i++;    /* skip value */
                if( ( tokbuf->tokens[i].class != TC_CL_SQ_BRACKET )
                  || ( i >= tokbuf->count ) ) {
                    AsmError( EXPECTED_CL_SQ_BRACKET );
                    return( RC_ERROR );
                }
                i++;    /* skip ']' character */
            }
        }

        if( i < tokbuf->count ) {
            if( tokbuf->tokens[i].class != TC_COLON ) {
                AsmError( COLON_EXPECTED );
                return( RC_ERROR );
            }
            i++;    /* skip ':' character */

            type = token_cmp( tokbuf->tokens[i].string_ptr, TOK_EXT_BYTE, TOK_EXT_TBYTE );
            if( type == TOK_INVALID ) {
                tmp = AsmGetSymbol( tokbuf->tokens[i].string_ptr );
                if( tmp == NULL || tmp->state != SYM_STRUCT ) {
                    AsmError( INVALID_QUALIFIED_TYPE );
                    return( RC_ERROR );
                }
                size = ((dir_node_handle)tmp)->e.structinfo->size;
                sym->mem_type = MT_STRUCT;
            } else {
                size = find_size( type );
                sym->mem_type = TypeInfo[type].value;
            }
            i++;    /* skip type name */
        }

        info->localsize += ( size * factor );

        local = AsmAlloc( sizeof( label_list ) );
        local->label = AsmStrDup( name );
        local->size = size;
        local->replace = NULL;
        local->sym = tmp;
        local->factor = factor;
        local->is_register = false;
        /*
         * add to the tail of single linked list
         */
        local->next = NULL;
        if( info->locals.head == NULL ) {
            info->locals.head = local;
        } else {
            info->locals.tail->next = local;
        }
        info->locals.tail = local;

        if( tokbuf->tokens[i].class != TC_COMMA ) {
            if( tokbuf->tokens[i].class == TC_DIRECTIVE
              && ( tokbuf->tokens[i].u.token == T_EQU2 )
              && ( tokbuf->tokens[i + 1].class == TC_ID )
              && ( tokbuf->tokens[i + 2].class == TC_FINAL ) ) {
                i++;
                StoreConstantNumber( tokbuf->tokens[i].string_ptr, info->localsize, true );
                i++;
            }
            break;
        }
        /*
         * skip comma character
         */
        i++;
    }
    if( tokbuf->tokens[i].class != TC_FINAL ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    return( RC_OK );
}

static bool GetArgType( proc_info *info, const char *token, const char *typetoken, paramsinfo *params )
{
    label_list      *paramnode;
    int             type;
    int             parameter_size;
    int             parameter_size_aligned;
    asm_sym_handle  param;
    asm_sym_handle  tmp = NULL;
    bool            is_vararg;

    /*
     * now read qualified type
     */
    type = token_cmp( typetoken, TOK_EXT_BYTE, TOK_EXT_TBYTE );

    if( ( type == TOK_INVALID )
      && (Options.mode & MODE_IDEAL) ) {
        tmp = AsmGetSymbol( typetoken );
        if( tmp != NULL ) {
            if( tmp->state == SYM_STRUCT ) {
                type = MT_STRUCT;
            }
        }
    }
    is_vararg = false;
    if( type == TOK_INVALID ) {
        type = token_cmp( typetoken, TOK_PROC_VARARG, TOK_PROC_VARARG );
        if( type == TOK_INVALID ) {
            AsmError( INVALID_QUALIFIED_TYPE );
            return( RC_ERROR );
        }
        switch( CurrProc->sym.langtype ) {
        case WASM_LANG_NONE:
        case WASM_LANG_BASIC:
        case WASM_LANG_FORTRAN:
        case WASM_LANG_PASCAL:
            AsmError( VARARG_REQUIRES_C_CALLING_CONVENTION );
            return( RC_ERROR );
        case WASM_LANG_WATCOM_C:
            info->parasize += params->unused_stack_space;
            params->on_stack = true;
            break;
        default:
            break;
        }
        is_vararg = true;
        info->is_vararg = true;
    }
    param = AsmLookup( token );
    if( param == NULL )
        return( RC_ERROR );

    if( param->state != SYM_UNDEFINED ) {
        AsmErr( SYMBOL_PREVIOUSLY_DEFINED, param->name );
        return( RC_ERROR );
    }
    param->state = SYM_INTERNAL;
    param->mem_type = TypeInfo[type].value;
    if( type == MT_STRUCT ) {
        parameter_size = ( (dir_node_handle)tmp)->e.structinfo->size;
        params->on_stack = true;
    } else {
        parameter_size = find_size( type );
        tmp = NULL;
    }
    if( !params->on_stack ) {
        if( get_watcom_argument( parameter_size, params ) ) {
            return( RC_ERROR );
        }
    }
    paramnode = AsmAlloc( sizeof( label_list ) );
    paramnode->u.is_vararg = is_vararg;
    paramnode->size = parameter_size;
    paramnode->label = AsmStrDup( token );
    paramnode->replace = NULL;
    paramnode->sym = tmp;
    if( Use32 ) {
        parameter_size_aligned = __ROUND_UP_SIZE_DWORD( parameter_size );
    } else {
        parameter_size_aligned = __ROUND_UP_SIZE_WORD( parameter_size );
    }
    if( params->on_stack ) {
        paramnode->is_register = false;
        info->parasize += parameter_size_aligned;
    } else {
        paramnode->is_register = true;
        params->param_number++;
        params->unused_stack_space += parameter_size_aligned;
    }

    switch( CurrProc->sym.langtype ) {
    case WASM_LANG_BASIC:
    case WASM_LANG_FORTRAN:
    case WASM_LANG_PASCAL:
        /*
         * for these calling conventions parameters are stored in reverse order
         *
         * add to the head of linked list
         */
        paramnode->next = info->params.head;
        if( info->params.head == NULL ) {
            info->params.tail = paramnode;
        }
        info->params.head = paramnode;
        break;
    default:
        /*
         * for others calling conventions parameters are stored in native order
         *
         * add to the tail of linked list
         */
        paramnode->next = NULL;
        if( info->params.head == NULL ) {
            info->params.head = paramnode;
        } else {
            info->params.tail->next = paramnode;
        }
        info->params.tail = paramnode;
        break;
    }
    return( RC_OK );
}

bool ArgDef( token_buffer *tokbuf, token_idx i )
/***********************************************
 *
 * ARG argument[,argument]...[=symbol]
 *
 * argument:
 *       name [[count]] [:[type]]
 * count:
 *       number of array elements, default is 1
 * type:
 *       one of BYTE,SBYTE,WORD,SWORD,DWORD,SDWORD,PWORD,FWORD,TBYTE, default is WORD
 */
{
    const char      *token;
    proc_info       *info;
    paramsinfo      params;

    if( !DefineProc ) {
        AsmError( ARG_MUST_FOLLOW_PROC );
        return( RC_ERROR );
    }
    i++;    /* skip ARG word */

    params.param_number = 0;
    params.unused_stack_space = 0;
    params.on_stack = true;

    /**/myassert( CurrProc != NULL );

    info = CurrProc->e.procinfo;

    if( ( CurrProc->sym.langtype == WASM_LANG_WATCOM_C )
      && ( Options.watcom_params_passed_by_regs
      || !Use32 ) ) {
        params.on_stack = false;
    }
    for( ; i < tokbuf->count; ) {
        if( tokbuf->tokens[i].class != TC_ID ) {
            AsmError( LABEL_IS_EXPECTED );
            return( RC_ERROR );
        }
        /*
         * read symbol
         */
        token = tokbuf->tokens[i].string_ptr;
        i++;
        /*
         * read colon
         */
        if( tokbuf->tokens[i].class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;
        /*
         * now read qualified type
         */
        if( GetArgType( info, token, tokbuf->tokens[i].string_ptr, &params ) ) {
            return( RC_ERROR );
        }
        i++;
        if( tokbuf->tokens[i].class != TC_COMMA ) {
            if( tokbuf->tokens[i].class == TC_DIRECTIVE
              && ( tokbuf->tokens[i].u.token == T_EQU2 )
              && ( tokbuf->tokens[i + 1].class == TC_ID )
              && ( tokbuf->tokens[i + 2].class == TC_FINAL ) ) {
                i++;
                StoreConstantNumber( tokbuf->tokens[i].string_ptr, info->parasize, true );
                i++;
            }
            break;
        }
        /*
         * skip comma character
         */
        i++;
    }
    if( tokbuf->tokens[i].class != TC_FINAL ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    return( RC_OK );
}

bool UsesDef( token_buffer *tokbuf, token_idx i )
/***********************************************/
{
    proc_info   *info;
    regs_list   *regist;
    regs_list   *temp_regist;

    if( !DefineProc ) {
        AsmError( USES_MUST_FOLLOW_PROC );
        return( RC_ERROR );
    }

    /**/myassert( CurrProc != NULL );

    if( CurrProc->sym.langtype == WASM_LANG_NONE ) {
        AsmError( USES_MEANINGLESS_WITHOUT_LANGUAGE );
        return( RC_ERROR );
    }

    info = CurrProc->e.procinfo;
    i++;
    for( ; ( i < tokbuf->count ) && ( tokbuf->tokens[i].class != TC_FINAL ); i++ ) {
        regist = AsmAlloc( sizeof( regs_list ));
        regist->next = NULL;
        regist->reg = AsmStrDup( tokbuf->tokens[i].string_ptr );
        if( info->regslist == NULL ) {
            info->regslist = regist;
        } else {
            for( temp_regist = info->regslist; ; temp_regist = temp_regist->next ) {
                if( temp_regist->next == NULL ) {
                    break;
                }
            }
            temp_regist->next = regist;
        }
        i++;
        if( tokbuf->tokens[i].class != TC_COMMA ) {
            break;
        }
    }
    if( tokbuf->tokens[i].class != TC_FINAL ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    return( RC_OK );
}

bool EnumDef( token_buffer *tokbuf, token_idx i )
/************************************************
 *
 * name ENUM [var [, var...]] {
 *           [var [, var]...]
 *           [var [, var]...] }
 * or
 * ENUM name [var [, var...]] {
 *           [var [, var]...]
 *           [var [, var]...] }
 *
 * var: name [= number]
 */
{
    char            *name, string[MAX_LINE_LEN];
    token_idx       n;
    int             enums, in_braces;
    long            count;
    asm_sym_handle  sym;
    dir_node_handle dir;

    if( Options.mode & MODE_IDEAL ) {
        n = i + 1;
    } else if( i > 0 ) {
        n = --i;
    } else {
        n = INVALID_IDX;
    }
    if( ISINVALID_IDX( n )
      || ( tokbuf->tokens[n].class != TC_ID ) ) {    /* name present? */
        AsmError( ENUM_NAME_MISSING );
        return( RC_ERROR );
    }
    count = enums = in_braces = 0;
    name = tokbuf->tokens[n].string_ptr;
    if( StoreConstantNumber( name, count, true ) )
        return( RC_ERROR );
    n = 2;
    do {
        for( i = n++; ; i++ ) {
            switch( tokbuf->tokens[i].class ) {
            case TC_OP_BRACE:
                in_braces++;
                continue;
            case TC_CL_BRACE:
                in_braces--;
                continue;
            case TC_FINAL:
                n = 0;
                break;
            case TC_ID:
                name = tokbuf->tokens[i].string_ptr;
                break;
            default:
                AsmError( SYNTAX_ERROR );
                return( RC_ERROR );
            }
            if( n ) {
                if( ( tokbuf->tokens[i + 1].class == TC_DIRECTIVE )
                  && ( tokbuf->tokens[i + 1].u.token == T_EQU2 ) ) {
                    i += 2;
                    switch( tokbuf->tokens[i].class ) {
                    case TC_NUM:
                        count = tokbuf->tokens[i].u.value;
                        break;
                    case TC_ID:
                        sym = AsmGetSymbol( tokbuf->tokens[i].string_ptr );
                        if( ( sym != NULL )
                          && ( sym->state == SYM_CONST ) ) {
                            dir = (dir_node_handle)sym;
                            if( dir->e.constinfo->tokens[0].class == TC_NUM ) {
                                count = dir->e.constinfo->tokens[0].u.value;
                                break;
                            }
                        }
                    default:
                        AsmError( EXPECTING_NUMBER );
                        return( RC_ERROR );
                    }
                }
                if( StoreConstantNumber( name, count++, true ) )
                    return( RC_ERROR );
                enums++;
                if( tokbuf->tokens[i + 1].class == TC_COMMA )
                    i++;
                continue;
            } else {
                if( in_braces ) {
                    if( !ScanLine( string, MAX_LINE_LEN ) ) {
                        AsmError( UNEXPECTED_END_OF_FILE );
                        return( RC_ERROR );
                    }
                    AsmScan( tokbuf, string );
                }
            }
            break;
        }
    } while( in_braces );
    if( ( tokbuf->tokens[i].class != TC_FINAL )
      || ( enums == 0 ) ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    return( RC_OK );
}

static bool proc_exam( dir_node_handle proc, token_buffer *tokbuf, token_idx i )
/******************************************************************************/
{
    const char      *token;
    int_8           minimum;        // Minimum value of the type of token to be read
//    int_8           finish;
    proc_info       *info;
    regs_list       *regist;
    regs_list       *temp_regist;
    int             type;
    paramsinfo      params;

    params.on_stack = true;

    info = proc->e.procinfo;

    minimum = TOK_PROC_FAR;
//    finish = false;
    if( proc->sym.langtype == WASM_LANG_NONE ) {
        proc->sym.langtype = ModuleInfo.langtype;
        SetMangler( &proc->sym, NULL, WASM_LANG_NONE );
    }

    // fixme ... we need error checking here --- for nested procs

    /*
     * Obtain all the default value
     */
    info->mem_type = ( IS_PROC_FAR() ) ? MT_FAR : MT_NEAR;
    info->parasize = 0;
    info->localsize = 0;
    info->export = false;
    info->is_vararg = false;
    info->pe_type = ( (Code->info.cpu & P_CPU_MASK) == P_286 ) || ( (Code->info.cpu & P_CPU_MASK) == P_386 );
    /*
     * Parse the definition line, except the parameters
     */
    i++;
    for( ; i < tokbuf->count && tokbuf->tokens[i].class != TC_COMMA; i++ ) {
        token = tokbuf->tokens[i].string_ptr;
        if( tokbuf->tokens[i].class == TC_STRING ) {
            /*
             * name mangling
             */
            SetMangler( &proc->sym, token, WASM_LANG_NONE );
            continue;
        }

        type = token_cmp( token, TOK_PROC_FAR, TOK_PROC_USES );
        if( type == TOK_INVALID )
            break;
        if( type < minimum ) {
            AsmError( SYNTAX_ERROR );
            return( RC_ERROR );
        }
        switch( type ) {
        case TOK_PROC_FAR:
        case TOK_PROC_NEAR:
            info->mem_type = TypeInfo[type].value;
            if( Options.mode & MODE_TASM ) {
                minimum = TOK_LANG_NOLANG;
            } else {
                minimum = TOK_LANG_BASIC;
            }
            break;
        case TOK_LANG_WATCOM_C:
            if( Options.watcom_params_passed_by_regs
              || !Use32 ) {
                params.on_stack = false;
            }
            // fall through
        case TOK_LANG_BASIC:
        case TOK_LANG_FORTRAN:
        case TOK_LANG_PASCAL:
        case TOK_LANG_C:
        case TOK_LANG_STDCALL:
        case TOK_LANG_SYSCALL:
        case TOK_LANG_NOLANG:
            proc->sym.langtype = TypeInfo[type].value;
            minimum = TOK_PROC_PRIVATE;
            break;
        case TOK_PROC_EXPORT:
            info->export = true;
            // fall through
        case TOK_PROC_PUBLIC:
            if( !proc->sym.public ) {
                AddPublicData( proc );
            }
            // fall through
        case TOK_PROC_PRIVATE:
            minimum = TOK_PROC_USES;
            break;
        case TOK_PROC_USES:
            i++;
            for( ; ( i < tokbuf->count ) && ( tokbuf->tokens[i].class != TC_COMMA ); i++ ) {
                token = tokbuf->tokens[i].string_ptr;
                regist = AsmAlloc( sizeof( regs_list ));
                regist->next = NULL;
                regist->reg = AsmStrDup( token );
                if( info->regslist == NULL ) {
                    info->regslist = regist;
                } else {
                    for( temp_regist = info->regslist;;
                            temp_regist = temp_regist->next ) {
                        if( temp_regist->next == NULL ) {
                            break;
                        }
                    }
                    temp_regist->next = regist;
                }
            }
            if( tokbuf->tokens[i].class == TC_COMMA )
                i--;
            break;
        default:
            goto parms;
        }
    }

parms:
    CurrProc = proc;
    DefineProc = true;
    proc->sym.mem_type = info->mem_type;

    if( i >= tokbuf->count ) {
        return( RC_OK );
    } else if( ( proc->sym.langtype == WASM_LANG_NONE )
      && ( (Options.mode & MODE_IDEAL) == 0 ) ) {
        AsmError( LANG_MUST_BE_SPECIFIED );
        return( RC_ERROR );
    } else if( tokbuf->tokens[i].class == TC_COMMA ) {
        i++;
    }
    /*
     * reset register count and unused stack space counter
     */
    params.unused_stack_space = 0;
    params.param_number = 0;
    /*
     * now parse params
     */
    for( ; i < tokbuf->count; i++ ) {
        /*
         * read symbol
         */
        token = tokbuf->tokens[i].string_ptr;
        i++;
        /*
         * read colon
         */
        if( tokbuf->tokens[i].class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;
        /*
         * now read qualified type
         */
        if( GetArgType( info, token, tokbuf->tokens[i].string_ptr, &params ) ) {
            return( RC_ERROR );
        }
        /*
         * go past comma
         */
        i++;
        if( ( i < tokbuf->count )
          && ( tokbuf->tokens[i].class != TC_COMMA ) ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

bool ProcDef( token_buffer *tokbuf, token_idx i, bool proc_def )
/**************************************************************/
{
    dir_node_handle dir;
    char            *name;
    token_idx       n;

    /* unused parameters */ (void)proc_def;

    if( Options.mode & MODE_IDEAL ) {
        n = ++i;
    } else if( i > 0 ) {
        n = i - 1;
    } else {
        n = INVALID_IDX;
    }

    if( CurrProc != NULL ) {
        /*
         * nested procs ... push currproc on a stack
         */
        push_proc( CurrProc );
    }

    if( Parse_Pass == PASS_1 ) {
        if( ISINVALID_IDX( n )
          || ( tokbuf->tokens[n].class != TC_ID ) ) {
            AsmError( PROC_MUST_HAVE_A_NAME );
            return( RC_ERROR );
        }
        name = tokbuf->tokens[n].string_ptr;
        dir = (dir_node_handle)AsmGetSymbol( name );
        if( dir == NULL ) {
            dir = dir_insert( name, TAB_PROC );
        } else if( dir->sym.state == SYM_UNDEFINED ) {
            /*
             * alloc the procinfo struct
             */
            dir_change( dir, TAB_PROC );
        } else if( dir->sym.state == SYM_EXTERNAL ) {
            if( dir->e.extinfo->global ) {
                dir_change( dir, TAB_PROC );
                AddPublicData( dir );
            } else {
                AsmErr( SYMBOL_PREVIOUSLY_DEFINED, dir->sym.name );
                return( RC_ERROR );
            }
        } else {
            AsmErr( SYMBOL_PREVIOUSLY_DEFINED, dir->sym.name );
            return( RC_ERROR );
        }
        GetSymInfo( &dir->sym );
        if( proc_exam( dir, tokbuf, i ) ) {
            return( RC_ERROR );
        }
    } else {
        // fixme -- nested procs can be ok /**/myassert( CurrProc == NULL );
        /**/myassert( ISVALID_IDX( n ) );
        if( ISINVALID_IDX( n ) )
            return( RC_ERROR );
        dir = (dir_node_handle)AsmGetSymbol( tokbuf->tokens[n].string_ptr );
        /**/myassert( dir != NULL );
        CurrProc = dir;
        GetSymInfo( &dir->sym );
        DefineProc = true;
    }
    BackPatch( &dir->sym );
    return( RC_OK );
}

static void ProcFini( void )
/**************************/
{
    proc_info   *info;
    label_list  *curr;

    info = CurrProc->e.procinfo;
    CurrProc->sym.total_size = GetCurrAddr() - CurrProc->sym.offset;

    if( Parse_Pass == PASS_1 ) {
        for( curr = info->params.head; curr != NULL; curr = curr->next ) {
            AsmTakeOut( curr->label );
        }
        for( curr = info->locals.head; curr != NULL; curr = curr->next ) {
            AsmTakeOut( curr->label );
        }
    }
    CurrProc = pop_proc();
}

bool ProcEnd( token_buffer *tokbuf, token_idx i )
/***********************************************/
{
    if( CurrProc == NULL ) {
        AsmError( BLOCK_NESTING_ERROR );
        return( RC_ERROR );
    } else if( Options.mode & MODE_IDEAL ) {
        i++;
        if( tokbuf->tokens[i].class == TC_ID ) {
            if( ( (dir_node_handle)AsmGetSymbol( tokbuf->tokens[i].string_ptr ) != CurrProc ) ) {
                AsmError( PROC_NAME_DOES_NOT_MATCH );
            }
        }
        ProcFini();
        return( RC_OK );
    } else if( i == 0
      && (Options.mode & MODE_TASM) ) {
        ProcFini();
        return( RC_OK );
    } else if( i == 0 ) {
        AsmError( PROC_MUST_HAVE_A_NAME );
        ProcFini();
        return( RC_ERROR );
    } else if( (dir_node_handle)AsmGetSymbol( tokbuf->tokens[i-1].string_ptr ) == CurrProc ) {
        ProcFini();
        return( RC_OK );
    } else {
        AsmError( PROC_NAME_DOES_NOT_MATCH );
        ProcFini();
        return( RC_OK );
    }
}

void ProcStackInit( void )
/************************/
{
    ProcStack = NULL;
}

void ProcStackFini( void )
/************************/
{
    while( CurrProc != NULL ) {
        if( Parse_Pass == PASS_1 )
            AsmErr( PROC_IS_NOT_CLOSED, CurrProc->sym.name );
        ProcFini();
    }
}

static void push_registers( regs_list *regist )
/**********************************************
 * Push the registers list
 */
{
    char        buffer[20];
    char        *p;
    size_t      len;

    if( regist == NULL )
        return;
    p = CATLIT( buffer, "push " );
    len = strlen( regist->reg );
    p = CATSTR( p, regist->reg, len );
    *p = '\0';
    InputQueueLine( buffer );
    push_registers( regist->next );
}

static void pop_registers( regs_list *regist )
/*********************************************
 * Pop the registers list
 */
{
    char        buffer[20];
    char        *p;
    size_t      len;

    if( regist == NULL )
        return;
    pop_registers( regist->next );
    p = CATLIT( buffer, "pop " );
    len = strlen( regist->reg );
    p = CATSTR( p, regist->reg, len );
    *p = '\0';
    InputQueueLine( buffer );
}

bool WritePrologue( const char *curline )
/***************************************/
{
    char                buffer[MAX_LINE_LEN];
    proc_info           *info;
    label_list          *curr;
    unsigned long       offset;
    unsigned long       size;
    char                *p;
    paramsinfo          params;

    params.param_number = 0;
    params.on_stack = true;

    /**/myassert( CurrProc != NULL );
    info = CurrProc->e.procinfo;
    /*
     * save 1st instruction following a procedure definition
     */
    InputQueueLine( curline );

    if( Parse_Pass == PASS_1 ) {
        /*
         * Figure out the replacing string for local variables
         */
        offset = 0;
        for( curr = info->locals.head; curr != NULL; curr = curr->next ) {
            size = curr->size * curr->factor;
            if( Use32 ) {
                offset += __ROUND_UP_SIZE_DWORD( size );
            } else {
                offset += __ROUND_UP_SIZE_WORD( size );
            }
            size_override( buffer, curr->size );
            if( Options.mode & MODE_IDEAL ) {
                if( curr->sym != NULL ) {
                    sprintf( buffer + strlen( buffer ), "(%s %s%lu)", curr->sym->name,
                             ( Use32 ) ? IDEAL_LOCAL_STRING_32 : IDEAL_LOCAL_STRING, offset );
                } else {
                    sprintf( buffer + strlen( buffer ), "%s%lu",
                             ( Use32 ) ? IDEAL_LOCAL_STRING_32 : IDEAL_LOCAL_STRING, offset );
                }
            } else {
                sprintf( buffer + strlen( buffer ), "%s%lu]",
                         ( Use32 ) ? LOCAL_STRING_32 : LOCAL_STRING, offset );
            }
            curr->replace = AsmStrDup( buffer );
        }
        info->localsize = offset;
        /*
         * Figure out the replacing string for the parameters
         */
        if( info->mem_type == MT_NEAR ) {
            /*
             * offset from BP : return addr + old BP
             */
            offset = 4;
        } else {
            offset = 6;
        }
        if( Use32 )
            offset *= 2;
        if( ( CurrProc->sym.langtype == WASM_LANG_WATCOM_C )
          && ( Options.watcom_params_passed_by_regs
          || !Use32 )
          && !info->is_vararg ) {
            params.on_stack = false;
        }
        for( curr = info->params.head; curr != NULL; curr = curr->next ) {
            if( !params.on_stack ) {
                if( get_watcom_argument_string( buffer, curr->size, &params ) ) {
                    return( RC_ERROR );
                }
            }
            if( params.on_stack ) {
                size_override( buffer, curr->size );
                if( Use32 ) {
                    if( Options.mode & MODE_IDEAL ) {
                        if( curr->sym != NULL ) {
                            sprintf( buffer + strlen(buffer), "(%s %s%lu)", curr->sym->name,
                                     IDEAL_ARGUMENT_STRING_32, offset );
                        } else {
                            sprintf( buffer + strlen(buffer), "%s%lu",
                                     IDEAL_ARGUMENT_STRING_32, offset );
                        }
                    } else {
                        sprintf( buffer + strlen(buffer), "%s%lu]",
                                 ARGUMENT_STRING_32, offset );
                    }
                } else {
                    if( Options.mode & MODE_IDEAL ) {
                        if( curr->sym != NULL ) {
                            sprintf( buffer + strlen(buffer), "(%s %s%lu)", curr->sym->name,
                                     IDEAL_ARGUMENT_STRING, offset );
                        } else {
                            sprintf( buffer + strlen(buffer), "%s%lu",
                                     IDEAL_ARGUMENT_STRING, offset );
                        }
                    } else {
                        sprintf( buffer + strlen(buffer), "%s%lu]",
                                 ARGUMENT_STRING, offset );
                    }
                }
                if( Use32 ) {
                    offset += __ROUND_UP_SIZE_DWORD( curr->size );
                } else {
                    offset += __ROUND_UP_SIZE_WORD( curr->size );
                }
            } else {
                params.param_number++;
            }
            curr->replace = AsmStrDup( buffer );
            if( curr->replace[0] == ' ' ) {
                curr->replace[0] = '\0';
                if( Use32 ) {
                    curr->replace[4] = '\0';
                } else {
                    curr->replace[3] = '\0';
                }
            }
        }
    }
    if( (Options.mode & MODE_IDEAL)
      && ( CurrProc->sym.langtype == WASM_LANG_NONE ) )
        return( RC_OK );
    in_prologue = true;
    PushLineQueue();
    if( ( info->localsize != 0 )
      || ( info->parasize != 0 )
      || ( info->is_vararg )
      || Options.trace_stack == 2 ) {
        /*
         * prolog code timmings
         *
         *                                                   best result
         *               size  86  286  386  486  P     86  286  386  486  P
         * push bp       2     11  3    2    1    1
         * mov bp,sp     2     2   2    2    1    1
         * sub sp,immed  4     4   3    2    1    1
         *              -----------------------------
         *               8     17  8    6    3    3     x   x    x    x    x
         *
         * push ebp      2     -   -    2    1    1
         * mov ebp,esp   2     -   -    2    1    1
         * sub esp,immed 6     -   -    2    1    1
         *              -----------------------------
         *               10    -   -    6    3    3              x    x    x
         *
         * enter imm,0   4     -   11   10   14   11
         *
         * write prolog code
         */
        if( Options.trace_stack
          && info->mem_type == MT_FAR ) {
            if( Use32 ) {
                CPYLIT( buffer, "inc ebp" );
            } else {
                CPYLIT( buffer, "inc bp" );
            }
            InputQueueLine( buffer );
        }
        if( Use32 ) {
            /*
             * write 80386 prolog code
             * PUSH EBP
             * MOV  EBP, ESP
             * SUB  ESP, the number of localbytes
             */
            CPYLIT( buffer, "push ebp" );
            InputQueueLine( buffer );
            CPYLIT( buffer, "mov ebp,esp" );
            if( info->localsize != 0 ) {
                InputQueueLine( buffer );
                p = CATLIT( buffer, "sub esp," );
                sprintf( p, "%lu", info->localsize );
            }
        } else {
            /*
             * write 8086 prolog code
             * PUSH BP
             * MOV  BP, SP
             * SUB  SP, the number of localbytes
             */
            CPYLIT( buffer, "push bp" );
            InputQueueLine( buffer );
            CPYLIT( buffer, "mov bp,sp" );
            if( info->localsize != 0 ) {
                InputQueueLine( buffer );
                p = CATLIT( buffer, "sub sp," );
                sprintf( p, "%lu", info->localsize );
            }
        }
        InputQueueLine( buffer );
    }
    /*
     * Push the registers
     */
    push_registers( info->regslist );
    return( RC_OK );
}

static void write_epilogue( void )
/********************************/
{
    char        buffer[MAX_LINE_LEN];
    proc_info   *info;

    /**/myassert( CurrProc != NULL );

    if( (Options.mode & MODE_IDEAL)
      && ( CurrProc->sym.langtype == WASM_LANG_NONE ) )
        return;

    info = CurrProc->e.procinfo;
    /*
     * Pop the registers
     */
    pop_registers( CurrProc->e.procinfo->regslist );

    if( ( info->localsize == 0 )
      && ( info->parasize == 0 )
      && ( !info->is_vararg )
      && Options.trace_stack != 2 ) {
        return;
    }
    /*
     * epilog code timmings
     *
     *                                                  best result
     *              size  86  286  386  486  P      86  286  386  486  P
     * mov sp,bp    2     2   2    2    1    1
     * pop bp       2     8   5    4    4    1
     *             -----------------------------
     *              4     10  7    6    5    2      x             x    x
     *
     * mov esp,ebp  2     -   -    2    1    1
     * pop ebp      2     -   -    4    4    1
     *             -----------------------------
     *              4     -   -    6    5    2                    x    x
     *
     * leave        1     -   5    4    5    3          x    x    x
     *
     * !!!! DECISION !!!!
     *
     * leave will be used for .286 and .386
     * .286 code will be best working on 286,386 and 486 processors
     * .386 code will be best working on 386 and 486 processors
     * .486 code will be best working on 486 and above processors
     *
     *   without LEAVE
     *
     *         86  286  386  486  P
     *  .8086  0   -2   -2   0    +1
     *  .286   -   -2   -2   0    +1
     *  .386   -   -    -2   0    +1
     *  .486   -   -    -    0    +1
     *
     *   LEAVE 286 only
     *
     *         86  286  386  486  P
     *  .8086  0   -2   -2   0    +1
     *  .286   -   0    +2   0    -1
     *  .386   -   -    -2   0    +1
     *  .486   -   -    -    0    +1
     *
     *   LEAVE 286 and 386
     *
     *         86  286  386  486  P
     *  .8086  0   -2   -2   0    +1
     *  .286   -   0    +2   0    -1
     *  .386   -   -    0    0    -1
     *  .486   -   -    -    0    +1
     *
     *   LEAVE 286, 386 and 486
     *
     *         86  286  386  486  P
     *  .8086  0   -2   -2   0    +1
     *  .286   -   0    +2   0    -1
     *  .386   -   -    0    0    -1
     *  .486   -   -    -    0    -1
     *
     * write epilog code
     */
    if( info->pe_type ) {
        /*
         * write 80286 and 80386 epilog code
         * LEAVE
         */
        CPYLIT( buffer, "leave" );
    } else if( Use32 ) {
        /*
         * write 32-bit 80486 or P epilog code
         * Mov ESP, EBP
         * POP EBP
         */
        if( info->localsize != 0 ) {
            CPYLIT( buffer, "mov esp,ebp" );
            InputQueueLine( buffer );
        }
        CPYLIT( buffer, "pop ebp" );
    } else {
        /*
         * write 16-bit 8086 or 80486 or P epilog code
         * Mov SP, BP
         * POP BP
         */
        if( info->localsize != 0 ) {
            CPYLIT( buffer, "mov sp,bp" );
            InputQueueLine( buffer );
        }
        CPYLIT( buffer, "pop bp" );
    }
    InputQueueLine( buffer );
    if( Options.trace_stack
      && info->mem_type == MT_FAR ) {
        if( Use32 ) {
            CPYLIT( buffer, "dec ebp" );
        } else {
            CPYLIT( buffer, "dec bp" );
        }
        InputQueueLine( buffer );
    }
}

bool Ret( token_buffer *tokbuf, token_idx i, bool flag_iret )
/***********************************************************/
{
    char        buffer[40];
    proc_info   *info;
    expr_list   opndx;
    char        *p;

    info = CurrProc->e.procinfo;

    if( flag_iret ) {
        if( tokbuf->tokens[i].u.token == T_IRET ) {
            p = CATLIT( buffer, "iretf" );
        } else {
            p = CATLIT( buffer, "iretdf" );
        }
    } else {
        if( info->mem_type == MT_NEAR ) {
            p = CATLIT( buffer, "retn " );
        } else {
            p = CATLIT( buffer, "retf " );
        }
    }
    *p = '\0';

    write_epilogue();

    if( !flag_iret ) {
        if( tokbuf->count == i + 1 ) {
            switch( CurrProc->sym.langtype ) {
            case WASM_LANG_BASIC:
            case WASM_LANG_FORTRAN:
            case WASM_LANG_PASCAL:
                if( info->parasize != 0 ) {
                    sprintf( p, "%lu", info->parasize );
                }
                break;
            case WASM_LANG_STDCALL:
                if( !info->is_vararg
                  && info->parasize != 0 ) {
                    sprintf( p, "%lu", info->parasize );
                }
                break;
            case WASM_LANG_WATCOM_C:
                if( ( Options.watcom_params_passed_by_regs
                  || !Use32 )
                  && !info->is_vararg
                  && ( info->parasize != 0 ) ) {
                    sprintf( p, "%lu", info->parasize );
                }
                break;
            default:
                break;
            }
        } else {
            ++i;
            if( EvalOperand( tokbuf, &i, tokbuf->count, &opndx, true )
              || (opndx.type != EXPR_CONST) ) {
                AsmError( CONSTANT_EXPECTED );
                return( RC_ERROR );
            }
            sprintf( p, "%d", opndx.value );
        }
    }
    InputQueueLine( buffer );
    return( RC_OK );
}

void GetSymInfo( asm_sym_handle sym )
/***********************************/
{
    sym->segment = &GetCurrSeg()->sym;
    sym->offset = GetCurrAddr();
}

bool Comment( int what_to_do, token_buffer *tokbuf, token_idx i, const char *line )
/*********************************************************************************/
{
    static bool in_comment = false;
    static char delim_char = '\0';

    switch( what_to_do ) {
    case QUERY_COMMENT:
        return( in_comment );
    case QUERY_COMMENT_DELIM:
        /*
         * return delimiting character
         */
        return( in_comment && strchr( line, delim_char ) != NULL );
    case START_COMMENT:
        i++;
        if( tokbuf->tokens[i].string_ptr == NULL ) {
            AsmError( COMMENT_DELIMITER_EXPECTED );
            return( RC_ERROR );
        }
        delim_char = tokbuf->tokens[i].string_ptr[strspn( tokbuf->tokens[i].string_ptr, " \t" )];
        if( ( delim_char == '\0' )
            || ( strchr( tokbuf->tokens[i].string_ptr, delim_char ) == NULL ) ) {
            AsmError( COMMENT_DELIMITER_EXPECTED );
            return( RC_ERROR );
        }
        if( strchr( tokbuf->tokens[i].string_ptr, delim_char )
            != strrchr( tokbuf->tokens[i].string_ptr, delim_char ) ) {
            /*
             * we have COMMENT delim. ..... delim. -- only 1 line
             */
        } else {
            in_comment = true;
        }
        return( RC_OK );
    case END_COMMENT:
        in_comment = false;
        return( RC_OK );
    }
    return( RC_ERROR );
}

bool AddAlias( token_buffer *tokbuf, token_idx i )
/************************************************/
{
    char    *tmp;
    char    *p;
    size_t  len1;
    size_t  len2;

    if( ( tokbuf->tokens[i + 1].class == TC_DIRECTIVE )
      && ( tokbuf->tokens[i + 1].u.token == T_EQU2 ) ) {
        /*
         * ALIAS <alias> = <target> (MASM, TASM)
         */
        i++;
    } else if( i > 0 ) {
        /*
         * <alias> ALIAS <target> (WASM)
         */
        i--;
    } else {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    if( ( tokbuf->tokens[i].class != TC_ID )
      || ( tokbuf->tokens[i + 2].class != TC_ID ) ) {
        AsmError( SYMBOL_EXPECTED );
        return( RC_ERROR );
    }
    /*
     * add this alias to the alias queue
     *
     * aliases are stored as:  <len><alias_name><len><substitute_name>
     */
    len1 = strlen( tokbuf->tokens[i].string_ptr );
    len2 = strlen( tokbuf->tokens[i + 2].string_ptr );
    tmp = AsmAlloc( len1 + len2 + 2 );
    AddAliasData( tmp );
    p = tmp;
    *p++ = (uint_8)len1;
    p = CATSTR( p, tokbuf->tokens[i].string_ptr, len1 );
    *p++ = (uint_8)len2;
    p = CATSTR( p, tokbuf->tokens[i + 2].string_ptr, len2 );
    return( RC_OK );
}

bool NameDirective( token_buffer *tokbuf, token_idx i )
/*****************************************************/
{
    if( Options.module_name == NULL ) {
        Options.module_name = AsmStrDup( tokbuf->tokens[i + 1].string_ptr );
        ConvertModuleName( Options.module_name );
    }
    return( RC_OK );
}

bool CommDef( token_buffer *tokbuf, token_idx i )
/***********************************************/
{
    const char      *token;
    const char      *typetoken;
    const char      *mangle_type = NULL;
    int             type;
    int             distance;
    int             count;
    dir_node_handle dir;
    int             lang_type;
    memtype         mem_type;

    mangle_type = Check4Mangler( tokbuf, &i );
    for( ; i < tokbuf->count; i++ ) {
        count = 1;
        /*
         * get the distance ( near or far )
         */
        typetoken = tokbuf->tokens[i].string_ptr;
        distance = token_cmp( typetoken, TOK_EXT_NEAR, TOK_EXT_FAR );

        if( distance != TOK_INVALID ) {
            /*
             * otherwise, there is no distance specified
             */
            i++;
        } else {
            distance = TOK_EXT_NEAR;
        }
        /*
         * get the symbol language type if present
         */
        lang_type = GetLangType( tokbuf, &i );
        /*
         * get the symbol name
         */
        token = tokbuf->tokens[i].string_ptr;
        i++;
        /*
         * go past the colon
         */
        if( tokbuf->tokens[i].class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;
        typetoken = tokbuf->tokens[i].string_ptr;
        type = token_cmp( typetoken, TOK_EXT_BYTE, TOK_EXT_TBYTE );

        if( type == TOK_INVALID ) {
            AsmError( INVALID_QUALIFIED_TYPE );
            return( RC_ERROR );
        }
        for( ; i< tokbuf->count && tokbuf->tokens[i].class != TC_COMMA; i++ ) {
            if( tokbuf->tokens[i].class == TC_COLON ) {
                i++;
                /*
                 * count
                 */
                if( tokbuf->tokens[i].class != TC_NUM ) {
                    AsmError( EXPECTING_NUMBER );
                    return( RC_ERROR );
                }
                count = tokbuf->tokens[i].u.value;
            }
        }
        mem_type = TypeInfo[type].value;
        dir = (dir_node_handle)AsmGetSymbol( token );
        if( dir == NULL ) {
            dir = dir_insert( token, TAB_EXT );
            if( dir == NULL ) {
                return( RC_ERROR );
            }
        } else if( dir->sym.state == SYM_UNDEFINED ) {
            dir_change( dir, TAB_EXT );
        } else if( dir->sym.mem_type != mem_type ) {
            AsmError( EXT_DEF_DIFF );
            return( RC_ERROR );
        }
        dir->sym.referenced = true;
        dir->e.extinfo->comm = true;
        GetSymInfo( &dir->sym );
        dir->sym.offset = 0;
        dir->sym.mem_type = mem_type;
        dir->e.extinfo->comm_size = count;
        dir->e.extinfo->comm_distance = TypeInfo[distance].value;
        SetMangler( &dir->sym, mangle_type, lang_type );
    }
    return( RC_OK );
}

bool Locals( token_buffer *tokbuf, token_idx i )
/**********************************************/
{
    Options.locals_len = ( tokbuf->tokens[i].u.token == T_LOCALS ) ? 2 : 0;
    if( i + 1 == tokbuf->count ) {
        return( RC_OK );
    }
    if( tokbuf->tokens[i].u.token == T_LOCALS ) {
        ++i;
        if( i < tokbuf->count
          && tokbuf->tokens[i].class == TC_ID
          && strlen( tokbuf->tokens[i].string_ptr ) >= 2 ) {
            Options.locals_prefix[0] = tokbuf->tokens[i].string_ptr[0];
            Options.locals_prefix[1] = tokbuf->tokens[i].string_ptr[1];
            if( tokbuf->count - i == 1
              && strlen( tokbuf->tokens[i].string_ptr ) == 2 ) {
                return( RC_OK );
            }
        }
    }
    AsmError( SYNTAX_ERROR );
    return( RC_ERROR );
}

bool Radix( token_buffer *tokbuf, token_idx i )
/*********************************************/
{
    unsigned    new_radix;

    if( i < tokbuf->count
      && tokbuf->tokens[i].class == TC_NUM ) {
        new_radix = tokbuf->tokens[i].u.value;
        switch( new_radix ) {
        case 16:
        case 10:
        case 8:
        case 2:
            RadixSet( new_radix );
            return( RC_OK );
        }
    }
    AsmError( SYNTAX_ERROR );
    return( RC_ERROR );
}
