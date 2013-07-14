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
* Description:  Object file output for RISC architectures.
*
****************************************************************************/


#include "wio.h"
#include <assert.h>
#include <string.h>
#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "cgaux.h"
#include "ocentry.h"
#include "cgmem.h"
#include "reloc.h"
#include "cgswitch.h"
#include "dbcue.h"
#include "import.h"
#include "owl.h"
#include "rscobj.h"
#include "autodep.h"
#include "axpencod.h"
#include "data.h"
#include "feprotos.h"
#include "rtrtn.h"
#include "utils.h"

#define HANDLE_TO_OWL(x)    ((owl_file_handle)(x + 1))
#define OWL_TO_HANDLE(x)    ((pointer_int)x - 1)

extern  void            CloseObj( void );
extern  void            OpenObj( void );
extern  void            PutObjBytes( const char *, uint );
extern  char            *AskRTName( rt_class );
extern  void            TryScrapLabel( code_lbl * );
extern  void            DoOutObjectName(sym_handle,void(*)(char*,void*),void*,import_type);
extern  bool            SymIsExported( sym_handle );
extern  code_lbl        *GetWeirdPPCDotDotLabel( code_lbl * );
extern  void            TellAddress( code_lbl *, offset );
extern  type_length     TempLocation( name * );
extern  pointer         SymBack( pointer );
extern  void            EmptyQueue( void );
extern  void            TellUnreachLabels( void );
extern  void            *SortList( void *, unsigned, bool (*)( void *, void * ) );
extern  void            EmitInsReloc( axp_ins, pointer, owl_reloc_type );

/* DF interface */
extern  void            DFObjInitInfo( void );
extern  void            DFObjLineInitInfo( void );
extern  void            DFBegCCU( seg_id code, long sym );
extern  void            DFDefSegs( void );
extern  void            DFObjFiniDbgInfo( offset codesize );
extern  void            DFObjLineFiniDbgInfo( void );
extern  void            DFLineNum( cue_state *, offset );
extern  void            DFSegRange( void );
extern  void            DFSymRange( sym_handle, offset );
/* CV interface */
extern  void            CVObjInitInfo( void );
extern  void            CVDefSegs( void );
extern  void            CVDefSymNormal( void );
extern  void            CVDefSymComdat( owl_section_handle depof );
extern  void            CVLineNum( cue_state *, offset );
extern  void            CVObjFiniDbgInfo( void );

static  owl_section_handle      owlTocSect; // contributions to TOC for PPC
static  owl_section_handle      globalPdata;



static  owl_handle              owlHandle;
static  owl_file_handle         owlFile;

static  seg_id                  codeSection;
static  seg_id                  dataSection;
static  seg_id                  backSectIdx;

static  section_def             *currSection;

#define N_SECTIONS              16

static  section_def             *sectionDefs[ N_SECTIONS ];

extern  int                     ObjFile;
static  short                   CurrFNo;

extern section_def *FindSection( seg_id id )
/******************************************/
{
    section_def         *curr;

    curr = sectionDefs[ id % N_SECTIONS ];
    while( curr != NULL ) {
        if( curr->id == id ) break;
        curr = curr->next;
    }
    return( curr );
}

extern section_def *AddSection( seg_id id )
/*****************************************/
{
    section_def         *new;
    unsigned            bucket;

    new = CGAlloc( sizeof( section_def ) );
    bucket = id % N_SECTIONS;
    new->id = id;
    new->next = sectionDefs[ bucket ];
    sectionDefs[ bucket ] = new;
    new->func  = NULL;
    new->is_start = TRUE;
    return( new );
}

static void DeleteSections( void )
/********************************/
{
    unsigned            bucket;
    section_def         *ptr;
    section_def         *next;

    for( bucket = 0; bucket < N_SECTIONS; bucket++ ) {
        if( sectionDefs[ bucket ] != NULL ) {
            for( ptr = sectionDefs[ bucket ]; ptr != NULL; ptr = next ) {
                next = ptr->next;
                CGFree( ptr );
            }
            sectionDefs[ bucket ] = NULL;
        }
    }
}

extern owl_section_handle DbgSectDefComdat( char *str )
/*****************************************************/
{
    owl_section_handle  owl_handle;

    owl_handle = OWLSectionInit( owlFile, str, OWL_SECTION_COMDAT_DEBUG, 1 );
    return( owl_handle );
}

