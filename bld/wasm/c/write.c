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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#include "myassert.h"
#include "watcom.h"

#include "asmglob.h"
#include "asmerr.h"
#include "asmsym.h"
#include "asmops1.h"
#include "asmalloc.h"
#include "fatal.h"
#include "directiv.h"

#include "womp.h"
#include "objio.h"
#include "objprs.h"
#include "objrec.h"
#include "pcobj.h"
#include "fixup.h"
#include "queue.h"

#define JUMP_OFFSET(cmd)    ((cmd)-CMD_POBJ_MIN_CMD)

extern char             *ScanLine( char *, int );
extern void             AsmLine( char * );
extern void             AsmSymFini( void );
extern void             AsmError( int );
extern void             FreeIncludePath( void );
extern void             PrepAnonLabels( void );
extern char             *Mangle( struct asm_sym *, char * );
extern void             AsmEvalInit( void );
extern void             AsmEvalFini( void );
extern void             CheckForOpenConditionals();
extern bool             PopLineQueue();

extern uint_32          Address;
extern unsigned char    *CodeBuffer;
extern pobj_state       pobjState;      // for WOMP interface
extern File_Info        AsmFiles;
extern int              Token_Count;    // number of tokens on line
extern pobj_filter      jumpTable[ CMD_MAX_CMD - CMD_POBJ_MIN_CMD + 1 ];
extern seg_list         *CurrSeg;       // points to stack of opened segments
extern symbol_queue     Tables[];       // tables of definitions
extern qdesc            *PubQueue;      // queue of pubdefs
extern qdesc            *GlobalQueue;   // queue of global / externdefs
extern qdesc            *AliasQueue;    // queue of aliases
extern qdesc            *LnameQueue;    // queue of LNAME structs
extern struct fixup     *FixupListHead; // head of list of fixups ( from WOMP )
extern struct fixup     *FixupListTail;
extern uint_32          BufSize;
extern uint             LnamesIdx;      // Number of LNAMES definition
extern obj_rec          *ModendRec;     // Record for Modend
extern sim_seg          LastSimSeg;     // last opened simplified segment
extern module_info      ModuleInfo;     // general info about the module

int                     MacroLocalVarCounter = 0; // counter for temp. var names
char                    Parse_Pass;     // phase of parsing
char                    write_to_file;  // write if there is no error
uint                    LineNumber;
char                    Modend;         // end of module is reached
int_8                   DefineProc;     // TRUE if the definition of procedure
                                        // has not ended
dir_node                *CurrProc;      // current procedure
int_8                   Use32;          // if 32-bit code is use
unsigned long           PassTotal;      // Total number of ledata bytes generated
int_8                   PhaseError;
char                    EndDirectiveFound = FALSE;
extern qdesc            *LinnumQueue;// queue of linnum_data structs

extern uint             segdefidx;      // Number of Segment definition
extern uint             extdefidx;      // Number of Extern definition

char                    **NameArray;

global_vars     Globals = { 0, 0, 0, 0, 0, 0, 0 };

static void write_init( void )
{
#ifndef _WASM_
    Address     = 0;
#endif
    BufSize     = 0;
    FixupListHead = NULL;
    FixupListTail = NULL;
    ModendRec   = NULL;
    CurrProc    = NULL;
    DefineProc  = FALSE;
    Use32       = FALSE;
    write_to_file = TRUE;

    IdxInit();
    LnameInsert( "" );

    ModuleInit();
    FixInit();
    AssumeInit();
}

static void write_fini( void )
{
    FixFini();
}

void write_record( obj_rec *objr, char kill )
{
    /**/myassert( objr != NULL );
    ObjRSeek( objr, 0 );
    jumpTable[ JUMP_OFFSET(objr->command) ] ( objr, &pobjState );
    if( kill ) {
        ObjKillRec( objr );
    }
}

