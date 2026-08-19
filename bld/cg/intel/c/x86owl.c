/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Minimal OWL object emitter for the 32-bit Intel backend.
*
****************************************************************************/

#include "_cgstd.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "coderep.h"
#include "data.h"
#include "cgauxcc.h"
#include "cgauxinf.h"
#include "cgmem.h"
#include "utils.h"
#include "cgswitch.h"
#include "model.h"
#include "owl.h"
#include "objio.h"
#include "onexit.h"
#include "opttell.h"
#include "x86objd.h"
#include "objout.h"
#include "x86obj.h"
#include "x86emit.h"
#include "cgsegids.h"
#include "feprotos.h"
#include "depinfo.h"

#if _TARGET & _TARG_80386

#define COMMENTV( a )       a, sizeof( a ) - 1
#define COFF_DRECTVE_DEFLIB "-defaultlib:"

typedef struct owl_x86_section {
    struct owl_x86_section *next;
    segment_id              segid;
    seg_attr                attr;
    owl_section_handle      handle;
    owl_symbol_handle       base_symbol;
} owl_x86_section;

typedef struct owl_x86_patch {
    owl_section_handle      section;
    owl_offset              where;
    patch_attr              attr;
} owl_x86_patch;

typedef struct owl_x86_abs_patch {
    struct owl_x86_abs_patch *next;
    owl_x86_patch             patch;
    long_offset               value;
    bool                      have_value;
    bool                      have_offset;
} owl_x86_abs_patch;

static owl_handle           OwlHandle;
static owl_file_handle      OwlFile;
static owl_x86_section      *Sections;
static owl_x86_section      *CurrSection;
static owl_x86_abs_patch    *AbsPatches;
static segment_id           CodeSectionId;
static segment_id           DataSectionId;
static segment_id           BackSectionId;
static unsigned             PendingLine;
static char                 ObjName[1024];

static void owlUnsupported( const char *what )
{
    FatalError( what );
}

static int putBytes( owl_client_file file, const char *buffer, size_t len )
{
    /* unused parameters */ (void)file;
    PutObjBytes( buffer, len );
    return( 0 );
}

static owl_x86_section *findSection( segment_id segid )
{
    owl_x86_section *section;

    for( section = Sections; section != NULL; section = section->next ) {
        if( section->segid == segid )
            return( section );
    }
    return( NULL );
}

static void gatherName( const char *name, char *buffer )
{
    strcpy( buffer, name );
}

static const char *symbolName( cg_sym_handle sym )
{
    import_kind kind;
    fe_attr     attr;

    attr = FEAttr( sym );
    kind = NORMAL;
    if( attr & FE_DLLIMPORT )
        kind = DLLIMPORT;
    OUTPUT_OBJECT_NAME( sym, gatherName, ObjName, kind );
    return( ObjName );
}

static const char *labelName( label_handle label )
{
    cg_sym_handle sym;

    sym = AskForLblSym( label );
    if( AskIfRTLabel( label ) )
        return( AskRTName( SYM2RTIDX( sym ) ) );
    if( sym != NULL )
        return( symbolName( sym ) );
    return( NULL );
}

static owl_symbol_handle labelSymbol( label_handle label )
{
    const char *name;
    char        generated[32];

    if( label->owl_symbol == NULL ) {
        name = labelName( label );
        if( name == NULL ) {
            sprintf( generated, "$L%p", label );
            name = generated;
        }
        label->owl_symbol = OWLSymbolInit( OwlFile, name );
    }
    return( label->owl_symbol );
}

static owl_symbol_handle symbolHandle( cg_sym_handle sym )
{
    return( labelSymbol( FEBack( sym )->lbl ) );
}

static array_control *newPatchArray( void )
{
    array_control *array;

    array = CGAlloc( sizeof( *array ) );
    array->array = NULL;
    array->alloc = 0;
    array->used = 0;
    array->entry = sizeof( owl_x86_patch );
    array->inc = 8;
    return( array );
}

