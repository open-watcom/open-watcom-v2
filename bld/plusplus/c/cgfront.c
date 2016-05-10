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


#include "plusplus.h"

#include <limits.h>

#include "preproc.h"
#include "cgfront.h"
#include "memmgr.h"
#include "codegen.h"
#include "toggle.h"
#include "label.h"
#include "cgsegid.h"
#include "initdefs.h"
#include "pcheader.h"
#include "objmodel.h"
#include "stats.h"
#include "fnbody.h"
#include "tgtenv.h"
#ifndef NDEBUG
#include "pragdefn.h"
#endif

#define CS_LABEL_BLOCK  4       // number of CS labels to allocate at a time

#define CGFilePCHRead()     CGFileMapIndex( (CGFILE *)(pointer_int)PCHReadCVIndex() )
#define CGFilePCHWrite(x)   PCHWriteCVIndex( (cv_index)(pointer_int)CGFileGetIndex(x) )

static CGFILE *dataCGFILE;      // file for data
static CGFILE *codeCGFILE;      // file for code
static CGFILE **emitDataCGFILE; // points to dataCGFILE or codeCGFILE

static CGINTER ins_def_seg      // IC_DEF_SEG instruction
       = { 0, IC_DEF_SEG };
static CGINTER ins_init_beg     // IC_INIT_BEG instruction
       = { 0, IC_INIT_BEG };
static CGINTER ins_init_done    // IC_INIT_DONE instruction
       = { 0, IC_INIT_DONE };
static CGINTER ins_init_test    // IC_INIT_TEST instruction
       = { 0, IC_INIT_TEST };

static struct {                  // FLAGS FOR CGFRONT
    unsigned init_data_beg   :1; // - init-data-beg required
    unsigned init_data_end   :1; // - init-data-end required
} flags;

#ifndef NDEBUG
static void cgfront_debug(      // DEBUGGING ROUTINE
    char *str )                 // - prefix
{
    if( PragDbgToggle.dump_emit_ic ) {
        printf( "%s\n", str );
    }
}

#define dump_label( inst ) if( PragDbgToggle.dump_labels ) inst
#else
#define cgfront_debug( str )
#define dump_label( inst )
#endif


// function macros
#ifndef NDEBUG
static void cgWriteIC( CGFILE *f, CGINTER *i )
{
    CgioWriteIC( f, i );
}
#else
#define cgWriteIC( f, i )       CgioWriteIC( f, i );
#endif


SYMBOL CgFrontCurrentFunction(  // GET CURRENT FUNCTION BEING GENERATED
    void )
{
    SYMBOL curr;                // - current function

    if( codeCGFILE == NULL ) {
        curr = NULL;
    } else {
        curr = codeCGFILE->symbol;
    }
    return curr;
}


void CgFrontResumeFunction(     // RESUME A FUNCTION
    SYMBOL func )               // - function being resumed
{
    if( func != NULL ) {
        codeCGFILE = CgioLocateFile( func );
    }
}


static CGFILE_GEN *getGenData(  // GET PTR TO GENERATION DATA
    void )
{
    CGFILE *stat_file;          // - file for statistics

    if( codeCGFILE == NULL ) {
        stat_file = dataCGFILE;
    } else {
        stat_file = codeCGFILE;
    }
    return( stat_file->gen );
}


static void cgEmitData(         // EMIT AN INSTRUCTION TO DATA FILE
    CGINTER *instruction )      // - instruction to be emitted
{
    CGFILE *emit_file;          // - file for emit

    emit_file = *emitDataCGFILE;
    if( flags.init_data_beg ) {
        cgWriteIC( emit_file, &ins_init_beg );
        flags.init_data_beg = false;
        flags.init_data_end = true;
    }
    cgWriteIC( emit_file, instruction );
}


static void cgEmitIns(          // EMIT AN INSTRUCTION TO CODE FILE
    CGFILE_GEN *gen,            // - generation data
    CGINTER *instruction )      // - instruction to be emitted
{
    if( gen->emit_init_beg ) {
        ins_init_beg.value.pvalue = gen->init_sym;
        cgWriteIC( codeCGFILE, &ins_init_beg );
        gen->emit_init_beg = false;
        gen->emit_init_end = true;
    }
    cgWriteIC( codeCGFILE, instruction );
}