void OutSelect( bool starts )
/***************************/
{
    obj_rec             *objr;
    unsigned long       curr;


    if( starts ) {
        Globals.sel_start = GetCurrAddr();
        Globals.data_in_code = TRUE;
    } else {
        Globals.sel_idx = CurrSeg->seg->sym.segidx;
        Globals.data_in_code = FALSE;
        FlushCurrSeg();

        if( (Parse_Pass > PASS_1) && !PhaseError ) {
            objr = ObjNewRec( CMD_COMENT );
            objr->d.coment.attr = 0x80;
            objr->d.coment.class = CMT_DISASM_DIRECTIVE;

            ObjAllocData( objr, 11 );
            curr = GetCurrAddr();
            if( (Globals.sel_start > 0xffffUL) || (curr > 0xffffUL) ) {
                ObjPut8( objr, DDIR_SCAN_TABLE_32 );
                ObjPutIndex( objr, Globals.sel_idx );
                ObjPut32( objr, Globals.sel_start );
                ObjPut32( objr, GetCurrAddr() );
            } else {
                ObjPut8( objr, DDIR_SCAN_TABLE );
                ObjPutIndex( objr, Globals.sel_idx );
                ObjPut16( objr, Globals.sel_start );
                ObjPut16( objr, GetCurrAddr() );
            }
            ObjTruncRec( objr );
            write_record( objr, TRUE );
        }
        Globals.sel_idx = 0;
        Globals.sel_start = 0;
    }
}

static void write_end_of_pass1( void )
{
    obj_rec     *objr;

    objr = ObjNewRec( CMD_COMENT );
    objr->d.coment.attr = 0x00;
    objr->d.coment.class = CMT_MS_END_PASS_1;
    ObjAttachData( objr, "\x001", 1 );
    write_record( objr, TRUE );
}

static void write_dosseg( void )
{
    obj_rec     *objr;

    objr = ObjNewRec( CMD_COMENT );
    objr->d.coment.attr = 0x80;
    objr->d.coment.class = CMT_DOSSEG;
    ObjAttachData( objr, "", 0 );
    write_record( objr, TRUE );
}

static void write_lib( void )
{
    obj_rec     *objr;
    dir_node    *curr;
    char        *name;

    for( curr = Tables[TAB_LIB].head; curr; curr = curr->next ) {
        name = curr->sym.name;
        objr = ObjNewRec( CMD_COMENT );
        objr->d.coment.attr = 0x80;
        objr->d.coment.class = CMT_DEFAULT_LIBRARY;
        ObjAttachData( objr, name, strlen( name ) );
        write_record( objr, TRUE );
    }
}

static void write_one_export( dir_node *dir )
{
    obj_rec     *objr;
    char        *name;
    proc_info   *info;

    info = dir->e.procinfo;
    if( info->visibility == VIS_EXPORT ) {
        objr = ObjNewRec( CMD_COMENT );
        objr->d.coment.attr = 0x00;
        objr->d.coment.class = CMT_DLL_ENTRY;

        name = Mangle( &dir->sym, NULL );

        ObjAllocData( objr, 4 + strlen( name )  );
        ObjPut8( objr, 2 );
        ObjPut8( objr, 0 );             // temporary
        ObjPutName( objr, name, strlen( name ) );
        ObjPut8( objr, 0 );
        write_record( objr, TRUE );

        AsmFree( name );
    }
}

static void write_export( void )
{
    dir_node    *dir;

    for( dir = Tables[TAB_PROC].head; dir != NULL; dir = dir->next ) {
        write_one_export( dir );
    }
}


static void write_grp( void )
{
    dir_node    *curr;
    dir_node    *seg_info;
    seg_list    *seg;
    obj_rec     *grp;
    uint        line;
    char        writeseg;
    unsigned    i = 1;

    line = LineNumber;

    for( curr = Tables[TAB_GRP].head; curr; curr = curr->next, i++ ) {

        grp = ObjNewRec( CMD_GRPDEF );
        /**/myassert( grp != NULL );

        grp->d.grpdef.idx = curr->e.grpinfo->idx;

        /* we might need up to 3 bytes for each seg in dgroup and 1 byte for
           the group name index */
        ObjAllocData( grp, 1 + 3 * curr->e.grpinfo->numseg );
        ObjPut8( grp, GetLnameIdx( curr->sym.name ) );

        for( seg = curr->e.grpinfo->seglist; seg; seg = seg->next ) {
            writeseg = TRUE;
            seg_info = (dir_node *)(seg->seg);
            if( seg_info->sym.state != SYM_SEG ) {
                LineNumber = curr->line;
                AsmError( SEG_NOT_DEFINED );
                write_to_file = FALSE;
                LineNumber = line;
            } else {
                ObjPut8( grp, GRP_SEGIDX );
                ObjPutIndex( grp, seg_info->e.seginfo->segrec->d.segdef.idx);
            }
        }
        if( write_to_file ) {
            ObjTruncRec( grp );
            write_record( grp, TRUE );
        }
    }
}