static owl_x86_patch *appendPatch( array_control *array )
{
    owl_x86_patch *patch;
    void          *old;
    unsigned      size;

    if( array->used == array->alloc ) {
        array->alloc += array->inc;
        size = array->alloc * array->entry;
        if( array->array == NULL ) {
            array->array = CGAlloc( size );
        } else {
            old = array->array;
            array->array = CGAlloc( size );
            memcpy( array->array, old, array->used * array->entry );
            CGFree( old );
        }
    }
    patch = (owl_x86_patch *)array->array + array->used;
    ++array->used;
    return( patch );
}

static void deletePatchArray( array_control *array )
{
    if( array->array != NULL )
        CGFree( array->array );
    CGFree( array );
}

static void applyPatch( const owl_x86_patch *patch, offset value )
{
    owl_offset  save;
    uint_32     dword;
    uint_16     word;
    byte        one;

    save = OWLTellLocation( patch->section );
    OWLSetLocation( patch->section, patch->where );
    if( patch->attr & LONG_PATCH ) {
        if( patch->attr & ADD_PATCH ) {
            OWLTellData( patch->section, patch->where, (char *)&dword, sizeof( dword ) );
            dword += value;
        } else {
            dword = value;
        }
        OWLEmitData( patch->section, (char *)&dword, sizeof( dword ) );
    } else if( patch->attr & WORD_PATCH ) {
        if( patch->attr & ADD_PATCH ) {
            OWLTellData( patch->section, patch->where, (char *)&word, sizeof( word ) );
            word += value;
        } else {
            word = value;
        }
        OWLEmitData( patch->section, (char *)&word, sizeof( word ) );
    } else {
        if( patch->attr & ADD_PATCH ) {
            OWLTellData( patch->section, patch->where, (char *)&one, sizeof( one ) );
            one += value;
        } else {
            one = value;
        }
        OWLEmitData( patch->section, (char *)&one, sizeof( one ) );
    }
    OWLSetLocation( patch->section, save );
}

static void freeAbsPatch( owl_x86_abs_patch *patch )
{
    owl_x86_abs_patch **owner;

    for( owner = &AbsPatches; *owner != patch; owner = &(*owner)->next ) {
    }
    *owner = patch->next;
    CGFree( patch );
}

static owl_reloc_type relocType( fix_class class, bool relative )
{
    if( class & (F_TLS | F_FAR16) ) {
        owlUnsupported( "OWL x86 MVP does not support TLS or far16 relocations" );
    }
    switch( F_CLASS( class ) ) {
    case F_OFFSET:
    case F_BIG_OFFSET:
        return( relative ? OWL_RELOC_BRANCH_REL : OWL_RELOC_WORD );
    default:
        owlUnsupported( "OWL x86 MVP supports only flat 32-bit offset relocations" );
        return( OWL_RELOC_ABSOLUTE );
    }
}

static bool inlineFunction( cg_sym_handle sym )
{
    if( FEAttr( sym ) & FE_PROC ) {
        if( FindAuxInfoSym( sym, FEINF_CALL_BYTES ) != NULL
          || ((call_class)(pointer_uint)FindAuxInfoSym( sym, FEINF_CALL_CLASS ) & FECALL_GEN_MAKE_CALL_INLINE) ) {
            return( true );
        }
    }
    return( false );
}

static void owlInitSegDefs( void )
{
    owl_client_funcs funcs = { putBytes, NULL, NULL, CGAlloc, CGFree };
    owl_format       format;

    format = _IsModel( CGSW_GEN_OBJ_ELF ) ? OWL_FORMAT_ELF : OWL_FORMAT_COFF;
    OwlHandle = OWLInit( &funcs, OWL_CPU_X86 );
    OwlFile = OWLFileInit( OwlHandle, FEAuxInfo( NULL, FEINF_SOURCE_NAME ), NULL, format, OWL_FILE_OBJECT );
    Sections = NULL;
    CurrSection = NULL;
    AbsPatches = NULL;
    CodeSectionId = BACKSEGS;
    DataSectionId = BACKSEGS;
    BackSectionId = BACKSEGS;
    PendingLine = 0;
}

