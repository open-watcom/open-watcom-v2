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
#include "template.h"
#include "icopmask.h"
#include "brmtypes.h"
#include "b_write.h"

#ifndef NDEBUG
#include "dbg.h"
#include "pragdefn.h"
#endif

//extern CLASSINFO *ClassInfoGetIndex( CLASSINFO *c );
static BRI_HANDLE      bri_handle;


static BRI_StringID addString       // ADD A STRING
    ( char const * str )            // - the string
{
    return BRIAddString( bri_handle, (BRI_StringID)str, str );
}


static BRI_StringID addStringLower  // ADD A STRING, IN LOWER CASE
    ( BRI_StringID id               // - id to be used
    , char const * str )            // - the string
{
    char buffer[ 256 ];             // - buffer big enough for most file names
    char * alloced = NULL;          // - allocated buffer
    char * lowername = buffer;      // - buffer used
    BRI_StringID string_id;         // - return: string id
    int buf_size;                   // - buffer size required

    buf_size = strlen(str) + 1;
    buf_size *= sizeof(char);
    if( buf_size > sizeof( buffer ) ) {
        alloced = (char*)CMemAlloc( buf_size );
        lowername = alloced;
    }
    lowername = memcpy( lowername, str, buf_size );
    lowername = strlwr( lowername );
    string_id = BRIAddString( bri_handle, id, lowername );
    if( NULL != alloced ) {
        CMemFree( alloced );
    }
    return string_id;
}


static BRI_StringID addStringFile   // ADD A STRING, FOR A FILE
    ( SRCFILE file )                // - id of file
{
    return addStringLower( (BRI_StringID)file, SrcFileFullName( file ) );
}


#if 0
static int trivialTypeDef
    ( SYMBOL sym )
{
    int         result = FALSE;

    if( sym->id == SC_TYPEDEF ) {
        if( sym->sym_type->of->id == TYP_CLASS ) {
            if( sym->sym_type->of->u.c.info->name == sym->name->name ){
                result = TRUE;
            }
        }
    }

    return result;
}
#endif


#if 0
static SYMBOL findClassSymbol
    ( TYPE cltype )
{
    SYMBOL      result = NULL;
    SCOPE       clscope, parent;
    SYMBOL      sym, last;

    if( cltype->id != TYP_CLASS ){
        return NULL;
    }
    clscope = cltype->u.c.scope;
    if( clscope != NULL ){
        parent = clscope->enclosing;
        if( parent != NULL ){
            sym = ScopeOrderedFirst( parent );
            if( sym != NULL ){
                last = ScopeOrderedLast( parent );
                for( ; sym != NULL ; sym = ScopeOrderedNext( last, sym ) ) {
                    if( sym->name->name == cltype->u.c.info->name &&
                        (sym->id == SC_CLASS_TEMPLATE ||
                         sym->id == SC_TYPEDEF) ){
                        result = sym;
                        break;
                    }
                }
            }
        }
    }
    return result;
}
#endif


static BRI_SymbolID symbolIDForClass( CLASSINFO *c )
{
    return ~((BRI_SymbolID) ClassInfoGetIndex( c ));
}


// NOTE:  the following static data is initialized and de-initialized
//        by brinfWriteFileContents().
static uint_32 *        type_ops=NULL;
static int              type_ops_size=0;