extern  void    ObjInit( void )
/*****************************/
{
    OpenObj();
    CurrFNo = 0;
    if( _IsModel( DBG_DF ) ) {
        if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
            DFDefSegs();
            DFObjInitInfo();
#if 0 //save for jimr
        } else if( _IsModel( NUMBERS ) ) {
            DFDefSegs();
            DFObjLineInitInfo();
#endif
        }
    } else if( _IsModel( DBG_CV ) ) {
        CVDefSegs();
        CVObjInitInfo();
    }
}


static  void    DefaultLibs( void )
/*********************************/
{
    char               *lib;
    char               *name;
    owl_section_handle  comments;

    comments = NULL;
    lib = NULL;
    for( ;; ) {  //Library dependencies
        lib = FEAuxInfo( lib, NEXT_LIBRARY );
        if( lib == NULL ) break;
        name =  (char*)FEAuxInfo( lib, LIBRARY_NAME ) + 1;
        if( name == NULL || *name == '\0' ) continue;
        if( comments == NULL ){
            comments = OWLSectionInit( owlFile, ".drectve", OWL_SECTION_INFO, 1 );
            if( comments == NULL )break;
        }
        OWLEmitData( comments, COMMENTV( COFF_DRECTVE_DEFLIB ) );
        OWLEmitData( comments, name, strlen( name ) );
        OWLEmitData( comments, " ", 1 );
    }
    if( comments != NULL ) {
        OWLEmitData( comments, "", 1 );
    }
}

static void stringOut( char *name, void *data )
/*********************************************/
{
    *(char **)data = name;
}

static  void    AliasNames( void )
/*********************************/
{
    pointer             alias;
    char                *alias_name;
    char                *subst_name;
    owl_symbol_handle   owl_alias;
    owl_symbol_handle   owl_subst;

    alias = NULL;
    for( ;; ) {  // Aliases
        alias = FEAuxInfo( alias, NEXT_ALIAS );
        if( alias == NULL ) break;
        alias_name = FEAuxInfo( alias, ALIAS_NAME );
        if( alias_name == NULL ) {
            DoOutObjectName( FEAuxInfo( alias, ALIAS_SYMBOL ),
                             stringOut, &alias_name, NORMAL );
        }
        subst_name = FEAuxInfo( alias, ALIAS_SUBST_NAME );
        owl_alias = OWLSymbolInit( owlFile, alias_name );
        if( subst_name == NULL ) {
            DoOutObjectName( FEAuxInfo( alias, ALIAS_SUBST_SYMBOL ),
                             stringOut, &subst_name, NORMAL );
        }
        owl_subst = OWLSymbolInit( owlFile, subst_name );
        OWLWeakExt( owlFile, owl_alias, owl_subst, OWL_WKSYM_ALIAS );
    }
}

static  void    EmitImports( void )
/*********************************/
{
    void        *auto_import;
    char        *name;

    auto_import = NULL;
    for( ;; ) {
        auto_import = FEAuxInfo( auto_import, NEXT_IMPORT );
        if( auto_import == NULL )
            break;
        OWLEmitImport( owlFile, FEAuxInfo( auto_import, IMPORT_NAME ) );
    }
    auto_import = NULL;
    for( ;; ) {
        auto_import = FEAuxInfo( auto_import, NEXT_IMPORT_S );
        if( auto_import == NULL )
            break;
        DoOutObjectName( FEAuxInfo( auto_import, IMPORT_NAME_S ),
                         stringOut, &name, NORMAL );
        OWLEmitImport( owlFile, name );
    }
}

// FIXME - should likely be different for ELF under OS/2 et al
// and should match stuff in langenv (except we need runtime dependency on OS,
// not compile time).
static char *dependSectionName = ".depend";

static  void    EmitDependencyInfo( void )
/****************************************/
{
    owl_section_handle  sect;
    void                *depend;
    char                *name;
    DepInfo             info;

    sect = NULL;
    depend = NULL;
    for( ;; ) {
        depend = FEAuxInfo( depend, NEXT_DEPENDENCY );
        if( depend == NULL ) break;
        if( sect == NULL ) {
            sect = OWLSectionInit( owlFile, dependSectionName, OWL_SECTION_INFO, 16 );
        }
        name = (char *)FEAuxInfo( depend, DEPENDENCY_NAME );
        info.time = *(time_t *)FEAuxInfo( depend, DEPENDENCY_TIMESTAMP );
        info.len = strlen( name ) + 1;
        OWLEmitData( sect, (char *)&info, sizeof( DepInfo ) - 1 );
        OWLEmitData( sect, (char *)name, strlen( name ) + 1 );
    }
    /* put out a handy little sentinel value at the end */
    if( sect != NULL ) {
        info.len = 0;
        OWLEmitData( sect, (char *)&info, sizeof( DepInfo ) );
    }
}