static bool owlFreeObjCache( void ) { return( false ); }
static segment_id owlAskOP( void ) { return( CurrSection == NULL ? UNDEFSEG : CurrSection->segid ); }

static void owlSetOP( segment_id segid )
{
    if( segid == UNDEFSEG ) {
        CurrSection = NULL;
    } else {
        CurrSection = findSection( segid );
        if( CurrSection == NULL )
            owlUnsupported( "OWL x86 selected an undefined section" );
    }
}

static segment_id owlChangeOP( segment_id segid )
{
    segment_id old;

    old = owlAskOP();
    owlSetOP( segid );
    return( old );
}

static void owlDefSegment( segment_id segid, seg_attr attr, const char *name, uint align, bool use_16 )
{
    owl_x86_section *section;
    owl_section_type type;
    char             base_name[32];

    if( use_16 )
        owlUnsupported( "OWL x86 MVP does not support 16-bit sections" );
    section = CGAlloc( sizeof( *section ) );
    section->next = Sections;
    Sections = section;
    section->segid = segid;
    section->attr = attr;
    if( attr & EXEC ) {
        type = (attr & COMDAT) ? OWL_SECTION_COMDAT_CODE : OWL_SECTION_CODE;
        if( CodeSectionId == BACKSEGS )
            CodeSectionId = segid;
    } else if( attr & INIT ) {
        type = (attr & COMDAT) ? OWL_SECTION_COMDAT_DATA : OWL_SECTION_DATA;
        if( attr & BACK )
            DataSectionId = segid;
    } else {
        type = (attr & (COMDAT | COMMON)) ? OWL_SECTION_COMDAT_BSS : OWL_SECTION_BSS;
    }
    section->handle = OWLSectionInit( OwlFile, name, type, align == 0 ? 1 : align );
    sprintf( base_name, "$S%d", segid );
    section->base_symbol = OWLSymbolInit( OwlFile, base_name );
    /* OWL creates its own private OWL_TYPE_SECTION symbol.  A normal local
     * symbol gives the x86 emitter a public-API relocation target for offset
     * zero without corrupting OWL's section-symbol bookkeeping. */
    OWLEmitLabel( section->handle, section->base_symbol, OWL_TYPE_OBJECT, OWL_SYM_STATIC );
}

static void owlEmptyQueue( void ) {}
static char owlGetMemModel( void ) { return( 'f' ); }

static segment_id owlDbgSegDef( const char *name, const char *class_name, int modifier )
{
    segment_id segid;

    /* unused parameters */ (void)class_name; (void)modifier;
    segid = --BackSectionId;
    owlDefSegment( segid, INIT | GIVEN_NAME, name, 1, false );
    return( segid );
}

static void owlObjInit( void )
{
    if( _IsModel( CGSW_GEN_DBG_TYPES | CGSW_GEN_DBG_LOCALS | CGSW_GEN_DBG_NUMBERS ) )
        owlUnsupported( "debug information is not implemented by the OWL x86 MVP" );
    OpenObj();
}

static offset owlAskLocation( void ) { return( OWLTellLocation( CurrSection->handle ) ); }
static void owlChkDbgSegSize( offset max, bool typing ) { (void)max; (void)typing; }

static bool owlUseImportForm( fe_attr attr )
{
    if( attr & (FE_GLOBAL | FE_IMPORT) )
        return( true );
    return( (attr & FE_INTERNAL) == 0 );
}

