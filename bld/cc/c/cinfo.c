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
* Description:  Segment management and FE callbacks.
*
****************************************************************************/


#include "cvars.h"
#include "cgdefs.h"
#include "cg.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "standard.h"
#include "cgaux.h"
#include "langenv.h"
#include "cgprotos.h"
#include "feprotos.h"


struct user_seg {
    struct user_seg *next;
    char            *name;
    SYM_HANDLE      sym_handle;                     /* 15-mar-92 */
    int             segtype;                        /* 22-oct-92 */
    int             segment;                        /* 22-oct-92 */
    char            *class_name;                    /* 22-oct-92 */
    hw_reg_set      pegged_register;                /* 29-may-93 */
    bool            used;
};


void AssignSeg( SYM_ENTRY *sym )
{
    SetFarHuge( sym, 1 );
    if( (sym->stg_class == SC_AUTO) || (sym->stg_class == SC_REGISTER)
     || (sym->stg_class == SC_TYPEDEF) ) {
        /* if stack/register var, there is no segment */
        sym->u.var.segment = 0;
    } else if( sym->stg_class != SC_EXTERN ) {  /* if not imported */
        if( (sym->flags & SYM_INITIALIZED) == 0 ) {
            if( sym->u.var.segment == 0 ) {             /* 15-mar-92 */
                SetSegment( sym );
            }
            if( sym->u.var.segment == SEG_DATA ) {
                sym->u.var.segment = SEG_BSS;
                CompFlags.bss_segment_used = 1;
            }
            SetSegAlign( sym );                 /* 02-feb-92 */
        }
    } else if( sym->attrib & (FLAG_FAR | FLAG_HUGE) ) {
        sym->u.var.segment = SegImport;
        --SegImport;
    } else if( (SegData != 0) && (sym->attrib & FLAG_NEAR) ) {  // imported and near
        sym->u.var.segment = SegData;
    }
}


void SetFarHuge( SYMPTR sym, int report )
{
    TYPEPTR             typ;
    type_modifiers      attrib;
    unsigned long       size;

    report = report; /* in case not used */
#if _CPU == 8086
    if( sym->declspec == DECLSPEC_DLLIMPORT
     || sym->declspec == DECLSPEC_DLLEXPORT ) {      /* 16-dec-94 */
        sym->attrib |= FLAG_FAR;
    } else if( sym->attrib & FLAG_EXPORT ) {
        sym->attrib |= FLAG_FAR;
    }
#endif
    size = SizeOfArg( sym->sym_type );
    if( TargetSwitches & BIG_DATA ) {
        attrib = sym->attrib;
        if( (attrib & (FLAG_NEAR | FLAG_FAR | FLAG_HUGE)) == 0 ) {
            if( size == 0 ) {   /* unspecified array size */
                if( sym->stg_class == SC_EXTERN ) {
                    typ = sym->sym_type;
                    if( typ->decl_type == TYPE_ARRAY ) {
                        attrib |= FLAG_FAR;
                    }
                }
            } else if( size > DataThreshold ) {
                attrib |= FLAG_FAR;
            } else if( CompFlags.strings_in_code_segment
                       && ( sym->attrib & FLAG_CONST ) ) {
                attrib |= FLAG_FAR;
            }
#if _CPU == 8086
            if( (attrib & FLAG_FAR) && size > 0x10000 ) {
                attrib &= ~FLAG_FAR;
                attrib |= FLAG_HUGE;
            }
#endif
            sym->attrib = attrib;
        }
    }
#if _CPU == 8086
   if( report && size > 0x10000 && !(sym->attrib & FLAG_HUGE) ) {
        SetErrLoc( &sym->src_loc );
        CErr( ERR_VAR_TOO_LARGE );
   }
#endif
}


#define CONSTANT( decl_flags ) ( ( decl_flags & (FLAG_CONST | FLAG_VOLATILE) ) == FLAG_CONST )



