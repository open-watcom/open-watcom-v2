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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#include "asmglob.h"
#include "asmalloc.h"
#include "asmerr.h"
#include "asmops1.h"
#include "asmops2.h"
#include "asmins1.h"
#include "namemgr.h"
#include "asmsym.h"
#include "asmerr.h"

#include "womp.h"
#include "pcobj.h"
#include "objrec.h"
#include "myassert.h"
#include "fixup.h"
#include "queue.h"
#include "expand.h"

#include "directiv.h"
#undef _DIRECT_H_
#define _DIRECT_FIX_
#include "directiv.h"

#define LOCAL_DEFAULT_SIZE      2
#define DEFAULT_STACK_SIZE      1024
#define SIM_CODE_OFFSET         5       // strlen("_TEXT")
#define SIM_DATA_OFFSET         5       // strlen("_DATA")
#define FAROFFSET(seg)          ( seg == SIM_FARDATA ? 8 : 7 )
                                /* strlen of "FAR_DATA" or "FAR_BSS" */

#define ARGUMENT_STRING         " [bp+ "
#define ARGUMENT_STRING_32      " [ebp+ "
#define LOCAL_STRING            " [bp- "
#define LOCAL_STRING_32         " [ebp- "

extern char             *ScanLine( char * );
extern void             FlushCurrSeg( void );
extern void             AsmError( int );
extern int              InputQueueFile( char * );
extern int              AsmScan( char *, char * );
extern struct fixup     *CreateFixupRec( int );
extern void             InputQueueLine( char * );
extern void             AsmTakeOut( char * );
extern int              EvalExpr( int, int, int );
extern void             GetInsString( enum asm_token, char *, int );
extern void             MakeConstant( long );
extern void             SetMangler( struct asm_sym *sym, char *mangle_type );

static char *Check4Mangler( int *i );

extern  const struct asm_ins    ASMFAR AsmOpTable[];
extern  uint            LineNumber;
extern  char            write_to_file;  // write if there is no error
extern  uint_32         BufSize;
extern  struct asm_tok  *AsmBuffer[];   // buffer to store token
extern  struct AsmCodeName AsmOpcode[];
extern  char            StringBuf[];
extern  char            Parse_Pass;     // phase of parsing
extern  int_8           Frame;
extern  uint_8          Frame_Datum;
extern  int_8           DefineProc;     // TRUE if the definition of procedure
                                        // has not ended
extern dir_node         *CurrProc;
extern int_8            Use32;          // if 32-bit code is use
extern File_Info        AsmFiles;
extern char             *CurrString;    // Current Input Line
extern char             EndDirectiveFound;
extern char             AsmChars[];
extern int              Token_Count;    // number of tokens on line

qdesc                   *LnameQueue = NULL; // queue of LNAME structs
seg_list                *CurrSeg;       // points to stack of opened segments
uint                    LnamesIdx;      // Number of LNAMES definition
obj_rec                 *ModendRec;     // Record for Modend
extern struct asm_code  *Code;

assume_info             AssumeTable[ASSUME_LAST];
symbol_queue            Tables[TAB_LAST];// tables of definitions
qdesc                   *PubQueue = NULL; // queue of pubdefs
qdesc                   *GlobalQueue = NULL; // queue of global / externdefs
qdesc                   *AliasQueue = NULL; // queue of aliases
module_info             ModuleInfo;

//proc_stack_node               *ProcStackTop = NULL;
seg_list                *ProcStack = NULL;

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
static char *ExitOS2[] = { /* mov al, retval  followed by: */
        " xor ah, ah",
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
static char *RetVal = " mov    al, ";

static char *StartAddr = "`symbol_reserved_for_start_address`";
char StartupDirectiveFound = FALSE;

/* code generated by @startup */

char            *SimCodeBegin[2][ SIM_LAST ] = {
    {   "_TEXT SEGMENT WORD PUBLIC 'CODE' IGNORE",
        "STACK SEGMENT PARA STACK 'STACK' IGNORE",
        "_DATA SEGMENT WORD PUBLIC 'DATA' IGNORE",
        "_BSS SEGMENT WORD PUBLIC 'BSS' IGNORE",
        "FAR_DATA SEGMENT PARA PRIVATE 'FAR_DATA' IGNORE",
        "FAR_BSS SEGMENT PARA PRIVATE 'FAR_DATA?' IGNORE",
        "CONST SEGMENT WORD PUBLIC 'CONST' IGNORE",             },

    {   "_TEXT SEGMENT DWORD USE32 PUBLIC 'CODE' IGNORE",
        "STACK SEGMENT PARA USE32 STACK 'STACK' IGNORE",
        "_DATA SEGMENT DWORD USE32 PUBLIC 'DATA' IGNORE",
        "_BSS SEGMENT DWORD USE32 PUBLIC 'BSS' IGNORE",
        "FAR_DATA SEGMENT PARA USE32 PRIVATE 'FAR_DATA' IGNORE",
        "FAR_BSS SEGMENT PARA USE32 PRIVATE 'FAR_DATA?' IGNORE",
        "CONST SEGMENT DWORD PUBLIC 'CONST' IGNORE",            },
};

char    *SimCodeEnd[ SIM_LAST ] = {
        "_TEXT ENDS",
        "STACK ENDS",
        "_DATA ENDS",
        "_BSS ENDS",
        "FAR_DATA ENDS",
        "FAR_BSS ENDS",
        "CONST ENDS"
};

/* Code generated by simplified segment definitions */

       uint             segdefidx;      // Number of Segment definition
static uint             grpdefidx;      // Number of Group definition
       uint             extdefidx;      // Number of External definition
static last_seg_info    lastseg;        // last opened simplified segment

char *parm_reg[3][4]= {
    { " AL ", " DL ", " BL ", " CL " },
    { " AX ", " DX ", " BX ", " CX " },
    { " EAX ", " EDX ", " EBX ", " ECX " },
};

enum regsize {
    A_BYTE = 0,
    A_WORD,
    A_DWORD,
};


#define ROUND_UP( i, r ) (((i)+((r)-1)) & ~((r)-1))

int get_watcom_argument_string( char *buffer, uint_8 size, uint_8 *parm_number )
/******************************************************************************/
/* get the register for parms 0 to 3,
 * using the watcom register parm passing conventions ( A D B C ) */
{
    int parm = *parm_number;

    if( parm > 3 ) return( FALSE );
    switch( size ) {
    case 1:
        sprintf( buffer, parm_reg[A_BYTE][parm] );
        break;
    case 2:
        sprintf( buffer, parm_reg[A_WORD][parm] );
        break;
    case 4:
        if( Use32 ) {
            sprintf( buffer, parm_reg[A_DWORD][parm] );
            break;
        } else {
            switch( parm ) {
            case 0:
                sprintf( buffer, " [DX AX]" );
                buffer[0] = 0;
                *parm_number = 1; // take up 0 and 1
                break;
            case 1:
            case 2:
                sprintf( buffer, " [CX BX]" );
                buffer[0] = 0;
                *parm_number = 3; // take up 2 and 3
                break;
            default:
                // passed on stack ... it's ok
                return( FALSE );
            }
            return( TRUE );
        }
    case 10:
        AsmErr( TBYTE_NOT_SUPPORTED );
        return( ERROR );
    case 6:
        if( Use32 ) {
            switch( parm ) {
            case 0:
                sprintf( buffer, " [DX EAX]" );
                buffer[0]=0;
                *parm_number = 1; // take up 0 and 1
                break;
            case 1:
            case 2:
                sprintf( buffer, " [CX EBX]" );
                buffer[0]=0;
                *parm_number = 3; // take up 2 and 3
                break;
            default:
                // passed on stack ... it's ok
                return( FALSE );
            }
            return( TRUE );
        }
        // fall through for 16 bit to default
    case 8:
        if( Use32 ) {
            switch( parm ) {
            case 0:
                sprintf( buffer, " [EDX EAX]" );
                buffer[0]=0;
                *parm_number = 1; // take up 0 and 1
                break;
            case 1:
            case 2:
                sprintf( buffer, " [ECX EBX]" );
                buffer[0]=0;
                *parm_number = 3; // take up 2 and 3
                break;
            default:
                // passed on stack ... it's ok
                return( FALSE );
            }
            return( TRUE );
        }
        // fall through for 16 bit to default
    default:
        // something wierd
        AsmError( STRANGE_PARM_TYPE );
        return( ERROR );
    }
    return( TRUE );
}

#ifdef DEBUG_OUT
void heap( char *func ) // for debugging only
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

void IdxInit( void )
/******************/
{
    LnamesIdx   = 0;
    segdefidx   = 0;
    grpdefidx   = 0;
    extdefidx   = 0;
}

void push( void **stack, void *elt )
/**********************************/
{
    stacknode *node;

    node = AsmAlloc( sizeof( stacknode ));
    node->next = *stack;
    node->elt = elt;
    *stack = node;
}

void *pop( void **stack )
/***********************/
{
    stacknode   *node;
    void        *elt;

    node = (stacknode *)(*stack);
    *stack = node->next;
    elt = node->elt;
    AsmFree( node );
    return( elt );
}


static int push_seg( dir_node *seg )
/**********************************/
/* Push a segment into the current segment stack */
{
    seg_list    *curr;

    for( curr = CurrSeg; curr; curr = curr->next ) {
        if( curr->seg == seg ) {
            AsmError( BLOCK_NESTING_ERROR );
            return( ERROR );
        }
    }
    push( &CurrSeg, seg );
    return( NOT_ERROR );
}

static dir_node *pop_seg( void )
/******************************/
/* Pop a segment out of the current segment stack */
{
    /**/myassert( CurrSeg != NULL );
    return( (dir_node *)pop( &CurrSeg ) );
}

static void push_proc( dir_node *proc )
/*************************************/
{
    push( &ProcStack, proc );
    return;
}

static dir_node *pop_proc( void )
/***********************************/
{
    if( ProcStack == NULL ) return( NULL );
    return( (dir_node *)pop( &ProcStack ) );
}

static dir_add( dir_node *new, int tab )
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

dir_node *dir_insert( char *name, int tab )
/*****************************************/
/* Insert a node into the table specified by tab */
{
    dir_node            *new;
    struct asm_sym      *sym;

    new = AsmAlloc( sizeof( dir_node ) );
    new->next = NULL;
    new->line = LineNumber;
    new->sym.name = AsmAlloc( strlen( name ) + 1 );
    strcpy( new->sym.name, name );
    new->next = new->prev = NULL;

    if( tab != TAB_CLASS_LNAME ) {
        sym = AsmAdd( (struct asm_sym*)new );
    } else {
        /* don't put class lnames into the symbol table - separate name space */
        sym = (struct asm_sym*)new;
    }

    /**/myassert( name != NULL );

    switch( tab ) {
    case TAB_SEG:
        new->e.seginfo = AsmAlloc( sizeof( seg_info ) );
        new->e.seginfo->lname_idx = 0;
        break;
    case TAB_GRP:
        new->e.grpinfo = AsmAlloc( sizeof( grp_info ) );
        new->e.grpinfo->idx = ++grpdefidx;
        new->e.grpinfo->seglist = NULL;
        new->e.grpinfo->numseg = 0;
        new->e.grpinfo->lname_idx = 0;
        break;
    case TAB_EXT:
        new->e.extinfo = AsmAlloc( sizeof( ext_info ) );
        break;
    case TAB_COMM:
        new->e.comminfo = AsmAlloc( sizeof( comm_info ) );
        new->e.comminfo->size = 1;
        break;
    case TAB_CONST:
        sym->state = SYM_CONST;
        sym->grpidx = sym->segidx = sym->offset = 0;
        new->e.constinfo = AsmAlloc( sizeof( const_info ) );
        return( new );
    case TAB_PROC:
        new->e.procinfo = AsmAlloc( sizeof( proc_info ) );
        new->e.procinfo->regslist = NULL;
        new->e.procinfo->paralist = NULL;
        new->e.procinfo->locallist = NULL;
        break;
    case TAB_MACRO:
        new->e.macroinfo = AsmAlloc( sizeof( macro_info ) );
        new->e.macroinfo->parmlist = NULL;
        new->e.macroinfo->data = NULL;
        new->e.macroinfo->filename = NULL;
        new->e.macroinfo->start_line = LineNumber;
        break;
    case TAB_CLASS_LNAME:
    case TAB_LNAME:
        sym->state = tab == TAB_LNAME ? SYM_LNAME : SYM_CLASS_LNAME;
        new->e.lnameinfo = AsmAlloc( sizeof( lname_info ) );
        new->e.lnameinfo->idx = LnamesIdx;
        // fixme
        return( new );
    case TAB_PUB:
        sym->state = SYM_UNDEFINED;
        sym->public = TRUE;
        return( new );
    case TAB_GLOBAL:
        sym->state = SYM_UNDEFINED;
        return( new );
    case TAB_STRUCT:
        sym->state = SYM_STRUCT;
        new->e.structinfo = AsmAlloc( sizeof( struct_info ) );
        new->e.structinfo->size = 0;
        new->e.structinfo->alignment = 0;
        new->e.structinfo->head = NULL;
        new->e.structinfo->tail = NULL;
        sym->total_size = 0;
        sym->total_length = 0;
        sym->first_size = 0;
        sym->first_length = 0;
        return( new );
    default:
        break;
    }
    dir_add( new, tab );

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
                    if( segnext == NULL ) break;
                    segcurr = segnext;
                }
            }
            AsmFree( dir->e.grpinfo );
        }
        break;
    case SYM_SEG:
        ObjKillRec( dir->e.seginfo->segrec );
        AsmFree( dir->e.seginfo );
        break;
    case SYM_EXTERNAL:
        AsmFree( dir->e.extinfo );
        break;
    case SYM_LNAME:
        AsmFree( dir->e.lnameinfo );
        break;
    case SYM_CONST:
        DebugMsg(( "freeing const: %s = ", dir->sym.name ));

        for( i=0; i < dir->e.constinfo->count; i++ ) {
            #ifdef DEBUG_OUT
            if( dir->e.constinfo->data[i].token == T_NUM ) {
                DebugMsg(( "%d ", dir->e.constinfo->data[i].value ));
            } else {
                DebugMsg(( "%s ", dir->e.constinfo->data[i].string_ptr ));
            }
            #endif
            AsmFree( dir->e.constinfo->data[i].string_ptr );
        }
        DebugMsg(( "\n" ));
        AsmFree( dir->e.constinfo->data );
        AsmFree( dir->e.constinfo );
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
                    if( labelnext == NULL ) break;
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
                    if( labelnext == NULL ) break;
                    labelcurr = labelnext;
                }
            }

            regcurr = dir->e.procinfo->regslist;
            if( regcurr != NULL ) {
                for( ;; ) {
                    regnext = regcurr->next;
                    AsmFree( regcurr->reg );
                    AsmFree( regcurr );
                    if( regnext == NULL ) break;
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
                    if( labelnext == NULL ) break;
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
                    if( datanext == NULL ) break;
                    datacurr = datanext;
                }
            }
            if( dir->e.macroinfo->filename != NULL ) {
                AsmFree( dir->e.macroinfo->filename );
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
                next = ptr->next;
                AsmFree( ptr );
            }
            AsmFree( dir->e.structinfo );
        }
    default:
        break;
    }
}