static void write_seg( void )
{
    dir_node    *curr;
    obj_rec     *objr;
    uint        seg_index;
    uint        total_segs = 0;

    for( curr = Tables[TAB_SEG].head; curr; curr = curr->next ) total_segs++;

    curr = Tables[TAB_SEG].head;
    for( seg_index = 1; seg_index <= total_segs; seg_index++ ) {
        if( curr->sym.segidx != seg_index ) {
            /* segment is not in order ... so find it */
            for( curr = Tables[TAB_SEG].head; curr; curr = curr->next ) {
                if( curr->sym.segidx == seg_index ) break;
            }
        }
        if( curr == NULL || curr->sym.state != SYM_SEG ) {
            AsmError( SEG_NOT_DEFINED );
            curr = Tables[TAB_SEG].head;
            continue;
        }
        objr = curr->e.seginfo->segrec;
        objr->is_32 = TRUE;
        objr->d.segdef.ovl_name_idx = 1;
        objr->d.segdef.seg_name_idx = GetLnameIdx( curr->sym.name );
        write_record( objr, FALSE );
    }
}
static void write_lnames( void )
{
    obj_rec     *objr;
    uint        total_size = 0;
    uint        total_num = 0;
    uint        i = 0;
    char        *lname = NULL;
    uint        len;
    dir_node    *dir;
    queuenode   *curr;

    if( LnameQueue == NULL ) {
        return;
    }

    objr = ObjNewRec( CMD_LNAMES );
    objr->d.lnames.first_idx = 1;
    objr->d.lnames.num_names = LnamesIdx;

    for( curr = LnameQueue->head; curr != NULL ; curr = curr->next ) {
        dir = (dir_node *)(curr->data);
        myassert( dir != NULL );
        total_size += 1 + strlen( dir->sym.name );
        total_num++;
    }

    if( total_size > 0 ) {
        lname = AsmAlloc( total_size * sizeof( char ) + 1 );
        for( curr = LnameQueue->head; curr != NULL ; curr = curr->next ) {
            dir = (dir_node *)(curr->data);

            len = strlen( dir->sym.name );
            lname[i] = (char)len;
            i++;
            strcpy( lname+i, dir->sym.name );
            //For the Q folks... strupr( lname+i );
            i += len; // overwrite the null char
        }
        ObjAttachData( objr, lname, total_size );
    }

    ObjCanFree( objr );
    /**/myassert( total_num == LnamesIdx );
    if( total_num == 0 ) return;
    write_record( objr, TRUE );
}

static void write_global( void )
/******************************/
/* turn the globals into either externs or publics as appropriate */
{
    queuenode           *curr;
    struct asm_sym      *sym;
    struct asm_sym      *new;
    char                *(*save)( struct asm_sym *sym, char *buffer );

    if( GlobalQueue == NULL ) return;
    for( ; ; ) {
        curr = (queuenode *)QDequeue( GlobalQueue );
        if( curr == NULL ) break;
        sym = (asm_sym *)curr->data;
        if( sym->state == SYM_UNDEFINED ) {
            int seg;
            int grp;

            seg = sym->segidx;
            grp = sym->grpidx;
            save = sym->mangler;
            new = MakeExtern( sym->name, sym->mem_type, TRUE );
            new->mangler = save;
            new->segidx = seg;
            new->grpidx = grp;
            AsmFree( curr );
        } else {
            /* make this record a pubdef */
            sym->public = TRUE;
            if( PubQueue == NULL ) {
                PubQueue = AsmAlloc( sizeof( qdesc ) );
                QInit( PubQueue );
            }
            QEnqueue( PubQueue, curr );
        }
    }
    AsmFree( GlobalQueue );
    GlobalQueue = NULL;
}