static void DoDFSegRange( void )
/******************************/
{
    unsigned            bucket;
    section_def         *ptr;
    section_def         *old;
    owl_section_type    tipe;

    old = currSection;
    for( bucket = 0; bucket < N_SECTIONS; bucket++ ) {
        if( sectionDefs[ bucket ] != NULL ) {
            for( ptr = sectionDefs[ bucket ]; ptr != NULL; ptr = ptr->next ) {

                tipe = OWLTellSectionType( ptr->owl_handle );
                switch( tipe ){
                case OWL_SECTION_INFO:
                case OWL_SECTION_DEBUG:
                case OWL_SECTION_PDATA:
                case OWL_SECTION_COMDAT_DEBUG:
                    break;
                case OWL_SECTION_COMDAT_PDATA:
                case OWL_SECTION_CODE:
                case OWL_SECTION_DATA:
                case OWL_SECTION_BSS:
                case OWL_SECTION_COMDAT_CODE:
                case OWL_SECTION_COMDAT_DATA:
                // took this out - can't drop a 2nd static label in a comdat bss
                // section - there can be only one!
                // case OWL_SECTION_COMDAT_BSS:
                    currSection = ptr;
                    DFSegRange();
                    break;
                }
            }
        }
    }
    currSection = old;
}

extern  void    ObjFini( void )
/*****************************/
{
    offset          code_size;
    section_def     *curr;

    curr = FindSection( codeSection );
    code_size = OWLTellSize( curr->owl_handle  );

    if( _IsModel( DBG_DF ) ) {
        if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
            DoDFSegRange();
            DFObjFiniDbgInfo( code_size );
#if 0 // save for jimr
        } else if( _IsModel( NUMBERS ) ) {
            DFObjLineFiniDbgInfo();
#endif
        }
    } else if( _IsModel( DBG_CV ) ) {
        CVObjFiniDbgInfo();
    }
    DefaultLibs();
    AliasNames();
    EmitImports();
    EmitDependencyInfo();
    OWLFileFini( owlFile );
    OWLFini( owlHandle );
    DeleteSections();
    CloseObj();
    FEMessage( MSG_CODE_SIZE, (pointer)code_size );
//    FEMessage( MSG_DATA_SIZE, (pointer)data_size );
}


static  int PutBytes( void *handle, const char *buffer, unsigned len )
/********************************************************************/
{
    handle = handle;
#ifndef NDEBUG
    // enable OWL logging
    if( handle == NULL ) {
        PutObjBytes( buffer, len );
    } else {
        write( OWL_TO_HANDLE( handle ), buffer, len );
    }
#else
    PutObjBytes( buffer, len );
#endif
    return( len );
}


#define MAX_OBJ_NAME    1024

static  char            objName[ MAX_OBJ_NAME ];

static  void            NameGatherer( char *name, void *data )
/************************************************************/
{
    CopyStr( name, (char *)data );
}

static  char            *LabelName( code_lbl *label )
/***************************************************/
{
    sym_handle          sym;
    char                *name;
    char                *buff;
    import_type         kind;
    fe_attr             attr;

    sym = AskForLblSym( label );
    if( AskIfRTLabel( label ) ) {
        name = AskRTName( (rt_class)(int)sym );
        if( _TstStatus( label, WEIRD_PPC_ALIAS ) ) {
            objName[ 0 ] = '.';
            objName[ 1 ] = '.';
            strcpy( &objName[ 2 ], name );
            name = &objName[ 0 ];
        }
        return( name );
    }
    if( sym != NULL ) {
        buff = &objName[ 0 ];
        attr = FEAttr( sym );
        kind = NORMAL;
        if( (attr & FE_INTERNAL) == 0 ) {
            if( attr & FE_DLLIMPORT ) {
                kind = DLLIMPORT;
            } else {
                if( _TstStatus( label, WEIRD_PPC_ALIAS ) ) {
                    objName[ 0 ] = '.';
                    objName[ 1 ] = '.';
                    buff = &objName[ 2 ];
                }
            }
            DoOutObjectName( sym, NameGatherer, buff, kind );
            return( objName );
        }
    }
    return( NULL );
}

static  owl_symbol_handle labelOwlSym( code_lbl *lbl )
/****************************************************/
{
    if( lbl->owl_symbol == NULL ) {
        lbl->owl_symbol = OWLSymbolInit( owlFile, LabelName( lbl ) );
    }
    return( lbl->owl_symbol );
}