void CgFrontStatInit(           // START OF STATIC-INIT TEST IN FUNCTION
    void )
{
    CGFILE_GEN *gen;            // - generation data

    if( emitDataCGFILE == &codeCGFILE ) {
        gen = getGenData();
        ins_init_done.value.pvalue = gen->init_sym;
        cgWriteIC( codeCGFILE, &ins_init_done );
        ins_init_test.value.pvalue = gen->init_sym;
        cgWriteIC( codeCGFILE, &ins_init_test );
        flags.init_data_beg = false;
        flags.init_data_end = true;
    }
}


void CgFrontFuncInitFlags(      // GENERATE INIT FLAGS FOR FUNCTION
    SYMBOL var )                // - NULL OR FLAGS VARIABLE
{
    if( var != NULL ) {
        ins_init_beg.value.pvalue = var;
        ins_init_done.value.pvalue = var;
        CgSegIdData( var, SI_ALL_ZERO );
        cgWriteIC( codeCGFILE, &ins_init_beg );
        emitDataCGFILE = &codeCGFILE;
        DgSymbol( var );
        cgWriteIC( codeCGFILE, &ins_init_done );
        emitDataCGFILE = &dataCGFILE;
    }
}


static void cgEmitCodeUint(     // EMIT INSTRUCTION TO CODE FILE WITH UINT PARM
    CGFILE_GEN *gen,            // - generation data
    CGINTEROP opcode,           // - opcode
    unsigned value )            // - value
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.uvalue = value;
    cgEmitIns( gen, &ins );
}


static void cgEmitCodePtr(      // EMIT INSTRUCTION TO CODE FILE WITH PTR PARM
    CGFILE_GEN *gen,            // - generation data
    CGINTEROP opcode,           // - opcode
    void *value )               // - value
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.pvalue = value;
    cgEmitIns( gen, &ins );
}


static void emitSetType(        // GENERATE TYPE IF REQ'D
    CGFILE_GEN *gen )           // - generation data
{
    if( gen->curr_type != gen->emit_type && gen->curr_type != NULL ) {
        gen->emit_type = gen->curr_type;
        cgEmitCodePtr( gen, IC_SET_TYPE, gen->curr_type );
    }
}


void CgSetTypeExact(            // SET EXACT TYPE FOR GENERATION
    TYPE type )                 // - the type
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    gen->curr_type = type;
}


void CgFrontResultBoolean(      // SET RESULTANT TYPE TO BE BOOLEAN
    void )
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    gen->emit_type = GetBasicType( TYP_BOOL );
    gen->curr_type = gen->emit_type;
}


void CgSetType(                 // SET NORMAL TYPE FOR GENERATION
    TYPE type )                 // - the type
{
    if( OMR_CLASS_REF == ObjModelArgument( type ) ) {
        type = MakeReferenceTo( type );
    }
    CgSetTypeExact( type );
}


static void emitSourcePosn(     // GENERATE DEBUG POSITION IF REQ'D
    CGFILE_GEN *gen,            // - generation data
    TOKEN_LOCN *srcposn )       // - source position
{
    DbgAssert( srcposn != NULL );
    if( srcposn->src_file != NULL ) {
        if( gen->emit_line_no.src_file != srcposn->src_file ) {
            cgEmitCodePtr( gen, IC_DBG_SRCFILE, srcposn->src_file );
            cgEmitCodeUint( gen, IC_DBG_LINE, srcposn->line );
            gen->emit_line_no.src_file = srcposn->src_file;
            gen->emit_line_no.line = srcposn->line;
        } else if( gen->emit_line_no.line != srcposn->line ) {
            cgEmitCodeUint( gen, IC_DBG_LINE, srcposn->line );
            gen->emit_line_no.line = srcposn->line;
        }
    }
}


static void cgEmit(             // EMIT INSTRUCTION TO CODE FILE, IF REQUIRED
    CGFILE_GEN *gen,            // - generation data
    CGINTER *instruction )      // - instruction to be emitted
{
    emitSetType( gen );
    switch( instruction->opcode ) {
    case IC_INIT_DONE :
    case IC_FUNCTION_OPEN :
        gen->curr_type = NULL;
        break;
    }
    cgEmitIns( gen, instruction );
}


void CgFrontDbgLine(            // SET LINE FOR DEBUGGING
    TOKEN_LOCN* posn )          // - source file position
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    emitSourcePosn( gen, posn );
#ifndef NDEBUG
    if( PragDbgToggle.dump_tokens || PragDbgToggle.dump_emit_ic ) {
        printf( "CgFrontDbgLine: %d\n", posn->line );
    }