static void write_ext( void )
{
    obj_rec     *objr;
    dir_node    *curr;
    dir_node    *start;
    dir_node    *next;
    uint        num;
    uint        total_size;
    uint        i;
    char        name[MAX_EXT_LENGTH];
    char        buffer[MAX_LINE_LEN];
    uint        len;

    next = Tables[TAB_EXT].head;

    for( ; ; ) {
        if( next == NULL ) break;
        start = next;
        num = 0;
        total_size = 0;
        i = 0;

        objr = ObjNewRec( CMD_EXTDEF );
        objr->d.extdef.first_idx = 0;

        for( curr = start; curr != NULL; curr = curr->next ) {
            Mangle( &curr->sym, buffer );
            len = strlen( buffer );

            if( total_size + len >= MAX_EXT_LENGTH ) break;
            total_size += len + 2;
            num++;

            name[i] = (char)len;
            i++;
            memcpy( name+i, buffer, len );
            i += len;
            name[i++] = 0;      // for the type index

        }
        next = curr;
        ObjAttachData( objr, name, total_size );
        objr->d.extdef.num_names = num;
        if( objr->d.extdef.num_names == 0 ) return;
        write_record( objr, TRUE );
    }
}

static int opsize( unsigned mem_type )
/************************************/
{
    switch( mem_type ) {
    case EMPTY:     return( 0 );
#ifdef _WASM_
    case T_SBYTE:
#endif
    case T_BYTE:    return( 1 );
#ifdef _WASM_
    case T_SWORD:
#endif
    case T_WORD:    return( 2 );
#ifdef _WASM_
    case T_SDWORD:
#endif
    case T_DWORD:   return( 4 );
    case T_FWORD:   return( 6 );
    case T_PWORD:   return( 6 );
    case T_QWORD:   return( 8 );
    case T_TBYTE:   return( 10 );
    default:        return( 0 );
    }
}

#define THREE_BYTE_MAX ( (1UL << 24) - 1 )

int get_number_of_bytes_for_size_in_commdef( unsigned long value )
/****************************************************************/
{
    /* The spec allows up to 128 in a one byte size field, but lots
       of software has problems with that, so we'll restrict ourselves
       to 127.
    */
    if( value < 128 ) {
        return( 1 );    /* 1 byte value */
    } else if( value <= USHRT_MAX ) {
        return( 3 );    /* 1 byte flag + 2 byte value */
    } else if( value <= THREE_BYTE_MAX ) {
        return( 4 );    /* 1 byte flag + 3 byte value */
    } else { // if( value <= ULONG_MAX )
        return( 5 );    /* 1 byte flag + 4 byte value */
    }
}

