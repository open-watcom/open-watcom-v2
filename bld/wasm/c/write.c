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
* Description:  Write translated object module.
*
****************************************************************************/


#include "asmglob.h"
#include <errno.h>
#include <ctype.h>
#include <time.h>

#include "asmalloc.h"
#include "fatal.h"
#include "asmeval.h"
#include "objprs.h"
#include "fixup.h"
#include "autodept.h"
#include "mangle.h"
#include "directiv.h"
#include "queues.h"
#include "womputil.h"
#include "asmlabel.h"
#include "asminput.h"
#include "asmfixup.h"
#include "condasm.h"

#include "myassert.h"


// MASM doesn't mix 16 and 32-bit fixupp into one record, but WASM does
// use following macro to separate 16 and 32-bit fixupp into two fixupp records
// it is for better compatibility with MASM
#define SEPARATE_FIXUPP_16_32

// MASM doesn't put private PROC symbol into object module, but WASM does
// if you want exactly same behaviour as MASM then undefine following macro
#define PRIVATE_PROC_INFO

#define MAX_REC_LENGTH 0xFFFEL

extern void             set_cpu_parameters( void );
extern void             set_fpu_parameters( void );
extern void             CmdlParamsInit( void );
extern void             PrintStats( void );

extern symbol_queue     Tables[];       // tables of definitions
extern uint_32          BufSize;

extern int              MacroExitState;
extern int              in_prologue;

int                     MacroLocalVarCounter = 0; // counter for temp. var names
char                    Parse_Pass;     // phase of parsing
char                    write_to_file;  // write if there is no error
unsigned long           LineNumber;
bool                    Modend;         // end of module is reached
bool                    DefineProc;     // TRUE if the definition of procedure
                                        // has not ended
dir_node                *CurrProc;      // current procedure
bool                    Use32;          // if 32-bit code is use
unsigned long           PassTotal;      // Total number of ledata bytes generated
bool                    PhaseError;
bool                    EndDirectiveFound = FALSE;

struct asmfixup         *ModendFixup = NULL; // start address fixup

global_vars     Globals = { 0, 0, 0, 0, 0, 0, 0 };

static const FNAME      *FNames = NULL;
static unsigned long    lastLineNumber;

const FNAME *AddFlist( char const *name )
/***************************************/
{
    FNAME   *flist;
    FNAME   *last;
    int     index;
    char    *fname;
    char    buff[2*_MAX_PATH];

    index = 0;
    fname = _getFilenameFullPath( buff, name, sizeof( buff ) );
    last = (FNAME *)FNames;
    for( flist = last; flist != NULL; flist = flist->next ) {
        if( strcmp( name, flist->name ) == 0 )
            return( flist );
        if( strcmp( fname, flist->fullname ) == 0 )
            return( flist );
        index++;
        last = flist;
    }
    flist = AsmAlloc( sizeof( FNAME ) );
    flist->name = AsmAlloc( strlen( name ) + 1 );
    strcpy( flist->name, name );
    flist->fullname = AsmAlloc( strlen( fname ) + 1 );
    strcpy( flist->fullname, fname );
    flist->mtime = _getFilenameTimeStamp( fname );
    flist->next = NULL;
    if( FNames == NULL ) {
        FNames = flist;
    } else {
        last->next = flist;
    }
    return( flist );
}

static void FreeFlist( void )
/***************************/
{
    const FNAME   *curr;
    const FNAME   *last;

    for( curr = FNames; curr != NULL; ) {
        AsmFree( curr->fullname );
        AsmFree( curr->name );
        last = curr;
        curr = curr->next;
        AsmFree( (void *)last );
    }
    FNames = NULL;
    return;
}

static void write_init( void )
/****************************/
{
    BufSize       = 0;
    FixupListHead = NULL;
    FixupListTail = NULL;
    CurrProc      = NULL;
    DefineProc    = FALSE;
    Use32         = FALSE;
    write_to_file = TRUE;

    AddLnameData( dir_insert( "", TAB_CLASS_LNAME ) );
    ModuleInit();
    FixInit();
}

static void write_fini( void )
/****************************/
{
    FixFini();
    FreeFlist();
}