direct_idx GetLnameIdx( char *name )
/**********************************/
{
    struct asm_sym      *sym;
    dir_node            *dir;

    sym = AsmGetSymbol( name );
    if( sym == NULL ) return( LNAME_NULL );
    dir = (dir_node *)sym;

    if( sym->state == SYM_UNDEFINED ) {
        return( LNAME_NULL );
    } else if( sym->state == SYM_GRP ) {
        return( dir->e.grpinfo->lname_idx );
    } else if( sym->state == SYM_SEG ) {
        return( dir->e.seginfo->lname_idx );
    } else {    /* it is an lname record */
        return( dir->e.lnameinfo->idx );
    }
}

direct_idx InsertClassLname( char *name )
/***************************************/
{
    dir_node            *dir;
    struct queuenode    *node;

    if( strlen( name ) > MAX_LNAME ) {
        AsmError( LNAME_TOO_LONG );
        return( LNAME_NULL );
    }

    LnamesIdx++;

    dir = dir_insert( name, TAB_CLASS_LNAME );

    /* put it into the lname table */

    node = AsmAlloc( sizeof( queuenode ) );
    node ->data = (void *)dir;

    if( LnameQueue == NULL ) {
        LnameQueue = AsmAlloc( sizeof( qdesc ) );
        QInit( LnameQueue );
    }
    QEnqueue( LnameQueue, node );

    return( LnamesIdx );
}

direct_idx FindClassLnameIdx( char *name )
/****************************************/
{
    void                **ptr;
    queuenode           *node;
    dir_node            *dir;

    if( LnameQueue == NULL ) return( LNAME_NULL);
    for( ptr = LnameQueue->head; ptr != NULL; ptr = *(void**)ptr ) {
        node = (queuenode *)ptr;
        dir = (dir_node *)node->data;
        if( dir->sym.state != SYM_CLASS_LNAME ) continue;
        if( stricmp( dir->sym.name, name ) == 0 ) {
            return( dir->e.lnameinfo->idx );
        }
    }
    return( LNAME_NULL );
}

direct_idx LnameInsert( char *name )
/**********************************/
{
    struct asm_sym      *sym;
    dir_node            *dir;
    struct queuenode    *node;

    sym = AsmGetSymbol( name );

    if( sym != NULL ) {
        dir = (dir_node *)sym;
        /* The name is already in the table*/
        if( sym->state == SYM_GRP ) {
            if( dir->e.grpinfo->lname_idx == 0 ) {
                dir->e.grpinfo->lname_idx = ++LnamesIdx;
            }
        } else if( sym->state == SYM_SEG ) {
            if( dir->e.seginfo->lname_idx == 0 ) {
                dir->e.seginfo->lname_idx = ++LnamesIdx;
            }
        } else {
            return( ERROR );
        }
    } else {
        if( strlen( name ) > MAX_LNAME ) {
            AsmError( LNAME_TOO_LONG );
            return( ERROR );
        }

        LnamesIdx++;

        dir = dir_insert( name, TAB_LNAME );
    }

    /* put it into the lname table */

    node = AsmAlloc( sizeof( queuenode ) );
    node ->data = (void *)dir;

    if( LnameQueue == NULL ) {
        LnameQueue = AsmAlloc( sizeof( qdesc ) );
        QInit( LnameQueue );
    }
    QEnqueue( LnameQueue, node );

    return( LnamesIdx );
}

void FreePubQueue( void )
/***********************/
{
    if( PubQueue != NULL ) {
        while( PubQueue->head != NULL ) {
            AsmFree( QDequeue( PubQueue ) );
        }
        AsmFree( PubQueue );
    }
}

void FreeAliasQueue( void )
/*************************/
{
    if( AliasQueue != NULL ) {
        while( AliasQueue->head != NULL ) {
            AsmFree( QDequeue( AliasQueue ) );
        }
        AsmFree( AliasQueue );
    }
}

void FreeLnameQueue( void )
/***********************/
{
    dir_node *dir;
    queuenode *node;

    if( LnameQueue != NULL ) {
        while( LnameQueue->head != NULL ) {
            node = QDequeue( LnameQueue );
            dir = (dir_node *)node->data;
            if( dir->sym.state == SYM_CLASS_LNAME ) {
                AsmFree( dir->e.lnameinfo );
                AsmFree( dir->sym.name );
                AsmFree( dir );
            }
            AsmFree( node );
        }
        AsmFree( LnameQueue );
    }
}

void wipe_space( char *token )
/* wipe out the spaces at the beginning of a token */
{
    char        *start;

    if( token == NULL ) return;
    if( strlen( token ) == 0 ) return;

    for( start = token;; start++ ){
        if( *start != ' ' && *start != '\t' ) break;
    }
    if( start == token ) return;

    memmove( token, start, strlen( start ) + 1 );
}

static uint checkword( char **token )
/* wipes out prceding and tailing spaces, and make sure token contains only
   one word */
{
    char        *ptrhead;
    char        *ptrend;

    /* strip the space in the front */
    for( ptrhead = *token; ; ptrhead++ ) {
        if( *(ptrhead) != ' ' && *(ptrhead) != '\t' ) break;
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
        *ptrend = '\0';
        ptrend++;
        while( *ptrend != '\0' ) {
            if( *ptrend != ' ' && *ptrend != '\t' && *ptrend != '\n' ) {
                return( ERROR );
            }
            ptrend++;
        }
    }

    *token = ptrhead;
    return( NOT_ERROR );
}

uint GetDirIdx( char *name, int tab )
/************************************/
{
    dir_node            *dir;
    struct asm_sym              *sym;

    sym = AsmGetSymbol( name );

    dir = (dir_node *)sym;
    /**/myassert( dir != NULL );

    switch( tab ) {
        case TAB_SEG:
            return( dir->e.seginfo->segrec->d.segdef.idx );
        case TAB_EXT:
            return( dir->e.extinfo->idx );
        case TAB_COMM:
            return( dir->e.comminfo->idx );
        case TAB_GRP:
            return( dir->e.grpinfo->idx );
        default:
            /**/myassert( 0 ); /* should only be called with the above values */
    }
    return( ERROR );
}

uint_32 GetCurrAddr( void )
/*************************/
{
    if( CurrSeg == NULL ) return( 0 );
//    return( CurrSeg->seg->e.seginfo->segrec->d.segdef.seg_length );
    return( CurrSeg->seg->e.seginfo->current_loc );
}

uint GetCurrSeg( void )
/*********************/
{
    if( CurrSeg == NULL ) return( 0 );
    return( CurrSeg->seg->e.seginfo->segrec->d.segdef.idx );
}