extern  void    InitSegDefs( void )
/*********************************/
{
    // fixme - should use routines with some error checking
    owl_client_funcs    funcs = { PutBytes, NULL, NULL, CGAlloc, CGFree };
    owl_format          format;

    owlHandle = OWLInit( &funcs,
#if _TARGET & _TARG_AXP
        OWL_CPU_ALPHA
#elif _TARGET & _TARG_PPC
        OWL_CPU_PPC
#elif _TARGET & _TARG_MIPS
        OWL_CPU_MIPS
#else
    #error Unknown RISC target
#endif
        );

    if( _IsModel( OBJ_ELF ) ) {
        format = OWL_FORMAT_ELF;
    } else {
        format = OWL_FORMAT_COFF;
    }

    owlFile = OWLFileInit( owlHandle, FEAuxInfo( NULL, SOURCE_NAME ), NULL, format, OWL_FILE_OBJECT );
    if( _IsTargetModel( OWL_LOGGING ) ) {
        OWLLogEnable( owlFile, HANDLE_TO_OWL( STDOUT_FILENO ) );
    }

    codeSection = BACKSEGS;
    dataSection = BACKSEGS;
    backSectIdx = BACKSEGS;
    currSection = NULL;
    owlTocSect = NULL;
    globalPdata = NULL;
}


extern  void    DefSegment( seg_id id, seg_attr attr, char *str, uint align, bool use_16 )
/****************************************************************************************/
{
    section_def         *new;
    owl_section_type    type;

    align = align;
    use_16 = use_16;
    new = AddSection( id );
    if( attr & EXEC ) {
        type = OWL_SECTION_CODE;
        if( attr & COMDAT ) {
            type = OWL_SECTION_COMDAT_CODE;
        }
        if( codeSection == BACKSEGS ) {
            codeSection = id;
            if( _IsModel( DBG_DF ) ) {
                DFBegCCU( id, 0 );
            }
        }
    } else if( attr & INIT ) {
        type = OWL_SECTION_DATA;
        if( attr & COMDAT ) {
            type = OWL_SECTION_COMDAT_DATA;
        }
        if( attr & BACK ) {
            dataSection = id;
        }
    } else {
        type = OWL_SECTION_BSS;
        if( attr & ( COMDAT | COMMON ) ) {
            type = OWL_SECTION_COMDAT_BSS;
        }
    }
    new->owl_handle = OWLSectionInit( owlFile, str, type, 16 );
}

extern void    OutFileStart( int line )
/*************************************/
{
    cue_state           info;
    char                *fname;

    if( _IsModel( DBG_DF ) || _IsModel( DBG_CV ) ){
        CueFind( line, &info );
        line = info.line;
        if( info.fno != CurrFNo ){
            fname = SrcFNoFind( info.fno );
            CurrFNo = info.fno;
            OWLFileSymbol( owlFile, fname );
        }
    }
}
extern void    OutFuncStart( code_lbl *label, offset start, cg_linenum line )
/***************************************************************************/
{
    cue_state            info;

    if( _IsModel( DBG_DF ) || _IsModel( DBG_CV ) ){
        CueFind( line, &info );
        line = info.line;
        if( _IsModel( DBG_DF ) ){
            if( _IsModel( DBG_LOCALS | DBG_TYPES ) ){
                DFLineNum( &info, start );
            }
        }
    }
    currSection->func = OWLDebugFuncBegin( currSection->owl_handle,
                   labelOwlSym( label ), line, start );
    currSection->line = line;
    currSection->start = line;

}

extern void    OutFuncEnd( offset end )
/*************************************/
{
    OWLDebugFuncEnd( currSection->func, currSection->line, end );
    currSection->func = NULL;
}

extern  void    OutLineNum( cg_linenum line, bool label_line )
/************************************************************/
{
    cue_state            info;
    offset               lc;

    label_line = label_line;
    lc = OWLTellOffset( currSection->owl_handle );
    if( _IsModel( DBG_DF ) || _IsModel( DBG_CV ) ) {
        CueFind( line, &info );
        if( _IsModel( DBG_DF ) ) {
            if( _IsModel( DBG_LOCALS | DBG_TYPES ) ) {
                DFLineNum( &info, lc );
            }
        } else if( _IsModel( DBG_CV ) ) {
            char    *fname;

            if( info.fno != CurrFNo ) {
                fname = SrcFNoFind( info.fno );
                CurrFNo = info.fno;
                OWLDebugFuncFile( currSection->func, fname );
            }
        }
        line = info.line;
    }
    if( currSection->start != line ) {  // Else we get two func starts
        if( currSection->func != NULL ) // FIXME: added check to prevent crashes - MN
            OWLDebugFuncLine( currSection->func, line, lc );
        currSection->line = line;
    }
}


