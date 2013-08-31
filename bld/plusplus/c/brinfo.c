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
* Description:  Browsing information generation routines.
*
****************************************************************************/


#include "plusplus.h"

#include <stdio.h>
#include <errno.h>

#ifdef OPT_BR

#include "preproc.h"
#include "memmgr.h"
#include "srcfile.h"
#include "iosupp.h"
#include "initdefs.h"
#include "brinfoim.h"
#include "ring.h"
#include "stats.h"
#include "pstk.h"
#include "pcheader.h"
#include "fname.h"

#if defined(__UNIX__)
 #include <unistd.h>
#else
 #include <direct.h>
#endif

#ifndef NDEBUG
#include "dbg.h"
#include "pragdefn.h"
#endif

#ifdef XTRA_RPT
ExtraRptCtr( ctr_namespace_decl );
ExtraRptCtr( ctr_symbol_decl );
ExtraRptCtr( ctr_template_class_decl );
ExtraRptCtr( ctr_typedef );
ExtraRptCtr( ctr_class_decl );
ExtraRptCtr( ctr_class_base );
ExtraRptCtr( ctr_class_memb );
ExtraRptCtr( ctr_class_fr );
ExtraRptCtr( ctr_defn_macro );
ExtraRptCtr( ctr_ref_func );
ExtraRptCtr( ctr_ref_data_memb_this );
ExtraRptCtr( ctr_ref_data_memb_static );
ExtraRptCtr( ctr_ref_data_bare_auto );
ExtraRptCtr( ctr_ref_data_bare_static );
ExtraRptCtr( ctr_ref_typedef );
ExtraRptCtr( ctr_ref_eval );
ExtraRptCtr( ctr_ref_evar );
ExtraRptCtr( ctr_ref_other );
ExtraRptCtr( ctr_ref_macro );
ExtraRptCtr( ctr_dep_macro_defined );
ExtraRptCtr( ctr_dep_macro_value );
ExtraRptCtr( ctr_nontrivial_scopes );
ExtraRptCtr( ctr_open_scope );
ExtraRptCtr( ctr_open_source );
#endif

typedef enum                    // BRINFO STATES:
{   BRS_INACTIVE                // - inactive
,   BRS_CMDLINE                 // - command line being processed
,   BRS_COLLECTING              // - collecting into CGIO file
,   BRS_COLLECTED               // - collected into CGIO file
,   BRS_WRITING                 // - writing browse file
,   BRS_PCH_RESTART             // - restarting during PCH read
} BRS;

typedef struct ACTBLK ACTBLK;
typedef struct SCPINS SCPINS;

struct SCPINS                   // SCPINS -- scope instruction
{   SCPINS* next;               // - next in ring
    CGFILE_INS scope_ins;       // - scope instruction
};

struct ACTBLK                   // ACTBLK -- active block scope
{   ACTBLK* next;               // - next in ring
    SCOPE scope;                // - the scope
    SCPINS* ins;                // - ring of instruction addresses
};


static FILE* usage_file;        // control for usage file
static char* usage_name;        // name of usage file
static CGFILE* virtual_file;    // virtual file for intermediate codes
static BRS brinfo_state;        // state for processing
static SRCFILE ref_file;        // reference file
static LINE_NO ref_line;        // reference line
static COLUMN_NO ref_col;       // reference column
static PSTK_CTL active_scopes;  // active scopes
static PSTK_CTL sym_scopes;     // active scopes for a definition
static carve_t carve_blk_scope; // carver: active block scopes
static carve_t carve_scope_ins; // carver: scope instructions
static ACTBLK* ring_active_blks;// ring of active block scopes
static boolean enableMacRefs;   // TRUE ==> macro references to be processed
static BRI_Handle* bri_handle;  // handle for browse-file writer


#define canWriteIc()                \
    brinfo_state == BRS_COLLECTING


static char* brinfPchGetBuffer  // GET BUFFER FOR READ
    ( BRI_PCH_CTL* ctl        // - control
    , unsigned size )           // - size required
{
    if( size > ctl->bsize ) {
        CMemFreePtr( &ctl->buffer );
        ctl->buffer = CMemAlloc( ( size + 1024 - 1 ) &  -1024 );
    }
    return ctl->buffer;
}


static BRI_PCH_CTL* brinfPchInit // INITIALIZATION OF CONTROL INFO
    ( BRI_PCH_CTL* ctl )      // - control
{
    ctl->bsize = 0;
    ctl->buffer = 0;
    brinfPchGetBuffer( ctl, 8 * 1024 );
    return ctl;
}


static BRI_PCH_CTL* brinfPchFini // COMPLETION OF CONTROL INFO
    ( BRI_PCH_CTL* ctl )         // - control
{
    CMemFreePtr( &ctl->buffer );
    return ctl;
}


static uint_8 brinfReadPch1     // READ ONE BYTE FROM PCH
    ( BRI_PCH_CTL * ctl )     // - control
{
    PCHReadUnaligned( ctl->buffer, 1 );
    return *ctl->buffer;
}


static uint_32 brinfReadPch4    // READ FOUR BYTES FROM PCH
    ( BRI_PCH_CTL * ctl )     // - control
{
    ctl = ctl;

    return PCHReadUIntUnaligned();
}