static bool owlSegIsNear( segment_id segid ) { return( findSection( segid ) != NULL ); }
static bool owlSegIsBlank( segment_id segid )
{
    owl_section_type type = OWLTellSectionType( findSection( segid )->handle );
    return( type == OWL_SECTION_BSS || type == OWL_SECTION_COMDAT_BSS );
}
static bool owlSegIsPrivate( segment_id segid )
{
    owl_x86_section *section = findSection( segid );
    return( section == NULL || (section->attr & (PRIVATE | EXEC)) != 0 );
}
static bool owlSegIsROM( segment_id segid )
{
    owl_x86_section *section = findSection( segid );
    return( section != NULL && (section->attr & ROM) != 0 );
}
static segment_id owlAskBackSeg( void ) { return( DataSectionId ); }
static segment_id owlAskCodeSeg( void ) { return( CodeSectionId ); }
static bool owlHaveCodeSeg( void ) { return( CodeSectionId != BACKSEGS ); }
static segment_id owlAskAltCodeSeg( void ) { return( CodeSectionId ); }
static segment_id owlAskCode16Seg( void ) { owlUnsupported( "OWL x86 has no 16-bit code section" ); return( UNDEFSEG ); }
static void owlOutSelect( bool starts ) { (void)starts; }
static void owlSetUpObj( bool is_data ) { (void)is_data; }
static void owlFlushOP( segment_id segid ) { (void)segid; }

static void owlDefaultLibs( void )
{
    owl_section_handle comments;
    char               *lib;
    char               *name;

    comments = NULL;
    lib = NULL;
    for( ;; ) {
        lib = FEAuxInfo( lib, FEINF_NEXT_LIBRARY );
        if( lib == NULL )
            break;
        name = (char *)FEAuxInfo( lib, FEINF_LIBRARY_NAME );
        if( name == NULL || *name == '\0' )
            continue;
        if( comments == NULL )
            comments = OWLSectionInit( OwlFile, ".drectve", OWL_SECTION_INFO, 1 );
        OWLEmitData( comments, COMMENTV( COFF_DRECTVE_DEFLIB ) );
        OWLEmitData( comments, name, strlen( name ) );
        OWLEmitData( comments, " ", 1 );
    }
    if( comments != NULL )
        OWLEmitData( comments, "", 1 );
}

static void stringOut( const char *name, const char **data )
{
    *data = name;
}

static void owlAliasNames( void )
{
    pointer             alias;
    const char          *alias_name;
    const char          *subst_name;
    owl_symbol_handle   owl_alias;
    owl_symbol_handle   owl_subst;

    alias = NULL;
    for( ;; ) {
        alias = FEAuxInfo( alias, FEINF_NEXT_ALIAS );
        if( alias == NULL )
            break;
        alias_name = FEAuxInfo( alias, FEINF_ALIAS_NAME );
        if( alias_name == NULL ) {
            OUTPUT_OBJECT_NAME( FEAuxInfo( alias, FEINF_ALIAS_SYMBOL ),
                                stringOut, &alias_name, NORMAL );
        }
        subst_name = FEAuxInfo( alias, FEINF_ALIAS_SUBST_NAME );
        if( subst_name == NULL ) {
            OUTPUT_OBJECT_NAME( FEAuxInfo( alias, FEINF_ALIAS_SUBST_SYMBOL ),
                                stringOut, &subst_name, NORMAL );
        }
        owl_alias = OWLSymbolInit( OwlFile, alias_name );
        owl_subst = OWLSymbolInit( OwlFile, subst_name );
        OWLWeakExt( OwlFile, owl_alias, owl_subst, OWL_WKSYM_ALIAS );
    }
}

static void owlEmitImports( void )
{
    void        *auto_import;
    const char  *name;

    auto_import = NULL;
    for( ;; ) {
        auto_import = FEAuxInfo( auto_import, FEINF_NEXT_IMPORT );
        if( auto_import == NULL )
            break;
        OWLEmitImport( OwlFile, FEAuxInfo( auto_import, FEINF_IMPORT_NAME ) );
    }
    auto_import = NULL;
    for( ;; ) {
        auto_import = FEAuxInfo( auto_import, FEINF_NEXT_IMPORT_S );
        if( auto_import == NULL )
            break;
        OUTPUT_OBJECT_NAME( FEAuxInfo( auto_import, FEINF_IMPORT_NAME_S ),
                            stringOut, &name, NORMAL );
        OWLEmitImport( OwlFile, name );
    }
}