extern char GetMemModel( void )
/*****************************/
{
    char    model;

    if( _IsTargetModel( BIG_CODE ) ) {
        if( _IsTargetModel( BIG_DATA ) ) {
            if( _IsntTargetModel( CHEAP_POINTER ) ) {
                model = 'h';
            } else {
                model = 'l';
            }
        } else {
            model = 'm';
        }
    } else if( _IsTargetModel( BIG_DATA ) ) {
        model = 'c';
    } else {
        model = 'f';
    }
    return( model );
}

extern seg_id DbgSegDef( char *str )
/**********************************/
{
    section_def         *new;
    seg_id              id;

    id = --backSectIdx;
    new = AddSection( id );
    new->owl_handle = OWLSectionInit( owlFile, str, OWL_SECTION_DEBUG, 1 );
    return( id );
}

extern  bool    HaveCodeSeg( void )
/*********************************/
{
    return( codeSection != BACKSEGS );
}

extern  seg_id  AskCodeSeg( void )
/********************************/
{
     return( codeSection );
}

extern  seg_id  AskAltCodeSeg( void )
/***********************************/
{
    return( codeSection );
}

extern  seg_id  AskBackSeg( void )
/********************************/
{
     return( dataSection );
}


extern  seg_id  AskOP( void )
/***************************/
{
    assert( currSection != NULL );
     return( currSection->id );
}

static  bool            InlineFunction( pointer hdl )
/***************************************************/
{
    aux_handle          aux;

    if( (FEAttr( hdl ) & FE_PROC) == 0 )
        return( FALSE );
    aux = FEAuxInfo( hdl, AUX_LOOKUP );
    if( FEAuxInfo( aux, CALL_BYTES ) != NULL )
        return( TRUE );
    return( (*(call_class *)FEAuxInfo( aux, CALL_CLASS ) & MAKE_CALL_INLINE) != 0 );
}

extern  seg_id      AskSegID( pointer hdl, cg_class class )
/*********************************************************/
{
    switch( class ) {
    case CG_FE:
        if( InlineFunction( hdl ) ) {
            return( AskCodeSeg() );
        }
        return( FESegID( hdl ) );
    case CG_BACK:
        return( ((bck_info*)hdl)->seg );
    case CG_TBL:
    case CG_VTB:
        return( AskCodeSeg() );
    case CG_CLB:
        return( AskAltCodeSeg() );
    default:
        return( AskBackSeg() );
    }
}

extern  void    ObjBytes( char *buffer, unsigned size )
/*****************************************************/
{
    assert( currSection != NULL );
    OWLEmitData( currSection->owl_handle, buffer, size );
}

extern  bool    AskSegBlank( seg_id id )
/**************************************/
{
    section_def         *sect;
    owl_section_type    tipe;

    sect = FindSection( id );
    tipe = OWLTellSectionType( sect->owl_handle );
    return( tipe == OWL_SECTION_BSS || tipe == OWL_SECTION_COMDAT_BSS );
}

#define MAX_ALIGN       128

extern  void    AlignObject( unsigned align )
/*******************************************/
{
    offset      off;
    unsigned    mod;
    unsigned    add;
    char        buffer[ MAX_ALIGN ];

    assert( currSection != NULL );
    assert( _IsPowerOfTwo( align ) );
    assert( align != 0 );
    assert( align <= MAX_ALIGN );

    off = OWLTellOffset( currSection->owl_handle );
    mod = off & ( align - 1 );
    if( mod ) {
        add = align - mod;
        if( AskSegBlank( currSection->id ) ) {
            OWLEmitData( currSection->owl_handle, NULL, add );
        } else {
            memset( buffer, 0, add );
            OWLEmitData( currSection->owl_handle, buffer, add );
        }
    }

}


extern  seg_id  SetOP( seg_id seg )
/*********************************/
{
    seg_id      old;
    section_def *new;


    if( currSection == NULL ) {
        old = (seg_id)-1;
    } else {
        old = currSection->id;
    }
    if( seg == (seg_id)-1 ) {
        currSection = NULL;
    } else {
        new = FindSection( seg );
        currSection = new;
    }
    return( old );
}