static fe_attr FESymAttr( SYMPTR sym )
/************************************/
{
    fe_attr         attr;

    attr = 0;
    switch( sym->stg_class ) {
    case SC_FORWARD:
    case SC_EXTERN:
        attr = FE_GLOBAL | FE_IMPORT | FE_STATIC;
        break;
    case SC_NULL:
        attr = FE_GLOBAL | FE_STATIC;
        break;
    case SC_STATIC:
        attr = FE_STATIC | FE_VISIBLE;
        if( sym->level != 0 ) {
            attr |= FE_INTERNAL;
        }
        break;
    }
    if( sym->flags & SYM_FUNCTION ) {
        attr |= FE_PROC | FE_STATIC;
        if( VarFunc( sym ) )
            attr |= FE_VARARGS;
        if( CompFlags.unique_functions ) {
            if( (attr & FE_GLOBAL) || (sym->flags & SYM_ADDR_TAKEN) ) {
                attr |= FE_UNIQUE;
            }
        }
    }
    if( sym->flags & SYM_USED_IN_PRAGMA ) {
        attr |= FE_MEMORY | FE_ADDR_TAKEN | FE_VOLATILE;
    }
    if( sym->flags & SYM_TRY_VOLATILE ) {
        attr |= FE_MEMORY | FE_VOLATILE;
    }
    if( sym->attrib & FLAG_VOLATILE ) {
        attr |= FE_MEMORY | FE_VOLATILE;
    } else if( sym->attrib & FLAG_CONST ) {
        attr |= FE_CONSTANT;
    }
    switch( sym->declspec ) {
    case DECLSPEC_DLLIMPORT:
        if( (attr & FE_IMPORT) || CompFlags.rent ) {
            attr |= FE_DLLIMPORT;
        }
        break;
    case DECLSPEC_DLLEXPORT:
        if( sym->stg_class == SC_NULL ) {
            attr |= FE_DLLEXPORT;
        }
        break;
    case DECLSPEC_THREAD:
        attr |= FE_THREAD_DATA;
        break;
    }
    if( sym->naked ) {
        attr |= FE_NAKED;
    }
    if( sym->rent ) {   //Override on function or r/w data
        attr |= FE_THREAD_DATA;
    }
    return( attr );
}


void    FEGenProc( CGSYM_HANDLE hdl, call_handle call_list )
/**********************************************************/
{
    SYM_HANDLE      sym_handle = hdl;

    GenInLineFunc( sym_handle );
}


fe_attr FEAttr( CGSYM_HANDLE cgsym_handle )
/*****************************************/
{
    SYM_HANDLE sym_handle = cgsym_handle;

    return( FESymAttr( SymGetPtr( sym_handle ) ) );
}


segment_id SymSegId( SYMPTR sym )
{
    fe_attr     attr;

    attr = FESymAttr( sym );
    if( attr & FE_PROC ) return( SEG_CODE );
    if( !CompFlags.rent ) {
        if( attr & FE_CONSTANT ) return( SEG_CONST2 );
    }
    return( SEG_DATA );
}