#endif
}


CGFILE_INS CgFrontLastIns(      // RETURN LOCATION OF LAST WRITTEN CODE IC
    void )
{
    return CgioLastWrite( codeCGFILE );
}


void CgFrontCode(               // EMIT TO CODE SEGMENT
    CGINTEROP opcode )          // - intermediate code
{
    CGINTER ins = { 0 };        // - instruction

    ins.opcode = opcode;
    cgEmit( getGenData(), &ins );
}


void CgFrontCodeInt(            // EMIT (code,int) TO CODE SEGMENT
    CGINTEROP opcode,           // - opcode
    int value )                 // - value
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.ivalue = value;
    cgEmit( getGenData(), &ins );
}


void CgFrontCodeUint(           // EMIT (code,unsigned) TO CODE SEGMENT
    CGINTEROP opcode,           // - opcode
    unsigned value )            // - value
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.uvalue = value;
    cgEmit( getGenData(), &ins );
}


void CgFrontCodePtr(            // EMIT (code,ptr) TO CODE SEGMENT
    CGINTEROP opcode,           // - opcode
    void *value )               // - value
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.pvalue = value;
    cgEmit( getGenData(), &ins );
}


void CgFrontData(               // EMIT TO DATA SEGMENT
    CGINTEROP opcode )          // - intermediate code
{
    CGINTER ins = { 0 };        // - instruction

    ins.opcode = opcode;
    cgEmitData( &ins );
}


void CgFrontDataInt(            // EMIT (code,int) TO DATA SEGMENT
    CGINTEROP opcode,           // - opcode
    int value )                 // - value
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.ivalue = value;
    cgEmitData( &ins );
}


void CgFrontDataUint(           // EMIT (code,unsigned) TO DATA SEGMENT
    CGINTEROP opcode,           // - opcode
    unsigned value )            // - value
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.uvalue = value;
    cgEmitData( &ins );
}


void CgFrontDataPtr(            // EMIT (code,ptr) TO DATA SEGMENT
    CGINTEROP opcode,           // - opcode
    void *value )               // - value
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.pvalue = value;
    cgEmitData( &ins );
}

static void cgSetupSegment(     // SET UP DATA SEGMENT, EMIT INSTRUCTION
    unsigned seg_number )       // - segment number
{
    if( seg_number != ins_def_seg.value.ivalue ) {
        ins_def_seg.value.uvalue = seg_number;
        cgEmitData( &ins_def_seg );
    }
}


void CgFrontFiniRef(            // EMIT TO FINI-REF SEGMENT
    void )
{
    cgSetupSegment( SEG_FINI_REF );
}


void CgFrontInitRef(            // EMIT TO INIT-REF SEGMENT
    void )
{
    cgSetupSegment( SEG_INIT_REF );
}


void CgFrontZapPtr(             // ZAP A WRITTEN RECORD (PTR OPERAND)
    CGFILE_INS location,        // - location to be zapped
    CGINTEROP opcode,           // - opcode
    void *operand )             // - operand
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.pvalue = operand;
    CgioRewriteRecord( location, &ins );
}


void CgFrontZapUint(            // ZAP A WRITTEN RECORD (UNSIGNED OPERAND)
    CGFILE_INS location,        // - location to be zapped
    CGINTEROP opcode,           // - opcode
    unsigned operand )          // - operand
{
    CGINTER ins;                // - instruction

    ins.opcode = opcode;
    ins.value.uvalue = operand;
    CgioRewriteRecord( location, &ins );
}


void *CgFrontSwitchFile(        // SWITCH VIRTUAL FUNCTION FILES
    void *symbol )              // - symbol for opened file
{
    CGFILE *retn;               // - old code file is returned

    retn = codeCGFILE;
    if( ( retn == NULL ) || ( retn->symbol != symbol ) ) {
        codeCGFILE = CgioLocateFile( symbol );
    }
    return retn;
}


unsigned CgFrontLabelCs(        // GET NEXT AVAILABLE LABEL # (CONTROL SEQ)
    void )
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    gen->cs_label++;
    if( gen->cs_allocated != 0 ) {
        gen->cs_allocated--;
    } else {
        cgEmitCodeUint( gen, IC_LABGET_CS, CS_LABEL_BLOCK );
        gen->cs_allocated = CS_LABEL_BLOCK - 1;
    }
    dump_label( printf( "Allocate CS Label: %u\n", gen->cs_label ) );
    return( gen->cs_label );
}