extern  void    FlushOP( seg_id id )
/**********************************/
{
    section_def         *sect;
    seg_id              old;
    owl_section_type    tipe;

    sect = FindSection( id );
    if( _IsModel( DBG_DF ) ) {
        tipe = OWLTellSectionType( sect->owl_handle );
        switch( tipe ) {
        case OWL_SECTION_INFO:
        case OWL_SECTION_DEBUG:
        case OWL_SECTION_PDATA:
        case OWL_SECTION_COMDAT_DEBUG:
            break;
        case OWL_SECTION_COMDAT_PDATA:
        case OWL_SECTION_CODE:
        case OWL_SECTION_DATA:
        case OWL_SECTION_BSS:
        case OWL_SECTION_COMDAT_CODE:
        case OWL_SECTION_COMDAT_DATA:
        case OWL_SECTION_COMDAT_BSS:
            old = SetOP( id );
            DFSegRange();
            SetOP( old );
            break;
        }
    }
    OWLSectionFini( sect->owl_handle );
}

extern  bool    NeedBaseSet( void )
/*********************************/
{
    bool        need;

    if( currSection->is_start ) {
        need = TRUE;
        currSection->is_start = FALSE;
    } else {
        need = FALSE;
    }
    return( need );
}

extern  offset  AskLocation( void )
/*********************************/
{
    assert( currSection != NULL );
    return( OWLTellOffset( currSection->owl_handle ) );
}


extern  long_offset  AskBigLocation( void )
/*****************************************/
{
    assert( currSection != NULL );
    return( OWLTellOffset( currSection->owl_handle ) );
}

extern  offset  AskMaxSize( void )
/********************************/
{
    assert( currSection != NULL );
    return( OWLTellSize( currSection->owl_handle ) );
}

extern  long_offset  AskBigMaxSize( void )
/****************************************/
{
    assert( currSection != NULL );
    return( OWLTellSize( currSection->owl_handle ) );
}

extern  void    SetLocation( offset loc )
/***************************************/
{
    OWLSetLocation( currSection->owl_handle, loc );
}

extern  void    SetBigLocation( long_offset loc )
/***********************************************/
{
    OWLSetLocation( currSection->owl_handle, loc );
}

static void DumpImportResolve( code_lbl *label )
/**********************************************/
{
    sym_handle          def_resolve;
    sym_handle          sym;
    pointer             cond;
    int                 type;
    bck_info            *bck;

    if( AskIfRTLabel( label ) ) return;
    sym = AskForLblSym( label );
    if( sym != NULL ){
        def_resolve = FEAuxInfo( sym, DEFAULT_IMPORT_RESOLVE );
        if( def_resolve != NULL && def_resolve != sym ) {
            bck =  FEBack( def_resolve);
            type = (int) FEAuxInfo( sym, IMPORT_TYPE );
            switch( type ) {
            case IMPORT_IS_LAZY:
                OWLWeakExt( owlFile, labelOwlSym( label ), labelOwlSym( bck->lbl ), OWL_WKSYM_LAZY );
                break;
            case IMPORT_IS_WEAK:
                OWLWeakExt( owlFile, labelOwlSym( label ), labelOwlSym( bck->lbl ), OWL_WKSYM_NORMAL );
                break;
            case IMPORT_IS_CONDITIONAL_PURE:
                /* fall through */
            case IMPORT_IS_CONDITIONAL:
                cond = FEAuxInfo( sym, CONDITIONAL_IMPORT );
                while( cond != NULL ) {
                    sym = FEAuxInfo( cond, CONDITIONAL_SYMBOL );
                    cond = FEAuxInfo( cond, NEXT_CONDITIONAL );
                }
                assert( 0 ); // not implemented
                break;
            }
        }
    }
}

extern  void    OutReloc( code_lbl *label, owl_reloc_type tipe, unsigned offset )
/*******************************************************************************/
{
    DumpImportResolve( label );
    offset = offset;
    OWLEmitReloc( currSection->owl_handle,
        OWLTellOffset( currSection->owl_handle ),
        labelOwlSym( label ), tipe );
}

extern  void    OutSegReloc( code_lbl *label, seg_id seg )
/********************************************************/
{
    section_def             *sect;

    label = label;
    sect = FindSection( seg );
    OWLEmitMetaReloc( currSection->owl_handle,
        OWLTellOffset( currSection->owl_handle ),
        sect->owl_handle, OWL_RELOC_SECTION_INDEX );
}

extern  owl_sym_linkage labelLinkage( label_handle label )
/********************************************************/
{
    sym_handle          sym;
    owl_sym_linkage     linkage;
    fe_attr             attr;

    linkage = OWL_SYM_STATIC;
    sym = AskForLblSym( label );
    if( sym != NULL ) {
        attr = FEAttr( sym );
        if( attr & FE_GLOBAL ) {
            linkage = OWL_SYM_GLOBAL;
        }
    }
    return( linkage );
}