void SetSegment( SYMPTR sym )
{
    struct segment_list     *seg;
    unsigned long           size;


#if _CPU == 8086
    if( (sym->attrib & FLAG_FAR) && CompFlags.zc_switch_used ) {
        if( CONSTANT( sym->attrib )
        || (sym->stg_class == SC_STATIC  &&  (sym->flags & SYM_TEMP)) ) {
            sym->u.var.segment = SEG_CODE;
            return;
        }
    }
#elif _CPU == 386
    if( !CompFlags.rent ) {
        if( (sym->attrib & FLAG_FAR) || (TargetSwitches & FLAT_MODEL) ) {
           if( CONSTANT( sym->attrib ) && CompFlags.zc_switch_used ) {
                sym->u.var.segment = SEG_CODE;
                return;
           }
           if( (sym->stg_class == SC_STATIC) && (sym->flags & SYM_TEMP) ) {
                sym->u.var.segment = SEG_CODE;
                return;
            }
         }
     }
#endif
    if( sym->attrib & ( FLAG_FAR | FLAG_HUGE ) ) {
        size = SizeOfArg( sym->sym_type );
        seg = NULL;
#if _CPU == 8086
        if( size < 0x10000 ) {
            unsigned int    isize;

            isize = size;
            for( seg = SegListHead; seg; seg = seg->next_segment ) {
                if( seg->size_left >= isize ) break;
            }
        }
#else
        seg = SegListHead;
#endif
        if( seg == NULL ) {
            if( SegListHead == NULL ) {
                SegListHead = CMemAlloc( sizeof( struct segment_list ) );
                seg = SegListHead;
            } else {
                for( seg = SegListHead; seg->next_segment; ) {
                    seg = seg->next_segment;
                }
                seg->next_segment =
                            CMemAlloc( sizeof( struct segment_list ) );
                seg = seg->next_segment;
            }
            seg->segment_number = SegmentNum;
            ++SegmentNum;
#if _CPU == 8086
            if( size > 0xFFFF ) {
                while( size > 0x0FFFF ) {       /* while >= 64K */
                    ++SegmentNum;
                    size -= 0x10000;
                }
                seg->size_left = 0;
            } else {
                seg->size_left = 0x10000 - size;
            }
#endif
        } else {
            seg->size_left -= size;
        }
        sym->u.var.segment = seg->segment_number;
    } else {
        sym->u.var.segment = SymSegId( sym );
    }
}

struct  seg_name {
    char            *name;
    int             segment;
    SYM_HANDLE      sym_handle;                     /* 13-jan-06 */
};

static struct seg_name Predefined_Segs[] = {
    { "_CODE",      SEG_CODE,   0 },
    { "_CONST",     SEG_CONST,  0 },
    { "_DATA",      SEG_DATA,   0 },
    { "_STACK",     SEG_STACK,  0 },                /* 13-dec-92 */
    { NULL,         0,          0 }
};

static  int     UserSegment;

#define FIRST_USER_SEGMENT      10000

static struct user_seg *AllocUserSeg( char *segname, char *class_name, seg_type segtype )
{
    struct user_seg     *useg;

    useg = CMemAlloc( sizeof( struct user_seg ) );
    useg->next = NULL;
    useg->name = CStrSave( segname );
    useg->class_name = NULL;
    useg->segtype = segtype;
    if( class_name != NULL ) {
        useg->class_name = CStrSave( class_name );
    }
    useg->segment = UserSegment;
    ++UserSegment;
    return( useg );
}

#define INITFINI_SIZE 12

struct spc_info {
    char        *name;
    char        *class_name;
    seg_type    segtype;
};

static struct spc_info InitFiniSegs[INITFINI_SIZE] = {
    { TS_SEG_TIB, "DATA",           SEGTYPE_INITFINI },
    { TS_SEG_TI,  "DATA",           SEGTYPE_INITFINI },
    { TS_SEG_TIE, "DATA",           SEGTYPE_INITFINI },
    { TS_SEG_XIB, "DATA",           SEGTYPE_INITFINI },
    { TS_SEG_XI,  "DATA",           SEGTYPE_INITFINI },
    { TS_SEG_XIE, "DATA",           SEGTYPE_INITFINI },
    { TS_SEG_YIB, "DATA",           SEGTYPE_INITFINI },
    { TS_SEG_YI,  "DATA",           SEGTYPE_INITFINI },
    { TS_SEG_YIE, "DATA",           SEGTYPE_INITFINI },
    { TS_SEG_TLSB, TS_SEG_TLS_CLASS,SEGTYPE_INITFINITR },
    { TS_SEG_TLS,  TS_SEG_TLS_CLASS,SEGTYPE_INITFINITR },
    { TS_SEG_TLSE, TS_SEG_TLS_CLASS,SEGTYPE_INITFINITR },
};


static struct spc_info *InitFiniLookup( char *name )
{
    int     i;