static BRI_TypeID writeType     // DUMP A TYPE
    ( TYPE dtype )              // - type to dump
{
    BRI_TypeID  result = (BRI_TypeID) TypeGetIndex( dtype );
    BRI_TypeID  sub_type;
    BRI_TypeID  host_type;
    int         num_ops;
    int         i;

    if( dtype == NULL ) {
        return (BRI_TypeID) 0;
    }
    if( ! BRITypeAlreadySeen( bri_handle, result ) ) {
        switch( dtype->id ) {
            case TYP_BOOL:
            case TYP_CHAR:
            case TYP_SCHAR:
            case TYP_UCHAR:
            case TYP_WCHAR:
            case TYP_SSHORT:
            case TYP_USHORT:
            case TYP_SINT:
            case TYP_UINT:
            case TYP_SLONG:
            case TYP_ULONG:
            case TYP_SLONG64:
            case TYP_ULONG64:
            case TYP_FLOAT:
            case TYP_DOUBLE:
            case TYP_LONG_DOUBLE:
            case TYP_VOID:
            case TYP_DOT_DOT_DOT:
            case TYP_GENERIC:
                BRIAddType( bri_handle
                          , result
                          , BRI_TC_BaseType
                          , 1
                          , (uint_32) dtype->id );
            break;

            case TYP_ENUM:
                BRIAddType( bri_handle
                          , result
                          , BRI_TC_Enum
                          , 2
                          , (uint_32) dtype->u.t.sym->name->name
                          , (uint_32) dtype->u.t.sym );
            break;

            case TYP_POINTER:
                sub_type = writeType( dtype->of );
                BRIAddType( bri_handle
                          , result
                          , (BRI_TypeCode) dtype->id
                          , 1
                          , sub_type );
            break;

            case TYP_TYPEDEF:
                sub_type = writeType( dtype->of );
                BRIAddType( bri_handle
                          , result
                          , (BRI_TypeCode) dtype->id
                          , 1
                          , sub_type );
            break;

            case TYP_CLASS:
            {
                BRI_TypeCode    code = BRI_TC_Class;

                if( dtype->flag & TF1_UNION ){
                    code = BRI_TC_Union;
                } else if( dtype->flag & TF1_STRUCT ){
                    code = BRI_TC_Struct;
                }
                BRIAddType( bri_handle
                          , result
                          , code
                          , 2
                          , addString( NameStr( dtype->u.c.info->name ) )
                          , symbolIDForClass( dtype->u.c.info ) );
            }
            break;

            case TYP_BITFIELD:
                BRIAddType( bri_handle
                          , result
                          , BRI_TC_BitField
                          , 1
                          , dtype->u.b.field_width );
            break;

            case TYP_FUNCTION:
                num_ops = 1 + dtype->u.f.args->num_args;
                if( num_ops > type_ops_size ){
                    CMemFree( type_ops );
                    type_ops_size = num_ops;
                    type_ops = CMemAlloc( type_ops_size * sizeof(uint_32) );
                }
                type_ops[0] = writeType( dtype->of );
                for( i=1; i<num_ops; i++ ) {
                    type_ops[i] = writeType(dtype->u.f.args->type_list[i-1]);
                }
                BRIVAddType( bri_handle
                           , result
                           , BRI_TC_Function
                           , num_ops
                           , type_ops );
            break;

            case TYP_ARRAY:
                sub_type = writeType( dtype->of );
                BRIAddType( bri_handle
                          , result
                          , BRI_TC_Array
                          , 2
                          , dtype->u.a.array_size
                          , sub_type );
            break;

            case TYP_MODIFIER:
                sub_type = writeType( dtype->of );
                BRIAddType( bri_handle
                          , result
                          , BRI_TC_Modifier
                          , 2
                          , dtype->flag
                          , sub_type );
            break;

            case TYP_MEMBER_POINTER:
                sub_type = writeType( dtype->of );
                host_type = writeType( dtype->u.mp.host );
                BRIAddType( bri_handle
                          , result
                          , BRI_TC_PtrToMember
                          , 2
                          , host_type
                          , sub_type );
            break;

            default:
                // do nothing
            break;
        }
    }

    return result;
}


static void writeDefinition     // WRITE A DEFINITION
    ( TOKEN_LOCN* tl            // - location
    , BRI_SymbolID id )         // - id
{
    BRIAddDefinition( bri_handle
                    , addStringFile( tl->src_file )
                    , tl->line
                    , tl->column
                    , id );
}