static void owlEmitDependencyInfo( void )
{
    owl_section_handle section;
    void               *depend;
    char               *name;
    DepInfo            info;

    section = NULL;
    depend = NULL;
    for( ;; ) {
        depend = FEAuxInfo( depend, FEINF_NEXT_DEPENDENCY );
        if( depend == NULL )
            break;
        if( section == NULL )
            section = OWLSectionInit( OwlFile, ".depend", OWL_SECTION_INFO, 16 );
        name = (char *)FEAuxInfo( depend, FEINF_DEPENDENCY_NAME );
        info.time = *(time_t *)FEAuxInfo( depend, FEINF_DEPENDENCY_TIMESTAMP );
        info.len = strlen( name ) + 1;
        OWLEmitData( section, (char *)&info, offsetof( DepInfo, name ) );
        OWLEmitData( section, name, info.len );
    }
    if( section != NULL ) {
        info.time = 0;
        info.len = 0;
        OWLEmitData( section, (char *)&info, offsetof( DepInfo, name ) );
    }
}

static void owlObjFini( void )
{
    owl_x86_section *section;
    owl_x86_section *next;
    owl_x86_abs_patch *patch;

    while( (patch = AbsPatches) != NULL ) {
        AbsPatches = patch->next;
        applyPatch( &patch->patch, patch->value );
        CGFree( patch );
    }
    owlDefaultLibs();
    owlAliasNames();
    owlEmitImports();
    owlEmitDependencyInfo();
    OWLFileFini( OwlFile );
    OWLFini( OwlHandle );
    for( section = Sections; section != NULL; section = next ) {
        next = section->next;
        CGFree( section );
    }
    CloseObj();
}

static bool owlSymIsExported( cg_sym_handle sym )
{
    return( sym != NULL && ((FEAttr( sym ) & FE_DLLEXPORT) != 0
        || ((call_class)(pointer_uint)FindAuxInfoSym( sym, FEINF_CALL_CLASS ) & FECALL_GEN_DLL_EXPORT) != 0) );
}

static void owlOutDLLExport( uint words, cg_sym_handle sym )
{
    /* unused parameters */ (void)words;
    OWLEmitExport( OwlFile, symbolHandle( sym ) );
}

static void owlOutLabel( label_handle label )
{
    array_control      *patches;
    owl_x86_patch      *patch;
    cg_sym_handle      sym;
    owl_sym_type       type;
    owl_sym_linkage    linkage;
    unsigned           i;
    offset             location;

    location = owlAskLocation();
    TellAddress( label, location );
    sym = AskForLblSym( label );
    type = OWL_TYPE_OBJECT;
    linkage = OWL_SYM_STATIC;
    if( sym != NULL ) {
        if( FEAttr( sym ) & FE_PROC )
            type = OWL_TYPE_FUNCTION;
        if( FEAttr( sym ) & FE_GLOBAL )
            linkage = OWL_SYM_GLOBAL;
    }
    OWLEmitLabel( CurrSection->handle, labelSymbol( label ), type, linkage );
    if( owlSymIsExported( sym ) )
        OWLEmitExport( OwlFile, labelSymbol( label ) );
    patches = AskLblPatches( label );
    for( i = 0; i < patches->used; ++i ) {
        patch = (owl_x86_patch *)patches->array + i;
        applyPatch( patch, location );
    }
    deletePatchArray( patches );
    TellDonePatches( label );
}

static void owlAbsPatch( abspatch_handle handle, offset value )
{
    owl_x86_abs_patch *patch = (owl_x86_abs_patch *)handle;
    if( patch->have_offset ) {
        applyPatch( &patch->patch, value );
        freeAbsPatch( patch );
    } else {
        patch->value = value;
        patch->have_value = true;
    }
}

static void *owlInitPatches( void ) { return( newPatchArray() ); }
static void owlSetLocation( offset location ) { OWLSetLocation( CurrSection->handle, location ); }
static void owlSetBigLocation( long_offset location ) { OWLSetLocation( CurrSection->handle, location ); }
static void owlIncLocation( offset amount ) { OWLSetLocation( CurrSection->handle, owlAskLocation() + amount ); }
static void owlOutFPPatch( fp_patches patch ) { (void)patch; owlUnsupported( "floating-point patch records require OMF" ); }