    for( i = 0; i < INITFINI_SIZE; ++i ) {
        if( strcmp( InitFiniSegs[i].name, name ) == 0 ) {
            i = (i / 3) * 3;
            return( &InitFiniSegs[i] );
        }
    }
    return( NULL );
}


static int AddSeg( char *segname, char *class_name, int segtype )
{
    struct seg_name     *seg;
    struct user_seg     *useg, **lnk;
    hw_reg_set          reg;
    char                *p;

    for( seg = &Predefined_Segs[0]; seg->name; seg++ ) {
        if( strcmp( segname, seg->name ) == 0 ) {
            return( seg->segment );
        }
    }
    HW_CAsgn( reg, HW_EMPTY );
    p = segname;
    for( ;; ) {
        if( *p == '\0' ) break;
        if( *p == ':' ) {
            *p = '\0';
            reg = PragRegName( segname );
            *p = ':';
            segname = p + 1;
            break;
        }
        ++p;
    }
    lnk = &UserSegments;
    while( (useg = *lnk) != NULL ) {
        if( strcmp( segname, useg->name ) == 0 ) {
            return( useg->segment ); /* 11-mar-93 - was return( segment ) */
        }
        lnk = &useg->next;
    }
    useg = AllocUserSeg( segname, class_name, segtype );
    useg->next = *lnk;
    useg->pegged_register = reg;
    *lnk = useg;
    return( useg->segment );
}


int AddSegName( char *segname, char *class_name, int segtype )
{
    struct spc_info     *initfini;
    int                 segid;

    initfini = InitFiniLookup( segname );
    if( initfini != NULL ) {
        AddSeg( initfini[0].name, initfini[0].class_name, initfini[0].segtype );
        AddSeg( initfini[1].name, initfini[1].class_name, initfini[1].segtype );
        AddSeg( initfini[2].name, initfini[2].class_name, initfini[2].segtype );
    }
    segid = AddSeg( segname, class_name, segtype );
    return( segid );
}


int DefThreadSeg( void )
{
    int     segid;

    segid = AddSegName( TS_SEG_TLS, "DATA", SEGTYPE_INITFINI );
    return( segid );
}


void SetFuncSegment( SYMPTR sym, int segment )
{
    struct user_seg     *useg;

    for( useg = UserSegments; useg; useg = useg->next ) {
        if( useg->segment == segment ) {
            sym->seginfo = LkSegName( useg->name, "CODE" );
            break;
        }
    }
}


char *SegClassName( unsigned requested_seg )
{
    struct user_seg     *useg;
    struct textsegment  *tseg;
    char                *classname;
    int                 len;

    if( requested_seg == SEG_CODE ) {
        return( CodeClassName );                        /* 01-mar-90*/
    }
    for( useg = UserSegments; useg; useg = useg->next ) {
        if( useg->segment == requested_seg  &&  useg->class_name != NULL ) {
            classname = useg->class_name;
            if( classname[0] == '\0' ) {                /* 07-jun-94 */
                len = strlen( useg->name );
                if( len >= 4 ) {
                    if( stricmp( useg->name + len - 4, "DATA" ) == 0 ) {
                        classname = "FAR_DATA";
                    } else if( stricmp( useg->name + len - 4, "TEXT" ) == 0 ) {
                        classname = "CODE";
                    }
                }
            }
            return( classname );
        }
    }
    for( tseg = TextSegList; tseg != NULL; tseg = tseg->next ) {/*04-jun-94*/
        if( tseg->segment_number == requested_seg )  {
            // class name appears after the segment name
            classname = strchr( tseg->segname, '\0' ) + 1;
            // if class name not specified, then use the default
            if( *classname == '\0' )  classname = CodeClassName;
            return( classname );
        }
    }
    if( DataSegName != NULL && *DataSegName != '\0' ) {
        return( "FAR_DATA" );                           /* 10-nov-92 */
    }
    return( NULL );
}