static void writeSymbolLocn     // WRITE A DEFINITION FOR A SYMBOL
    ( SYMBOL sym
    , BRI_SymbolID sym_id )              // - the symbol
{
    writeDefinition( &sym->locn->tl, sym_id );
}


static void writeClassHdr       // WRITE BROWSE DEFN FOR CLASS HEADER
    ( TYPE cltype )             // - class type
{
    BRI_TypeID  type_id;
    BRI_SymbolAttributes        sym_attribs;

    if( bri_handle != NULL ) {
        type_id = writeType( cltype );
        sym_attribs = BRI_SA_Class;
        if( cltype->flag & TF1_INSTANTIATION ){
            sym_attribs = (BRI_SymbolAttributes)(sym_attribs|BRI_SA_TempInst);
        }
        BRIAddSymbol( bri_handle
                      , symbolIDForClass( cltype->u.c.info )
                      , addString( NameStr( cltype->u.c.info->name ) )
                      , type_id
                      , sym_attribs
                      , BRI_SA_AccessNone );
    }
}


static void writeClassEnd       // WRITE BROWSE DEFN FOR CLASS HEADER
    ( TYPE cltype )             // - class type
{
    cltype = cltype;
}


static void writeClassBases     // WRITE BROWSE DEFN FOR BASES
    ( TYPE cltype )             // - class type
{
    CLASSINFO* inf;             // - class information
    BASE_CLASS* base;           // - current base
    CLASSINFO* binf;            // - class information for base

    inf = cltype->u.c.info;
    RingIterBeg( inf->bases, base ) {
        binf = base->type->u.c.info;
        if( bri_handle != NULL ) {
            BRIAddReference( bri_handle
                           , BRI_NO_CHANGE
                           , BRI_NO_CHANGE
                           , BRI_NO_CHANGE
                           , symbolIDForClass( binf )
                           , BRI_RT_InheritFrom );
        }
#if 0
        IfDbgToggle( browse ) {
            DbgStmt( printf( "Br-inf-def: base is %s\n", binf->name ) );
        }
#endif
    } RingIterEnd( base );
}


static void writeClassFriends   // WRITE BROWSE DEFN FOR FRIENDS
    ( TYPE cltype )             // - class type
{
    CLASSINFO* inf;             // - class information
    FRIEND* friend;             // - current friend
    SYMBOL sym;                 // - symbol for friend

    inf = cltype->u.c.info;
    RingIterBeg( inf->friends, friend ) {
        if( FriendIsSymbol( friend ) ) {
            sym = FriendGetSymbol( friend );
            if( bri_handle != NULL ) {
                BRIAddReference( bri_handle
                                 , BRI_NO_CHANGE
                                 , BRI_NO_CHANGE
                                 , BRI_NO_CHANGE
                                 , (BRI_SymbolID) SymbolGetIndex( sym )
                                 , BRI_RT_Friend );
            }
        }
    } RingIterEnd( friend );
}


static void writeClassMembers   // WRITE BROWSE DEFN FOR MEMBERS
    ( TYPE cltype )             // - class type
{
    SCOPE scope;                // - scope for class
    SYMBOL last;                // - last symbol
    SYMBOL sym;                 // - current symbol
    BRI_SymbolID sym_id;
    BRI_TypeID type_id;
    BRI_SymbolAttributes sym_type;

    scope = cltype->u.c.scope;
    sym = ScopeOrderedFirst( scope );
    if( NULL != sym ) {
        last = ScopeOrderedLast( scope );
        for( ; sym != NULL ; sym = ScopeOrderedNext( last, sym ) ) {
            type_id = writeType( sym->sym_type );
            if( sym->sym_type->id == TYP_FUNCTION ){
                sym_type = BRI_SA_Function;
            } else if( sym->id == SC_TYPEDEF ) {
                sym_type = BRI_SA_Typedef;
            } else {
                sym_type = BRI_SA_Variable;
            }
            sym_id = (BRI_SymbolID) SymbolGetIndex( sym );
            BRIAddSymbol( bri_handle
                          , sym_id
                          , addString( NameStr( sym->name->name ) )
                          , type_id
                          , sym_type
                          , BRI_SA_AccessNone );
            if( sym->locn != NULL ){
                writeSymbolLocn( sym, sym_id );
            }
        }
    }
}