static void write_comm( void )
/****************************/
{
    obj_rec     *objr;
    dir_node    *curr;
    uint        num = 0;
    uint        total_size = 0;
    uint        varsize = 0;
    uint        mem_type;
    uint        i = 0;
    uint        j = 0;
    char        *name;
    uint        len;
    unsigned long value;
    char        buffer[MAX_LINE_LEN];
    char        *ptr;


    objr = ObjNewRec( CMD_COMDEF );
    objr->d.comdef.first_idx = 0;

    for( curr = Tables[TAB_COMM].head; ; curr = curr->next ) {
        if( curr == NULL ) break;
        ptr = Mangle( &curr->sym, buffer );
        total_size += 3 + strlen( ptr );
        /* 3 = 1 for string len + 1 for type index + 1 for data type */

        varsize = opsize( curr->sym.mem_type );
        if( curr->e.comminfo->distance == T_FAR ) {
            total_size += get_number_of_bytes_for_size_in_commdef( varsize );
            total_size += get_number_of_bytes_for_size_in_commdef( curr->e.comminfo->size );
        } else {
            total_size += get_number_of_bytes_for_size_in_commdef( curr->e.comminfo->size );
        }
        num++;
    }

    if( total_size > 0 ) {
        name = AsmAlloc( total_size * sizeof( char ) );
        for( curr = Tables[TAB_COMM].head; ; curr = curr->next ) {
            if( curr == NULL ) break;
            ptr = Mangle( &curr->sym, buffer );
            len = strlen( ptr );
            name[i] = (char)len;
            i++;
            memcpy( name+i, ptr, len );
            i += len;
            name[i++] = 0;      // for the type index

            /* now add the data type & communal length */
            if( curr->e.comminfo->distance == T_FAR ) {
                name[i++] = COMDEF_FAR;
            } else {
                name[i++] = COMDEF_NEAR;
            }

            value = curr->e.comminfo->size;
            varsize = get_number_of_bytes_for_size_in_commdef( value );
            switch( varsize ) {
            case 1:
                break;
            case 3:
                name[i++] = COMDEF_LEAF_2;
                break;
            case 4:
                name[i++] = COMDEF_LEAF_3;
                break;
            case 5:
                name[i++] = COMDEF_LEAF_4;
                break;
            }
            if( varsize > 1 ) varsize--; /* we already output 1 byte */

            mem_type = opsize( curr->sym.mem_type );
            if( curr->e.comminfo->distance != T_FAR ) {
                value *= mem_type;
            }

            for( j=0; j < varsize; j++ ) {
                name[i++] = value % ( UCHAR_MAX + 1 );
                value >>= 8;
            }

            if( curr->e.comminfo->distance == T_FAR ) {
                /* mem type always needs <= 1 byte */
                myassert( mem_type < UCHAR_MAX );
                name[i++] = mem_type;
            }
        }
        ObjAttachData( objr, name, total_size );
    }

    objr->d.comdef.num_names = num;
    ObjCanFree( objr );
    if( objr->d.extdef.num_names == 0 ) return;
    write_record( objr, TRUE );
}

static void write_header( void )
{
    obj_rec     *objr;
    unsigned    len;
    char        *name;
    char        full_name[_MAX_PATH];

    objr = ObjNewRec( CMD_THEADR );
    if( Options.module_name != NULL ) {
        name = Options.module_name;
    } else {
        _fullpath( full_name, AsmFiles.fname[ASM], sizeof( full_name ) );
        name = full_name;
        #if defined(__QNX__)
            if( full_name[0] == '/'
             && full_name[1] == '/'
             && (AsmFiles.fname[ASM][0] != '/' || AsmFiles.fname[ASM][1] != '/') ) {
                /*
                   if the _fullpath result has a node number and
                   the user didn't specify one, strip the node number
                   off before returning
                */
                name += 2;
                while( *name != '/' ) ++name;
            }
        #endif
    }
    objr->is_32 = Use32;
    len = strlen( name );
    ObjAllocData( objr, len + 1 );
    ObjPutName( objr, name, len );
    ObjTruncRec( objr );
    write_record( objr, TRUE );
}

static int write_modend( void )
{
    if( ModendRec == NULL ) {
        AsmError( UNEXPECTED_END_OF_FILE );
        return ERROR;
    }
    write_record( ModendRec, TRUE );
    return NOT_ERROR;
}

static long QCount( qdesc *q ) {
/****************************/
/* count the # of entries in the queue, if the retval is -ve we have an error */
    long        count = 0;
    void        *curr = 0;

    if( q == NULL ) return( 0 );
    for( curr = q->head; curr !=NULL; curr = *(void **)curr ) {
        count++;
        if( count < 0 ) return( -1 );
    }
    return( count );
}

static void write_linnum( void )
/******************************/
{
    queuenode   *node;
    linnum_data *ldata;
    linnum_data *next;
    int         count, i;
    obj_rec     *objr;
    bool        need_32;

    count = QCount( LinnumQueue );

    if( count == 0 ) return;

    need_32 = FALSE;
    ldata = AsmAlloc( count * sizeof( linnum_data ) );
    for( i = 0; i < count; i++ ) {
        node = QDequeue( LinnumQueue );
        next = (linnum_data *)(node->data);
        ldata[i].number = next->number;
        ldata[i].offset = next->offset;
        if( next->offset > 0xffffUL ) need_32 = TRUE;
        AsmFree( next );
        AsmFree( node );
    }
    AsmFree( LinnumQueue );
    LinnumQueue = NULL;
    objr = ObjNewRec( CMD_LINNUM );
    objr->is_32 = need_32;
    objr->d.linnum.num_lines = count;
    objr->d.linnum.lines = ldata;

    objr->d.linnum.d.base.grp_idx = GetCurrGrp(); // fixme ?
    objr->d.linnum.d.base.seg_idx = CurrSeg->seg->e.seginfo->segrec->d.segdef.idx;
    objr->d.linnum.d.base.frame = 0; // fixme ?

    write_record( objr, TRUE );
}