extern  void    OutLabel( label_handle label )
/********************************************/
{
    sym_handle          sym;
    fe_attr             attr;
    owl_sym_type        tipe;

    assert( currSection != NULL );
    tipe = OWL_TYPE_OBJECT;
    sym = AskForLblSym( label );
    if( sym != NULL ) {
        attr = FEAttr( sym );
        if( attr & FE_PROC ) {
            label = GetWeirdPPCDotDotLabel( (code_lbl *)label );
            tipe = OWL_TYPE_FUNCTION;
        }
    }
    OWLEmitLabel( currSection->owl_handle, labelOwlSym( label ), tipe, labelLinkage( label ) );
    TellAddress( label, OWLTellLocation( currSection->owl_handle ) );
    if( sym != NULL ) {
        if( SymIsExported( sym ) ) {
            OWLEmitExport( owlFile, labelOwlSym( label ) );
        }
    }
}

static long const Zero = 0;

#if _TARGET & _TARG_PPC
extern void OutTOCRec( code_lbl *label )
/**************************************/
{
    code_lbl            *dot_lbl;
    code_lbl            *toc_lbl;

    if( owlTocSect == NULL ) {
        owlTocSect = OWLSectionInit( owlFile, ".reldata", OWL_SEC_ATTR_DATA|OWL_SEC_ATTR_PERM_READ|OWL_SEC_ATTR_PERM_WRITE, 8 );
    }
    dot_lbl = GetWeirdPPCDotDotLabel( label );
    toc_lbl = RTLabel( RT_TOC_NAME );
    OWLEmitLabel( owlTocSect, labelOwlSym( label ), OWL_TYPE_OBJECT, labelLinkage( label ) );
    OWLEmitReloc( owlTocSect, OWLTellOffset( owlTocSect ), labelOwlSym( dot_lbl ), OWL_RELOC_WORD );
    OWLEmitData( owlTocSect, (char *)&Zero, 4 );
    OWLEmitReloc( owlTocSect, OWLTellOffset( owlTocSect ), labelOwlSym( toc_lbl ), OWL_RELOC_WORD );
    OWLEmitData( owlTocSect, (char *)&Zero, 4 );
}
#endif

static owl_section_handle getPData( code_lbl *label )
/***************************************************/
{
    sym_handle          sym;
    owl_section_handle  pdata;

    sym = AskForLblSym( label );
    if( sym != NULL ) {
        if( FEAttr( sym ) & FE_COMMON ) {
            pdata = OWLSectionInit( owlFile, ".pdata", OWL_SECTION_COMDAT_PDATA, 4 );
            OWLComdatDep( pdata, currSection->owl_handle );
            return( pdata );
        }
    }
    if( globalPdata == NULL ) {
        globalPdata = OWLSectionInit( owlFile, ".pdata", OWL_SECTION_PDATA, 4 );
    }
    return( globalPdata );
}

extern void OutPDataRec( code_lbl *label, offset proc_size, offset pro_size )
/***************************************************************************/
{
    owl_section_handle  owl_pdata;
    sym_handle          sym;
    sym_handle          curr;
    code_lbl            *lbl;

    owl_pdata = getPData( label );
    label = GetWeirdPPCDotDotLabel( label );
    sym = AskForLblSym( label );
    OWLEmitReloc( owl_pdata, OWLTellOffset( owl_pdata ), labelOwlSym( label ), OWL_RELOC_WORD );
    OWLEmitData( owl_pdata, (char *)&Zero, 4 );
    OWLEmitReloc( owl_pdata,OWLTellOffset( owl_pdata ), labelOwlSym( label ), OWL_RELOC_WORD );
    OWLEmitData( owl_pdata, (char *)&proc_size, 4 );
    if( sym != NULL ) { // put  out exception handler stuff
        curr = FEAuxInfo( sym, EXCEPTION_HANDLER );
        if( curr != NULL ) {
            lbl =  AskForSymLabel( curr, CG_FE );
            OWLEmitReloc( owl_pdata, OWLTellOffset( owl_pdata ), labelOwlSym( lbl ), OWL_RELOC_WORD );
        } else if( _IsTargetModel( EXCEPT_FILTER_USED ) ) {
            lbl = RTLabel( RT_EXCEPT_RTN );
            OWLEmitReloc( owl_pdata, OWLTellOffset( owl_pdata ), labelOwlSym( lbl ), OWL_RELOC_WORD );
        }
        OWLEmitData( owl_pdata, (char *)&Zero, 4 );
        curr = FEAuxInfo( sym, EXCEPTION_DATA );
        if( curr != NULL ) {
            lbl =  AskForSymLabel( curr, CG_FE );
            OWLEmitReloc( owl_pdata, OWLTellOffset( owl_pdata ), labelOwlSym( lbl ), OWL_RELOC_WORD );
        }
        OWLEmitData( owl_pdata, (char *)&Zero, 4 );
    } else {
        OWLEmitData( owl_pdata, (char *)&Zero, 4 );
        OWLEmitData( owl_pdata, (char *)&Zero, 4 );
    }
    OWLEmitReloc( owl_pdata,OWLTellOffset( owl_pdata ), labelOwlSym( label ), OWL_RELOC_WORD );
    OWLEmitData( owl_pdata, (char *)&pro_size, 4 );
}