static void processMacroDefness     // PROCESS MACRO DEFINED/UNDEFINED
    ( MACVALUE const * mptr         // - macro ptr
    , BRI_GuardTypes   gtype )      // - id
{
    char const * string = BrinfMacValueName( mptr );

    BRIAddGuard( bri_handle
               , gtype
               , addString( string )
               , 0
               , 0
               , NULL );
}


static void processMacroValue       // PROCESS MACRO VALUE
    ( MACVALUE const * mptr         // - macro ptr
    , BRI_GuardTypes   gtype )      // - id
{
    char const *     string = BrinfMacValueName( mptr );
    unsigned         num_parms = BrinfMacValueParmCount( mptr );
    unsigned         length;
    uint_8 const *   defn = BrinfMacValueDefn( mptr, &length );

    BRIAddGuard( bri_handle
               , gtype
               , addString( string )
               , num_parms
               , length
               , defn );
}


void BrinfWriteDepMacDefed          // WRITE DEPENDENCY ON MACRO DEFINED
    ( MACVALUE const * value )      // - value
{
    processMacroDefness( value, BRI_GT_Defined );
}


void BrinfWriteDepMacUndefed        // WRITE DEPENDENCY ON MACRO UNDEFINED
    ( MACVALUE const * value )      // - value
{
    processMacroDefness( value, BRI_GT_NotDefined );
}


void BrinfWriteDepMacVal            // WRITE DEPENDENCY ON MACRO VALUE
    ( MACVALUE const * value )      // - value
{
    processMacroValue( value, BRI_GT_Value );
}