unsigned CgFrontLabelGoto(      // GET NEXT AVAILABLE LABEL # (GOTO)
    void )
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    gen->goto_label++;
    cgEmitCodeUint( gen, IC_LABGET_GOTO, 0 );
    dump_label( printf( "LabelGoto -- %d\n", gen->goto_label ) );
    return( gen->goto_label );
}


static void label_reference(    // EMIT NEAR LABEL REFERENCE
    CGLABEL label,              // - label
    CGINTEROP opcode )          // - opcode
{
    CgFrontCodeInt( opcode, label - 1 );
}


void CgFrontGotoNear(           // EMIT GOTO IN NEAR SPACE (CS,GOTO)
    CGINTEROP opcode,           // - opcode to determine type of label ref.
    unsigned condition,         // - condition for goto
    CGLABEL label )             // - label number
{
    label_reference( label, opcode );
    CgFrontCodeUint( IC_GOTO_NEAR, condition );
    dump_label( printf( "GotoNear -- %d\n", label ) );
}


void CgFrontLabfreeCs(          // FREE CS LABELS
    unsigned count )            // - number of labels to be freed
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    gen->cs_label -= count;
    count += gen->cs_allocated;
    gen->cs_allocated = 0;
    dump_label( printf( "LabfreeCs -- %d\n", count ) );
    cgEmitCodeUint( gen, IC_LABFREE_CS, count );
}


void CgFrontLabdefCs(           // DEFINE A CS LABEL
    CGLABEL label )             // - label number
{
    label_reference( label, IC_LABDEF_CS );
    dump_label( printf( "LabdefCs -- %d\n", label ) );
}


void CgFrontLabdefGoto(         // DEFINE A GOTO LABEL
    CGLABEL label )             // - label number
{
    label_reference( label, IC_LABDEF_GOTO );
    dump_label( printf( "LabdefGoto -- %d\n", label ) );
}


CGFILE_INS CgFrontFuncOpen(     // OPEN A FUNCTION (AND ITS FILE)
    SYMBOL func,                // - symbol for function
    TOKEN_LOCN *posn )          // - source position
{
    CGFILE_GEN *gen;            // - generation data
    DT_METHOD dtm;              // - destruction method for function
    CGFILE_INS reg;             // - position to zap for function registration

    codeCGFILE = CgioCreateFile( func );
    if( posn != NULL ) {
        codeCGFILE->defined = *posn;
    }
    gen = getGenData();
    if( posn != NULL ) {
        emitSourcePosn( gen, posn );
    }
    CgFrontCodePtr( IC_FUNCTION_OPEN, func );
    CgFrontCodePtr( IC_FUNCTION_ARGS, GetCurrScope() );
    dtm = CompInfo.dt_method_speced;
    if( ! SymIsDtor( func ) ) {
        switch( dtm ) {
          case DTM_DIRECT_SMALL :
            dtm = DTM_DIRECT;
            break;
          case DTM_TABLE_SMALL :
            dtm = DTM_TABLE;
            break;
        }
    }
    CompInfo.dt_method = dtm;
    CgFrontCodeUint( IC_FUNCTION_DTM, dtm );
    CgFrontCode( IC_NO_OP );    // - may be zapped to IC_FUNCTION_STAB
    reg = CgioLastWrite( codeCGFILE );
    gen->emit_line_no.src_file = NULL;
    if( posn != NULL ) {
        emitSourcePosn( gen, posn );
    }
    return reg;
}


void CgFrontFuncClose(          // CLOSE A FUNCTION (AND ITS FILE)
    SYMBOL func )               // - symbol for function
{
    CgFrontCode( IC_FUNCTION_CLOSE );
    if( FnRetnOpt() ) {
        SYMBOL opt_sym = FnRetnOptSym();
        if( opt_sym != NULL ) {
            codeCGFILE->opt_retn = opt_sym;
            switch( ObjModelFunctionReturn( func->sym_type ) ) {
              case OMR_CLASS_REF :
                codeCGFILE->u.s.opt_retn_ref = true;
                break;
              case OMR_CLASS_VAL :
              case OMR_SCALAR :
                codeCGFILE->u.s.opt_retn_val = true;
                break;
            }
        }
    }
    CgioCloseOutputFile( codeCGFILE );
    codeCGFILE = NULL;
}