void SetSegSymHandle( SYM_HANDLE sym_handle, int segment )
{
    struct seg_name     *seg;

    for( seg = &Predefined_Segs[0]; seg->name; seg++ ) {
        if( seg->segment == segment ) {
            seg->sym_handle = sym_handle;
            break;
        }
    }
}


SYM_HANDLE SegSymHandle( int segment )                  /* 15-mar-92 */
{
    struct seg_name     *seg;
    struct user_seg     *useg;
    char                *sym_name;

    for( seg = &Predefined_Segs[0]; seg->name; seg++ ) {
        if( seg->segment == segment ) {
            return( seg->sym_handle );
        }
    }
    for( useg = UserSegments; useg; useg = useg->next ) {
        if( useg->segment == segment ) {
            if( useg->sym_handle == 0 ) {
                sym_name = CMemAlloc( strlen( useg->name ) + 2 );
                strcpy( &sym_name[1], useg->name );
                sym_name[0] = '.';
                useg->sym_handle = SegSymbol( sym_name, 0 );
            }
            return( useg->sym_handle );
        }
    }
    return( 0 );
}

hw_reg_set *SegPeggedReg( unsigned requested_seg )
{
    struct user_seg     *useg;

    for( useg = UserSegments; useg; useg = useg->next ) {
        if( useg->segment == requested_seg )
            return( &useg->pegged_register );
    }
    return( NULL );
}


static unsigned SegAlign( unsigned suggested )
/********************************************/
{
    unsigned            align;

    align = suggested;
    if( CompFlags.unaligned_segs ) {
        align = 1;
    }
    return( align );
}

void    SetSegs( void )
/*********************/
{
    int                 seg;
    struct user_seg     *useg;
    struct textsegment  *tseg;
    int                 flags;
    char                *name;

    CompFlags.low_on_memory_printed = 0;
    flags = GLOBAL | INIT | EXEC;
    if( *TextSegName == '\0' ) {
        name = TS_SEG_CODE;
    } else {
        name = TextSegName;
        flags |= GIVEN_NAME;
    }
    BEDefSeg( SEG_CODE, flags, name,
                SegAlign( (OptSize == 0) ? BETypeLength( TY_INTEGER ) : 1 ) );
    BEDefSeg( SEG_CONST, BACK|INIT|ROM, TS_SEG_CONST,
                SegAlign( SegAlignment[SEG_CONST] ) );
    BEDefSeg( SEG_CONST2, INIT | ROM, TS_SEG_CONST2,
                SegAlign( SegAlignment[SEG_CONST2] ) );
    BEDefSeg( SEG_DATA,  GLOBAL | INIT, TS_SEG_DATA,
                SegAlign( SegAlignment[SEG_DATA] ) );
    if( CompFlags.ec_switch_used ) {            /* 04-apr-92 */
        BEDefSeg( SEG_YIB,      GLOBAL | INIT,  TS_SEG_YIB, 2 );
        BEDefSeg( SEG_YI,       GLOBAL | INIT,  TS_SEG_YI,  2 );
        BEDefSeg( SEG_YIE,      GLOBAL | INIT,  TS_SEG_YIE, 2 );
    }
    if( CompFlags.bss_segment_used ) {
        BEDefSeg( SEG_BSS,      GLOBAL,         TS_SEG_BSS,
                SegAlign( SegAlignment[ SEG_BSS ] ) );
    }
    if( CompFlags.far_strings ) {
        FarStringSegment = SegmentNum;          /* 10-mar-95 */
        ++SegmentNum;
    }
    for( seg = FIRST_PRIVATE_SEGMENT; seg < SegmentNum; ++seg ) {
        sprintf( Buffer, "%s%d_DATA", ModuleName, seg );
        BEDefSeg( seg, INIT | PRIVATE, Buffer, SegAlign( 16 ) );
    }
    for( useg = UserSegments; useg != NULL ; useg = useg->next ) {
        seg = useg->segment;
        switch( useg->segtype ) {
//      case SEGTYPE_CODE:
//          BEDefSeg( seg, INIT | GLOBAL | EXEC, useg->name, 1 );
//          break;
        case SEGTYPE_DATA:  /* created through #pragma data_seg */
            BEDefSeg( seg, INIT | GLOBAL | NOGROUP, useg->name, SegAlign( TARGET_INT ) );
            break;
        case SEGTYPE_BASED:
            BEDefSeg( seg, INIT | PRIVATE | GLOBAL, useg->name, SegAlign( TARGET_INT ) );
            break;
        case SEGTYPE_INITFINI:
            BEDefSeg( seg, INIT | GLOBAL, useg->name, SegAlign( 1 ) );
            break;
        case SEGTYPE_INITFINITR:
            BEDefSeg( seg, INIT | GLOBAL| THREAD_LOCAL, useg->name, SegAlign( 1 ) );
            break;
        }
    }
    for( tseg = TextSegList; tseg != NULL; tseg = tseg->next ) {
        tseg->segment_number = ++SegmentNum;
        BEDefSeg( tseg->segment_number,  GLOBAL | INIT | EXEC | GIVEN_NAME,
                tseg->segname,
                SegAlign( (OptSize == 0) ? BETypeLength( TY_INTEGER ) : 1 ) );
    }
}

