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
#include "pragdefn.h"
#include "caux.h"
#include "cfeinfo.h"

#include "clibext.h"


typedef struct user_seg {
    struct user_seg *next;
    char            *name;
    SYM_HANDLE      sym_handle;
    int             segtype;
    segment_id      segid;
    char            *class_name;
    hw_reg_set      pegged_register;
    bool            used;
} user_seg;

typedef struct seg_name {
    char            *name;
    segment_id      segid;
    SYM_HANDLE      sym_handle;
} seg_name;

static seg_name Predefined_Segs[] = {
    { "_CODE",      SEG_CODE,       SYM_NULL },
    { "_CONST",     SEG_CONST,      SYM_NULL },
    { "_DATA",      SEG_DATA,       SYM_NULL },
    { "_STACK",     SEG_STACK,      SYM_NULL },
    { NULL,         SEG_UNKNOWN,    SYM_NULL }
};

#define FIRST_USER_SEGMENT      10000

static  user_seg    *userSegments;
static  segment_id  userSegment;


void AssignSeg( SYMPTR sym )
{
    SetFarHuge( sym, TRUE );
    if( (sym->attribs.stg_class == SC_AUTO) || (sym->attribs.stg_class == SC_REGISTER)
     || (sym->attribs.stg_class == SC_TYPEDEF) ) {
        /* if stack/register var, there is no segment */
        sym->u.var.segid = SEG_UNKNOWN;
    } else if( sym->attribs.stg_class != SC_EXTERN ) {  /* if not imported */
        if( (sym->flags & SYM_INITIALIZED) == 0 ) {
            if( sym->u.var.segid == SEG_UNKNOWN ) {
                SetSegment( sym );
            }
            if( sym->u.var.segid == SEG_DATA ) {
                sym->u.var.segid = SEG_BSS;
                CompFlags.bss_segment_used = 1;
            }
            SetSegAlign( sym );
        }
    } else if( sym->mods & (FLAG_FAR | FLAG_HUGE) ) {
        sym->u.var.segid = SegImport--;
    } else if( (SegData != SEG_UNKNOWN) && (sym->mods & FLAG_NEAR) ) {  // imported and near
        sym->u.var.segid = SegData;
    }
}


void SetFarHuge( SYMPTR sym, bool report )
{
    TYPEPTR             typ;
    type_modifiers      attrib;
    target_size         size;

    report = report; /* in case not used */
#if _CPU == 8086
    if( sym->attribs.declspec == DECLSPEC_DLLIMPORT
      || sym->attribs.declspec == DECLSPEC_DLLEXPORT ) {
        sym->mods |= FLAG_FAR;
    } else if( sym->mods & FLAG_EXPORT ) {
        sym->mods |= FLAG_FAR;
    }
#endif
    size = SizeOfArg( sym->sym_type );
    if( TargetSwitches & BIG_DATA ) {
        attrib = sym->mods;
        if( (attrib & MASK_ALL_MEM_MODELS) == 0 ) {
            if( size == 0 ) {   /* unspecified array size */
                if( sym->attribs.stg_class == SC_EXTERN ) {
                    typ = sym->sym_type;
                    if( typ->decl_type == TYPE_ARRAY ) {
                        attrib |= FLAG_FAR;
                    }
                }
            } else if( size > DataThreshold ) {
                attrib |= FLAG_FAR;
            } else if( CompFlags.strings_in_code_segment && ( sym->mods & FLAG_CONST ) ) {
                attrib |= FLAG_FAR;
            }
#if _CPU == 8086
            if( (attrib & FLAG_FAR) && size > 0x10000 ) {
                attrib &= ~FLAG_FAR;
                attrib |= FLAG_HUGE;
            }
#endif
            sym->mods = attrib;
        }
    }
#if _CPU == 8086
   if( report && size > 0x10000 && (sym->mods & FLAG_HUGE) == 0 ) {
        SetErrLoc( &sym->src_loc );
        CErr1( ERR_VAR_TOO_LARGE );
        InitErrLoc();
   }
#endif
}


#define CONSTANT( decl_flags ) ( (decl_flags & MASK_CV_QUALIFIERS) == FLAG_CONST )