static void check_need_32bit( obj_rec *objr ) {
/**********************************************/
/* figure out if we need the 16-bit or 32-bit form of a fixup record */

    struct fixup        *fix;

    fix = objr->d.fixup.fixup;
    for( ;; ) {
        if( fix == NULL ) break;
        switch( fix->loc_method ) {
        case FIX_OFFSET386:
        case FIX_POINTER386:
            objr->is_32 = 1;
            break;
        }
        if( (unsigned_32)fix->lr.target_offset > 0xffffUL ) {
            objr->is_32 = 1;
        }
        if( objr->is_32 ) break;
        fix = fix->next;
    }
}

static void write_ledata( void )
{
    obj_rec     *objr;

    if( BufSize > 0 ) {
        objr = ObjNewRec( CMD_LEDATA );
        ObjAttachData( objr, CodeBuffer, BufSize );
        objr->d.ledata.idx = CurrSeg->seg->e.seginfo->segrec->d.segdef.idx;
//      objr->d.ledata.offset = CurrSeg->seg->e.seginfo->current_loc - BufSize;
        objr->d.ledata.offset = CurrSeg->seg->e.seginfo->start_loc;
        if( objr->d.ledata.offset > 0xffffUL ) objr->is_32 = TRUE;
//      CurrSeg->seg->e.seginfo->start_loc = CurrSeg->seg->e.seginfo->segrec->d.segdef.seg_length;
        CurrSeg->seg->e.seginfo->start_loc = CurrSeg->seg->e.seginfo->current_loc;
        write_record( objr, TRUE );

        /* Process Fixup, if any */
        if( FixupListHead != NULL ) {
            objr = ObjNewRec( CMD_FIXUP );
            objr->d.fixup.fixup = FixupListHead;
            check_need_32bit( objr );
            write_record( objr, TRUE );
            FixupListHead = FixupListTail = NULL;
        }

        /* add line numbers if debugging info is desired */
        if( Options.debug_flag ) {
            write_linnum();
        }
    }
}

static void put_public_procs_in_public_table( void )
/**************************************************/
{
    dir_node            *proc;
    struct queuenode    *qnode;

    for( proc = Tables[TAB_PROC].head; proc != NULL; proc = proc->next ) {

        /* put it into the pub table */
        if( !proc->sym.public ) {
            proc->sym.public = TRUE;

            qnode = AsmAlloc( sizeof( queuenode ) );
            qnode->data = (void *)proc;

            if( PubQueue == NULL ) {
                PubQueue = AsmAlloc( sizeof( qdesc ) );
                QInit( PubQueue );
            }
            QEnqueue( PubQueue, qnode );
        }
    }
}

static void write_alias()
/***********************/
{
    obj_rec             *objr;
    queuenode           *curr;
    char                *alias;
    char                *subst;
    char                *new;
    char                len1;
    char                len2;

    if( AliasQueue == NULL ) return;
    for( curr = AliasQueue->head; curr != NULL; curr = curr->next ) {
        /* output an alias record for this alias */
        alias = (char *)curr->data;
        subst = alias + strlen( alias ) + 1;

        len1 = strlen( alias );
        len2 = strlen( subst );

        new = AsmTmpAlloc( len1 + len2 + 2 );

        *new = len1;
        new++;
        strcpy( new, alias );
        new+=len1;
        *new = len2;
        new++;
        strcpy( new, subst );
        new -= len1 + 2;

        objr = ObjNewRec( CMD_ALIAS );
        ObjAttachData( objr, new, len1+len2+2);
        write_record( objr, TRUE );
    }
}