void EmitSegLabels( void )                                  /* 15-mar-92 */
{
    int                 seg;
    struct user_seg     *useg;
    BACK_HANDLE         bck;
    SYM_ENTRY           sym;

    seg = FIRST_USER_SEGMENT;
    for( useg = UserSegments; useg; useg = useg->next ) {
        if( useg->sym_handle != 0 ) {
            SymGet( &sym, useg->sym_handle );
            bck = BENewBack( useg->sym_handle );
            sym.info.backinfo = bck;
            SymReplace( &sym, useg->sym_handle );
            BESetSeg( seg );
            DGLabel( bck );
        }
        ++seg;
    }
}


void FiniSegLabels( void )                                  /* 15-mar-92 */
{
    struct user_seg     *useg;
    SYM_ENTRY           sym;

    for( useg = UserSegments; useg; useg = useg->next ) {
        if( useg->sym_handle != 0 ) {
            SymGet( &sym, useg->sym_handle );
            BEFiniBack( sym.info.backinfo );
        }
    }
}


void FiniSegBacks( void )                                   /* 15-mar-92 */
{
    struct user_seg     *useg;
    SYM_ENTRY           sym;

    for( useg = UserSegments; useg; useg = useg->next ) {
        if( useg->sym_handle != 0 ) {
            SymGet( &sym, useg->sym_handle );
            BEFreeBack( sym.info.backinfo );
        }
    }
}


char *FEName( CGSYM_HANDLE cgsym_handle )
/**** return unmangled names ***********/
{
    SYM_HANDLE  sym_handle;
    SYMPTR      sym;

    sym_handle = cgsym_handle;
    if( sym_handle == 0 ) return( "*** NULL ***" );
    sym = SymGetPtr( sym_handle );
    return( sym->name );
}