static void owlOutPatch( label_handle label, patch_attr attr )
{
    owl_x86_patch *patch = appendPatch( AskLblPatches( label ) );
    patch->section = CurrSection->handle;
    patch->where = owlAskLocation();
    patch->attr = attr;
}

static abs_patch *owlNewAbsPatch( void )
{
    owl_x86_abs_patch *patch = CGAlloc( sizeof( *patch ) );
    memset( patch, 0, sizeof( *patch ) );
    patch->next = AbsPatches;
    AbsPatches = patch;
    return( (abs_patch *)patch );
}

static void owlOutDataByte( byte value ) { OWLEmitData( CurrSection->handle, (char *)&value, sizeof( value ) ); }
static void owlOutDataShort( uint_16 value ) { OWLEmitData( CurrSection->handle, (char *)&value, sizeof( value ) ); }
static void owlOutDataLong( uint_32 value ) { OWLEmitData( CurrSection->handle, (char *)&value, sizeof( value ) ); }

static void owlOutAbsPatch( abs_patch *handle, patch_attr attr )
{
    owl_x86_abs_patch *patch = (owl_x86_abs_patch *)handle;
    long_offset value = 0;
    if( patch->have_value ) {
        value = patch->value;
        freeAbsPatch( patch );
    } else {
        patch->patch.section = CurrSection->handle;
        patch->patch.where = owlAskLocation();
        patch->patch.attr = attr;
        patch->have_offset = true;
    }
    if( attr & LONG_PATCH ) owlOutDataLong( value );
    else if( attr & WORD_PATCH ) owlOutDataShort( value );
    else owlOutDataByte( value );
}

static void owlOutReloc( segment_id segid, fix_class class, bool relative )
{
    owl_x86_section *target = findSection( segid );
    if( target == NULL )
        owlUnsupported( "OWL x86 relocation targets an undefined section" );
    OWLEmitReloc( CurrSection->handle, owlAskLocation(), target->base_symbol, relocType( class, relative ) );
}

static void owlOutSpecialCommon( import_handle handle, fix_class class, bool relative )
{
    (void)handle; (void)class; (void)relative;
    owlUnsupported( "conditional/common import handles are not implemented by the OWL x86 MVP" );
}

static void owlOutImport( cg_sym_handle sym, fix_class class, bool relative )
{
    OWLEmitReloc( CurrSection->handle, owlAskLocation(), symbolHandle( sym ), relocType( class, relative ) );
}

static void owlOutRTImportRel( rt_class index, fix_class class, bool relative )
{
    owl_symbol_handle symbol = OWLSymbolInit( OwlFile, AskRTName( index ) );
    OWLEmitReloc( CurrSection->handle, owlAskLocation(), symbol, relocType( class, relative ) );
}

static void owlOutRTImport( rt_class index, fix_class class )
{
    owlOutRTImportRel( index, class, F_CLASS( class ) == F_OFFSET || F_CLASS( class ) == F_LDR_OFFSET );
}

static void owlOutBckExport( const char *name, bool is_export )
{
    owl_symbol_handle symbol = OWLSymbolInit( OwlFile, name );
    OWLEmitLabel( CurrSection->handle, symbol, OWL_TYPE_OBJECT, OWL_SYM_GLOBAL );
    if( is_export )
        OWLEmitExport( OwlFile, symbol );
}

static void owlOutBckImport( const char *name, back_handle bck, fix_class class )
{
    owl_symbol_handle symbol;
    (void)bck;
    symbol = OWLSymbolInit( OwlFile, name );
    OWLEmitReloc( CurrSection->handle, owlAskLocation(), symbol, relocType( class, false ) );
}

static void owlOutLineNum( cg_linenum line, bool label_line ) { (void)label_line; PendingLine = line; }
static unsigned owlSavePendingLine( unsigned line ) { unsigned old = PendingLine; PendingLine = line; return( old ); }
static void owlOutBytes( unsigned len, const byte *src ) { if( len != 0 ) OWLEmitData( CurrSection->handle, (const char *)src, len ); }