void CgFrontModInitInit(        // INITIALIZE MODULE-INITIALIZATION
    void )
{
    ModuleInitInit();
    codeCGFILE = NULL;
    emitDataCGFILE = &dataCGFILE;
    DbgAssert( ins_def_seg.opcode == IC_DEF_SEG );
    DbgAssert( ins_init_beg.opcode == IC_INIT_BEG );
    DbgAssert( ins_init_done.opcode == IC_INIT_DONE );
    DbgAssert( ins_init_test.opcode == IC_INIT_TEST );
}


void *CgFrontModInitOpen(       // OPEN FOR MODULE-INITIALIZATION CODE GEN
    SCOPE *save_scope )         // - more info to save
{
    void *save;                 // - save value

    save = codeCGFILE;
    CgFrontSwitchFile( ModuleInitFuncSym() );
    *save_scope = ModuleInitResume();
    return( save );
}


void CgFrontModInitUsed(        // MARK MODULE-INIT. FUNCTION USED
    void )
{
    ModuleInitUsed();
}


void CgFrontModInitFini(        // PROCESS MODULE-INIT. AFTER FRONT-END
    void )
{
    SYMBOL mod;                 // - symbol for module-initialization func.
    CGFILE *fp;                 // - file for module initialization

    mod = ModuleInitFuncSym();
    if( SymIsReferenced( mod ) && ErrCount == 0 ) {
        ModuleInitConnect();
    } else {
        mod->flag |= SF_REFERENCED;
        fp = CgioLocateFile( mod );
        CgioFreeFile( fp );
    }
}


void CgFrontModInitClose(       // CLOSE AFTER MODULE-INIT. PROCESSING
    SCOPE save_scope,           // - from ModuleInitResume
    void *save )                // - result from CgFrontModInitOpen
{
    ModuleInitRestore( save_scope );
    codeCGFILE = save;
}


void CgFrontSwitchBeg(          // START A SWITCH STATEMENT
    void )
{
    CgFrontCode( IC_SWITCH_BEG );
}


void CgFrontSwitchEnd(          // END A SWITCH STATEMENT
    void )
{
    LabelSwitchEnd();
}


void CgFrontSwitchDefault(      // DEFAULT STATEMENT IN SWITCH STATEMENT
    SCOPE scope_sw,             // - scope for switch jump
    bool deadcode,              // - dead-code state
    TOKEN_LOCN *posn )          // - source-file position
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    emitSourcePosn( gen, posn );
    CgFrontCode( IC_SWITCH_DEFAULT );
    LabelSwitchLabel( scope_sw, deadcode );
    cgfront_debug( "default" );
}


void CgFrontSwitchCase(         // CASE STATEMENT IN SWITCH STATEMENT
    SCOPE scope_sw,             // - scope for switch jump
    bool deadcode,              // - dead-code state
    TOKEN_LOCN *posn,           // - source-file position
    uint_32 value )             // - switch value
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    emitSourcePosn( gen, posn );
    CgFrontCodeUint( IC_SWITCH_CASE, value );
    LabelSwitchLabel( scope_sw, deadcode );
    cgfront_debug( "case" );
}


void CgFrontSymbol(             // EMIT A SYMBOL
    SYMBOL sym )                // - the symbol
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    gen->emit_type = sym->sym_type;
    CgSetType( sym->sym_type );
    CgFrontCodePtr( IC_LEAF_NAME_FRONT, sym );
}


void CgFrontReturnSymbol(       // SET RETURN SYMBOL
    SYMBOL return_operand )     // - the symbol
{
    LabelReturn();
    CgFrontCodePtr( IC_PROC_RETURN, return_operand );
}