void FEMessage( int class, void *parm )
/*************************************/
{
    char    msgtxt[80];
    char    msgbuf[MAX_MSG_LEN];

    switch( class ) {
    case MSG_SYMBOL_TOO_LONG:
        {
            SYM_ENTRY   *sym;

            sym = SymGetPtr( (SYM_HANDLE)parm );
            SetErrLoc( &sym->src_loc );
            CWarn( WARN_SYMBOL_NAME_TOO_LONG, ERR_SYMBOL_NAME_TOO_LONG, sym->name );
        }
        break;
    case MSG_BLIP:
        if( !CompFlags.quiet_mode ) {
            ConBlip();
        }
        break;
    case MSG_INFO_FILE:
    case MSG_INFO_PROC:
        NoteMsg( parm );
        break;
    case MSG_CODE_SIZE:
        if( !CompFlags.quiet_mode ) {
            CGetMsg( msgtxt, PHRASE_CODE_SIZE );
            sprintf( msgbuf, "%s: %u", msgtxt, (unsigned)parm );
            NoteMsg( msgbuf );
        }
        break;
    case MSG_DATA_SIZE:
        break;
    case MSG_ERROR:
        CErr2p( ERR_USER_ERROR_MSG, parm );
        break;
    case MSG_FATAL:
        CErr2p( ERR_FATAL_ERROR, parm );
        CloseFiles();       /* get rid of temp file */
        MyExit( 1 );        /* exit to DOS do not pass GO */
        break;
    case MSG_BAD_PARM_REGISTER:
        /* this will be issued after a call to CGInitCall or CGProcDecl */
        CErr2( ERR_BAD_PARM_REGISTER, (int)parm );
        break;
    case MSG_BAD_RETURN_REGISTER:
        CErr2p( ERR_BAD_RETURN_REGISTER, FEName( (CGSYM_HANDLE)parm ) );
        break;
    case MSG_SCHEDULER_DIED:                    /* 26-oct-91 */
    case MSG_REGALLOC_DIED:
    case MSG_SCOREBOARD_DIED:
        if( !(GenSwitches & NO_OPTIMIZATION) ) {
            if( LastFuncOutOfMem != (CGSYM_HANDLE)parm ) {
                CInfoMsg( INFO_NOT_ENOUGH_MEMORY_TO_FULLY_OPTIMIZE,
                            FEName( (CGSYM_HANDLE)parm ) );
                LastFuncOutOfMem = (CGSYM_HANDLE)parm;
            }
        }
        break;
    case MSG_PEEPHOLE_FLUSHED:
        if( !(GenSwitches & NO_OPTIMIZATION) ) {
            if( WngLevel >= 4 ) {
                if( CompFlags.low_on_memory_printed == 0 ) {
                    CInfoMsg( INFO_NOT_ENOUGH_MEMORY_TO_MAINTAIN_PEEPHOLE);
                    CompFlags.low_on_memory_printed = 1;
                }
            }
        }
        break;
    case MSG_BACK_END_ERROR:
        CErr2( ERR_BACK_END_ERROR, (int)parm );
        break;
    case MSG_BAD_SAVE:
        CErr2p( ERR_BAD_SAVE, FEName( (CGSYM_HANDLE)parm ) );
        break;
    case MSG_BAD_LINKAGE:
        CErr2p( ERR_BAD_LINKAGE, FEName( (CGSYM_HANDLE)parm ) );
        break;
    case MSG_NO_SEG_REGS:
        CErr2p( ERR_NO_SEG_REGS, FEName( (CGSYM_HANDLE)parm ) );
        break;
    case MSG_BAD_PEG_REG:
        CErr2p( ERR_BAD_PEG_REG, FEName( (CGSYM_HANDLE)parm ) );
        break;
    default:
        break;
    }
}


char *FEModuleName( void )
/************************/
{
    return( ModuleName );
}


int FETrue( void )
/****************/
{
    return( 1 );
}


segment_id FESegID( CGSYM_HANDLE cgsym_handle )
/*********************************************/
{
    SYM_HANDLE  sym_handle = cgsym_handle;
    segment_id  id;
    SYMPTR      sym;

    sym = SymGetPtr( sym_handle );
#if _CPU == 370
    {
        id = SymSegId( sym );
    }
#else
    {
        fe_attr     attr;

        attr = FESymAttr( sym );
        if( attr & FE_PROC ) {
            /* in large code models, should return different segment #
             * for every imported routine.  24-jun-88
             */
            id = SEG_CODE;
            if( sym->seginfo != NULL ) {
                id = sym->seginfo->segment_number;
            } else if( attr & FE_IMPORT ) {
                if( (sym->attrib & FLAG_FAR) || (TargetSwitches & BIG_CODE) ) {
                    if( sym->flags & SYM_ADDR_TAKEN ) {
                        id = SegImport;
                        --SegImport;
                    }
                }
            }
        } else if( sym->u.var.segment != 0 ) {
            id = sym->u.var.segment;
        } else if( attr & FE_GLOBAL ) {
            id = SEG_DATA;
        } else {
            id = SEG_CONST;
        }
    }
#endif
    return( id );
}