extern  void    *InitPatch( void )
/********************************/
{
    return( NULL );
}

extern  void    AbsPatch( void * patch, offset lc )
/*************************************************/
{
    patch = patch;
    lc = lc;
}

extern  void    DoEmptyQueue( void )
/**********************************/
{
    EmptyQueue();
    TellUnreachLabels();
}

extern  void    TellObjNewProc( sym_handle proc )
/***********************************************/
{
    segment_id  proc_id;
    segment_id  old;

    old = SetOP( codeSection );
    proc_id = FESegID( proc );
    if( codeSection != proc_id ) {
        DoEmptyQueue();
        codeSection = proc_id;
        SetOP( codeSection );
        currSection->is_start = TRUE;
    }
    if( FEAttr( proc ) & FE_COMMON ) {
        if( _IsModel( DBG_CV ) ) { // set the $debug for comdat
            CVDefSymComdat( currSection->owl_handle );
        }
    } else {
        if( _IsModel( DBG_CV ) ) {
            CVDefSymNormal();  // reset to normal $debug section
        }
    }
    SetOP( old );
}

extern  void    IncLocation( offset by )
/**************************************/
{
    /* This should only be used for bumping up our location in a BSS section */
    OWLEmitData( currSection->owl_handle, NULL, by );
}

extern  bool    AskNameROM( name *n )
/***********************************/
{
    n = n;
    return( FALSE );
}


extern  unsigned DepthAlign( unsigned depth )
/*******************************************/
{
    depth = depth;
    return( 4 );
}

extern  bool    CodeHasAbsPatch( oc_entry *code )
/***********************************************/
{
    code = code;
    return( FALSE );    // NYI
}

static  bool    relocBefore( void *_p1, void *_p2 )
/*************************************************/
{
    byte_seq_reloc *p1 = _p1;
    byte_seq_reloc *p2 = _p2;

    if( p1->off == p2->off ) {
        /*
         * Only thing which can have multiple relocs to same address
         * should be an OWL_RELOC_HI and OWL_RELOC_PAIR sequence and we
         * want to make sure the HI comes first.
         */
        return( p1->type < p2->type );
    }
    return( p1->off < p2->off );
}

extern  void    ObjEmitSeq( byte_seq *code )
/******************************************/
{
    byte_seq_reloc      *curr;
    bck_info            *back;
    type_length         loc;
    byte_seq_len        i;
    axp_ins             *code_ptr;
    axp_ins             opcode;
    pointer             reloc_sym;
    owl_reloc_type      reloc_type;

    assert( code->length % 4 == 0 );
    curr = SortList( code->relocs, offsetof( byte_seq_reloc, next ), relocBefore );
    code_ptr = (axp_ins *)&code->data[ 0 ];
    for( i = 0; i < code->length; i += 4 ) {
        opcode = *code_ptr++;
        reloc_type = 0;
        reloc_sym = NULL;
        while( curr != NULL && curr->off == i ) {
            back = (bck_info *)SymBack( curr->sym );
            switch( curr->type ) {
            case OWL_RELOC_FP_OFFSET:
                loc = TempLocation( (name *)back );
                if( loc > 32767 ) {
                    FEMessage( MSG_ERROR, "auto variable out of range for reference within inline assembly sequence" );
                }
                opcode |= _SignedImmed( loc );
                break;
            case OWL_RELOC_PAIR:
                break;
            default:
                reloc_type = curr->type;
                reloc_sym = back->lbl;
            }
            curr = curr->next;
        }
        EmitInsReloc( opcode, reloc_sym, reloc_type );
    }
}

extern  void    DoAlignment( int align )
/**************************************/
{
    // NYI
    align = align;
}