static void brinfWriteFileContents  // WRITE OUT BROWSE INFORMATION CONTENTS
    ( CGFILE *virtual_file )
{
    CGINTER *ins;               // - current instruction
    TOKEN_LOCN locn_ref = {NULL, 0, 0};

    // Initialize static data used by writeType
    type_ops_size = 4;
    type_ops = (uint_32 *) CMemAlloc( type_ops_size * sizeof( uint_32 ) );

    for( ; ; ) {
    // The following comment is a trigger for the ICMASK program to start
    // scanning for case IC_* patterns.
    // ICMASK BEGIN BRINFO (do not remove)
        unsigned not_used;

        ins = CgioReadICMaskCount( virtual_file
                                 , ICOPM_BRINFO
                                 , 0
                                 , &not_used );
        switch( ins->opcode ) {
                                        // NO OPERATION
            case IC_BR_NO_OP:           // - no parameter
            continue;
                                        // SET REFERENCE SOURCE FILE
            case IC_BR_REFTO_FILE :     // - SRCFILE
                locn_ref.src_file = ins->value.pvalue;
            continue;
                                        // SET REFERENCE LINE
            case IC_BR_REFTO_LINE :     // - line #
                locn_ref.line = ins->value.uvalue;
            continue;
                                        // SET REFERENCE COLUMN
            case IC_BR_REFTO_COL :      // - column #
                locn_ref.column = ins->value.uvalue;
            continue;
                                        // SET REFERENCE LINE, COLUMN
            case IC_BR_REFTO_LINECOL :  // - line #
                locn_ref.line = ins->value.uvalue >> 8;
                locn_ref.column = ins->value.uvalue & 0xFF;
            continue;
                                        // SCOPE: OPEN
            case IC_BR_SCOPE_OPN :      // - scope
            {
                SCOPE           scope = ins->value.pvalue;
                uint_32         owner;
                BRI_ScopeType   flags;
                SYMBOL          template_sym;
                BRI_StringID    string_id;

                switch( scope->id ) {
                    case SCOPE_FILE:
                        flags = BRI_ST_File;
                        owner = 0x0;
                    break;

                    case SCOPE_CLASS:
                        flags = BRI_ST_Class;
                        owner = writeType( scope->owner.type );
                    break;

                    case SCOPE_FUNCTION:
                        flags = BRI_ST_Function;
                        owner = (uint_32) scope->owner.sym;
                        DbgAssert( owner != 0 );
                    break;

                    case SCOPE_BLOCK:
                        flags = BRI_ST_Block;
                        owner = 0x0;
                    break;

                    case SCOPE_TEMPLATE_DECL:
                        flags = BRI_ST_TemplateDecl;
                        owner = 0x0;
                    break;

                    case SCOPE_TEMPLATE_INST:
                        flags = BRI_ST_TemplateInst;
                        owner = 0x0;
                    break;

                    case SCOPE_TEMPLATE_PARM:
                        flags = BRI_ST_TemplateParm;
                        if( scope->s.fn_template ) {
                            template_sym = scope->owner.defn->sym;
                        } else {
                            template_sym = scope->owner.tinfo->sym;
                        }
                        if( template_sym->locn != NULL ){
                            TOKEN_LOCN  *decl_locn;
                            decl_locn = &template_sym->locn->tl;
                            string_id = addStringFile( decl_locn->src_file );
                        } else {
                            string_id = (BRI_StringID) 0;
                        }
                        BRIStartTemplate( bri_handle
                                        , string_id );
                        owner = (uint_32) SymbolGetIndex( template_sym );
                    break;

                    default:
                        flags = BRI_ST_Block;
                        owner = 0x0;
                }
                if( flags != BRI_ST_Function ){
                    BRIStartScope( bri_handle
                                 , (BRI_ScopeID) ScopeGetIndex( scope )
                                 , flags
                                 , owner );
                } else {
                    SYMBOL      fn_symbol = (SYMBOL) owner;
                    BRIStartFnScope( bri_handle
                                 , (BRI_ScopeID) ScopeGetIndex( scope )
                                 , addString( NameStr( fn_symbol->name->name ) )
                                 , writeType( fn_symbol->sym_type ) );
                }
            }
            continue;
                                        // SCOPE: CLOSE
            case IC_BR_SCOPE_CLS :      // - scope
            {
                SCOPE   scope = ins->value.pvalue;

                BRIEndScope( bri_handle );
                if( scope->id == SCOPE_TEMPLATE_PARM ){
                    BRIEndTemplate( bri_handle );
                }
            }
            continue;
                                        // START OF SOURCE FILE INCLUSION
            case IC_BR_SRC_BEG :        // - full file name
            {
                SRCDEP* sd = ins->value.pvalue;
                char const* name = BrinfDepSrcFname( sd );
                /* To ensure that each file is identified correctly
                   across multiple #include's, convert to lower case */
                BRIStartFile( bri_handle
                            , addStringLower( (BRI_StringID)name, name ) );
                BrinfDepWrite( sd );
            }
            continue;
                                        // END OF SOURCE FILE INCLUSION
            case IC_BR_SRC_END :
                BRIEndFile( bri_handle );
            continue;
                                        // SOURCE-FILE INCLUSION
            case IC_BR_INC_SRC :        // - name as coded
            continue;
                                        // REFERENCE LOCATION
#if 0
            case IC_BR_REF_LOC :        // - token location of reference
            {
            locn_ref = ins->value.pvalue;
            } continue;
#endif
                                        // REFERENCE: Symbols, Type
            case IC_BR_REF_FUN :        // - function symbol
            case IC_BR_REF_CLM :        // - member symbol
            case IC_BR_REF_VAR :        // - variable symbol
            case IC_BR_REF_EVAL :       // - enumeration value
            case IC_BR_REF_EVAR :       // - enumeration variable
            case IC_BR_REF_TYPE :       // - type referenced
            {
                BRI_ReferenceType       ref_type;
                uint_32                 target;

                switch( ins->opcode ){
                    case IC_BR_REF_FUN:  ref_type = BRI_RT_Function; break;
                    case IC_BR_REF_CLM:  ref_type = BRI_RT_ClsMember; break;
                    case IC_BR_REF_VAR:  ref_type = BRI_RT_Var; break;
                    case IC_BR_REF_EVAL: ref_type = BRI_RT_Enum; break;
                    case IC_BR_REF_EVAR: ref_type = BRI_RT_Enum; break;
                    case IC_BR_REF_TYPE: ref_type = BRI_RT_TypeOf; break;
                    DbgDefault( "bad opcode" );
                }

                target = (uint_32) ins->value.pvalue;
                if( ins->opcode == IC_BR_REF_TYPE ) {
                    target = (uint_32) TypeGetIndex( (TYPE) target );
                } else {
                    target = (uint_32) SymbolGetIndex( (SYMBOL) target );
                }

                BRIAddReference( bri_handle
                               , addStringFile( locn_ref.src_file )
                               , locn_ref.line
                               , locn_ref.column
                               , target
                               , ref_type );

            }
            continue;
                                        // DECLARE: CLASS
            case IC_BR_DCL_CLASS :      // - class type
            {
                TYPE    cltype = ins->value.pvalue;
                TOKEN_LOCN* cllocn = LocnForClass( cltype );

                if( NULL != cllocn ) {
// This disables browsing for anonymous structs
// After we upgrade, the check should always succeed
                    writeClassHdr( cltype );
                    writeClassBases( cltype );
                    writeClassFriends( cltype );
                    writeClassMembers( cltype );
                    writeClassEnd( cltype );
                    writeDefinition( cllocn,
                                     symbolIDForClass( cltype->u.c.info ) );
                }
            }
            continue;
                                        // DECLARE: TYPEDEF
            case IC_BR_DCL_TDEF :       // - class type
            {
                SYMBOL td = ins->value.pvalue;
                BRI_SymbolID sym_id;
                TYPE cltype = td->sym_type;
                BRI_TypeID  type_id;

                type_id = writeType( cltype );
                sym_id = (BRI_SymbolID) SymbolGetIndex( td );
                BRIAddSymbol( bri_handle
                              , sym_id
                              , addString( NameStr( td->name->name ) )
                              , type_id
                              , BRI_SA_Typedef
                              , BRI_SA_AccessNone );
                if( td->locn != NULL ){
                    writeSymbolLocn( td, sym_id );
                }
            }
            continue;
                                        // DECLARE: VARIABLE
            case IC_BR_DCL_VAR :        // - variable symbol
            {
                SYMBOL sym = ins->value.pvalue;
                BRI_SymbolID sym_id;
                BRI_TypeID  type_id;
                BRI_SymbolAttributes sym_flag;

                if( SymIsEnumeration( sym ) ){
                    sym_flag = BRI_SA_Enum;
                } else {
                    sym_flag = BRI_SA_Variable;
                }
                type_id = writeType( sym->sym_type );
                sym_id = (BRI_SymbolID) SymbolGetIndex( sym );
                BRIAddSymbol( bri_handle
                              , sym_id
                              , addString( NameStr( sym->name->name ) )
                              , type_id
                              , sym_flag
                              , BRI_SA_AccessNone );
                if( sym->locn != NULL ){
                    writeSymbolLocn( sym, sym_id );
                }
            }
            continue;

            case IC_BR_DCL_FUN :
            {
                SYMBOL sym = ins->value.pvalue;
                BRI_SymbolID sym_id;
                BRI_TypeID  type_id;
                BRI_SymbolAttributes    attribs;

                type_id = writeType( sym->sym_type );
                sym_id = (BRI_SymbolID) SymbolGetIndex( sym );
                attribs = BRI_SA_Function;
                BRIAddSymbol( bri_handle
                              , sym_id
                              , addString( NameStr( sym->name->name ) )
                              , type_id
                              , attribs
                              , BRI_SA_AccessNone );
                if( sym->locn != NULL ){
                    writeSymbolLocn( sym, sym_id );
                }
            }
            continue;

            case IC_BR_REF_UMACRO:      // REFERENCE: UNDEFINED MACRO
                                        // - saved MACVALUE
                processMacroDefness( ins->value.pvalue, BRI_GT_RefUndef );
                continue;


            case IC_BR_DCL_MACRO :      // DECLARE: MACRO
                                        // - saved MACVALUE
            {
                MACVALUE const   *mptr = ins->value.pvalue;
                char const       *string = BrinfMacValueName( mptr );
                unsigned         num_parms = BrinfMacValueParmCount( mptr );
                unsigned         length;
                uint_8 const     *defn = BrinfMacValueDefn( mptr, &length );
                TOKEN_LOCN const *locn = BrinfMacValueLocn( mptr );
                LINE_NO          line;
                COLUMN_NO        column;

                if( defn != NULL ){
                    line = locn->line;
                    column = locn->column;
                } else {
                    line = BRI_NO_CHANGE;
                    column = BRI_NO_CHANGE;
                }

                BRISetPos( bri_handle, line, column );

                BRIAddGuard( bri_handle
                           , BRI_GT_Declaration
                           , addString( string )
                           , num_parms
                           , length
                           , defn );
            }
            continue;

            case IC_BR_REF_MACRO :      // REFERENCE: MACRO VALUE
                                        // - saved MACVALUE
                processMacroValue( ins->value.pvalue, BRI_GT_RefValue );
            continue;

            case IC_BR_PCH :            // PCH FILE REFERENCE
                                        // - full file name
            {
                char    *fname = ins->value.pvalue;

                BRIAddPCHInclude( bri_handle
                                , addStringLower((BRI_StringID)fname,fname) );
            }
            continue;

                                        // END OF FILE
            case IC_EOF :
            break;
                                        // DEFAULT -- AN ERROR
            default:
                DbgNever();
        }
        break;
    // ICMASK END (do not remove)
    }

    // Free static data used by writeType
    CMemFree( type_ops );
    type_ops = NULL;
    type_ops_size = 0;
}