static int write_pub()
/*********************/
/* note that procedures with public or export visibility are written out here */
{
    queuenode           *curr;
    queuenode           *first;
    struct asm_sym      *sym;

    dir_node            *pub;
    obj_rec             *objr;
    struct pubdef_data  *data;
    uint                i;
    uint                count = 0;
    uint                seg;
    uint                grp;
    uint                static_def;

    put_public_procs_in_public_table();

    if( PubQueue == NULL ) return( NOT_ERROR );

    for( first = PubQueue->head; first != NULL ; first = curr ) {
        static_def = FALSE;
        sym = (asm_sym *)first->data;
        if( sym->state == SYM_UNDEFINED ) {
            AsmErr( SYMBOL_S_NOT_DEFINED, sym->name );
            curr = curr->next;
            return( ERROR );
        }
        seg = sym->segidx;
        grp = sym->grpidx;
        curr = first;
        for( count = 0; curr != NULL; curr = curr->next ) {
            if( count == MAX_PUB_SIZE ) break; // don't let the records get too big
            sym = (asm_sym *)curr->data;
            if( sym->segidx != seg ) break;
            if( sym->state == SYM_PROC ) {
                if( ((dir_node *)sym)->e.procinfo->visibility == VIS_PRIVATE ) {
                    if( !static_def ) {
                        if( curr != first ) break;
                        static_def = TRUE;
                    } /* else we are just continuing a static def. */
                }
            }
            count++;
            /* if we don't get to here, this entry is part of the next pubdef */
        }
        NameArray = AsmAlloc( count * sizeof( char * ) );
        for( i = 0; i < count; i++ ) {
            NameArray[i] = NULL;
        }

        /* create a public record for this segment */
        objr = ObjNewRec( static_def ? CMD_STATIC_PUBDEF : CMD_PUBDEF );
        objr->d.pubdef.base.grp_idx = grp;
        objr->d.pubdef.base.seg_idx = seg;
        objr->d.pubdef.base.frame = 0;
        data = AsmAlloc( count * sizeof( struct pubdef_data ) );


        for( curr = first, i = 0; i < count; i++, curr = curr->next ) {
            sym = (asm_sym *)curr->data;
            if( sym->segidx != seg ) break;
            if( sym->offset > 0xffffUL ) objr->is_32 = TRUE;

            NameArray[i] = Mangle( sym, NULL );

            (*(data+i)).name = i;
            /* No namecheck is needed by name manager */
            if( sym->state != SYM_CONST ) {
                (*(data+i)).offset = sym->offset;
            } else {
                pub = (dir_node *)sym;
                if( pub->e.constinfo->data[0].token != T_NUM  ) {
                    AsmWarn( 2, PUBLIC_CONSTANT_NOT_NUMERIC );
                    (*(data+i)).offset = 0;
                } else {
                    (*(data+i)).offset = pub->e.constinfo->data[0].value;
                }
            }
            (*(data+i)).type.idx = 0;
        }
        objr->d.pubdef.num_pubs = count;
        objr->d.pubdef.pubs = data;
        objr->d.pubdef.free_pubs = TRUE;
        write_record( objr, TRUE );

        /* free the table */
        for( i = 0; i < count; i++ ) {
            if( NameArray[i] != NULL ) {
                AsmFree( NameArray[i] );
            }
        }
        AsmFree( NameArray );
    }
    return( NOT_ERROR );
}

const char *NameGet( uint_16 hdl )
/********************************/
{
    return( NameArray[hdl] );
}


void FlushCurrSeg( void )
/***********************/
{
    unsigned i;

    /* this IF is for code segments which contain entirely data up to the
     * point when they are flushed
     * outselect calls flushcurrseg right back
     */
    if( CurrSeg == NULL ) return;
    if( Options.output_data_in_code_records && Globals.data_in_code ) {
        if( CurrSeg->seg->e.seginfo->segrec->d.segdef.class_name_idx
            == FindClassLnameIdx( "code" ) ) {
            Globals.data_in_code = FALSE;
            OutSelect( FALSE );
            return;
        }
    }

    /* first check if the last fixup overlaps the end of the ledata record
     * if so, wait until we get more bytes output so that it will not
     */

    if( FixupListTail != NULL ) {
        switch( FixupListTail->loc_method ) {
        case FIX_LO_BYTE:
        case FIX_HI_BYTE:
            i = 1;
            break;
        case FIX_OFFSET:
        case FIX_BASE:
            i = 2;
            break;
        case FIX_POINTER:
        case FIX_OFFSET386:
            i = 4;
            break;
        case FIX_POINTER386:
            i = 6;
            break;
        }
        if( FixupListTail->loc_offset + i > BufSize ) {
            return; // can't output the ledata record as is
        }
    }

    write_ledata();
    BufSize = 0;
}