BACK_HANDLE FEBack( CGSYM_HANDLE cgsym_handle )
/*********************************************/
{
    SYM_HANDLE          sym_handle = cgsym_handle;
    BACK_HANDLE         bck;
    SYMPTR              symptr;
    SYM_ENTRY           sym;

    symptr = SymGetPtr( sym_handle );
    bck = symptr->info.backinfo;
    if( bck == NULL ) {
        bck = BENewBack( sym_handle );
        SymGet( &sym, sym_handle );
        sym.info.backinfo = bck;
        SymReplace( &sym, sym_handle );
    }
    return( bck );
}


int FELexLevel( CGSYM_HANDLE cgsym_handle )
/*****************************************/
{
    cgsym_handle = cgsym_handle;
    return( 0 );
}


#if _CPU == _AXP
cg_type FEParmType( CGSYM_HANDLE func, CGSYM_HANDLE parm, cg_type tipe )
/**********************************************************************/
{
    func = func;
    parm = parm;
    switch( tipe ) {
    case TY_INT_1:
    case TY_INT_2:
    case TY_INT_4:
    case TY_UINT_4:
    case TY_INTEGER:
    case TY_UNSIGNED:
    case TY_POINTER:
    case TY_CODE_PTR:
    case TY_NEAR_POINTER:
    case TY_NEAR_CODE_PTR:
        return( TY_INT_8 );
    case TY_UINT_1:
    case TY_UINT_2:
        return( TY_UINT_8 );
    case TY_DOUBLE:
    case TY_SINGLE:
    case TY_LONG_DOUBLE:
        return( TY_DOUBLE );
    default:
        return( tipe );
    }
}
#else
cg_type FEParmType( CGSYM_HANDLE func, CGSYM_HANDLE parm, cg_type tipe )
/**********************************************************************/
{
#if _CPU == 386
    SYM_HANDLE  sym_handle = func;
    SYMPTR      sym;
#else
    func = func;
#endif

    parm = parm;
    switch( tipe ) {
#if _CPU == 386 || _CPU == 370 || _CPU == _PPC || _CPU == _MIPS
    case TY_UINT_2:
    case TY_INT_2:
#endif
    case TY_INT_1:
    case TY_UINT_1:
#if _CPU == 386
        if( sym_handle != 0 ) {                         /* 22-mar-94 */
            sym = SymGetPtr( sym_handle );
            if( sym->attrib & FLAG_FAR16 ) {
                return( TY_INT_2 );
            }
        }
#endif
        tipe = TY_INTEGER;
    }
    return( tipe );
}
#endif


int FEStackChk( CGSYM_HANDLE cgsym_handle )
/*****************************************/
{
    SYM_HANDLE  sym_handle = cgsym_handle;
    SYMPTR      sym;

    sym = SymGetPtr( sym_handle );
    return( (sym->flags & SYM_CHECK_STACK) != 0 );
}

void SegInit( void )
{
    int         seg;

    UserSegment = FIRST_USER_SEGMENT;
    for( seg = 0; seg < FIRST_PRIVATE_SEGMENT; seg++ ) {
        SegAlignment[seg] = TARGET_INT;
    }
}

void SetSegAlign( SYMPTR sym )                          /* 02-feb-92 */
{
    unsigned int        align;
    unsigned int        segment;

    segment = sym->u.var.segment;
    if( segment < FIRST_PRIVATE_SEGMENT  &&  OptSize == 0 ) {
        align = GetTypeAlignment( sym->sym_type );
        SegAlignment[segment] = max( align, SegAlignment[segment] );
    }
}
