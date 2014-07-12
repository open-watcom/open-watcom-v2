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
* Description:  Processing of assembly directives.
*
****************************************************************************/


#include "asmglob.h"
#include <ctype.h>

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

#define BIT16           0
#define BIT32           1

#define MAGIC_FLAT_GROUP        ModuleInfo.flat_grp

#define INIT_ALIGN      0x1
#define INIT_COMBINE    0x2
#define INIT_USE        0x4
#define INIT_CLASS      0x8
#define INIT_MEMORY     0x10
#define INIT_STACK      0x20

typedef struct {
      char      *string;        // the token string
      uint      value;          // value connected to this token
      uint      init;           // explained in direct.c ( look at SegDef() )
} typeinfo;

typedef struct {
    asm_sym             *symbol;        /* segment or group that is to
                                           be associated with the register */
    bool                error:1;        // the register is assumed to ERROR
    bool                flat:1;         // the register is assumed to FLAT
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
    SIM_LAST = SIM_NONE
} sim_seg;

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

static char             *Check4Mangler( token_idx *i );
static int              token_cmp( char **token, int start, int end );
static void             ModelAssumeInit( void );

extern char             write_to_file;  // write if there is no error
extern unsigned         BufSize;
extern bool             DefineProc;     // TRUE if the definition of procedure
                                        // has not ended
extern struct asm_sym   *SegOverride;
extern void             FreeASym( struct asm_sym *sym );
extern struct asmfixup  *ModendFixup;   // start address fixup

bool                    EndDirectiveFound = FALSE;
bool                    EndDirectiveProc = FALSE;

seg_list                *CurrSeg;       // points to stack of opened segments

int                     in_prologue;

static assume_info      AssumeTable[ASSUME_LAST];

symbol_queue            Tables[TAB_LAST];// tables of definitions
module_info             ModuleInfo;

static seg_list         *ProcStack = NULL;