static void reset_seg_len( void )
/* Reset length of all segments to zero */
{
    dir_node    *curr;

    for( curr = Tables[TAB_SEG].head; curr; curr = curr->next ) {
        if( curr->sym.state != SYM_SEG ) {
            AsmError( SEG_NOT_DEFINED );
            continue;
        }
        curr->e.seginfo->segrec->d.segdef.seg_length = 0;
        curr->e.seginfo->start_loc = 0; // fixme ?
        curr->e.seginfo->current_loc = 0;
    }
}

static void writepass1stuff( void )
/*********************************/
{
    if( CurrProc != NULL ) {
        AsmError( END_OF_PROCEDURE_NOT_FOUND );
        return;
    }
    write_header();
    if( Globals.dosseg ) write_dosseg();
    write_lib();
    write_lnames();
    write_seg();
    write_grp();
    write_global();
    write_ext();
    write_comm();
    write_alias();
    if( write_pub() == ERROR ) return;
    write_export();
    write_end_of_pass1();
}

static unsigned long OnePass( char *string )
{
    EndDirectiveFound = FALSE;
    PhaseError = FALSE;
    Modend = FALSE;
    PassTotal = 0;
    LineNumber = 0;

    for(;;) {
        if( ScanLine( string, MAX_LINE_LEN ) == NULL ) break; // EOF
        AsmLine( string );
        if( EndDirectiveFound ) break;
    }
    return( PassTotal );
}

void WriteObjModule( void )
/**************************/
{
    char                codebuf[ MAX_LEDATA_THRESHOLD ];
    char                string[ MAX_LINE_LEN ];
    char                *p;
    unsigned long       prev_total;
    unsigned long       curr_total;

    CodeBuffer = codebuf;

    AsmEvalInit();

    write_init();

    Parse_Pass = PASS_1;
    prev_total = OnePass( string );
    CheckForOpenConditionals();
    if( EndDirectiveFound ) {
        if( !Options.stop_at_end ) {
            for( ;; ) {
                if( ScanLine( string, MAX_LINE_LEN ) == NULL ) break;
                p = string;
                while( isspace( *p ) ) ++p;
                if( *p != '\0' ) {
                    AsmWarn( 1, END_DIRECTIVE_REQUIRED );
                    break;
                }
            }
        }
    } else {
        AsmError( END_DIRECTIVE_REQUIRED );
    }
    while( PopLineQueue() ) {
    }
    for( ;; ) {
        if( !write_to_file || Options.error_count > 0 ) break;
        writepass1stuff();
        ++Parse_Pass;
        rewind( AsmFiles.file[ASM] );
        reset_seg_len();
        BufSize = 0;
        MacroLocalVarCounter = 0;
        Globals.data_in_code = FALSE;
        PrepAnonLabels();

        curr_total = OnePass( string );
        while( PopLineQueue() ) {
        }
        if( !PhaseError && prev_total == curr_total ) break;
        ObjWriteClose( pobjState.file_out );
        /* This remove works around an NT networking bug */
        remove( AsmFiles.fname[OBJ] );
        pobjState.file_out = ObjWriteOpen( AsmFiles.fname[OBJ] );
        if( pobjState.file_out == NULL ) {
            Fatal( MSG_CANNOT_OPEN_FILE, AsmFiles.fname[OBJ] );
        }
        prev_total = curr_total;
    }
    if( write_to_file && Options.error_count == 0 ) write_modend();

    AsmSymFini();
    FreeIncludePath();
    write_fini();
    AsmEvalFini();
}