static void* brinfReadPch       // READ SUPPLIED # OF BYTES
    ( BRI_PCH_CTL * ctl       // - control
    , unsigned size )           // - size required
{
    return PCHReadUnaligned( brinfPchGetBuffer( ctl, size ), size );
}


static void brinfWriteIns       // WRITE AN INSTRUCTION
    ( CGINTER* ins )            // - instruction
{
    DbgVerify( NULL != virtual_file, "Attempt write to unopened file" );
    CgioWriteIC( virtual_file, ins );
}


void BrinfWriteIns              // WRITE IC, PTR OPERAND
    ( CGINTEROP opcode          // - opcode
    , void const *ptr )         // - operand
{
    CGINTER ins;                // - IC instruction

    ins.opcode = opcode;
    ins.value.pvalue = (void*)ptr;
    brinfWriteIns( &ins );
}


static void brinfIcPtr          // WRITE IC, PTR OPERAND
    ( CGINTEROP opcode          // - opcode
    , void const *ptr )         // - operand
{
    if( canWriteIc() ) {
        BrinfWriteIns( opcode, ptr );
    }
}


static void brinfIcBin          // WRITE IC, BINARY OPERAND
    ( CGINTEROP opcode          // - opcode
    , unsigned bin )            // - operand
{
    if( canWriteIc() ) {
        CGINTER ins;            // - IC instruction
        ins.opcode = opcode;
        ins.value.uvalue = bin;
        brinfWriteIns( &ins );
    }
}


#if 0
static void brinfIc             // WRITE IC, NO OPERAND
    ( CGINTEROP opcode )        // - opcode
{
    brinfIcBin( opcode, 0 );
}
#endif


static ACTBLK* findActiveScope  // FIND ACTIVE SCOPE ENTRY
    ( SCOPE scope )             // - the scope
{
    ACTBLK* retn;               // - entry for scope
    ACTBLK* curr;               // - search entry

    retn = NULL;
    RingIterBeg( ring_active_blks, curr ) {
        if( scope == curr->scope ) {
            retn = curr;
            break;
        }
    } RingIterEnd( curr );
    return retn;
}


static void recordBlkScopeIns   // RECORD A BLK-SCOPE INSTRUCTION
    ( SCOPE scope )             // - scope for instruction
{
    if( canWriteIc() ) {
        SCPINS* ins;
        ACTBLK* act = findActiveScope( scope );
        if( NULL == act ) {
            act = RingCarveAlloc( carve_blk_scope, &ring_active_blks );
            act->scope = scope;
            act->ins = NULL;
        }
        ins = RingCarveAlloc( carve_scope_ins, &act->ins );
        ins->scope_ins = CgioLastWrite( virtual_file );
    }
}

#ifndef NDEBUG
void _dbgScope
    ( SCOPE scope
    , char const * text )
{
    IfDbgToggle( browse ) {
        VBUF vbuf;
        switch( scope->id ) {
          case SCOPE_FILE :
            printf( "%s: File Scope %x\n", text, scope );
            break;
          case SCOPE_CLASS :
            printf( "%s: Class Scope: %x", text, scope );
            PrintFullType( scope->owner.type );
            break;
          case SCOPE_FUNCTION :
            printf( "%s: Function Scope: %x %s\n"
                  , text
                  , scope
                  , DbgSymNameFull( scope->owner.sym, &vbuf ) );
            VbufFree( &vbuf );
            break;
          case SCOPE_BLOCK :
            printf( "%s: Block Scope: %x %u\n"
                  , text
                  , scope
                  , scope->owner.index );
            break;
          case SCOPE_TEMPLATE_DECL :
            printf( "%s: Template Declaration Scope %x\n", text, scope );
            break;
          case SCOPE_TEMPLATE_INST :
            printf( "%s: Template Instantiation Scope %x\n", text, scope );
            break;
          case SCOPE_TEMPLATE_PARM :
            printf( "%s: Template Parameters Scope %x\n", text, scope );
            break;
        }
    }
}
#else
#define _dbgScope(a,b)
#endif

static void closeScopeIc        // INTERMEDIATE CODE TO CLOSE A SCOPE
    ( SCOPE scope )             // - scope for instruction
{
    _dbgScope( scope, "Br-inf-cls" );
    brinfIcPtr( IC_BR_SCOPE_CLS, scope );
    recordBlkScopeIns( scope );
}


static CGINTER nop_ins = { 0, IC_BR_NO_OP };

static void endBlkScope         // END OF BLK SCOPE
    ( SCOPE scope )             // - scope being closed
{
    if( canWriteIc() ) {
        ACTBLK* act = findActiveScope( scope );
        if( NULL != act ) {
            if( ! scope->s.keep ) {
                SCPINS* curr;
                RingIterBeg( act->ins, curr ) {
                    CgioBuffZap( curr->scope_ins, &nop_ins );
                } RingIterEnd( curr );
            }
            RingCarveFree( carve_scope_ins, &act->ins );
            RingPrune( &ring_active_blks, act );
            CarveFree( carve_blk_scope, act );
        }
    }
}


static TOKEN_LOCN const* adjustLocn // ADJUST FOR UNSPECIFIED LOCATION
    ( TOKEN_LOCN const* locn    // - the location
    , TOKEN_LOCN* temporary )   // - a TOKEN_LOCN temporary
{
    temporary = temporary;
    if( NULL != locn ) {
        SRCFILE sf = locn->src_file;
        if( NULL == sf
         || IsSrcFileCmdLine( sf ) ) {
            locn = NULL;
        }
    }
    return locn;
}