uint_32 GetCurrSegAlign( void )
/*****************************/
{
    if( CurrSeg == NULL ) return( 0 );
    switch( CurrSeg->seg->e.seginfo->segrec->d.segdef.align ) {
    case ALIGN_ABS: // same as byte aligned ?
    case ALIGN_BYTE:
        return( 1 );
    case ALIGN_WORD:
        return( 2 );
    case ALIGN_DWORD:
        return( 4 );
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
/******************************/
{
    if( CurrSeg == NULL ) return( 0 );
//    /**/myassert(  ( CurrSeg->seg->e.seginfo->segrec->d.segdef.seg_length - BufSize ) == CurrSeg->seg->e.seginfo->start_loc );
    /**/myassert(  !write_to_file ||
                ( CurrSeg->seg->e.seginfo->current_loc - BufSize )
                     == CurrSeg->seg->e.seginfo->start_loc );
    return( CurrSeg->seg->e.seginfo->start_loc );
}

uint GetCurrGrp( void )
/*********************/
{
    dir_node    *grp;
    seg_list    *seg;
    seg_list    *curr;

    if( CurrSeg == NULL ) return( 0 );
    curr = CurrSeg;
    // fixme  - cleanup?
    for( grp = Tables[TAB_GRP].head; grp; grp = grp->next ) {
        for( seg = grp->e.grpinfo->seglist; seg; seg = seg->next ) {
            if( curr->seg == seg->seg ) {
                return( grp->e.grpinfo->idx);
            }
        }
    }
    return( 0 );
}

int GlobalDef( int i )
/********************/
{
    char        *token;
    char        *mangle_type = NULL;
    char        *typetoken;
    int         type;
    struct asm_sym *sym;
    dir_node    *dir;
    struct queuenode    *qnode;

    mangle_type = Check4Mangler( &i );
    for( ; i < Token_Count; i++ ) {
        /* get the symbol name */
        token = AsmBuffer[i++]->string_ptr;

        /* go past the colon */
        if( AsmBuffer[i]->token != T_COLON ) {
            AsmError( COLON_EXPECTED );
            return( ERROR );
        }
        i++;

        typetoken = AsmBuffer[i]->string_ptr;
        type = token_cmp( &typetoken, TOK_EXT_NEAR, TOK_EXT_ABS );

        if( type == ERROR ) {
            AsmError( INVALID_QUALIFIED_TYPE );
            return( ERROR );
        }
        for( ; i< Token_Count && AsmBuffer[i]->token != T_COMMA; i++ );

        sym = AsmGetSymbol( token );

        if( sym != NULL && sym->mem_type != SYM_UNDEFINED ) {
            return( PubDef( 0 ) ); // it is defined here, so make a pubdef
        }

        if( sym == NULL ) {
            dir = dir_insert( token, TAB_GLOBAL );
            sym = (struct asm_sym*)dir;
        } else {
            dir = (dir_node*)sym;
        }

        if( dir == NULL ) return( ERROR );

        GetSymInfo( sym );
        sym->state = SYM_UNDEFINED;
        sym->offset = 0;
        sym->mem_type = TypeInfo[type].value;

        SetMangler( sym, mangle_type );

        /* put the symbol in the globaldef table */
        qnode = AsmAlloc( sizeof( queuenode ) );
        qnode->data = (void *)dir;

        if( GlobalQueue == NULL ) {
            GlobalQueue = AsmAlloc( sizeof( qdesc ) );
            QInit( GlobalQueue );
        }
        QEnqueue( GlobalQueue, qnode );
    }
    return( NOT_ERROR );
}

asm_sym *MakeExtern( char *name, int type, bool already_defd )
/************************************************************/
{
    dir_node    *ext;
    struct asm_sym *sym;

    if( already_defd ) {
        char    *tmp;
        // in case sym->name was passed in
        tmp = AsmTmpAlloc( strlen( name ) + 1 );
        strcpy( tmp, name );
        AsmTakeOut( name );
        name = tmp;
    }
    ext = dir_insert( name, TAB_EXT );

    if( ext == NULL ) return( NULL );
    sym = (struct asm_sym *)ext;
    ext->e.extinfo->idx = ++extdefidx;

    GetSymInfo( sym );
    sym->state = SYM_EXTERNAL;
    sym->offset = 0;
    sym->mem_type = type;
    return( sym );
}

int ExtDef( int i )
/*****************/
{
    char                *token;
    char                *mangle_type = NULL;
    char                *typetoken;
    int                 type;
    int                 mem_type;
    struct asm_sym      *sym;
    struct asm_sym      *struct_sym;

    mangle_type = Check4Mangler( &i );
    for( ; i < Token_Count; i++ ) {
        /* get the symbol name */
        token = AsmBuffer[i++]->string_ptr;

        /* go past the colon */
        if( AsmBuffer[i]->token != T_COLON ) {
            AsmError( COLON_EXPECTED );
            return( ERROR );
        }
        i++;

        typetoken = AsmBuffer[i]->string_ptr;
        type = token_cmp( &typetoken, TOK_EXT_NEAR, TOK_EXT_ABS );

        if( type == ERROR ) {
            struct_sym = AsmGetSymbol( AsmBuffer[i]->string_ptr );
            if( struct_sym == NULL || struct_sym->state != SYM_STRUCT ) {
                AsmError( INVALID_QUALIFIED_TYPE );
                return( ERROR );
            }
            mem_type = T_STRUCT;
        } else {
            mem_type = TypeInfo[type].value;
        }
        for( ; i< Token_Count && AsmBuffer[i]->token != T_COMMA; i++ );

        sym = AsmGetSymbol( token );

        if( sym != NULL ) {
            if( sym->mem_type == SYM_UNDEFINED ) {
                if( MakeExtern( token, mem_type, TRUE ) == NULL ) {
                    return( ERROR );
                }
            } else if( sym->mem_type != mem_type ) {
                AsmError( EXT_DEF_DIFF );
                return( ERROR );
            }
        } else {
            if( MakeExtern( token, mem_type, FALSE ) == NULL ) {
                return( ERROR );
            }
        }
    }
    sym = AsmGetSymbol( token );
    SetMangler( sym, mangle_type );
    return( NOT_ERROR );
}

static char *Check4Mangler( int *i )
/**********************************/
{
    char *mangle_type = NULL;

    if( AsmBuffer[*i]->token == T_STRING ) {
        mangle_type = AsmBuffer[*i]->string_ptr;
        (*i)++;
        if( AsmBuffer[*i]->token != T_COMMA ) {
            AsmWarn( 2, EXPECTING_COMMA );
        } else {
            (*i)++;
        }
    }
    return( mangle_type );
}

static void MakePublic( void *node )
{
    struct queuenode    *qnode;

    qnode = AsmAlloc( sizeof( queuenode ) );
    qnode->data = node;

    if( PubQueue == NULL ) {
        PubQueue = AsmAlloc( sizeof( qdesc ) );
        QInit( PubQueue );
    }
    QEnqueue( PubQueue, qnode );
}

int PubDef( int i )
/*****************/
{
    char                *mangle_type = NULL;
    char                *token;
    struct asm_sym      *sym;
    dir_node            *node;

    mangle_type = Check4Mangler( &i );
    for( ; i < Token_Count; i+=2 ) {
        token = AsmBuffer[i]->string_ptr;
        /* Add the public name */

        if( checkword( &token ) == ERROR ) {
            AsmError( EXPECTING_COMMA );
            return( ERROR );
        }

        sym = AsmGetSymbol( token );
        if( sym != NULL ) {
            node = (dir_node *)sym;
            if( sym->state == SYM_CONST ) {
                /* check if the symbol expands to another symbol,
                 * and if so, expand it */
                 if( node->e.constinfo->data[0].token == T_ID ) {
                    ExpandTheWorld( i, FALSE );
                    return( PubDef( i ) );
                 }
            }
            /* make the existing symbol public */

            switch( sym->state ) {
            case SYM_UNDEFINED:
            case SYM_INTERNAL:
            case SYM_EXTERNAL:
            case SYM_STACK:
            case SYM_CONST:
                if( sym->public ) {
                    return( NOT_ERROR );
                }
                sym->public = TRUE;
                // fixme
                break;
            case SYM_PROC:
                sym->public = TRUE;
                if( node->e.procinfo->visibility != VIS_EXPORT ) {
                    node->e.procinfo->visibility = VIS_PUBLIC;
                }
                break;
            }
        } else {
            node = dir_insert( token, TAB_PUB );
        }
        SetMangler( &node->sym, mangle_type );
        /* put it into the pub table */

        MakePublic( node );
    }

    return( NOT_ERROR );
}

static dir_node *CreateGroup( char *name )
/****************************************/
{
    dir_node    *grp;

    grp = (dir_node *)AsmGetSymbol( name );

    if( grp == NULL ) {
        grp = dir_insert( name, TAB_GRP );

        myassert( grp->sym.state == SYM_UNDEFINED );
        grp->sym.segidx = 0;
        grp->sym.grpidx = grpdefidx;
        grp->sym.state = SYM_GRP;
        grp->sym.offset = 0;
        LnameInsert( name );
    } else if( grp->sym.state != SYM_GRP ) {
        AsmErr( SYMBOL_PREVIOUSLY_DEFINED, name );
        grp = NULL;
    }
    return( grp );
}

int GrpDef( int i )
{
    char        *name;
    dir_node    *grp;
    seg_list    *new;
    seg_list    *curr;
    seg_list    **owner;

    if( i < 0 ) {
        AsmError( GRP_NAME_MISSING );
        return( ERROR );
    }

    name = AsmBuffer[i]->string_ptr;
    grp = CreateGroup( name );
    if( grp == NULL ) return( ERROR );

    for( i+=2;              // skip over the GROUP directive
         i < Token_Count;   // stop at the end of the line
         i+=2 ) {           // skip over commas
        name = AsmBuffer[i]->string_ptr;
        /* Add the segment name */

        if( checkword( &name ) == ERROR ) {
            AsmError( EXPECTING_COMMA );
            return( ERROR );
        }

        new = AsmAlloc( sizeof(seg_list) );
        new->seg = (dir_node *)AsmGetSymbol( name );
        if( new->seg == NULL ) {
            // fixme
            new->seg = dir_insert( name, TAB_SEG );
            new->seg->e.seginfo->lname_idx = 0;
        } else if( new->seg->sym.state != SYM_SEG ) {
            AsmErr( SYMBOL_PREVIOUSLY_DEFINED, name );
            return( ERROR );
        } else {
            /* set the grp index of the segment */
            new->seg->e.seginfo->idx = grp->sym.grpidx;
        }
        /* set the new symbol's grpidx, as a flag that it was properly def'd
         * by a group stmt. */
        new->seg->sym.grpidx = grp->sym.grpidx;

        /* insert the seg. into the linked list */
        owner = &grp->e.grpinfo->seglist;
        for( ;; ) {
            curr = *owner;
            if( curr == NULL ) {
                *owner = new;
                new->next = NULL;
                grp->e.grpinfo->numseg++;
                break;
            }
            if( curr->seg == new->seg ) {
                /* seg is already in the group */
                AsmFree( new );
                break;
            }
            owner = &curr->next;
        }
    }
    return( NOT_ERROR );
}

int  SetCurrSeg( int i )
/**********************/
{
    char        *name;
    struct asm_sym      *sym;

    name = AsmBuffer[i]->string_ptr;
    Use32 = ModuleInfo.use32;

    if( write_to_file ) {
        switch( AsmBuffer[i+1]->value ) {
        case T_SEGMENT:

            /* Flushes the data in current segment, then set up the new
               current segment */

            FlushCurrSeg();

            sym = AsmGetSymbol( name );

            /**/ myassert( sym != NULL );
            push_seg( (dir_node *)sym );

            break;
        case T_ENDS:
            FlushCurrSeg();
            pop_seg();
            break;
        default:
            break;
        }
    }
    if( CurrSeg != NULL ) {
        if( CurrSeg->seg->e.seginfo->segrec->d.segdef.class_name_idx
            == FindClassLnameIdx( "code" ) ) {
            Globals.code_seg = TRUE;
        } else {
            Globals.code_seg = FALSE;
        }
    }
    find_use32();
    return( NOT_ERROR );
}

static int token_cmp( char **token, int start, int end )
/* compare token against those specified in TypeInfo[ start...end ] */
{
    int         i;
    char        *str;

    str = *token;

    /* Check to see if it is a class name */
    if( (*str == '\'') || (*str == '"') ) {
        // it is the class name
        (*token)++;
        for( str = *token; ; str++ ) {
            if( ( *str == '\'' ) || ( *str == '"' ) ) {
                if( *(str+1) == '\0' ) {
                    break;
                } else {
                    *str = '\0'; // so that the following 'if' statement
                                 // will detect an error
                }
            }
            if( *str == '\0' ) {
                AsmError( SEGMENT_DEF_ERROR );
                return( ERROR );
            }
        }
        *str = '\0';    // replace the ' with \0
        return( TOK_CLASS );

    } else {

        for( i = start; i <= end; i++ ) {
            if( stricmp( TypeInfo[i].string, str ) == 0 ) {
                // type is found
                return( i );
            }
        }
        return( ERROR );        // No type is found
    }
}

static void find_use32( void )
{
    if( CurrSeg == NULL ) {
        Use32 = ModuleInfo.use32;
    } else {
        Use32 = CurrSeg->seg->e.seginfo->segrec->d.segdef.use_32;
    }
}

int SegDef( int i )
/*****************/
{
    char                defined = FALSE;
    char                *token;
    obj_rec             *seg;
    obj_rec             *oldobj;
    direct_idx          classidx;
    uint                type;           // type of option
    uint                initstate = 0;  // to show if a field is initialized
    char                oldreadonly;    // readonly value of a defined segment
    bool                ignore;
    dir_node            *dirnode;
    char                *name;
    struct asm_sym      *sym;

    if( i < 0 ) {
        AsmError( SEG_NAME_MISSING );
        return( ERROR );
    }

    name = AsmBuffer[i]->string_ptr;

    switch( AsmBuffer[i+1]->value ) {

        case T_SEGMENT:
            seg = ObjNewRec( CMD_SEGDEF );

            /* Check to see if the segment is already defined */
            sym = AsmGetSymbol( name );
            if( sym != NULL && ( sym->state == SYM_SEG || sym->grpidx != 0 ) ) {
                // segment already defined
                dirnode = (dir_node *)sym;
                defined = TRUE;
                oldreadonly = dirnode->e.seginfo->readonly;
                ignore = dirnode->e.seginfo->ignore;
                if( dirnode->e.seginfo->lname_idx == 0 ) {
                    // segment was mentioned in a group statement, but not really set up
                    defined = FALSE;
                    LnameInsert( name );
                    seg->d.segdef.idx = ++segdefidx;
                }
            } else {
                if( sym != NULL && sym->state != SYM_SEG ) {
                    AsmTakeOut( name );
                }
                dirnode = dir_insert( name, TAB_SEG );
                seg->d.segdef.idx = ++segdefidx;
            }

            /* Setting up default values */
            if( !defined ) {
                seg->d.segdef.align = ALIGN_PARA;
                seg->d.segdef.combine = COMB_INVALID;
                seg->d.segdef.use_32 = ( (Code->info.cpu&P_CPU_MASK) >= P_386 ) ? TRUE : FALSE;
                seg->d.segdef.class_name_idx = 1;
                /* null class name, in case none is mentioned */
                dirnode->e.seginfo->readonly = FALSE;
            } else {
                oldobj = dirnode->e.seginfo->segrec;
                dirnode->e.seginfo->readonly = oldreadonly;
                seg->d.segdef.align = oldobj->d.segdef.align;
                seg->d.segdef.combine = oldobj->d.segdef.combine;
                if( !ignore ) {
                    seg->d.segdef.use_32 = oldobj->d.segdef.use_32;
                } else {
                    seg->d.segdef.use_32 = ( (Code->info.cpu&P_CPU_MASK) >= P_386 ) ? TRUE : FALSE;
                }
                seg->d.segdef.class_name_idx = oldobj->d.segdef.class_name_idx;
            }
            dirnode->e.seginfo->ignore = FALSE; // always false unless set explicitly
            seg->d.segdef.access_valid = FALSE;
            seg->d.segdef.seg_length = 0;

            if( lastseg.stack_size > 0 ) {
                seg->d.segdef.seg_length = lastseg.stack_size;
                lastseg.stack_size = 0;
            }

            i+=2; /* go past segment name and "SEGMENT " */

            for( ; i < Token_Count; i ++ ) {
                if( AsmBuffer[i]->token == T_STRING ) {
                    /* the class name - the only token which is of type STRING */
                    token = AsmBuffer[i]->string_ptr;

                    classidx = FindClassLnameIdx( token );
                    if( classidx == LNAME_NULL ) {
                        classidx = InsertClassLname( token );
                        if( classidx == LNAME_NULL ) return( ERROR );
                    }
                    seg->d.segdef.class_name_idx = classidx;
                    Globals.code_seg = ( stricmp( token, "code" ) == 0 );
                    continue;
                }

                /* go through all tokens EXCEPT the class name */
                token = AsmBuffer[i]->string_ptr;

                // look up the type of token
                type = token_cmp( &token, TOK_READONLY, TOK_AT );
                if( type == ERROR ) {
                    AsmError( UNDEFINED_SEGMENT_OPTION );
                    return( ERROR );
                }

                /* initstate is used to check if any field is already
                   initialized */

                if( ( initstate & TypeInfo[type].init ) ) {
                    AsmError( SEGMENT_PARA_DEFINED ); // initialized already
                    return( ERROR );
                } else {
                    initstate |= TypeInfo[type].init; // mark it initialized
                }
                switch( type ) {
                    case TOK_READONLY:
                        dirnode->e.seginfo->readonly = TRUE;
                        break;
                    case TOK_BYTE:
                    case TOK_WORD:
                    case TOK_DWORD:
                    case TOK_PARA:
                    case TOK_PAGE:
                        seg->d.segdef.align = TypeInfo[type].value;
                        break;
                    case TOK_PRIVATE:
                    case TOK_PUBLIC:
                    case TOK_STACK:
                    case TOK_COMMON:
                    case TOK_MEMORY:
                        seg->d.segdef.combine = TypeInfo[type].value;
                        break;
                    case TOK_USE16:
                    case TOK_USE32:
                        seg->d.segdef.use_32 = TypeInfo[type].value;
                        break;
                    case TOK_IGNORE:
                        dirnode->e.seginfo->ignore = TRUE;
                        break;
                    case TOK_AT:
                        AsmError( SEGDEF_AT_NOT_SUPPORTED );
                        return( ERROR );
                        break;
                    default:
                        AsmError( UNDEFINED_SEGMENT_OPTION );
                        return( ERROR );
                }
            }

            if( defined && !ignore && !dirnode->e.seginfo->ignore ) {
                /* Check if new definition is different from previous one */

                oldobj = dirnode->e.seginfo->segrec;
                if( oldreadonly != dirnode->e.seginfo->readonly ||
                    oldobj->d.segdef.align != seg->d.segdef.align ||
                    oldobj->d.segdef.combine != seg->d.segdef.combine ||
                    oldobj->d.segdef.use_32 != seg->d.segdef.use_32 ||
                    oldobj->d.segdef.class_name_idx !=  seg->d.segdef.class_name_idx
                  ) {
                    ObjKillRec( seg );
                    AsmError( SEGDEF_CHANGED );
                    return( ERROR );
                } else {
                    // definition is the same as before
                    ObjKillRec( seg );
                    if( push_seg( dirnode ) == ERROR ) {
                        return( ERROR );
                    }
                }
            } else if( defined && ignore ) {
                /* reset to the new values */
                oldobj = dirnode->e.seginfo->segrec;
                oldobj->d.segdef.align = seg->d.segdef.align;
                oldobj->d.segdef.combine = seg->d.segdef.combine;
                oldobj->d.segdef.use_32 = seg->d.segdef.use_32;
                oldobj->d.segdef.class_name_idx = seg->d.segdef.class_name_idx;
                dirnode->sym.state = SYM_SEG;

                ObjKillRec( seg );
                if( push_seg( dirnode ) == ERROR ) {
                    return( ERROR );
                }
            } else if( defined && dirnode->e.seginfo->ignore ) {
                /* keep the old values */
                dirnode->e.seginfo->readonly = oldreadonly;
                dirnode->e.seginfo->ignore = ignore;
                ObjKillRec( seg );
                if( push_seg( dirnode ) == ERROR ) {
                    return( ERROR );
                }
            } else {
                /* A new definition */
                dirnode->e.seginfo->segrec = seg;
                dirnode->e.seginfo->start_loc = 0;
                dirnode->e.seginfo->current_loc = 0;
                if( push_seg( dirnode ) == ERROR ) {
                    return( ERROR );
                }
                sym = (struct asm_sym *)dirnode;
                myassert( sym->state == SYM_UNDEFINED );
                sym->state = SYM_SEG;
                GetSymInfo( sym );
                sym->offset = 0;
                LnameInsert( name );

            }
            break;
        case T_ENDS:
            if( CurrSeg == NULL ) {
                AsmError( SEGMENT_NOT_OPENED );
                return( ERROR );
            }

            sym = AsmGetSymbol( name );
            if( sym == NULL ) {
                AsmError( SEG_NOT_DEFINED );
                return( ERROR );
            }
            if( (struct dir_node *)sym != CurrSeg->seg ) {
                AsmError( BLOCK_NESTING_ERROR );
                return( ERROR );
            }
            pop_seg();
            if( CurrSeg == NULL ) break;
            if( CurrSeg->seg->e.seginfo->segrec->d.segdef.class_name_idx
                == FindClassLnameIdx( "code" ) ) {
                Globals.code_seg = TRUE;
            } else {
                Globals.code_seg = FALSE;
            }
            break;
        default:
            return( ERROR );
    }
    find_use32();
    return( NOT_ERROR );
}

int Include( int i )
/******************/
{
    switch( AsmBuffer[i]->token ) {
    case T_ID:
    case T_STRING:
    case T_PATH:
        return( InputQueueFile( AsmBuffer[i]->string_ptr ) );
    default:
        AsmError( EXPECTED_FILE_NAME );
        return( ERROR );
    }
}

int IncludeLib( int i )
/*********************/
{
    char *name;
    struct asm_sym *sym;

    name = AsmBuffer[i]->string_ptr;
    if( name == NULL ) {
        AsmError( LIBRARY_NAME_MISSING );
        return( ERROR );
    }

    sym = AsmGetSymbol( name );
    if( sym == NULL ) {
        // fixme
        if( dir_insert( name, TAB_LIB ) == NULL ) {
            return( ERROR );
        }
    }
    return( NOT_ERROR );
}

static int find_bit( void )
{
    if( ModuleInfo.use32 ) {
        return( BIT32 );
    } else {
        return( BIT16 );
    }
}

static void input_group( int type )
/* emit any GROUP instruction */
{
    char        buffer[MAX_LINE_LEN];

    if( ModuleInfo.model == MOD_FLAT ) return;
    strcpy( buffer, "DGROUP GROUP " );

    switch( type ) {
        case T_DATA:
            strcat( buffer, "_DATA" );
            break;
        case T_DATA_UN:
            strcat( buffer, "_BSS" );
            break;
        case T_CONST:
            strcat( buffer, "CONST" );
            break;
        case T_STACK:
            strcat( buffer, "STACK" );
    }
    InputQueueLine( buffer );
}

static void close_lastseg( void )
/* close the last opened simplified segment */
{
    if( lastseg.seg != SIM_NONE ) {
        lastseg.seg = SIM_NONE;
        if( CurrSeg == NULL && *lastseg.close != '\0' ) {
            AsmError( DONT_MIX_SIM_WITH_REAL_SEGDEFS );
            return;
        }
        InputQueueLine( lastseg.close );
    }
}
int Startup( int i )
/******************/
{
    /* handles .STARTUP directive */

    int         count;
    char        buffer[ MAX_LINE_LEN ];

    if( !ModuleInfo.init ) {
        AsmError( MODEL_IS_NOT_DECLARED );
        return( ERROR );
    }
    ModuleInfo.cmdline = FALSE;

    switch( AsmBuffer[i]->value ) {
    case T_STARTUP:
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
    case T_EXIT:
        i++;
        if( AsmBuffer[i]->string_ptr != NULL &&
             *(AsmBuffer[i]->string_ptr) != '\0' ) {
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
    return( NOT_ERROR );
}

int SimSeg( int i )
/*****************/
/* Handles simplified segment, based on optasm pg. 142-146 */
{
    char        buffer[ MAX_LINE_LEN ];
    int         bit;
    char        *string;
    int         type;
    int         seg;

    if( !ModuleInfo.init ) {
        AsmError( MODEL_IS_NOT_DECLARED );
        return( ERROR );
    }
    ModuleInfo.cmdline = FALSE;

    if( AsmBuffer[i]->value != T_STACK ) {
        close_lastseg();
    }
    buffer[0] = '\0';
    bit = find_bit();
    type = AsmBuffer[i]->value;
    i++; /* get past the directive token */
    if( i < Token_Count ) {
        string = AsmBuffer[i]->string_ptr;
    } else {
        switch( type ) {
        case T_CODE:
            string = Options.text_seg;
            break;
        case T_DATA:
            string = Options.data_seg;
            break;
        default:
            string = NULL;
        }
    }

    switch( type ) {
    case T_CODE:
        strcpy( buffer, string );
        /* we already have a name */
        strcat( buffer, SimCodeBegin[bit][SIM_CODE] + SIM_CODE_OFFSET  );
        InputQueueLine( buffer );
        strcpy( lastseg.close, string );
        strcat( lastseg.close, SimCodeEnd[SIM_CODE] + SIM_CODE_OFFSET );
        lastseg.seg = SIM_CODE;

        switch( ModuleInfo.model ) {
            case MOD_TINY:
                InputQueueLine( "ASSUME CS:DGROUP" );
                break;
            case MOD_SMALL:
            case MOD_COMPACT:
            case MOD_FLAT:                              // may not be correct
                strcpy( buffer, "ASSUME CS: " );
                if( string != NULL ) {
                    strcat( buffer, string );
                } else {
                    strcat( buffer, "_TEXT" );
                }
                InputQueueLine( buffer );
                break;
            case MOD_MEDIUM:
            case MOD_LARGE:
            case MOD_HUGE:
                strcpy( buffer, "ASSUME CS:" );
                if( string != NULL ) {
                    strcat( buffer, string );
                } else {
                    strcat( buffer, ModuleInfo.name );
                    strcat( buffer, "_TEXT" );
                }
                InputQueueLine( buffer );
                break;
            default:
                break;
        }
        break;
    case T_STACK:
        InputQueueLine( SimCodeBegin[bit][SIM_STACK] );
        input_group( type );
        InputQueueLine( SimCodeEnd[SIM_STACK] );
        if( i < Token_Count ) {
            if( AsmBuffer[i]->token != T_NUM ) {
                AsmError( CONSTANT_EXPECTED );
                return( ERROR );
            } else {
                lastseg.stack_size = AsmBuffer[i]->value;
            }
        } else {
            lastseg.stack_size = DEFAULT_STACK_SIZE;
        }
        break;
    case T_DATA:
    case T_DATA_UN:             // .data?
    case T_CONST:
        if( type == T_DATA ) {
            seg = SIM_DATA;
        } else if( type == T_DATA_UN ) {
            seg = SIM_DATA_UN;
        } else {
            seg = SIM_CONST;
        }

        if( string != NULL && type == T_DATA ) {
            strcpy( buffer, string );
            /* we already have a name */
            strcat( buffer, SimCodeBegin[bit][seg] + SIM_DATA_OFFSET  );
        } else {
            strcpy( buffer, SimCodeBegin[bit][seg] );
        }
        InputQueueLine( buffer );
        InputQueueLine( "ASSUME CS:ERROR" );
        input_group( type );
        lastseg.seg = seg;
        if( string != NULL ) {
            strcpy( lastseg.close, string );
            strcat( lastseg.close, SimCodeEnd[seg] + SIM_DATA_OFFSET );
        } else {
            strcpy( lastseg.close, SimCodeEnd[seg] );
        }
        break;
    case T_FARDATA:
    case T_FARDATA_UN:  // .fardata?
        seg = type == T_FARDATA ? SIM_FARDATA : SIM_FARDATA_UN;
        if( string != NULL ) {
            strcpy( buffer, string );
            strcat( buffer, SimCodeBegin[bit][seg] + FAROFFSET(seg) );
        } else {
            strcpy( buffer, SimCodeBegin[bit][seg] );
        }
        InputQueueLine( buffer );
        InputQueueLine( "ASSUME CS:ERROR" );
        if( string != NULL ) {
            strcpy( lastseg.close, string );
            strcat( lastseg.close, SimCodeEnd[seg] + FAROFFSET(seg) );
        } else {
            strcpy( lastseg.close, SimCodeEnd[seg] );
        }
        lastseg.seg = seg;
        break;
    default:
        /**/myassert( 0 );
        break;
    }
    return( NOT_ERROR );
}

static void module_prologue( int type )
/* Generates codes for .MODEL; based on optasm pg.142-146 */
{
    int         bit;
    char        buffer[ MAX_LINE_LEN ];

    bit = find_bit();

    /* Generates codes for code segment */
    strcpy( buffer, Options.text_seg );
    strcat( buffer, SimCodeBegin[bit][SIM_CODE] + SIM_CODE_OFFSET );
    InputQueueLine( buffer );
    strcpy( buffer + strlen( Options.text_seg ), SimCodeEnd[SIM_CODE] + SIM_CODE_OFFSET );
    InputQueueLine( buffer );

    /* Generates codes for data segment */
    if( type != MOD_TINY ) {
        InputQueueLine( SimCodeBegin[bit][SIM_DATA] );
        InputQueueLine( SimCodeEnd[SIM_DATA] );
    }

    /* Generates codes for grouping */
    switch( type ) {
        case MOD_TINY:
            InputQueueLine( "DGROUP GROUP _TEXT" );
            break;
        case MOD_SMALL:
        case MOD_COMPACT:
        case MOD_MEDIUM:
        case MOD_LARGE:
        case MOD_HUGE:
            InputQueueLine( "DGROUP GROUP _DATA" );
            break;
    }

    /* Generates codes for assume */
    if( type == MOD_FLAT ) {
        InputQueueLine( "ASSUME CS:FLAT,DS:FLAT,SS:FLAT,ES:FLAT,FS:ERROR,GS:ERROR");
    } else {
        InputQueueLine( "ASSUME DS:DGROUP, SS:DGROUP" );
        switch( type ) {
            case MOD_TINY:
                InputQueueLine( "ASSUME CS:DGROUP, ES:DGROUP" );
                break;
            case MOD_SMALL:
            case MOD_COMPACT:
            case MOD_MEDIUM:
            case MOD_LARGE:
            case MOD_HUGE:
                strcpy( buffer, "ASSUME CS:" );
                strcat( buffer, Options.text_seg );
                InputQueueLine( buffer );
                break;
        }
    }
}

void ModuleInit( void )
/**********************/
{
    ModuleInfo.model = MOD_NONE;
    ModuleInfo.distance = STACK_NONE;
    ModuleInfo.langtype = LANG_NONE;
    ModuleInfo.ostype = OPSYS_DOS;
    ModuleInfo.use32 = FALSE;
    ModuleInfo.init = FALSE;
    ModuleInfo.cmdline = FALSE;
}

static void get_module_name( void )
{
    char dummy[_MAX_EXT];
    char        *p;

    /**/myassert( AsmFiles.fname[ASM] != NULL );
    _splitpath( AsmFiles.fname[ASM], NULL, NULL, ModuleInfo.name, dummy );
    for( p = ModuleInfo.name; *p != '\0'; ++p ) {
        if( !(isalnum( *p ) || *p == '_' || *p == '$' || *p == '@' || *p == '?') ) {
            /* it's not a legal character for a symbol name */
            *p = '_';
        }
    }
    /* first character can't be a number either */
    if( isdigit( ModuleInfo.name[0] ) ) ModuleInfo.name[0] = '_';

}

#if 0
void MakeConstant( long token )
/*****************************/
{
    char buffer[20];

    /* define a macro */
    buffer[0]='\0';
    strcat( buffer, "__" );
    GetInsString( (enum asm_token)token, buffer+2, 18 );
    strcat( buffer, "__" );
    strupr( buffer );
    StoreConstant( buffer, "1", TRUE );
}
#endif

void set_text_seg_name( void )
/****************************/
{
    int len;

    /* set Options.text_seg based on module name */
    if( Options.text_seg == NULL ) {
        if( ModuleInfo.model >= MOD_MEDIUM ) {
            len = strlen( ModuleInfo.name ) + 1 + SIM_CODE_OFFSET;
            Options.text_seg = AsmAlloc( len );
            Options.text_seg[len-1] = 0;
            strcpy( Options.text_seg, ModuleInfo.name );
            strncat( Options.text_seg, SimCodeBegin[Use32][SIM_CODE], SIM_CODE_OFFSET );
        } else {
            Options.text_seg = AsmAlloc( SIM_CODE_OFFSET + 1 );
            Options.text_seg[SIM_CODE_OFFSET] = 0;
            strncpy( Options.text_seg, SimCodeBegin[Use32][SIM_CODE], SIM_CODE_OFFSET );
        }
    }
    return;
}

void DefFlatGroup()
{
    dir_node    *grp;

    if( ModuleInfo.flat_idx == 0 ) {
        grp = CreateGroup( "FLAT" );
        if( grp != NULL ) ModuleInfo.flat_idx = grp->sym.grpidx;
    }
}

int Model( int i )
/****************/
{
    char        *token;
    int         initstate = 0;
    uint        type;           // type of option

    if( ModuleInfo.init && !ModuleInfo.cmdline ) {
        AsmError( MODEL_DECLARED_ALREADY );
        return( ERROR );
    }
    ModuleInfo.cmdline = FALSE;

    get_module_name();

    for( i++; i < Token_Count; i++ ) {

        token = AsmBuffer[i]->string_ptr;
        wipe_space( token );
        /* Add the public name */

        // look up the type of token
        type = token_cmp( &token, TOK_TINY, TOK_FARSTACK );
        if( type == ERROR ) {
            type = token_cmp( &token, TOK_PROC_BASIC, TOK_PROC_SYSCALL );
            if( type == ERROR ) {
                type = token_cmp( &token, TOK_OS_OS2, TOK_OS_DOS );
                if( type == ERROR ) {
                    AsmError( UNDEFINED_MODEL_OPTION );
                    return( ERROR );
                }
            }
        }
        MakeConstant( AsmBuffer[i]->value );

        if( initstate & TypeInfo[type].init ) {
            AsmError( MODEL_PARA_DEFINED ); // initialized already
            return( ERROR );
        } else {
            initstate |= TypeInfo[type].init; // mark it initialized
        }
        switch( type ) {
            case TOK_FLAT:
                DefFlatGroup();
            case TOK_TINY:
            case TOK_SMALL:
            case TOK_COMPACT:
            case TOK_MEDIUM:
            case TOK_LARGE:
            case TOK_HUGE:
                ModuleInfo.model = TypeInfo[type].value;
                set_text_seg_name();
                module_prologue( ModuleInfo.model );
                break;
            case TOK_NEARSTACK:
            case TOK_FARSTACK:
                ModuleInfo.distance = TypeInfo[type].value;
                break;
            case TOK_PROC_BASIC:
            case TOK_PROC_FORTRAN:
            case TOK_PROC_PASCAL:
            case TOK_PROC_C:
            case TOK_PROC_STDCALL:
            case TOK_PROC_SYSCALL:
                ModuleInfo.langtype = TypeInfo[type].value;
                break;
            case TOK_OS_DOS:
            case TOK_OS_OS2:
                ModuleInfo.ostype = TypeInfo[type].value;
                break;
        }
        i++;

        /* go past comma */
        if( i < Token_Count && AsmBuffer[i]->token != T_COMMA ) {
            AsmError( EXPECTING_COMMA );
            return( ERROR );
        }
    }

    if( ( initstate & INIT_MEMORY ) == 0 ) {
        AsmError( MEMORY_NOT_FOUND );
        return( ERROR );
    }

    lastseg.seg = SIM_NONE;
    lastseg.stack_size = 0;
    ModuleInfo.init = TRUE;
    ModuleInfo.cmdline = (LineNumber == 0);
    return(NOT_ERROR);
}

void AssumeInit( void )
/*********************/
{
    uint reg;

    for( reg = ASSUME_DS; reg <= ASSUME_CS; reg++ ) {
        AssumeTable[reg].symbol = NULL;
        AssumeTable[reg].error = FALSE;
        AssumeTable[reg].flat = FALSE;
    }
}

int SetAssume( int i )
/********************/
/* Handles ASSUME statement */
{
    char        *token;
    char        *segloc;
    uint        reg;
    assume_info *info;
    struct asm_sym      *sym;


    for( i++; i < Token_Count; i++ ) {
        if( AsmBuffer[i]->token==T_RES_ID && AsmBuffer[i]->value==T_NOTHING ) {
            reg = TOK_NOTHING;
            AssumeInit();
            continue;
        }

        token = AsmBuffer[i]->string_ptr;
        wipe_space( token );

        i++;

        if( AsmBuffer[i]->token != T_COLON ) {
            AsmError( COLON_EXPECTED );
            return( ERROR );
        }
        i++;

        if( AsmBuffer[i]->token == T_UNARY_OPERATOR && AsmBuffer[i]->value == T_SEG2 ) {
            i++;
        }

        segloc = AsmBuffer[i]->string_ptr;
        i++;
        if( *segloc == '\0' ) {
            AsmError( SEGLOCATION_EXPECTED );
            return( ERROR );
        }

        /*---- Now store the information ----*/

        reg = token_cmp( &token, TOK_DS, TOK_CS );
        if( reg == ERROR ) {
            AsmError( INVALID_REGISTER );
            return( ERROR );
        }

        info = &(AssumeTable[TypeInfo[reg].value]);

        if( reg <= TOK_CS ) {
            if( token_cmp( &segloc, TOK_ERROR, TOK_ERROR ) != ERROR ) {
                info->error = TRUE;
            } else if( token_cmp( &segloc, TOK_FLAT, TOK_FLAT ) != ERROR ) {
                DefFlatGroup();
                info->flat = TRUE;
            } else {
                sym = AsmGetSymbol( segloc );
                info->symbol = sym;
                info->error = FALSE;
            }
        } else {        // NOTHING
            info->symbol = NULL;
            info->error = FALSE;
            info->flat = FALSE;
        }

        /* go past comma */
        if( i < Token_Count && AsmBuffer[i]->token != T_COMMA ) {
            AsmError( EXPECTING_COMMA );
            return( ERROR );
        }

    }
    return( NOT_ERROR );
}

dir_node *GetSeg( struct asm_sym *sym )
/**************************************/
/* get ptr to sym's segment */
{
    dir_node            *curr;
    uint                idx;

    idx = sym->segidx;
    for( curr = Tables[TAB_SEG].head; curr; curr = curr->next ) {
        if( curr->sym.state != SYM_SEG ) continue;
        if( idx == curr->e.seginfo->segrec->d.segdef.idx ) {
            return( curr );
        }
    }
    return( NULL );
}

static dir_node *get_grp( struct asm_sym *sym )
/*********************************************/
/* get ptr to sym's group */
{
    dir_node            *curr;
    uint                idx;

    idx = sym->grpidx;
    if( idx == 0 ) {
        /* get the group index from the segment */
        curr = GetSeg( sym );
        if( curr != NULL ) {
            idx = curr->e.seginfo->idx;
        }
    }
    for( curr = Tables[TAB_GRP].head; curr; curr = curr->next ) {
        if( idx == curr->e.grpinfo->idx ) {
            return( curr );
        }
    }
    return( NULL );
}

uint GetGrpIdx( struct asm_sym *sym )
/***********************************/
/* get index of sym's group */
{
    dir_node            *curr;
    uint                idx;

    idx = sym->grpidx;
    if( idx == 0 ) {
        /* get the group index from the segment */
        curr = GetSeg( sym );
        if( curr != NULL ) {
            idx = curr->e.seginfo->idx;
        }
    }
    return( idx );
}

int FixOverride( int index )
/**************************/
/* Fix segment or group override */
{
    struct asm_sym      *sym1;
    struct asm_sym      *sym2;
    dir_node            *tmp;

    sym1 = AsmLookup( AsmBuffer[index-2]->string_ptr );
    /**/myassert( sym1 != NULL );

    tmp = GetSeg( sym1 );
    if( tmp != NULL ) {
        sym2 = AsmLookup( AsmBuffer[index]->string_ptr );
        /**/myassert( sym2 != NULL );
        Frame = F_SEG;
        Frame_Datum = sym1->segidx;
        return( NOT_ERROR );
    }

    tmp = get_grp( sym1 );
    if( tmp != NULL ) {
        sym2 = AsmLookup( AsmBuffer[index]->string_ptr );
        /**/myassert( sym2 != NULL );
        Frame = F_GRP;
        Frame_Datum = sym1->grpidx;
        return( NOT_ERROR );
    }
    AsmError( SYNTAX_ERROR );
    return( ERROR );
}

static uint search_assume( dir_node *grp_or_seg, uint def )
/*********************************************************/
{
    uint        reg;
    char        found = FALSE;

    if( grp_or_seg == NULL ) {
        return( ASSUME_NOTHING );
    }

    if( def != ASSUME_NOTHING ) {
        if( AssumeTable[def].symbol == (struct asm_sym *)grp_or_seg ) {
            reg = def;
            found = TRUE;
        }
    }
    if( !found ) {
        for( reg = ASSUME_DS; reg <= ASSUME_CS; reg++ ) {
            if( (struct asm_sym *)grp_or_seg == AssumeTable[reg].symbol ) {
                found = TRUE;
                break;
            }
        }
    }

    if( !found ) {
        return( ASSUME_NOTHING );
    } else {
        return( reg );
    }
}

uint GetPrefixAssume( struct asm_sym* sym, uint prefix )
/******************************************************/
{
    int         type;
    dir_node    *dir;
    struct asm_sym      *assume;

    if( Parse_Pass == PASS_1 ) return( prefix );

    if( AssumeTable[prefix].flat ) {
        Frame = F_GRP;
        Frame_Datum = MAGIC_FLAT_GROUP;
        return( prefix );
    }
    assume = AssumeTable[prefix].symbol;
    if( assume == NULL ) {
        if( sym->state == SYM_EXTERNAL ) {
#if 0 //NYI: Don't know what's going on here
            type = GetCurrGrp();
            if( type != 0 ) {
                Frame = F_GRP;
            } else {
                type = GetCurrSeg();
                /**/myassert( type != 0 );
                Frame = F_SEG;
            }
            Frame_Datum = type;
#endif
            return( prefix );
        } else {
            return( ASSUME_NOTHING );
        }
    }

    dir = (dir_node *)assume;
    if( assume->state == SYM_SEG ) {
        type = TAB_SEG;
    } else if( assume->state == SYM_GRP ) {
        type = TAB_GRP;
    }

    if( get_grp( sym ) == dir ||    // dir is the group for this symbol
        GetSeg( sym ) == dir ||    // dir is the seg. for this symbol
        sym->state == SYM_EXTERNAL ) {

        if( type == TAB_GRP ) {
            Frame = F_GRP;
            if( sym->state == SYM_EXTERNAL ) {
                Frame_Datum = dir->e.grpinfo->idx;
            } else {
                Frame_Datum = sym->grpidx;
            }
        } else {
            Frame = F_SEG;
            if( sym->state == SYM_EXTERNAL ) {
                Frame_Datum = dir->e.seginfo->segrec->d.segdef.idx;
            } else {
                Frame_Datum = sym->segidx;
            }
        }
        return( prefix );
    } else {
        return( ASSUME_NOTHING );
    }
}

uint GetAssume( struct asm_sym* sym, uint def )
/**********************************************/
{
    uint        reg;

    if( AssumeTable[def].flat ) {
        Frame = F_GRP;
        Frame_Datum = MAGIC_FLAT_GROUP;
        return( def );
    }

    reg = search_assume( get_grp( sym ), def );

    if( reg != ASSUME_NOTHING ) {

        Frame = F_GRP;
        Frame_Datum = sym->grpidx;

    } else if( reg == ASSUME_NOTHING ) {

        reg = search_assume( GetSeg( sym ), def );
        if( reg != ASSUME_NOTHING ) {
            if( Frame == EMPTY ) {
                Frame = F_SEG;
                Frame_Datum = sym->segidx;
            }
        } else {
            if( sym->state == SYM_EXTERNAL ) {
                if( Frame == EMPTY ) {
                    Frame = F_EXT;
                    Frame_Datum = GetDirIdx( sym->name, TAB_EXT );
                }
            }
        }

    }

    return( reg );
}

int ModuleEnd( int count )
/*************************/
{
    struct fixup        *fixup;

    if( lastseg.seg != SIM_NONE ) {
        close_lastseg();
        InputQueueLine( CurrString );
        return( NOT_ERROR );
    }
    EndDirectiveFound = TRUE;

    if( Parse_Pass == PASS_1 ) return( NOT_ERROR );

    if( StartupDirectiveFound ) {
        if( count == 2 ) {
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        }
        strcat( CurrString, " " );
        strcat( CurrString, StartAddr );
        InputQueueLine( CurrString );
        StartupDirectiveFound = FALSE;
        return( NOT_ERROR );
    }

    ModendRec = ObjNewRec( CMD_MODEND );
    ModendRec->d.modend.main_module = FALSE;

    if( count == 1 ) {
        ModendRec->d.modend.start_addrs = FALSE;
        return( NOT_ERROR );
    }

    ModendRec->d.modend.start_addrs = TRUE;
    ModendRec->d.modend.is_logical = TRUE;

    if( AsmBuffer[1]->token != T_ID ) {
        AsmError( INVALID_START_ADDRESS );
        return( ERROR );
    }
    ModendRec->d.modend.main_module = TRUE;

    fixup = CreateFixupRec( 0 );
    if( fixup == NULL ) {
        return( ERROR );
    }
    ModendRec->d.modend.ref.log = fixup->lr;
    return( NOT_ERROR );
}

static int find_size( int type )
{
    switch( type ) {
        case TOK_EXT_BYTE:
        case TOK_EXT_SBYTE:
            return( 1 );
            break;
        case TOK_EXT_WORD:
        case TOK_EXT_SWORD:
            return( 2 );
            break;
        case TOK_EXT_DWORD:
        case TOK_EXT_SDWORD:
            return( 4 );
            break;
        case TOK_EXT_FWORD:
        case TOK_EXT_PWORD:
            return( 6 );
            break;
        case TOK_EXT_QWORD:
            return( 8 );
            break;
        case TOK_EXT_TBYTE:
            return( 10 );
            break;
        case TOK_PROC_VARARG:
            return( 0 );
            break;
        default:
            return( ERROR );
    }
}

static void size_override( char *buffer, int size )
{
    switch( size ) {
    default:
    case 0:
        buffer[0] = '\0';
        break;
    case 1:
        strcpy( buffer, " byte ptr " );
        break;
    case 2:
        strcpy( buffer, " word ptr " );
        break;
    case 4:
        strcpy( buffer, " dword ptr " );
        break;
    case 6:
        strcpy( buffer, " fword ptr " );
        break;
    case 8:
        strcpy( buffer, " qword ptr " );
        break;
    case 10:
        strcpy( buffer, " tbyte ptr " );
        break;
    }
}

int LocalDef( int i )
/*******************/
{
    char        *string;
    int         type;
    label_list  *local;
    label_list  *curr;
    proc_info   *info;
    struct asm_sym      *sym;

    if( DefineProc == FALSE ) {
        AsmError( LOCAL_VAR_MUST_FOLLOW_PROC );
        return( ERROR );
    }

    /**/myassert( CurrProc != NULL );

    info = CurrProc->e.procinfo;

    for( i++; i< Token_Count; i++ ) {
        string = AsmBuffer[i]->string_ptr;

        if( AsmBuffer[i]->token != T_ID ) {
            AsmError( LABEL_IS_EXPECTED );
            return( ERROR );
        }

        sym = AsmLookup( AsmBuffer[i]->string_ptr );
        if( sym == NULL ) return( ERROR );

        if( sym->state != SYM_UNDEFINED ) {
            AsmError( SYMBOL_ALREADY_DEFINED );
            return( ERROR );
        } else {
            sym->state = SYM_INTERNAL;
        }

        local = AsmAlloc( sizeof( label_list ) );
        local->label = AsmAlloc( strlen( AsmBuffer[i]->string_ptr ) + 1 );
        strcpy( local->label, AsmBuffer[i++]->string_ptr );
        local->size = LOCAL_DEFAULT_SIZE;
        local->replace = NULL;
        local->factor = 1;
        local->next = NULL;

        if( i < Token_Count ) {
            if( AsmBuffer[i]->token == T_OP_SQ_BRACKET ) {
                i++;
                if( AsmBuffer[i]->token != T_NUM || i >= Token_Count ) {
                    AsmError( SYNTAX_ERROR );
                    return( ERROR );
                }
                local->factor = AsmBuffer[i++]->value;
                if( AsmBuffer[i]->token != T_CL_SQ_BRACKET || i >= Token_Count ) {
                    AsmError( EXPECTED_CL_SQ_BRACKET );
                    return( ERROR );
                }
                i++;
            }
        }

        if( i < Token_Count ) {
            if( AsmBuffer[i]->token != T_COLON ) {
                AsmError( COLON_EXPECTED );
                return( ERROR );
            }
            i++;

            type = token_cmp( &(AsmBuffer[i]->string_ptr), TOK_EXT_BYTE,
                                TOK_EXT_TBYTE );
            if( type == ERROR ) {
                AsmError( INVALID_QUALIFIED_TYPE );
                return( ERROR );
            }
            local->size = find_size( type );
        }

        info->localsize += ( local->size * local->factor );

        if( info->locallist == NULL ) {
            info->locallist = local;
        } else {
            for( curr = info->locallist;; curr = curr->next ) {
                if( curr->next == NULL ) break;
            }
            curr->next = local;
        }

        /* go past comma */
        i++;
        if( i < Token_Count && AsmBuffer[i]->token != T_COMMA ) {
            AsmError( EXPECTING_COMMA );
            return( ERROR );
        }

    }

    return( NOT_ERROR );
}

static int proc_exam( int i )
{
    char        *name;
    char        *token;
    char        *typetoken;
    int_8       minimum;        // Minimum value of the type of token to be read
    int_8       finish;
    dir_node    *dir;
    proc_info   *info;
    regs_list   *regist;
    regs_list   *temp_regist;
    label_list  *paranode;
    label_list  *paracurr;
    int         type;
    int         vis;
    struct asm_sym      *sym;

    name = AsmBuffer[i++]->string_ptr;
    dir = (dir_node *)AsmGetSymbol( name );
    info = dir->e.procinfo;

    minimum = TOK_PROC_FAR;
    finish = FALSE;
    info->langtype = ModuleInfo.langtype;

    // fixme ... we need error checking here --- for nested procs

    /* Obtain all the default value */

    if( ModuleInfo.model <= MOD_FLAT ) {
        info->mem_type = T_NEAR;
    } else {
        info->mem_type = T_FAR;
    }
    info->visibility = dir->sym.public ? VIS_PUBLIC : VIS_PRIVATE;
    info->parasize = 0;
    info->localsize = 0;
    info->is_vararg = FALSE;
    SetMangler( &dir->sym, NULL );

    /* Parse the definition line, except the parameters */
    for( i++; i < Token_Count && AsmBuffer[i]->token != T_COMMA; i++ ) {
        token = AsmBuffer[i]->string_ptr;
        if( AsmBuffer[i]->token == T_STRING ) {
            /* name mangling */
            SetMangler( &dir->sym, token );
            continue;
        }

        type = token_cmp( &token, TOK_PROC_FAR, TOK_PROC_USES );
        if( type == ERROR ) break;
        if( type < minimum ) {
            AsmError( SYNTAX_ERROR );
            return( ERROR );
        }
        switch( type ) {
        case TOK_PROC_FAR:
        case TOK_PROC_NEAR:
            info->mem_type = TypeInfo[type].value;;
            minimum = TOK_PROC_BASIC;
            break;
        case TOK_PROC_BASIC:
        case TOK_PROC_FORTRAN:
        case TOK_PROC_PASCAL:
        case TOK_PROC_C:
        case TOK_PROC_WATCOM_C:
        case TOK_PROC_STDCALL:
        case TOK_PROC_SYSCALL:
            info->langtype = TypeInfo[type].value;
            minimum = TOK_PROC_PRIVATE;
            break;
        case TOK_PROC_PRIVATE:
        case TOK_PROC_PUBLIC:
        case TOK_PROC_EXPORT:
            vis = TypeInfo[type].value;
            if( info->visibility < vis ) {
                info->visibility = vis;
            }
            minimum = TOK_PROC_USES;
            break;
        case TOK_PROC_USES:
            for( i++; i < Token_Count && AsmBuffer[i]->token != T_COMMA; i++ ) {
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
                        if( temp_regist->next == NULL ) break;
                    }
                    temp_regist->next = regist;
                }
            }
            if( AsmBuffer[i]->token == T_COMMA ) i--;
            break;
        default:
            goto parms;
        }
    }

parms:
    CurrProc = dir;
    DefineProc = TRUE;

    if( i >= Token_Count ) {
        return( info->mem_type );
    } else if( info->langtype == LANG_NONE ) {
        AsmError( LANG_MUST_BE_SPECIFIED );
        return( ERROR );
    } else if( AsmBuffer[i]->token == T_COMMA ) {
        i++;
    }

    /* now parse parms */

    for( ; i < Token_Count; i++ ) {
        /* read symbol */
        token = AsmBuffer[i++]->string_ptr;

        /* read colon */
        if( AsmBuffer[i]->token != T_COLON ) {
            AsmError( COLON_EXPECTED );
            return( ERROR );
        }
        i++;

        /* now read qualified type */
        typetoken = AsmBuffer[i]->string_ptr;

        type = token_cmp( &typetoken, TOK_EXT_BYTE, TOK_EXT_TBYTE );

        if( type == ERROR ) {
            type = token_cmp( &typetoken, TOK_PROC_VARARG, TOK_PROC_VARARG );
            if( type == ERROR ) {
                AsmError( INVALID_QUALIFIED_TYPE );
                return( ERROR );
            } else {
                if( info->langtype <= LANG_PASCAL ) {
                    AsmError( VARARG_REQUIRES_C_CALLING_CONVENTION );
                    return( ERROR );
                }
            }
        }

        sym = AsmLookup( token );
        if( sym == NULL ) return( ERROR );

        if( sym->state != SYM_UNDEFINED ) {
            AsmError( SYMBOL_ALREADY_DEFINED );
            return( ERROR );
        } else {
            sym->state = SYM_INTERNAL;
        }

        paranode = AsmAlloc( sizeof( label_list ) );
        paranode->is_vararg = type == TOK_PROC_VARARG ? TRUE : FALSE;
        paranode->size = find_size( type );
        paranode->label = AsmAlloc( strlen( token ) + 1 );
        paranode->replace = NULL;
        strcpy( paranode->label, token );

        if( Use32 ) {
            info->parasize += ROUND_UP( paranode->size, 4 );
        } else {
            info->parasize += ROUND_UP( paranode->size, 2 );
        }
        info->is_vararg |= paranode->is_vararg;

        if( info->langtype >= LANG_BASIC && info->langtype <= LANG_PASCAL ) {

            /* Parameters are stored in reverse order */
            paranode->next = info->paralist;
            info->paralist = paranode;

        } else {

            paranode->next = NULL;
            if( info->paralist == NULL ) {
                info->paralist = paranode;
            } else {
                for( paracurr = info->paralist;; paracurr = paracurr->next ) {
                    if( paracurr->next == NULL ) break;
                }
                paracurr->next = paranode;
            }
        }
        /* go past comma */
        i++;
        if( i < Token_Count && AsmBuffer[i]->token != T_COMMA ) {
            AsmError( EXPECTING_COMMA );
            return( ERROR );
        }
    }
    return( info->mem_type );
}

int ProcDef( int i )
/******************/
{
    struct asm_sym      *sym;
    dir_node            *dir;
    char                *name;

    if( CurrProc != NULL ) {
        /* nested procs ... push currproc on a stack */
        push_proc( CurrProc );
    }

    if( Parse_Pass == PASS_1 ) {
        if( i < 0 ) {
            AsmError( PROC_MUST_HAVE_A_NAME );
            return( ERROR );
        }
        name = AsmBuffer[i]->string_ptr;
        sym = AsmGetSymbol( name );

        if( sym != NULL ) {
            if( sym->state != SYM_UNDEFINED ) {
                AsmError( PROC_ALREADY_DEFINED );
                return( ERROR );
            }
        }

        if( sym == NULL ) {
            dir = dir_insert( name, TAB_PROC );
            sym = &dir->sym;
        } else {
            /* alloc the procinfo struct */
            dir = (dir_node *)sym;
            dir->e.procinfo = AsmAlloc( sizeof( proc_info ) );
            dir->e.procinfo->regslist = NULL;
            dir->e.procinfo->paralist = NULL;
            dir->e.procinfo->locallist = NULL;
            sym->public = TRUE;
            dir_add( dir, TAB_PROC );
        }
        sym->state = SYM_PROC;
        GetSymInfo( sym );
        sym->mem_type = proc_exam( i );
        if( sym->mem_type == ERROR ) {
            return( ERROR );
        }
        return( NOT_ERROR );
    } else {
        // fixme -- nested procs can be ok /**/myassert( CurrProc == NULL );
        /**/myassert( i >= 0 );
        sym = AsmGetSymbol( AsmBuffer[i]->string_ptr );
        CurrProc = (dir_node *)sym;
        /**/myassert( CurrProc != NULL );
        GetSymInfo( sym );
        DefineProc = TRUE;
        return( NOT_ERROR );
    }
}

int ProcEnd( int i )
/******************/
{
    char        *name;
    proc_info   *info;
    label_list  *curr;
    struct asm_sym      *sym;

    if( CurrProc == NULL ) {
        AsmError( NO_PROC_IS_CURRENTLY_DEFINED );
        return( ERROR );
    }
    if( i < 0 ) {
        AsmError( PROC_MUST_HAVE_A_NAME );
        return( ERROR );
    }
    name = AsmBuffer[i]->string_ptr;
    info = CurrProc->e.procinfo;

    sym = AsmGetSymbol( name );

    if( (dir_node *)sym != CurrProc ) {
        AsmError( PROC_NAME_DOES_NOT_MATCH );
        return( ERROR );
    }

    if( Parse_Pass == PASS_1 ) {
        for( curr = info->paralist; curr; curr = curr->next ) {
            AsmTakeOut( curr->label );
        }
        for( curr = info->locallist; curr; curr = curr->next ) {
            AsmTakeOut( curr->label );
        }
    }

    CurrProc = pop_proc();
    return( NOT_ERROR );
}

int WritePrologue( void )
/***********************/
{
    char                buffer[80];
    regs_list           *regist;
    int                 len;
    proc_info           *info;
    label_list          *curr;
    long                offset;
    unsigned long       size;
    int                 retcode;
    uint_8              parm_count = 0;
    int                 align = Use32 ? 4 : 2;

    /**/myassert( CurrProc != NULL );
    info = CurrProc->e.procinfo;

    if( Parse_Pass == PASS_1 ) {

        /* Figure out the replacing string for local variables */
        offset = 0;
        for( curr = info->locallist; curr != NULL; curr = curr->next ) {
            size = curr->size * curr->factor;
            offset += ROUND_UP( size, align );
            size_override( buffer, curr->size );
            sprintf( buffer + strlen(buffer), "%s%d] ",
                Use32 ? LOCAL_STRING_32 : LOCAL_STRING, offset );
            curr->replace = AsmAlloc( strlen( buffer ) + 1 );
            strcpy( curr->replace, buffer );

        }

        /* Figure out the replacing string for the parameters */

        if( info->mem_type == T_NEAR ) {
            offset = 4;         // offset from BP : return addr + old BP
        } else {
            offset = 6;
        }

        if( Use32 ) offset *= 2;

        for( curr = info->paralist; curr; curr = curr->next, parm_count++ ) {
            if( info->langtype == LANG_WATCOM_C ) {
                retcode = get_watcom_argument_string( buffer, curr->size, &parm_count );
                if( retcode == ERROR ) return( ERROR );
            }
            if( info->langtype != LANG_WATCOM_C || retcode == FALSE ) {
                size_override( buffer, curr->size );
                if( Use32 ) {
                    strcat( buffer, ARGUMENT_STRING_32 );
                } else {
                    strcat( buffer, ARGUMENT_STRING );
                }
                sprintf( buffer + strlen( buffer ), "%d", offset );
                strcat( buffer, "] " );

                offset += ROUND_UP( curr->size, align );
            }
            if( *buffer == '\0' ) {
                len = strlen( buffer+1 ) + 2;
                curr->replace = AsmAlloc( len );
                memcpy( curr->replace, buffer, len );
            } else {
                curr->replace = AsmAlloc( strlen( buffer ) + 1 );
                strcpy( curr->replace, buffer );
            }
        }
    }

    if( info->localsize != 0 || info->parasize != 0 || info->is_vararg ) {

        /* Push BP or EBP */
        if( Use32 ) {
            strcpy( buffer, "push ebp" );
        } else {
            strcpy( buffer, "push bp" );
        }
        InputQueueLine( buffer );

        /* Move SP/ESP to BP/EBP */
        if( Use32 ) {
            strcpy( buffer, "mov ebp, esp" );
        } else {
            strcpy( buffer, "mov bp, sp" );
        }
        InputQueueLine( buffer );

        /* Substract SP/ESP by the number of localbytes */
        if( info->localsize != 0 ) {
            if( Use32 ) {
                strcpy( buffer, "sub esp, " );
            } else {
                strcpy( buffer, "sub sp, " );
            }
            sprintf( buffer+strlen(buffer), "%d", info->localsize );
            InputQueueLine( buffer );
        }
    }

    /* Push the registers */
    strcpy( buffer, "push " );
    len = strlen( buffer );
    for( regist = info->regslist; regist; regist = regist->next ) {
        strcpy( buffer + len, regist->reg );
        InputQueueLine( buffer );
    }
    return( NOT_ERROR );
}

static void pop_register( regs_list *regist )
/* Pop the register when a procedure ends */
{
    char        buffer[20];

    if( regist == NULL ) return;
    pop_register( regist->next );
    strcpy( buffer, "pop " );
    strcpy( buffer + strlen( buffer ), regist->reg );
    InputQueueLine( buffer );
}

static void write_epilogue( void )
{
    char        buffer[80];
    proc_info   *info;

    /**/myassert( CurrProc != NULL );
    info = CurrProc->e.procinfo;

    /* Pop the registers */
    pop_register( CurrProc->e.procinfo->regslist );

    if( info->localsize == 0 && info->parasize == 0 && !(info->is_vararg) ) return;

    if( info->localsize != 0 ) {
        /* Move BP/EBP to SP/ESP */
        if( Use32 ) {
            strcpy( buffer, "mov esp, ebp" );
        } else {
            strcpy( buffer, "mov sp, bp" );
        }
        InputQueueLine( buffer );
    }

    /* Pop BP or EBP */
    if( Use32 ) {
        strcpy( buffer, "pop ebp" );
    } else {
        strcpy( buffer, "pop bp" );
    }
    InputQueueLine( buffer );
}

int Ret( int i, int count )
/*************************/
{
    char        buffer[20];
    proc_info   *info;

    info = CurrProc->e.procinfo;

    if( info->mem_type == T_NEAR ) {
        strcpy( buffer, "retn " );
    } else {
        strcpy( buffer, "retf " );
    }

    write_epilogue();
    if( count == i + 1 ) {
        if( ( info->langtype >= LANG_BASIC &&
                info->langtype <= LANG_PASCAL ) ||
            ( info->langtype == LANG_STDCALL && !(info->is_vararg) ) ) {
            if( info->parasize != 0 ) {
                sprintf( buffer + strlen(buffer), "%d", info->parasize );
            }
        }
    } else {
        if( AsmBuffer[i+1]->token != T_NUM ) {
            AsmError( CONSTANT_EXPECTED );
            return( ERROR );
        }
        sprintf( buffer + strlen(buffer), "%d", AsmBuffer[i+1]->value );
    }

    InputQueueLine( buffer );

    return( NOT_ERROR );
}

extern void GetSymInfo( struct asm_sym *sym )
/*******************************************/
{
    sym->segidx = GetCurrSeg();
    sym->grpidx = GetCurrGrp();
    sym->offset = GetCurrAddr();
}

int Comment( int what_to_do, int i  )
/***********************************/
{
    static int in_comment = FALSE;
    static char delim_char;

    switch( what_to_do ) {
    case QUERY_COMMENT:
        return( in_comment );
    case QUERY_COMMENT_DELIM:
        /* return delimiting character */
        return( (int)delim_char );
        break;
    case START_COMMENT:
        i++;
        if( AsmBuffer[i]->string_ptr == NULL ) {
            AsmError( COMMENT_DELIMITER_EXPECTED );
            return( ERROR );
        }
        delim_char = *(AsmBuffer[i]->string_ptr+strspn(AsmBuffer[i]->string_ptr," \t") );
        if( delim_char == NULL ||
            strchr( AsmBuffer[i]->string_ptr, delim_char ) == NULL ) {
            AsmError( COMMENT_DELIMITER_EXPECTED );
            return( ERROR );
        }
        if( strchr( AsmBuffer[i]->string_ptr, delim_char ) !=
            strrchr( AsmBuffer[i]->string_ptr, delim_char ) ) {
            /* we have COMMENT delim. ..... delim. -- only 1 line */
        } else {
            in_comment = TRUE;
        }
        return( NOT_ERROR );
    case END_COMMENT:
        in_comment = FALSE;
        return( NOT_ERROR );
    }
    return( ERROR );
}

int AddAlias( int i )
/*******************/
{
    /* takes directive of form: <alias_name> alias <substitute_name> */
    struct queuenode    *qnode;
    char *tmp;

    if( i < 0 ) {
        AsmError( SYNTAX_ERROR );
        return( ERROR );
    }
    if( AsmBuffer[i]->token != T_ID ||
        AsmBuffer[i+2]->token != T_ID ) {
        AsmError( SYMBOL_EXPECTED );
        return( ERROR );
    }

    /* add this alias to the alias queue */

    /* aliases are stored as:  <len><alias_name><len><substitute_name> */

    qnode = AsmAlloc( sizeof( queuenode ) );
    tmp = AsmAlloc( strlen( AsmBuffer[i]->string_ptr ) +
                    strlen( AsmBuffer[i+2]->string_ptr ) + 2 );
    strcpy( tmp, AsmBuffer[i]->string_ptr );
    qnode->data = (void *)tmp;
    tmp += strlen( tmp ) + 1 ;
    strcpy( tmp, AsmBuffer[i+2]->string_ptr );
    if( AliasQueue == NULL ) {
        AliasQueue = AsmAlloc( sizeof( qdesc ) );
        QInit( AliasQueue );
    }
    QEnqueue( AliasQueue, qnode );
    return( NOT_ERROR );
}

int NameDirective( int i )
/************************/
{
    if( Options.module_name != NULL ) return( NOT_ERROR );
    Options.module_name = AsmAlloc( strlen( AsmBuffer[i+1]->string_ptr ) + 1 );
    strcpy( Options.module_name, AsmBuffer[i+1]->string_ptr );
    return( NOT_ERROR );
}

int MakeComm( char *name, int type, bool already_defd, int number, int distance )
/*******************************************************************************/
{
    dir_node    *dir;
    struct asm_sym *sym;

    if( already_defd ) {
        char    *tmp;
        // in case sym->name was passed in
        tmp = AsmTmpAlloc( strlen( name ) + 1 );
        strcpy( tmp, name );
        AsmTakeOut( name );
        name = tmp;
    }
    dir = dir_insert( name, TAB_COMM );
    // what do we do with number ?
    number = number;

    if( dir == NULL ) return( ERROR );
    sym = (struct asm_sym *)dir;
    dir->e.comminfo->idx = ++extdefidx;
    dir->e.comminfo->size = number;
    dir->e.comminfo->distance = distance;

    GetSymInfo( sym );
    sym->state = SYM_EXTERNAL;
    sym->offset = 0;
    sym->mem_type = type;

    return( NOT_ERROR );
}

int CommDef( int i )
/******************/
{
    char        *token;
    char        *mangle_type = NULL;
    char        *typetoken;
    int         type;
    int         distance;
    int         count;
    struct asm_sym *sym;

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

        /* get the symbol name */
        token = AsmBuffer[i++]->string_ptr;

        /* go past the colon */
        if( AsmBuffer[i]->token != T_COLON ) {
            AsmError( COLON_EXPECTED );
            return( ERROR );
        }
        i++;
        typetoken = AsmBuffer[i]->string_ptr;
        type = token_cmp( &typetoken, TOK_EXT_BYTE, TOK_EXT_TBYTE );

        if( type == ERROR ) {
            AsmError( INVALID_QUALIFIED_TYPE );
            return( ERROR );
        }
        for( ; i< Token_Count && AsmBuffer[i]->token != T_COMMA; i++ ) {
            if( AsmBuffer[i]->token == T_COLON ) {
                i++;
                /* count */
                if( AsmBuffer[i]->token != T_NUM ) {
                    AsmError( EXPECTING_NUMBER );
                    return( ERROR );
                }
                count = AsmBuffer[i]->value;
            }
        }

        sym = AsmGetSymbol( token );

        if( sym != NULL ) {
            SetMangler( sym, mangle_type );
            if( sym->mem_type == SYM_UNDEFINED ) {
                if( MakeComm( token, TypeInfo[type].value, TRUE, count,
                    TypeInfo[distance].value ) == ERROR ) {
                    return( ERROR );
                }
            } else if( sym->mem_type != TypeInfo[type].value ) {
                AsmError( EXT_DEF_DIFF );
                return( ERROR );
            }
        } else {
            if( MakeComm( token, TypeInfo[type].value, FALSE, count,
                    TypeInfo[distance].value ) == ERROR ) {
                return( ERROR );
            }
            sym = AsmGetSymbol( token );
            SetMangler( sym, mangle_type );
        }
    }
    return( NOT_ERROR );
}