void OutSelect( bool starts )
/***************************/
{
    obj_rec             *objr;
    unsigned long       curr;

    if( starts ) {
        if( !Options.output_comment_data_in_code_records || Globals.data_in_code
            || !Globals.code_seg )
            return;
        Globals.sel_start = GetCurrAddr();
        Globals.data_in_code = TRUE;
    } else {
        if( !Options.output_comment_data_in_code_records || !Globals.data_in_code )
            return;
        Globals.sel_idx = CurrSeg->seg->e.seginfo->idx;
        Globals.data_in_code = FALSE;

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
/************************************/
{
    obj_rec     *objr;

    objr = ObjNewRec( CMD_COMENT );
    objr->d.coment.attr = 0x00;
    objr->d.coment.class = CMT_MS_END_PASS_1;
    ObjAttachData( objr, (uint_8 *)"\x001", 1 );
    write_record( objr, TRUE );
}

static void write_dosseg( void )
/******************************/
{
    obj_rec     *objr;

    objr = ObjNewRec( CMD_COMENT );
    objr->d.coment.attr = 0x80;
    objr->d.coment.class = CMT_DOSSEG;
    ObjAttachData( objr, (uint_8 *)"", 0 );
    write_record( objr, TRUE );
}

static void write_lib( void )
/***************************/
{
    obj_rec             *objr;
    struct dir_node     *curr;
    char                *name;

    for( curr = Tables[TAB_LIB].head; curr; curr = curr->next ) {
        name = curr->sym.name;
        objr = ObjNewRec( CMD_COMENT );
        objr->d.coment.attr = 0x80;
        objr->d.coment.class = CMT_DEFAULT_LIBRARY;
        ObjAttachData( objr, (uint_8 *)name, strlen( name ) );
        write_record( objr, TRUE );
    }
}

static void write_one_export( dir_node *dir )
/*******************************************/
{
    obj_rec     *objr;
    char        *name;
    proc_info   *info;

    info = dir->e.procinfo;
    if( info->export ) {
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
/******************************/
{
    dir_node    *dir;

    for( dir = Tables[TAB_PROC].head; dir != NULL; dir = dir->next ) {
        write_one_export( dir );
    }
}


static void write_grp( void )
/***************************/
{
    dir_node        *grp;
    dir_node        *segminfo;
    seg_list        *seg;
    obj_rec         *objr;
    unsigned long   line_num;
    direct_idx      grp_idx;

    line_num = LineNumber;
    grp_idx = 0;
    for( grp = Tables[TAB_GRP].head; grp; grp = grp->next ) {
        objr = ObjNewRec( CMD_GRPDEF );
        /**/myassert( objr != NULL );
        grp_idx++;
        objr->d.grpdef.idx = grp_idx;
        /* we might need up to 3 bytes for each seg in dgroup and 1 byte for
           the group name index */
        ObjAllocData( objr, 1 + 3 * grp->e.grpinfo->numseg );
        ObjPut8( objr, grp->e.grpinfo->idx );
        for( seg = grp->e.grpinfo->seglist; seg != NULL; seg = seg->next ) {
            segminfo = (dir_node *)seg->seg;
            if( segminfo->sym.segment == NULL ) {
                LineNumber = grp->line_num;
                AsmErr( SEG_NOT_DEFINED, segminfo->sym.name );
                write_to_file = FALSE;
                LineNumber = line_num;
            } else {
                ObjPut8( objr, GRP_SEGIDX );
                ObjPutIndex( objr, segminfo->e.seginfo->idx);
            }
        }
        grp->e.grpinfo->idx = grp_idx;
        if( write_to_file ) {
            ObjTruncRec( objr );
            write_record( objr, TRUE );
        } else {
            ObjKillRec( objr );
        }
    }
}

static void write_seg( void )
/***************************/
{
    dir_node    *dir;
    obj_rec     *objr;
    uint        seg_idx;

    seg_idx = 0;
    for( dir = Tables[TAB_SEG].head; dir; dir = dir->next ) {
        if( dir->sym.segment == NULL ) {
            AsmErr( SEG_NOT_DEFINED, dir->sym.name );
            continue;
        }
        seg_idx++;
        objr = ObjNewRec( CMD_SEGDEF );
        objr->is_32 = TRUE;
        objr->d.segdef.ovl_name_idx = 1;
        objr->d.segdef.seg_name_idx = dir->e.seginfo->idx;
        objr->d.segdef.class_name_idx = ((dir_node *)dir->e.seginfo->class_name)->e.lnameinfo->idx;
        objr->d.segdef.seg_length = dir->e.seginfo->length;
        objr->d.segdef.align = dir->e.seginfo->align;
        objr->d.segdef.combine = dir->e.seginfo->combine;
        objr->d.segdef.use_32 = dir->e.seginfo->use_32;
        objr->d.segdef.access_valid = FALSE;
        objr->d.segdef.abs.frame = dir->e.seginfo->abs_frame;
        objr->d.segdef.abs.offset = 0;
        objr->d.segdef.idx = seg_idx;
        write_record( objr, FALSE );

        if( dir->e.seginfo->iscode == SEGTYPE_ISCODE ) {
            objr = ObjNewRec( CMD_COMENT );
            objr->d.coment.attr = CMT_TNP;
            objr->d.coment.class = CMT_LINKER_DIRECTIVE;
            ObjAllocData( objr, 3  );
            ObjPut8( objr, LDIR_OPT_FAR_CALLS );
            ObjPutIndex( objr, seg_idx );
            write_record( objr, TRUE );
        }
        dir->e.seginfo->idx = seg_idx;
    }
}

static void write_lnames( void )
/******************************/
{
    obj_rec     *objr;

    objr = ObjNewRec( CMD_LNAMES );
    objr->d.lnames.first_idx = 1;
    objr->d.lnames.num_names = 0;
    if( GetLnameData( objr ) ) {
        write_record( objr, TRUE );
    } else {
        ObjKillRec( objr );
    }
}

static int opsize( memtype mem_type )
/************************************/
{
    switch( mem_type ) {
    case MT_EMPTY:  return( 0 );
    case MT_SBYTE:
    case MT_BYTE:   return( 1 );
    case MT_SWORD:
    case MT_WORD:   return( 2 );
    case MT_SDWORD:
    case MT_DWORD:  return( 4 );
    case MT_FWORD:  return( 6 );
    case MT_QWORD:  return( 8 );
    case MT_TBYTE:  return( 10 );
    case MT_OWORD:  return( 16 );
    default:        return( 0 );
    }
}

#define THREE_BYTE_MAX ( (1UL << 24) - 1 )

static int get_size_in_commdef( unsigned long value )
/***************************************************/
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

static void write_external( void )
/********************************/
{
    obj_rec         *objr;
    dir_node        *start;
    dir_node        *first;
    dir_node        *curr;
    dir_node        *last;
    direct_idx      ext_idx;
    long            total_size;
    unsigned        len;
    char            *name;
    uint            varsize;
    uint            symsize;
    unsigned long   value;
    int             i;

    last = NULL;
    ext_idx = 0;
    for( start = Tables[TAB_EXT].head; start != NULL; start = last ) {
        first = NULL;
        total_size = 0;
        for( curr = start;
          ( curr != NULL ) && ( curr->e.extinfo->comm == start->e.extinfo->comm );
          curr = curr->next ) {
            if( !curr->sym.referenced )
                continue;
            if( first == NULL ) {
                first = curr;
            }
            /* + 1 for string len + 1 for type index */
            name = Mangle( &curr->sym, NULL );
            len = strlen( name ) + 2;
            AsmFree( name );
            if( first->e.extinfo->comm ) {
                //  + 1 for data type //
                len += 1;
                varsize = opsize( curr->sym.mem_type );
                if( curr->e.comminfo->distance == T_FAR ) {
                    len += get_size_in_commdef( varsize );
                    len += get_size_in_commdef( curr->e.comminfo->size );
                } else {
                    len += get_size_in_commdef( curr->e.comminfo->size );
                }
            }
            if( total_size + len > MAX_REC_LENGTH )
                break;
            total_size += len;
        }
        last = curr;
        if( total_size == 0 )
            continue;
        objr = ObjNewRec( ( first->e.extinfo->comm ) ? CMD_COMDEF : CMD_EXTDEF );
        objr->d.extdef.first_idx = 0;
        objr->d.extdef.num_names = 0;
        ObjAllocData( objr, total_size );
        for( curr = first;
          ( curr != last ) && ( curr->e.extinfo->comm == first->e.extinfo->comm );
          curr = curr->next ) {
            if( !curr->sym.referenced )
                continue;
            ext_idx++;
            objr->d.extdef.num_names++;
            curr->e.extinfo->idx = ext_idx;
            name = Mangle( &curr->sym, NULL );
            len = strlen( name );
            ObjPutName( objr, name, len );
            AsmFree( name );
            ObjPut8( objr, 0 );    // for the type index
            if( first->e.extinfo->comm ) {
                /* now add the data type & communal length */
                if( curr->e.comminfo->distance == T_FAR ) {
                    ObjPut8( objr, COMDEF_FAR );
                } else {
                    ObjPut8( objr, COMDEF_NEAR );
                }
                value = curr->e.comminfo->size;
                varsize = get_size_in_commdef( value );
                switch( varsize ) {
                case 1:
                    break;
                case 3:
                    ObjPut8( objr, COMDEF_LEAF_2 );
                    break;
                case 4:
                    ObjPut8( objr, COMDEF_LEAF_3 );
                    break;
                case 5:
                    ObjPut8( objr, COMDEF_LEAF_4 );
                    break;
                }
                if( varsize > 1 )
                    varsize--; /* we already output 1 byte */
                symsize = opsize( curr->sym.mem_type );
                if( curr->e.comminfo->distance != T_FAR ) {
                    value *= symsize;
                }
                for( i = 0; i < varsize; i++ ) {
                    ObjPut8( objr, value % ( UCHAR_MAX + 1 ) );
                    value >>= 8;
                }
                if( curr->e.comminfo->distance == T_FAR ) {
                    /* mem type always needs <= 1 byte */
                    myassert( symsize < UCHAR_MAX );
                    ObjPut8( objr, symsize );
                }
            }
        }
        if( objr->d.extdef.num_names ) {
            write_record( objr, TRUE );
        } else {
            ObjKillRec( objr );
        }
    }
}

static void write_header( char *name )
/************************************/
{
    obj_rec     *objr;
    unsigned    len;

    objr = ObjNewRec( CMD_THEADR );
    len = strlen( name );
    ObjAllocData( objr, len + 1 );
    ObjPutName( objr, name, len );
    write_record( objr, TRUE );
}

void get_frame( fixup *fixnode, struct asmfixup *fixup )
/*************************************************************/
{
    if( fixup->frame == NULL ) {
        if( fixup->fixup_type == FIX_FPPATCH ) {
            fixnode->lr.frame = FRAME_LOC;
            fixnode->lr.frame_datum = 0;
        } else {
            fixnode->lr.frame = FRAME_TARG;
            fixnode->lr.frame_datum = fixnode->lr.target_datum;
        }
    } else if( fixup->frame->state == SYM_GRP ) {
        fixnode->lr.frame = FRAME_GRP;
        fixnode->lr.frame_datum = ((dir_node *)fixup->frame)->e.grpinfo->idx;
    } else if( fixup->frame->state == SYM_SEG ) {
        fixnode->lr.frame = FRAME_SEG;
        fixnode->lr.frame_datum = ((dir_node *)fixup->frame)->e.seginfo->idx;
    } else if( fixup->frame->state == SYM_EXTERNAL ) {
        fixnode->lr.frame = FRAME_EXT;
        fixnode->lr.frame_datum = ((dir_node *)fixup->frame)->e.extinfo->idx;
    } else {
        fixnode->lr.frame = FRAME_TARG;
        fixnode->lr.frame_datum = 0;
    }
}

static struct fixup *CreateFixupRecModend( struct asmfixup *fixup )
/*****************************************************************/
/* Create a fixup record for WOMP */
{
    struct fixup        *fixnode;       // fixup structure from WOMP
    struct asm_sym      *sym;

    fixnode = FixNew();
    fixnode->next = NULL;
    fixnode->self_relative = FALSE;
    fixnode->lr.is_secondary = FALSE;
    fixnode->lr.target_offset = fixup->offset;
    fixnode->loader_resolved = FALSE;
    fixnode->loc_offset = 0;

    /*------------------------------------*/
    /* Determine the Target and the Frame */
    /*------------------------------------*/

    sym = fixup->sym;
    switch( sym->state ) {
    case SYM_UNDEFINED:
        AsmErr( SYMBOL_NOT_DEFINED, sym->name );
        return( NULL );
    case SYM_GRP:
    case SYM_SEG:
        AsmError( INVALID_START_ADDRESS );
        return( NULL );
    case SYM_EXTERNAL:
        if( sym->mem_type == MT_NEAR || sym->mem_type == MT_FAR || sym->mem_type == MT_SHORT ) {
            fixnode->lr.target = TARGET_EXT & TARGET_WITH_DISPL;
            fixnode->lr.target_datum = ((dir_node *)sym)->e.extinfo->idx;
            get_frame( fixnode, fixup );
            return( fixnode );
        } else {
            AsmError( MUST_BE_ASSOCIATED_WITH_CODE );
            return( NULL );
        }
    default:
        /**/myassert( sym->segment != NULL );
        fixnode->lr.target = TARGET_SEG & TARGET_WITH_DISPL;
        fixnode->lr.target_datum = ((dir_node *)sym->segment)->e.seginfo->idx;
        fixnode->lr.frame = FRAME_TARG;
        fixnode->lr.frame_datum = 0;
        return( fixnode );
    }
}

static int write_modend( void )
/*****************************/
{
    obj_rec     *objr;
    fixup       *fix;

    objr = ObjNewRec( CMD_MODEND );
    if( ModendFixup == NULL ) {
        objr->d.modend.start_addrs = FALSE;
        objr->d.modend.is_logical = FALSE;
        objr->d.modend.main_module = FALSE;
    } else {
        objr->d.modend.start_addrs = TRUE;
        objr->d.modend.is_logical = TRUE;
        objr->d.modend.main_module = TRUE;
        fix = CreateFixupRecModend( ModendFixup );
        if( fix != NULL ) {
            objr->d.modend.ref.log = fix->lr;
        }
    }
    write_record( objr, TRUE );
    return( NOT_ERROR );
}

static int write_autodep( void )
/******************************/
{
    obj_rec         *objr;
    char            buff[2*PATH_MAX + 5];
    unsigned int    len;
    const FNAME     *curr;

    if( !Options.emit_dependencies )
        return NOT_ERROR;

    for( curr = FNames; curr != NULL; curr = curr->next ) {
        objr = ObjNewRec( CMD_COMENT );
        objr->d.coment.attr = 0x80;
        objr->d.coment.class = CMT_DEPENDENCY;

        len = strlen(curr->fullname);
        *((time_t *)buff) = _timet2dos(curr->mtime);
        *(buff + 4) = (unsigned char)len;
        strcpy(buff + 5, curr->fullname);
        len += 5;

        ObjAttachData( objr, (uint_8 *)buff, len );

        write_record( objr, TRUE );
    }
    // one NULL dependency record must be on the end
    objr = ObjNewRec( CMD_COMENT );
    objr->d.coment.attr = 0x80;
    objr->d.coment.class = CMT_DEPENDENCY;
    ObjAttachData( objr, (uint_8 *)"", 0 );
    write_record( objr, TRUE );
    return NOT_ERROR;
}

void AddLinnumDataRef( void )
/***************************/
/* store a reference for the current line at the current address */
{
    struct line_num_info    *curr;
    unsigned long           line_num;

    if( in_prologue ) {
        line_num = CurrProc->line_num;
    } else {
        line_num = LineNumber;
    }
    if( line_num < 0x8000 )  {
        if( lastLineNumber != line_num ) {
            curr = AsmAlloc( sizeof( struct line_num_info ) );
            curr->number = line_num;
            curr->offset = AsmCodeAddress;
            curr->srcfile = get_curr_srcfile();

            AddLinnumData( curr );
            lastLineNumber = line_num;
        }
    }
}

static void write_linnum( void )
/******************************/
{
    struct linnum_data  *ldata;
    int                 count;
    obj_rec             *objr;
    bool                need_32;

    count = GetLinnumData( &ldata, &need_32 );
    if( count == 0 )
        return;
    if( ldata == NULL ) {
        AsmError( NO_MEMORY );
    } else {
        objr = ObjNewRec( CMD_LINNUM );
        objr->is_32 = need_32;
        objr->d.linnum.num_lines = count;
        objr->d.linnum.lines = ldata;
        objr->d.linnum.d.base.seg_idx = CurrSeg->seg->e.seginfo->idx;
        if( CurrSeg->seg->e.seginfo->group == NULL ) {
            objr->d.linnum.d.base.grp_idx = 0;
        } else {
            objr->d.linnum.d.base.grp_idx = ((dir_node *)CurrSeg->seg->e.seginfo->group)->e.grpinfo->idx;
        }
        objr->d.linnum.d.base.frame = 0; // fixme ?

        write_record( objr, TRUE );
    }
}

static struct fixup *CreateFixupRec( unsigned long offset, struct asmfixup *fixup )
/*********************************************************************************/
/* Create a fixup record for WOMP */
{
    struct fixup        *fixnode;       // fixup structure from WOMP
    struct asm_sym      *sym;

    fixnode = FixNew();
    fixnode->next = NULL;
    fixnode->self_relative = FALSE;
    fixnode->lr.target_offset = 0;
    fixnode->lr.is_secondary = TRUE;
    fixnode->loader_resolved = FALSE;
    fixnode->loc_offset = fixup->fixup_loc - offset;

    switch( fixup->fixup_type ) {
    case FIX_RELOFF8:
        fixnode->self_relative = TRUE;
    case FIX_LOBYTE:
        fixnode->loc_method = FIX_LO_BYTE;
        break;
    case FIX_RELOFF16:
        fixnode->self_relative = TRUE;
    case FIX_FPPATCH:
    case FIX_OFF16:
        fixnode->loc_method = FIX_OFFSET;
        break;
    case FIX_RELOFF32:
        fixnode->self_relative = TRUE;
    case FIX_OFF32:
        fixnode->loc_method = FIX_OFFSET386;
        break;
    case FIX_SEG:
        fixnode->loc_method = FIX_BASE;
        break;
    case FIX_PTR16:
        fixnode->loc_method = FIX_POINTER;
        break;
    case FIX_PTR32:
        fixnode->loc_method = FIX_POINTER386;
        break;
    }

    /*------------------------------------*/
    /* Determine the Target and the Frame */
    /*------------------------------------*/

    sym = fixup->sym;
    switch( sym->state ) {
    case SYM_UNDEFINED:
        AsmErr( SYMBOL_NOT_DEFINED, sym->name );
        return( NULL );
    case SYM_GRP:
        fixnode->lr.target = TARGET_GRP;
        fixnode->lr.target_datum = ((dir_node *)sym)->e.grpinfo->idx;
        if( fixup->frame == NULL ) {
            fixnode->lr.frame = FRAME_GRP;
            fixnode->lr.frame_datum = fixnode->lr.target_datum;
        } else {
            get_frame( fixnode, fixup );
        }
        break;
    case SYM_SEG:
        fixnode->lr.target = TARGET_SEG;
        fixnode->lr.target_datum = ((dir_node *)sym)->e.seginfo->idx;
        if( fixup->frame == NULL ) {
            fixnode->lr.frame = FRAME_SEG;
            fixnode->lr.frame_datum = fixnode->lr.target_datum;
        } else {
            get_frame( fixnode, fixup );
        }
        break;
    case SYM_EXTERNAL:
        fixnode->lr.target = TARGET_EXT;
        fixnode->lr.target_datum = ((dir_node *)sym)->e.extinfo->idx;
        get_frame( fixnode, fixup );
        break;
    default:
        /**/myassert( sym->segment != NULL );
        fixnode->lr.target = TARGET_SEG;
        fixnode->lr.target_datum = ((dir_node *)sym->segment)->e.seginfo->idx;
        get_frame( fixnode, fixup );
        break;
    }

    /*--------------------*/
    /* Optimize the fixup */
    /*--------------------*/

    if( fixnode->lr.frame == ( fixnode->lr.target - TARGET_SEG ) ) {
        fixnode->lr.frame = FRAME_TARG;
    }
    return( fixnode );
}

#ifdef SEPARATE_FIXUPP_16_32

static void get_fixup_list( unsigned long start, struct fixup **fl16, struct fixup **fl32 )
/*****************************************************************************************/
/* divide fixup record list to the 16-bit or 32-bit list of a fixup record */
{
    struct asmfixup     *fixi;
    struct fixup        *fix;
    struct fixup        *fix16;
    struct fixup        *fix32;

    fix16 = NULL;
    fix32 = NULL;
    for( fixi = FixupListHead; fixi != NULL; fixi = fixi->next_loc ) {
        fix = CreateFixupRec( start, fixi );
        switch( fix->loc_method ) {
        case FIX_OFFSET386:
        case FIX_POINTER386:
            if( fix32 == NULL ) {
                *fl32 = fix;
            } else {
                fix32->next = fix;
            }
            fix32 = fix;
            break;
        default:
            if( fix16 == NULL ) {
                *fl16 = fix;
            } else {
                fix16->next = fix;
            }
            fix16 = fix;
            break;
        }
    }
    if( fix32 != NULL ) {
        fix32->next = NULL;
    }
    if( fix16 != NULL ) {
        fix16->next = NULL;
    }
}

#else

int get_fixup_list( unsigned long start, struct fixup **fl )
{
    struct asmfixup     *fixi;
    struct fixup        *fix;
    struct fixup        *fixo;

    fixo = NULL;
    for( fixi = FixupListHead; fixi != NULL; fixi = fixi->next_loc ) {
        fix = CreateFixupRec( start, fixi );
        if( fixo == NULL ) {
            *fl = fix;
        } else {
            fixo->next = fix;
        }
        fixo = fix;
    }
    if( fixo != NULL ) {
        fixo->next = NULL;
    }
}

static void check_need_32bit( obj_rec *objr ) {
/**********************************************/
/* figure out if we need the 16-bit or 32-bit form of a fixup record */

    struct fixup        *fix;

    fix = objr->d.fixup.fixup;
    for( ;; ) {
        if( fix == NULL )
            break;
        switch( fix->loc_method ) {
        case FIX_OFFSET386:
        case FIX_POINTER386:
            objr->is_32 = 1;
            break;
        }
        if( (unsigned_32)fix->lr.target_offset > 0xffffUL ) {
            objr->is_32 = 1;
        }
        if( objr->is_32 )
            break;
        fix = fix->next;
    }
}

#endif

static void write_ledata( void )
/******************************/
{
    obj_rec         *objr;
#ifdef SEPARATE_FIXUPP_16_32
    struct fixup    *fl16 = NULL;
    struct fixup    *fl32 = NULL;
#else
    struct fixup    *fl = NULL;
#endif

    if( BufSize > 0 ) {
        objr = ObjNewRec( CMD_LEDATA );
        ObjAttachData( objr, AsmCodeBuffer, BufSize );
        objr->d.ledata.idx = CurrSeg->seg->e.seginfo->idx;
        objr->d.ledata.offset = CurrSeg->seg->e.seginfo->start_loc;
        if( objr->d.ledata.offset > 0xffffUL )
            objr->is_32 = TRUE;
        write_record( objr, TRUE );

        /* Process Fixup, if any */
        if( FixupListHead != NULL ) {
#ifdef SEPARATE_FIXUPP_16_32
            get_fixup_list( CurrSeg->seg->e.seginfo->start_loc, &fl16, &fl32 );
            /* Process Fixup, if any */
            if( fl16 != NULL ) {
                objr = ObjNewRec( CMD_FIXUP );
                objr->is_32 = FALSE;
                objr->d.fixup.fixup = fl16;
                write_record( objr, TRUE );
            }
            if( fl32 != NULL ) {
                objr = ObjNewRec( CMD_FIXUP );
                objr->is_32 = TRUE;
                objr->d.fixup.fixup = fl32;
                write_record( objr, TRUE );
            }
#else
            get_fixup_list( CurrSeg->seg->e.seginfo->start_loc, &fl );
            objr = ObjNewRec( CMD_FIXUP );
            objr->d.fixup.fixup = FixupListHead;
            check_need_32bit( objr );
            write_record( objr, TRUE );
#endif
            FixupListHead = FixupListTail = NULL;
        }
        /* add line numbers if debugging info is desired */
        if( Options.debug_flag ) {
            write_linnum();
        }
        CurrSeg->seg->e.seginfo->start_loc = CurrSeg->seg->e.seginfo->current_loc;
    }
}

static void put_private_proc_in_public_table( void )
/**************************************************/
{
    dir_node            *proc;

    /* put private PROC into the pub table */
    for( proc = Tables[TAB_PROC].head; proc != NULL; proc = proc->next ) {
        if( !proc->sym.public ) {
            AddPublicProc( proc );
        }
    }
}

static void write_alias( void )
/*****************************/
{
    obj_rec             *objr;
    char                *alias;
    char                *subst;
    char                *new;
    char                len1;
    char                len2;
    bool                first = TRUE;

    while( ( alias = GetAliasData( first ) ) != NULL ) {
        /* output an alias record for this alias */
        subst = alias + strlen( alias ) + 1;

        len1 = strlen( alias );
        len2 = strlen( subst );

        new = AsmTmpAlloc( len1 + len2 + 2 );

        *new = len1;
        new++;
        strncpy( new, alias, len1 );
        new+=len1;
        *new = len2;
        new++;
        strncpy( new, subst, len2 );
        new -= len1 + 2;

        objr = ObjNewRec( CMD_ALIAS );
        ObjAttachData( objr, (uint_8 *)new, len1+len2+2);
        write_record( objr, TRUE );
        first = FALSE;
    }
}

static int write_pub( void )
/**************************/
/* note that procedures with public or export visibility are written out here */
{
    GetPublicData();
    return( NOT_ERROR );
}

void FlushCurrSeg( void )
/***************************/
{
    unsigned i = 0;

    /* this IF is for code segments which contain entirely data up to the
     * point when they are flushed
     * outselect calls flushcurrseg right back
     */
    if( CurrSeg == NULL )
        return;

    /* first check if the last fixup overlaps the end of the ledata record
     * if so, wait until we get more bytes output so that it will not
     */

    if( FixupListTail != NULL ) {
        switch( FixupListTail->fixup_type ) {
        case FIX_LOBYTE:
        case FIX_RELOFF8:
            i = 1;
            break;
        case FIX_FPPATCH:
        case FIX_OFF16:
        case FIX_RELOFF16:
        case FIX_SEG:
            i = 2;
            break;
        case FIX_PTR16:
        case FIX_OFF32:
        case FIX_RELOFF32:
            i = 4;
            break;
        case FIX_PTR32:
            i = 6;
            break;
        }
        if( FixupListTail->fixup_loc + i - CurrSeg->seg->e.seginfo->start_loc > BufSize ) {
            return; // can't output the ledata record as is
        }
    }

    write_ledata();
    BufSize = 0;
    OutSelect( FALSE );
}

static void reset_seg_len( void )
/*******************************/
/* Reset length of all segments to zero */
{
    dir_node    *curr;

    for( curr = Tables[TAB_SEG].head; curr; curr = curr->next ) {
        if( ( curr->sym.state != SYM_SEG ) || ( curr->sym.segment == NULL ) )
            continue;
        if( curr->e.seginfo->combine != COMB_STACK ) {
            curr->e.seginfo->length = 0;
        }
        curr->e.seginfo->start_loc = 0; // fixme ?
        curr->e.seginfo->current_loc = 0;
    }
}

static void writepass1stuff( char *name )
/***************************************/
{
    if( CurrProc != NULL ) {
        AsmError( END_OF_PROCEDURE_NOT_FOUND );
        return;
    }
    write_header( name );
    write_autodep();
    if( Globals.dosseg )
        write_dosseg();
    write_lib();
    write_lnames();
    write_seg();
    write_grp();
    write_external();
    write_alias();
    if( write_pub() == ERROR )
        return;
    write_export();
    write_end_of_pass1();
}

static void OnePassInit( void )
/*****************************/
{
    CmdlParamsInit();
    AssumeInit();
    
    Options.locals_prefix[0] = '@';
    Options.locals_prefix[1] = '@';
    Options.locals_len = 0;

    EndDirectiveFound = FALSE;
    PhaseError = FALSE;
    Modend = FALSE;
    PassTotal = 0;
    LineNumber = 0;
    lastLineNumber = 0;
    MacroExitState = 0;
}

static unsigned long OnePass( char *string )
/******************************************/
{
    OnePassInit();

    IfCondInit();
    ProcStackInit();

    for(;;) {
        if( ScanLine( string, MAX_LINE_LEN ) == NULL )
            break; // EOF
        AsmLine( string );
        if( EndDirectiveFound ) {
            break;
        }
    }
    ProcStackFini();
    IfCondFini();
    return( PassTotal );
}

void WriteObjModule( void )
/**************************/
{
    uint_8              codebuf[ MAX_LEDATA_THRESHOLD ];
    char                string[ MAX_LINE_LEN ];
    char                *p;
    unsigned long       prev_total;
    unsigned long       curr_total;
    char                *mod_name;

    AsmCodeBuffer = codebuf;

    AsmSymInit();
    write_init();

    Parse_Pass = PASS_1;
#ifdef DEBUG_OUT
    if( Options.debug )
        printf( "*************\npass %u\n*************\n", Parse_Pass + 1 );
#endif
    prev_total = OnePass( string );
    if( EndDirectiveFound ) {
        if( !Options.stop_at_end ) {
            for( ;; ) {
                if( ScanLine( string, MAX_LINE_LEN ) == NULL )
                    break;
                p = string;
                while( isspace( *p ) )
                    ++p;
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
#ifdef PRIVATE_PROC_INFO
    put_private_proc_in_public_table();
#else
    if( Options.debug_flag ) {
        put_private_proc_in_public_table();
    }
#endif
    if( Options.module_name != NULL ) {
        mod_name = Options.module_name;
    } else {
        mod_name = ModuleInfo.srcfile->fullname;
    }
    for( ;; ) {
        if( !write_to_file || Options.error_count > 0 )
            break;
        writepass1stuff( mod_name );
        ++Parse_Pass;
        rewind( AsmFiles.file[ASM] );
        reset_seg_len();
        BufSize = 0;
        MacroLocalVarCounter = 0;
        Globals.data_in_code = FALSE;
        PrepAnonLabels();

#ifdef DEBUG_OUT
        if( Options.debug )
            printf( "*************\npass %u\n*************\n", Parse_Pass + 1 );
#endif
        curr_total = OnePass( string );
        // remove all remaining lines and deallocate corresponding memory
        while( ScanLine( string, MAX_LINE_LEN ) != NULL ) {
        }
        while( PopLineQueue() ) {
        }
        if( !PhaseError && prev_total == curr_total )
            break;
#ifdef DEBUG_OUT
        write_modend();
#endif
        ObjWriteClose( pobjState.file_out );
        /* This remove works around an NT networking bug */
        remove( AsmFiles.fname[OBJ] );
        pobjState.file_out = ObjWriteOpen( AsmFiles.fname[OBJ] );
        if( pobjState.file_out == NULL ) {
            Fatal( MSG_CANNOT_OPEN_FILE, AsmFiles.fname[OBJ] );
        }
        prev_total = curr_total;
    }
    if( write_to_file && Options.error_count == 0 )
        write_modend();
    if( !Options.quiet )
        PrintStats();

    /* Write a symbol listing file (if requested) */
    OpenLstFile();
    WriteListing();

    FreeIncludePath();
    write_fini();
    AsmSymFini();
}