static void owlOutIterated( byte pattern, offset len )
{
    byte buffer[64];
    unsigned amount;
    memset( buffer, pattern, sizeof( buffer ) );
    while( len != 0 ) {
        amount = len > sizeof( buffer ) ? sizeof( buffer ) : len;
        OWLEmitData( CurrSection->handle, (char *)buffer, amount );
        len -= amount;
    }
}

static bool owlNeedBaseSet( void ) { return( false ); }
static offset owlAskMaxSize( void ) { return( OWLTellSize( CurrSection->handle ) ); }
static long_offset owlAskBigLocation( void ) { return( OWLTellLocation( CurrSection->handle ) ); }
static long_offset owlAskBigMaxSize( void ) { return( OWLTellSize( CurrSection->handle ) ); }
static void owlTellNewLabel( cg_sym_handle sym ) { (void)sym; }

static void owlTellNewProc( cg_sym_handle proc )
{
    segment_id segid = FESegID( proc );
    if( findSection( segid ) != NULL ) {
        CodeSectionId = segid;
        owlSetOP( segid );
    }
}

static void owlTellVirtFuncRef( void *cookie ) { (void)cookie; owlUnsupported( "virtual-function linker directives are not implemented by the OWL x86 MVP" ); }

static segment_id owlAskSegID( pointer handle, cg_class class )
{
    switch( class ) {
    case CG_FE:
        if( inlineFunction( (cg_sym_handle)handle ) ) return( CodeSectionId );
        return( FESegID( (cg_sym_handle)handle ) );
    case CG_BACK: return( ((back_handle)handle)->segid );
    case CG_TBL:
    case CG_VTB:
    case CG_CLB: return( CodeSectionId );
    default: return( DataSectionId );
    }
}

static bool owlNameIsCode( pointer handle, cg_class class )
{
    switch( class ) {
    case CG_FE: return( (FEAttr( handle ) & FE_PROC) != 0 );
    case CG_TBL:
    case CG_VTB:
    case CG_CLB: return( true );
    default: return( false );
    }
}

static bool owlNameIsROM( pointer handle, cg_class class ) { return( owlSegIsROM( owlAskSegID( handle, class ) ) ); }

const x86_obj_emitter X86OwlEmitter = {
    owlInitSegDefs,
    owlFreeObjCache,
    owlAskOP,
    owlChangeOP,
    owlSetOP,
    owlDefSegment,
    owlEmptyQueue,
    owlGetMemModel,
    owlDbgSegDef,
    owlObjInit,
    owlAskLocation,
    owlChkDbgSegSize,
    owlUseImportForm,
    owlSegIsNear,
    owlSegIsBlank,
    owlSegIsPrivate,
    owlSegIsROM,
    owlAskBackSeg,
    owlAskCodeSeg,
    owlHaveCodeSeg,
    owlAskAltCodeSeg,
    owlAskCode16Seg,
    owlOutSelect,
    owlSetUpObj,
    owlFlushOP,
    owlObjFini,
    owlOutDLLExport,
    owlOutLabel,
    owlAbsPatch,
    owlInitPatches,
    owlSetBigLocation,
    owlIncLocation,
    owlSetLocation,
    owlOutFPPatch,
    owlOutPatch,
    owlNewAbsPatch,
    owlOutDataByte,
    owlOutDataShort,
    owlOutDataLong,
    owlOutAbsPatch,
    owlOutReloc,
    owlOutSpecialCommon,
    owlOutImport,
    owlOutRTImportRel,
    owlOutRTImport,
    owlOutBckExport,
    owlOutBckImport,
    owlOutLineNum,
    owlSavePendingLine,
    owlOutBytes,
    owlOutIterated,
    owlNeedBaseSet,
    owlAskMaxSize,
    owlAskBigLocation,
    owlAskBigMaxSize,
    owlTellNewLabel,
    owlTellNewProc,
    owlTellVirtFuncRef,
    owlAskSegID,
    owlNameIsCode,
    owlNameIsROM,
    owlSymIsExported
};

#endif