void CgFrontScopeCall(          // GENERATE IC_SCOPE_CALL, IF REQ'D
    SYMBOL fun,                 // - function called
    SYMBOL dtor,                // - dtor, when function is ctor
    DTORING_KIND kind )         // - kind of dtoring
{
    bool keep_scope;            // - true ==> keep the current scope

    keep_scope = false;
    if( dtor != NULL ) {
        switch( kind ) {
          case DTORING_SCOPE :
            CgFrontCodePtr( IC_SCOPE_CALL_BDTOR, dtor );
            keep_scope = true;
            break;
          case DTORING_TEMP :
            CgFrontCodePtr( IC_SCOPE_CALL_TDTOR, dtor );
            break;
          case DTORING_COMPONENT :
            CgFrontCodePtr( IC_SCOPE_CALL_CDTOR, dtor );
            keep_scope = true;
            break;
          DbgDefault( "CgFrontScopeCall -- bad DTORING_KIND" );
        }
    }
    if( fun != NULL && ( fun->flag & SF_NO_LONGJUMP ) ) {
        fun = NULL;
    }
    if( fun != NULL || dtor != NULL ) {
        CgFrontCodePtr( IC_SCOPE_CALL_FUN, fun );
    }
    if( keep_scope ) {
        ScopeKeep( GetCurrScope() );
    }
}


void CgFrontCtorTest(           // INDICATE FUNCTION MIGHT HAVE CTOR-TEST
    void )
{
    codeCGFILE->u.s.ctor_test = true;
}


bool CgFrontRetnOptVar(         // START BRACKETTING FOR VAR. (RETURN OPT>)
    SYMBOL var )                // - the symbol
{
    bool retb;                  // - return: true ==> bracketting started

    if( FnRetnOptimizable( var ) ) {
        CgFrontCodePtr( IC_RETNOPT_VAR, var );
        retb = true;
    } else {
        retb = false;
    }
    return( retb );
}


void CgFrontRetnOptEnd(         // COMPLETE BRACKETTING FOR RETURN OPT.
    void )
{
    if( FnRetnOpt() ) {
        CgFrontCode( IC_RETNOPT_END );
    }
}


void DgInitBegin(               // START INITIALIZATION OF SYMBOL
    SYMBOL sym )                // - the symbol
{
    CGFILE_GEN *gen;            // - generation data

    ins_init_beg.value.pvalue = sym;
    gen = getGenData();
    flags.init_data_beg = true;
    flags.init_data_end = false;
    gen->init_sym = sym;
    if( sym->id == SC_STATIC && ScopeId( SymScope( sym ) ) != SCOPE_FILE ) {
        emitDataCGFILE = &codeCGFILE;
    } else {
        gen->emit_init_beg = true;
        gen->emit_init_end = false;
    }
}


void DgInitDone(                // COMPLETE INITIALIZATION OF SYMBOL
    void )
{
    CGFILE_GEN *gen;            // - generation data

    gen = getGenData();
    if( gen->emit_init_end ) {
        cgEmit( gen, &ins_init_done );
    }
    gen->emit_init_beg = false;
    gen->emit_init_end = false;
    if( flags.init_data_end ) {
        cgEmitData( &ins_init_done );
    }
    flags.init_data_beg = false;
    flags.init_data_end = false;
    emitDataCGFILE = &dataCGFILE;
}


static void cgfrontInit(        // INITIALIZE FOR FRONT-END CODE GENERATION
    INITFINI* defn )            // - definition
{
    char seg_name[30];          // - computed segment name

    defn = defn;
    if( *TextSegName == '\0' ) {
        if( IsBigCode() ) {
            stpcpy( stpcpy( seg_name, ModuleName ), TS_SEG_CODE );
        } else {
            stpcpy( seg_name, TS_SEG_CODE );
        }
        CMemFree( TextSegName );
        TextSegName = strsave( seg_name );
    }
    SegmentInit( TextSegName );
    codeCGFILE = NULL;
    dataCGFILE = CgioCreateFile( NULL );
    emitDataCGFILE = &dataCGFILE;
    flags.init_data_beg = false;
    flags.init_data_end = false;
}


static void cgfrontFini(        // COMPLETE FRONT-END CODE GENERATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    if( dataCGFILE != NULL ) {
        CgioCloseOutputFile( dataCGFILE );
        dataCGFILE = NULL;
    }
    emitDataCGFILE = NULL;
}

void CgFrontFini(               // release any memory used
    void )
{
    CMemFreePtr( &TextSegName );
}


INITDEFN( front_end, cgfrontInit, cgfrontFini )

pch_status PCHWriteFrontData( void )
{
    CGFilePCHWrite( dataCGFILE );
    CGFilePCHWrite( codeCGFILE );
    return( PCHCB_OK );
}

pch_status PCHReadFrontData( void )
{
    dataCGFILE = CGFilePCHRead();
    codeCGFILE = CGFilePCHRead();
    return( PCHCB_OK );
}

pch_status PCHInitFrontData( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniFrontData( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}