static fe_attr FESymAttr( SYMPTR sym )
/************************************/
{
    fe_attr         attr;

    attr = 0;
    switch( sym->attribs.stg_class ) {
    case SC_FORWARD:
    case SC_EXTERN:
        attr = FE_GLOBAL | FE_IMPORT | FE_STATIC;
        break;
    case SC_NONE:
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
    if( sym->mods & FLAG_VOLATILE ) {
        attr |= FE_MEMORY | FE_VOLATILE;
    } else if( sym->mods & FLAG_CONST ) {
        attr |= FE_CONSTANT;
    }
    switch( sym->attribs.declspec ) {
    case DECLSPEC_DLLIMPORT:
        if( (attr & FE_IMPORT) || CompFlags.rent ) {
            attr |= FE_DLLIMPORT;
        }
        break;
    case DECLSPEC_DLLEXPORT:
        if( sym->attribs.stg_class == SC_NONE ) {
            attr |= FE_DLLEXPORT;
        }
        break;
    case DECLSPEC_THREAD:
        attr |= FE_THREAD_DATA;
        break;
    }
    if( sym->attribs.naked ) {
        attr |= FE_NAKED;
    }
    if( sym->attribs.rent ) {   //Override on function or r/w data
        attr |= FE_THREAD_DATA;
    }
    return( attr );
}


void    FEGenProc( CGSYM_HANDLE hdl, call_handle call_list )
/**********************************************************/
{
    SYM_HANDLE      sym_handle = (SYM_HANDLE)hdl;

    call_list = call_list;
    GenInLineFunc( sym_handle );
}


fe_attr FEAttr( CGSYM_HANDLE cgsym_handle )
/*****************************************/
{
    SYM_HANDLE sym_handle = (SYM_HANDLE)cgsym_handle;

    return( FESymAttr( SymGetPtr( sym_handle ) ) );
}

segment_id SymSegId( SYMPTR sym )
{
    fe_attr     attr;

    attr = FESymAttr( sym );
    if( attr & FE_PROC )
        return( SEG_CODE );
    if( !CompFlags.rent ) {
        if( attr & FE_CONSTANT ) {
            return( SEG_CONST2 );
        }
    }
    return( SEG_DATA );
}


void SetSegment( SYMPTR sym )
{
    segment_list        *seg;
    target_size         size;

#if _CPU == 8086
    if( (sym->mods & FLAG_FAR) && CompFlags.zc_switch_used ) {
        if( CONSTANT( sym->mods )
        || (sym->attribs.stg_class == SC_STATIC && (sym->flags & SYM_TEMP)) ) {
            sym->u.var.segid = SEG_CODE;
            return;
        }
    }
#elif _CPU == 386
    if( !CompFlags.rent ) {
        if( (sym->mods & FLAG_FAR) || (TargetSwitches & FLAT_MODEL) ) {
           if( CONSTANT( sym->mods ) && CompFlags.zc_switch_used ) {
                sym->u.var.segid = SEG_CODE;
                return;
           }
           if( (sym->attribs.stg_class == SC_STATIC) && (sym->flags & SYM_TEMP) ) {
                sym->u.var.segid = SEG_CODE;
                return;
            }
         }
     }
#endif
    if( sym->mods & ( FLAG_FAR | FLAG_HUGE ) ) {
        size = SizeOfArg( sym->sym_type );
        seg = NULL;
#if _CPU == 8086
        if( size < 0x10000 ) {
            for( seg = SegListHead; seg != NULL; seg = seg->next_segment ) {
                if( seg->size_left >= size ) {
                    break;
                }
            }
        }
#else
        seg = SegListHead;
#endif
        if( seg == NULL ) {
            if( SegListHead == NULL ) {
                SegListHead = CMemAlloc( sizeof( segment_list ) );
                seg = SegListHead;
            } else {
                for( seg = SegListHead; seg->next_segment; ) {
                    seg = seg->next_segment;
                }
                seg->next_segment = CMemAlloc( sizeof( segment_list ) );
                seg = seg->next_segment;
            }
            seg->segid = SegmentNum++;
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
        sym->u.var.segid = seg->segid;
    } else {
        sym->u.var.segid = SymSegId( sym );
    }
}

static user_seg *AllocUserSeg( const char *segname, const char *class_name, seg_type segtype )
{
    user_seg    *useg;

    useg = CMemAlloc( sizeof( user_seg ) );
    useg->next = NULL;
    useg->name = CStrSave( segname );
    useg->class_name = NULL;
    useg->segtype = segtype;
    if( class_name != NULL ) {
        useg->class_name = CStrSave( class_name );
    }
    useg->segid = userSegment++;
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


static struct spc_info *InitFiniLookup( const char *name )
{
    int     i;
    size_t  len;

    len = strlen( name ) + 1;
    for( i = 0; i < INITFINI_SIZE; ++i ) {
        if( memcmp( InitFiniSegs[i].name, name, len ) == 0 ) {
            i = (i / 3) * 3;
            return( &InitFiniSegs[i] );
        }
    }
    return( NULL );
}


static segment_id AddSeg( const char *segname, const char *class_name, int segtype )
{
    seg_name        *seg;
    user_seg        *useg, **lnk;
    hw_reg_set      reg;
    const char      *p;
    size_t          len;

    len = strlen( segname ) + 1;
    for( seg = &Predefined_Segs[0]; seg->name != NULL; seg++ ) {
        if( memcmp( segname, seg->name, len ) == 0 ) {
            return( seg->segid );
        }
    }
    HW_CAsgn( reg, HW_EMPTY );
    for( p = segname; *p != '\0'; ++p ) {
        if( *p == ':' ) {
            reg = PragRegName( segname, p - segname );
            segname = p + 1;
            len = strlen( segname ) + 1;
            break;
        }
    }
    for( lnk = &userSegments; (useg = *lnk) != NULL; lnk = &useg->next ) {
        if( memcmp( segname, useg->name, len ) == 0 ) {
            return( useg->segid ); /* was return( segment ) */
        }
    }
    useg = AllocUserSeg( segname, class_name, segtype );
    useg->next = *lnk;
    useg->pegged_register = reg;
    *lnk = useg;
    return( useg->segid );
}


segment_id AddSegName( const char *segname, const char *class_name, int segtype )
{
    struct spc_info     *initfini;
    segment_id          segid;

    initfini = InitFiniLookup( segname );
    if( initfini != NULL ) {
        AddSeg( initfini[0].name, initfini[0].class_name, initfini[0].segtype );
        AddSeg( initfini[1].name, initfini[1].class_name, initfini[1].segtype );
        AddSeg( initfini[2].name, initfini[2].class_name, initfini[2].segtype );
    }
    segid = AddSeg( segname, class_name, segtype );
    return( segid );
}


segment_id DefThreadSeg( void )
{
    segment_id  segid;

    segid = AddSegName( TS_SEG_TLS, "DATA", SEGTYPE_INITFINI );
    return( segid );
}


void SetFuncSegment( SYMPTR sym, segment_id segid )
{
    user_seg    *useg;

    for( useg = userSegments; useg != NULL; useg = useg->next ) {
        if( useg->segid == segid ) {
            sym->seginfo = LkSegName( useg->name, "CODE" );
            break;
        }
    }
}


char *SegClassName( segment_id segid )
{
    user_seg        *useg;
    textsegment     *tseg;
    char            *classname;
    size_t          len;

    if( segid == SEG_CODE ) {
        return( CodeClassName );
    }
    for( useg = userSegments; useg != NULL; useg = useg->next ) {
        if( useg->segid == segid && useg->class_name != NULL ) {
            classname = useg->class_name;
            if( classname[0] == '\0' ) {
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
    for( tseg = TextSegList; tseg != NULL; tseg = tseg->next ) {
        if( tseg->segid == segid )  {
            // class name appears after the segment name
            classname = tseg->segname + tseg->class;
            // if class name not specified, then use the default
            if( *classname == '\0' )
                classname = CodeClassName;
            return( classname );
        }
    }
    if( DataSegName != NULL && *DataSegName != '\0' ) {
        return( "FAR_DATA" );
    }
    return( NULL );
}


void SetSegSymHandle( SYM_HANDLE sym_handle, segment_id segid )
{
    seg_name    *seg;

    for( seg = &Predefined_Segs[0]; seg->name != NULL; seg++ ) {
        if( seg->segid == segid ) {
            seg->sym_handle = sym_handle;
            break;
        }
    }
}


SYM_HANDLE SegSymHandle( segment_id segid )
{
    seg_name        *seg;
    user_seg        *useg;

    for( seg = &Predefined_Segs[0]; seg->name != NULL; seg++ ) {
        if( seg->segid == segid ) {
            return( seg->sym_handle );
        }
    }
    for( useg = userSegments; useg != NULL; useg = useg->next ) {
        if( useg->segid == segid ) {
            if( useg->sym_handle == SYM_NULL ) {
                useg->sym_handle = SegSymbol( useg->name, SEG_UNKNOWN );
            }
            return( useg->sym_handle );
        }
    }
    return( SYM_NULL );
}

hw_reg_set *SegPeggedReg( segment_id segid )
{
    user_seg    *useg;

    for( useg = userSegments; useg != NULL; useg = useg->next ) {
        if( useg->segid == segid ) {
            return( &useg->pegged_register );
        }
    }
    return( NULL );
}


static align_type SegAlign( align_type suggested )
/************************************************/
{
    align_type     align;

    align = suggested;
    if( CompFlags.unaligned_segs ) {
        align = 1;
    }
    return( align );
}

void    SetSegs( void )
/*********************/
{
    segment_id      segid;
    user_seg        *useg;
    textsegment     *tseg;
    int             flags;
    char            *name;
    align_type      optsize_segalign;

    CompFlags.low_on_memory_printed = 0;
    flags = GLOBAL | INIT | EXEC;
    if( *TextSegName == '\0' ) {
        name = TS_SEG_CODE;
    } else {
        name = TextSegName;
        flags |= GIVEN_NAME;
    }
    optsize_segalign = ( OptSize == 0 ) ? (align_type)BETypeLength( TY_INTEGER ) : 1;

    BEDefSeg( SEG_CODE, flags, name, SegAlign( optsize_segalign ) );
    BEDefSeg( SEG_CONST, BACK|INIT|ROM, TS_SEG_CONST, SegAlign( SegAlignment[SEG_CONST] ) );
    BEDefSeg( SEG_CONST2, INIT | ROM, TS_SEG_CONST2, SegAlign( SegAlignment[SEG_CONST2] ) );
    BEDefSeg( SEG_DATA,  GLOBAL | INIT, TS_SEG_DATA, SegAlign( SegAlignment[SEG_DATA] ) );
    if( CompFlags.ec_switch_used ) {
        BEDefSeg( SEG_YIB,      GLOBAL | INIT,  TS_SEG_YIB, 2 );
        BEDefSeg( SEG_YI,       GLOBAL | INIT,  TS_SEG_YI,  2 );
        BEDefSeg( SEG_YIE,      GLOBAL | INIT,  TS_SEG_YIE, 2 );
    }
    if( CompFlags.bss_segment_used ) {
        BEDefSeg( SEG_BSS, GLOBAL, TS_SEG_BSS, SegAlign( SegAlignment[SEG_BSS] ) );
    }
    if( CompFlags.far_strings ) {
        FarStringSegment = SegmentNum++;
    }
    name = CMemAlloc( strlen( ModuleName ) + 10 + sizeof( "_DATA" ) );
    for( segid = FIRST_PRIVATE_SEGMENT; segid < SegmentNum; ++segid ) {
        sprintf( name, "%s%d_DATA", ModuleName, segid );
        BEDefSeg( segid, INIT | PRIVATE, name, SegAlign( 16 ) );
    }
    CMemFree( name );
    for( useg = userSegments; useg != NULL ; useg = useg->next ) {
        segid = useg->segid;
        switch( useg->segtype ) {
//      case SEGTYPE_CODE:
//          BEDefSeg( segid, INIT | GLOBAL | EXEC, useg->name, 1 );
//          break;
        case SEGTYPE_DATA:  /* created through #pragma data_seg */
            BEDefSeg( segid, INIT | GLOBAL | NOGROUP, useg->name, SegAlign( TARGET_INT ) );
            break;
        case SEGTYPE_BASED:
            BEDefSeg( segid, INIT | PRIVATE | GLOBAL, useg->name, SegAlign( TARGET_INT ) );
            break;
        case SEGTYPE_INITFINI:
            BEDefSeg( segid, INIT | GLOBAL, useg->name, SegAlign( 1 ) );
            break;
        case SEGTYPE_INITFINITR:
            BEDefSeg( segid, INIT | GLOBAL| THREAD_LOCAL, useg->name, SegAlign( 1 ) );
            break;
        }
    }
    for( tseg = TextSegList; tseg != NULL; tseg = tseg->next ) {
        tseg->segid = ++SegmentNum;
        BEDefSeg( tseg->segid,  GLOBAL | INIT | EXEC | GIVEN_NAME, tseg->segname, SegAlign( optsize_segalign ) );
    }
}

void EmitSegLabels( void )
{
    segment_id          segid;
    user_seg            *useg;
    BACK_HANDLE         bck;
    SYM_ENTRY           sym;

    segid = FIRST_USER_SEGMENT;
    for( useg = userSegments; useg != NULL; useg = useg->next ) {
        if( useg->sym_handle != SYM_NULL ) {
            SymGet( &sym, useg->sym_handle );
            bck = BENewBack( (CGSYM_HANDLE)useg->sym_handle );
            sym.info.backinfo = bck;
            SymReplace( &sym, useg->sym_handle );
            BESetSeg( segid );
            DGLabel( bck );
        }
        ++segid;
    }
}


void FiniSegLabels( void )
{
    user_seg        *useg;
    SYM_ENTRY       sym;

    for( useg = userSegments; useg != NULL; useg = useg->next ) {
        if( useg->sym_handle != SYM_NULL ) {
            SymGet( &sym, useg->sym_handle );
            BEFiniBack( sym.info.backinfo );
        }
    }
}


void FiniSegBacks( void )
{
    user_seg        *useg;
    SYM_ENTRY       sym;

    for( useg = userSegments; useg != NULL; useg = useg->next ) {
        if( useg->sym_handle != SYM_NULL ) {
            SymGet( &sym, useg->sym_handle );
            BEFreeBack( sym.info.backinfo );
        }
    }
}


const char *FEName( CGSYM_HANDLE cgsym_handle )
/********** return unmangled names ***********/
{
    SYM_HANDLE  sym_handle;
    SYMPTR      sym;

    sym_handle = (SYM_HANDLE)cgsym_handle;
    if( sym_handle == SYM_NULL )
        return( "*** NULL ***" );
    sym = SymGetPtr( sym_handle );
    return( sym->name );
}


void FEMessage( int class, CGPOINTER parm )
/*****************************************/
{
    char    msgtxt[80];
    char    msgbuf[MAX_MSG_LEN];

    switch( class ) {
    case MSG_SYMBOL_TOO_LONG:
        {
            SYM_ENTRY   *sym;

            sym = SymGetPtr( (SYM_HANDLE)parm );
            SetErrLoc( &sym->src_loc );
            CWarn2p( WARN_SYMBOL_NAME_TOO_LONG, ERR_SYMBOL_NAME_TOO_LONG, sym->name );
            InitErrLoc();
        }
        break;
    case MSG_BLIP:
        if( !CompFlags.quiet_mode ) {
            ConBlip();
        }
        break;
    case MSG_INFO:
    case MSG_INFO_FILE:
    case MSG_INFO_PROC:
        NoteMsg( parm );
        break;
    case MSG_CODE_SIZE:
        if( !CompFlags.quiet_mode ) {
            CGetMsg( msgtxt, PHRASE_CODE_SIZE );
            sprintf( msgbuf, "%s: %u", msgtxt, (unsigned)(pointer_int)parm );
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
        CErr2( ERR_BAD_PARM_REGISTER, (int)(pointer_int)parm );
        break;
    case MSG_BAD_RETURN_REGISTER:
        CErr2p( ERR_BAD_RETURN_REGISTER, FEName( (CGSYM_HANDLE)parm ) );
        break;
    case MSG_SCHEDULER_DIED:
    case MSG_REGALLOC_DIED:
    case MSG_SCOREBOARD_DIED:
        if( (GenSwitches & NO_OPTIMIZATION) == 0 ) {
            if( LastFuncOutOfMem != parm ) {
                CInfoMsg( INFO_NOT_ENOUGH_MEMORY_TO_FULLY_OPTIMIZE, FEName( (CGSYM_HANDLE)parm ) );
                LastFuncOutOfMem = parm;
            }
        }
        break;
    case MSG_PEEPHOLE_FLUSHED:
        if( (GenSwitches & NO_OPTIMIZATION) == 0 ) {
            if( WngLevel >= 4 ) {
                if( CompFlags.low_on_memory_printed == 0 ) {
                    CInfoMsg( INFO_NOT_ENOUGH_MEMORY_TO_MAINTAIN_PEEPHOLE);
                    CompFlags.low_on_memory_printed = 1;
                }
            }
        }
        break;
    case MSG_BACK_END_ERROR:
        CErr2( ERR_BACK_END_ERROR, (int)(pointer_int)parm );
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


const char *FEModuleName( void )
/******************************/
{
    return( (const char *)ModuleName );
}


int FETrue( void )
/****************/
{
    return( 1 );
}


segment_id FESegID( CGSYM_HANDLE cgsym_handle )
/*********************************************/
{
    SYM_HANDLE  sym_handle = (SYM_HANDLE)cgsym_handle;
    segment_id  segid;
    SYMPTR      sym;

    sym = SymGetPtr( sym_handle );
#if _CPU == 370
    {
        segid = SymSegId( sym );
    }
#else
    {
        fe_attr     attr;

        attr = FESymAttr( sym );
        if( attr & FE_PROC ) {
            /* in large code models, should return different segment #
             * for every imported routine.
             */
            segid = SEG_CODE;
            if( sym->seginfo != NULL ) {
                segid = sym->seginfo->segid;
            } else if( attr & FE_IMPORT ) {
                if( (sym->mods & FLAG_FAR) || (TargetSwitches & BIG_CODE) ) {
                    if( sym->flags & SYM_ADDR_TAKEN ) {
                        segid = SegImport--;
                    }
                }
            }
        } else if( sym->u.var.segid != SEG_UNKNOWN ) {
            segid = sym->u.var.segid;
        } else if( attr & FE_GLOBAL ) {
            segid = SEG_DATA;
        } else {
            segid = SEG_CONST;
        }
    }
#endif
    return( segid );
}


BACK_HANDLE FEBack( CGSYM_HANDLE cgsym_handle )
/*********************************************/
{
    SYM_HANDLE          sym_handle = (SYM_HANDLE)cgsym_handle;
    BACK_HANDLE         bck;
    SYMPTR              symptr;
    SYM_ENTRY           sym;

    symptr = SymGetPtr( sym_handle );
    bck = symptr->info.backinfo;
    if( bck == NULL ) {
        bck = BENewBack( (CGSYM_HANDLE)sym_handle );
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
    SYM_HANDLE  sym_handle = (SYM_HANDLE)func;
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
        if( sym_handle != SYM_NULL ) {
            sym = SymGetPtr( sym_handle );
            if( sym->mods & FLAG_FAR16 ) {
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
    SYM_HANDLE  sym_handle = (SYM_HANDLE)cgsym_handle;
    SYMPTR      sym;

    sym = SymGetPtr( sym_handle );
    return( (sym->flags & SYM_CHECK_STACK) != 0 );
}

void SegInit( void )
{
    segment_id  segid;

    userSegments = NULL;
    userSegment = FIRST_USER_SEGMENT;
    for( segid = 0; segid < FIRST_PRIVATE_SEGMENT; segid++ ) {
        SegAlignment[segid] = TARGET_INT;
    }
}

void SetSegAlign( SYMPTR sym )
{
    align_type          align;
    segment_id          segid;

    segid = sym->u.var.segid;
    if( segid < FIRST_PRIVATE_SEGMENT && OptSize == 0 ) {
        align = GetTypeAlignment( sym->sym_type );
        if( SegAlignment[segid] < align ) {
            SegAlignment[segid] = align;
        }
    }
}