void BrinfWriteFile                 // WRITE OUT BROWSE INFORMATION TO FILE
    ( CGFILE *virtual_file
    , BRI_Routines const *rtns
    , void * file_handle
    , BRI_HANDLE b_handle )
{
    if( NULL == b_handle ) {
        bri_handle = BRIBeginWrite( rtns, (int) file_handle, 0 );
    } else {
        bri_handle = BRIOpen( b_handle, rtns, (int) file_handle, 0 );
    }
    brinfWriteFileContents( virtual_file );
    BRIEndWrite( bri_handle );
}


void* BrinfCreateFile
    ( BRI_Routines const *rtns )
{
    DbgAssert( bri_handle == NULL );
    bri_handle = BRICreate( rtns );
    return bri_handle;
}


void* BrinfWritePchFile             // WRITE OUT BROWSE INFORMATION TO PCH
    ( CGFILE *virtual_file
    , BRI_Routines const *rtns
    , void * file_handle
    , unsigned long start )
{
    bri_handle = BRIBeginWrite( rtns, (int) file_handle, start );
    brinfWriteFileContents( virtual_file );
    BRIClose( bri_handle );
    return bri_handle;
}


void BrinfReadPchFile
    ( BRI_PCHRtns const *rtns
    , void * file_handle )
{
    BRIBuildHandle( bri_handle, rtns, file_handle );
}


#endif  // OPT_BR