static void adjustSrcFile       // ADJUST OPEN/CLOSE OF SOURCE FILE(S)
    ( TOKEN_LOCN const * locn ) // - location for reference/declaration
{
    TOKEN_LOCN curr_locn;
    locn = adjustLocn( locn, &curr_locn );
    if( NULL != locn ) {
        BrinfSrcSync( locn );
    }
}

#ifndef NDEBUG
#   define ShcVerify(t,m) if( !(t) ) printf( "%s\n", m );
#else
#   define ShcVerify(a,b)
#endif


static boolean activeScopesReset// RESET ACTIVE SCOPES
    ( SCOPE scope               // - new active scope
    , TOKEN_LOCN const * locn ) // - file location to synchronize to
{
    PSTK_ITER act_iter;         // - iterator: old active scopes
    PSTK_ITER sym_iter;         // - iterator: new active scopes
    SCOPE ascope;               // - scope: old active scopes
    SCOPE sscope;               // - scope: new active scopes
    boolean retn;               // - return: TRUE ==> scope was adjusted
    boolean short_circuit;      // - can short-circuit scope adjustment
    boolean in_template;        // - active scope is in a TEMPLATE_PARM scope

    if( canWriteIc() ) {
        SCOPE top = PstkTopElement( &active_scopes );
        DbgVerify( NULL == PstkTopElement( &sym_scopes )
                 , "Non-empty definition scope" );
        if( scope->id == SCOPE_FUNCTION
         && NULL == scope->owner.sym ) {
            // this is our kluge for analysing default-argument expr's
            // we instead use the enclosing scope
            scope = scope->enclosing;
        }
        switch( scope->id ) {
          default :
            if( scope == top ) {
                short_circuit = TRUE;
                break;
            }
            // drops thru
          case SCOPE_TEMPLATE_DECL :
          case SCOPE_TEMPLATE_PARM :
          case SCOPE_TEMPLATE_INST :
            short_circuit = FALSE;
            break;
        }
#ifdef NDEBUG
        if( ! short_circuit )
#endif
        {
            sscope = scope;
            for( ; sscope != NULL; sscope = sscope->enclosing ) {
                PstkPush( &sym_scopes, sscope );
            }
            PstkIterUpOpen( &act_iter, &active_scopes );
            PstkIterDnOpen( &sym_iter, &sym_scopes );
            in_template = FALSE;
            for( ; ; ) {
                ascope = PstkIterUpNext( &act_iter );
                sscope = PstkIterDnNext( &sym_iter );
                if( ascope != sscope ) break;
                if( ascope == NULL ) break;
                if( ascope->id == SCOPE_TEMPLATE_PARM ){
                    in_template = TRUE;
                }
            }
            if( ascope != NULL ) {
                ShcVerify( ! short_circuit
                         , "failed short-circuit assumption" );
                for( ; ; ) {
                    SCOPE top = PstkPopElement( &active_scopes );
                    closeScopeIc( top );
                    if( top == ascope ) break;
                }
            }
            if( !in_template && sscope != NULL ) {
                for( ;; ) {
                    if( scope->id == SCOPE_TEMPLATE_PARM ) {
                        in_template = TRUE;
                        break;
                    }
                    if( scope == sscope ) break;
                    scope = scope->enclosing;
                }
            }
            if( !in_template ) {
                adjustSrcFile( locn );
            }
            for( ; sscope != NULL; ) {
                _dbgScope( sscope, "Br-inf-opn" );
                ShcVerify( ! short_circuit
                         , "failed short-circuit assumption" );
                brinfIcPtr( IC_BR_SCOPE_OPN, sscope );
                recordBlkScopeIns( sscope );
                PstkPush( &active_scopes, sscope );
                sscope = PstkIterDnNext( &sym_iter );
            }
            for( ; ; ) {
                SCOPE top = PstkPopElement( &sym_scopes );
                if( NULL == top ) break;
            }
        }
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}

#undef ShcVerify


static boolean activeScopesAdjust // ADJUST ACTIVE SCOPES
    ( SYMBOL sym )
{
    boolean retn;               // - return: TRUE ==> scope for symbol
    TOKEN_LOCN *locn;

    if( NULL == sym->name ) {
        retn = FALSE;
    } else {
        SCOPE scope = sym->name->containing;
        if( NULL == scope ) {
            retn = FALSE;
        } else {
            if( sym->locn != NULL ){
                locn = &sym->locn->tl;
            } else {
                locn = NULL;
            }
            retn = activeScopesReset( scope, locn );
        }
    }
    return retn;
}


static void destroyBrinfData        // DESTROY ALL DATA
    ( void )
{
    CarveDestroy( carve_scope_ins );
    CarveDestroy( carve_blk_scope );
    PstkClose( &active_scopes );
    PstkClose( &sym_scopes );
}


static void activeScopesClose       // CLOSE ACTIVE SCOPES
    ( void )
{
    if( canWriteIc() ) {
        for( ; ; ) {
            SCOPE act = PstkPopElement( &active_scopes );
            if( NULL == act ) break;
            closeScopeIc( act );
        }
    }
    if( BrinfActive() ) {
        ACTBLK* act;                // - active blk
        RingIterBegSafe( ring_active_blks, act ) {
            endBlkScope( act->scope );
        } RingIterEndSafe( act );
        destroyBrinfData();
    }
}


void BrinfIcReference           // WRITE OUT A REFERENCE
    ( CGINTEROP opcode          // - opcode
    , void const *ptr           // - operand
    , TOKEN_LOCN const* locn )  // - location
{
    if( ref_file != locn->src_file ) {
        ref_file = locn->src_file;
        brinfIcPtr( IC_BR_REFTO_FILE, ref_file );
        ref_line = 0;
        ref_col = 0;
    }
    if( ref_line != locn->line ) {
        ref_line = locn->line;
        if( ref_line <= 0xFFFFFF
         && locn->column <= 0xFF ) {
            ref_col = locn->column;
            brinfIcBin( IC_BR_REFTO_LINECOL
                      , (ref_line << 8 ) + ref_col );
        } else {
            brinfIcBin( IC_BR_REFTO_LINE, ref_line );
        }
    }
    if( ref_col != locn->column ) {
        ref_col = locn->column;
        brinfIcBin( IC_BR_REFTO_COL, ref_col );
    }
    brinfIcPtr( opcode, ptr );
}


static void brinfIcReference    // WRITE OUT A REFERENCE, IF REQ'D
    ( CGINTEROP opcode          // - opcode
    , void const *ptr           // - operand
    , TOKEN_LOCN const* locn )  // - location
{
    TOKEN_LOCN curr_locn;

    locn = adjustLocn( locn, &curr_locn );
    if( NULL == locn ) return;
    if( canWriteIc()
     && activeScopesReset( GetCurrScope(), locn ) ) {
        BrinfIcReference( opcode, ptr, locn );
    }
}


void BrinfWriteSrcOpen          // WRITE IC'S FOR SOURCE OPEN
    ( char const * fname        // - file name (as entered)
    , TOKEN_LOCN const* locn    // - location
    , SRCDEP const *dep )       // - dependency entry
{
    brinfIcReference( IC_BR_INC_SRC, fname, locn );
    brinfIcPtr( IC_BR_SRC_BEG, dep );
}


void BrinfWriteSrcClose         // WRITE IC'S FOR SOURCE CLOSE
    ( void )
{
    brinfIcPtr( IC_BR_SRC_END, NULL );
}


static void brinfIcDclSym       // PUT OUT A SYMBOL DECLARATION
    ( CGINTEROP opcode          // - opcode
    , SYMBOL sym )              // - the symbol
{
    //adjustSrcFile( &sym->locn->tl );
    brinfIcPtr( opcode, sym );
}


static void typeUsage           // TYPE USAGE
    ( TYPE type                 // - the type
    , TOKEN_LOCN* locn )        // - reference location
{
    if( CompFlags.optbr_t ) {
        for( ; type != NULL; type = type->of ) {
            switch( type->id ) {
              case TYP_ENUM :
              case TYP_CLASS :
                brinfIcReference( IC_BR_REF_TYPE, type, locn );
                break;
              case TYP_MEMBER_POINTER :
                brinfIcReference( IC_BR_REF_TYPE, type->u.mp.host, locn );
                continue;
              case TYP_FUNCTION :
              { arg_list* args = type->u.f.args;
                int count = args->num_args;
                for( ; count > 0; ) {
                    -- count;
                    typeUsage( args->type_list[count], locn );
                }
              }
              // drops thru
              default :
                continue;
            }
            break;
        }
    }
}


static void refSymType          // REFERENCE TYPES FOR A DECLARED SYMBOL
    ( SYMBOL sym )              // - the symbol
{
    if( sym->flag2 & SF2_TOKEN_LOCN ) {
        typeUsage( sym->sym_type, &sym->locn->tl );
    }
}


static void fatalIoErr          // SIGNAL FATAL I/O ERROR
    ( char const * msg          // - message
    , char const * file_name )  // - file name
{
    puts( strerror( errno ) );
    puts( file_name );
    CFatal( msg );
}


static void usageIoErr          // SIGNAL FATAL I/O ERROR: USAGE FILE
    ( char const * msg )        // - message
{
    fatalIoErr( msg, usage_name );
}


static int brinfWrite           // WRITE CALLBACK FUNCTION
    ( int cookie
    , void const *buf           // - write buffer
    , unsigned len )            // - write length
{
    FILE *fp = (FILE *) cookie;
    int result;

    result = fwrite(buf, 1, len, fp);
    return result;
}


static long brinfLSeek          // LSEEK CALLBACK FUNCTION
    ( int cookie
    , long offset               // - seek offset
    , int whence )              // - seek direction, see b_write.h
{
    FILE *fp = (FILE *) cookie;

    fseek(fp, offset, whence);
    return ftell(fp);
}


static BRI_Routines const rtns =// CALL-BACK FOR BROWSE WRITER
{   &brinfWrite
,   &brinfLSeek
,   &CMemAlloc
,   &CMemFree
};


static int brinfWritePch        // PCH WRITE CALLBACK FUNCTION
    ( int cookie
    , void const *buf           // - write buffer
    , unsigned len )            // - write length
{
    DbgStmt( PCHVerifyFile( cookie ) );
    PCHWriteUnaligned( buf, len );
    return len;
}


static long brinfLSeekPch       // PCH LSEEK CALLBACK FUNCTION
    ( int cookie
    , long offset               // - seek offset
    , int whence )              // - seek direction, see b_write.h
{
    DbgStmt( PCHVerifyFile( cookie ) );
    PCHFlushBuffer();
    return PCHSeek( offset, whence );
}


static BRI_Routines const rtns_pch =// CALL-BACK FOR PCH WRITER
{   &brinfWritePch
,   &brinfLSeekPch
,   &CMemAlloc
,   &CMemFree
};


static void brinfOpen           // OPEN BROWSE-USE INFO, IF REQD
    ( void )
{
    usage_name = strsave( IoSuppOutFileName( OFT_BRI ) );
    usage_file = SrcFileFOpen( usage_name, SFO_WRITE_BINARY );
    if( NULL == usage_file ) {
        usageIoErr( "(-fbi) unable to open browse file for writing" );
    }
    IfDbgToggle( browse ) {
        DbgStmt( printf( "Br-inf-use: open %s\n", usage_name ) );
    }
}


static void brinfClose          // CLOSE BROWSE-USE INFO, IF REQD
    ( void )
{
    FILE *temp;                 // - file to be closed

    temp = usage_file;
    if( NULL != temp ) {
        usage_file = NULL;
        BrinfDepFini();
        brinfo_state = BRS_INACTIVE;
        if( 0 != SrcFileFClose( temp ) ) {
            usageIoErr( "(-fbi) unable to close usage file" );
        }
        IfDbgToggle( browse ) {
            DbgStmt( printf( "Br-inf-use: close %s\n", usage_name ) );
        }
    } else {
        brinfo_state = BRS_INACTIVE;
    }
}


void BrinfDeclClass             // VALID CLASS HAS BEEN DECLARED
    ( TYPE cltype )             // - class type
{
    TOKEN_LOCN* locn = LocnForClass( cltype );  // - location of the class
    TOKEN_LOCN  currLocn;                       // - parser's current location

    if( locn == NULL ) {
        SrcFileGetTokenLocn( &currLocn );
        locn = &currLocn;
    }
    if( activeScopesReset( cltype->u.c.scope, locn ) ) {
        ExtraRptIncrementCtr( ctr_class_decl );
        if( BrinfActive() ) {
            brinfIcPtr( IC_BR_DCL_CLASS, cltype );
        }
    }
}


void BrinfDeclNamespace         // VALID NAMESPACE HAS BEEN DECLARED
    ( SYMBOL ns )               // - namespace defined

{
    if( activeScopesAdjust( ns ) ) {
        ExtraRptIncrementCtr( ctr_namespace_decl );
        IfDbgToggle( browse ) {
            DbgStmt( printf( "Br-inf-dcl: namespace is %s"
                           , ScopeNameSpaceName( ns->u.ns->scope )
                           )
                   );
            DbgStmt( DbgDumpTokenLocn( &ns->locn->tl ) );
            DbgStmt( printf( "\n" ) );
        }
    }
}


void BrinfDeclSymbol            // VALID SYMBOL HAS BEEN DECLARED
    ( SYMBOL sym )              // - symbol defined
{
    if( ! SymIsClassMember( sym )
     && SymIsBrowsable( sym )
     && activeScopesAdjust( sym ) ) {
        ExtraRptIncrementCtr( ctr_symbol_decl );
        IfDbgToggle( browse ) {
            DbgStmt( VBUF vbuf );
            DbgStmt( printf( "Br-inf-dcl: symbol is %s", DbgSymNameFull( sym, &vbuf ) ) );
            DbgStmt( DbgDumpTokenLocn( &sym->locn->tl ) );
            DbgStmt( printf( "\n" ) );
            DbgStmt( VbufFree( &vbuf ) );
        }
        if( SymIsFunction( sym ) ) {
            if( CompFlags.optbr_f ) {
                brinfIcDclSym( IC_BR_DCL_FUN, sym );
            }
        } else {
            if( CompFlags.optbr_v
             && sym->locn != NULL
             && sym->locn->tl.src_file != NULL ) {
                brinfIcDclSym( IC_BR_DCL_VAR, sym );
            }
        }
        refSymType( sym );
    }
}


void BrinfDeclTemplateClass     // VALID CLASS TEMPLATE HAS BEEN DECLARED
    ( SYMBOL tc )               // - template defined
{
    if( ! SymIsClassMember( tc )
     && activeScopesAdjust( tc ) ) {
        ExtraRptIncrementCtr( ctr_template_class_decl );
        IfDbgToggle( browse ) {
            DbgStmt( VBUF vbuf );
            DbgStmt( printf( "Br-inf-def: template class is %s", DbgSymNameFull( tc, &vbuf ) ) );
            DbgStmt( DbgDumpTokenLocn( &tc->locn->tl ) );
            DbgStmt( printf( "\n" ) );
            DbgStmt( VbufFree( &vbuf ) );
        }
        refSymType( tc );
    }
}


void BrinfDeclTypedef           // VALID TYPEDEF HAS BEEN DECLARED
    ( SYMBOL td )               // - namespace defined
{
    if( ! SymIsClassMember( td )
     && activeScopesAdjust( td ) ) {
        ExtraRptIncrementCtr( ctr_typedef );
        IfDbgToggle( browse ) {
            DbgStmt( VBUF vbuf );
            DbgStmt( printf( "Br-inf-def: typedef is %s", DbgSymNameFull( td, &vbuf ) ) );
            DbgStmt( DbgDumpTokenLocn( &td->locn->tl ) );
            DbgStmt( printf( "\n" ) );
            DbgStmt( VbufFree( &vbuf ) );
        }
        if( CompFlags.optbr_t ) {
            brinfIcDclSym( IC_BR_DCL_TDEF, td );
        }
        refSymType( td );
    }
}


void BrinfReferenceSymbol       // SYMBOL REFERENCE
    ( TOKEN_LOCN* locn          // - location of access
    , SYMBOL sym )              // - symbol accessed
{
    if( BrinfActive()
     && SymIsBrowsable( sym ) ) {
        IfDbgToggle( browse ) {
            DbgStmt( VBUF vbuf );
            DbgStmt( printf( "Br-inf-ref: symbol is %s", DbgSymNameFull( sym, &vbuf ) ) );
            DbgStmt( DbgDumpTokenLocn( locn ) );
            DbgStmt( printf( "\n" ) );
            DbgStmt( VbufFree( &vbuf ) );
        }
        if( SymIsFunction( sym ) ) {
            ExtraRptIncrementCtr( ctr_ref_func );
            if( CompFlags.optbr_f ) {
                brinfIcReference( IC_BR_REF_FUN, sym, locn );
            }
        } else if( SymIsData( sym ) ) {
            if( SymIsClassMember( sym ) ) {
                if( SymIsThisMember( sym ) ) {
                    ExtraRptIncrementCtr( ctr_ref_data_memb_this );
                    if( CompFlags.optbr_m ) {
                        brinfIcReference( IC_BR_REF_CLM, sym, locn );
                    }
                } else {
                    ExtraRptIncrementCtr( ctr_ref_data_memb_static );
                    if( CompFlags.optbr_v
                     || CompFlags.optbr_m ) {
                        brinfIcReference( IC_BR_REF_VAR, sym, locn );
                    }
                }
            } else {
            #ifndef NDEBUG
                if( SymIsAutomatic( sym ) ) {
                    ExtraRptIncrementCtr( ctr_ref_data_bare_auto );
                } else {
                    ExtraRptIncrementCtr( ctr_ref_data_bare_static );
                }
            #endif
                if( CompFlags.optbr_v ) {
                    brinfIcReference( IC_BR_REF_VAR, sym, locn );
                }
            }
        } else if( SymIsTypedef( sym ) ) {
            ExtraRptIncrementCtr( ctr_ref_typedef );
            if( CompFlags.optbr_t ) {
                brinfIcReference( IC_BR_REF_TYPE, sym->sym_type, locn );
            }
        } else if( SymIsEnumeration( sym ) ) {
            ExtraRptIncrementCtr( ctr_ref_eval );
            if( CompFlags.optbr_v ) {
                brinfIcReference( IC_BR_REF_EVAL, sym, locn );
            }
        } else if( SymIsEnumDefinition( sym ) ) {
            ExtraRptIncrementCtr( ctr_ref_evar );
            if( CompFlags.optbr_v ) {
                brinfIcReference( IC_BR_REF_EVAR, sym, locn );
            }
        } else {
            ExtraRptIncrementCtr( ctr_ref_other );
        }
    }
}


MEPTR BrinfDeclMacro            // DECLARE MACRO
    ( MEPTR mac )               // - the macro
{
    if( NULL != mac
     && CompFlags.optbr_p ) {
        if( canWriteIc() ) {
            MACVALUE* val = BrinfMacAddValue( mac );
            ExtraRptIncrementCtr( ctr_defn_macro );
            BrinfSrcMacDecl( val );
        }
    }
    return mac;
}


void BrinfDependsMacroValue     // DEPENDENCY: MACRO VALUE
    ( MEPTR mac )               // - the macro
{
    MACVALUE* val;              // - value for macro

    if( BrinfActive() ) {
        ExtraRptIncrementCtr( ctr_dep_macro_value );
        val = BrinfMacAddValue( mac );
        BrinfDepMacAdd( mac, val, MVT_VALUE );
    }
}


MEPTR BrinfReferenceMacro       // REFERENCE A MACRO VALUE
    ( MEPTR mac )               // - the macro
{
    if( enableMacRefs
     && CompFlags.optbr_p ) {
        MACVALUE* val = BrinfMacAddValue( mac );
        ExtraRptIncrementCtr( ctr_ref_macro );
        // the following is over-conservative, since a reference is also
        // generated for #ifdef, #ifndef
        BrinfDependsMacroValue( mac );
        BrinfSrcMacReference( val );
    }
    return mac;
}


boolean BrinfDependsMacroDefined // DEPENDENCY: MACRO DEFINED OR NOT
    ( boolean defed             // - whether defined or not
    , char const * name         // - characters in name
    , unsigned nlen )           // - name length
{
    MACVALUE* val;              // - value for macro
    MAC_VTYPE type;             // - type of dependency
    MEPTR mac;                  // - NULL or entry def'ed or undef'ed

    if( BrinfActive() ) {
        ExtraRptIncrementCtr( ctr_dep_macro_defined );
        if( defed ) {
            val = BrinfMacAddDefin( name, nlen );
            enableMacRefs = FALSE;
            mac = MacroLookup( name, nlen );
            enableMacRefs = TRUE;
            type = MVT_DEFINED;
        } else {
            val = BrinfMacAddUndef( name, nlen );
            mac = NULL;
            type = MVT_UNDEFED;
        }
        BrinfDepMacAdd( mac, val, type );
    }
    return defed;
}


void BrinfUndefMacro            // UNDEFINE A MACRO
    ( MEPTR mac )               // - that macro
{
    if( BrinfActive() ) {
        BrinfMacUndef( mac, SrcFileCurrent() );
    }
}


void BrinfOpenScope             // OPEN A SCOPE
    ( SCOPE scope )             // - the scope
{
    scope = scope;
}


void BrinfCloseScope            // CLOSE A SCOPE
    ( SCOPE scope )             // - the scope
{
    if( BrinfActive() ) {
#ifdef XTRA_RPT
        _dbgScope( scope, "Br-inf-end" );
        if( ! ScopeType( scope, SCOPE_CLASS )
         && scope->s.keep
         && NULL != scope->ordered ) {
            ExtraRptIncrementCtr( ctr_nontrivial_scopes );
        }
        if( scope == PstkTopElement( &active_scopes ) ) {
            scope = PstkPopElement( &active_scopes );
            closeScopeIc( scope );
        }
        endBlkScope( scope );
#endif
    }
}


void BrinfIncludeSource         // INCLUDE A SOURCE FILE
    ( char const * fname        // - file name (scanned)
    , TOKEN_LOCN * locn )       // - place
{
    IfDbgToggle( browse ) {
        DbgStmt( printf( "Br-include-source: %s"
                       , fname
                       )
               );
        DbgStmt( DbgDumpTokenLocn( locn ) );
        DbgStmt( printf( "\n" ) );
    }
    if( BrinfActive() ) {
        BrinfSrcInc( (void*)fname, locn );
    }
}


void BrinfOpenSource            // OPEN A SOURCE FILE
    ( SRCFILE srcfile )         // - source file
{
    ExtraRptIncrementCtr( ctr_open_source );
    IfDbgToggle( browse ) {
        DbgStmt( printf( "Br-open-source: %s\n"
                       , SrcFileFullName( srcfile )
                       )
               );
    }
    if( BrinfActive() ) {
        BrinfSrcBeg( srcfile );
    }
}

void BrinfCloseSource           // CLOSE A SOURCE FILE
    ( SRCFILE srcfile )         // - file being closed
{
    IfDbgToggle( browse ) {
        DbgStmt( printf( "Br-close-source\n" ) );
    }
    if( BrinfActive() ) {
        BrinfDepSrcEnd();
        BrinfSrcEndFile( srcfile );
    }
}


static void completeInputPhase  // COMPLETION OF INPUT PHASE
    ( void )
{
    activeScopesClose();
    BrinfSrcFini();
    CgioCloseOutputFile( virtual_file );
}


void BrinfWrite                 // WRITE OUT BROWSE INFORMATION
    ( void )
{
    if( BrinfActive() ) {
        completeInputPhase();
        brinfo_state = BRS_COLLECTED;
        brinfOpen();
        brinfo_state = BRS_WRITING;
        CgioOpenInput( virtual_file );
        BrinfWriteFile( virtual_file, &rtns, usage_file, bri_handle );
        CgioCloseInputFile( virtual_file );
        brinfClose();
        CgioFreeFile( virtual_file );
        BrinfMacroRelease();
    }
}


static void startBrinfPCH       // START A BROWSE-FILE WHICH HAS PCH REFERENCE
    ( void )
{
    char full_name[ _MAX_PATH ];
    char *fname;
    SRCFILE srcf;
    TOKEN_LOCN start = { 0, 0, 0 };

    BrinfInit( FALSE );
    fname = IoSuppFullPath( PCHFileName(), full_name, sizeof( full_name ) );
    fname = FNameAdd( fname );
    BrinfWriteIns( IC_BR_PCH, fname );
    srcf = SrcFileCurrent();
    BrinfSrcInc( SrcFileName( srcf ), &start );
    BrinfSrcBeg( srcf );
}


unsigned long BrinfPch          // WRITE OUT PCH IF REQ'D
    ( int handle )              // - file handle
{
    unsigned long retn;         // - posn for browse info in PCH file

    if( BrinfActive() ) {
        PCHFlushBuffer();
        retn = tell( handle );
// error return ?
        completeInputPhase();   // - note: this destroys a carver in brinfsrc
        brinfo_state = BRS_WRITING;
        CgioOpenInput( virtual_file );
        bri_handle = BrinfWritePchFile( virtual_file
                                      , &rtns_pch
                                      , (void*)handle
                                      , retn );
        CgioCloseInputFile( virtual_file );
        CgioFreeFile( virtual_file );
        BrinfSrcInit();         // - to restart the carver in brinfsrc
        startBrinfPCH();
    } else {
        retn = 0;
    }
    return retn;
}


void BrinfPchRead               // INSERT PCH REFERENCE INTO BROWSING
    ( void )
{
    BRI_PCH_CTL         ctl;
    BRI_PCHRtns         pchrtns = {
        &brinfReadPch1,
        &brinfReadPch4,
        &brinfReadPch
    };

    if( BrinfActive() ) {
        brinfo_state = BRS_PCH_RESTART;
        destroyBrinfData();
        BrinfDepRestart();
        BrinfMacRestart();
        BrinfSrcRestart();
        DbgAssert( bri_handle == NULL );
        bri_handle = BrinfCreateFile( &rtns );
        startBrinfPCH();
        brinfPchInit( &ctl );
        BrinfReadPchFile( &pchrtns, &ctl );
        brinfPchFini( &ctl );
    }
}


void BrinfInit                  // START OF PROCESSING FOR BROWSE INFO
    ( boolean primary )         // - TRUE ==> primary initialization
{
    if( primary ) {
        bri_handle = NULL;
    }
    usage_file = NULL;
    usage_name = NULL;
    virtual_file = NULL;
    brinfo_state = BRS_INACTIVE;
    if( BrinfActive() ) {
        ring_active_blks = NULL;
        carve_blk_scope = CarveCreate( sizeof( ACTBLK ), 32 );
        carve_scope_ins = CarveCreate( sizeof( SCPINS ), 64 );
        if( primary ) {
            BrinfDepInit();
        }
        virtual_file = CgioCreateFile( BRINF_SYMBOL );
        if( primary ) {
            BrinfSrcInit();
        }
        brinfo_state = BRS_COLLECTING;
        PstkOpen( &active_scopes );
        PstkOpen( &sym_scopes );
        ref_file = NULL;
        ref_line = 0;
        ref_col = 0;
        enableMacRefs = TRUE;
        if( primary ) {
            MacroWriteBrinf();
        }
    } else {
        activeScopesClose();
    }
}


static void brinfInit           // START OF PROCESSING FOR BROWSE INFO
    ( INITFINI* defn )          // - definition
{
    ExtraRptRegisterCtr( &ctr_namespace_decl
                       , "BrowseInfo: # namespace declarations" );
    ExtraRptRegisterCtr( &ctr_symbol_decl
                       , "BrowseInfo: # other symbol declarations" );
    ExtraRptRegisterCtr( &ctr_template_class_decl
                       , "BrowseInfo: # template class declarations" );
    ExtraRptRegisterCtr( &ctr_typedef
                       , "BrowseInfo: # declarations" );
    ExtraRptRegisterCtr( &ctr_defn_macro
                       , "BrowseInfo: # macro declarations" );
    ExtraRptRegisterCtr( &ctr_class_decl
                       , "BrowseInfo: # class declarations" );
    ExtraRptRegisterCtr( &ctr_class_base
                       , "BrowseInfo: # class bases" );
    ExtraRptRegisterCtr( &ctr_class_memb
                       , "BrowseInfo: # class members" );
    ExtraRptRegisterCtr( &ctr_class_fr
                       , "BrowseInfo: # class friends" );
    ExtraRptRegisterCtr( &ctr_ref_func
                       , "BrowseInfo: # ref. to functions" );
    ExtraRptRegisterCtr( &ctr_ref_data_memb_this
                       , "BrowseInfo: # ref. to this data members" );
    ExtraRptRegisterCtr( &ctr_ref_data_memb_static
                       , "BrowseInfo: # ref. to static data members" );
    ExtraRptRegisterCtr( &ctr_ref_data_bare_auto
                       , "BrowseInfo: # ref. to auto data variables" );
    ExtraRptRegisterCtr( &ctr_ref_data_bare_static
                       , "BrowseInfo: # ref. to static data variables" );
    ExtraRptRegisterCtr( &ctr_ref_typedef
                       , "BrowseInfo: # ref. to typedefs" );
    ExtraRptRegisterCtr( &ctr_ref_eval
                       , "BrowseInfo: # ref. to enumeration values" );
    ExtraRptRegisterCtr( &ctr_ref_evar
                       , "BrowseInfo: # ref. to enumeration variables" );
    ExtraRptRegisterCtr( &ctr_ref_other
                       , "BrowseInfo: # ref. to unknown stuff" );
    ExtraRptRegisterCtr( &ctr_ref_macro
                       , "BrowseInfo: # ref. to macros" );
    ExtraRptRegisterCtr( &ctr_dep_macro_defined
                       , "BrowseInfo: # dep. on macro (un)defined" );
    ExtraRptRegisterCtr( &ctr_dep_macro_value
                       , "BrowseInfo: # dep. on macro values" );
    ExtraRptRegisterCtr( &ctr_open_scope
                       , "BrowseInfo: # opened scopes" );
    ExtraRptRegisterCtr( &ctr_nontrivial_scopes
                       , "BrowseInfo: # non-trivial scopes" );
    ExtraRptRegisterCtr( &ctr_open_source
                       , "BrowseInfo: # opened source files" );
}


static void brinfFini           // COMPLETION OF PROCESSING FOR BROWSE INFO
    ( INITFINI* defn )          // - definition
{
    defn = defn;
    if( brinfo_state != BRS_CMDLINE
     && brinfo_state != BRS_INACTIVE ) {
        switch( brinfo_state ) {
          DbgDefault( "impossible brinfo state" );
          case BRS_COLLECTING :
            completeInputPhase();
            BrinfDepFini();
            break;
          case BRS_COLLECTED :
            BrinfDepFini();
            break;
          case BRS_WRITING :
            brinfClose();
            BrinfDepFini();
            break;
        }
        CgioFreeFile( virtual_file );
        virtual_file = NULL;
        brinfo_state = BRS_INACTIVE;
    }
    CMemFreePtr( &usage_name );
}


INITDEFN( browse_info, brinfInit, brinfFini );


void BrinfCmdLine               // INDICATE CMD-LINE BEING PROCESSED
    ( void )
{
    brinfo_state = BRS_CMDLINE;
}


#endif