static char *StartupDosNear[] = {
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
static char *StartupDosFar[] = {
        " mov     dx,DGROUP",
        " mov     ds,dx",
        NULL
};
static char *ExitOS2[] = { /* mov al,retval  followed by: */
        " xor ah,ah",
        " push 01h",
        " push ax",
        " call far DOSEXIT",
        NULL
};
static char *ExitDos[] = { /* mov al, retval  followed by: */
        " mov     ah,4ch",
        " int     21h",
        NULL
};
static char *RetVal = " mov    al,";

static char *StartAddr = "`symbol_reserved_for_start_address`";
static char StartupDirectiveFound = FALSE;

#define DEFAULT_CODE_CLASS      "CODE"
#define DEFAULT_CODE_NAME       "_TEXT"
#define DEFAULT_DATA_NAME       "_DATA"
#define SIM_DATA_OFFSET         5        // strlen("_DATA")
#define SIM_FARDATA_OFFSET      8        // strlen("FAR_DATA")

/* code generated by @startup */

static char *SimCodeBegin[2][2][SIM_LAST] = {
    {
        {
            "_TEXT SEGMENT WORD PUBLIC 'CODE' IGNORE",
            "STACK SEGMENT PARA STACK 'STACK' IGNORE",
            "_DATA SEGMENT WORD PUBLIC 'DATA' IGNORE",
            "_BSS  SEGMENT WORD PUBLIC 'BSS' IGNORE",
            "FAR_DATA SEGMENT PARA PRIVATE 'FAR_DATA' IGNORE",
            "FAR_BSS  SEGMENT PARA PRIVATE 'FAR_BSS' IGNORE",
            "CONST SEGMENT WORD PUBLIC 'CONST' READONLY IGNORE"
        },
        {
            "_TEXT SEGMENT DWORD USE32 PUBLIC 'CODE' IGNORE",
            "STACK SEGMENT DWORD USE32 STACK 'STACK' IGNORE",
            "_DATA SEGMENT DWORD USE32 PUBLIC 'DATA' IGNORE",
            "_BSS  SEGMENT DWORD USE32 PUBLIC 'BSS' IGNORE",
            "FAR_DATA SEGMENT DWORD USE32 PRIVATE 'FAR_DATA' IGNORE",
            "FAR_BSS  SEGMENT DWORD USE32 PRIVATE 'FAR_BSS' IGNORE",
            "CONST SEGMENT DWORD USE32 PUBLIC 'CONST' READONLY IGNORE"
        }
    },
    {
        {
            "SEGMENT _TEXT WORD PUBLIC 'CODE' IGNORE",
            "SEGMENT STACK PARA STACK 'STACK' IGNORE",
            "SEGMENT _DATA WORD PUBLIC 'DATA' IGNORE",
            "SEGMENT _BSS  WORD PUBLIC 'BSS' IGNORE",
            "SEGMENT FAR_DATA PARA PRIVATE 'FAR_DATA' IGNORE",
            "SEGMENT FAR_BSS  PARA PRIVATE 'FAR_BSS' IGNORE",
            "SEGMENT CONST WORD PUBLIC 'CONST' READONLY IGNORE"
        },
        {
            "SEGMENT _TEXT DWORD USE32 PUBLIC 'CODE' IGNORE",
            "SEGMENT STACK DWORD USE32 STACK 'STACK' IGNORE",
            "SEGMENT _DATA DWORD USE32 PUBLIC 'DATA' IGNORE",
            "SEGMENT _BSS  DWORD USE32 PUBLIC 'BSS' IGNORE",
            "SEGMENT FAR_DATA DWORD USE32 PRIVATE 'FAR_DATA' IGNORE",
            "SEGMENT FAR_BSS  DWORD USE32 PRIVATE 'FAR_BSS' IGNORE",
            "SEGMENT CONST DWORD USE32 PUBLIC 'CONST' READONLY IGNORE"
        }
    }
};

static char *SimCodeEnd[2][ SIM_LAST ] = {
    {
        "_TEXT ENDS",
        "STACK ENDS",
        "_DATA ENDS",
        "_BSS  ENDS",
        "FAR_DATA ENDS",
        "FAR_BSS  ENDS",
        "CONST ENDS"
    },
    {
        "ENDS _TEXT",
        "ENDS STACK",
        "ENDS _DATA",
        "ENDS _BSS",
        "ENDS FAR_DATA",
        "ENDS FAR_BSS",
        "ENDS CONST"
    }
};

/* Code generated by simplified segment definitions */

static last_seg_info    lastseg;        // last opened simplified segment

static char *parm_reg[3][4]= {
    { "al", "dl", "bl", "cl" },
    { "ax", "dx", "bx", "cx" },
    { "eax", "edx", "ebx ", "ecx" },
};

enum regsize {
    A_BYTE = 0,
    A_WORD,
    A_DWORD,
};

static char         code_segment_name[ MAX_LINE_LEN ];
static asm_tok      const_CodeSize = { TC_NUM, NULL, 0 };
static asm_tok      const_DataSize = { TC_NUM, NULL, 0 };
static asm_tok      const_Model = { TC_NUM, NULL, 0 };
static asm_tok      const_Interface = { TC_NUM, NULL, 0 };
static asm_tok      const_data = { TC_ID, NULL, 0 };
static asm_tok      const_code = { TC_ID, code_segment_name, 0 };

static const_info   info_CodeSize = { TRUE, 0, 0, TRUE, &const_CodeSize };
static const_info   info_DataSize = { TRUE, 0, 0, TRUE, &const_DataSize };
static const_info   info_Model = { TRUE, 0, 0, TRUE, &const_Model };
static const_info   info_Interface = { TRUE, 0, 0, TRUE, &const_Interface };
static const_info   info_data = { TRUE, 0, 0, TRUE, &const_data };
static const_info   info_code = { TRUE, 0, 0, TRUE, &const_code };

#define ROUND_UP( i, r ) (((i)+((r)-1)) & ~((r)-1))

int AddPredefinedConstant( char *name, const_info *info )
/*******************************************************/
{
    dir_node            *dir;

    dir = (dir_node *)AsmGetSymbol( name );
    if( dir == NULL ) {
        dir = dir_insert( name, TAB_CONST );
        if( dir == NULL ) {
            return( ERROR );
        }
    } else if( dir->sym.state == SYM_UNDEFINED ) {
        dir_change( dir, TAB_CONST );
    } else if( dir->sym.state != SYM_CONST ) {
        AsmError( LABEL_ALREADY_DEFINED );
        return( ERROR );
    }
    if( !dir->e.constinfo->predef ) {
        FreeInfo( dir );
        dir->e.constinfo = info;
    }
    return( NOT_ERROR );
}

static int get_watcom_argument( int size, int *parm_number )
/* returns status for a parameter using the watcom register parm passing
   conventions ( A D B C ) */
{
    int parm = *parm_number;

    switch( size ) {
    case 1:
    case 2:
        if( parm > 3 )
            return( TRUE );
        return( FALSE );
    case 4:
        if( Use32 ) {
            if( parm > 3 )
                return( TRUE );
        } else {
            if( parm > 2 )
                return( TRUE );
            *parm_number = ++parm;
        }
        return( FALSE );
    case 10:
        AsmErr( TBYTE_NOT_SUPPORTED );
        return( ERROR );
    case 6:
    case 8:
        if( Use32 ) {
            if( parm > 2 )
                return( TRUE );
            *parm_number = ++parm;
            return( FALSE );
        }
    default:
        // something wierd
        AsmError( STRANGE_PARM_TYPE );
        return( ERROR );
    }
}

static int get_watcom_argument_string( char *buffer, int size, int *parm_number )
/*************************************************************************************/
/* get the register for parms 0 to 3,
   using the watcom register parm passing conventions ( A D B C ) */
{
    int parm = *parm_number;

    if( parm > 3 )
        return( TRUE );
    switch( size ) {
    case 1:
        sprintf( buffer, parm_reg[A_BYTE][parm] );
        return( FALSE );
    case 2:
        sprintf( buffer, parm_reg[A_WORD][parm] );
        return( FALSE );
    case 4:
        if( Use32 ) {
            sprintf( buffer, parm_reg[A_DWORD][parm] );
        } else {
            if( parm > 2 )
                return( TRUE );
            sprintf( buffer, " %s %s", parm_reg[A_WORD][parm],
                     parm_reg[A_WORD][parm + 1] );
            *parm_number = ++parm;
        }
        return( FALSE );
    case 10:
            AsmErr( TBYTE_NOT_SUPPORTED );
            return( ERROR );
    case 6:
        if( Use32 ) {
            if( parm < 3 ) {
                sprintf( buffer, " %s %s", parm_reg[A_DWORD][parm],
                         parm_reg[A_WORD][parm + 1] );
                *parm_number = ++parm;
                return( FALSE );
            }
            return( TRUE );
        }
    case 8:
        if( Use32 ) {
            if( parm < 3 ) {
                sprintf( buffer, " %s %s", parm_reg[A_DWORD][parm],
                         parm_reg[A_DWORD][parm + 1] );
                *parm_number = ++parm;
                return( FALSE );
            }
            return( TRUE );
        }
    default:
        // something wierd
        AsmError( STRANGE_PARM_TYPE );
        return( ERROR );
    }
}

#ifdef DEBUG_OUT
void heap( char *func ) // for debugging only
/*********************/
{
    switch(_heapchk()) {
    case _HEAPBADNODE:
    case _HEAPBADBEGIN:
    DebugMsg(("Function : %s - ", func ));
        DebugMsg(("ERROR - heap is damaged\n"));
        exit(1);
        break;
    default:
        break;
    }
}
#endif

static int SetAssumeCSCurrSeg( void )
/*************************************/
{
    assume_info     *info;

    info = &(AssumeTable[ ASSUME_CS ]);
    if( CurrSeg == NULL ) {
        info->symbol = NULL;
        info->flat = FALSE;
        info->error = TRUE;
    } else {
        if( CurrSeg->seg->e.seginfo->group != NULL )
            info->symbol = GetGrp( &CurrSeg->seg->sym );
        else
            info->symbol = &CurrSeg->seg->sym;
        info->flat = FALSE;
        info->error = FALSE;
    }
    return( NOT_ERROR );
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

static bool push_seg( dir_node *seg )
/***********************************/
/* Push a segment into the current segment stack */
{
    seg_list    *curr;

    for( curr = CurrSeg; curr; curr = curr->next ) {
        if( curr->seg == seg ) {
            AsmError( BLOCK_NESTING_ERROR );
            return( RC_ERROR );
        }
    }
    push( &CurrSeg, seg );
    SetAssumeCSCurrSeg();
    return( RC_OK );
}

static dir_node *pop_seg( void )
/******************************/
/* Pop a segment out of the current segment stack */
{
    dir_node    *seg;

    /**/myassert( CurrSeg != NULL );
    seg = pop( &CurrSeg );
    SetAssumeCSCurrSeg();
    return( seg );
}

static void push_proc( dir_node *proc )
/*************************************/
{
    push( &ProcStack, proc );
    return;
}

static dir_node *pop_proc( void )
/*******************************/
{
    if( ProcStack == NULL )
        return( NULL );
    return( (dir_node *)pop( &ProcStack ) );
}

static void dir_add( dir_node *new, int tab )
/*******************************************/
{
    /* note: this is only for those above which do NOT return right away */
    /* put the new entry into the queue for its type of symbol */
    if( Tables[tab].head == NULL ) {
        Tables[tab].head = Tables[tab].tail = new;
        new->next = new->prev = NULL;
    } else {
        new->prev = Tables[tab].tail;
        Tables[tab].tail->next = new;
        Tables[tab].tail = new;
        new->next = NULL;
    }
}

void dir_init( dir_node *dir, int tab )
/********************************************/
/* Change node and insert it into the table specified by tab */
{
    dir->line_num = LineNumber;
    dir->next = dir->prev = NULL;

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
        dir->sym.state = SYM_EXTERNAL;
        dir->e.extinfo = AsmAlloc( sizeof( ext_info ) );
        dir->e.extinfo->idx = 0;
        dir->e.extinfo->use32 = Use32;
        dir->e.extinfo->comm = 0;
        dir->e.extinfo->global = 0;
        break;
    case TAB_COMM:
        dir->sym.state = SYM_EXTERNAL;
        dir->sym.referenced = TRUE;
        dir->e.comminfo = AsmAlloc( sizeof( comm_info ) );
        dir->e.comminfo->idx = 0;
        dir->e.comminfo->use32 = Use32;
        dir->e.comminfo->comm = 1;
        tab = TAB_EXT;
        break;
    case TAB_CONST:
        dir->sym.state = SYM_CONST;
        dir->sym.segment = NULL;
        dir->sym.offset = 0;
        dir->e.constinfo = AsmAlloc( sizeof( const_info ) );
        dir->e.constinfo->data = NULL;
        dir->e.constinfo->count = 0;
        dir->e.constinfo->predef = FALSE;
        return;
    case TAB_PROC:
        dir->sym.state = SYM_PROC;
        dir->e.procinfo = AsmAlloc( sizeof( proc_info ) );
        dir->e.procinfo->regslist = NULL;
        dir->e.procinfo->paralist = NULL;
        dir->e.procinfo->locallist = NULL;
        dir->e.procinfo->labellist = NULL;
        break;
    case TAB_MACRO:
        dir->sym.state = SYM_MACRO;
        dir->e.macroinfo = AsmAlloc( sizeof( macro_info ) );
        dir->e.macroinfo->parmlist = NULL;
        dir->e.macroinfo->data = NULL;
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
        dir->e.structinfo->head = NULL;
        dir->e.structinfo->tail = NULL;
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

static void dir_to_tail( dir_node *dir, int tab )
/***********************************************/
{
    if( dir->next == NULL )
        return;
    if( dir->prev == NULL ) {
        Tables[tab].head = dir->next;
        dir->next->prev = NULL;
    } else {
        dir->prev->next = dir->next;
        dir->next->prev = dir->prev;
    }
    dir->prev = Tables[tab].tail;
    dir->prev->next = dir;
    Tables[tab].tail = dir;
    dir->next = NULL;
}

static dir_node *RemoveFromTable( dir_node *dir )
/***********************************************/
{
    int tab;

    if( dir->prev != NULL && dir->next != NULL ) {
        dir->next->prev = dir->prev;
        dir->prev->next = dir->next;
    } else {
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
        default:
            return( NULL );
        }
        if( dir->next != NULL ) {
            dir->next->prev = NULL;
            Tables[tab].head = dir->next;
        } else if( dir->prev != NULL ) {
            dir->prev->next = NULL;
            Tables[tab].tail = dir->prev;
        } else {
            Tables[tab].head = Tables[tab].tail = NULL;
        }
    }
    return( dir->next );
}

void dir_to_sym( dir_node *dir )
/******************************/
/* Remove node type/info to be symbol only again */
{
    FreeInfo( dir );
    RemoveFromTable( dir);
    dir->sym.state = SYM_UNDEFINED;
}

void dir_change( dir_node *dir, int tab )
/***************************************/
/* Change node type and insert it into the table specified by tab */
{
    FreeInfo( dir );
    RemoveFromTable( dir);
    dir_init( dir, tab );
}

dir_node *dir_insert( const char *name, int tab )
/***********************************************/
/* Insert a node into the table specified by tab */
{
    dir_node            *new;

    /**/myassert( name != NULL );

    /* don't put class lnames into the symbol table - separate name space */
    new = (dir_node *)AllocDSym( name, ( tab != TAB_CLASS_LNAME ) );
    if( new != NULL )
        dir_init( new, tab );
    return( new );
}

void FreeInfo( dir_node *dir )
/****************************/
{
    int i;

    switch( dir->sym.state ) {
    case SYM_GRP:
        {
            seg_list    *segcurr;
            seg_list    *segnext;

            segcurr = dir->e.grpinfo->seglist;
            if( segcurr != NULL ) {
                for( ;; ) {
                    segnext = segcurr->next;
                    AsmFree( segcurr );
                    if( segnext == NULL )
                        break;
                    segcurr = segnext;
                }
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
                && ( dir->e.constinfo->data[0].class != TC_NUM ) ) {
                DebugMsg( ( "freeing const(String): %s = ", dir->sym.name ) );
            } else {
                DebugMsg( ( "freeing const(Number): %s = ", dir->sym.name ) );
            }
#endif
            for( i = 0; i < dir->e.constinfo->count; i++ ) {
#ifdef DEBUG_OUT
                if( dir->e.constinfo->data[i].class == TC_NUM ) {
                    DebugMsg(( "%d ", dir->e.constinfo->data[i].u.value ));
                } else {
                    DebugMsg(( "%s ", dir->e.constinfo->data[i].string_ptr ));
                }
#endif
                AsmFree( dir->e.constinfo->data[i].string_ptr );
            }
            DebugMsg(( "\n" ));
            AsmFree( dir->e.constinfo->data );
            AsmFree( dir->e.constinfo );
        }
        break;
    case SYM_PROC:
        {
            label_list  *labelcurr;
            label_list  *labelnext;
            regs_list   *regcurr;
            regs_list   *regnext;

            labelcurr = dir->e.procinfo->paralist;
            if( labelcurr != NULL ) {
                for( ;; ) {
                    labelnext = labelcurr->next;
                    AsmFree( labelcurr->label );
                    AsmFree( labelcurr->replace );
                    AsmFree( labelcurr );
                    if( labelnext == NULL )
                        break;
                    labelcurr = labelnext;
                }
            }

            labelcurr = dir->e.procinfo->locallist;
            if( labelcurr != NULL ) {
                for( ;; ) {
                    labelnext = labelcurr->next;
                    AsmFree( labelcurr->label );
                    AsmFree( labelcurr->replace );
                    AsmFree( labelcurr );
                    if( labelnext == NULL )
                        break;
                    labelcurr = labelnext;
                }
            }

            labelcurr = dir->e.procinfo->labellist;
            if( labelcurr != NULL ) {
                for( ;; ) {
                    labelnext = labelcurr->next;
                    AsmFree( labelcurr->label );
                    AsmFree( labelcurr->replace );
                    if( labelcurr->sym != NULL )
                        FreeASym( labelcurr->sym );
                    AsmFree( labelcurr );
                    if( labelnext == NULL )
                        break;
                    labelcurr = labelnext;
                }
            }

            regcurr = dir->e.procinfo->regslist;
            if( regcurr != NULL ) {
                for( ;; ) {
                    regnext = regcurr->next;
                    AsmFree( regcurr->reg );
                    AsmFree( regcurr );
                    if( regnext == NULL )
                        break;
                    regcurr = regnext;
                }
            }
            AsmFree( dir->e.procinfo );
        }
        break;
    case SYM_MACRO:
        {
            parm_list       *labelcurr;
            parm_list       *labelnext;
            asmlines        *datacurr;
            asmlines        *datanext;

            /* free the parm list */
            labelcurr = dir->e.macroinfo->parmlist;
            if( labelcurr != NULL ) {
                for( ;; ) {
                    labelnext = labelcurr->next;
                    AsmFree( labelcurr->label );
                    if( labelcurr->replace != NULL ) {
                        AsmFree( labelcurr->replace );
                    }
                    if( labelcurr->def != NULL ) {
                        AsmFree( labelcurr->def );
                    }
                    AsmFree( labelcurr );
                    if( labelnext == NULL )
                        break;
                    labelcurr = labelnext;
                }
            }

            /* free the lines list */
            datacurr = dir->e.macroinfo->data;
            if( datacurr != NULL ) {
                for( ;; ) {
                    datanext = datacurr->next;
                    AsmFree( datacurr->line );
                    AsmFree( datacurr );
                    if( datanext == NULL )
                        break;
                    datacurr = datanext;
                }
            }
            AsmFree( dir->e.macroinfo );
        }
        break;
    case SYM_STRUCT:
        {
            field_list      *ptr;
            field_list      *next;

            for( ptr = dir->e.structinfo->head; ptr != NULL; ptr = next ) {
                AsmFree( ptr->initializer );
                AsmFree( ptr->value );
                if( ptr->sym != NULL )
                    FreeASym( ptr->sym );
                next = ptr->next;
                AsmFree( ptr );
            }
            AsmFree( dir->e.structinfo );
        }
        break;
    default:
        break;
    }
}

static struct asm_sym *InsertClassLname( char *name )
/***************************************************/
{
    dir_node            *dir;

    if( strlen( name ) > MAX_LNAME ) {
        AsmError( LNAME_TOO_LONG );
        return( NULL );
    }
    for( dir = Tables[ TAB_CLASS_LNAME ].head; dir != NULL; dir = dir->next ) {
        if( stricmp( dir->sym.name, name ) == 0 ) {
            return( &dir->sym );
        }
    }
    dir = dir_insert( name, TAB_CLASS_LNAME );
    if( dir == NULL )
        return( NULL );
    AddLnameData( dir );
    return( &dir->sym );
}

void wipe_space( char *token )
/****************************/
/* wipe out the spaces at the beginning of a token */
{
    char        *start;

    if( token == NULL )
        return;
    if( strlen( token ) == 0 )
        return;

    for( start = token;; start++ ){
        if( *start != ' ' && *start != '\t' ) {
            break;
        }
    }
    if( start == token )
        return;

    memmove( token, start, strlen( start ) + 1 );
}

static bool checkword( char **token )
/***********************************/
/* wipes out prceding and tailing spaces, and make sure token contains only
   one word */
{
    char        *ptrhead;
    char        *ptrend;

    /* strip the space in the front */
    for( ptrhead = *token; ; ptrhead++ ) {
        if( ( *ptrhead != ' ' ) && ( *ptrhead != '\t' ) ) {
            break;
        }
    }

    /* Then search for the first ending space */
    ptrend = strchr( ptrhead, ' ' );
    if( ptrend == NULL ) {
        ptrend = strchr( ptrhead, '\t' );
    }
    if( ptrend == NULL ) {
        ptrend = strchr( ptrhead, '\n' );
    }

    /* Check if there is any letters following that ending space */
    if( ptrend != NULL ) {
        *ptrend++ = '\0';
        while( *ptrend != '\0' ) {
            if( ( *ptrend != ' ' ) && ( *ptrend != '\t' ) && ( *ptrend != '\n' ) ) {
                return( TRUE );
            }
            ptrend++;
        }
    }

    *token = ptrhead;
    return( FALSE );
}

uint_32 GetCurrAddr( void )
/*************************/
{
    if( CurrSeg == NULL )
        return( 0 );
    return( CurrSeg->seg->e.seginfo->current_loc );
}

dir_node *GetCurrSeg( void )
/**************************/
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

int CheckForLang( token_idx i )
/*****************************/
{
    char        *token;
    int         lang;

    if( AsmBuffer[i]->class == TC_ID) {
        token = AsmBuffer[i]->string_ptr;
        // look up the type of token
        if( Options.mode & MODE_TASM ) {
            lang = token_cmp( &token, TOK_LANG_NOLANG, TOK_LANG_SYSCALL );
        } else {
            lang = token_cmp( &token, TOK_LANG_BASIC, TOK_LANG_SYSCALL );
        }
        if( lang != ERROR ) {
            return( TypeInfo[lang].value );
        }
    }
    return( ERROR );
}

static int GetLangType( token_idx *i )
/************************************/
{
    int         lang_type;

    lang_type = CheckForLang( *i );
    if( lang_type == ERROR ) {
        return( ModuleInfo.langtype );
    } else {
        (*i)++;
    }
    return( lang_type );
}

bool ExtDef( token_idx i, bool glob_def )
/***************************************/
{
    char                *token;
    char                *mangle_type = NULL;
    char                *typetoken;
    int                 type;
    memtype             mem_type;
    dir_node            *dir;
    int                 lang_type;

    mangle_type = Check4Mangler( &i );
    for( ; i < Token_Count; i++ ) {

        /* get the symbol language type if present */
        lang_type = GetLangType( &i );

        /* get the symbol name */
        token = AsmBuffer[i++]->string_ptr;

        /* go past the colon */
        if( AsmBuffer[i]->class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;

        typetoken = AsmBuffer[i]->string_ptr;
        type = token_cmp( &typetoken, TOK_EXT_NEAR, TOK_EXT_ABS );

        if( type == ERROR ) {
            if( glob_def || !IsLabelStruct( AsmBuffer[i]->string_ptr ) ) {
                AsmError( INVALID_QUALIFIED_TYPE );
                return( RC_ERROR );
            }
            mem_type = MT_STRUCT;
        } else {
            mem_type = TypeInfo[type].value;
        }
        for( ; i< Token_Count && AsmBuffer[i]->class != TC_COMMA; i++ );

        dir = (dir_node *)AsmGetSymbol( token );
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
        } else if( dir->sym.state != SYM_EXTERNAL ) {
            SetMangler( &dir->sym, mangle_type, lang_type );
            if( glob_def && !dir->sym.public ) {
                AddPublicData( dir );
            }
            return( RC_OK );
        } else {
            SetMangler( &dir->sym, mangle_type, lang_type );
            return( RC_OK );
        }

        if( glob_def ) {
            dir->e.extinfo->global = TRUE;
        } else {
            dir->sym.referenced = TRUE;
        }
        GetSymInfo( &dir->sym );
        dir->sym.offset = 0;
        // FIXME !! symbol can have different type
        dir->sym.mem_type = mem_type;
        SetMangler( &dir->sym, mangle_type, lang_type );
    }
    return( RC_OK );
}

static char *Check4Mangler( token_idx *i )
/****************************************/
{
    char *mangle_type = NULL;

    if( AsmBuffer[*i]->class == TC_STRING ) {
        mangle_type = AsmBuffer[*i]->string_ptr;
        (*i)++;
        if( AsmBuffer[*i]->class != TC_COMMA ) {
            AsmWarn( 2, EXPECTING_COMMA );
        } else {
            (*i)++;
        }
    }
    return( mangle_type );
}

bool PubDef( token_idx i )
/************************/
{
    char                *mangle_type = NULL;
    char                *token;
    dir_node            *dir;
    int                 lang_type;

    mangle_type = Check4Mangler( &i );
    for( ; i < Token_Count; i += 2 ) {

        /* get the symbol language type if present */
        lang_type = GetLangType( &i );

        /* get the symbol name */
        token = AsmBuffer[i]->string_ptr;

        /* Add the public name */
        if( checkword( &token ) ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }

        dir = (dir_node *)AsmGetSymbol( token );
        if( dir == NULL ) {
            dir = (dir_node *)AllocDSym( token, TRUE );
            AddPublicData( dir );
        } else if( dir->sym.state == SYM_CONST ) {
            /* check if the symbol expands to another symbol,
             * and if so, expand it */
            if( dir->e.constinfo->data[0].class == TC_ID ) {
                ExpandTheWorld( i, FALSE, TRUE );
                return( PubDef( i ) );
            }
        }
        SetMangler( &dir->sym, mangle_type, lang_type );
        if( !dir->sym.public ) {
            /* put it into the pub table */
            AddPublicData( dir );
        }
    }
    return( RC_OK );
}

static dir_node *CreateGroup( char *name )
/****************************************/
{
    dir_node    *grp;

    grp = (dir_node *)AsmGetSymbol( name );

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

bool GrpDef( token_idx i )
/************************/
{
    char        *name;
    dir_node    *grp;
    seg_list    *new;
    seg_list    *curr;
    dir_node    *seg;
    token_idx   n;

    if( Options.mode & MODE_IDEAL ) {
        n = i + 1;
    } else if( i > 0 ) {
        n = --i;
    } else {
        n = INVALID_IDX;
    }
    if( ( n == INVALID_IDX ) || ( AsmBuffer[n]->class != TC_ID ) ) {    /* name present? */
        AsmError( GRP_NAME_MISSING );
        return( RC_ERROR );
    }
    name = AsmBuffer[n]->string_ptr;
    grp = CreateGroup( name );
    if( grp == NULL )
        return( RC_ERROR );

    for( i += 2;            // skip over the GROUP directive
         i < Token_Count;   // stop at the end of the line
         i += 2 ) {         // skip over commas
        name = AsmBuffer[i]->string_ptr;
        /* Add the segment name */

        if( checkword( &name ) ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }

        seg = (dir_node *)AsmGetSymbol( name );
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
        /* set the grp index of the segment */
        seg->e.seginfo->group = &grp->sym;

        new = AsmAlloc( sizeof(seg_list) );
        new->seg = seg;
        new->next = NULL;
        grp->e.grpinfo->numseg++;

        /* insert the segment at the end of linked list */
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
        Use32 = ModuleInfo.defUse32;
    } else {
        Globals.code_seg = SEGISCODE( CurrSeg );
        Use32 = CurrSeg->seg->e.seginfo->use_32;
        if( Use32 && ( ( Code->info.cpu & P_CPU_MASK ) < P_386 ) ) {
            AsmError( WRONG_CPU_FOR_32BIT_SEGMENT );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

bool SetUse32Def( bool flag )
/***************************/
{
    if( ( CurrSeg == NULL )               // outside any segments
        && ( ModuleInfo.model == MOD_NONE             // model not defined
            || ModuleInfo.cmdline ) ) {   // model defined on cmdline by -m?
        ModuleInfo.defUse32 = flag;
    }
    return( SetUse32() );
}

bool SetCurrSeg( token_idx i )
/****************************/
{
    char        *name;
    dir_node    *seg;

    switch( AsmBuffer[i]->u.token ) {
    case T_SEGMENT:
        FlushCurrSeg();
        if( Options.mode & MODE_IDEAL ) {
            name = AsmBuffer[i + 1]->string_ptr;
        } else {
            name = AsmBuffer[i-1]->string_ptr;
        }
        seg = (dir_node *)AsmGetSymbol( name );
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

static int token_cmp( char **token, int start, int end )
/******************************************************/
/* compare token against those specified in TypeInfo[ start...end ] */
{
    int         i;
    char        *str;
    char        *tkn;

    str = *token;

    for( i = start; i <= end; i++ ) {
        tkn = TypeInfo[i].string;
        if( tkn == NULL )
            continue;
        if( stricmp( tkn, str ) == 0 ) {
            // type is found
            return( i );
        }
    }
    return( ERROR );        // No type is found
}

static seg_type ClassNameType( char *name )
/*****************************************/
{
    size_t  slen;
    char    uname[257];

    if( name == NULL )
        return( SEGTYPE_UNDEF );
    if( ModuleInfo.model != MOD_NONE ) {
        if( stricmp( name, Options.code_class ) == 0 ) {
            return( SEGTYPE_ISCODE );
        }
    }
    slen = strlen( name );
    strcpy( uname, name );
    strupr( uname );
    switch( slen ) {
    default:
    case 5:
        // 'CONST'
        if( memcmp( uname, "CONST", 6 ) == 0 )
            return( SEGTYPE_ISDATA );
        // 'STACK'
        if( memcmp( uname, "STACK", 6 ) == 0 )
            return( SEGTYPE_ISDATA );
        // 'DBTYP'
        if( memcmp( uname, "DBTYP", 6 ) == 0 )
            return( SEGTYPE_ISDATA );
        // 'DBSYM'
        if( memcmp( uname, "DBSYM", 6 ) == 0 )
            return( SEGTYPE_ISDATA );
    case 4:
        // 'CODE'
        if( memcmp( uname , "CODE", 5 ) == 0 )
            return( SEGTYPE_ISCODE );
        // '...DATA'
        if( memcmp( uname + slen - 4, "DATA", 4 ) == 0 )
            return( SEGTYPE_ISDATA );
    case 3:
        // '...BSS'
        if( memcmp( uname + slen - 3, "BSS", 3 ) == 0 )
            return( SEGTYPE_ISDATA );
    case 2:
    case 1:
    case 0:
        return( SEGTYPE_UNDEF );
    }
}

static seg_type SegmentNameType( char *name )
/*******************************************/
{
    size_t  slen;
    char    uname[257];

    slen = strlen( name );
    strcpy( uname, name );
    strupr( uname );
    switch( slen ) {
    default:
    case 5:
        // '..._TEXT'
        if( memcmp( uname + slen - 5, DEFAULT_CODE_NAME, 5 ) == 0 )
            return( SEGTYPE_ISCODE );
        // '..._DATA'
        if( memcmp( uname + slen - 5, DEFAULT_DATA_NAME, 5 ) == 0 )
            return( SEGTYPE_ISDATA );
        // 'CONST'
        if( memcmp( uname, "CONST", 5 ) == 0 )
            return( SEGTYPE_ISDATA );
    case 4:
        // '..._BSS'
        if( memcmp( uname + slen - 4, "_BSS", 4 ) == 0 )
            return( SEGTYPE_ISDATA );
    case 3:
    case 2:
    case 1:
    case 0:
        return( SEGTYPE_UNDEF );
    }
}


bool SegDef( token_idx i )
/************************/
{
    char                *token;
    seg_info            *new;
    seg_info            *old;
    uint                type;       // type of option
    uint                initstate;  // to show if a field is initialized
    dir_node            *seg;
    char                *name;
    token_idx           n;

    if( Options.mode & MODE_IDEAL ) {
        n = i + 1;
        if( ( AsmBuffer[n]->class == TC_DIRECTIVE ) &&
            ( ( AsmBuffer[n]->u.token == T_STACK ) ||
              ( AsmBuffer[n]->u.token == T_CONST ) ) )
            AsmBuffer[n]->class = TC_ID;
        if( ( AsmBuffer[i]->u.token == T_SEGMENT ) &&
            ( AsmBuffer[n]->class != TC_ID ) ) {
            AsmError( SEG_NAME_MISSING );
            return( RC_ERROR );
        }
    } else {
        if( i > 0 ) {
            n = i - 1;
        } else {
            n = INVALID_IDX;
        }
        if( ( n == INVALID_IDX ) || ( AsmBuffer[n]->class != TC_ID ) ) {
            AsmError( SEG_NAME_MISSING );
            return( RC_ERROR );
        }
    }
    name = AsmBuffer[n]->string_ptr;
    switch( AsmBuffer[i]->u.token ) {
    case T_SEGMENT:
        /* Check to see if the segment is already defined */
        seg = (dir_node *)AsmGetSymbol( name );
        if( seg == NULL ) {
            // segment is not defined
            seg = dir_insert( name, TAB_SEG );
            old = new = seg->e.seginfo;
        } else if ( seg->sym.state == SYM_SEG ) {
            // segment already defined
            old = seg->e.seginfo;
            if( seg->sym.segment == NULL ) {
                // segment was mentioned in a group statement, but not really set up
                new = old;
            } else {
                new = AsmAlloc( sizeof( seg_info ) );
            }
        } else {
            // symbol is different kind, change to segment
            dir_change( seg, TAB_SEG );
            old = new = seg->e.seginfo;
        }

        /* Setting up default values */
        if( new == old ) {
            new->align = ALIGN_PARA;
            new->combine = COMB_INVALID;
            new->use_32 = ModuleInfo.defUse32;
            new->class_name = InsertClassLname( "" );
            new->readonly = FALSE;
            new->iscode = SEGTYPE_UNDEF;
        } else {
            new->readonly = old->readonly;
            new->iscode = old->iscode;
            new->align = old->align;
            new->combine = old->combine;
            if( !old->ignore ) {
                new->use_32 = old->use_32;
            } else {
                new->use_32 = ModuleInfo.defUse32;
            }
            new->class_name = old->class_name;
        }
        new->ignore = FALSE; // always false unless set explicitly
        new->length = 0;

        if( lastseg.stack_size > 0 ) {
            new->length = lastseg.stack_size;
            lastseg.stack_size = 0;
        }

        initstate = 0;
        if( Options.mode & MODE_IDEAL ) {
            i += 2;     /* Go past SEGMENT and name */
        } else {
            i++;        /* Go past SEGMENT */
        }
        for( ; i < Token_Count; i ++ ) {
            if( AsmBuffer[i]->class == TC_STRING ) {

                /* the class name - the only token which is of type STRING */
                token = AsmBuffer[i]->string_ptr;
                new->class_name = InsertClassLname( token );
                if( new->class_name == NULL ) {
                    goto error;
                }
                continue;
            }

            /* go through all tokens EXCEPT the class name */
            token = AsmBuffer[i]->string_ptr;

            // look up the type of token
            type = token_cmp( &token, TOK_READONLY, TOK_AT );
            if( type == ERROR ) {
                AsmError( UNDEFINED_SEGMENT_OPTION );
                goto error;
            }

            /* initstate is used to check if any field is already
            initialized */

            if( initstate & TypeInfo[type].init ) {
                AsmError( SEGMENT_PARA_DEFINED ); // initialized already
                goto error;
            } else {
                initstate |= TypeInfo[type].init; // mark it initialized
            }
            switch( type ) {
            case TOK_READONLY:
                new->readonly = TRUE;
                break;
            case TOK_BYTE:
            case TOK_WORD:
            case TOK_DWORD:
            case TOK_PARA:
            case TOK_PAGE:
                new->align = (uint_8)TypeInfo[type].value;
                break;
            case TOK_PRIVATE:
            case TOK_PUBLIC:
            case TOK_STACK:
            case TOK_COMMON:
            case TOK_MEMORY:
                new->combine = (uint_8)TypeInfo[type].value;
                break;
            case TOK_USE16:
            case TOK_USE32:
                new->use_32 = ( TypeInfo[type].value != 0 );
                break;
            case TOK_IGNORE:
                new->ignore = TRUE;
                break;
            case TOK_AT:
                new->align = SEGDEF_ALIGN_ABS;
                ExpandTheWorld( i + 1, FALSE, TRUE);
                if( AsmBuffer[i + 1]->class == TC_NUM ) {
                    i++;
                    new->abs_frame = (uint_16)AsmBuffer[i]->u.value;
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
        token = new->class_name->name;
        if( new->iscode != SEGTYPE_ISCODE ) {
            seg_type res;

            res = ClassNameType( token );
            if( res != SEGTYPE_UNDEF ) {
                new->iscode = res;
            } else {
                res = SegmentNameType( name );
                new->iscode = res;
            }
        }

        if( new == old ) {
            /* A new definition */
            new->start_loc = 0;
            new->current_loc = 0;
            seg->sym.segment = &seg->sym;
            seg->sym.offset = 0;
            dir_to_tail( seg, TAB_SEG );
            AddLnameData( seg );
        } else if( !old->ignore && !new->ignore ) {
            /* Check if new definition is different from previous one */
            if( ( old->readonly != new->readonly )
                || ( old->align != new->align )
                || ( old->combine != new->combine )
                || ( old->use_32 != new->use_32 )
                || ( old->class_name != new->class_name ) ) {
                AsmError( SEGDEF_CHANGED );
                goto error;
            } else {
                // definition is the same as before
            }
        } else if( old->ignore ) {
            /* reset to the new values */
            old->align = new->align;
            old->combine = new->combine;
            old->readonly = new->readonly;
            old->iscode = new->iscode;
            old->ignore = new->ignore;
            old->use_32 = new->use_32;
            old->class_name = new->class_name;
        } else {    // new->ignore
            /* keep the old values */
            new->align = old->align;
            new->combine = old->combine;
            new->readonly = old->readonly;
            new->iscode = old->iscode;
            new->ignore = old->ignore;
            new->use_32 = old->use_32;
            new->class_name = old->class_name;
        }
        if( !ModuleInfo.mseg && ( seg->e.seginfo->use_32 != ModuleInfo.use32 ) ) {
            ModuleInfo.mseg = TRUE;
        }
        if( seg->e.seginfo->use_32 ) {
            ModuleInfo.use32 = TRUE;
        }
        if( new != old )
            AsmFree( new );
        if( push_seg( seg ) ) {
            return( RC_ERROR );
        }
        break;
    case T_ENDS:
        if( CurrSeg == NULL ) {
            AsmError( SEGMENT_NOT_OPENED );
            return( RC_ERROR );
        }
        if( AsmBuffer[n]->class == TC_ID ) {
            seg = (dir_node *)AsmGetSymbol( name );
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
    if( new != old )
        AsmFree( new );
    return( RC_ERROR );
}

bool Include( token_idx i )
/*************************/
{
    switch( AsmBuffer[i]->class ) {
    case TC_ID:
    case TC_STRING:
    case TC_PATH:
        return( InputQueueFile( AsmBuffer[i]->string_ptr ) );
    default:
        AsmError( EXPECTED_FILE_NAME );
        return( RC_ERROR );
    }
}

bool IncludeLib( token_idx i )
/****************************/
{
    char *name;
    struct asm_sym *sym;

    name = AsmBuffer[i]->string_ptr;
    if( name == NULL ) {
        AsmError( LIBRARY_NAME_MISSING );
        return( RC_ERROR );
    }

    sym = AsmGetSymbol( name );
    if( sym == NULL ) {
        // fixme
        if( dir_insert( name, TAB_LIB ) == NULL ) {
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

static void input_group( asm_token type, char *name )
/***************************************************/
/* emit any GROUP instruction */
{
    char        buffer[MAX_LINE_LEN];

    if( ModuleInfo.model == MOD_FLAT )
        return;
    if( Options.mode & MODE_IDEAL ) {
        strcpy( buffer, "GROUP DGROUP " );
    } else {
        strcpy( buffer, "DGROUP GROUP " );
    }
    if( name != NULL ) {
        strcat( buffer, name );
    } else {
        switch( type ) {
        case T_DOT_DATA:
        case T_DATASEG:
            strcat( buffer, DEFAULT_DATA_NAME );
            break;
        case T_DOT_DATA_UN:
        case T_UDATASEG:
            strcat( buffer, "_BSS" );
            break;
        case T_DOT_CONST:
        case T_CONST:
            strcat( buffer, "CONST" );
            break;
        case T_DOT_STACK:
        case T_STACK:
            strcat( buffer, "STACK" );
            break;
        }
    }
    InputQueueLine( buffer );
}

static void close_lastseg( void )
/*******************************/
/* close the last opened simplified segment */
{
    if( lastseg.seg != SIM_NONE ) {
        lastseg.seg = SIM_NONE;
        if( ( CurrSeg == NULL ) && ( *lastseg.close != '\0' ) ) {
            AsmError( DONT_MIX_SIM_WITH_REAL_SEGDEFS );
            return;
        }
        InputQueueLine( lastseg.close );
    }
}

bool Startup( token_idx i )
/*************************/
{
    /* handles .STARTUP/STARTUPCODE and .EXIT/EXITCODE directives */

    int         count;
    char        buffer[ MAX_LINE_LEN ];

    if( ModuleInfo.model == MOD_NONE ) {
        AsmError( MODEL_IS_NOT_DECLARED );
        return( RC_ERROR );
    }
    ModuleInfo.cmdline = FALSE;

    switch( AsmBuffer[i]->u.token ) {
    case T_DOT_STARTUP:
    case T_STARTUPCODE:
        count = 0;
        strcpy( buffer, StartAddr );
        strcat( buffer, ":" );
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
        StartupDirectiveFound = TRUE;
        break;
    case T_DOT_EXIT:
    case T_EXITCODE:
        i++;
        if( ( AsmBuffer[i]->string_ptr != NULL )
            && ( *(AsmBuffer[i]->string_ptr) != '\0' ) ) {
            strcpy( buffer, RetVal );
            strcat( buffer, AsmBuffer[i]->string_ptr );
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

static char *get_sim_code_beg( char *buffer, char *name, int bit )
/****************************************************************/
{
    if( Options.mode & MODE_IDEAL ) {
        strcpy( buffer, "SEGMENT " );
        strcat( buffer, name );
    } else {
        strcpy( buffer, name );
        strcat( buffer, " SEGMENT" );
    }
    if( bit == BIT16 ) {
        strcat( buffer, " WORD PUBLIC '" );
    } else {
        strcat( buffer, " DWORD USE32 PUBLIC '" );
    }
    strcat( buffer, Options.code_class );
    strcat( buffer, "' IGNORE");
    return( buffer );
}

static char *get_sim_code_end( char *buffer, char *name )
/*******************************************************/
{
    if( Options.mode & MODE_IDEAL ) {
        strcpy( buffer, "ENDS ");
        strcat( buffer, name );
    } else {
        strcpy( buffer, name );
        strcat( buffer, " ENDS");
    }
    return( buffer );
}

bool SimSeg( token_idx i )
/************************/
/* Handles simplified segment, based on optasm pg. 142-146 */
{
    char        buffer[ MAX_LINE_LEN ];
    int         bit;
    char        *string;
    asm_token   type;
    int         seg;
    int         ideal;

    if( ModuleInfo.model == MOD_NONE ) {
        AsmError( MODEL_IS_NOT_DECLARED );
        return( RC_ERROR );
    }
    ModuleInfo.cmdline = FALSE;
    close_lastseg();
    buffer[0] = '\0';
    bit = ( ModuleInfo.defUse32 ) ? BIT32 : BIT16;
    type = AsmBuffer[i]->u.token;
    i++; /* get past the directive token */
    if( i < Token_Count ) {
        string = AsmBuffer[i]->string_ptr;
    } else {
        string = NULL;
    }
    ideal = ( Options.mode & MODE_IDEAL ) ? 1 : 0;
    switch( type ) {
    case T_DOT_CODE:
    case T_CODESEG:
        if( string == NULL )
            string = Options.text_seg;
        strcpy( code_segment_name, string );
        InputQueueLine( get_sim_code_beg( buffer, string, bit ) );
        get_sim_code_end( lastseg.close, string );
        lastseg.seg = SIM_CODE;

        if( ModuleInfo.model == MOD_TINY ) {
            InputQueueLine( "ASSUME CS:DGROUP" );
        } else {
            strcpy( buffer, "ASSUME CS:" );
            strcat( buffer, string );
            InputQueueLine( buffer );
        }
        break;
    case T_DOT_STACK:
    case T_STACK:
        InputQueueLine( SimCodeBegin[ideal][bit][SIM_STACK] );
        input_group( type, NULL );
        InputQueueLine( SimCodeEnd[ideal][SIM_STACK] );
        if( i < Token_Count ) {
            if( AsmBuffer[i]->class != TC_NUM ) {
                AsmError( CONSTANT_EXPECTED );
                return( RC_ERROR );
            } else {
                lastseg.stack_size = (int_16)AsmBuffer[i]->u.value;
            }
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
        if( ( type == T_DOT_DATA ) || ( type == T_DATASEG ) ) {
            if( string == NULL )
                string = Options.data_seg;
            seg = SIM_DATA;
        } else if( ( type == T_DOT_DATA_UN ) || ( type == T_UDATASEG ) ) {
            seg = SIM_DATA_UN;
            string = NULL;
        } else {
            seg = SIM_CONST;
            string = NULL;
        }

        if( string != NULL ) {
            if( ideal ) {
                strcpy( buffer, "SEGMENT " );
                strcat( buffer, string );
                strcat( buffer, SimCodeBegin[ideal][bit][seg] + SIM_DATA_OFFSET + 8 );
            } else {
                strcpy( buffer, string );
                strcat( buffer, SimCodeBegin[ideal][bit][seg] + SIM_DATA_OFFSET  );
            }
        } else {
            strcpy( buffer, SimCodeBegin[ideal][bit][seg] );
        }
        InputQueueLine( buffer );
        InputQueueLine( "ASSUME CS:ERROR" );
        input_group( type, string );
        lastseg.seg = seg;
        if( string != NULL ) {
            if( ideal ) {
                strcpy( lastseg.close, "ENDS " );
                strcat( lastseg.close, string );
            } else {
                strcpy( lastseg.close, string );
                strcat( lastseg.close, " ENDS" );
            }
        } else {
            strcpy( lastseg.close, SimCodeEnd[ideal][seg] );
        }
        break;
    case T_DOT_FARDATA:
    case T_DOT_FARDATA_UN:  // .fardata?
    case T_FARDATA:
    case T_UFARDATA:
        seg = ( ( type == T_DOT_FARDATA ) ||
                ( type == T_FARDATA )  ) ? SIM_FARDATA : SIM_FARDATA_UN;
        if( string != NULL ) {
            if( ideal ) {
                strcpy( buffer, "SEGMENT " );
                strcat( buffer, string );
                strcat( buffer, SimCodeBegin[ideal][bit][seg] + SIM_FARDATA_OFFSET + 8 );
            } else {
                strcpy( buffer, string );
                strcat( buffer, SimCodeBegin[ideal][bit][seg] + SIM_FARDATA_OFFSET  );
            }
        } else {
            strcpy( buffer, SimCodeBegin[ideal][bit][seg] );
        }
        InputQueueLine( buffer );
        InputQueueLine( "ASSUME CS:ERROR" );
        if( string != NULL ) {
            if( ideal ) {
                strcpy( lastseg.close, "ENDS " );
                strcat( lastseg.close, string );
            } else {
                strcpy( lastseg.close, string );
                strcat( lastseg.close, " ENDS" );
            }
        } else {
            strcpy( lastseg.close, SimCodeEnd[ideal][seg] );
        }
        lastseg.seg = seg;
        break;
    default:
        /**/myassert( 0 );
        break;
    }
    return( RC_OK );
}

static void module_prologue( int type )
/*************************************/
/* Generates codes for .MODEL; based on optasm pg.142-146 */
{
    int         bit, ideal;
    char        buffer[ MAX_LINE_LEN ];

    if( type == MOD_FLAT ) {
        const_data.string_ptr = "FLAT";
    } else {
        const_data.string_ptr = "DGROUP";
    }
    AddPredefinedConstant( "@data", &info_data );
    strcpy( code_segment_name, Options.text_seg );
    AddPredefinedConstant( "@code", &info_code );

    bit = ( ModuleInfo.defUse32 ) ? BIT32 : BIT16;
    ideal = ( Options.mode & MODE_IDEAL ) ? 1 : 0;

    /* Generates codes for code segment */
    InputQueueLine( get_sim_code_beg( buffer, Options.text_seg, bit ) );
    InputQueueLine( get_sim_code_end( buffer, Options.text_seg ) );

    /* Generates codes for data segment */
    InputQueueLine( SimCodeBegin[ideal][bit][SIM_DATA] );
    InputQueueLine( SimCodeEnd[ideal][SIM_DATA] );

    if( type != MOD_FLAT ) {
        /* Generates codes for grouping */
        if( ideal )
            strcpy( buffer, "GROUP DGROUP " );
        else
            strcpy( buffer, "DGROUP GROUP " );
        if( type == MOD_TINY ) {
            strcat( buffer, Options.text_seg );
            strcat( buffer, "," );
        }
        strcat( buffer, Options.data_seg );
        InputQueueLine( buffer );
    }

    ModelAssumeInit();

// FIXME !!
// it is temporary fix for PROC parameters
// but PTR operator should be resolved globaly
    /* Fix up PTR size */
    switch( type ) {
    case MOD_COMPACT:
    case MOD_LARGE:
    case MOD_HUGE:
    case MOD_FLAT:
        TypeInfo[TOK_EXT_PTR].value = MT_DWORD;
        break;
    }

    /* Set @CodeSize */
    switch( type ) {
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

    /* Set @DataSize */
    switch( type ) {
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

    /* Set @Model */
    const_Model.u.value = ModuleInfo.model;
    AddPredefinedConstant( "@Model", &info_Model );

    /* Set @Interface */
    const_Interface.u.value = ModuleInfo.langtype;
    AddPredefinedConstant( "@Interface", &info_Interface );
}

void ModuleInit( void )
/*********************/
{
    ModuleInfo.model = MOD_NONE;
    ModuleInfo.distance = STACK_NONE;
    ModuleInfo.langtype = LANG_NONE;
    ModuleInfo.ostype = OPSYS_DOS;
    ModuleInfo.use32 = FALSE;
    ModuleInfo.defUse32 = FALSE;
    ModuleInfo.cmdline = FALSE;
    ModuleInfo.mseg = FALSE;
    ModuleInfo.flat_grp = NULL;
    *ModuleInfo.name = 0;
    // add source file to autodependency list
    ModuleInfo.srcfile = AddFlist( AsmFiles.fname[ASM] );
}

static void get_module_name( void )
/*********************************/
{
    char dummy[_MAX_EXT];
    char        *p;

    /**/myassert( AsmFiles.fname[ASM] != NULL );
    _splitpath( AsmFiles.fname[ASM], NULL, NULL, ModuleInfo.name, dummy );
    for( p = ModuleInfo.name; *p != '\0'; ++p ) {
        if( !( isalnum( *p ) || ( *p == '_' ) || ( *p == '$' )
            || ( *p == '@' ) || ( *p == '?') ) ) {
            /* it's not a legal character for a symbol name */
            *p = '_';
        }
    }
    /* first character can't be a number either */
    if( isdigit( ModuleInfo.name[0] ) ) {
        ModuleInfo.name[0] = '_';
    }
}

static void set_def_seg_name( void )
/***********************************/
{
    size_t  len;

    /* set Options.code_class */
    if( Options.code_class == NULL ) {
        Options.code_class = AsmAlloc( sizeof( DEFAULT_CODE_CLASS ) + 1 );
        strcpy( Options.code_class, DEFAULT_CODE_CLASS );
    }
    /* set Options.text_seg based on module name */
    if( Options.text_seg == NULL ) {
        switch( ModuleInfo.model ) {
        case MOD_MEDIUM:
        case MOD_LARGE:
        case MOD_HUGE:
            len = strlen( ModuleInfo.name ) + sizeof( DEFAULT_CODE_NAME ) + 1;
            Options.text_seg = AsmAlloc( len );
            strcpy( Options.text_seg, ModuleInfo.name );
            strcat( Options.text_seg, DEFAULT_CODE_NAME );
            break;
        default:
            Options.text_seg = AsmAlloc( sizeof( DEFAULT_CODE_NAME ) + 1 );
            strcpy( Options.text_seg, DEFAULT_CODE_NAME );
            break;
        }
    }
    /* set Options.data_seg */
    if( Options.data_seg == NULL ) {
        Options.data_seg = AsmAlloc( sizeof( DEFAULT_DATA_NAME ) + 1 );
        strcpy( Options.data_seg, DEFAULT_DATA_NAME );
    }
    return;
}

void DefFlatGroup( void )
/***********************/
{
    if( MAGIC_FLAT_GROUP == NULL ) {
        MAGIC_FLAT_GROUP = &CreateGroup( "FLAT" )->sym;
    }
}

bool Model( token_idx i )
/***********************/
{
    char        *token;
    int         initstate = 0;
    uint        type;           // type of option

    if( Parse_Pass != PASS_1 ) {
        ModelAssumeInit();
        return( RC_OK );
    }

    if( ModuleInfo.model != MOD_NONE && !ModuleInfo.cmdline ) {
        AsmError( MODEL_DECLARED_ALREADY );
        return( RC_ERROR );
    }
    ModuleInfo.cmdline = FALSE;

    get_module_name();

    if( Options.mode & MODE_IDEAL ) {
        token = AsmBuffer[i + 1]->string_ptr;
        wipe_space( token );
        type = token_cmp( &token, TOK_USE16, TOK_USE32 );
        if( type != ERROR ) {
            SetUse32Def( TypeInfo[type].value != 0 );
            i++;
        }
    }

    for( i++; i < Token_Count; i++ ) {

        token = AsmBuffer[i]->string_ptr;
        wipe_space( token );
        /* Add the public name */

        // look up the type of token
        type = token_cmp( &token, TOK_TINY, TOK_FARSTACK );
        if( type == ERROR ) {
            if( Options.mode & MODE_TASM ) {
                type = token_cmp( &token, TOK_LANG_NOLANG, TOK_LANG_SYSCALL );
            } else {
                type = token_cmp( &token, TOK_LANG_BASIC, TOK_LANG_SYSCALL );
            }
            if( type == ERROR ) {
                type = token_cmp( &token, TOK_OS_OS2, TOK_OS_DOS );
                if( type == ERROR ) {
                    AsmError( UNDEFINED_MODEL_OPTION );
                    return( RC_ERROR );
                }
            }
        }
        MakeConstantUnderscored( token );

        if( initstate & TypeInfo[type].init ) {
            AsmError( MODEL_PARA_DEFINED ); // initialized already
            return( RC_ERROR );
        } else {
            initstate |= TypeInfo[type].init; // mark it initialized
        }
        switch( type ) {
        case TOK_FLAT:
            DefFlatGroup();
            SetUse32Def( TRUE );
            // fall through
        case TOK_TINY:
        case TOK_SMALL:
        case TOK_COMPACT:
        case TOK_MEDIUM:
        case TOK_LARGE:
        case TOK_HUGE:
            ModuleInfo.model = TypeInfo[type].value;
            set_def_seg_name();
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

        /* go past comma */
        if( ( i < Token_Count ) && ( AsmBuffer[i]->class != TC_COMMA ) ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }
    }

    if( ( initstate & INIT_MEMORY ) == 0 ) {
        AsmError( MEMORY_NOT_FOUND );
        return( RC_ERROR );
    }

    module_prologue( ModuleInfo.model );
    lastseg.seg = SIM_NONE;
    lastseg.stack_size = 0;
    ModuleInfo.cmdline = (LineNumber == 0);
    return( RC_OK );
}

void AssumeInit( void )
/*********************/
{
    enum assume_reg reg;

    for( reg = ASSUME_FIRST; reg < ASSUME_LAST; reg++ ) {
        AssumeTable[reg].symbol = NULL;
        AssumeTable[reg].error = FALSE;
        AssumeTable[reg].flat = FALSE;
    }
}

static void ModelAssumeInit( void )
/**********************************/
{
    char        buffer[ MAX_LINE_LEN ];

    /* Generates codes for assume */
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
        strcpy( buffer, "ASSUME CS:" );
        strcat( buffer, Options.text_seg );
        strcat( buffer, ", DS:DGROUP, SS:DGROUP" );
        InputQueueLine( buffer );
        break;
    }
}

bool SetAssume( token_idx i )
/***************************/
/* Handles ASSUME statement */
{
    char            *token;
    char            *segloc;
    int             reg;
    assume_info     *info;
    struct asm_sym  *sym;


    for( i++; i < Token_Count; i++ ) {

        token = AsmBuffer[i]->string_ptr;
        wipe_space( token );
        if( token_cmp( &token, TOK_NOTHING, TOK_NOTHING ) != ERROR ) {
            AssumeInit();
            continue;
        }

        i++;

        if( AsmBuffer[i]->class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;

        if( ( AsmBuffer[i]->class == TC_UNARY_OPERATOR )
            && ( AsmBuffer[i]->u.token == T_SEG ) ) {
            i++;
        }

        segloc = AsmBuffer[i]->string_ptr;
        i++;
        if( *segloc == '\0' ) {
            AsmError( SEGLOCATION_EXPECTED );
            return( RC_ERROR );
        }

        /*---- Now store the information ----*/

        reg = token_cmp( &token, TOK_DS, TOK_CS );
        if( reg == ERROR ) {
            AsmError( INVALID_REGISTER );
            return( RC_ERROR );
        }
        if( ( ( Code->info.cpu & P_CPU_MASK ) < P_386 )
            && ( ( reg == TOK_FS ) || ( reg == TOK_GS ) ) ) {
            AsmError( INVALID_REGISTER );
            return( RC_ERROR );
        }

        info = &(AssumeTable[TypeInfo[reg].value]);

        if( token_cmp( &segloc, TOK_ERROR, TOK_ERROR ) != ERROR ) {
            info->error = TRUE;
            info->flat = FALSE;
            info->symbol = NULL;
        } else if( token_cmp( &segloc, TOK_FLAT, TOK_FLAT ) != ERROR ) {
            DefFlatGroup();
            info->flat = TRUE;
            info->error = FALSE;
            info->symbol = NULL;
        } else if( token_cmp( &segloc, TOK_NOTHING, TOK_NOTHING ) != ERROR ) {
            info->flat = FALSE;
            info->error = FALSE;
            info->symbol = NULL;
        } else {
            sym = AsmLookup( segloc );
            if( sym == NULL )
                return( RC_ERROR );
            if ( ( Parse_Pass != PASS_1 ) && ( sym->state == SYM_UNDEFINED ) ) {
                AsmErr( SYMBOL_NOT_DEFINED, segloc );
                return( RC_ERROR );
            }
            info->symbol = sym;
            info->flat = FALSE;
            info->error = FALSE;
        }

        /* go past comma */
        if( ( i < Token_Count ) && ( AsmBuffer[i]->class != TC_COMMA ) ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

extern struct asm_sym *GetGrp( struct asm_sym *sym )
/**************************************************/
/* get ptr to sym's group sym */
{
    dir_node            *curr;

    curr = GetSeg( sym );
    if( curr != NULL )
        return( curr->e.seginfo->group );
    return( NULL );
}

bool SymIs32( struct asm_sym *sym )
/********************************/
/* get sym's segment size */
{
    dir_node            *curr;

    curr = GetSeg( sym );
    if( curr == NULL ) {
        if( sym->state == SYM_EXTERNAL ) {
            if( ModuleInfo.mseg ) {
                curr = (dir_node *)sym;
                return( curr->e.extinfo->use32 );
            } else {
                return( ModuleInfo.use32 );
            }
        }
    } else {
        return( curr->e.seginfo->use_32 );
    }
    return( RC_OK );
}

bool FixOverride( token_idx index )
/*********************************/
/* Fix segment or group override */
{
    struct asm_sym      *sym;

    sym = AsmLookup( AsmBuffer[index]->string_ptr );
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

static enum assume_reg search_assume( struct asm_sym *sym,
                         enum assume_reg def, int override )
/**********************************************************/
{
    if( sym == NULL )
        return( ASSUME_NOTHING );
    if( def != ASSUME_NOTHING ) {
        if( AssumeTable[def].symbol != NULL ) {
            if( AssumeTable[def].symbol == sym )
                return( def );
            if( !override && ( AssumeTable[def].symbol == GetGrp( sym ) ) ) {
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
int Use32Assume( enum assume_reg prefix )
/***************************************/
{
    dir_node        *dir;
    seg_list        *seg_l;
    struct asm_sym  *sym_assume;

    if( AssumeTable[prefix].flat )
        return( 1 );
    sym_assume = AssumeTable[prefix].symbol;
    if( sym_assume == NULL )
        return( EMPTY );
    if( sym_assume->state == SYM_SEG ) {
        dir = (dir_node *)sym_assume;
        return( dir->e.seginfo->use_32 );
    } else if( sym_assume->state == SYM_GRP ) {
        dir = (dir_node *)sym_assume;
        seg_l = dir->e.grpinfo->seglist;
        dir = seg_l->seg;
        return( dir->e.seginfo->use_32 );
    }
    return( EMPTY );
}
#endif

enum assume_reg GetPrefixAssume( struct asm_sym *sym, enum assume_reg prefix )
/****************************************************************************/
{
    struct asm_sym  *sym_assume;

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
        } else {
            return( ASSUME_NOTHING );
        }
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
    } else {
        return( ASSUME_NOTHING );
    }
}

enum assume_reg GetAssume( struct asm_sym *sym, enum assume_reg def )
/*******************************************************************/
{
    enum assume_reg  reg;

    if( ( def != ASSUME_NOTHING ) && AssumeTable[def].flat ) {
        Frame = MAGIC_FLAT_GROUP;
        return( def );
    }
    if( SegOverride != NULL ) {
        reg = search_assume( SegOverride, def, 1 );
    } else {
        reg = search_assume( sym->segment, def, 0 );
    }
    if( reg == ASSUME_NOTHING ) {
        if( sym->state == SYM_EXTERNAL && sym->segment == NULL ) {
            reg = def;
        }
    }
    if( reg != ASSUME_NOTHING ) {
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

bool ModuleEnd( token_idx count )
/*******************************/
{
    char        buffer[ MAX_LINE_LEN ];
    token_idx   i;

    if( lastseg.seg != SIM_NONE ) {
        close_lastseg();
    }

    if( !EndDirectiveProc ) {
        EndDirectiveProc = TRUE;
        strcpy( buffer, "end" );
        if( StartupDirectiveFound ) {
            StartupDirectiveFound = FALSE;
            if( count > 1 ) {
                AsmError( SYNTAX_ERROR );
            }
            strcat( buffer, " " );
            strcat( buffer, StartAddr );
        } else {
            for( i = 1; i < count; ++i ) {
                strcat( buffer, " " );
                strcat( buffer, AsmBuffer[i]->string_ptr );
            }
        }
        InputQueueLine( buffer );
        return( RC_OK );
    }

    EndDirectiveFound = TRUE;

    if( count == 1 ) {
        return( RC_OK );
    }

    if( AsmBuffer[1]->class != TC_ID ) {
        AsmError( INVALID_START_ADDRESS );
        return( RC_ERROR );
    }
    ModendFixup = InsFixups[ OPND1 ];
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
        /* first determine offset size */
        if( (Code->info.cpu&P_CPU_MASK) >= P_386 ) {
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
        return( ERROR );
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

bool LocalDef( token_idx i )
/**************************/
{
    int             type;
    label_list      *local;
    label_list      *curr;
    proc_info       *info;
    struct asm_sym  *sym;
    struct asm_sym  *tmp = NULL;

    /*

     LOCAL symbol[,symbol]...[=symbol]

     symbol:
           name [[count]] [:[type]]
     count:
           number of array elements, default is 1
     type:
           one of BYTE,SBYTE,WORD,SWORD,DWORD,SDWORD,PWORD,FWORD,TBYTE, default is WORD

    */

    if( DefineProc == FALSE ) {
        AsmError( LOCAL_VAR_MUST_FOLLOW_PROC );
        return( RC_ERROR );
    }

    /**/myassert( CurrProc != NULL );

    info = CurrProc->e.procinfo;

    for( i++; i < Token_Count; i++ ) {
        if( AsmBuffer[i]->class != TC_ID ) {
            AsmError( LABEL_IS_EXPECTED );
            return( RC_ERROR );
        }

        sym = AsmLookup( AsmBuffer[i]->string_ptr );
        if( sym == NULL )
            return( RC_ERROR );

        if( sym->state != SYM_UNDEFINED ) {
            AsmErr( SYMBOL_PREVIOUSLY_DEFINED, sym->name );
            return( RC_ERROR );
        } else {
            sym->state = SYM_INTERNAL;
            sym->mem_type = MT_WORD;
        }

        local = AsmAlloc( sizeof( label_list ) );
        local->label = AsmAlloc( strlen( AsmBuffer[i]->string_ptr ) + 1 );
        strcpy( local->label, AsmBuffer[i++]->string_ptr );
        local->size = LOCAL_DEFAULT_SIZE;
        local->replace = NULL;
        local->sym = NULL;
        local->factor = 1;
        local->next = NULL;
        local->is_register = 0;

        if( i < Token_Count ) {
            if( AsmBuffer[i]->class == TC_OP_SQ_BRACKET ) {
                i++;
                if( ( AsmBuffer[i]->class != TC_NUM ) || ( i >= Token_Count ) ) {
                    AsmError( SYNTAX_ERROR );
                    return( RC_ERROR );
                }
                local->factor = AsmBuffer[i++]->u.value;
                if( ( AsmBuffer[i]->class != TC_CL_SQ_BRACKET ) || ( i >= Token_Count ) ) {
                    AsmError( EXPECTED_CL_SQ_BRACKET );
                    return( RC_ERROR );
                }
                i++;
            }
        }

        if( i < Token_Count ) {
            if( AsmBuffer[i]->class != TC_COLON ) {
                AsmError( COLON_EXPECTED );
                return( RC_ERROR );
            }
            i++;

            type = token_cmp( &(AsmBuffer[i]->string_ptr), TOK_EXT_BYTE,
                              TOK_EXT_TBYTE );
            if( type == ERROR ) {
                tmp = AsmGetSymbol( AsmBuffer[i]->string_ptr );
                if( tmp != NULL ) {
                    if( tmp->state == SYM_STRUCT ) {
                        type = MT_STRUCT;
                        local->sym = tmp;
                    }
                }
            }
            if( type == ERROR ) {
                AsmError( INVALID_QUALIFIED_TYPE );
                return( RC_ERROR );
            }
            sym->mem_type = TypeInfo[type].value;
            if( type == MT_STRUCT ) {
                local->size = ( ( dir_node *)tmp)->e.structinfo->size;
            } else {
                local->size = find_size( type );
            }
        }

        info->localsize += ( local->size * local->factor );

        if( info->locallist == NULL ) {
            info->locallist = local;
        } else {
            for( curr = info->locallist;; curr = curr->next ) {
                if( curr->next == NULL ) {
                    break;
                }
            }
            curr->next = local;
        }

        switch( AsmBuffer[++i]->class ) {
        case TC_DIRECTIVE:
            if( ( AsmBuffer[i]->u.token == T_EQU2 ) &&
                ( AsmBuffer[i + 1]->class == TC_ID ) &&
                ( AsmBuffer[i + 2]->class == TC_FINAL ) ) {
                i++;
                StoreConstantNumber( AsmBuffer[i++]->string_ptr, info->localsize, TRUE );
            }
            break;
        case TC_COMMA:
            continue;
        case TC_FINAL:
        default:
            break;
        }
        break;
    }
    if( AsmBuffer[i]->class != TC_FINAL ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    return( RC_OK );
}

bool ArgDef( token_idx i )
/************************/
{
    char            *token;
    char            *typetoken;
    proc_info       *info;
    label_list      *paranode;
    label_list      *paracurr;
    int             type, register_count, parameter_size, unused_stack_space, parameter_on_stack = TRUE;

    struct asm_sym  *param;
    struct asm_sym  *tmp = NULL;

    /*

    ARG argument[,argument]...[=symbol]

    argument:
          name [[count]] [:[type]]
    count:
          number of array elements, default is 1
    type:
          one of BYTE,SBYTE,WORD,SWORD,DWORD,SDWORD,PWORD,FWORD,TBYTE, default is WORD

    */

    if( DefineProc == FALSE ) {
        AsmError( ARG_MUST_FOLLOW_PROC );
        return( RC_ERROR );
    }

    register_count = 0;
    unused_stack_space = 0;

    /**/myassert( CurrProc != NULL );

    info = CurrProc->e.procinfo;

    if( ( CurrProc->sym.langtype == LANG_WATCOM_C ) &&
        ( Options.watcom_parms_passed_by_regs || !Use32 ) ) {
        parameter_on_stack = FALSE;
    }
    for( i++; i < Token_Count; i++ ) {
        if( AsmBuffer[i]->class != TC_ID ) {
            AsmError( LABEL_IS_EXPECTED );
            return( RC_ERROR );
        }

        /* read symbol */
        token = AsmBuffer[i++]->string_ptr;

        /* read colon */
        if( AsmBuffer[i]->class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;

        /* now read qualified type */
        typetoken = AsmBuffer[i]->string_ptr;

        type = token_cmp( &typetoken, TOK_EXT_BYTE, TOK_EXT_TBYTE );

        if( ( type == ERROR ) && (Options.mode & MODE_IDEAL) ) {
            tmp = AsmGetSymbol( AsmBuffer[i]->string_ptr );
            if( tmp != NULL ) {
                if( tmp->state == SYM_STRUCT ) {
                    type = MT_STRUCT;
                }
            }
        }
        if( type == ERROR ) {
            type = token_cmp( &typetoken, TOK_PROC_VARARG, TOK_PROC_VARARG );
            if( type == ERROR ) {
                AsmError( INVALID_QUALIFIED_TYPE );
                return( RC_ERROR );
            } else {
                switch( CurrProc->sym.langtype ) {
                case LANG_NONE:
                case LANG_BASIC:
                case LANG_FORTRAN:
                case LANG_PASCAL:
                    AsmError( VARARG_REQUIRES_C_CALLING_CONVENTION );
                    return( RC_ERROR );
                case LANG_WATCOM_C:
                    info->parasize += unused_stack_space;
                    parameter_on_stack = TRUE;
                    break;
                default:
                    break;
                }
            }
        }
        param = AsmLookup( token );
        if( param == NULL )
            return( RC_ERROR );

        if( param->state != SYM_UNDEFINED ) {
            AsmErr( SYMBOL_PREVIOUSLY_DEFINED, param->name );
            return( RC_ERROR );
        } else {
            param->state = SYM_INTERNAL;
            param->mem_type = TypeInfo[type].value;
        }
        if( type == MT_STRUCT ) {
            parameter_size = ( ( dir_node *)tmp)->e.structinfo->size;
            parameter_on_stack = TRUE;
        } else {
            parameter_size = find_size( type );
            tmp = NULL;
        }
        if( parameter_on_stack == FALSE ) {
            parameter_on_stack = get_watcom_argument( parameter_size, &register_count );
            if( parameter_on_stack == ERROR )
                return( RC_ERROR );
        }
        paranode = AsmAlloc( sizeof( label_list ) );
        paranode->u.is_vararg = type == TOK_PROC_VARARG ? TRUE : FALSE;
        paranode->size = parameter_size;
        paranode->label = AsmAlloc( strlen( token ) + 1 );
        paranode->replace = NULL;
        paranode->sym = tmp;
        strcpy( paranode->label, token );
        if( parameter_on_stack ) {
            paranode->is_register = FALSE;
            if( Use32 ) {
                info->parasize += ROUND_UP( parameter_size, 4 );
            } else {
                info->parasize += ROUND_UP( parameter_size, 2 );
            }
        } else {
            paranode->is_register = TRUE;
            register_count++;
            if( Use32 ) {
                unused_stack_space += ROUND_UP( parameter_size, 4 );
            } else {
                unused_stack_space += ROUND_UP( parameter_size, 2 );
            }
        }

        info->is_vararg |= paranode->u.is_vararg;

        switch( CurrProc->sym.langtype ) {
        case LANG_BASIC:
        case LANG_FORTRAN:
        case LANG_PASCAL:
            /* Parameters are stored in reverse order */
            paranode->next = info->paralist;
            info->paralist = paranode;
            break;
        default:
            paranode->next = NULL;
            if( info->paralist == NULL ) {
                info->paralist = paranode;
            } else {
                for( paracurr = info->paralist;; paracurr = paracurr->next ) {
                    if( paracurr->next == NULL ) {
                        break;
                    }
                }
                paracurr->next = paranode;
            }
            break;
        }
        switch( AsmBuffer[++i]->class ) {
        case TC_DIRECTIVE:
            if( ( AsmBuffer[i]->u.token == T_EQU2 ) &&
                ( AsmBuffer[i + 1]->class == TC_ID ) &&
                ( AsmBuffer[i + 2]->class == TC_FINAL ) ) {
                i++;
                StoreConstantNumber( AsmBuffer[i++]->string_ptr, info->parasize, TRUE );
            }
            break;
        case TC_COMMA:
            continue;
        case TC_FINAL:
        default:
            break;
        }
        break;
    }
    if( AsmBuffer[i]->class != TC_FINAL ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    return( RC_OK );
}

bool UsesDef( token_idx i )
/*************************/
{
    char        *token;
    proc_info   *info;
    regs_list   *regist;
    regs_list   *temp_regist;

    if( DefineProc == FALSE ) {
        AsmError( USES_MUST_FOLLOW_PROC );
        return( RC_ERROR );
    }

    /**/myassert( CurrProc != NULL );

    if( CurrProc->sym.langtype == LANG_NONE ) {
        AsmError( USES_MEANINGLESS_WITHOUT_LANGUAGE );
        return( RC_ERROR );
    }

    info = CurrProc->e.procinfo;

    for( i++; ( i < Token_Count ) && ( AsmBuffer[i]->class != TC_FINAL ); i++ ) {
        token = AsmBuffer[i]->string_ptr;
        regist = AsmAlloc( sizeof( regs_list ));
        regist->next = NULL;
        regist->reg = AsmAlloc( strlen(token) + 1 );
        strcpy( regist->reg, token );
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
        if( AsmBuffer[++i]->class != TC_COMMA )
            break;
    }
    if( AsmBuffer[i]->class != TC_FINAL ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    return( RC_OK );
}

bool EnumDef( token_idx i )
{
    char            *name, string[ MAX_LINE_LEN ];
    token_idx       n;
    int             enums, in_braces;
    long            count;
    struct asm_sym  *sym;
    dir_node        *dir;

    /*
    name ENUM [var [, var...]] {
              [var [, var]...]
              [var [, var]...] }
    or
    ENUM name [var [, var...]] {
              [var [, var]...]
              [var [, var]...] }

    var: name [= number]
    */
    if( Options.mode & MODE_IDEAL ) {
        n = i + 1;
    } else if( i > 0 ) {
        n = --i;
    } else {
        n = INVALID_IDX;
    }
    if( ( n == INVALID_IDX ) || ( AsmBuffer[n]->class != TC_ID ) ) {    /* name present? */
        AsmError( ENUM_NAME_MISSING );
        return( RC_ERROR );
    }
    count = enums = in_braces = 0;
    name = AsmBuffer[n]->string_ptr;
    if( StoreConstantNumber( name, count, TRUE ) )
        return( RC_ERROR );
    n = 2;
    do {
        for( i = n++; ; i++ ) {
            switch( AsmBuffer[i]->class ) {
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
                name = AsmBuffer[i]->string_ptr;
                break;
            default:
                AsmError( SYNTAX_ERROR );
                return( RC_ERROR );
            }
            if( n ) {
                if( ( AsmBuffer[i + 1]->class == TC_DIRECTIVE ) &&
                    ( AsmBuffer[i + 1]->u.token == T_EQU2 ) ) {
                    i += 2;
                    switch( AsmBuffer[i]->class ) {
                    case TC_NUM:
                        count = AsmBuffer[i]->u.value;
                        break;
                    case TC_ID:
                        sym = AsmGetSymbol( AsmBuffer[i]->string_ptr );
                        if( ( sym != NULL ) && ( sym->state == SYM_CONST ) ) {
                            dir = ( dir_node * ) sym;
                            if( dir->e.constinfo->data[0].class == TC_NUM ) {
                                count = dir->e.constinfo->data[0].u.value;
                                break;
                            }
                        }
                    default:
                        AsmError( EXPECTING_NUMBER );
                        return( RC_ERROR );
                    }
                }
                if( StoreConstantNumber( name, count++, TRUE ) )
                    return( RC_ERROR );
                enums++;
                if( AsmBuffer[i + 1]->class == TC_COMMA )
                    i++;
                continue;
            } else {
                if( in_braces ) {
                    if( ScanLine( string, MAX_LINE_LEN ) == NULL ) {
                        AsmError( UNEXPECTED_END_OF_FILE );
                        return( RC_ERROR );
                    }
                    Token_Count = AsmScan( string );
                }
            }
            break;
        }
    } while( in_braces );
    if( ( AsmBuffer[i]->class != TC_FINAL ) || ( enums == 0 ) ) {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    return( RC_OK );
}

static bool proc_exam( dir_node *proc, token_idx i )
/**************************************************/
{
    char            *token;
    char            *typetoken;
    int_8           minimum;        // Minimum value of the type of token to be read
//    int_8           finish;
    proc_info       *info;
    regs_list       *regist;
    regs_list       *temp_regist;
    label_list      *paranode;
    label_list      *paracurr;
    int             type, register_count, parameter_size, unused_stack_space, parameter_on_stack = TRUE;
    struct asm_sym  *param;

    info = proc->e.procinfo;

    minimum = TOK_PROC_FAR;
//    finish = FALSE;
    proc->sym.langtype = ModuleInfo.langtype;

    // fixme ... we need error checking here --- for nested procs

    /* Obtain all the default value */

    info->mem_type = IS_PROC_FAR() ? MT_FAR : MT_NEAR;
    info->parasize = 0;
    info->localsize = 0;
    info->export = FALSE;
    info->is_vararg = FALSE;
    info->pe_type = ( ( Code->info.cpu & P_CPU_MASK ) == P_286 ) || ( ( Code->info.cpu & P_CPU_MASK ) == P_386 );
    SetMangler( &proc->sym, NULL, LANG_NONE );

    /* Parse the definition line, except the parameters */
    for( i++; i < Token_Count && AsmBuffer[i]->class != TC_COMMA; i++ ) {
        token = AsmBuffer[i]->string_ptr;
        if( AsmBuffer[i]->class == TC_STRING ) {
            /* name mangling */
            SetMangler( &proc->sym, token, LANG_NONE );
            continue;
        }

        type = token_cmp( &token, TOK_PROC_FAR, TOK_PROC_USES );
        if( type == ERROR )
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
            if( Options.watcom_parms_passed_by_regs || !Use32 ) {
                parameter_on_stack = FALSE;
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
            info->export = TRUE;
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
            for( i++; ( i < Token_Count ) && ( AsmBuffer[i]->class != TC_COMMA ); i++ ) {
                token = AsmBuffer[i]->string_ptr;
                regist = AsmAlloc( sizeof( regs_list ));
                regist->next = NULL;
                regist->reg = AsmAlloc( strlen(token) + 1 );
                strcpy( regist->reg, token );
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
            if( AsmBuffer[i]->class == TC_COMMA )
                i--;
            break;
        default:
            goto parms;
        }
    }

parms:
    CurrProc = proc;
    DefineProc = TRUE;
    proc->sym.mem_type = info->mem_type;

    if( i >= Token_Count ) {
        return( RC_OK );
    } else if( ( proc->sym.langtype == LANG_NONE ) && ( (Options.mode & MODE_IDEAL) == 0 ) ) {
        AsmError( LANG_MUST_BE_SPECIFIED );
        return( RC_ERROR );
    } else if( AsmBuffer[i]->class == TC_COMMA ) {
        i++;
    }


    /* reset register count and unused stack space counter */
    register_count = unused_stack_space = 0;

    /* now parse parms */
    for( ; i < Token_Count; i++ ) {
        /* read symbol */
        token = AsmBuffer[i++]->string_ptr;

        /* read colon */
        if( AsmBuffer[i]->class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;

        /* now read qualified type */
        typetoken = AsmBuffer[i]->string_ptr;

        type = token_cmp( &typetoken, TOK_EXT_BYTE, TOK_EXT_TBYTE );

        if( type == ERROR ) {
            type = token_cmp( &typetoken, TOK_PROC_VARARG, TOK_PROC_VARARG );
            if( type == ERROR ) {
                AsmError( INVALID_QUALIFIED_TYPE );
                return( RC_ERROR );
            } else {
                switch( proc->sym.langtype ) {
                case LANG_NONE:
                case LANG_BASIC:
                case LANG_FORTRAN:
                case LANG_PASCAL:
                    AsmError( VARARG_REQUIRES_C_CALLING_CONVENTION );
                    return( RC_ERROR );
                case LANG_WATCOM_C:
                    info->parasize += unused_stack_space;
                    parameter_on_stack = TRUE;
                    break;
                default:
                    break;
                }
            }
        }

        param = AsmLookup( token );
        if( param == NULL )
            return( RC_ERROR );

        if( param->state != SYM_UNDEFINED ) {
            AsmErr( SYMBOL_PREVIOUSLY_DEFINED, param->name );
            return( RC_ERROR );
        } else {
            param->state = SYM_INTERNAL;
            param->mem_type = TypeInfo[type].value;
        }

        parameter_size = find_size( type );
        if( parameter_on_stack == FALSE ) {
            parameter_on_stack = get_watcom_argument( parameter_size, &register_count );
            if( parameter_on_stack == ERROR )
                return( RC_ERROR );
        }
        paranode = AsmAlloc( sizeof( label_list ) );
        paranode->u.is_vararg = type == TOK_PROC_VARARG ? TRUE : FALSE;
        paranode->size = parameter_size;
        paranode->label = AsmAlloc( strlen( token ) + 1 );
        paranode->replace = NULL;
        strcpy( paranode->label, token );
        if( parameter_on_stack ) {
            paranode->is_register = FALSE;
            if( Use32 ) {
                info->parasize += ROUND_UP( parameter_size, 4 );
            } else {
                info->parasize += ROUND_UP( parameter_size, 2 );
            }
        } else {
            paranode->is_register = TRUE;
            register_count++;
            if( Use32 ) {
                unused_stack_space += ROUND_UP( parameter_size, 4 );
            } else {
                unused_stack_space += ROUND_UP( parameter_size, 2 );
            }
        }

        info->is_vararg |= paranode->u.is_vararg;

        switch( proc->sym.langtype ) {
        case LANG_BASIC:
        case LANG_FORTRAN:
        case LANG_PASCAL:
            /* Parameters are stored in reverse order */
            paranode->next = info->paralist;
            info->paralist = paranode;
            break;
        default:
            paranode->next = NULL;
            if( info->paralist == NULL ) {
                info->paralist = paranode;
            } else {
                for( paracurr = info->paralist;; paracurr = paracurr->next ) {
                    if( paracurr->next == NULL ) {
                        break;
                    }
                }
                paracurr->next = paranode;
            }
            break;
        }
        /* go past comma */
        i++;
        if( ( i < Token_Count ) && ( AsmBuffer[i]->class != TC_COMMA ) ) {
            AsmError( EXPECTING_COMMA );
            return( RC_ERROR );
        }
    }
    return( RC_OK );
}

bool ProcDef( token_idx i, bool proc_def )
/****************************************/
{
    dir_node        *dir;
    char            *name;
    token_idx       n;

    proc_def = proc_def;
    if( Options.mode & MODE_IDEAL ) {
        n = ++i;
    } else if( i > 0 ) {
        n = i - 1;
    } else {
        n = INVALID_IDX;
    }

    if( CurrProc != NULL ) {
        /* nested procs ... push currproc on a stack */
        push_proc( CurrProc );
    }

    if( Parse_Pass == PASS_1 ) {
        if( ( n == INVALID_IDX ) || ( AsmBuffer[n]->class != TC_ID ) ) {
            AsmError( PROC_MUST_HAVE_A_NAME );
            return( RC_ERROR );
        }
        name = AsmBuffer[n]->string_ptr;
        dir = (dir_node *)AsmGetSymbol( name );
        if( dir == NULL ) {
            dir = dir_insert( name, TAB_PROC );
        } else if( dir->sym.state == SYM_UNDEFINED ) {
            /* alloc the procinfo struct */
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
        if( proc_exam( dir, i ) ) {
            return( RC_ERROR );
        }
    } else {
        // fixme -- nested procs can be ok /**/myassert( CurrProc == NULL );
        /**/myassert( n != INVALID_IDX );
        dir = (dir_node *)AsmGetSymbol( AsmBuffer[n]->string_ptr );
        /**/myassert( dir != NULL );
        CurrProc = dir;
        GetSymInfo( &dir->sym );
        DefineProc = TRUE;
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
        for( curr = info->paralist; curr; curr = curr->next ) {
            AsmTakeOut( curr->label );
        }
        for( curr = info->locallist; curr; curr = curr->next ) {
            AsmTakeOut( curr->label );
        }
    }
    CurrProc = pop_proc();
}

bool ProcEnd( token_idx i )
/*************************/
{
    if( CurrProc == NULL ) {
        AsmError( BLOCK_NESTING_ERROR );
        return( RC_ERROR );
    } else if( Options.mode & MODE_IDEAL ) {
        if( AsmBuffer[++i]->class == TC_ID ) {
            if( ( (dir_node *)AsmGetSymbol( AsmBuffer[i]->string_ptr ) != CurrProc ) ) {
                AsmError( PROC_NAME_DOES_NOT_MATCH );
            }
        }
        ProcFini();
        return( RC_OK );
    } else if( i == 0 ) {
        AsmError( PROC_MUST_HAVE_A_NAME );
        ProcFini();
        return( RC_ERROR );
    } else if( (dir_node *)AsmGetSymbol( AsmBuffer[i-1]->string_ptr ) == CurrProc ) {
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
/*********************************************/
/* Push the registers list */
{
    char        buffer[20];

    if( regist == NULL )
        return;
    strcpy( buffer, "push " );
    strcpy( buffer + strlen( buffer ), regist->reg );
    InputQueueLine( buffer );
    push_registers( regist->next );
}

static void pop_registers( regs_list *regist )
/********************************************/
/* Pop the registers list */
{
    char        buffer[20];

    if( regist == NULL )
        return;
    pop_registers( regist->next );
    strcpy( buffer, "pop " );
    strcpy( buffer + strlen( buffer ), regist->reg );
    InputQueueLine( buffer );
}

bool WritePrologue( const char *curline )
/***************************************/
{
    char                buffer[80];
    proc_info           *info;
    label_list          *curr;
    unsigned long       offset;
    unsigned long       size;
    int                 register_count = 0;
    int                 parameter_on_stack = TRUE;
    int                 align = Use32 ? 4 : 2;

    /**/myassert( CurrProc != NULL );
    info = CurrProc->e.procinfo;

    /* save 1st instruction following a procedure definition */
    InputQueueLine( curline );

    if( Parse_Pass == PASS_1 ) {
        /* Figure out the replacing string for local variables */
        offset = 0;
        for( curr = info->locallist; curr != NULL; curr = curr->next ) {
            size = curr->size * curr->factor;
            offset += ROUND_UP( size, align );
            size_override( buffer, curr->size );
            if( Options.mode & MODE_IDEAL ) {
                if( curr->sym != NULL ) {
                    sprintf( buffer + strlen(buffer), "(%s %s%lu)", curr->sym->name,
                             Use32 ? IDEAL_LOCAL_STRING_32 : IDEAL_LOCAL_STRING, offset );
                } else {
                    sprintf( buffer + strlen(buffer), "%s%lu",
                             Use32 ? IDEAL_LOCAL_STRING_32 : IDEAL_LOCAL_STRING, offset );
                }
            } else {
                sprintf( buffer + strlen(buffer), "%s%lu]",
                         Use32 ? LOCAL_STRING_32 : LOCAL_STRING, offset );
            }
            curr->replace = AsmAlloc( strlen( buffer ) + 1 );
            strcpy( curr->replace, buffer );
        }
        info->localsize = offset;

        /* Figure out the replacing string for the parameters */

        if( info->mem_type == MT_NEAR ) {
            offset = 4;    // offset from BP : return addr + old BP
        } else {
            offset = 6;
        }
        if( Use32 )
            offset *= 2;
        if( ( CurrProc->sym.langtype == LANG_WATCOM_C ) &&
            ( Options.watcom_parms_passed_by_regs || !Use32 ) &&
            ( info->is_vararg == FALSE ) ) {
            parameter_on_stack = FALSE;
        }
        for( curr = info->paralist; curr; curr = curr->next ) {
            if( parameter_on_stack == FALSE ) {
                parameter_on_stack = get_watcom_argument_string( buffer, curr->size, &register_count );
                if( parameter_on_stack == ERROR ) {
                    return( RC_ERROR );
                }
            }
            if( parameter_on_stack ) {
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
                offset += ROUND_UP( curr->size, align );
            } else {
                register_count++;
            }
            curr->replace = AsmAlloc( strlen( buffer ) + 1 );
            strcpy( curr->replace, buffer );
            if( curr->replace[0] == ' ' ) {
                curr->replace[0] = '\0';
                if( Use32 )
                    curr->replace[4] = '\0';
                else
                    curr->replace[3] = '\0';
            }
        }
    }
    if( (Options.mode & MODE_IDEAL) && ( CurrProc->sym.langtype == LANG_NONE ) )
        return( RC_OK );
    in_prologue = TRUE;
    PushLineQueue();
    if( ( info->localsize != 0 ) || ( info->parasize != 0 ) ||
        ( info->is_vararg ) || Options.trace_stack == 2 ) {
        //
        // prolog code timmings
        //
        //                                                   best result
        //               size  86  286  386  486  P     86  286  386  486  P
        // push bp       2     11  3    2    1    1
        // mov bp,sp     2     2   2    2    1    1
        // sub sp,immed  4     4   3    2    1    1
        //              -----------------------------
        //               8     17  8    6    3    3     x   x    x    x    x
        //
        // push ebp      2     -   -    2    1    1
        // mov ebp,esp   2     -   -    2    1    1
        // sub esp,immed 6     -   -    2    1    1
        //              -----------------------------
        //               10    -   -    6    3    3              x    x    x
        //
        // enter imm,0   4     -   11   10   14   11
        //
        // write prolog code
        if( Options.trace_stack && info->mem_type == MT_FAR ) {
            if( Use32 ) {
                strcpy( buffer, "inc ebp" );
            } else {
                strcpy( buffer, "inc bp" );
            }
            InputQueueLine( buffer );
        }
        if( Use32 ) {
            // write 80386 prolog code
            // PUSH EBP
            // MOV  EBP, ESP
            // SUB  ESP, the number of localbytes
            strcpy( buffer, "push ebp" );
            InputQueueLine( buffer );
            strcpy( buffer, "mov ebp,esp" );
            if( info->localsize != 0 ) {
                InputQueueLine( buffer );
                strcpy( buffer, "sub esp," );
                sprintf( buffer + strlen( buffer ), "%lu", info->localsize );
            }
        } else {
            // write 8086 prolog code
            // PUSH BP
            // MOV  BP, SP
            // SUB  SP, the number of localbytes
            strcpy( buffer, "push bp" );
            InputQueueLine( buffer );
            strcpy( buffer, "mov bp,sp" );
            if( info->localsize != 0 ) {
                InputQueueLine( buffer );
                strcpy( buffer, "sub sp," );
                sprintf( buffer + strlen( buffer ), "%lu", info->localsize );
            }
        }
        InputQueueLine( buffer );
    }
    /* Push the registers */
    push_registers( info->regslist );
    return( RC_OK );
}

static void write_epilogue( void )
/********************************/
{
    char        buffer[80];
    proc_info   *info;

    /**/myassert( CurrProc != NULL );

    if( (Options.mode & MODE_IDEAL) && ( CurrProc->sym.langtype == LANG_NONE ) )
        return;

    info = CurrProc->e.procinfo;

    /* Pop the registers */
    pop_registers( CurrProc->e.procinfo->regslist );

    if( ( info->localsize == 0 ) && ( info->parasize == 0 ) &&
        ( !info->is_vararg ) && Options.trace_stack != 2 ) {
        return;
    }
    // epilog code timmings
    //
    //                                                  best result
    //              size  86  286  386  486  P      86  286  386  486  P
    // mov sp,bp    2     2   2    2    1    1
    // pop bp       2     8   5    4    4    1
    //             -----------------------------
    //              4     10  7    6    5    2      x             x    x
    //
    // mov esp,ebp  2     -   -    2    1    1
    // pop ebp      2     -   -    4    4    1
    //             -----------------------------
    //              4     -   -    6    5    2                    x    x
    //
    // leave        1     -   5    4    5    3          x    x    x
    //
    // !!!! DECISION !!!!
    //
    // leave will be used for .286 and .386
    // .286 code will be best working on 286,386 and 486 processors
    // .386 code will be best working on 386 and 486 processors
    // .486 code will be best working on 486 and above processors
    //
    //   without LEAVE
    //
    //         86  286  386  486  P
    //  .8086  0   -2   -2   0    +1
    //  .286   -   -2   -2   0    +1
    //  .386   -   -    -2   0    +1
    //  .486   -   -    -    0    +1
    //
    //   LEAVE 286 only
    //
    //         86  286  386  486  P
    //  .8086  0   -2   -2   0    +1
    //  .286   -   0    +2   0    -1
    //  .386   -   -    -2   0    +1
    //  .486   -   -    -    0    +1
    //
    //   LEAVE 286 and 386
    //
    //         86  286  386  486  P
    //  .8086  0   -2   -2   0    +1
    //  .286   -   0    +2   0    -1
    //  .386   -   -    0    0    -1
    //  .486   -   -    -    0    +1
    //
    //   LEAVE 286, 386 and 486
    //
    //         86  286  386  486  P
    //  .8086  0   -2   -2   0    +1
    //  .286   -   0    +2   0    -1
    //  .386   -   -    0    0    -1
    //  .486   -   -    -    0    -1
    //
    // write epilog code
    if( info->pe_type ) {
        // write 80286 and 80386 epilog code
        // LEAVE
        strcpy( buffer, "leave" );
    } else if( Use32 ) {
        // write 32-bit 80486 or P epilog code
        // Mov ESP, EBP
        // POP EBP
        if( info->localsize != 0 ) {
            strcpy( buffer, "mov esp,ebp" );
            InputQueueLine( buffer );
        }
        strcpy( buffer, "pop ebp" );
    } else {
        // write 16-bit 8086 or 80486 or P epilog code
        // Mov SP, BP
        // POP BP
        if( info->localsize != 0 ) {
            strcpy( buffer, "mov sp,bp" );
            InputQueueLine( buffer );
        }
        strcpy( buffer, "pop bp" );
    }
    InputQueueLine( buffer );
    if( Options.trace_stack && info->mem_type == MT_FAR ) {
        if( Use32 ) {
            strcpy( buffer, "dec ebp" );
        } else {
            strcpy( buffer, "dec bp" );
        }
        InputQueueLine( buffer );
    }
}

bool Ret( token_idx i, token_idx count, bool flag_iret )
/******************************************************/
{
    char        buffer[20];
    proc_info   *info;
    expr_list   opndx;

    info = CurrProc->e.procinfo;

    if( flag_iret ) {
        if( AsmBuffer[i]->u.token == T_IRET ) {
            strcpy( buffer, "iretf" );
        } else {
            strcpy( buffer, "iretdf" );
        }
    } else {
        if( info->mem_type == MT_NEAR ) {
            strcpy( buffer, "retn " );
        } else {
            strcpy( buffer, "retf " );
        }
    }

    write_epilogue();

    if( !flag_iret ) {
        if( count == i + 1 ) {
            switch( CurrProc->sym.langtype ) {
            case LANG_BASIC:
            case LANG_FORTRAN:
            case LANG_PASCAL:
                if( info->parasize != 0 ) {
                    sprintf( buffer + strlen( buffer ), "%lu", info->parasize );
                }
                break;
            case LANG_STDCALL:
                if( !info->is_vararg && info->parasize != 0 ) {
                    sprintf( buffer + strlen( buffer ), "%lu", info->parasize );
                }
                break;
            case LANG_WATCOM_C:
                if( ( Options.watcom_parms_passed_by_regs || !Use32 ) &&
                    ( info->is_vararg == FALSE ) &&
                    ( info->parasize != 0 ) ) {
                    sprintf( buffer + strlen( buffer ), "%lu", info->parasize );
                }
                break;
            default:
                break;
            }
        } else {
            ++i;
            if( EvalOperand( &i, count, &opndx, TRUE ) || (opndx.type != EXPR_CONST) ) {
                AsmError( CONSTANT_EXPECTED );
                return( RC_ERROR );
            }
            sprintf( buffer + strlen( buffer ), "%d", opndx.value );
        }
    }
    InputQueueLine( buffer );
    return( RC_OK );
}

extern void GetSymInfo( struct asm_sym *sym )
/*******************************************/
{
    sym->segment = &GetCurrSeg()->sym;
    sym->offset = GetCurrAddr();
}

bool Comment( int what_to_do, token_idx i, const char *line )
/***********************************************************/
{
    static bool in_comment = FALSE;
    static char delim_char = '\0';

    switch( what_to_do ) {
    case QUERY_COMMENT:
        return( in_comment );
    case QUERY_COMMENT_DELIM:
        /* return delimiting character */
        return( in_comment && strchr( line, delim_char ) != NULL );
    case START_COMMENT:
        i++;
        if( AsmBuffer[i]->string_ptr == NULL ) {
            AsmError( COMMENT_DELIMITER_EXPECTED );
            return( RC_ERROR );
        }
        delim_char = *(AsmBuffer[i]->string_ptr + strspn( AsmBuffer[i]->string_ptr, " \t" ) );
        if( ( delim_char == '\0' )
            || ( strchr( AsmBuffer[i]->string_ptr, delim_char ) == NULL ) ) {
            AsmError( COMMENT_DELIMITER_EXPECTED );
            return( RC_ERROR );
        }
        if( strchr( AsmBuffer[i]->string_ptr, delim_char )
            != strrchr( AsmBuffer[i]->string_ptr, delim_char ) ) {
            /* we have COMMENT delim. ..... delim. -- only 1 line */
        } else {
            in_comment = TRUE;
        }
        return( RC_OK );
    case END_COMMENT:
        in_comment = FALSE;
        return( RC_OK );
    }
    return( RC_ERROR );
}

bool AddAlias( token_idx i )
/**************************/
{
    char    *tmp;

    if( ( AsmBuffer[i + 1]->class == TC_DIRECTIVE )
      && ( AsmBuffer[i + 1]->u.token == T_EQU2 ) ) {
        i++;  /* ALIAS <alias> = <target> (MASM, TASM) */
    } else if( i > 0 ) {
        i--;  /* <alias> ALIAS <target> (WASM) */
    } else {
        AsmError( SYNTAX_ERROR );
        return( RC_ERROR );
    }
    if( ( AsmBuffer[i]->class != TC_ID )
      || ( AsmBuffer[i + 2]->class != TC_ID ) ) {
        AsmError( SYMBOL_EXPECTED );
        return( RC_ERROR );
    }

    /* add this alias to the alias queue */

    /* aliases are stored as:  <len><alias_name><len><substitute_name> */

    tmp = AsmAlloc( strlen( AsmBuffer[i]->string_ptr ) +
                    strlen( AsmBuffer[i + 2]->string_ptr ) + 2 );
    AddAliasData( tmp );

    strcpy( tmp, AsmBuffer[i]->string_ptr );
    tmp += strlen( tmp ) + 1 ;
    strcpy( tmp, AsmBuffer[i + 2]->string_ptr );

    return( RC_OK );
}

bool NameDirective( token_idx i )
/*******************************/
{
    if( Options.module_name != NULL )
        return( RC_OK );
    Options.module_name = AsmAlloc( strlen( AsmBuffer[i + 1]->string_ptr ) + 1 );
    strcpy( Options.module_name, AsmBuffer[i + 1]->string_ptr );
    return( RC_OK );
}

bool CommDef( token_idx i )
/*************************/
{
    char            *token;
    char            *mangle_type = NULL;
    char            *typetoken;
    int             type;
    int             distance;
    int             count;
    dir_node        *dir;
    int             lang_type;
    memtype         mem_type;

    mangle_type = Check4Mangler( &i );
    for( ; i < Token_Count; i++ ) {
        count = 1;

        /* get the distance ( near or far ) */
        typetoken = AsmBuffer[i]->string_ptr;
        distance = token_cmp( &typetoken, TOK_EXT_NEAR, TOK_EXT_FAR );

        if( distance != ERROR ) {
            /* otherwise, there is no distance specified */
            i++;
        } else {
            distance = TOK_EXT_NEAR;
        }

        /* get the symbol language type if present */
        lang_type = GetLangType( &i );

        /* get the symbol name */
        token = AsmBuffer[i++]->string_ptr;

        /* go past the colon */
        if( AsmBuffer[i]->class != TC_COLON ) {
            AsmError( COLON_EXPECTED );
            return( RC_ERROR );
        }
        i++;
        typetoken = AsmBuffer[i]->string_ptr;
        type = token_cmp( &typetoken, TOK_EXT_BYTE, TOK_EXT_TBYTE );

        if( type == ERROR ) {
            AsmError( INVALID_QUALIFIED_TYPE );
            return( RC_ERROR );
        }
        for( ; i< Token_Count && AsmBuffer[i]->class != TC_COMMA; i++ ) {
            if( AsmBuffer[i]->class == TC_COLON ) {
                i++;
                /* count */
                if( AsmBuffer[i]->class != TC_NUM ) {
                    AsmError( EXPECTING_NUMBER );
                    return( RC_ERROR );
                }
                count = AsmBuffer[i]->u.value;
            }
        }
        mem_type = TypeInfo[type].value;
        dir = (dir_node *)AsmGetSymbol( token );
        if( dir == NULL ) {
            dir = dir_insert( token, TAB_COMM );
            if( dir == NULL ) {
                return( RC_ERROR );
            }
        } else if( dir->sym.state == SYM_UNDEFINED ) {
            dir_change( dir, TAB_COMM );
        } else if( dir->sym.mem_type != mem_type ) {
            AsmError( EXT_DEF_DIFF );
            return( RC_ERROR );
        }
        GetSymInfo( &dir->sym );
        dir->sym.offset = 0;
        dir->sym.mem_type = mem_type;
        dir->e.comminfo->size = count;
        dir->e.comminfo->distance = TypeInfo[distance].value;
        SetMangler( &dir->sym, mangle_type, lang_type );
    }
    return( RC_OK );
}

bool Locals( token_idx i )
/************************/
{
    Options.locals_len = ( AsmBuffer[i]->u.token == T_LOCALS ) ? 2 : 0;
    if( i + 1 == Token_Count ) {
        return( RC_OK );
    }
    if( AsmBuffer[i]->u.token == T_LOCALS ) {
        ++i;
        if( i < Token_Count && AsmBuffer[i]->class == TC_ID
            && strlen( AsmBuffer[i]->string_ptr ) >= 2 ) {
            Options.locals_prefix[0] = AsmBuffer[i]->string_ptr[0];
            Options.locals_prefix[1] = AsmBuffer[i]->string_ptr[1];
            if( Token_Count - i == 1 && strlen( AsmBuffer[i]->string_ptr ) == 2 ) {
                return( RC_OK );
            }
        }
    }
    AsmError( SYNTAX_ERROR );
    return( RC_ERROR );
}
